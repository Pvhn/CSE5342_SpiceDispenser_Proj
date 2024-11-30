/* =======================================================
 * File Name: MotorControl
 * =======================================================
 * File Description: Header File for MotorControl.c
 * =======================================================
 */

#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

#include <stdbool.h>
#include <stdint.h>
#include "System.h"

/*========================================================
 * Preprocessor Defintions
 *========================================================
 */
#define ERRORHOMEFAIL 0xDEAF

/*========================================================
 * Variable Definitions
 *========================================================
 */

 // System Calibration Values
extern int16_t HOME_OFFSET;
extern int16_t AUG_OFFSET;
extern uint16_t SVO_ENG_POS;
extern uint16_t SVO_DIS_POS;

extern uint16_t rack_pos;

typedef enum
{
	RACK,
	AUGER
}MotorTypeEnum;

/*========================================================
 * Function Declarations
 *========================================================
 */

extern uint16_t StepRackHome(void);
extern void SetRackPos(uint16_t angle);
extern void SetAugerPos(uint16_t rotations);
extern void DispenseSequence(uint8_t position, uint16_t quantity);
extern void TestMotors(void);

#endif /* STEPPER_H_ */
