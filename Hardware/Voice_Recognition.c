#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "Delay.h"
#include "Menu.h"
#include "Motor.h"
#include "MyRTC.h"
#include "AD.h"
#include "HC_SR04.h"
#include "Count_Down.h"
#include "Timer.h"
#include <string.h> 
#include "ESP8266.h"

/*
STM32 PA9 - ASRPRO PB6/PA3
STM32 PA10 - ASRPRO PB5/PA2
0x01:开启温度调节	0x02:关闭温度调节	0x03:报温度
0x11:升档	0x12:降档	0x13:报档位	0x14:距离过近提示
0x2C:设置倒计时	依次说 小时：分钟：秒钟 每个块对应一个时间	0x2D:开始倒计时
0x20 ~ 0x2B 对应0 ~ 55
0x31:报时	0x32:联网校准时间		0x33:播报天气
*/

uint8_t RxData;
uint8_t temp_int;
uint8_t temp_dec;
extern volatile uint8_t Working;
extern volatile uint8_t Gear;
extern volatile uint8_t Last_Gear;
extern volatile uint8_t Temp2Gear;
extern volatile float Temp;
extern uint8_t Count_Started;
uint8_t Count_State = 9;
uint8_t High_Byte;
uint8_t Low_Byte;

void Voice_Recognition_Init(void){
	Serial_Init(1);
}

