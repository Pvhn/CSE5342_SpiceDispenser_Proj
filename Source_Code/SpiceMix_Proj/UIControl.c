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
