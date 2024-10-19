/* =======================================================
 * File Name: System.h
 * =======================================================
 * File Description: Header File for System.c
 * =======================================================
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "tm4c123gh6pm.h"

/*========================================================
 * Preprocessor Defintions
 *========================================================
 */

#ifndef SYSCLOCK
	#define SYSCLOCK 40e6	// System Clock Frequency (Change as needed)
#endif

/*========================================================
 * Variable Declarations
 *========================================================
 */

/*========================================================
 * Function Declarations
 *========================================================
 */
extern void System_Init(void);

#endif /* SYSTEM_H_ */
