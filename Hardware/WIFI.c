#include "stm32f10x.h"
#include "ESP8266.h"
#include "Delay.h"
#include "MyRTC.h"
#include <stdio.h>
#include <string.h>

/*
ESP8266 Tx - Rx PB11
ESP8266 Rx - Tx PB10
*/

long long int time;
int code;
int temperature;
uint8_t weather_temp_u8;
char climate[10];
void Weather_Parse(void);

void WIFI_Init(void)
{
		Serial3_Init();
		Delay_ms(1000);
	
		Serial3_ClearRxBuffer();
		Serial3_SendString("+++");//退出透传模式
	
		Delay_ms(1000);
	
		Serial3_ClearRxBuffer();
		Serial3_SendString("AT\r\n");//AT测试
	
		Delay_ms(2000);
		
		if (strstr(Serial3_RxPacket,"OK\r\n") == NULL){//模块异常
			return;
		}
		
		Serial3_ClearRxBuffer();
		Serial3_SendString("AT+RST\r\n");//模块复位
		Delay_ms(1500);
		
		Serial3_ClearRxBuffer();
		Serial3_SendString("AT+CWMODE=1\r\n");//开始连接wifi
		Delay_ms(1000);
		
		Serial3_ClearRxBuffer();
		Serial3_SendString("AT+CWJAP=\"Ryan\",\"ryh031207\"\r\n");//正在连接wifi...
		
		// 轮询检测WiFi连接状态
		uint8_t conn_flag = 0;
		uint8_t ip_flag = 0;
		unsigned int start_time = 0;
			
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
			return;
		}
		
		Serial3_ClearRxBuffer();
		Serial3_SendString("AT+CIPMUX=0\r\n");
		Serial3_Printf("开启单连接模式\r\n");
		
		Delay_ms(800);
		if (strstr(Serial3_RxPacket,"OK\r\n") == NULL){
			//单连接模式异常
			return;
		}
		Delay_ms(1000);
}

// 获取网络时间（拼多多API，老固件可用）
void WIFI_GetTime(void)
{
	Serial3_ClearRxBuffer();
	Serial3_SendString("AT+CIPSTART=\"TCP\",\"api.pinduoduo.com\",80\r\n");
	
	Delay_ms(1000);
	
	Serial3_ClearRxBuffer();
	Serial3_SendString("AT+CIPMODE=1\r\n");
	Delay_ms(800);
	
	Serial3_ClearRxBuffer();
	Serial3_SendString("AT+CIPSEND\r\n");
	Delay_ms(800);
	
	Serial3_SendString("GET http://api.pinduoduo.com/api/server/_stm\r\n");
	Delay_ms(1000);
	
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
	
	Serial3_SendString("+++");
	Delay_ms(1000);
	
	Serial3_ClearRxBuffer();
	Serial3_SendString("AT+CIPCLOSE\r\n");
	Delay_ms(1000);
}

// 获取福州天气（心知天气）
void WIFI_GetWeather(void)
{
	Serial3_ClearRxBuffer();
	Serial3_SendString("AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n");
	Delay_ms(1000);
	
	Serial3_ClearRxBuffer();
	Serial3_SendString("AT+CIPMODE=1\r\n");
	Delay_ms(800);
	
	Serial3_ClearRxBuffer();
	Serial3_SendString("AT+CIPSEND\r\n");//3
	Delay_ms(800);
	
	// 福州正确请求（无https + 正确格式）
	Serial3_ClearRxBuffer();
	Serial3_SendString("GET https://api.seniverse.com/v3/weather/now.json?key=SEyv448ckBXofHyY9&location=fujian fuzhou&language=en&unit=c\r\n");
	Delay_ms(1200);
	
	char *p = NULL;
	uint8_t time_out = 0;
	uint8_t flag1 = 0;
	uint8_t flag2 = 0;
	
	while (time_out < 50)
	{
		// 天气代码
		p=strstr (Serial3_RxPacket,"code");
		if (p!=NULL)
		{
			sscanf (p+7,"%d",&code);
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
	
	Serial3_SendString("+++");
	Delay_ms(1000);
	
	Serial3_ClearRxBuffer();
	Serial3_SendString("AT+CIPCLOSE\r\n");
	Delay_ms(1000);
}

void Weather_Parse(void)
{
	switch (code)
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
