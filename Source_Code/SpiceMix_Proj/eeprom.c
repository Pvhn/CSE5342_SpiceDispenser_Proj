/* =======================================================
 * File Name: eeprom.c
 * =======================================================
 * File Description: Driver Library for EEPROM usage
 *
 * Target: TM4C123GH6PM w/ 40MHz Clock
 * =======================================================
 */

#include "eeprom.h"

/*========================================================
 * Function Declarations
 *========================================================
 */

 /*=======================================================
  * Function Name: initEeprom
  *=======================================================
  * Parameters: None
  * Return: None
  * Description:
  * This function initializes the EEPROM for usage. This
  * includes the initialzation of the clock
  *=======================================================
  */
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

    // Wait for EEPROM to complete initailization
    while (EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);

    if ((EEPROM_EESUPP_R & 0x0C) == 0x0C)
    {
        // Indicate some error back to the system
    }
}

/*=======================================================
  * Function Name: writeEeprom
  *=======================================================
  * Parameters: addr, data
  * Return: error
  * Description:
  * This function writes the given data to the given
  * address in the EEPROM. An error code is returned if
  * there was an issue writing to the EEPROM. Note:
  * this is a 32-bit write
  *=======================================================
  */
uint16_t writeEeprom(uint16_t addr, uint32_t data)
{
    EEPROM_EEBLOCK_R = addr >> 4;
    EEPROM_EEOFFSET_R = addr & 0xF;
    EEPROM_EERDWR_R = data;
    while (EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);

    return (EEPROM_EEDONE_R & 0x3C);
}

/*=======================================================
  * Function Name: readEeprom
  *=======================================================
  * Parameters: addr
  * Return: data
  * Description:
  * This function reads and returns the 32-bit data
  * at the given address.
  *=======================================================
  */
uint32_t readEeprom(uint16_t add)
{
    EEPROM_EEBLOCK_R = add >> 4;
    EEPROM_EEOFFSET_R = add & 0xF;
    return EEPROM_EERDWR_R;
}
