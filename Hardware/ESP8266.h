#ifndef __ESP8266_H
#define __ESP8266_H

#include "stdio.h"

#define TX_PIN  GPIO_Pin_10
#define TX_PORT GPIOB
#define RX_PIN  GPIO_Pin_11
#define RX_PORT GPIOB

#define Serial3_SizeofTxPacket 16
#define Serial3_SizeofRxPacket 1024
extern char Serial3_TxPacket[];
extern char Serial3_RxPacket[];

void Serial3_Init(void);
void Serial3_SendByte(uint8_t);
void Serial3_SendArray(uint8_t*, uint16_t);
void Serial3_SendString(char*);
void Serial3_SendNumber(uint32_t);
void Serial3_Printf(char *format, ...);
uint8_t Serial3_GetRxFlag(void);
uint8_t Serial3_GetRxData(void);
uint8_t Serial3_GetRxPacketLength(void);
void Serial3_SendPacket(void);
uint8_t Serial3_GetRxPacketLength(void);
uint16_t Serial3_GetRxLength(void);
void Serial3_ClearRxBuffer(void);

#endif
