/* =======================================================
 * File Name: UIControl.c
 * =======================================================
 * File Description: Program Control for the User-Interface
 * This contains implementations for performing requested commands
 * from the User-Interface including dispensing a spice/recipe,
 * saving/updating a new recipe, etc. Detailed function
 * descriptions can be found in UIControl.h
 * Target: TM4C123GH6PM w/ 40MHz Clock
 * =======================================================
 */


#include "UIControl.h"
#include "MotorControl.h"
#include <stdio.h>
#include "string.h"
#include "uart0.h"
#include <ctype.h>

 /*========================================================
  * Variable Definitions
  *========================================================
  */
char SpiceList[MAXSLOTS][MAXNAMESIZE];
char RecipeList[MAXNUMRECP][MAXNAMESIZE];

/*========================================================
 * Function Defintions
 *========================================================
 */

/*=======================================================
 * Function Name: nameSearch
 *=======================================================
 * Parameters: name, arr, size
 * Return: i (index)
 * Description:
 * Function is used to search for a provided name against
 * a known list of entries. Name is the item to be checked.
 * arr is the list of known entries. Size is the size of the
 * entry list. This function is used for validating
 * an existing spice/recipe entry. If no match is found,
 * an ERRORMATCH code is returned. If a match is found,
 * the corresponding index in the dictionary is returned.
 * The calling function should check for the error code to
 * ensure a valid output was returned
 *=======================================================
 */
 // Forward Declaration
extern uint8_t nameSearch(char* name, char arr[][MAXNAMESIZE], int size);
extern bool isDigitString(char* string);

char* rusty_itoa(uint16_t num)
{
    uint8_t i = 0;
    uint8_t start = 0;
    uint8_t end = 5;
    // Static to ensure the pointer is properly returned to caller
    char buffer[6] = { '0', 0, 0, 0, 0, 0 };

    if (num == 0)
    {
        buffer[0] = '0';
        buffer[1] = 0;
        return buffer;
    }
    while (num != 0 && i < 5)
    {
        buffer[i] = num % 10 + 48;
        num = num / 10;
        i++;
    }

    end = i - 1;

    // Reverse the string since the conversion works in reverse
    while (start < end)
    {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        end--;
        start++;
    }
    return buffer;
}

bool isDigitString(char* string)
{
    uint8_t length = strlen(string);
    uint8_t i = 0;

    while (i < length && *(string+i) != '\0')
    {
        if (!isdigit(*(string + i)))
        {
            return false;
        }
        i++;
    }

    return true;
}

uint8_t nameSearch(char* name, char arr[][MAXNAMESIZE], int size)
{
	int i = 0;
	for (i = 0; i < size; i++)
	{
		if (strcmp(name, arr[i]) == 0)
		{
			return i;
		}
	}
	return ERRORMATCH;
}

void getUserInput(USER_DATA* data)
{
	clearBuffer(data);
	getsUart0(data);
	putsUart0("\n");
	parseFields(data);
}

void dispenseSpice(USER_DATA* data)
{
    //spice <name> <amount>
    uint8_t position = ERRORMATCH;
    uint16_t req_amount = (uint16_t)getFieldInteger(data, 2);
    uint16_t rem_amount = 0;

    position = nameSearch(getFieldString(data, 1), SpiceList, MAXSLOTS);

    if (position == ERRORMATCH)
    {
        putsUart0("dispenseSpice failed to find spice\n");
        return;
    }

    rem_amount = Read_SpiceRemQty(position);

    if (rem_amount < req_amount)
    {
        putsUart0("There is not enough spice for the requested amount\n");
        putsUart0("Please refill the spice or enter a smaller quantity\n");
        return;
    }
    else
    {
        putsUart0("Dispensing Please Wait...\n");
        Write_SpiceRemQty(position, rem_amount - req_amount);
        DispenseSequence(position, req_amount);
    }

    putsUart0("Command completed\n");
}

