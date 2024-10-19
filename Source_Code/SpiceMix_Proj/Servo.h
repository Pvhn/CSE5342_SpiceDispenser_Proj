/* =======================================================
 * File Name: Servo.h
 * =======================================================
 * File Description: Header File for Servo Control
 * =======================================================
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include "tm4c123gh6pm.h"

/*========================================================
 * Preprocessor Defintions
 *========================================================
 */

#ifndef SYSCLOCK
	#define SYSCLOCK 40e6	// System Clock Frequency
#endif

// Servo Macros
// Bias for Servo Angle Calculation (Based on a 50Hz PWM and 2.5% Duty Cycle at 0 degrees)
#define SERVOBIAS (SYSCLOCK/50)*0.025	
#define SERVOSF (SYSCLOCK/50)*0.1		// Scale Factor for Servo Angle Calculation

/*========================================================
 * Function Declarations
 *========================================================
 */
extern void ServoInit(void);
extern void SetServoPos(uint16_t angle);

#endif /* SERVO_H_ */
