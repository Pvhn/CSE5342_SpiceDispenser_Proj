/* =======================================================
 * File Name: eeprom
 * =======================================================
 * File Description: Header File for eeprom.c
 * =======================================================
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include "tm4c123gh6pm.h"

/*========================================================
 * Variable Definitions
 *========================================================
 */

// Union/Struct for accessing EEPROM data as 32-bit and 16-bit words.
typedef union
{
	uint32_t FullWord;

	struct
	{
		uint16_t Lower16Bits : 16;
		uint16_t Upper16Bits : 16;
	} HalfWord;

}EEPROMDataBlockType;

/*========================================================
 * Function Declarations
 *========================================================
 */

void initEeprom();
uint16_t writeEeprom(uint16_t add, uint32_t data);
uint32_t readEeprom(uint16_t add);

#endif
