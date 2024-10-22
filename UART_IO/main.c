#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "clock.h"
#include "wait.h"
#include "parsing.h"
#include "uart0.h"
#include "tm4c123gh6pm.h"

//Port E masks, names from schematic
#define A1_MASK 0x10
#define A2_MASK 0x20
#define B1_MASK 0x04
#define B2_MASK 0x08
char *commands[] = {"echo", "about", "count", "accumulate"};
uint8_t min_fields[4] = {2, 1, 2, 2};

// Initialize Hardware
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

    // Enable clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
    _delay_cycles(3);

    // Configure motor controller pins
    GPIO_PORTE_DIR_R |= A1_MASK | A2_MASK | B1_MASK | B2_MASK;  // all outputs
    GPIO_PORTE_DR2R_R |= A1_MASK | A2_MASK | B1_MASK | B2_MASK; // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTE_DEN_R |= A1_MASK | A2_MASK | B1_MASK | B2_MASK;  // enable all

}

void echo(USER_DATA *data)
{
    putsUart0(&(data->buffer[data->fieldPosition[1]]));
    putsUart0("\n");
}

void about()
{
    putsUart0("This is a test project to show the use of parsing.h.\n");
}

void count(USER_DATA *data)
{
    char str[10];
    uint8_t buffer = 0;
    uint8_t index = data->fieldPosition[1];
    while(data->buffer[index] != 0)
    {
        buffer++;
        index++;
    }
    sprintf(str,"%d", buffer);
    putsUart0(str);
    putsUart0("\n");
    if(data->fieldCount > 2)
        putsUart0("Spaces are used as delimiters, so excess fields were not counted.\n");
}

int8_t accumulate(USER_DATA *data, int8_t accumulator)
{
    int8_t buffer = 0;
    if(data->fieldCount == 3)
    {
    buffer = getFieldInteger(data, 2);
    }
    char str[25];
    if(strcmp(getFieldString(data, 1), "increase") == 0)
    {
        accumulator = accumulator + buffer;
    }
    else if(strcmp(getFieldString(data, 1), "decrease") == 0)
    {
        accumulator = accumulator - buffer;
    }
    else if(strcmp(getFieldString(data, 1), "check") == 0){}
    else
    {
        putsUart0("Error: missing syntax. Specify 'increase' or 'decrease'.\n");
    }
    sprintf(str, "Accumulator value is %d\n", accumulator);
    putsUart0(str);
    return accumulator;
}


/**
 * main.c
 */
int main(void)
{
    initHw();
    initUart0();
    USER_DATA data;
    int8_t code = -1;
    int8_t accumulator = 0;
    // Setup UART0 baud rate
    setUart0BaudRate(115200, 40e6);
    putsUart0("UART Test\n");
    while(true)
    {
        clearBuffer(&data);
        getsUart0(&data);
        putsUart0("\n");
        code = identifyCommand(&data, &commands, min_fields);
        if(code == -1)
            putsUart0("Error: command not recognized\n");
        else
        {
            switch(code)
            {
                case 0:
                    echo(&data);
                    break;
                case 1:
                    about();
                    break;
                case 2:
                    count(&data);
                    break;
                case 3:
                    accumulator = accumulate(&data, accumulator);
                    break;
            }
        }
    }
}
