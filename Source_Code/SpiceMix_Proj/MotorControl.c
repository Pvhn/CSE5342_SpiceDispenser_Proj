/* =======================================================
 * File Name: MotorControl.c
 * =======================================================
 * File Description: Contains variables and functions
 * used for controlling stepper and servo motors.
 * =======================================================
 */

#include "MotorControl.h"
#include "Servo.h"

/*========================================================
 * Variable Definitions
 *========================================================
 */
uint16_t rack_pos;

// System Calibration Values
int32_t HOME_OFFSET = -520;
uint16_t SVO_ENG_POS = 100;
uint16_t SVO_DIS_POS = 170;


/*========================================================
 * Function Declarations
 *========================================================
 */

/* =======================================================
 * Function Name: StepRackHome
 * =======================================================
 * Parameters: None
 * Return: None
 * Description: This function performs the homing of the
 * main rack. The rack motor is commanded to turn until
 * the hall sensors located at the home position, indicate
 * the rack is "home". The home_status state is set from
 * the hall sensor interrupt. As the rack approaches home,
 * the rack will be slowed until both sensors indicate
 * home. In the event home is never indicated, an
 * "HOME FAIL" error code will be returned
 * =======================================================
 */
uint16_t StepRackHome(void)
{
    MotorHomeStatEnumType home_status = NOTHOME;
    MotorRunStatEnumType run_status = RUNNING;
    bool nearhome_pv = false;

    home_status = GetMotorHomeStatus(RACK);

    if (home_status != HOME)
    {
        // Command the Rack Motor to make 3 full rotations
        CommandMotor(RACK, (USTEPFULL360 * 3 * GEARRATIO), 10);

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
                    SetMotorSpd(RACK, 6);
                    nearhome_pv = true;
                }
            }
            else
            {
                // If first transition out of near home, increase speed
                if (nearhome_pv == true)
                {
                    SetMotorSpd(RACK, 10);
                    nearhome_pv = 0;
                }
            }
        }

        if (home_status == HOME)
        {

            waitMicrosecond(100000);  // Wait atleast 1ms to allow motor to start running
            CommandMotor(RACK, HOME_OFFSET, 6);
            //TurnOffMotor(RACK);
            // Reset Rack position to 0 (Home);
            rack_pos = 0;
        }
        else
        {
            // Failed to find home before Motor Timeout
            if (run_status == HALTED)
            {
                run_status = FAILED;
                return ERRORHOMEFAIL;
            }
        }
    }

    // Reset Rack Motor Speed to default
    SetMotorSpd(RACK, 20);

    return 0;
}

/* =======================================================
 * Function Name: SetRackPos
 * =======================================================
 * Parameters: pos
 * Return: None
 * Description: This function will command the rack motor
 * to turn the rack to a specified position. Based on
 * the given position (0-7) the angle and corresponding
 * commanded steps is calculated. The function will
 * wait for the motor to execute and will return once
 * all steps have been executed.
 * =======================================================
 */
void SetRackPos(uint16_t pos)
{
    MotorRunStatEnumType status = OFF;
    uint16_t angle = 0;

    // Limit Position input
    if (pos > 7)
    {
        pos = 7;
    }

    // Calculate the required angle
    angle = pos*45;

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
    CommandMotor(RACK, microsteps, 30);

    while (status != HALTED)
    {
        status = GetMotorRunStatus(RACK);
    }

    //Wait half a second to let motor come to a full stop
    waitMicrosecond(500000);
}

/* =======================================================
 * Function Name: SetAugerPos
 * =======================================================
 * Parameters: rotations
 * Return: None
 * Description: This function commands the auger motor
 * to rotate a specified amount of full rotations.
 * The function will wait for the motor to complete
 * its command and will return once all steps have been
 * executed.
 * =======================================================
 */
void SetAugerPos(uint16_t rotations)
{
    MotorRunStatEnumType status = OFF;

    // Calculate position difference
    float delta = 360*rotations;

    // Convert angle to microsteps
    int32_t microsteps = (int32_t) delta/MICROSTEPSF;

    CommandMotor(AUGER, microsteps, 35);

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
 * Function Name: TestMotors
 * =======================================================
 * Parameters: None
 * Return: None
 * Description: This function is debugging only and allows
 * for testing of the motrs and the hardware.
 * A breakpoint can be used to set the desired position
 * and rotation arguments.
 * =======================================================
 */
void TestMotors(void)
{



}

/* =======================================================
 * Function Name: DispenseSequence
 * =======================================================
 * Parameters: position, quantity
 * Return: None
 * Description: This function will execute the dispense
 * sequence for the spice rack. This includes calls
 * to for setting the rack position, engaging/disengaging
 * the servo clutch, and turning the auger motor.
 * position is a specified slot on the rack. quantity
 * is the amount of half teaspoons.
 * =======================================================
 */
void DispenseSequence(uint8_t position, uint16_t quantity)
{
    SetRackPos(position);
    SetServoPos(SVO_ENG_POS);
    SetAugerPos(quantity);
    SetServoPos(SVO_DIS_POS);
}