void dispenseRecipe(USER_DATA* data)
{
    //recipe <name>
    uint8_t i = 0;
    char str[MAX_CHARS];
    uint8_t position = ERRORMATCH;
    uint16_t num_of_stored_recipes = Read_NumofRecipes();
    uint16_t rem_amount = 0;

    position = nameSearch(getFieldString(data, 1), RecipeList, num_of_stored_recipes);

    if (position == ERRORMATCH)
    {
        putsUart0("dispenseRecipe failed to find recipe\n");
        return;
    }

    RecipeStructType target = Read_Recipe(position);

    // Verify there is enough of each spice. Otherwise abort
    for (i = 0; i < MAXSLOTS; i++)
    {
        if (target.Data[i].DataBits.quantity == 0)
        {
            break;
        }

        // Read the remaining quantity
        rem_amount = Read_SpiceRemQty(target.Data[i].DataBits.position);

        // If there is not enough spice notify user and abort dispense
        if (rem_amount < target.Data[i].DataBits.quantity)
        {
            sprintf(str, "ERROR: There is not enough %s for the requested amount\n", SpiceList[target.Data[i].DataBits.position]);
            putsUart0(str);
            putsUart0("Please refill the spice \n");
            return;
        }

        Write_SpiceRemQty(target.Data[i].DataBits.position, rem_amount - target.Data[i].DataBits.quantity);
    }

    putsUart0("Dispensing Please Wait...\n");
    for (i = 0; i < MAXSLOTS; i++)
    {
        if (target.Data[i].DataBits.quantity == 0)
        {
            break;
        }

        DispenseSequence(target.Data[i].DataBits.position, target.Data[i].DataBits.quantity);
    }

    putsUart0("Command completed\n");
}

// NEED TO FIX
void printRecipe(USER_DATA* data)
{
    //check <name>
    uint8_t i = 0;
    uint8_t position = 255;
    char str[MAX_CHARS];
    char RecipeName[MAXNAMESIZE];
    uint16_t num_of_stored_recipes = Read_NumofRecipes();

    strcpy(RecipeName, getFieldString(data, 1));

    position = nameSearch(RecipeName, RecipeList, num_of_stored_recipes);

    if (position == 255)
    {
        putsUart0("printRecipe failed to find recipe\n");
        return;
    }

    sprintf(str, "%s found; it includes:\n", RecipeName);
    putsUart0(str);
    RecipeStructType target = Read_Recipe(position);
    for (i = 0; i < MAXSLOTS; i++)
    {
        if (target.Data[i].DataBits.quantity == 0)
        {
            break;
        }

        // TBD THIS DOES NOT WORK YET DUE TO SOME UNKNOWN CORRUPTION WITH
        // THE USAGE OF SPRINTF
        // Format the string: Note: Quantity is in half teaspons so divide by 2
        sprintf(str, "- %f teaspoons of %s \n", (float)(target.Data[i].DataBits.quantity) / 2, SpiceList[target.Data[i].DataBits.position]);
        putsUart0(str);
    }

    putsUart0("Command completed\n");
}

