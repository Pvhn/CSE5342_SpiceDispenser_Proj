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

#define NUMOFCMDS 11

// If a command is added here, don't forget to add
// verbage to the displayHelpPage function for syntax usage.
UICmdStructType commands[NUMOFCMDS] = 
{
    {"spice",  3},
    {"recipe", 2},
    {"view",   2},
    {"check",  2},
    {"save",   2},
    {"delete", 2},
    {"refill", 3},
    {"change", 1},
    {"home",   1},
    {"reset",  1},
    {"stop",   1},
};

void displayHelpPage(void)
{
    putsUart0("=====================================================================\n");
    putsUart0("Here are all the available commands and their usage:\n");
    putsUart0("=====================================================================\n");
    putsUart0("\n");
    putsUart0("spice <name> <qty>   - Dispenses the qty of a defined spice.\n");
    putsUart0("                       Note: qty is in half-teaspoon measurements.\n");
    putsUart0("\n");
    putsUart0("recipe <name>        - Dispenses the specified recipe.\n");
    putsUart0("\n");
    putsUart0("view <item>          - View a list of stored items. view Spices will \n");
    putsUart0("                       display a list of the stored spices and their \n");
    putsUart0("                       remaining quantities. view Recipes will display \n");
    putsUart0("                       a list of stored recipes\n");
    putsUart0("\n");
    putsUart0("check <recipe>       - View the elements of a stored recipe. <recipe> \n");
    putsUart0("                       must be the name of a stored recipe.\n");
    putsUart0("\n");
    putsUart0("save <recipe>        - Save a new recipe or update an existing recipe \n");
    putsUart0("                       <recipe> can either be a new name or the name \n");
    putsUart0("                       of an existing recipe\n");
    putsUart0("\n");
    putsUart0("delete <recipe>      - Delete an existing recipe <recipe> must be the \n");
    putsUart0("                       name of a stored recipe.\n");
    putsUart0("\n");
    putsUart0("refill <spice> <qty> - Refill a spice with a specified quantity \n");
    putsUart0("                       <spice> must be the name of an existing spice \n");
    putsUart0("\n");
    putsUart0("change               - Change or Update the name of an existing spice \n");
    putsUart0("\n");
    putsUart0("home                 - Perform the homing of the rack to reset the \n");
    putsUart0("                       the home position \n");
    putsUart0("\n");
    putsUart0("reset                - Reset the system. This will reset the system \n");
    putsUart0("                       back to the default values. This will also \n");
    putsUart0("                       remove all stored recipes. \n");
    putsUart0("\n");
    putsUart0("stop                 - Turns off all motors. Homing must be performed\n");
    putsUart0("                       after this command is used to ensure correct\n");
    putsUart0("                       home state\n");
    putsUart0("=====================================================================\n");
    putsUart0("\n");
}

int main(void)
{
    // Initialize System and Hardware Peripherals
    System_Init();
    StepMotorInit();
    ServoInit();
    HallSensorInit();

    // Initialize UARTspi
    initUart0();
    setUart0BaudRate(115200, 40e6);

    // Initialize EEPROM and UI List
	initEeprom();
    initSpiceData(false);
    initSpiceList();
    initRecipeList();

    USER_DATA data;
    int8_t code = -1;

    bool homing_performed = false;

    while(true)
    {
        putsUart0("\n============================= MAIN MENU =============================\n");
        putsUart0("Enter a command (Press Enter for a list of commands): ");
        clearBuffer(&data);
        getsUart0(&data);
        putsUart0("\n");
        code = identifyCommand(&data, commands, NUMOFCMDS);

        switch(code)
        {
            case 0:
                if(homing_performed == false)
                {
                    putsUart0("====================== WARNING ======================\n");
                    putsUart0("Homing has not been performed since the last start-up!\n");
                    putsUart0("or emergency stop. Please home the rack using the home\n");
                    putsUart0("command before dispensing\n");
                }
                else
                {
                    dispenseSpice(&data);
                }
                break;
            case 1:
                if(homing_performed == false)
                {
                    putsUart0("====================== WARNING ======================\n");
                    putsUart0("Homing has not been performed since the last start-up!\n");
                    putsUart0("or emergency stop. Please home the rack using the home\n");
                    putsUart0("command before dispensing\n");
                }
                else
                {
                    dispenseRecipe(&data);
                }
                break;
            case 2:
                viewItems(&data);
                break;
            case 3:
                printRecipe(&data);
                break;
            case 4:
                saveRecipe(&data);
                break;
            case 5:
                deleteRecipe(&data);
                break;
            case 6:
                refillSpice(&data);
                break;
            case 7:
                changeSpice(&data);
                break;
            case 8:
                UIRackHome();
                homing_performed = true;
                break;
            case 9:
                resetSystem(&data);
                break;
            case 10:
                putsUart0("Turning Off all Motors\n");
                TurnOffMotor(RACK);
                TurnOffMotor(AUGER);
                homing_performed = false;
            default:
                putsUart0("ERROR: Command not recognized.\n");
                displayHelpPage();
                break;
        }
    }
}
