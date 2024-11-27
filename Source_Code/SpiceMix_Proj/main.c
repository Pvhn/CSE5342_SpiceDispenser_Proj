/* =======================================================
 * File Name: main.c
 * =======================================================
 * File Description: Main Program File
 * =======================================================
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "System.h"
#include "StepMotor.h"
#include "Servo.h"
#include "MotorControl.h"
#include "eepromControl.h"
#include "eeprom.h"
#include "uart0.h"
#include "parsing.h"

uint16_t new_command = false;
uint16_t pos = 0;
uint32_t rotations = 0;
uint16_t eng_pos = 120;
uint16_t diseng_pos = 170;
uint16_t home = 0;

char *commands[] = {"spice", "recipe", "check", "save"};
uint8_t min_fields[4] = {3, 2, 2, 2};

char SpiceList[MAXSLOTS][MAXNAMESIZE];
char RecipeList[MAXNUMRECP][MAXNAMESIZE];

uint8_t nameSearch(char *name, char **arr)
{
    int i = 0;
    for(i = 0; i < MAXSLOTS; i++)
    {
        if(strcmp(name, arr[i]) == 0)
        {
            return i;
        }
    }
    return 255;
}

uint8_t nameRecipeSearch(char* name, char arr[MAXNUMRECP][MAXNAMESIZE])
{
    int i = 0;
    for (i = 0; i < MAXNUMRECP; i++)
    {
        if (strcmp(name, arr[i]) == 0)
        {
            return i;
        }
    }
    return 255;
}

void dispenseSpice(USER_DATA *data)
{
    //spice <name> <amount>
    uint8_t position = 255;
    uint8_t i = 0;
    uint16_t req_amount = (uint16_t) getFieldInteger(data, 2);
    uint16_t rem_amount = 0;

    for (i = 0; i < MAXSLOTS; i++)
    {
        if (strcmp(getFieldString(data, 1), SpiceList[i]) == 0)
        {
            position = i;
            break;
        }
    }

    if(position == 255)
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

void dispenseRecipe(USER_DATA *data)
{
    //recipe <name>
    uint8_t i = 0;
    char str[MAX_CHARS];
    uint8_t position = 255;
    uint16_t num_of_stored_recipes = Read_NumofRecipes();
    uint16_t rem_amount = 0;

    for (i = 0; i < num_of_stored_recipes; i++)
    {
        if (strcmp(getFieldString(data, 1), RecipeList[i]) == 0)
        {
            position = i;
            break;
        }
    }

    if(position == 255)
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
    for(i = 0; i < MAXSLOTS; i++)
    {
        if (target.Data[i].DataBits.quantity == 0)
        {
            break;
        }
            
        DispenseSequence(target.Data[i].DataBits.position, target.Data[i].DataBits.quantity);
    }

    putsUart0("Command completed\n");
}

void printRecipe(USER_DATA *data)
{
    //check <name>
    uint8_t i = 0;
    uint8_t position = 255;
    char str[MAX_CHARS];
    char RecipeName[MAXNAMESIZE]; 
    uint16_t num_of_stored_recipes = Read_NumofRecipes();

    strcpy(RecipeName, getFieldString(data, 1));

    for (i = 0; i < num_of_stored_recipes; i++)
    {
        if (strcmp(getFieldString(data, 1), RecipeList[i]) == 0)
        {
            position = i;
            break;
        }
    }

    if(position == 255)
    {
        putsUart0("printRecipe failed to find recipe\n");
        return;
    }

    sprintf(str, "%s found; it includes:\n", RecipeName);
    putsUart0(str);
    RecipeStructType target = Read_Recipe(position);
    for(i = 0; i < MAXSLOTS; i++)
    {
        if (target.Data[i].DataBits.quantity == 0)
        {
            break;
        }
        // Format the string: Note: Quantity is in half teaspons so divide by 2
        sprintf(str, "- %f teaspoons of %s \n", (float) (target.Data[i].DataBits.quantity)/2, SpiceList[target.Data[i].DataBits.position]);
        putsUart0(str);
    }

    putsUart0("Command completed\n");
}

void writeRecipe(USER_DATA *data)
{
    RecipeStructType recipe = { 0, };
    char str[MAX_CHARS];
    uint8_t i = 0;
    uint8_t index = 0;
    uint8_t pos = 0;
    uint8_t position = 255;
    
    // Save off the Recipe Name
    strcpy((char *) recipe.Name, getFieldString(data, 1));

    // Process the recipe data
    for(i = 2; i < data->fieldCount / 2; i++)
    {
        // Validate the given spice exists
        for (pos = 0; pos < MAXSLOTS; pos++)
        {
            if (strcmp(getFieldString(data, i * 2), SpiceList[pos]) == 0)
            {
                position = pos;
                break;
            }
        }

        if (position == 255)
        {
            sprintf("Failed to find %s. in currently available spices. Aborting...\n", getFieldString(data, i * 2));
            putsUart0(str);
            return;
        }

        if (getFieldInteger(data, i + 1) > MAXQTY)
        {
            putsUart0("ERROR: Quantity must be less than 48\n");
            return;
        }

        recipe.Data[index].DataBits.position = position;
        recipe.Data[index].DataBits.quantity = getFieldInteger(data, i+1);
        index++;
    }
    Write_Recipe(recipe);
}

void initSpiceList()
{
    int i = 0;
    char *temp;
    for(i = 0; i < MAXSLOTS; i++)
    {
        temp  = (char*) Read_SpiceName(i);

        strcpy(SpiceList[i], temp);
    }
}

void initRecipeList(void)
{
    int i = 0;
    uint16_t count = Read_NumofRecipes();
    for(i = 0; i < count; i++)
    {
        strcpy(RecipeList[i], (char *) Read_Recipe(i).Name);
    }
}

void displaySpicePage(void)
{
    uint8_t i = 0;
    char str[MAX_CHARS];
    putsUart0("Type in the Name and Quantity you would like to dispense (Name, Qty).\n");
    putsUart0("You may also type Return to return to do something else");

    for (i = 0; i < MAXSLOTS; i++)
    {
//        sprintf(str, "Hello %d", i);
//        putsUart0(str);
//        sprintf(str, " %d: %s\n",i, SpiceList[i]);
        putsUart0(SpiceList[i]);
        putsUart0("\n");
    }
}

int main(void)
{
    System_Init();
    StepMotorInit();
    ServoInit();
    HallSensorInit();
	initEeprom();
    initSpiceData();
    initSpiceList();
    initRecipeList();
    initUart0(); // If something breaks, put the needed pin assignments in System_Init()
    USER_DATA data;
    int8_t code = -1;

    // Setup UART0 baud rate
    setUart0BaudRate(115200, 40e6);
    putsUart0("UART Test\n");

    while(true)
    {
        clearBuffer(&data);
        getsUart0(&data);
        putsUart0("\n");
        code = identifyCommand(&data, &commands, min_fields);
        if(code == -1)
            putsUart0("Error: command not recognized\n");
        else
        {
            switch(code)
            {
                case 0:
                    dispenseSpice(&data);
                    break;
                case 1:
                    dispenseRecipe(&data);
                    break;
                case 2:
                    printRecipe(&data);
                    break;
                case 3:
                    writeRecipe(&data);
                    break;
                default:
                    break;
            }
        }

    }
}