void saveRecipe(USER_DATA* data)
{
    RecipeStructType recipe = { 0, };
    uint8_t i = 0;
    uint8_t index = 0;
    uint8_t position = 255;
    uint8_t number = 255;
    bool exists = false;
    bool update = false;
    uint16_t error = 0;

    // Save off the Recipe Name
    strcpy((char*)recipe.Name, getFieldString(data, 1));
    number = nameSearch(getFieldString(data, 1), RecipeList, MAXNUMRECP);

    // Check if the recipe already exists
    if (number != 255)
    {
        putsUart0("====================== NOTICE ======================\n");
        putsUart0("There exists a recipe with the name. Would you like to update it?\n");
        putsUart0("Press any key to overwrite the existing or type cancel to return\n");
        getUserInput(data);

        if (strcmp(getFieldString(data, 0), "cancel") == 0)
        {
            putsUart0("Cancelling...\n");
            putsUart0("Command completed\n");
            return;
        }
        else
        {
            update = true;
        }
    }

    while (index < MAXSLOTS)
    {
        putsUart0("Enter a spice followed by a quantity (less than 48)\n");
        putsUart0("Or type done when all spices have been entered\n");
        getUserInput(data);

        if (strcmp(getFieldString(data, 0), "done") == 0)
        {
            break;
        }
        else
        {
            if (strcmp(getFieldString(data, 0), "cancel") == 0)
            {
                putsUart0("Cancelling...");
                putsUart0("Command completed\n");
                return;
            }
        }

        position = nameSearch(getFieldString(data, 0), SpiceList, MAXSLOTS);

        if (position == 255)
        {
            putsUart0("====================== ERROR ======================\n");
            putsUart0("Entered spice does not exists. Please try again...\n\n");
        }
        else if (data->fieldCount < 2)
        {
            putsUart0("====================== ERROR ======================\n");
            putsUart0("No quantity was entered. Please enter a spice name and the quantity <name qty>\n\n");
        }
        else if (getFieldInteger(data, 1) > MAXQTY)
        {
            putsUart0("====================== ERROR ======================\n");
            putsUart0("The quantity entered must be less than 48. Please try again...\n\n");
        }
        else
        {
            // Check if the entered spice was already entered. If so, overwrite with new entry
            for (i = 0; i < index + 1; i++)
            {
                if (position == recipe.Data[i].DataBits.position && recipe.Data[i].DataBits.quantity != 0)
                {
                    putsUart0("====================== NOTICE ======================\n");
                    putsUart0("The entered spice was previously entered. Overwriting with new entry...\n\n");
                    recipe.Data[i].DataBits.position = position;
                    recipe.Data[i].DataBits.quantity = getFieldInteger(data, 1);
                    exists = true;
                    break;
                }
            }

            // If entered spice was not previously entered then save to new index and increment
            if (!exists)
            {
                recipe.Data[index].DataBits.position = position;
                recipe.Data[index].DataBits.quantity = getFieldInteger(data, 1);
                index++;
            }

            exists = false;
        }
    }

    // Check if the recipe is empty. 
    if (recipe.Data[0].DataBits.quantity != 0)
    {
        putsUart0("Saving Recipe. Please Wait...\n");

        // If updating a recipe use the appropriate function call
        if (update)
        {
            error = Write_RecipeX(recipe, number);
        }
        else
        {
            error = Write_Recipe(recipe);
        }

        if (error)
        {
            putsUart0("WARNING: There was an issue saving the recipe. You may try again or reset the system");
        }
        else
        {
            // Update the Recipe Dictionary
            position = Read_NumofRecipes();
            strcpy(RecipeList[position - 1], (char*)recipe.Name);
            putsUart0("Recipe was successfully saved!\n");
        }
    }
    else
    {
        putsUart0("No Recipe saved...\n");
    }

    putsUart0("Command completed\n");
}

void initSpiceList(void)
{
    int i = 0;
    char* temp;
    for (i = 0; i < MAXSLOTS; i++)
    {
        temp = (char*)Read_SpiceName(i);

        strcpy(SpiceList[i], temp);
    }
}

void initRecipeList(void)
{
    int i = 0;
    uint16_t count = Read_NumofRecipes();
    for (i = 0; i < count; i++)
    {
        strcpy(RecipeList[i], (char*)Read_Recipe(i).Name);
    }
}

void refillSpice(USER_DATA* data)
{
    uint8_t position = ERRORMATCH;
    uint16_t req_amount = (uint16_t)getFieldInteger(data, 2);

    position = nameSearch(getFieldString(data, 1), SpiceList, MAXSLOTS);

    if (position == ERRORMATCH)
    {
        putsUart0("Failed to find the spice. Aborting command.\n");
        return;
    }

    if (req_amount > MAXQTY)
    {
        putsUart0("====================== NOTICE ======================\n");
        putsUart0("The entered amount is greater than the max allowed quantity\n");
        putsUart0("Assuming slot is filled to max capacity\n");
    }

    Write_SpiceRemQty(position, req_amount);
    putsUart0("Command completed\n");
}

