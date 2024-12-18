/* =======================================================
 * File Name: StepMotor.c
 * =======================================================
 * File Description: Stepper Motor Library
 * Contains variables and functions
 * used for stepper motor initialization and control
 * =======================================================
 */

#include "StepMotor.h"
#include "tm4c123gh6pm.h"
#include "wait.h"

/*========================================================
 * Variable Definitions
 *========================================================
 */

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
  * motors. This will initialize Port B5-7 and PortF0-3
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
    GPIO_PORTB_DIR_R |= 0xE0;           // Enable PB5-7 as outputs
    GPIO_PORTB_DEN_R |= 0xE0;           // Set Digital Enable
    GPIO_PORTB_AFSEL_R |= 0x40;         // Enable Alternate Function (for PWM) PORTB6
    GPIO_PORTB_PCTL_R &= ~0x0F000000;   // Clear bits for enabling
    GPIO_PORTB_PCTL_R |= 0x04000000;    // Enable M0PWM0 (PB6))

    // Initialize PORTF (Motor Output 2)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     // Unlock PORTF0
    GPIO_PORTF_CR_R |= 0x01;            // Unlock PORTF0
    GPIO_PORTF_DIR_R |= 0x07;           // Enable PORTF0,1,2, as outputs
    GPIO_PORTF_DEN_R |= 0x07;           // Enable PORTF0,1,2 as digital pins
    GPIO_PORTF_AFSEL_R |= 0x01;         // Enable Alternate Function (for PWM) PORTF0-2
    GPIO_PORTF_PCTL_R &= ~0x0005;       // Clear bits for enabling
    GPIO_PORTF_PCTL_R |= 0x0005;        // Enable M1PWM2 (PF0)

    // Initialize PWM Module 0
    SYSCTL_SRPWM_R = SYSCTL_SRPWM_R0| SYSCTL_SRPWM_R1;   // Reset PWM1 module
    SYSCTL_SRPWM_R = 0;     // Turn off Reset
    PWM0_0_CTL_R = 0;       // Turn-off PWM1 Gen1 (Drives PWM2/3)
    PWM1_2_CTL_R = 0;       // Turn-off PWM1 Gen1 (Drives PWM2/3)

    // Enable Global Sync for PWM Generators
    PWM0_0_CTL_R |= 0x3F8;
    PWM1_2_CTL_R |= 0x3F8;

    // Set for PWM high when counter = comparator. Low when counter = 0
    // Generator B configured to look at comparator A
    PWM0_0_GENA_R = PWM_0_GENA_ACTCMPAD_ONE | PWM_0_GENA_ACTZERO_ZERO;
    PWM1_2_GENA_R = PWM_1_GENA_ACTCMPAD_ONE | PWM_1_GENA_ACTZERO_ZERO;

    // Set PWM Frequency
    PWM0_0_LOAD_R = 0xFFFF; //Max load = lowest RPM
    PWM1_2_LOAD_R = 0xFFFF; //Max load = lowest RPM

    // PWM Off on Power-up (Duty cycle = 0)
    PWM0_0_CMPA_R = 0x0FF;
    PWM1_2_CMPA_R = 0x0FF;

    PWM0_INTEN_R |= 0x01;
    PWM1_INTEN_R |= 0x04;

    NVIC_EN0_R |= 1 << (INT_PWM0_0 - 16);   // Register the PWM0 Interrupt
    NVIC_EN4_R |= 1 << (INT_PWM1_2 - 16-128);   // Register the PWM0 Interrupt
    
    // Enable PWM Generators
    PWM0_0_CTL_R |= 0x01;
    PWM0_1_CTL_R |= 0x01;
    PWM0_ENABLE_R = 0x00;;

    PWM1_2_CTL_R |= 0x01;
    PWM1_ENABLE_R = 0x00;

    // Sync 
    PWM0_CTL_R = 0xF;
    PWM1_CTL_R = 0xF;

    // Turn Off Motors on Start-up
    MOTOR0EN = 1;
    MOTOR1EN = 1;
}

/* =======================================================
 * Function Name: HallSensorInit
 * =======================================================
 * Parameters: None
 * Return: None
 * Description: This function initializes the peripherals
 * needed for the hall sensor detection. This will
 * initialize PORTB0 and 1 for the hall sensor.
 * =======================================================
 */
