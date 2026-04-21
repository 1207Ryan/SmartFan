#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "Motor.h"

/*
PWMA - PA7
AIN2 - PB0
AIN1 - PB1
*/

void Motor_Init(void){
	PWM_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_AIN1 | GPIO_PIN_AIN2;
 	GPIO_Init(GPIO_PORT_AIN, &GPIO_InitStructure);
}

void Motor_SetSpeed(int8_t Speed){
	if(Speed >= 0){
		GPIO_SetBits(GPIO_PORT_AIN, GPIO_PIN_AIN1);
		GPIO_ResetBits(GPIO_PORT_AIN, GPIO_PIN_AIN2);
		PWM_SetCompare2(Speed);
	}else{
		GPIO_SetBits(GPIO_PORT_AIN,GPIO_PIN_AIN2);
		GPIO_ResetBits(GPIO_PORT_AIN,GPIO_PIN_AIN1);
		PWM_SetCompare2(-Speed);
	}
}

void Motor_SetGear(uint8_t Gear_x){
	Motor_SetSpeed(Motor_Speed[Gear_x]);
}

void Motor_Stop(void){
	GPIO_ResetBits(GPIO_PORT_AIN, GPIO_PIN_AIN1);
	GPIO_ResetBits(GPIO_PORT_AIN, GPIO_PIN_AIN2);
	PWM_SetCompare2(0);
}

void Motor_Speed_Add(uint8_t Gear_x){
	Motor_Speed[Gear_x] += 1;
}

void Motor_Speed_Sub(uint8_t Gear_x){
	Motor_Speed[Gear_x] -= 1;
}

void Motor_Speed_Set(uint8_t Gear_x, uint8_t Speed){
	Motor_Speed[Gear_x] = Speed;
}