void changeSpice(USER_DATA* data)
{
    uint8_t position = ERRORMATCH;
    uint16_t req_amount = (uint16_t)getFieldInteger(data, 2);
    char str[MAX_CHARS];
    bool cancel = false;
    uint8_t step = 0;

    while (!cancel)
    {
        switch (step)
        {
        case 0:
        {
            putsUart0("Please Enter a slot number (0-");
            strcpy(str, rusty_itoa(MAXSLOTS - 1));
            putsUart0(str);
            putsUart0(") to change (or cancel to return): ");
            getUserInput(data);

            if (strcmp(getFieldString(data, 0), "cancel") == 0)
            {
                putsUart0("Cancelling...");
                putsUart0("Command completed\n");
                return;
            }
            else
            {
                // Slots are expected to only be 1 character so we only
                // need to validate that the first character is not a character.
                strcpy(str, getFieldString(data, 0));
                position = getFieldInteger(data, 0);

                if (!isDigitString(str) || position >= MAXSLOTS)
                {
                    putsUart0("====================== ERROR ======================\n");
                    putsUart0("The slot you entered is out of range. Please try again...\n\n");
                    break;
                }
            }

            putsUart0("====================== NOTICE ======================\n");
            putsUart0("The slot you entered currently belongs to ");
            putsUart0(SpiceList[position]);
            putsUart0(".\nWould you like to continue? Press any key to continue or type cancel to return to the Main Menu\n");
            getUserInput(data);

            if (strcmp(getFieldString(data, 0), "cancel") == 0)
            {
                putsUart0("Cancelling...");
                putsUart0("Command completed\n");
                return;
            }
            // Move to next step in set-up
            step = 1;
            putsUart0("Setting up new spice...\n");
            break;
        }
        case 1: // Obtain the name of the new spice
        {
            putsUart0("The name of your spice must be less than 15 characters and contain no spaces\n");
            putsUart0("Enter the name of your spice (or cancel to return): ");
            getUserInput(data);

            strcpy(str, getFieldString(data, 0));

            if (strcmp(getFieldString(data, 0), "cancel") == 0)
            {
                putsUart0("Cancelling...");
                putsUart0("Command completed\n");
                return;
            }
            else if (strlen(str) > MAXNAMESIZE)
            {
                putsUart0("====================== ERROR ======================\n");
                putsUart0("The name you entered is greater than ");
                strcpy(str, rusty_itoa(MAXNAMESIZE));
                putsUart0(str);
                putsUart0("\nPlease try again or enter cancel to return\n\n");
                break;
            }
            else
            {
                putsUart0("The name you entered was: ");
                putsUart0(str);
                putsUart0("\nIs this correct? Press any key to continue or type retry to reenter: ");
                getUserInput(data);

                if (strcmp(getFieldString(data, 0), "retry") == 0)
                {
                    break;
                }
                else
                {
                    if (strcmp(getFieldString(data, 0), "cancel") == 0)
                    {
                        putsUart0("Cancelling...");
                        putsUart0("Command completed\n");
                        return;
                    }
                }
            }
            // Write the name to the EEPROM
            Write_SpiceName(position, (uint8_t*)str);
            strcpy(SpiceList[position], str);
            putsUart0("Spice name was successfully updated!\n");

            putsUart0("Would you like to also change the quantity of the spice?\n");
            putsUart0("Press any key to continue or type done to exit: ");
            getUserInput(data);

            if (strcmp(getFieldString(data, 0), "done") == 0)
            {
                putsUart0("Returning to Main Menu...\n");
                putsUart0("Command completed\n");
                return;
            }

            step = 2;
            break;
        }
        case 2: // Ask if user would like to update the quantity of the slot
        {
            putsUart0("Enter the quantity of the spice you have filled to (Max is ");
            strcpy(str, rusty_itoa(MAXQTY));
            putsUart0(str);
            putsUart0(") or type cancel to exit.\n");
            putsUart0("Enter the quantity of the spice: ");
            getUserInput(data);

            if (strcmp(getFieldString(data, 0), "cancel") == 0)
            {
                putsUart0("Cancelling...");
                putsUart0("Command completed\n");
                return;
            }

            req_amount = getFieldInteger(data, 0);

            if (req_amount > MAXQTY)
            {
                putsUart0("====================== NOTICE ======================\n");
                putsUart0("The entered amount is greater than the max allowed quantity\n");
                putsUart0("Assuming slot is filled to max capacity\n");
            }

            Write_SpiceRemQty(position, req_amount);
            putsUart0("Command completed\n");
            return;
        }
        default:
            break;
        }

    }
    putsUart0("Command completed\n");
}

void UIRackHome(void)
{
    uint16_t error = 0;

    putsUart0("Homing the Rack. Please keep clear of the rack and ensure there are no obstructions\n");
    error = StepRackHome();

    if (error != 0)
    {
        putsUart0("====================== ERROR ======================\n");
        putsUart0("Rack Homing FAILED. Ensure there is no obstruction\n");
        putsUart0("on the hall sensors. You may try again or reset the system\n");
    }
    else
    {
        putsUart0("Homing Completed!\n");
    }

    putsUart0("Command completed\n");
}
