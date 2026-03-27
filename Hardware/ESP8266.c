#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "stdarg.h"
#include "ESP8266.h"
#include <string.h>
#include "MyRTC.h"

/*
ESP8266 Tx - Rx PB11
ESP8266 Rx - Tx PB10
*/

uint8_t Serial3_TxDataPacket[Serial3_SizeofTxPacket];
uint8_t Serial3_RxDataPacket[Serial3_SizeofRxPacket];
char Serial3_TxPacket[Serial3_SizeofTxPacket];
char Serial3_RxPacket[Serial3_SizeofRxPacket];
uint8_t Serial3_RxData;
uint8_t Serial3_RxFlag;
uint8_t Serial3_PassFlag =0;
static uint16_t RxNumber = 0;
uint16_t Serial3_RxLen = 0;

void Serial3_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//TX
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin = TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TX_PORT, &GPIO_InitStructure);
	
	//RX
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = RX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RX_PORT, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStructure);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART3, ENABLE);
}

void Serial3_SendByte(uint8_t Byte)//发送字节数据
{
	USART_SendData(USART3, Byte);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	
	//判断TXE是否置1，即数据是否从发送数据寄存器转移到发送移位寄存器
	//标志寄存器会自动清零
	
}

void Serial3_SendArray(uint8_t *Array, uint16_t length)
{
	for(uint8_t i=0; i<length; i++)
	{
		Serial3_SendByte(Array[i]);
	}
}

void Serial3_SendString(char* String)
{
	for(uint16_t i=0; String[i] != '\0'; i++)
	{
		Serial3_SendByte(String[i]);
	}
}

void Serial3_SendNumber(uint32_t Number)
{
	uint32_t mask =1;//位数
	uint32_t tem =Number;
	for(; tem>9; mask*=10)
	{
		tem /=10;
	}
	
	for(; Number!=0; Number%=mask,mask/=10)
	{
		Serial3_SendByte(Number/mask+'0');//从高位开始发
	}
}

int fputc3(int ch, FILE *f)
{
	Serial3_SendByte(ch);
	return ch;
}

void Serial3_Printf(char *format, ...)//可变参数
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);//释放参数表
	Serial3_SendString(String);
}

uint8_t Serial3_GetRxFlag(void)
{
	if(Serial3_RxFlag == 1)
	{
		Serial3_RxFlag =0;
		return 1;
	}
	return 0;
}

uint8_t Serial3_GetRxData(void)
{
	return Serial3_RxData;
}

void Serial3_SendPacket(void)
{
	Serial3_SendByte(0xFF);
	Serial3_SendArray(Serial3_TxDataPacket, Serial3_SizeofTxPacket);
	Serial3_SendByte(0xFE);
}

uint8_t Serial3_GetRxPacketLength(void)
{
	return RxNumber;
}

// 获取接收长度
uint16_t Serial3_GetRxLength(void)
{
    return Serial3_RxLen;
}

// 清空接收缓冲区
void Serial3_ClearRxBuffer(void)
{
	RxNumber = 0;
    Serial3_RxLen = 0;           // 接收长度归零
    Serial3_RxFlag = 0;          // 接收标志归零
    memset(Serial3_RxPacket, 0, Serial3_SizeofRxPacket);  // 清空
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    { 
        Serial3_RxPacket[RxNumber++] = USART_ReceiveData(USART3);
		if (RxNumber >= Serial3_SizeofRxPacket - 1){
			RxNumber = 0;
		}
        
        Serial3_RxFlag = 1;   // 一直标记收到数据
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
