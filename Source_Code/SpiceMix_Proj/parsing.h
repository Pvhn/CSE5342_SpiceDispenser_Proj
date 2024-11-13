/*
 * parsing.h
 *
 *  Created on: Oct 10, 2024
 *      Author: <>
 */

#ifndef PARSING_H_
#define PARSING_H_

#include <stdbool.h>
#include <stdint.h>

#define MAX_CHARS 80
#define MAX_FIELDS 5
#define NUM_REFERENCE 5

typedef struct _USER_DATA
{
    char buffer[MAX_CHARS + 1];
    uint8_t fieldCount;
    uint8_t fieldPosition[MAX_FIELDS];
    char fieldType[MAX_FIELDS];
} USER_DATA;

bool isDelimiter(char c);
bool isNumber(char c);
void delimitersToNULL(char *string);
void getsUart0(USER_DATA *data);
void parseFields(USER_DATA *data);
char* getFieldString(USER_DATA *data, uint8_t fieldNumber);
int32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber);
bool isCommand(USER_DATA *data, char *strcommand, uint8_t minArguments);
int8_t identifyCommand(USER_DATA *data, char *commands[], uint8_t min_fields[]);
void clearBuffer(USER_DATA *data);
#endif /* PARSING_H_ */
