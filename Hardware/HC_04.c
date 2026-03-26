#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "stdarg.h"
#include "HC_04.h"

/*
HC-04 Tx - Rx PA3
HC-04 Rx - Tx PA2
*/

uint8_t Serial2_RxData;
uint8_t Serial2_RxFlag;

void Serial2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
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
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
}

void Serial2_SendByte(uint8_t Byte)//发送字节数据
{
	USART_SendData(USART2, Byte);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	
	//判断TXE是否置1，即数据是否从发送数据寄存器转移到发送移位寄存器
	//标志寄存器会自动清零
	
}

void Serial2_SendArray(uint8_t *Array, uint16_t length)
{
	for(uint8_t i=0; i<length; i++)
	{
		Serial2_SendByte(Array[i]);
	}
}

void Serial2_SendString(char* String)
{
	for(uint8_t i=0; String[i] != '\0'; i++)
	{
		Serial2_SendByte(String[i]);
	}
}

void Serial2_SendNumber(uint32_t Number)
{
	uint32_t mask =1;//位数
	uint32_t tem =Number;
	for(; tem>9; mask*=10)
	{
		tem /=10;
	}
	
	for(; Number!=0; Number%=mask,mask/=10)
	{
		Serial2_SendByte(Number/mask+'0');//从高位开始发
	}
}

int fputc2(int ch, FILE *f)
{
	Serial2_SendByte(ch);
	return ch;
}

void Serial2_Printf(char *format, ...)//可变参数
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);//释放参数表
	Serial2_SendString(String);
}

uint8_t Serial2_GetRxFlag(void)
{
	if(Serial2_RxFlag == 1)
	{
		Serial2_RxFlag =0;
		return 1;
	}
	return 0;
}

uint8_t Serial2_GetRxData(void)
{
	return Serial2_RxData;
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
