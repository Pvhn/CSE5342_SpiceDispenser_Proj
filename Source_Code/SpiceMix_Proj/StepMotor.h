/* =======================================================
 * File Name: StepMotor.h
 * =======================================================
 * File Description: Header File for StepMotor.c
 * =======================================================
 */

#ifndef STEPMOTOR_H_
#define STEPMOTOR_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "wait.h"

/*========================================================
 * Preprocessor Defintions
 *========================================================
 */

#ifndef SYSCLOCK
	#define SYSCLOCK 40e6	// System Clock Frequency
#endif

 // Scale factor for angle to full step conversion
 // Selected motor is 200 steps per full rotation. (360/200)*4
#define MICROSTEPSF 0.05625

 // PWM Macros
#define PWMFREQ 20e3	// PWM Frequency (20kHz)
#define PWMLOAD (SYSCLOCK/2)/PWMFREQ // PWM Load Value 
#define PWMPERIODUS (1/PWMFREQ)*10e5 // PWM Period in Microseconds

// Memory Alias for Motor Outputs and Hall Sensor Input
#define RACKMOTOR ((volatile uint32_t *)0x4000503C)		// PORTB0-3
#define AUGERMOTOR ((volatile uint32_t *)0x4002503C)	// PORTF0-3
#define HALSEN (*((volatile uint32_t *)0x4002400C))		// PORTE0/1

/*========================================================
 * Variable Definitions
 *========================================================
 */

typedef enum
{
	RACK,
	AUGER
}MotorTypeEnum;

typedef struct
{
	MotorTypeEnum type;
	uint16_t globalstep;
	volatile uint32_t* output;
}MotorDataStruct;

extern const float sinarray[128];
extern const float cosarray[128];

/*========================================================
 * Function Declarations
 *========================================================
 */
extern void StepMotorInit(void);
extern void HallSensorInit(void);
extern void SetMotorCoilSpd(MotorTypeEnum motor, uint16_t CoilASpd, uint16_t CoilBSpd);
extern uint16_t MoveMotor(MotorDataStruct motor, int32_t microsteps, uint16_t speed);

#endif /* STEPMOTOR_H_ */
