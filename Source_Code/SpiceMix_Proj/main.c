/* =======================================================
 * File Name: main.c
 * =======================================================
 * File Description: Main Program File
 * =======================================================
 */

#include "System.h"
#include "StepMotor.h"
#include "Servo.h"
#include "MotorControl.h"

uint16_t pos = 0;
uint32_t rotations = 0;

int main(void)
{
    System_Init();
    StepMotorInit();
    ServoInit();
    HallSensorInit();

    while(1)
    {
        SetServoPos(pos);
        TestAugerMotor();
        TestRackMotor();
    }

}
