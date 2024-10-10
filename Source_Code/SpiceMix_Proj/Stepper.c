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

uint16_t rack_pos;

/*========================================================
 * Function Declarations
 *========================================================
 */

void StepMotorInit(void)
{
    // Enable GPIO PORTB Peripheral
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R4;
    _delay_cycles(3);

    // Initialize PORTB (Motor Outputs)
    GPIO_PORTB_ODR_R &= ~0xFF;  // Disable pen-Drain for PB0-7
    GPIO_PORTB_DIR_R |= 0xFF;   // Enable PB0-7 as outputs
    GPIO_PORTB_DEN_R |= 0xFF;   // Set Digital Enable

    // Initialize PORTE (Hall Sensor Input)
    GPIO_PORTE_DIR_R &= ~0x03;
    GPIO_PORTE_DEN_R |= 0x03;
}

void StepHome(void)
{
    bool dir1 = 0;
    bool dir2 = 0;
    uint16_t home = 0;
    float cosine = 0.0f;
    float sine = 0.0f;

    static uint16_t globalstep = 0;

    // De-energize the motor before homing.
    RACKMOTOR = 0;

    while (!home)
    {
        // Set home flag to the hall sensor input
        home = HALSEN;

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
        globalstep = globalstep + 1;

        if (globalstep < 0)
        {
            globalstep = 127;
        }
        else
        {
            globalstep = globalstep % 128;
        }

        // NOTE: CONVERT THIS DELAY TO A INTERRUPT INSTEAD
        // either using PWM or a Timer. This also removes the need for the for loop.
        // The interrupt can instead pass a "update" flag for when the step is to be incremented
        waitMicrosecond(500);
    }
    
    // Optional: There isn't any weight on the gear shaft so the motor
    // does not need to remain energized. De-energize the motor to save energy.
    RACKMOTOR = 0;
}

void SetRackAngle(uint16_t angle)
{
    // Limit the angle between 0 to 359
    angle = angle % 360;

    // Calculate position difference
    float delta = angle - rack_pos;

    // Store new position
    rack_pos = angle;

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
    // NOTE: This will be multiplied by some gain
    // factor for the gear ratio
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

    static uint16_t globalstep = 0;

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

        // Increment the step at a frequency of 5kHz
        waitMicrosecond(80);
    }
    
    // Optional: There isn't any weight on the gear shaft so the motor
    // does not need to remain energized. De-energize the motor to save energy.
//    RACKMOTOR = 0;
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
    uint16_t output = 0;
    uint16_t k = 0;
    float cosine = 0.0f;
    float sine = 0.0f;

    static int16_t aug_step = 0;

    for (k=0; k < microsteps; k++)
    {
        sine = sinarray[aug_step];
        cosine = cosarray[aug_step];

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

        // Set the output word (bits 0-4)
        output = dir1 | (!dir1 << 1) | (dir2 << 2) | (!dir2 << 3);

        // Set the actual output. Shift output by 4 to align with
        // the correct motor ports.(PB4-7)
        AUGRMOTOR = output << 4;

        // Increment the step
        aug_step = (aug_step+1)%128;

        // Pulse the motor at a frequency of 12.5kHz
        waitMicrosecond(80);
    }

    // Once Rotation is complete, de-energize the
    // auger motor since it does not need to be held in place.
    if (k >= microsteps)
    {
        AUGRMOTOR = 0;
    }
}

void TestRackMotor(void)
{
    SetRackAngle(45);
    waitMicrosecond(1000000);
    SetRackAngle(90);
    waitMicrosecond(1000000);
    SetRackAngle(135);
    waitMicrosecond(1000000);
    SetRackAngle(180);
    waitMicrosecond(1000000);
    SetRackAngle(225);
    waitMicrosecond(1000000);
    SetRackAngle(270);
    waitMicrosecond(1000000);
    SetRackAngle(315);
    waitMicrosecond(1000000);
    SetRackAngle(45);
    waitMicrosecond(1000000);
    SetRackAngle(270);
    waitMicrosecond(1000000);
    SetRackAngle(135);
    waitMicrosecond(1000000);
    SetRackAngle(225);
    waitMicrosecond(1000000);
    SetRackAngle(90);
    waitMicrosecond(1000000);
    SetRackAngle(315);
    waitMicrosecond(1000000);
    SetRackAngle(180);
    waitMicrosecond(1000000);
    SetRackAngle(0);
    waitMicrosecond(1000000);
}

void TestAugerMotor(void)
{
    MoveAugerMotor(6);
    waitMicrosecond(1000000);
    MoveAugerMotor(6);
    waitMicrosecond(1000000);
}
