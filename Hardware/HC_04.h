#ifndef __HC_SR04_H
#define __HC_SR04_H

/*
HC-04 Tx - Rx PA3
HC-04 Rx - Tx PA2
*/
#define TX_PIN  GPIO_Pin_2
#define TX_PORT GPIOA
#define RX_PIN  GPIO_Pin_3
#define RX_PORT GPIOA

// 函数声明
void Serial2_Init(void);
void Serial2_SendByte(uint8_t);
void Serial2_SendArray(uint8_t*, uint16_t);
void Serial2_SendString(char*);
void Serial2_SendNumber(uint32_t);
void Serial2_Printf(char *format, ...);
uint8_t Serial2_GetRxFlag(void);
uint8_t Serial2_GetRxData(void);

#endif
