#include "stm32f10x.h"
#include "Serial.h"
#include "ESP8266.h"
#include "Delay.h"
#include "MyRTC.h"
#include <stdio.h>
#include <string.h>
#include "OLED.h"

/*
ESP8266 Tx - Rx PB11
ESP8266 Rx - Tx PB10
*/

long long int time;
uint8_t weather_code;
int temperature;
uint8_t weather_temp_u8;
char climate[10];
void Weather_Parse(void);

void ESP8266_Init(void)
{
	OLED_ClearArea(0, 32, 128, 32);
	OLED_Update();
	
	Serial_Init(3);//模块初始化
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "模块初始化...", OLED_8X16);
	Delay_ms(1000);
	OLED_DrawBar(0,48, 128, 16, 10);
	OLED_Update();

	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "+++");//退出透传模式
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "退出透传...", OLED_8X16);
	Delay_ms(1000);
	OLED_DrawBar(0,48, 128, 16, 20);
	OLED_Update();

	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT\r\n");//AT测试
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "AT测试中...", OLED_8X16);
	Delay_ms(2000);
	if (strstr(Serial3_RxPacket,"OK\r\n") == NULL){//模块异常
		return;
	}
	OLED_DrawBar(0,48, 128, 16, 30);
	OLED_Update();

	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+RST\r\n");//模块复位
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "模块复位中...", OLED_8X16);
	Delay_ms(1500);
	OLED_DrawBar(0,48, 128, 16, 40);
	OLED_Update();

	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CWMODE=1\r\n");//开始连接wifi
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "设置STA模式...", OLED_8X16);
	Delay_ms(1000);
	OLED_DrawBar(0,48, 128, 16, 50);
	OLED_Update();

	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CWJAP=\"Ryan\",\"ryh031207\"\r\n");//正在连接wifi...
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "正在连接wifi...", OLED_8X16);
	
	// 轮询检测WiFi连接状态
	uint8_t conn_flag = 0;
	uint8_t ip_flag = 0;
	uint16_t start_time = 0;
		
	while(start_time < 10000)
	{
		if(strstr(Serial3_RxPacket, "WIFI CONNECTED") != NULL) 
			conn_flag = 1;
		if(strstr(Serial3_RxPacket, "WIFI GOT IP") != NULL) 
			ip_flag = 1;
		if(conn_flag && ip_flag) {	// 两个标识都出现，提前退出轮询
			break;
		}
		Delay_ms(10);
		start_time += 10;
	}
	
	if(!conn_flag || !ip_flag){
		//WiFi连接失败，超时/参数错误
		OLED_ClearArea(0, 32, 128, 32);
		OLED_ShowString(0, 32, "WiFi连接失败", OLED_8X16);
		return;
	}
	OLED_DrawBar(0,48, 128, 16, 80);
	OLED_Update();

	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPMUX=0\r\n");//开启单连接模式
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "开启单连接模式…", OLED_8X16);
	Delay_ms(800);
	if (strstr(Serial3_RxPacket,"OK\r\n") == NULL){
		//单连接模式异常
		return;
	}
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "初始化完成", OLED_8X16);
	OLED_DrawBar(0,48, 128, 16, 100);
	OLED_Update();
}

// 获取网络时间（拼多多API，老固件可用）
void ESP8266_GetTime(void)
{
	OLED_ClearArea(0, 48, 128, 16);
	OLED_Update();
	
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPSTART=\"TCP\",\"api.pinduoduo.com\",80\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "连接时间服务器……", OLED_8X16);
	Delay_ms(1000);
	OLED_DrawBar(0,48, 128, 16, 14);
	OLED_Update();
	
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPMODE=1\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "进入透传中……", OLED_8X16);
	Delay_ms(800);
	OLED_DrawBar(0,48, 128, 16, 28);
	OLED_Update();
	
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPSEND\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "发送请求中……", OLED_8X16);
	Delay_ms(800);
	OLED_DrawBar(0,48, 128, 16, 42);
	OLED_Update();
	
	Serial_SendString(3, "GET http://api.pinduoduo.com/api/server/_stm\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "获取时间中……", OLED_8X16);
	Delay_ms(1000);
	OLED_DrawBar(0,48, 128, 16, 56);
	OLED_Update();
	
	char *p = NULL;
	uint8_t time_out = 0;
	
	while (time_out < 20)
	{
		p = strstr(Serial3_RxPacket, "server_time");
		if (p != NULL)
		{
			sscanf(p + 13, "%lld", &time);
			RTC_SetCounter(time / 1000);
			RTC_WaitForLastTask();
			break;
		}
		
		Delay_ms(100);
		time_out++;
	}
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "时间同步成功", OLED_8X16);
	OLED_DrawBar(0,48, 128, 16, 72);
	OLED_Update();
	
	Serial_SendString(3, "+++");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "退出透传中……", OLED_8X16);
	Delay_ms(1000);
	OLED_DrawBar(0,48, 128, 16, 86);
	OLED_Update();
	
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPCLOSE\r\n");
	Delay_ms(1000);
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "时间同步完成", OLED_8X16);
	OLED_DrawBar(0,48, 128, 16, 100);
	OLED_Update();
}