void HallSensorInit(void)
{
    // Enable GPIO PORTB
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    _delay_cycles(3);

    // Initialize PORTB (Hall Sensor Input)
    GPIO_PORTB_DIR_R &= ~HALSEN_MASK;   // Enable PB0/1 as Inputs
    GPIO_PORTB_DEN_R |= HALSEN_MASK;    // Set Digital Enable
    GPIO_PORTB_PUR_R |= HALSEN_MASK;    // Set Internal Pull-Up

    // Configuring GPIO Interrupts
    GPIO_PORTB_IM_R &= ~HALSEN_MASK;    // Mask out interrupts to prevent false trip during config
    GPIO_PORTB_IS_R &= ~HALSEN_MASK;    // Set for interrupt on edge detect
    GPIO_PORTB_IBE_R |= HALSEN_MASK;   // Interrupt on both edges
    GPIO_PORTB_ICR_R |= HALSEN_MASK;    // Clear any pending Interrupts
    GPIO_PORTB_IM_R |= HALSEN_MASK;     // Turn on Interrupts
    NVIC_EN0_R |= 1 << (INT_GPIOB - 16);    // Enable interrupts for Port D
}

/* =======================================================
 * Function Name: CommandMotor
 * =======================================================
 * Parameters: MotorID, microsteps, speed
 * Return: None
 * Description: This function will begin the process of
 * commanding a stepper motor specified by the motorID.
 * The function will set the direction pins and enable 
 * the requested motor. The function will also enable the
 * corresponding PWM interrupt to begin the motor command.
 * =======================================================
 */
