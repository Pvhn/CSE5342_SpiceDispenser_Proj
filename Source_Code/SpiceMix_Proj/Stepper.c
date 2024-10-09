/*
 * Stepper.c
 *
 *  Created on: Oct 5, 2024
 *      Author: peter
 */

#include "Stepper.h"


/*========================================================
 * Variable Definitions
 *========================================================
 */
float sinarray[128] =
{
    0.0000, 0.0491, 0.0980, 0.1467, 0.1951, 0.2430, 0.2903, 0.3369, 0.3827, 0.4276,
    0.4714, 0.5141, 0.5556, 0.5957, 0.6344, 0.6716, 0.7071, 0.7410, 0.7730, 0.8032,
    0.8315, 0.8577, 0.8819, 0.9040, 0.9239, 0.9415, 0.9569, 0.9700, 0.9808, 0.9892,
    0.9952, 0.9988, 1.0000, 0.9988, 0.9952, 0.9892, 0.9808, 0.9700, 0.9569, 0.9415,
    0.9236, 0.9040, 0.8816, 0.8577, 0.8315, 0.8032, 0.7730, 0.7410, 0.7071, 0.6716,
    0.6344, 0.5957, 0.5556, 0.5141, 0.4714, 0.4276, 0.3827, 0.3369, 0.2903, 0.2430,
    0.1951, 0.1467, 0.0980, 0.0491, 0.0000,-0.0491,-0.0980,-0.1467,-0.1951,-0.2430,
    -0.2903,-0.3369,-0.3827,-0.4276,-0.4714,-0.5141,-0.5556,-0.5957,-0.6344,-0.6716,
    -0.7071,-0.7410,-0.7730,-0.8032,-0.8315,-0.8577,-0.8819,-0.9040,-0.9239,-0.9415,
    -0.9569,-0.9700,-0.9808,-0.9892,-0.9952,-0.9988,-1.0000,-0.9988,-0.9952,-0.9892,
    -0.9808,-0.9700,-0.9569,-0.9415,-0.9236,-0.9040,-0.8816,-0.8577,-0.8315,-0.8032,
    -0.7730,-0.7410,-0.7071,-0.6716,-0.6344,-0.5957,-0.5556,-0.5141,-0.4714,-0.4276,
    -0.3827,-0.3369,-0.2903,-0.2430,-0.1951,-0.1467,-0.0980,-0.0491
};

float cosarray[128] =
{
    1.0000, 0.9988, 0.9952, 0.9892, 0.9808, 0.9700, 0.9569, 0.9415, 0.9236, 0.9040,
    0.8816, 0.8577, 0.8315, 0.8032, 0.7730, 0.7410, 0.7071, 0.6716, 0.6344, 0.5957,
    0.5556, 0.5141, 0.4714, 0.4276, 0.3827, 0.3369, 0.2903, 0.2430, 0.1951, 0.1467,
    0.0980, 0.0491, 0.0000,-0.0491,-0.0980,-0.1467,-0.1951,-0.2430,-0.2903,-0.3369,
    -0.3827,-0.4276,-0.4714,-0.5141,-0.5556,-0.5957,-0.6344,-0.6716,-0.7071,-0.7410,
    -0.7730,-0.8032,-0.8315,-0.8577,-0.8819,-0.9040,-0.9239,-0.9415,-0.9569,-0.9700,
    -0.9808,-0.9892,-0.9952,-0.9988,-1.0000,-0.9988,-0.9952,-0.9892,-0.9808,-0.9700,
    -0.9569,-0.9415,-0.9236,-0.9040,-0.8816,-0.8577,-0.8315,-0.8032,-0.7730,-0.7410,
    -0.7071,-0.6716,-0.6344,-0.5957,-0.5556,-0.5141,-0.4714,-0.4276,-0.3827,-0.3369,
    -0.2903,-0.2430,-0.1951,-0.1467,-0.0980,-0.0491, 0.0000, 0.0491, 0.0980, 0.1467,
    0.1951, 0.2430, 0.2903, 0.3369, 0.3827, 0.4276, 0.4714, 0.5141, 0.5556, 0.5957,
    0.6344, 0.6716, 0.7071, 0.7410, 0.7730, 0.8032, 0.8315, 0.8577, 0.8819, 0.9040,
    0.9239, 0.9415, 0.9569, 0.9700, 0.9808, 0.9892, 0.9952, 0.9988
};

