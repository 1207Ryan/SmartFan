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
0x41:设置温度阈值		0x42:音量增加		0x43:音量减少		0x44:音量最大		0x45:音量最小		0x46:设置音量
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
extern uint8_t VoiceVolume;
uint8_t Count_State = 9;
uint8_t High_Byte;
uint8_t Low_Byte;

void Voice_Recognition_Init(void){
	Serial_Init(1);
}

void Voice_Recognition(void){
	if(Serial_GetRxFlag(1) == 1){
		RxData = Serial1_RxDataPacket[0];
		//Serial_SendByte(RxData);
		switch(RxData){
			case 0x00:
				VoiceVolume = Serial1_RxDataPacket[1];//音量
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
				
				Serial_SetTxDataPacket(3, 0x03, temp_int, temp_dec); // 第一位：整数位（25 → 0x19）；第二位：小数位（6 → 0x06）
				Serial_SendPacket(1, 3); 
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
				Serial_SetTxDataPacket(2, 0x13, Gear);
				Serial_SendPacket(1, 2); 
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
				
				Serial_ClearTxBuffer(); // 清空发送缓冲区
				
				// 【数据包内容： 年高 + 年低 + 月 + 日 + 星期 + 时 + 分 + 秒】
				Serial_SetTxDataPacket(9, 0x31, (MyRTC_Time.Year >> 8) & 0xFF, MyRTC_Time.Year & 0xFF,
					MyRTC_Time.Month, MyRTC_Time.Day, MyRTC_Time.Weekday, 
					MyRTC_Time.Hour, MyRTC_Time.Minute, MyRTC_Time.Second);
				Serial_SendPacket(1, 9);
				break;
			case 0x32:
				ESP8266_GetTime();
				MyRTC_ReadTime();
				
				Serial_ClearTxBuffer(); // 清空发送缓冲区
				
				// 【数据包内容： 年高 + 年低 + 月 + 日 + 星期 + 时 + 分 + 秒】
				Serial_SetTxDataPacket(9, 0x32, (MyRTC_Time.Year >> 8) & 0xFF, MyRTC_Time.Year & 0xFF,
					MyRTC_Time.Month, MyRTC_Time.Day, MyRTC_Time.Weekday, 
					MyRTC_Time.Hour, MyRTC_Time.Minute, MyRTC_Time.Second);
				Serial_SendPacket(1, 9);
				break;
			case 0x33:
				ESP8266_GetWeather();
				Serial_SetTxDataPacket(3, 0x33, weather_temp_u8, weather_code);//温度 , 天气代码
				Serial_SendPacket(1, 3);
				break;
		}
	}
}

void Distance_Warn(void){
	Serial_TxDataPacket[0] = 0x14;
	Serial_SendPacket(1, 1); 
}

void Fan_On(void){
	Serial_SetTxDataPacket(1, 0x01);
	Serial_SendPacket(1, 1);
}

void Fan_Off(void){
	Serial_SetTxDataPacket(1, 0x02);
	Serial_SendPacket(1, 1);
}

void Fan_Gear_Up(void){
	Serial_SetTxDataPacket(1, 0x11);
	Serial_SendPacket(1, 1);
}

void Fan_Gear_Max(void){
	Serial_SetTxDataPacket(1, 0x15);
	Serial_SendPacket(1, 1);
}

void Fan_Gear_Down(void){
	Serial_SetTxDataPacket(1, 0x12);
	Serial_SendPacket(1, 1);
}

void SetTempThreshold(void){
	Serial_SetTxDataPacket(1, 0x41);
	Serial_SendPacket(1, 1); 
}

void Volume_Up(void){
	Serial_SetTxDataPacket(1, 0x42);
	Serial_SendPacket(1, 1); 
}

void Volume_Max(void){
	Serial_SetTxDataPacket(1, 0x44);
	Serial_SendPacket(1, 1); 
}

void Volume_Down(void){
	Serial_SetTxDataPacket(1, 0x43);
	Serial_SendPacket(1, 1); 
}

void Volume_Min(void){
	Serial_SetTxDataPacket(1, 0x45);
	Serial_SendPacket(1, 1); 
}

void Volume_Set(uint8_t Volume){
	Serial_SetTxDataPacket(2, 0x46, Volume);
	Serial_SendPacket(1, 2); 
}
