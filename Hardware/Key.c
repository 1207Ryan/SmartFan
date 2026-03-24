#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "Delay.h"

/*
PB12 - 上一项
PB13 - 下一项
PB15 - 确认
*/
#define GPIO_Pin_Prev GPIO_Pin_12
#define GPIO_Pin_Next GPIO_Pin_13
#define GPIO_Pin_Confirm GPIO_Pin_15

uint8_t Key_Flag[KEY_COUNT];
//6=REPEAT长按后重复		5=LONG长按	4=DOUBLE双击		3=SINGLE单击
//2=UP松开瞬间	1=DOWN按下瞬间	0=HOLD按住不放

void Key_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Prev | GPIO_Pin_Next | GPIO_Pin_Confirm;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_GetState(uint8_t n){
	if(n == KEY_1){
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_Prev) == 0){
			return KEY_PRESSED;
		}
	}else if(n == KEY_2){
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_Next) == 0){
			return KEY_PRESSED;
		}
	}else if(n == KEY_3){
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_Confirm) == 0){
			return KEY_PRESSED;
		}
	}
	return KEY_UNPRESSED;
}


uint8_t Key_Check(uint8_t n, uint8_t Flag){
	if(Flag & Key_Flag[n]){
		if(Flag != KEY_HOLD){
			Key_Flag[n] &= ~Flag;
		}
		return 1;
	}
	return 0;
}

uint8_t Key_PressThenPress(uint8_t k1, uint8_t k2){//先按k1,再按k2
	if(KEY_PRESSED & Key_Flag[k1]){
		if(KEY_PRESSED & Key_Flag[k2]){
			Key_Flag[k1] &= ~KEY_PRESSED;
			Key_Flag[k2] &= ~KEY_PRESSED;
			return 1;
		}
	}
	return 0;
}

void Key_ClearFlag(void){
	for(uint8_t i =0;i<KEY_COUNT;i++){
		Key_Flag[i] = 0;
	}
}

void Key_Tick(){
	static uint8_t Count;
	static uint8_t PrevState[KEY_COUNT], CurrState[KEY_COUNT];
	static uint8_t S[KEY_COUNT];
	static uint16_t Timer[KEY_COUNT];
	
	for(int i = 0;i < KEY_COUNT; i++){
		if(Timer[i] >0){
			Timer[i]--;
		}
	}
	
	Count++;
	if(Count == 20){
		Count = 0;
		for(uint8_t i = 0;i<KEY_COUNT; i++){
			PrevState[i] = CurrState[i];
			CurrState[i] = Key_GetState(i);
			
	//		if(CurrState == KEY_PRESSED){
	//			//HOLD=1
	//			Key_Flag |= KEY_HOLD;
	//		}else{
	//			//HOLD=0
	//			Key_Flag &= ~KEY_HOLD;//&=0xFE
	//		}
	//		
	//		if(CurrState == KEY_PRESSED && PrevState == KEY_UNPRESSED){
	//			//KEY_DOWN
	//			Key_Flag |= KEY_DOWN;
	//		}
	//		
	//		if(CurrState == KEY_UNPRESSED && PrevState == KEY_PRESSED){
	//			//KEY_UP
	//			Key_Flag |= KEY_UP;
	//		}
			
			switch(S[i]){
				case 0://按键空闲
					if(CurrState[i] == KEY_PRESSED){//按键按下
						Timer[i] = LONG_TIME;//长按计时
						//HOLD=1
						Key_Flag[i] |= KEY_HOLD;
						S[i] = 1;
					}
					
					if(CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED){
						//KEY_DOWN
						Key_Flag[i] |= KEY_DOWN;
					}

					break;
				case 1://按键按下
					if(CurrState[i] == KEY_UNPRESSED){//按键松开
						Timer[i] = DOUBLE_TIME;
						//HOLD=0
						Key_Flag[i] &= ~KEY_HOLD;//&=0xFE
						S[i] = 2;
					}else if(Timer[i] == 0){//长按计时到
						Key_Flag[i] |= KEY_LONG;
						S[i] = 4;
					}
					
					if(CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED){
						//KEY_UP
						Key_Flag[i] |= KEY_UP;
					}

					break;
				case 2://按键松开
					if(Timer[i] == 0){//单击
						Key_Flag[i] |= KEY_SINGLE;
						S[i] = 0;
					}else if(CurrState[i] == KEY_PRESSED){
						Key_Flag[i] |= KEY_DOUBLE;
						//HOLD=1
						Key_Flag[i] |= KEY_HOLD;
						S[i] = 3;
					}
					
					if(CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED){
						//KEY_DOWN
						Key_Flag[i] |= KEY_DOWN;
					}

					break;
				case 3://按键已双击
					if(CurrState[i] == KEY_UNPRESSED){
						//HOLD=0
						Key_Flag[i] &= ~KEY_HOLD;//&=0xFE
						S[i] = 0;
					}
					
					if(CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED){
						//KEY_UP
						Key_Flag[i] |= KEY_UP;
					}

					break;				
				case 4://按键已长按
					if(CurrState[i] == KEY_UNPRESSED){
						//HOLD=0
						Key_Flag[i] &= ~KEY_HOLD;//&=0xFE
						S[i] = 0;
					}else if(Timer[i] == 0){
						Timer[i] = REPEAT_TIME;
						Key_Flag[i] |= KEY_REPEAT;
					}
					
					if(CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED){
						//KEY_UP
						Key_Flag[i] |= KEY_UP;
					}

					break;
			}
		}
	}
}