uint16_t position;

void StepperMotorInit(void)
{
    // Enable GPIO PORTE Peripheral
    SYSCTL_RCGCGPIO_R |= 0x0010;
    _delay_cycles(3);

    GPIO_PORTE_ODR_R &= ~0x0F;
    GPIO_PORTE_DIR_R |= 0x0F;     // Enable PF1 as an output
    GPIO_PORTE_DEN_R |= 0x0F;     // Set Digital Enable
}

void SetMotorSpd(uint16_t CoilASpd, uint16_t CoilBSpd)
{

}

void SetMotorAngle(uint16_t angle)
{
    // Ensure commanded angle is between 0 to 359
    angle = angle % 360;

    // Calculate position difference
    float delta = angle - position;

    // Store new position
    position = angle;

    // Calculate shortest distance
    if(delta > 180)
    {
        // Subtract 360 to obtain CCW command
        delta = delta - 360;
    }
    else
    {
        if (delta < -180)
        {
            // Add 360 to obtain CW command
            delta = delta + 360;
        }
    }

    // Convert angle to microsteps
    int16_t microsteps = (int16_t) delta/ MICROSTEPSF;

    // Command the new position
    MoveRackMotor(microsteps);
}

void MoveRackMotor(int16_t microsteps)
{
    bool dir1 = 0;
    bool dir2 = 0;
    uint16_t k = 0;
    int16_t sign = 1;
    float cosine = 0.0f;
    float sine = 0.0f;

    static int16_t globalstep = 0;

    // If negative steps, flip sign for CCW rotation
    if(microsteps < 0)
    {
        sign = -1;
    }

    for (k=0; k < (microsteps*sign); k++)
    {
        sine = sinarray[globalstep];
        cosine = cosarray[globalstep];

        if (sine > 0)
        {
            dir1 = 1;
        }
        else
        {
            dir1 = 0;
        }

        if (cosine > 0)
        {
            dir2 = 1;
        }

        else
        {
            dir2 = 0;
        }

        RACKMOTOR = dir1 | (!dir1 << 1) | (dir2 << 2) | (!dir2 << 3);

        // Increment/Decrement based on rotation
        globalstep = globalstep + (1*sign);

        if (globalstep < 0)
        {
            globalstep = 127;
        }
        else
        {
            globalstep = globalstep%128;
        }

        // NOTE: CONVERT THIS DELAY TO A INTERRUPT INSTEAD
        // either using PWM or a Timer. This also removes the need for the for loop.
        // The interrupt can instead pass a "update" flag for when the step is to be incremented
        waitMicrosecond(200);
    }

    RACKMOTOR = 0;
}

void MoveAugerMotor(uint16_t rotations)
{
    // Calculate position difference
    float delta = 360*rotations;

    // Convert angle to microsteps
    uint32_t microsteps = (uint32_t) delta/ MICROSTEPSF;

    // Each turn of the auger dispenses x amount.
    bool dir1 = 0;
    bool dir2 = 0;
    uint16_t k = 0;
    float cosine = 0.0f;
    float sine = 0.0f;

    static int16_t globalstep = 0;


    for (k=0; k < microsteps; k++)
    {
        sine = sinarray[globalstep];
        cosine = cosarray[globalstep];

        if (sine > 0)
        {
            dir1 = 1;
        }
        else
        {
            dir1 = 0;
        }

        if (cosine > 0)
        {
            dir2 = 1;
        }

        else
        {
            dir2 = 0;
        }

        RACKMOTOR = dir1 | (!dir1 << 1) | (dir2 << 2) | (!dir2 << 3);

        // Increment rotation
        globalstep = (globalstep+1)%128;

        // NOTE: CONVERT THIS DELAY TO A INTERRUPT INSTEAD
        // either using PWM or a Timer. This also removes the need for the for loop.
        // The interrupt can instead pass a "update" flag for when the step is to be incremented
        waitMicrosecond(80);
    }

    // Once Rotation is complete, de-energize the
    // auger motor since it does not need to be held in place.
    RACKMOTOR = 0;
}


