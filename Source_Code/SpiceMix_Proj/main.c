/*
 * main.c
 *
 *Created on: Sep 7, 2024
 *Author: Peter Nguyen
 *ID: 1001495756
 */

#include "Utilities.h"
#include "Stepper.h"

uint32_t angle = 0;
uint32_t rotations = 0;

int main(void)
{
    System_Init();
    StepMotorInit();

    while(1)
    {
        TestAugerMotor();
        TestRackMotor();
    }

}
