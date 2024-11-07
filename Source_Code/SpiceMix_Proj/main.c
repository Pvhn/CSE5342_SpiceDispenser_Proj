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
uint16_t new_command = false;
int main(void)
{
    System_Init();
    StepMotorInit();
    ServoInit();
    HallSensorInit();

//    StepRackHome();
    while(1)
    {
        if (new_command)
        {
            SetRackPos(180);
            SetServoPos(180);
            SetAugerPos(2);
            SetServoPos(0);

            SetRackPos(270);
            SetServoPos(180);
            SetAugerPos(1);
            SetServoPos(0);

            SetRackPos(90);
            SetServoPos(180);
            SetAugerPos(1);
            SetServoPos(0);

            SetRackPos(225);
            SetServoPos(180);
            SetAugerPos(1);
            SetServoPos(0);

            SetRackPos(0);
            SetServoPos(180);
            SetAugerPos(1);
            SetServoPos(0);
            new_command = 0;
        }
//        SetServoPos(pos);
//        TestAugerMotor();
//        TestRackMotor();
    }

}
