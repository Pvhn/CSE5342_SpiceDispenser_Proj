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
            TestMotors();
        }
    }

}
