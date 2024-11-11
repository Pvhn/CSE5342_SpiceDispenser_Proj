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
uint16_t speed = 50;

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
void StepRackHome(void)
{
    MotorHomeStatEnumType home_status = NOTHOME;
    MotorRunStatEnumType run_status = RUNNING;
    bool nearhome_pv = false;

    home_status = GetMotorHomeStatus(RACK);


    if (home_status != HOME)
    {
        // Command the Rack Motor to make 3 full rotations
        CommandMotor(RACK, USTEPFULL360 * 30, 20);

        run_status = GetMotorRunStatus(RACK);
        waitMicrosecond(1000);  // Wait atleast 1ms to allow motor to start running

        while (home_status != HOME && run_status != HALTED)
        {
            home_status = GetMotorHomeStatus(RACK);
            run_status = GetMotorRunStatus(RACK);
            
            // If Approaching Home position slow down.
            if (home_status == NEARHOME)
            {
                // If first transition to near home slow down the motor
                if (nearhome_pv == false)
                {
                    SetMotorSpd(RACK, 40);
                    nearhome_pv = true;
                }
            }
            else
            {
                // If first transition out of near home, increase speed
                if (nearhome_pv == true)
                {
                    SetMotorSpd(RACK, 20);
                    nearhome_pv = 0;
                }
            }
        }

        if (home_status == HOME)
        {
            // Reset Rack position to 0 (Home);
            rack_pos = 0;
            CommandMotor(RACK, 0, 20);
            TurnOffMotor(RACK);
        }
        else
        {
            // Failed to find home before Motor Timeout
            if (run_status == HALTED)
            {
                // Set Some Error Flag and indicate back to main program
            }
        }
    }

    // Reset Rack Motor Speed to default
    SetMotorSpd(RACK, 10);
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
    MotorRunStatEnumType status = OFF;

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
    int32_t microsteps = (int32_t) (delta/MICROSTEPSF)* GEARRATIO;

    // Command the new position
    CommandMotor(RACK, microsteps, speed);

    while (status != HALTED)
    {
        status = GetMotorRunStatus(RACK);
    }

    //Wait half a second to let motor come to a full stop
    waitMicrosecond(500000);
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
    MotorRunStatEnumType status = OFF;

    // Calculate position difference
    float delta = 360*rotations;

    // Convert angle to microsteps
    int32_t microsteps = (int32_t) delta/MICROSTEPSF;

    CommandMotor(AUGER, microsteps, speed);

    while (status != HALTED)
    {
        status = GetMotorRunStatus(AUGER);
    }

    // De-energize the Auger Motor after moving since it
    // does not need to be held in place
    TurnOffMotor(AUGER);

    //Wait half a second to let motor come to a full stop
    waitMicrosecond(500000);
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
