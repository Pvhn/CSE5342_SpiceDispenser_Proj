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

volatile bool home = false;
volatile bool slow = false;

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
uint16_t StepRackHome(void)
{
    bool dir1 = 0;
    bool dir2 = 0;
    uint16_t output;
    uint16_t coilAspd = 0;
    uint16_t coilBspd = 0;
    float cosine = 0.0f;
    float sine = 0.0f;

    uint16_t globalstep = 0;

    // De-energize the motor before homing.
    *RACKMOTOR = 0;
    SetMotorCoilSpd(RACK, 0, 0);

    while (!home)
    {
        sine = sinarray[globalstep];
        cosine = cosarray[globalstep];
        coilAspd = abs((PWMLOAD - 1) * sine);
        coilBspd = abs((PWMLOAD - 1) * cosine);

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

        output = dir1 | (!dir1 << 1) | (dir2 << 2) | (!dir2 << 3);

        SetMotorCoilSpd(RACK, coilAspd, coilBspd);
        *RACKMOTOR = output;

        // Increment/Decrement based on rotation
        globalstep = (globalstep + 1) & 0x7F;

        // Decrease speed as Rack Approaches Home
        if (slow)
        {
            waitMicrosecond(PWMPERIODUS * 40);
        }
        else
        {
            waitMicrosecond(PWMPERIODUS * 15);
        }
    }

    return globalstep;

    // Set Rack Position to 0 (Home)
    rack_pos = 0;
}

void PortDISR(void)
{
    uint16_t input = HALLSEN;
    if (input == 0)
    {
        home = true;
    }
    else
    {
        home = false;
        if (input == 0x1 || input == 0x02)
        {
            slow = true;
        }
        else
        {
            slow = false;
        }
    }

    GPIO_PORTD_ICR_R |= HALSEN_MASK;
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
