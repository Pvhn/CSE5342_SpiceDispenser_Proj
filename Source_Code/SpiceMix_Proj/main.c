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

char *commands[] = {"spice", "recipe", "check", "save", "home"};
uint8_t min_fields[5] = {3, 2, 2, 2, 1};

// NOT WORKING. NEED TO DEBUG SPRINTF MEMORY CORRUPTION
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

    putsUart0("UART Test\n");

    StepRackHome();
    while(true)
    {
        clearBuffer(&data);
        getsUart0(&data);
        putsUart0("\n");
        code = identifyCommand(&data, &commands, min_fields);

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
            default:
                putsUart0("Error: command not recognized\n");
                break;
        }
    }
}
