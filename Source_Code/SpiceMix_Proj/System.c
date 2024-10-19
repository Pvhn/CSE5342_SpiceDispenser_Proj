/* =======================================================
 * File Name: System.c
 * =======================================================
 * File Description: Contains functions and variables
 * for overall system configuration
 * 
 * Target: TM4C123GH6PM w/ 40MHz Clock
 * =======================================================
 */

#include "System.h"

/*=======================================================
 * Function Name: System_Init
 *=======================================================
 * Parameters: None
 * Description:
 * This function performs the initialization of
 * all system peripherals including the system
 * clock and all GPIO ports.
 *=======================================================
 */
void System_Init(void)
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    SYSCTL_GPIOHBCTL_R = 0;
}

