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
#include "Count_Down.h"
#include "Voice_Recognition.h"
#include "Bluetooth.h"

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
	Count_Down_Init();
	Voice_Recognition_Init();
	Bluetooth_Init();
	
	while (1)
	{
		Menu1();
	}
}


