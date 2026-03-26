#include "stm32f10x.h"                  // Device header
#include "PWM.h"

/*
PWMA - PA0
AIN1 - PB0
AIN2 - PB1
*/
#define GPIO_Pin_AIN1 GPIO_Pin_0
#define GPIO_Pin_AIN2 GPIO_Pin_1
#define GPIO_Pin_Port GPIOB

void Motor_Init(void){
	PWM_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_AIN1 | GPIO_Pin_AIN2;
 	GPIO_Init(GPIO_Pin_Port, &GPIO_InitStructure);

}

void Motor_SetSpeed(int8_t Speed){
	if(Speed >= 0){
		GPIO_SetBits(GPIO_Pin_Port, GPIO_Pin_AIN1);
		GPIO_ResetBits(GPIO_Pin_Port, GPIO_Pin_AIN2);
		PWM_SetCompare1(Speed);
	}else{
		GPIO_SetBits(GPIOA,GPIO_Pin_AIN2);
		GPIO_ResetBits(GPIOA,GPIO_Pin_AIN1);
		PWM_SetCompare1(-Speed);
	}
}

void Motor_SetGear(uint8_t Gear){
	Motor_SetSpeed(Gear * 20);
}

void Motor_Stop(void){
	GPIO_SetBits(GPIO_Pin_Port, GPIO_Pin_AIN1);
	GPIO_SetBits(GPIO_Pin_Port, GPIO_Pin_AIN2);
	PWM_SetCompare1(0);
}
