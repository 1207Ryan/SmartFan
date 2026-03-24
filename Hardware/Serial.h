#ifndef __SERIAL_H
#define __SERIAL_H

#include "stdio.h"

void Serial_Init(void);
void Serial_SendByte(uint8_t);
void Serial_SendArray(uint8_t*, uint16_t);
void Serial_SendString(char*);
void Serial_SendNumber(uint32_t);
void Serial_Printf(char *format, ...);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif
