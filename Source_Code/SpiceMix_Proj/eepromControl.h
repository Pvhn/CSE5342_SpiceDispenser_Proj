/* =======================================================
 * File Name: eepromControl
 * =======================================================
 * File Description: Header File for eepromControl.c
 * =======================================================
 */

#ifndef EEPROMCONTROL_H_
#define EEPROMCONTROL_H_

#include <stdbool.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"

#include "System.h"

/*========================================================
 * Preprocessor Defintions
 *========================================================
 */

// Address offsets to EEPROM Data blocks
#define SPICENMADDR 0x0000
#define SPICEDATADDR 0x0020
#define SPICEINITOFST 0x002F
#define NUMOFRECOFST 0x04
#define CALIBHOMEOFST 0x05
#define CALIBSVOOFST 0x06
#define RECBLKADDR 0x0030
#define RECBLKSIZE 0x08

// Max System Values
#define MAXNAMESIZE 16
#define MAXSLOTS 8
#define MAXQTY 96 // Quantity is in half-teaspoons

/* Max Number of Stored Recipes
 * Calculated as (16-3)*2. Minus 3 since the first
 * 3 blocks are reserved for system information.
 * Each Recipe Block can store 2 recipes.
 */
#define MAXNUMRECP 26

// Error Codes
#define ERROROOM 0xDEAD
#define ERRORINVALID 0xBAD

/*========================================================
* Variable Definitions
*========================================================
*/

// Data Union/Structure for storing/accessing Spice Data
typedef union
{
	uint16_t As16BitWord;

	struct
	{
		uint16_t position : 4;
		uint16_t quantity : 12;
	}DataBits;

}SpiceDataType;

// Struct for storing Spice Name and Data information
typedef struct
{
	uint8_t name[MAXNAMESIZE];
	SpiceDataType data;
}SpiceStructType;

// Struct for storing recipe information
typedef struct
{
	uint8_t Name[MAXNAMESIZE];
	SpiceDataType Data[MAXSLOTS];
}RecipeStructType;

/*========================================================
* Function Definitions
*========================================================
*/

extern RecipeStructType Read_Recipe(uint8_t number);
extern uint16_t Read_NumofRecipes(void);
extern uint16_t Read_SpiceRemQty(uint8_t position);
extern uint8_t *Read_SpiceName(uint8_t position);
extern uint8_t *Read_RecipeName(uint8_t position);

// Write functions return an error code
extern uint16_t initSpiceData(bool reset);
extern uint16_t Write_SpiceRemQty(uint8_t position, uint16_t qty);
extern uint16_t Write_Recipe(RecipeStructType recipe);
extern uint16_t Write_RecipeX(RecipeStructType recipe, uint16_t number);
extern uint16_t Write_SpiceName(uint8_t position, uint8_t *name);
extern uint16_t Update_RecipeName(uint8_t number, uint8_t* name);
extern uint16_t Update_NumRecipes(uint8_t number);
extern uint16_t Delete_Recipe(uint8_t number);
extern uint16_t Write_CalibVal(uint16_t type, int16_t value);
extern int16_t Read_CalibVal(uint16_t type);
extern void TestEEPROM(void);

#endif /* EEPROMCONTROL_H_ */
