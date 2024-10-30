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

static const float sinarray[128] =
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

static const float cosarray[128] =
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

// For Debugging Remove the Static
MotorDataStructType MotorData[2] =
{
    {OFF, CW, NOTHOME, 0, 0, 3},
    {OFF, CW, NOTHOME, 0, 0, 3}
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
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0 | SYSCTL_RCGCPWM_R1;  // PWM0 Module for Motor Control
    _delay_cycles(3);

    // Initialize PORTB (Motor Output and PWM Control)
    GPIO_PORTB_DIR_R |= 0xF0;           // Enable PB0-7 as outputs
    GPIO_PORTB_DEN_R |= 0xF0;           // Set Digital Enable
    GPIO_PORTB_AFSEL_R |= 0xF0;         // Enable Alternate Function (for PWM) PORTB4-7
    GPIO_PORTB_PCTL_R &= ~0xFFFF0000;   // Clear bits for enabling
    GPIO_PORTB_PCTL_R |= 0x44440000;    // Enable M0PWM0-3 (PB4-7)

    // Initialize PORTF (Motor Output 2)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     // Unlock PORTF0
    GPIO_PORTF_CR_R |= 0x01;            // Unlock PORTF0
    GPIO_PORTF_DIR_R |= 0x0F;           // Enable PORTF0,1,2,3 as outputs
    GPIO_PORTF_DEN_R |= 0x0F;           // Enable PORTF0,1,2,3 as digital pins
    GPIO_PORTF_AFSEL_R |= 0x0F;         // Enable Alternate Function (for PWM) PORTF0-3
    GPIO_PORTF_PCTL_R &= ~0xFFFF;       // Clear bits for enabling
    GPIO_PORTF_PCTL_R |= 0x5555;        // Enable M1PWM0-3 (PF0-3)

    // Initialize PWM Module 0
    SYSCTL_SRPWM_R = SYSCTL_SRPWM_R0 | SYSCTL_SRPWM_R1;   // Reset PWM1 module
    SYSCTL_SRPWM_R = 0;     // Turn off Reset
    PWM0_0_CTL_R = 0;       // Turn-off PWM0 Gen0 (Drives PWM0/1)
    PWM0_1_CTL_R = 0;       // Turn-off PWM0 Gen1 (Drives PWM2/3)
    PWM1_2_CTL_R = 0;       // Turn-off PWM1 Gen0 (Drives PWM0/1)
    PWM1_3_CTL_R = 0;       // Turn-off PWM1 Gen1 (Drives PWM2/3)

    // Enable Global Sync for PWM Generators
    PWM0_0_CTL_R |= 0x3F8; // Global Sync for any comparator/load/gen update
    PWM0_1_CTL_R |= 0x3F8;
    PWM1_2_CTL_R |= 0x3F8;
    PWM1_3_CTL_R |= 0x3F8;

    // Set for PWM high when counter = comparator. Low when counter = 0
    // Generator B configured to look at comparator A
    PWM0_0_GENA_R = PWM_0_GENA_ACTCMPAD_ONE | PWM_0_GENA_ACTZERO_ZERO;
    PWM0_0_GENB_R = PWM_0_GENB_ACTCMPAD_ONE | PWM_0_GENB_ACTZERO_ZERO;
    PWM0_1_GENA_R = PWM_0_GENA_ACTCMPAD_ONE | PWM_0_GENA_ACTZERO_ZERO;
    PWM0_1_GENB_R = PWM_0_GENB_ACTCMPAD_ONE | PWM_0_GENB_ACTZERO_ZERO;
    
    PWM1_2_GENA_R = PWM_1_GENA_ACTCMPAD_ONE | PWM_1_GENA_ACTZERO_ZERO;
    PWM1_2_GENB_R = PWM_1_GENB_ACTCMPAD_ONE | PWM_1_GENB_ACTZERO_ZERO;
    PWM1_3_GENA_R = PWM_1_GENA_ACTCMPAD_ONE | PWM_1_GENA_ACTZERO_ZERO;
    PWM1_3_GENB_R = PWM_1_GENB_ACTCMPAD_ONE | PWM_1_GENB_ACTZERO_ZERO;

    // Set PWM Frequency
    PWM0_0_LOAD_R = PWMLOAD; // Set PWM Frequency
    PWM0_1_LOAD_R = PWMLOAD;

    PWM1_2_LOAD_R = PWMLOAD; // Set PWM Frequency
    PWM1_3_LOAD_R = PWMLOAD;

    // PWM Off on Power-up (Duty cycle = 0)
    PWM0_0_CMPA_R = 0;
    PWM0_0_CMPB_R = 0;
    PWM0_1_CMPA_R = 0;
    PWM0_1_CMPB_R = 0;

    PWM1_2_CMPA_R = 0;
    PWM1_2_CMPB_R = 0;
    PWM1_3_CMPA_R = 0;
    PWM1_3_CMPB_R = 0;

    PWM0_INTEN_R |= 0x01;
    PWM1_INTEN_R |= 0x04;

    NVIC_EN0_R |= 1 << (INT_PWM0_0 - 16);   // Register the PWM0 Interrupt
    NVIC_EN4_R |= 1 << (INT_PWM1_2 - 16-128);   // Register the PWM0 Interrupt
    
    // Enable PWM Generators
    PWM0_0_CTL_R |= 0x01;
    PWM0_1_CTL_R |= 0x01;
    PWM0_ENABLE_R = 0x00; // Enable PWM0 and 1;

    PWM1_2_CTL_R |= 0x01;
    PWM1_3_CTL_R |= 0x01;
    PWM1_ENABLE_R = 0x00;

    // Sync 
    PWM0_CTL_R = 0xF;
    PWM1_CTL_R = 0xF;
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
    GPIO_PORTD_IBE_R |= HALSEN_MASK;   // Interrupt on both edges
    GPIO_PORTD_ICR_R |= HALSEN_MASK;    // Clear any pending Interrupts
    GPIO_PORTD_IM_R |= HALSEN_MASK;     // Turn on Interrupts
    NVIC_EN0_R |= 1 << (INT_GPIOD - 16);    // Enable interrupts for Port D
}

