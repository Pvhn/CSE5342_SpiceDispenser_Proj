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
#include "System.h"

/*========================================================
 * Preprocessor Definitions
 *========================================================
 */

 // Scale factor for angle to full step conversion
 // Selected motor is 200 steps per full rotation. (360/200)*4
#define MICROSTEPSF 0.05625

#define USTEPRES 32
#define USTEPRESX2 USTEPRES*2
#define USTEPFULL360 6400 // Amount of microsteps for a full rotation

// PWM Macros
#define PWMFREQ 50e3	// PWM Frequency (50kHz)
#define PWMLOAD (SYSCLOCK/2)/PWMFREQ // PWM Load Value
//#define PWMPERIODUS (1/PWMFREQ)*10e5 // PWM Period in Microseconds
#define STEPMINPERIOD 2

// Memory Alias for Motor Outputs and Hall Sensor Input
//#define RACKMOTOR ((volatile uint32_t *)0x4000503C)		// PORTB0-3
//#define AUGERMOTOR ((volatile uint32_t *)0x4002503C)	// PORTF0-3
#define HALLSEN (*((volatile uint32_t *)0x4000700C))		// PORTD0/1

// Mask
#define HALSEN_MASK 0x03


/*========================================================
 * Variable Definitions
 *========================================================
 */
typedef enum
{
	OFF,
	HALTED,
	RUNNING,
	FAILED,
}MotorRunStatEnumType;

typedef enum
{
	NOTHOME,
	NEARHOME,
	HOME
}MotorHomeStatEnumType;

typedef enum
{
	CW = 1,
	CCW = -1,
}MotorDirEnumType;

typedef struct
{
	MotorRunStatEnumType runstatus;
	MotorDirEnumType direction;
	MotorHomeStatEnumType homestatus;
	uint32_t steps;
	float position;
	uint32_t period;
}MotorDataStructType;



/*========================================================
 * Function Declarations
 *========================================================
 */
extern void StepMotorInit(void);
extern void HallSensorInit(void);
extern void SetMotorSpd(uint32_t motorID, uint16_t speed);
extern void CommandMotor(uint32_t motorID, int32_t microsteps, uint16_t speed);
extern void TurnOffMotor(uint32_t motorID);
extern MotorRunStatEnumType GetMotorRunStatus(uint32_t motorID);
extern MotorHomeStatEnumType GetMotorHomeStatus(uint32_t motorID);
#endif /* STEPMOTOR_H_ */