void CommandMotor(uint32_t motorID, int32_t microsteps, uint16_t speed)
{
    uint32_t dir = CW;
    int32_t sign = 1;

    SetMotorSpd(motorID, speed);

    if (microsteps < 0)
    {
        dir = CCW;
        sign = -1;
    }
    
    MotorData[motorID].steps = (uint32_t) microsteps*sign;

    switch (motorID)
    {
    case 0:
        MOTOR0DIR = dir;
        // Enable Load Interrupt for Gen 0

        PWM0_0_INTEN_R |= 0x02;
        break;
    case 1:
        MOTOR1DIR = dir;
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
 * Parameters: motorID, speed
 * Return: None
 * Description:
 * This function sets the PWM output for the specified
 * Stepper Motor. The function will save the speed to the
 * corresponding motor data struct which will then be
 * used by the PWM Interrupt handler to set the desired
 * speed.
 * =======================================================
 */
void SetMotorSpd(uint32_t motorID, uint16_t speed)
{
    if (speed < MINRPM)
    {
        MotorData[motorID].speed = MINRPM;
    }
    else
    {
        MotorData[motorID].speed = speed;
    }
}

/* =======================================================
 * Function Name: TurnOffMotor
 * =======================================================
 * Parameters: motorID
 * Return: None
 * Description:
 * This function will disable the specified motor. 
 * The function will disable the corresponding PWM interupt
 * as well as disable the PWM Outputs and stepper driver
 * which will effectively remove all power to the motor.
 * =======================================================
 */
void TurnOffMotor(uint32_t motorID)
{
    switch (motorID)
    {
    case 0:
        PWM0_ENABLE_R &= ~0x0F;
        PWM0_0_INTEN_R &= ~0x02;
        // Sync and Update Gen0 and Gen 1
        PWM0_CTL_R = 0x03;
        MOTOR0EN = 1;
        break;
    case 1:
        PWM1_ENABLE_R &= ~0xF0;
        PWM1_2_INTEN_R &= ~0x02;
        // Sync and Update Gen0 and Gen 1
        PWM1_CTL_R = 0x0C;
        MOTOR1EN = 1;
        break;
    default:
        break;
    }

    MotorData[motorID].runstatus = OFF;
}

/* =======================================================
 * Function Name: GetMotorRunStatus
 * =======================================================
 * Parameters: motorID
 * Return: RunStatus
 * Description:
 * This is a helper function to read the current status
 * of a motor. This is mainly used to prevent unintended
 * access to the Motor Data Structure by other modules.
 * =======================================================
 */
MotorRunStatEnumType GetMotorRunStatus(uint32_t motorID)
{
    return MotorData[motorID].runstatus;
}


/* =======================================================
 * Function Name: GetMotorHomeStatus
 * =======================================================
 * Parameters: motorID
 * Return: RunStatus
 * Description:
 * This is a helper function to read the current home status
 * of a motor. This is mainly used to prevent unintended
 * access to the Motor Data Structure by other modules.
 * =======================================================
 */
MotorHomeStatEnumType GetMotorHomeStatus(uint32_t motorID)
{
    return MotorData[motorID].homestatus;
}

/* =======================================================
 * Function Name: PWM0Gen0_ISR
 * =======================================================
 * Parameters: N/A
 * Return: N/A
 * Description:
 * This is the PWM Interrupt Handler for the Rack Motor.
 * This will interrupt when the PWM counter is zero. The
 * primary responsiblities of the interrupt handler is
 * to manage the steps the motor has moved as well as 
 * control the speed of the motor. All pertinent data
 * is provided from the corresponding MotorData structure.
 * The interrupt will read from this and then save the
 * updated value.
 * =======================================================
 */
void PWM0Gen0_ISR(void)
{
    static const uint32_t motorID = 0;
    MotorRunStatEnumType status = OFF;
    static uint32_t accel_steps = 0;
    static uint32_t deccel_steps = 0;
    static float deccel_factor = 0;
    static float speed = MINRPM;

    // Create a local copy for consistency
    uint32_t steps = MotorData[motorID].steps;
    float max_speed = MotorData[motorID].speed;
    MotorRunStatEnumType status_pv = MotorData[motorID].runstatus;
    uint32_t load = 0;

    if (status_pv != RUNNING)
    {
        accel_steps = steps / 2;
        deccel_steps = steps / 2;
        speed = MINRPM;
        load = (uint32_t) (RPMtoLOAD/speed);
        PWM0_0_LOAD_R = load;
        PWM0_0_CMPA_R = load * 0.5;
    }

    // Check if motor has moved the needed amount of steps.
    if (steps > 0)
    {
        if(accel_steps > 0 && steps > 600)
        {
            speed = speed + 0.03125;
            if (speed > max_speed)
            {
                speed = max_speed;
            }
            accel_steps--;

            deccel_factor = speed / deccel_steps;
        }
        else
        {
            if (deccel_steps > 0)
            {
                speed = speed - deccel_factor;
                if (speed < MINRPM)
                {
                    speed = MINRPM;
                }
            }
        }

        load = (uint32_t)(RPMtoLOAD / speed);
        PWM0_0_LOAD_R = load;
        PWM0_0_CMPA_R = load * 0.5;

        PWM0_ENABLE_R |= 0x01;
        MOTOR0EN = 0;
        status = RUNNING;
        steps--;
    }
    else
    {
        status = HALTED;
        // Disable Load Interrupt
        PWM0_ENABLE_R &= ~0x01;
        PWM0_0_INTEN_R &= ~0x02;
    }

    MotorData[motorID].runstatus = status;
    MotorData[motorID].steps = steps;

    // Sync and Update Gen0 and Gen 1
    PWM0_CTL_R = 0x03;

    // Clear Load Interrupt
    PWM0_0_ISC_R |= 0x02;
}

/* =======================================================
 * Function Name: PWM1Gen2_ISR
 * =======================================================
 * Parameters: N/A
 * Return: N/A
 * Description:
 * This is the PWM Interrupt Handler for the Auger Motor.
 * This will interrupt when the PWM counter is zero. The
 * primary responsiblities of the interrupt handler is
 * to manage the steps the motor has moved as well as
 * control the speed of the motor. All pertinent data
 * is provided from the corresponding MotorData structure.
 * The interrupt will read from this and then save the
 * updated value.
 * =======================================================
 */
void PWM1Gen2_ISR(void)
{
    static const uint32_t motorID = 1;
    MotorRunStatEnumType status = OFF;

    // Create a local copy for consistency
    uint32_t steps = MotorData[motorID].steps;
    float speed = MotorData[motorID].speed;
    uint32_t load = (uint32_t)(RPMtoLOAD / speed);

    PWM1_2_LOAD_R = load;
    PWM1_2_CMPA_R = load * 0.5;

    // Check if motor has moved the needed amount of steps.
    if (steps > 0)
    {
        PWM1_ENABLE_R |= 0x10;
        MOTOR1EN = 0;
        status = RUNNING;
        steps--;
    }
    else
    {
        status = HALTED;
        // Disable Load Interrupt
        PWM1_ENABLE_R &= ~0x10;
        PWM1_2_INTEN_R &= ~0x02;
    }

    MotorData[motorID].runstatus = status;
    MotorData[motorID].steps = steps;

    // Sync and Update Gen0 and Gen 1
    PWM1_CTL_R = 0x0C;

    // Clear Load Interrupt
    PWM1_2_ISC_R |= 0x02;
}

/* =======================================================
 * Function Name: PortBISR
 * =======================================================
 * Parameters: N/A
 * Return: N/A
 * Description:
 * This is the GPIO Interrupt Handler for the Hall
 * Sensors. This will interrupt on either the rising
 * or falling edge of the two rack hall sensors (PB0/PB1).
 * The primary responsiblities of the interrupt handler
 * is to handle the transition of a motor home status
 * and save the data for use by the main program.
 * =======================================================
 */
void PortBISR(void)
{
    uint16_t input = HALLSEN;
    MotorHomeStatEnumType homestatus = NOTHOME;
    
    // If both sensor indicates a detection
    if (input == 0)
    {
        homestatus = HOME;
    }
    else
    {
        // If atleast one sensor indicates
        // a detection. Otherwise, homestatus = NOTHOME
        if (input == 0x1 || input == 0x02)
        {
            homestatus = NEARHOME;
        }
        else
        {
            homestatus = NOTHOME;
        }
    }

    // Save the data to the rack motor dta
    MotorData[0].homestatus = homestatus;

    // Clear the interrupt flag.
    GPIO_PORTB_ICR_R |= HALSEN_MASK;
}

