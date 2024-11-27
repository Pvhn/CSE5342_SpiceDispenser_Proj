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
char *RecipeList[MAXSLOTS];

uint8_t nameSearch(char *name, char *arr[])
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

void dispenseSpice(USER_DATA *data)
{
    //spice <name> <amount>
    uint8_t position = 0;
    char spiceName = data->buffer[data->fieldPosition[1]];
    position = nameSearch(&spiceName, SpiceList);
    if(position == 255)
    {
        putsUart0("dispenseSpice failed to find spice\n");
        return;
    }
    SetRackPos(position);
    SetAugerPos((uint16_t) getFieldInteger(data, 2));
    putsUart0("Command completed\n");
}

void dispenseRecipe(USER_DATA *data, char *RecipeList[])
{
    //recipe <name>
    uint8_t i = 0;
    uint8_t position = 0;
    char *RecipeName = &(data->buffer[data->fieldPosition[1]]);
    position = nameSearch(RecipeName, RecipeList);
    if(position == 255)
    {
        putsUart0("dispenseRecipe failed to find recipe\n");
        return;
    }
    RecipeStructType target = Read_Recipe(position);
    for(i = 0; i < MAXSLOTS; i++)
    {
        if(target.Data[i].DataBits.quantity == 0)
            break;

        SetRackPos(target.Data[i].DataBits.position);
        SetAugerPos(target.Data[i].DataBits.quantity);
    }
    putsUart0("Command completed\n");
}

void printRecipe(USER_DATA *data)
{
    //check <name>
    uint8_t i = 0;
    uint8_t position = 0;
    char str[20];
    char *RecipeName = &(data->buffer[data->fieldPosition[1]]);
    position = nameSearch(RecipeName, RecipeList);
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
        sprintf(str, "- %d teaspoons of %s \n", target.Data[i].DataBits.quantity, SpiceList[target.Data[i].DataBits.position]);
        putsUart0(str);
    }
    putsUart0("Command completed\n");
}

void writeRecipe(USER_DATA *data)
{
    RecipeStructType new; // Can't figure out
    uint8_t i = 0;
    strcpy(new.Name, getFieldString(data, 1));
    for(i = 2; i < data->fieldCount / 2; i++)
    {
        new.Data[i].DataBits.position = nameSearch(getFieldString(data, i*2), RecipeList);
        new.Data[i].DataBits.quantity = getFieldInteger(data, i*2);
    }
    Write_Recipe(new);
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

void initRecipeList(char *RecipeList[])
{
    int i = 0;
    uint16_t count = Read_NumofRecipes();
    for(i = 0; i < count; i++)
    {
        RecipeList[i] = (char*) Read_Recipe(i).Name;
    }
}

void displaySpicePage(void)
{
    uint8_t i = 0;
    char str[20];

    putsUart0("Type in the Name and Quantity you would like to dispense (Name, Qty).\n");
    putsUart0("You may also type Return to return to do something else");

    for (i = 0; i < MAXSLOTS; i++)
    {
        sprintf(str, "%d: %s \n", i, SpiceList[i]);
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
    initRecipeList(&RecipeList);

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
                    dispenseRecipe(&data, &RecipeList);
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
        else
        {
            if(home)
            {
                StepRackHome();
            }
        }
    }
}
