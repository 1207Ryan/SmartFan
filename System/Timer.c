#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"
#include "Menu.h"
#include "AD.h"
#include "HC_SR04.h"
#include "Motor.h"

extern volatile uint8_t Gear;
extern volatile uint8_t Temp2Gear;
extern volatile float Temp;
volatile uint8_t IsSafe = 1;
float current_dist;
uint8_t volatile Last_Gear;

void Timer_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_InternalClockConfig(TIM3);
	
	//CK_PSC/（PSC+1）/（ARR+1）
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//1分频，定时器时钟 = 72MHz
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;				//ARR 定时器数到 999 就 “溢出”（更新），触发中断
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;			//PSC 把 72MHz 时钟分频：72MHz ÷ PSC = （每 .. μs 数 1 个数）
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM3, DISABLE);
}

/**
  * @brief  按温度自动匹配档位并控制电机
  * @retval 无
  */
void Temp_Match_Gear(void){
	if(Temp2Gear && IsSafe){
		// 根据当前温度匹配档位
		if(Temp >= TEMP_GEAR_5){
			Gear = 5;
		}else if(Temp >= TEMP_GEAR_4){
			Gear = 4;
		}else if(Temp >= TEMP_GEAR_3){
			Gear = 3;
		}else if(Temp >= TEMP_GEAR_2){
			Gear = 2;
		}else if(Temp >= TEMP_GEAR_1){
			Gear = 1;
		}else{
			Gear = 0;
		}
		
		if(Gear != Last_Gear){
			if(Gear == 1) 
				Motor_SetSpeed(MOTOR_SPEED_1);
			else if(Gear == 2) 
				Motor_SetSpeed(MOTOR_SPEED_2);
			else if(Gear == 3) 
				Motor_SetSpeed(MOTOR_SPEED_3);
			else if(Gear == 4) 
				Motor_SetSpeed(MOTOR_SPEED_4);
			else if(Gear == 5) 
				Motor_SetSpeed(MOTOR_SPEED_5);
			else Motor_SetSpeed(0);

			Last_Gear = Gear; // 更新旧档位
		}
	}else{
		Motor_SetSpeed(MOTOR_SPEED_0);
	}
}

void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		static uint16_t refresh_tick = 0; // 刷新计时，控制温度更新频率
		refresh_tick++;
		
		// 防溢出：累计到1000ms（1秒）重置，覆盖500/200ms的最小公倍数
		if(refresh_tick >= 1000){
			refresh_tick = 0;
		}
		
		if(Temp2Gear && refresh_tick % 500 == 0){
			//500ms刷新一次温度，同时变换档位
			Temp = AD_to_Temp(); // 更新平滑后的温度值
			Temp_Match_Gear();
		}
		
		if(refresh_tick % 200 == 0){
			//200ms测量一次距离
			current_dist = HC_SR04_GetDistance();
			if(current_dist < SATE_DISTANCE){
				IsSafe = 0;
				if(Gear > 0){
					Last_Gear = Gear;
					Gear = 0;
					Motor_SetSpeed(MOTOR_SPEED_0);
				}
			}else{
				IsSafe = 1;
				if(Gear == 0){
					Gear = Last_Gear;
					Last_Gear = 0;
					Motor_SetSpeed(Gear * 20);
				}
			}
			
		}
		
	}
}


