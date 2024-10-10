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
#include "tm4c123gh6pm.h"
#include "wait.h"



/*========================================================
 * Preprocessor Defintions
 *========================================================
 */

// Scale factor for angle to full step conversion
// Selected motor is 200 steps per full rotation. (360/200)*4
#define STEPSF 7.2
#define MICROSTEPSF 0.05625

#define PWMTICKS 1000
#define PWMPERIOD 20e3
#define PWMPERIODUS (1/PWMPERIOD)*10e5

// Memory Alias for Motor Outputs and Hall Sensor Input
#define RACKMOTOR (*((volatile uint32_t *)0x4000503C)) // PORTB0-3
#define AUGRMOTOR (*((volatile uint32_t *)0x400053C0)) // PORTB4-7
#define HALSEN (*((volatile uint32_t *)0x40024004)) // PORTE0

/*========================================================
 * Variable Definitions
 *========================================================
 */

typedef enum
{
	RACK,
	AUGER

}MotorTypeEnum;

extern uint16_t rack_pos;
extern float sinarray[128];
extern float cosarray[128];

/*========================================================
 * Function Declarations
 *========================================================
 */
extern void StepMotorInit(void);
extern void StepRackHome(void);
extern void SetMotorCoilSpd(uint16_t motor, uint16_t CoilASpd, uint16_t CoilBSpd);
extern void SetRackAngle(uint16_t angle);
extern void MoveRackMotor(int16_t microsteps);
extern void MoveAugerMotor(uint16_t rotations);

extern void TestRackMotor(void);
extern void TestAugerMotor(void);


#endif /* STEPPER_H_ */
