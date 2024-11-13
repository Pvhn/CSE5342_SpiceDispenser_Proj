/* =======================================================
 * File Name: StepMotor.c
 * =======================================================
 * File Description: Servo Motor Library.
 * Contains variables and functions
 * used for servo motor initialization and control
 * 
 * Target: TM4C123GH6PM w/ 40MHz Clock
 * =======================================================
 */

#include "Servo.h"
#include "wait.h"

/* =======================================================
 * Function Name: ServoInit
 * =======================================================
 * Parameters: None
 * Return: None
 * Description: Initializes peripherals necessary
 * for Servo Motor Control. This initializes Wide-Timer 3
 * and Port D4 for the PWM Timer output
 * =======================================================
 */
void ServoInit(void)
{
    // Initialize PORTD and Wide-Timer 3
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R3;
    _delay_cycles(3);

    // Initialize PORTD
    GPIO_PORTD_DIR_R |= 0x04;       // Enable PD4 as Output
    GPIO_PORTD_DEN_R |= 0x04;       // Set Digital Enable
    GPIO_PORTD_AFSEL_R |= 0x04;     // Set Alternate Function for PD4
    GPIO_PORTD_PCTL_R &= ~0x0F00;   // Clear function control for PD4
    GPIO_PORTD_PCTL_R |= 0x0700;    // Set PD4 for Wide-Timer 3 Output

    // Initialize Wide-Timer 3
    WTIMER3_CTL_R &= ~TIMER_CTL_TAEN;   // Disable Timer for Config
    WTIMER3_CFG_R = 0x04;               // Configure Wide-Timer as 32-Bit
    WTIMER3_TAMR_R = TIMER_TAMR_TAAMS | TIMER_TAMR_TAMR_PERIOD; // Configure Periodic PWM Timer Mode
    WTIMER3_TAILR_R = SYSCLOCK/ 50;      // Set Timer Frequency to 50Hz (ClockFreq/DesiredFreq)
    WTIMER3_TAV_R = 0;                  // Set Initial Value to 0
    WTIMER3_CTL_R |= TIMER_CTL_TAEN | TIMER_CTL_TAPWML; // Enable the timer and invert PWM output

    SetServoPos(180);
}

/* =======================================================
 * Function Name: SetServoPos
 * =======================================================
 * Parameters: angle
 * Return: None
 * Description: This sets the servo to the requested
 * angle by adjusting the Wide-Timer 3 Match Register
 * =======================================================
 */
void SetServoPos(uint16_t angle)
{
    uint32_t duty = 0;

    //Limit Servo Angle between 0 and 180
    if (angle > 180)
    {
        angle = 180;
    }

    // Calculate the required duty cycle
    duty = (uint32_t) ((angle / 180.0) * SERVOSF) + SERVOBIAS;

    // Set desired duty cycle to Wide Timer 3 Match Register
    WTIMER3_TAMATCHR_R = duty;

    waitMicrosecond(1000000);
}
