/* =======================================================
 * File Name: main.c
 * =======================================================
 * File Description: Main Program File
 * =======================================================
 */

#include "System.h"
#include "eepromControl.h"
#include "eeprom.h"

int main(void)
{


    System_Init();
    initEeprom();
    initSpiceData();

    while (1)
    {
        TestEEPROM();
    }
}
