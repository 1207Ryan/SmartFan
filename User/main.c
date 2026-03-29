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
#include "HC_04.h"
#include "ESP8266.h"

int main(void)
{
	OLED_Init(); 
	OLED_ShowString(0, 0, "正在努力加载中…", OLED_8X16);
	OLED_Update();
	ESP8266_Init();
	MyRTC_Init();
	AD_Init();
	Motor_Init();
	Key_Init();
	Timer_Init();
	HC_SR04_Init();
	Count_Down_Init();
	Voice_Recognition_Init();
	HC_04_Init();
	
	ESP8266_GetTime();
	
	OLED_Clear();
	OLED_Update();
	
	while (1)
	{
		Menu1();
	}
}


