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
#include "UIControl.h"


#define NUMOFCMDS 9

UICmdStructType commands[NUMOFCMDS] = 
{
    {"spice",  3},
    {"recipe", 2},
    {"check",  2},
    {"save",   2},
    {"delete", 2},
    {"refill", 3},
    {"change", 1},
    {"home",   1},
    {"reset",  1},
};

// NOT WORKING. NEED TO DEBUG SPRINTF MEMORY CORRUPTION
void displaySpicePage(void)
{
    uint8_t i = 0;
    char str[MAX_CHARS];
    uint16_t rem_amount = 0;
    putsUart0("Type in the Name and Quantity you would like to dispense (Name, Qty).\n");
    putsUart0("You may also type Return to return to do something else.\n");

    for (i = 0; i < MAXSLOTS; i++)
    {
        rem_amount = Read_SpiceRemQty(i);
        strcpy(str, rusty_itoa(i));
        putsUart0(str);
        putsUart0(": ");
        putsUart0(SpiceList[i]);
        putsUart0("\tQty: ");
        strcpy(str, rusty_itoa(rem_amount));
        putsUart0(str);
        putsUart0("\n");
    }
}

int main(void)
{
    // Initialize System and Hardware Peripherals
    System_Init();
    StepMotorInit();
    ServoInit();
    HallSensorInit();

    // Initialize UART
    initUart0();
    setUart0BaudRate(115200, 40e6);

    // Initialize EEPROM and UI List
	initEeprom();
    initSpiceData();
    initSpiceList();
    initRecipeList();

    USER_DATA data;
    int8_t code = -1;
    uint8_t i = 0;


    putsUart0("UART Test\n");

    // Perform Homing of the Rack
//    UIRackHome();

    while(true)
    {
        putsUart0("Enter a command: ");
        clearBuffer(&data);
        getsUart0(&data);
        putsUart0("\n");
        code = identifyCommand(&data, commands, NUMOFCMDS);

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
                saveRecipe(&data);
                break;
            case 4:
                break;
            case 5:
                refillSpice(&data);
                break;
            case 6:
                changeSpice(&data);
                break;
            case 7:
                UIRackHome();
                break;
            case 8:

                break;
            default:
                putsUart0("====================== ERROR ======================\n");
                putsUart0("Command not recognized. Valid commands are:\n");
                for (i = 0; i < NUMOFCMDS; i++)
                {
                    putsUart0(commands[i].cmd);
                    putsUart0("\n");
                }

                displaySpicePage();
                break;
        }
    }
}
