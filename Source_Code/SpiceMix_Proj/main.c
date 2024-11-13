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
uint16_t eng_pos = 90;
uint16_t diseng_pos = 180;

char *commands[] = {"spice", "recipe", "check", "save"};
uint8_t min_fields[4] = {3, 2, 2, 2};

uint8_t SpiceList[MAXSLOTS];

uint8_t nameSearch(char *name, char *arr[])
{
    int i = 0;
    for(i = 0; i < MAXSLOTS; i++)
    {
        if(strcmp(name, SpiceList[i]) == 0)
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
    bool valid = true;
    uint8_t spiceName = (uint8_t) data->buffer[data->fieldPosition[1]];
    position = nameSearch(spiceName, SpiceList);
    SetRackPos(position);
    SetAugerPos((uint16_t) getFieldInteger(data, 2)); // Potentially dangerous
}

void dispenseRecipe(USER_DATA *data)
{
    //recipe <name>
    uint8_t position = 0;
    bool valid = true;
    uint8_t RecipeName = (uint8_t) data->buffer[data->fieldPosition[1]];
    position = nameSearch(RecipeName, SpiceList);
    RecipeStructType target = Read_Recipe(position);
    //loop through each possible spice location and go to it, dispensing the amount
    //required. Needs to check for empty fields somehow.
    //uint8_t pos = test->DataBits->position; <- example of getting position
    //SetRackPos(position);
    //SetAugerPos((uint16_t) getFieldInteger(data, 2)); // Potentially dangerous
}

void printRecipe(USER_DATA *data)
{

}

void writeRecipe(USER_DATA *data)
{

}

void initSpiceList()
{
    int i = 0;
    for(i = 0; i < MAXSLOTS; i++)
    {
        SpiceList[i] = Read_SpiceName(i);
    }
}

void initRecipeList(uint8_t *RecipeList[])
{
    int i = 0;
    uint16_t count = Read_NumofRecipes;
    uint16_t buffer[count];
    for(i = 0; i < count; i++)
    {
        buffer[i] = Read_Recipe(i).Name;
    }
    RecipeList = buffer;
}

int main(void)
{
    uint8_t *RecipeList[] = {};

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
                    dispenseRecipe(&data);
                    break;
                case 2:
                    printRecipe(&data);
                    break;
                case 3:
                    writeRecipe(&data);
                    break;
            }
        }
    }
/*
//    StepRackHome();
    while(1)
    {
        if (new_command)
        {
            SetRackPos(pos);
//            SetServoPos(eng_pos);
            SetAugerPos(rotations);
//            SetServoPos(diseng_pos);
            new_command = 0;
        }
    }
*/

}
