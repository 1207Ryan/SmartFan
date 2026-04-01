#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"
#include "Menu.h"
#include "AD.h"
#include "HC_SR04.h"
#include "Motor.h"
#include "Voice_Recognition.h"
#include "Serial.h"
#include "HC_04.h"
#include "MyRTC.h"
#include <stdio.h>

#define WARN_INTERVAL 5    // 警告间隔：5秒
uint32_t last_warn_sec = 0;  // 上次发送警告的系统时间（s）
uint32_t now_sec;

extern char Str[30];
extern volatile uint8_t Working;
extern volatile uint8_t Gear;
extern volatile uint8_t Last_Gear;
extern volatile uint8_t Temp2Gear;
extern volatile float Temp;
extern uint8_t temp_int;
extern uint8_t temp_dec;
volatile uint8_t IsSafe = 1;
float current_dist;

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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  按温度自动匹配档位并控制电机
  * @retval 无
  */
void Temp_Match_Gear(void){
	if(Temp2Gear && IsSafe){
		// 根据当前温度匹配档位
		if(Temp >= Temp_Gear_5){
			Gear = 5;
		}else if(Temp >= Temp_Gear_4){
			Gear = 4;
		}else if(Temp >= Temp_Gear_3){
			Gear = 3;
		}else if(Temp >= Temp_Gear_2){
			Gear = 2;
		}else if(Temp >= Temp_Gear_1){
			Gear = 1;
		}else{
			Gear = 0;
		}
		
		if(Gear != Last_Gear){
			Motor_SetGear(Gear);
			Last_Gear = Gear; // 更新旧档位
			
			sprintf(Str, "当前档位：%d档", Gear);
			Serial_SendString(2, Str);
			Serial_SendByte(2, '\n');
		}
	}else{
		Motor_Stop();
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
			// 1. 提取整数位（直接强制类型转换）
			temp_int = (uint8_t)Temp; // 25.6 → 25
			// 2. 提取小数位（放大10倍后取余，保留1位）
			temp_dec = (uint8_t)((Temp - temp_int) * 10); // 25.6-25=0.6 → 0.6×10=6
			
			Serial_SendByte(1, 0x03);
			Delay_ms(1);  
			Serial_SendByte(1, temp_int);  // 第一次发送：整数位（25 → 0x19）
			Delay_ms(1);                // 短暂延时，避免接收端粘包（可选）
			Serial_SendByte(1, temp_dec);  // 第二次发送：小数位（6 → 0x06）
		
			// 合成一条完整字符串：当前温度：25.6℃
			sprintf(Str, "当前温度：%d.%d℃", temp_int, temp_dec);
			Serial_SendString(2, Str);
			Serial_SendByte(2, '\n');
			Temp_Match_Gear();
		}
		
		if(Working && refresh_tick % 200 == 0){
			//200ms测量一次距离
			current_dist = HC_SR04_GetDistance();
			if(current_dist < SATE_DISTANCE){
				IsSafe = 0;
				
				now_sec = MyRTC_GetCurrentSec(); // 获取当前秒数
				// 条件1：首次触发警告  条件2：距离过近且已过5秒
				if((last_warn_sec == 0) || (now_sec  - last_warn_sec >= WARN_INTERVAL)){
					Distance_Warn();              // 发送警告
					last_warn_sec = now_sec ; // 更新上次发送时间
				}
				
				if(Gear > 0){
					Last_Gear = Gear;
					Gear = 0;
					Motor_Stop();
				}
			}else{
				IsSafe = 1;
				if(Gear == 0){
					Gear = Last_Gear;
					Last_Gear = 0;
					Motor_SetGear(Gear);
				}
			}
		}
		
		Key_Tick();
		
		if(refresh_tick %10 == 0){
			// 10ms
			Voice_Recognition();
			HC_04_Detect();
		}
	}
}