void CommandMotor(uint32_t motorID, int32_t microsteps, uint16_t speed)
{
    int32_t sign = 1;
    if (speed < STEPMINPERIOD)
    {
        MotorData[motorID].period = STEPMINPERIOD;
    }
    else
    {
        MotorData[motorID].period = speed;
    }

    if (microsteps < 0)
    {
        MotorData[motorID].direction = CCW;
        sign = -1;
    }
    else
    {
        MotorData[motorID].direction = CW;
    }
    
    MotorData[motorID].steps = (uint32_t) microsteps*sign;

    switch (motorID)
    {
    case 0:
        // Enable Load Interrupt for Gen 0
        PWM0_0_INTEN_R |= 0x02;
        break;
    case 1:
        // Enable Load Interrupt for Gen 0
        PWM1_2_INTEN_R |= 0x02;
        break;
    default:
        break;
    }
}

/* =======================================================
 * Function Name: SetMotorSpd
 * =======================================================
 * Parameters: motor, CoilASpd, CoilBSpd
 * Return: None
 * Description:
 * This function sets the PWM output for the specified
 * Stepper Motor which is used to control the current
 * that each of the stepper motor coils will receive.
 * =======================================================
 */
void SetMotorSpd(uint32_t motorID, uint16_t speed)
{
    if (speed < 2)
    {
        MotorData[motorID].period = 2;
    }
    else
    {
        MotorData[motorID].period = speed;
    }
}

void TurnOffMotor(uint32_t motorID)
{
    switch (motorID)
    {
    case 0:
        PWM0_ENABLE_R &= ~0x0F;
        PWM0_0_INTEN_R &= ~0x02;
        // Sync and Update Gen0 and Gen 1
        PWM0_CTL_R = 0x03;
        break;
    case 1:
        PWM1_ENABLE_R &= ~0xF0;
        PWM1_2_INTEN_R &= ~0x02;
        // Sync and Update Gen0 and Gen 1
        PWM1_CTL_R = 0x0C;
        break;
    default:
        break;
    }

    MotorData[motorID].runstatus = OFF;
}

MotorRunStatEnumType GetMotorRunStatus(uint32_t motorID)
{
    return MotorData[motorID].runstatus;
}

MotorHomeStatEnumType GetMotorHomeStatus(uint32_t motorID)
{
    return MotorData[motorID].homestatus;
}

