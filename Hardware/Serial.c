#include "stm32f10x.h"                  // Device header
#include "stdarg.h"
#include "Serial.h"
#include <stdio.h>
#include <string.h>
#include "MyRTC.h"

uint8_t Serial_TxDataPacket[Serial_SizeofTxPacket];
uint8_t Serial1_RxDataPacket[Serial_SizeofRxPacket];
uint8_t Serial2_RxDataPacket[Serial_SizeofRxPacket];
uint8_t Serial3_RxDataPacket[Serial_SizeofRxPacket];
char Serial_TxPacket[Serial_SizeofTxPacket];
char Serial1_RxPacket[Serial_SizeofRxPacket];
char Serial2_RxPacket[Serial_SizeofRxPacket];
char Serial3_RxPacket[Serial_SizeofRxPacket];

uint8_t Serial1_RxData;
uint8_t Serial2_RxData;
uint8_t Serial3_RxData;
uint8_t Serial1_RxFlag;
uint8_t Serial2_RxFlag;
uint8_t Serial3_RxFlag;
static uint16_t Serial1_RxLen = 0;
static uint16_t Serial2_RxLen = 0;
static uint16_t Serial3_RxLen = 0;

void Serial_Init(uint8_t usartx)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	if(usartx == 1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		//TX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//推挽输出
		GPIO_InitStructure.GPIO_Pin = ASRPRO_TX_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(ASRPRO_TX_PORT, &GPIO_InitStructure);
		//RX
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
		GPIO_InitStructure.GPIO_Pin = ASRPRO_RX_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(ASRPRO_RX_PORT, &GPIO_InitStructure);
		
		USART_InitStructure.USART_BaudRate = 9600;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_Init(USART1, &USART_InitStructure);
		
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		
		//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
		NVIC_Init(&NVIC_InitStructure);
		
		USART_Cmd(USART1, ENABLE);
	}else if(usartx == 2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		//TX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//推挽输出
		GPIO_InitStructure.GPIO_Pin = HC_04_TX_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(HC_04_TX_PORT, &GPIO_InitStructure);
		
		//RX
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
		GPIO_InitStructure.GPIO_Pin = HC_04_RX_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(HC_04_RX_PORT, &GPIO_InitStructure);
		
		USART_InitStructure.USART_BaudRate = 9600;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_Init(USART2, &USART_InitStructure);
		
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		
		//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
		NVIC_Init(&NVIC_InitStructure);
		
		USART_Cmd(USART2, ENABLE);
	}else if(usartx == 3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		
		//TX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//推挽输出
		GPIO_InitStructure.GPIO_Pin = ESP8266_TX_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(ESP8266_TX_PORT, &GPIO_InitStructure);
		
		//RX
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
		GPIO_InitStructure.GPIO_Pin = ESP8266_RX_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(ESP8266_RX_PORT, &GPIO_InitStructure);
		
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_Init(USART3, &USART_InitStructure);
		
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		
		//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
		NVIC_Init(&NVIC_InitStructure);
		
		USART_Cmd(USART3, ENABLE);
	}
}

void Serial_SendByte(uint8_t usartx, uint8_t Byte)//发送字节数据
{
	if(usartx == 1){
		USART_SendData(USART1, Byte);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
		//判断TXE是否置1，即数据是否从发送数据寄存器转移到发送移位寄存器
		//标志寄存器会自动清零
	}else if(usartx == 2){
		USART_SendData(USART2, Byte);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	
	}else if(usartx == 3){
		USART_SendData(USART3, Byte);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	
	}
}

void Serial_SendArray(uint8_t usartx, uint8_t *Array, uint16_t length)
{
	for(uint8_t i=0; i<length; i++){
		Serial_SendByte(usartx, Array[i]);
	}
}

void Serial_SendString(uint8_t usartx, char* String)
{
	for(uint16_t i=0; String[i] != '\0'; i++){
		Serial_SendByte(usartx, String[i]);
	}
}

void Serial_SendNumber(uint8_t usartx, uint32_t Number)
{
	uint32_t mask =1;//位数
	uint32_t tem =Number;
	for(; tem>9; mask*=10){
		tem /=10;
	}
	
	for(; Number!=0; Number%=mask,mask/=10){
		Serial_SendByte(usartx, Number/mask+'0');//从高位开始发
	}
}

int fputc1(int ch, FILE *f)
{
	Serial_SendByte(1, ch);
	return ch;
}

int fputc2(int ch, FILE *f)
{
	Serial_SendByte(2, ch);
	return ch;
}

int fputc3(int ch, FILE *f)
{
	Serial_SendByte(3, ch);
	return ch;
}

void Serial_Printf(uint8_t usartx, char *format, ...)//可变参数
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);//释放参数表
	Serial_SendString(usartx, String);
}

uint8_t Serial_GetRxFlag(uint8_t usartx)
{
	if(usartx == 1){
		if(Serial1_RxFlag == 1){
			Serial1_RxFlag =0;
			return 1;
		}
		return 0;
	}else if(usartx == 2){
		if(Serial2_RxFlag == 1){
			Serial2_RxFlag =0;
			return 1;
		}
		return 0;
	}else if(usartx == 3){
		if(Serial3_RxFlag == 1){
			Serial3_RxFlag =0;
			return 1;
		}
		return 0;
	}
	
}

uint8_t Serial_GetRxData(uint8_t usartx)
{
	if(usartx == 1){
		return Serial1_RxData;
	}else if(usartx == 2){
		return Serial2_RxData;
	}else if(usartx == 3){
		return Serial3_RxData;
	}
}

void Serial_SendPacket(uint8_t usartx)
{
	Serial_SendByte(usartx, 0xFF);
	Serial_SendArray(usartx, Serial_TxDataPacket, Serial_SizeofTxPacket);
	Serial_SendByte(usartx, 0xFE);
}

uint16_t Serial_GetRxPacketLength(uint8_t usartx)
{
	if(usartx == 1){
		return Serial1_RxLen;
	}else if(usartx == 2){
		return Serial2_RxLen;
	}else if(usartx == 3){
		return Serial3_RxLen;
	}
}

// 清空接收缓冲区
void Serial_ClearRxBuffer(uint8_t usartx)
{
	if(usartx == 1){
		Serial1_RxLen = 0;
		Serial1_RxFlag = 0;          // 接收标志归零
		memset(Serial1_RxPacket, 0, Serial_SizeofRxPacket);  // 清空
	}else if(usartx == 2){
		Serial2_RxLen = 0;
		Serial2_RxFlag = 0;          // 接收标志归零
		memset(Serial2_RxPacket, 0, Serial_SizeofRxPacket);  // 清空
	}else if(usartx == 3){
		Serial3_RxLen = 0;
		Serial3_RxFlag = 0;          // 接收标志归零
		memset(Serial3_RxPacket, 0, Serial_SizeofRxPacket);  // 清空
	}
	
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Serial1_RxData = USART_ReceiveData(USART1);
		Serial1_RxFlag = 1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		Serial2_RxData = USART_ReceiveData(USART2);
		Serial2_RxFlag = 1;
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    { 
        Serial3_RxPacket[Serial3_RxLen++] = USART_ReceiveData(USART3);
		if (Serial3_RxLen >= Serial_SizeofRxPacket - 1){
			Serial3_RxLen = 0;
		}
        
        Serial3_RxFlag = 1;   // 一直标记收到数据
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
