/*
 * parsing.c
 *
 *  Created on: Oct 10, 2024
 *      Author: <>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "parsing.h"

#define MAX_CHARS 80
#define MAX_FIELDS 5
#define NUM_REFERENCE 5

//-----------------------------------------------------------------------------
// User Interface Subroutines
//-----------------------------------------------------------------------------

//Because I didn't want to write these messes again - checks for all delimiters...
bool isDelimiter(char c)
{
    return (c == 47 || c < 45 || (c > 57 && c < 65) || (c > 90 && c < 97) || c > 122);
}

//And checks for numerical values.
bool isNumber(char c)
{
    return (c > 44 && c < 47) || (c > 47 && c < 58);
}

//Didn't feel like cleverly incorporating this into one loop through the buffer.
//Replaces delimiters with NULL values.
void delimitersToNULL(char *string)
{
    uint32_t i = 0;
    while (i <= MAX_CHARS)
    {
        if(isDelimiter(string[i]))
            string[i] = '\0';
        i++;
    }
}

void getsUart0(USER_DATA *data)
{
    int count = 0;

    while(true)
    {
        while (UART0_FR_R & UART_FR_RXFE);               // wait if uart0 rx fifo empty
        uint32_t c = UART0_DR_R & 0xFF;                        // get character from fifo
        if ((c == 8 && count > 0) || (c == 127 && count > 0))
            count--;
        else if (c == 13)
        {
            data->buffer[count] = 0;
            return;
        } else if (c >= 32)
        {
            data->buffer[count] = c;
            count++;
            if (count == MAX_CHARS)
            {
                data->buffer[count] = 0;
                return;
            }
        }
    }
}

void parseFields(USER_DATA *data)
{
    data->fieldCount = 0;
    uint8_t i = 0;
    char last = 32;
    while(data->buffer[i] != '\0')
    {
        if(isDelimiter(last) && !isDelimiter(data->buffer[i]))
        {
            if(isNumber(data->buffer[i]))
                data->fieldType[data->fieldCount] = 'n';
            else
                data->fieldType[data->fieldCount] = 'a';
            data->fieldPosition[data->fieldCount] = i;
            data->fieldCount++;
        }
        if(data->fieldCount == MAX_FIELDS)
            return;
        i++;
        last = data->buffer[i-1];
    }
    data->fieldPosition[data->fieldCount] = i;
    //Lazy implementation
    delimitersToNULL(data->buffer);
}

char* getFieldString(USER_DATA *data, uint8_t fieldNumber)
{
    if(fieldNumber >= MAX_FIELDS)
        return 0;
    return &(data->buffer[data->fieldPosition[fieldNumber]]);
}

int32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber)
{
    return atoi(getFieldString(data, fieldNumber));
}

bool isCommand(USER_DATA *data, char *strcommand, uint8_t minArguments)
{
    if(strcmp(strcommand, getFieldString(data, 0)) == 0 && minArguments <= data->fieldCount)
        return true;
    else
        return false;
}

int8_t identifyCommand(USER_DATA *data, UICmdStructType commands[], uint8_t num_commands)
{
    parseFields(data);
    int8_t i = 0;
    for(i = 0; i < num_commands; i++)
    {
        if(isCommand(data, commands[i].cmd, commands[i].min_fields))
        {
            return i;
        }
    }
    return -1;
}

void clearBuffer(USER_DATA *data)
{
    uint8_t i;
    for(i = 0; i < MAX_CHARS; i++)
    {
        data->buffer[i] = 0;
    }
    for(i = 0; i < MAX_FIELDS; i++)
    {
        data->fieldPosition[i] = 0;
    }
    for(i = 0; i < MAX_FIELDS; i++)
    {
        data->fieldType[i] = 0;
    }
}
