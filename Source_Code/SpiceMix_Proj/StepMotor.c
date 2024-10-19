/* =======================================================
 * File Name: StepMotor.c
 * =======================================================
 * File Description: Stepper Motor Library
 * Contains variables and functions
 * used for stepper motor initialization and control
 * =======================================================
 */

#include "StepMotor.h"

/*========================================================
 * Variable Definitions
 *========================================================
 */
const float sinarray[128] =
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

const float cosarray[128] =
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

/*========================================================
 * Function Declarations
 *========================================================
 */

 /* =======================================================
  * Function Name: StepMotorInit
  * =======================================================
  * Parameters: None
  * Return: None
  * Description: Initializes the peripherals needed for
  * Stepper Motor Control. The system utilizes two stepper
  * motors. This will initialize Port B0-3 and PortF0-3
  * for the motor output. Additionally PWM module 0 (PB4-7)
  * are initialized for PWM control
  * =======================================================
  */
void StepMotorInit(void)
{
    // Enable GPIO PORTB, and PORTF
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R5;
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;  // PWM0 Module for Motor Control
    _delay_cycles(3);

    // Initialize PORTB (Motor Output and PWM Control)
    GPIO_PORTB_DIR_R |= 0xFF;           // Enable PB0-7 as outputs
    GPIO_PORTB_DEN_R |= 0xFF;           // Set Digital Enable
    GPIO_PORTB_AFSEL_R |= 0xF0;         // Enable Alternate Function (for PWM) PORTB4-7
    GPIO_PORTB_PCTL_R &= ~0xFFFF0000;   // Clear bits for enabling
    GPIO_PORTB_PCTL_R |= 0x44440000;    // Enable M0PWM0-3 (PB4-7)

    // Initialize PORTF (Motor Output 2)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     // Unlock PORTF0
    GPIO_PORTF_CR_R |= 0x01;            // Unlock PORTF0
    GPIO_PORTF_DIR_R |= 0x0F;           // Enable PORTF0,1,2,3 as outputs
    GPIO_PORTF_DEN_R |= 0x0F;           // Enable PORTF0,1,2,3 as digital pins

    // Initialize PWM Module 0
    SYSCTL_SRPWM_R = SYSCTL_SRPWM_R0;   // Reset PWM1 module
    SYSCTL_SRPWM_R = 0;     // Turn off Reset
    PWM0_0_CTL_R = 0;       // Turn-off PWM0 Gen0 (Drives PWM0/1)
    PWM0_1_CTL_R = 0;       // Turn-off PWM0 Gen1 (Drives PWM2/3)

    // Set for PWM high when counter = comparator. Low when = load
    PWM0_0_GENA_R = PWM_0_GENA_ACTCMPAD_ONE | PWM_0_GENA_ACTLOAD_ZERO;
    PWM0_0_GENB_R = PWM_0_GENB_ACTCMPBD_ONE | PWM_0_GENB_ACTLOAD_ZERO;
    PWM0_1_GENA_R = PWM_0_GENA_ACTCMPAD_ONE | PWM_0_GENA_ACTLOAD_ZERO;
    PWM0_1_GENB_R = PWM_0_GENB_ACTCMPBD_ONE | PWM_0_GENB_ACTLOAD_ZERO;

    // Set PWM Frequency
    PWM0_0_LOAD_R = PWMLOAD; // Set PWM Frequency
    PWM0_1_LOAD_R = PWMLOAD;

    // PWM Off on Power-up (Duty cycle = 0)
    PWM0_0_CMPA_R = 0;
    PWM0_0_CMPB_R = 0;
    PWM0_1_CMPA_R = 0;
    PWM0_1_CMPB_R = 0;

    // Enable PWM Generators
    PWM0_0_CTL_R = 0x01;
    PWM0_1_CTL_R = 0x01;
    PWM0_ENABLE_R = 0x0F; // Enable PWM0-3;
}

