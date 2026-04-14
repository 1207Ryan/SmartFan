#ifndef __SERIAL_H
#define __SERIAL_H

/*
ASRPRO Tx PA3 -  Tx PA9
ASRPRO Tx PA2 -  Rx PA10
HC-04 Tx - Rx PA3
HC-04 Rx - Tx PA2
ESP8266 Tx - Rx PB11
ESP8266 Rx - Tx PB10
*/

#define ASRPRO_TX_PIN   GPIO_Pin_9
#define ASRPRO_TX_PORT  GPIOA
#define ASRPRO_RX_PIN   GPIO_Pin_10
#define ASRPRO_RX_PORT  GPIOA
#define HC_04_TX_PIN    GPIO_Pin_2
#define HC_04_TX_PORT   GPIOA
#define HC_04_RX_PIN    GPIO_Pin_3
#define HC_04_RX_PORT   GPIOA
#define ESP8266_TX_PIN  GPIO_Pin_10
#define ESP8266_TX_PORT GPIOB
#define ESP8266_RX_PIN  GPIO_Pin_11
#define ESP8266_RX_PORT GPIOB

#define Serial_SizeofTxPacket 16
#define Serial_SizeofRxPacket 1024
extern char Serial_TxPacket[];
extern char Serial_RxPacket[];
extern uint8_t Serial_TxDataPacket[Serial_SizeofTxPacket];
extern uint8_t Serial1_RxDataPacket[Serial_SizeofRxPacket];
extern uint8_t Serial2_RxDataPacket[Serial_SizeofRxPacket];
extern uint8_t Serial3_RxDataPacket[Serial_SizeofRxPacket];
extern char Serial_TxPacket[Serial_SizeofTxPacket];
extern char Serial1_RxPacket[Serial_SizeofRxPacket];
extern char Serial2_RxPacket[Serial_SizeofRxPacket];
extern char Serial3_RxPacket[Serial_SizeofRxPacket];

void Serial_Init(uint8_t usartx);
void Serial_SendByte(uint8_t usartx, uint8_t);
void Serial_SendArray(uint8_t usartx, uint8_t*, uint16_t);
void Serial_SendString(uint8_t usartx, char*);
void Serial_SendNumber(uint8_t usartx, uint32_t);
void Serial_Printf(uint8_t usartx, char *format, ...);
uint8_t Serial_GetRxFlag(uint8_t usartx);
uint8_t Serial_GetRxData(uint8_t usartx);
uint16_t Serial_GetRxPacketLength(uint8_t usartx);
void Serial_Start(uint8_t usartx);
void Serial_Stop(uint8_t usartx);
void Serial_SendPacket(uint8_t usartx, uint16_t length);
void Serial_SetTxDataPacket(uint8_t len, ...);
void Serial_ClearTxBuffer(void);
void Serial_ClearRxBuffer(uint8_t usartx);

#endif
