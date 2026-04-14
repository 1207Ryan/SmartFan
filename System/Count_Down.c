#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "Voice_Recognition.h"
#include "Serial.h"
#include "HC_04.h"

uint32_t cnt = 0;
uint8_t Count_Started = 0;

extern volatile uint8_t Working;
extern volatile uint8_t Gear;
extern volatile uint8_t Last_Gear;
extern volatile uint8_t Temp2Gear;
extern volatile float Temp;

void Count_Down_Init(void){		//1s 计数
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_InternalClockConfig(TIM1);
	
	//CK_PSC/（PSC+1）/（ARR+1）
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//1分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;				//ARR 
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;			//PSC 
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
	
	TIM_Cmd(TIM1, DISABLE);
}

void Count_Set_0(void){
	cnt = 0;
}
void Count_Add_1s(void){
	cnt += 1;
}

void Count_Sub_1s(void){
	if(cnt >= 1)
		cnt -= 1;
	else
		cnt = 0;
}

void Count_Add_Second(uint8_t Second){
	cnt += Second;
}

void Count_Add_1m(void){
	cnt += 60;
}

void Count_Sub_1m(void){
	if(cnt >= 60)
		cnt -= 60;
	else
		cnt = 0;
}

void Count_Add_Minute(uint8_t Minute){
	cnt += Minute * 60;
}


void Count_Add_1h(void){
	cnt += 3600;
}

void Count_Sub_1h(void){
	if(cnt >= 3600)
		cnt -= 3600;
	else
		cnt = 0;
}

void Count_Add_Hour(uint8_t Hour){
	cnt += Hour * 3600;
}


void Count_Start(void){
	TIM_Cmd(TIM1, ENABLE);
	Count_Started = 1;
}

void Count_Stop(void){
	TIM_Cmd(TIM1, DISABLE);
	Count_Started = 0;
}

uint32_t Get_Count(void){
	return cnt;
}

void TIM1_UP_IRQHandler(void){
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET){
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		
		if(cnt > 0)
			cnt--;
		
		if(cnt == 0){
			TIM_Cmd(TIM1, DISABLE);
			Motor_Stop();
			Gear = 0;
			Last_Gear = 0;
			Working = 0;
			Temp2Gear = 0;
			Temp = 0;
			Count_Started = 0;
			Count_Down_Over();
		}
		
	}
}

void Count_Down_Over(void)
{
	Serial_TxDataPacket[0] = 0x2E;
	Serial_SendPacket(1, 1); 
	Serial_SendString(2, "倒计时已结束");
	Serial_SendByte(2, '\n');
}