/* =======================================================
 * Function Name:
 * =======================================================
 * Parameters: None
 * Return: None
 * Description:
 * =======================================================
 */
void HallSensorInit(void)
{
    // Enable GPIO PORTD
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
    _delay_cycles(3);

    // Initialize PORTD (Hall Sensor Input)
    GPIO_PORTD_DIR_R &= ~HALSEN_MASK;   // Enable PD0/1 as Inputs
    GPIO_PORTD_DEN_R |= HALSEN_MASK;    // Set Digital Enable
    GPIO_PORTD_PUR_R |= HALSEN_MASK;    // Set Internal Pull-Up

    // Configuring GPIO Interrupts
    GPIO_PORTD_IM_R &= ~HALSEN_MASK;    // Mask out interrupts to prevent false trip during config
    GPIO_PORTD_IS_R &= ~HALSEN_MASK;    // Set for interrupt on edge detect
    GPIO_PORTD_IBE_R |= HALSEN_MASK;   // Clear IBE to use only single edge detect
    //GPIO_PORTD_IEV_R &= ~HALSEN_MASK;   // Set Falling Edge Interrupt
    GPIO_PORTD_ICR_R |= HALSEN_MASK;    // Clear any pending Interrupts
    GPIO_PORTD_IM_R |= HALSEN_MASK;     // Turn on Interrupts
    NVIC_EN0_R |= 1 << (INT_GPIOD - 16);    // Enable interrupts for Port D
}

/* =======================================================
 * Function Name: MoveMotor
 * =======================================================
 * Parameters: motor, microsteps, speed
 * Return: globalstep
 * Description: 
 * This function will move the specified
 * motor the number of microsteps at a specified frequency.
 * Speed is a value ranging from 2 to 6 and is used to 
 * determine the frequency at which the Stepper Motor will
 * move, where 2 is the fastest, and 6 being the slowest.
 * The motor input is a data structure containing
 * information that is used to output to a specified motor.
 * The function returns a globalstep value which is used
 * to track the last coil position of the motor.
 * =======================================================
 */
uint16_t MoveMotor(MotorDataStruct motor, int32_t microsteps, uint16_t speed)
{
    bool dir1 = 0;
    bool dir2 = 0;
    uint16_t output;
    uint32_t k = 0;
    int16_t sign = 1;
    uint16_t coilAspd = 0;
    uint16_t coilBspd = 0;
    float cosine = 0.0f;
    float sine = 0.0f;

    uint16_t globalstep = motor.globalstep;

    if (speed < 2)
    {
        speed = 2;
    }
    else
    {
        if (speed > 10)
        {
            speed = 6;
        }
    }

    // If negative steps, flip sign for CCW rotation
    if (microsteps < 0)
    {
        sign = -1;
    }

    for (k = 0; k < (microsteps * sign); k++)
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

        SetMotorCoilSpd(motor.type, coilAspd, coilBspd);
        *(motor.output) = output;

        // Increment/Decrement based on rotation
        globalstep = (globalstep + (1 * sign)) & 0x7F;

        // Increment the step
        waitMicrosecond(PWMPERIODUS * speed);
    }

    return globalstep;
}

/* =======================================================
 * Function Name: SetMotorCoilSpd
 * =======================================================
 * Parameters: motor, CoilASpd, CoilBSpd
 * Return: None
 * Description:
 * This function sets the PWM output for the specified
 * Stepper Motor which is used to control the current
 * that each of the stepper motor coils will receive.
 * =======================================================
 */
void SetMotorCoilSpd(MotorTypeEnum motor, uint16_t CoilASpd, uint16_t CoilBSpd)
{
    if (motor == RACK)
    {
        PWM0_0_CMPA_R = CoilASpd;
        PWM0_0_CMPB_R = CoilBSpd;
    }
    else
    {
        PWM0_1_CMPA_R = CoilASpd;
        PWM0_1_CMPB_R = CoilBSpd;
    }
}