void PWM0Gen0_ISR(void)
{
    static const uint32_t motorID = 0;
    static uint16_t globalstep = 0;
    static uint32_t period_count = 0;
    int16_t sine = 0;
    int16_t cosine = 0;
    MotorRunStatEnumType status = OFF;

    // Create a local copy for consistency
    uint32_t steps = MotorData[motorID].steps;
    MotorDirEnumType dir = MotorData[motorID].direction;
    uint32_t period = MotorData[motorID].period * 2;

    // Check if motor has moved the needed amount of steps.
    if (steps > 0)
    {
        status = RUNNING;

        if (period_count < period)
        {
            sine = (sinarray[globalstep] * (PWMLOAD - 1));
            cosine = (cosarray[globalstep] * (PWMLOAD - 1));

            PWM0_0_CMPA_R = abs(sine);
            PWM0_1_CMPA_R = abs(cosine);

            if (sine > 0)
            {
                PWM0_ENABLE_R |= 0x1;
                PWM0_ENABLE_R &= ~0x2;
            }
            else
            {
                PWM0_ENABLE_R &= ~0x1;
                PWM0_ENABLE_R |= 0x2;
            }

            if (cosine > 0)
            {
                PWM0_ENABLE_R |= 0x4;
                PWM0_ENABLE_R &= ~0x8;
            }
            else
            {
                PWM0_ENABLE_R &= ~0x4;
                PWM0_ENABLE_R |= 0x8;
            }

            period_count++;
        }
        else
        {
            period_count = 0;
            steps--;
            globalstep = (globalstep + (1 * dir)) & 0x7F;
        }
    }
    else
    {
        status = HALTED;
        // Disable Load Interrupt
        PWM0_0_INTEN_R &= ~0x02;
    }

    MotorData[motorID].runstatus = status;
    MotorData[motorID].steps = steps;

    // Sync and Update Gen0 and Gen 1
    PWM0_CTL_R = 0x03;

    // Clear Load Interrupt
    PWM0_0_ISC_R |= 0x02;
}

void PWM1Gen2_ISR(void)
{
    static const uint32_t motorID = 1;
    static uint16_t globalstep = 0;
    static uint32_t period_count = 0;
    int16_t sine = 0;
    int16_t cosine = 0;
    MotorRunStatEnumType status = OFF;

    // Create a local copy for consistency
    uint32_t steps = MotorData[motorID].steps;
    MotorDirEnumType dir = MotorData[motorID].direction;
    uint32_t period = MotorData[motorID].period*2;

    // Check if motor has moved the needed amount of steps.
    if (steps > 0)
    {
        status = RUNNING;

        if(period_count < period)
        {
            sine = (sinarray[globalstep]*(PWMLOAD-1));
            cosine = (cosarray[globalstep]*(PWMLOAD-1));

            PWM1_2_CMPA_R = abs(sine);
            PWM1_3_CMPA_R = abs(cosine);

            if (sine > 0)
            {
                PWM1_ENABLE_R |= 0x10;
                PWM1_ENABLE_R &= ~0x20;
            }
            else
            {
                PWM1_ENABLE_R &= ~0x10;
                PWM1_ENABLE_R |= 0x20;
            }

            if (cosine > 0)
            {
                PWM1_ENABLE_R |= 0x40;
                PWM1_ENABLE_R &= ~0x80;
            }
            else
            {
                PWM1_ENABLE_R &= ~0x40;
                PWM1_ENABLE_R |= 0x80;
            }
            
            period_count++;
        }
        else
        {
            period_count = 0;
            steps--;
            globalstep = (globalstep + (1 * dir)) & 0x7F;
        }
    }
    else
    {
        status = HALTED;
        // Disable Load Interrupt
        PWM1_2_INTEN_R &= ~0x02;
    }

    MotorData[motorID].runstatus = status;
    MotorData[motorID].steps = steps;

    // Sync and Update Gen0 and Gen 1
    PWM1_CTL_R = 0x0C;

    // Clear Load Interrupt
    PWM1_2_ISC_R |= 0x02;
}

void PortDISR(void)
{
    uint16_t input = HALLSEN;
    MotorHomeStatEnumType homestatus = NOTHOME;

    if (input == 0)
    {
        homestatus = HOME;
    }
    else
    {
        if (input == 0x1 || input == 0x02)
        {
            homestatus = NEARHOME;
        }
        else
        {
            homestatus = NOTHOME;
        }
    }

    MotorData[0].homestatus = homestatus;

    GPIO_PORTD_ICR_R |= HALSEN_MASK;
}

