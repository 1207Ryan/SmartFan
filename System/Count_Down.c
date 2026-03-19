#include "stm32f10x.h"                  // Device header

uint32_t cnt = 0;

void Count_Down_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_InternalClockConfig(TIM1);
	
	//CK_PSC/（PSC+1）/（ARR+1）
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//1分频，定时器时钟 = 72MHz
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;				//ARR 定时器数到 999 就 “溢出”（更新），触发中断
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72000 - 1;			//PSC 把 72MHz 时钟分频：72MHz ÷ PSC = （每 .. μs 数 1 个数）
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM1, ENABLE);
}

void TIM1_IRQHandler(void){
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET){
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		
		
		
	}
}


