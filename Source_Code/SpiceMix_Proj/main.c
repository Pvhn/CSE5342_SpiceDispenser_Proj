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


uint16_t new_command = false;
uint16_t pos = 0;
uint32_t rotations = 0;
uint16_t eng_pos = 90;
uint16_t diseng_pos = 180;

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
            SetRackPos(pos);
//            SetServoPos(eng_pos);
            SetAugerPos(rotations);
//            SetServoPos(diseng_pos);
            new_command = 0;
        }
    }

}
