/* =======================================================
 * File Name: MotorControl.c
 * =======================================================
 * File Description: Contains variables and functions
 * used for controlling stepper and servo motors.
 * =======================================================
 */

#include "MotorControl.h"

/*========================================================
 * Variable Definitions
 *========================================================
 */
uint16_t rack_pos;

MotorDataStruct MotorData[2] =
{
    { RACK, 0, RACKMOTOR},
    { AUGER, 0, AUGERMOTOR}
};

/*========================================================
 * Function Declarations
 *========================================================
 */

/* =======================================================
 * Function Name:
 * =======================================================
 * Parameters: None
 * Return: None
 * Description:
 * =======================================================
 */
void StepHome(void)
{
    bool dir1 = 0;
    bool dir2 = 0;
    uint16_t home = 0;
    float cosine = 0.0f;
    float sine = 0.0f;

    static int16_t globalstep = 0;

    // De-energize the motor before homing.
    *RACKMOTOR = 0;
    SetMotorCoilSpd(RACK, 0, 0);

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

        *RACKMOTOR = dir1 | (!dir1 << 1) | (dir2 << 2) | (!dir2 << 3);

        // Increment/Decrement based on rotation
        globalstep = (globalstep + 1) & 0x7F;

        waitMicrosecond(500);
    }

    // Set Rack Position to 0 (Home)
    rack_pos = 0;
}

/* =======================================================
 * Function Name:
 * =======================================================
 * Parameters: None
 * Return: None
 * Description:
 * =======================================================
 */
void SetRackPos(uint16_t angle)
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
    int32_t microsteps = (int32_t) delta/ MICROSTEPSF;

    // Command the new position
    MotorData[RACK].globalstep = MoveMotor(MotorData[RACK], microsteps, 3);
}

/* =======================================================
 * Function Name:
 * =======================================================
 * Parameters: None
 * Return: None
 * Description:
 * =======================================================
 */
void SetAugerPos(uint16_t rotations)
{
    // Calculate position difference
    float delta = 360*rotations;

    // Convert angle to microsteps
    int32_t microsteps = (int32_t) delta/ MICROSTEPSF;

    MotorData[AUGER].globalstep = MoveMotor(MotorData[AUGER], microsteps, 2);

    // De-energize the Auger Motor after moving since it
    // does not need to be held in place
    *(MotorData[AUGER].output) = 0;
}

/* =======================================================
 * Function Name:
 * =======================================================
 * Parameters: None
 * Return: None
 * Description:
 * =======================================================
 */
void TestRackMotor(void)
{
    SetRackPos(45);
    waitMicrosecond(1000000);
    SetRackPos(90);
    waitMicrosecond(1000000);
    SetRackPos(135);
    waitMicrosecond(1000000);
    SetRackPos(180);
    waitMicrosecond(1000000);
    SetRackPos(225);
    waitMicrosecond(1000000);
    SetRackPos(270);
    waitMicrosecond(1000000);
    SetRackPos(315);
    waitMicrosecond(1000000);
    SetRackPos(45);
    waitMicrosecond(1000000);
    SetRackPos(270);
    waitMicrosecond(1000000);
    SetRackPos(135);
    waitMicrosecond(1000000);
    SetRackPos(225);
    waitMicrosecond(1000000);
    SetRackPos(90);
    waitMicrosecond(1000000);
    SetRackPos(315);
    waitMicrosecond(1000000);
    SetRackPos(180);
    waitMicrosecond(1000000);
    SetRackPos(0);
    waitMicrosecond(1000000);
}

/* =======================================================
 * Function Name:
 * =======================================================
 * Parameters: None
 * Return: None
 * Description:
 * =======================================================
 */
void TestAugerMotor(void)
{
    SetAugerPos(6);
    waitMicrosecond(1000000);
    SetAugerPos(6);
    waitMicrosecond(1000000);
}
