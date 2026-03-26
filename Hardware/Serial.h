#ifndef __SERIAL_H
#define __SERIAL_H

#include "stdio.h"

#define Serial_SizeofTxPacket 16
#define Serial_SizeofRxPacket 16
extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];

void Serial_Init(void);
void Serial_SendByte(uint8_t);
void Serial_SendArray(uint8_t*, uint16_t);
void Serial_SendString(char*);
void Serial_SendNumber(uint32_t);
void Serial_Printf(char *format, ...);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);
uint8_t Serial_GetRxPacketLength(void);
void Serial_SendPacket(void);

#endif
