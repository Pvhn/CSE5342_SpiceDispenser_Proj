/* =======================================================
 * File Name: eeprom.c
 * =======================================================
 * File Description: Driver Library for EEPROM usage
 *
 * Target: TM4C123GH6PM w/ 40MHz Clock
 * =======================================================
 */

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "eeprom.h"

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initEeprom()
{
    // Enable EEPROM Clock
    SYSCTL_RCGCEEPROM_R = 1;
    _delay_cycles(3);

    // Wait for EEPROM to complete initailization
    while (EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);

    if ((EEPROM_EESUPP_R & 0x0C) == 0x0C)
    {
        // Indicate some error back to the system
    }

//    SYSCTL_SREEPROM_R |= 0x01; // Reset the EEPROM Module
//    _delay_cycles(6);
//    SYSCTL_SREEPROM_R |= 0x00; // Clear the Reset

    // Wait for EEPROM to complete initailization
    while (EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);

    if ((EEPROM_EESUPP_R & 0x0C) == 0x0C)
    {
        // Indicate some error back to the system
    }
}


uint16_t writeEeprom(uint16_t addr, uint32_t data)
{
    EEPROM_EEBLOCK_R = addr >> 4;
    EEPROM_EEOFFSET_R = addr & 0xF;
    EEPROM_EERDWR_R = data;
    while (EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);

    return (EEPROM_EEDONE_R & 0x3C);
}

uint32_t readEeprom(uint16_t add)
{
    EEPROM_EEBLOCK_R = add >> 4;
    EEPROM_EEOFFSET_R = add & 0xF;
    return EEPROM_EERDWR_R;
}