void Voice_Recognition(void){
	if(Serial_GetRxFlag(1) == 1)
	{
		RxData =Serial_GetRxData(1);
		//Serial_SendByte(RxData);
		switch(RxData){
			case 0x01:	//开启温度调节
				AD_Collect_Start();
				Working = 1;
				Temp2Gear = 1;
				Last_Gear = 0;
				break;
			case 0x02:	//关闭温度调节
				AD_Collect_Stop();
				Working = 0;
				Temp = 0;
				Temp2Gear = 0;
				Gear = 0;
				Last_Gear = 0;
				Motor_Stop();
				break;
			case 0x03:	//报温度
				Serial_SendByte(1, RxData);
				// 1. 提取整数位（直接强制类型转换）
				temp_int = (uint8_t)Temp; // 25.6 → 25
                // 2. 提取小数位（放大10倍后取余，保留1位）
                temp_dec = (uint8_t)((Temp - temp_int) * 10); // 25.6-25=0.6 → 0.6×10=6
			
				Serial_SendByte(1, temp_int);  // 第一次发送：整数位（25 → 0x19）
                Delay_ms(1);                // 短暂延时，避免接收端粘包（可选）
                Serial_SendByte(1, temp_dec);  // 第二次发送：小数位（6 → 0x06）
				break;
			case 0x11:
				Temp2Gear = 0;
				Working = 1;
				AD_Collect_Stop();
				if(Gear < 5){
					Gear++;
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				break;
			case 0x12:
				Temp2Gear = 0;
				AD_Collect_Stop();
				if(Gear > 0){
					Gear--;
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				if(Gear == 0) Working = 0;
				break;
			case 0x13:
				Serial_SendByte(1, RxData);
                Serial_SendByte(1, Gear);  
				break;
			case 0x2C:
				Count_Set_0();
				Count_State = 0;
				break;
			case 0x2D:
				if(!Count_Started)
					Count_Start();
				else
					Count_Stop();
				break;
			case 0x20:
				switch(Count_State){
					case 0:
						Count_Add_Hour(0);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(0);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(0);
						Count_State = 9;
						break;
				}
				break;
			case 0x21:
				switch(Count_State){
					case 0:
						Count_Add_Hour(5);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(5);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(5);
						Count_State = 9;
						break;
				}
				break;
			case 0x22:
				switch(Count_State){
					case 0:
						Count_Add_Hour(10);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(10);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(10);
						Count_State = 9;
						break;
				}
				break;
			case 0x23:
				switch(Count_State){
					case 0:
						Count_Add_Hour(15);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(15);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(15);
						Count_State = 9;
						break;
				}
				break;
			case 0x24:
				switch(Count_State){
					case 0:
						Count_Add_Hour(20);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(20);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(20);
						Count_State = 9;
						break;
				}
				break;
			case 0x25:
				switch(Count_State){
					case 0:
						Count_Add_Hour(25);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(25);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(25);
						Count_State = 9;
						break;
				}
				break;
			case 0x26:
				switch(Count_State){
					case 0:
						Count_Add_Hour(30);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(30);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(30);
						Count_State = 9;
						break;
				}
				break;
			case 0x27:
				switch(Count_State){
					case 0:
						Count_Add_Hour(35);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(35);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(35);
						Count_State = 9;
						break;
				}
				break;
			case 0x28:
				switch(Count_State){
					case 0:
						Count_Add_Hour(40);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(40);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(40);
						Count_State = 9;
						break;
				}
				break;
			case 0x29:
				switch(Count_State){
					case 0:
						Count_Add_Hour(45);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(45);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(45);
						Count_State = 9;
						break;
				}
				break;
			case 0x2A:
				switch(Count_State){
					case 0:
						Count_Add_Hour(50);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(50);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(50);
						Count_State = 9;
						break;
				}
				break;
			case 0x2B:
				switch(Count_State){
					case 0:
						Count_Add_Hour(55);
						Count_State = 1;
						break;
					case 1:
						Count_Add_Minute(55);
						Count_State = 2;
						break;
					case 2:
						Count_Add_Second(55);
						Count_State = 9;
						break;
				}
				break;
			case 0x31:
				MyRTC_ReadTime();
				
				memset(Serial_TxDataPacket, 0, Serial_SizeofTxPacket);// 清空发送缓冲区
				
				// 【数据包内容： 年高 + 年低 + 月 + 日 + 星期 + 时 + 分 + 秒】
				Serial_TxDataPacket[0] = (MyRTC_Time.Year >> 8) & 0xFF; // 年高8位
				Serial_TxDataPacket[1] = MyRTC_Time.Year & 0xFF;        // 年低8位
				Serial_TxDataPacket[2] = MyRTC_Time.Month;
				Serial_TxDataPacket[3] = MyRTC_Time.Day;
				Serial_TxDataPacket[4] = MyRTC_Time.Weekday;
				Serial_TxDataPacket[5] = MyRTC_Time.Hour;
				Serial_TxDataPacket[6] = MyRTC_Time.Minute;
				Serial_TxDataPacket[7] = MyRTC_Time.Second;
				
				Serial_SendPacket(1, 8);
				break;
			case 0x32:
				ESP8266_GetTime();
				MyRTC_ReadTime();
				
				memset(Serial_TxDataPacket, 0, Serial_SizeofTxPacket);// 清空发送缓冲区
				
				// 【数据包内容： 年高 + 年低 + 月 + 日 + 星期 + 时 + 分 + 秒】
				Serial_TxDataPacket[0] = (MyRTC_Time.Year >> 8) & 0xFF; // 年高8位
				Serial_TxDataPacket[1] = MyRTC_Time.Year & 0xFF;        // 年低8位
				Serial_TxDataPacket[2] = MyRTC_Time.Month;
				Serial_TxDataPacket[3] = MyRTC_Time.Day;
				Serial_TxDataPacket[4] = MyRTC_Time.Weekday;
				Serial_TxDataPacket[5] = MyRTC_Time.Hour;
				Serial_TxDataPacket[6] = MyRTC_Time.Minute;
				Serial_TxDataPacket[7] = MyRTC_Time.Second;
				
				Serial_SendPacket(1, 8);
				break;
			case 0x33:
				ESP8266_GetWeather();
				Serial_SendByte(1, weather_temp_u8);
				Delay_ms(1);
				Serial_SendByte(1, weather_code);
				break;
		}
	}
}

void Distance_Warn(void){
	Serial_SendByte(1, 0x14);
}
