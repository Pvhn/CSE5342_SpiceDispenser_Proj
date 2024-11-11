/* =======================================================
 * File Name: main.c
 * =======================================================
 * File Description: Main Program File
 * =======================================================
 */

#include "System.h"
#include "eepromControl.h"
#include "eeprom.h"
uint16_t data;
uint8_t name[MAXNAMESIZE];
int main(void)
{
    int x = 0;
    uint16_t data;
    uint8_t *name;

    System_Init();
    initEeprom();
//    initSpiceData();

    data = Read_NumofRecipes();
    while (1)
    {
        for (x = 0; x < MAXSLOTS; x++)
        {
            data = Read_SpiceRemQty(x);
            name = Read_SpiceName(x);
        }
    }
}
