/*
 * Stepper.h
 *
 *  Created on: Oct 5, 2024
 *      Author: peter
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "gpio.h"
#include <tm4c123gh6pm.h>



/*========================================================
 * Preprocessor Defintions
 *========================================================
 */

// Scale factor for angle to full step conversion
// Selected motor is 200 steps per full rotation. (360/200)*4
#define STEPSF 7.2
#define MICROSTEPSF 0.05625

#define RACKMOTOR (*((volatile uint32_t *)0x4000503C)) // PORTB0-3
#define AUGRMOTOR (*((volatile uint32_t *)0x400053C0)) // PORTB4-7

/*========================================================
 * Variable Definitions
 *========================================================
 */
extern uint16_t position;
extern float sinarray[128];
extern float cosarray[128];

/*========================================================
 * Function Declarations
 *========================================================
 */
extern void StepperMotorInit(void);
extern void SetMotorAngle(uint16_t angle);
extern void MoveRackMotor(int16_t microsteps);
extern void SetMotorSpd(uint16_t CoilASpd, uint16_t CoilBSpd);
extern void MoveAugerMotor(uint16_t rotations);

#endif /* STEPPER_H_ */
