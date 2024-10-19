/* =======================================================
 * File Name: MotorControl
 * =======================================================
 * File Description: Header File for MotorControl.c
 * =======================================================
 */

#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "wait.h"

#include "System.h"
#include "StepMotor.h"
#include "Servo.h"

/*========================================================
 * Preprocessor Defintions
 *========================================================
 */

/*========================================================
 * Variable Definitions
 *========================================================
 */
extern uint16_t rack_pos;
extern MotorDataStruct MotorData[2];


/*========================================================
 * Function Declarations
 *========================================================
 */

extern void StepHome(void);

extern void SetRackPos(uint16_t angle);
extern void SetAugerPos(uint16_t rotations);

extern void TestRackMotor(void);
extern void TestAugerMotor(void);

#endif /* STEPPER_H_ */
