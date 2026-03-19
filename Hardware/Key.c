#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/*
	按键功能
	B11	上
	B1	下
	A0	确认
*/

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == RESET)
    {
        Delay_ms(20);  // 20ms消抖（过滤机械抖动）
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == RESET)
        {
            KeyNum = 1;
            // 等待按键释放（避免长按重复加档）
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == RESET);
        }
    }
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == RESET)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == RESET)
        {
            KeyNum = 2;
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == RESET);
        }
    }
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == RESET)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == RESET)
        {
            KeyNum = 3;
            while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == RESET);
        }
    }
	
	return KeyNum;
}
