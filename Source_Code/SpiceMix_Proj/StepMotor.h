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


#define USTEPRES 16
#define USTEPFULL360 USTEPRES*200
#define MICROSTEPSF 0.1125 //(Calculated as 360/(200*USTEPRES)

#define MINRPM (SYSCLOCK*60)/(65535*USTEPFULL360*2)
#define RPMtoLOAD (SYSCLOCK*60)/(USTEPFULL360*2)

#define GEARRATIO 3.5 //(56/15)

// Memory Alias for Motor Outputs and Hall Sensor Input
//#define RACKMOTOR ((volatile uint32_t *)0x4000503C)		// PORTB0-3
//#define AUGERMOTOR ((volatile uint32_t *)0x4002503C)	// PORTF0-3
#define MOTOR0STEP	(*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 6*4)))
#define MOTOR0DIR	(*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 7*4)))
#define MOTOR0EN	(*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 5*4)))
#define MOTOR1STEP	(*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 0*4)))
#define MOTOR1DIR	(*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))
#define MOTOR1EN	(*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))

#define HALLSEN (*((volatile uint32_t *)0x4000500C))		// PORTB0/1

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
	CW,
	CCW
}MotorDirEnumType;

typedef struct
{
	MotorRunStatEnumType runstatus;
	MotorDirEnumType direction;
	MotorHomeStatEnumType homestatus;
	uint32_t steps;
	float position;
	uint32_t speed;
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