// 获取福州天气（心知天气）
void ESP8266_GetWeather(void)
{
	OLED_ClearArea(0, 48, 128, 16);
	OLED_Update();
	
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "连接天气服务器…", OLED_8X16);
	Delay_ms(1000);
	OLED_DrawBar(0,48, 128, 16, 14);
	OLED_Update();
	
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPMODE=1\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "进入透传中……", OLED_8X16);
	Delay_ms(800);
	OLED_DrawBar(0,48, 128, 16, 28);
	OLED_Update();
	
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPSEND\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "发送请求中……", OLED_8X16);
	Delay_ms(800);
	OLED_DrawBar(0,48, 128, 16, 42);
	OLED_Update();
	
	// 福州正确请求（无https + 正确格式）
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "GET https://api.seniverse.com/v3/weather/now.json?key=SEyv448ckBXofHyY9&location=fujian fuzhou&language=en&unit=c\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "获取天气中……", OLED_8X16);
	Delay_ms(1200);
	OLED_DrawBar(0,48, 128, 16, 56);
	OLED_Update();
	
	char *p = NULL;
	int c;
	uint8_t time_out = 0;
	uint8_t flag1 = 0;
	uint8_t flag2 = 0;
	
	while (time_out < 50){
		// 天气代码
		p=strstr (Serial3_RxPacket,"code");
		if (p!=NULL)
		{
			sscanf (p+7,"%d",&c);
			weather_code = (uint8_t)c;
			Weather_Parse();
			flag1 = 1;
		}

		// 温度（uint8_t）
		p=strstr (Serial3_RxPacket,"temperature");
		if (p!=NULL)
		{
			sscanf (p+14,"%d",&temperature);
			weather_temp_u8 = (uint8_t)temperature;
			flag2 = 1;
		}
		
		if(flag1 && flag2)
			break;
		
		Delay_ms(100);
		time_out++;
	}
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "天气获取成功", OLED_8X16);
	OLED_DrawBar(0,48, 128, 16, 72);
	OLED_Update();
	
	Serial_SendString(3, "+++");
	Delay_ms(1000);
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "退出透传中……", OLED_8X16);
	OLED_DrawBar(0,48, 128, 16, 86);
	OLED_Update();
	
	Serial_ClearRxBuffer(3);
	Serial_SendString(3, "AT+CIPCLOSE\r\n");
	OLED_ClearArea(0, 32, 128, 32);
	OLED_ShowString(0, 32, "天气获取完成", OLED_8X16);
	OLED_DrawBar(0,48, 128, 16, 100);
	OLED_Update();
}

void Weather_Parse(void)
{
	switch (weather_code)
	{
		case 0:strcpy (climate,"晴");break;
		case 1:strcpy (climate,"晴");break;
		case 2:strcpy (climate,"晴");break;
		case 3:strcpy (climate,"晴");break;
		case 4:strcpy (climate,"多云");break;
		case 5:strcpy (climate,"多云");break;
		case 6:strcpy (climate,"多云");break;
		case 7:strcpy (climate,"多云");break;
		case 8:strcpy (climate,"多云");break;
		case 9:strcpy (climate,"阴");break;
		case 10:strcpy (climate,"阵雨");break;
		case 11:strcpy (climate,"雷阵雨");break;
		case 12:strcpy (climate,"雷阵雨");break;
		case 13:strcpy (climate,"小雨");break;
		case 14:strcpy (climate,"中雨");break;
		case 15:strcpy (climate,"大雨");break;
		case 16:strcpy (climate,"暴雨");break;
		case 17:strcpy (climate,"大暴雨");break;
		case 18:strcpy (climate,"大暴雨");break;
		case 19:strcpy (climate,"冻雨");break;
		case 20:strcpy (climate,"雨夹雪");break;
		case 21:strcpy (climate,"阵雪");break;
		case 22:strcpy (climate,"小雪");break;
		case 23:strcpy (climate,"中雪");break;
		case 24:strcpy (climate,"大雪");break;
		case 25:strcpy (climate,"暴雪");break;
		case 30:strcpy (climate,"雾");break;
		case 31:strcpy (climate,"霾");break;
		case 32:strcpy (climate,"风");break;
		case 33:strcpy (climate,"大风");break;
		case 37:strcpy (climate,"冷");break;
		case 38:strcpy (climate,"热");break;
	}
}
