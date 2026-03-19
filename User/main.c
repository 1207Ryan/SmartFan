#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "Menu.h"
#include "AD.h"
#include "Timer.h"
#include "MyRTC.h"
#include "HC_SR04.h"

int main(void)
{
	AD_Init();
	OLED_Init(); 
	OLED_Update();
	Motor_Init();
	Key_Init();
	Timer_Init();
	MyRTC_Init();
	HC_SR04_Init();
	
	uint8_t Menu1_Select;
	
	while (1)
	{
		Menu1_Select = Menu1();
		switch(Menu1_Select){
			case 1:
				Menu2_Temp();
				break;
			case 2:
				Menu2_Fan();
				break;
			case 3:
				Menu2_CountDown();
				break;
			case 4:
				Menu2_Clock();
				break;
			case 5:
				Menu2_Debug();
				break;
			
		}
		
	}
}


