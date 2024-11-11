/* =======================================================
 * File Name: main.c
 * =======================================================
 * File Description: Main Program File
 * =======================================================
 */

#include "System.h"
#include "eepromControl.h"
#include "eeprom.h"

RecipeStructType recipes[8] =
{
    {"Cajun", {0xABC0, 0xDEF5}},
    {"Pizza", {0x1234, 0x5677, 0x89A2, 0x0021}},
    {"5Spice", {0xBCD3, 0xFED2, 0x7654}},
    {"BBQ", {0xDEAD, 0xBEEF, 0xFACE, 0xCAFE, 0xFEED, 0xBADE, 0xBABE, 0xDADE}},
    {"Italian", {0xFEDC, 0xAAAA, 0xBBBB,0xDDDD, 0xEEEE}},
    {"Medditeranean", {0x5432, 0x9876, 0x5678}},
};

int main(void)
{
    int x,y,z;
    uint16_t data;
    uint8_t* name;
    uint16_t remrec = 0;
    RecipeStructType recipe;
    uint16_t error = 0;

    System_Init();
    initEeprom();
    initSpiceData();

    while (1)
    {
        for (x = 0; x < MAXSLOTS; x++)
        {
            data = Read_SpiceRemQty(x);
            name = Read_SpiceName(x);
            remrec = Read_NumofRecipes();
//
//            error = Write_Recipe(recipes[x]);
//
            recipe = Read_Recipe(x);

            
        }
    }
}
