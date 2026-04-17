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
#include "Voice_Recognition.h"
#include <stdio.h>
#include <string.h> 

/*
HC-04 Tx - Rx PA3
HC-04 Rx - Tx PA2
0x01:开启温度调节	0x02:关闭温度调节	0x03:报温度
0x11:升档	0x12:降档	0x13:报档位	0x14:距离过近提示	0x15:已经是最大档	0x16:风扇已停止
0x2C:设置倒计时	依次说 小时：分钟：秒钟 每个块对应一个时间	0x2D:开始倒计时
0x20 ~ 0x2B 对应0 ~ 55
0x31:报时
0x41:设置温度阈值
0xEE:蓝牙已连接	0xEF:蓝牙已断开
*/

uint8_t TxData2;
uint8_t RxData2;
extern uint8_t temp_int;
extern uint8_t temp_dec;
char Str[30];
extern volatile uint8_t Working;
extern volatile uint8_t Gear;
extern volatile uint8_t Last_Gear;
extern volatile uint8_t Temp2Gear;
extern volatile float Temp;
extern uint8_t Count_Started;
extern uint32_t cnt;
extern volatile uint8_t Music_IsOn;
char Weekday[2];

void HC_04_Init(void){
	Serial_Init(2);
}

void HC_04_Detect(void){
	if(Serial_GetRxFlag(2) == 1){
		RxData2 = Serial2_RxDataPacket[0];
		//Serial_SendByte(RxData);
		switch(RxData2){
			case 0x01:	//开启温度调节
				AD_Collect_Start();
				Working = 1;
				Temp2Gear = 1;
				Last_Gear = 0;
				Fan_On();
				//Serial_SendByte(1, 0x01);
				Serial_SendString(2, "风扇已开启");
                Serial_SendByte(2, '\n');
				break;
			case 0x02:	//关闭温度调节
				AD_Collect_Stop();
				Working = 0;
				Temp = 0;
				Temp2Gear = 0;
				Gear = 0;
				Last_Gear = 0;
				Motor_Stop();
				Fan_Off();
				//Serial_SendByte(1, 0x02);
				Serial_SendString(2, "风扇已关闭");
                Serial_SendByte(2, '\n');
				break;
			case 0x03:	//报温度
				// 1. 提取整数位（直接强制类型转换）
				temp_int = (uint8_t)Temp; // 25.6 → 25
                // 2. 提取小数位（放大10倍后取余，保留1位）
                temp_dec = (uint8_t)((Temp - temp_int) * 10); // 25.6-25=0.6 → 0.6×10=6
				
				Serial_SetTxDataPacket(3, 0x03, temp_int, temp_dec); // 第一位：整数位（25 → 0x19）；第二位：小数位（6 → 0x06）
				Serial_SendPacket(1, 3); 
			
				// 合成一条完整字符串：当前温度：25.6℃
				sprintf(Str, "当前温度：%d.%d℃", temp_int, temp_dec);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x11:
				Temp2Gear = 0;
				Working = 1;
				AD_Collect_Stop();
				if(Gear >= 5){
					Fan_Gear_Max();	//已经是最大档位
				}else if(Gear < 5){
					Gear++;
					Fan_Gear_Up();	//档位已上升
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				
				Serial_SetTxDataPacket(1, TxData2);
				Serial_SendPacket(1, 1); 
				sprintf(Str, "当前档位：%d档", Gear);
				Serial_SendString(2, Str);
				Serial_SendByte(2, '\n');
				break;
			case 0x12:
				Temp2Gear = 0;
				AD_Collect_Stop();
				if(Gear > 0){
					Gear--;
					Fan_Gear_Down();
					if(Gear == 0){
						Working = 0;
					}
				}else if(Gear == 0){
					Fan_Off();
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				
				sprintf(Str, "当前档位：%d档", Gear);
				Serial_SendString(2, Str);
				Serial_SendByte(2, '\n');
				break;
			case 0x13:
				Serial_SetTxDataPacket(2, 0x13, Gear);
				Serial_SendPacket(1, 2); 
			
				sprintf(Str, "当前档位：%d档", Gear);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x21:
				Count_Add_1h();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x22:
				Count_Sub_1h();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x23:
				Count_Add_1m();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x24:
				Count_Sub_1m();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x25:
				Count_Add_1s();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x26:
				Count_Sub_1s();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x27:
				if(!Count_Started)
					Count_Start();
				Serial_SetTxDataPacket(1, 0x27);
				Serial_SendPacket(1, 1); 
				
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x28:
				if(Count_Started)
					Count_Stop();
				Serial_SendString(2, "倒计时已停止");
				Serial_SendByte(2, '\n');
				break;
			case 0x31:
				MyRTC_ReadTime();
				
				switch(MyRTC_Time.Weekday){
					case 0:
						strcpy(Weekday ,"日");
						break;
					case 1:
						strcpy(Weekday ,"一");
						break;
					case 2:
						strcpy(Weekday ,"二");
						break;
					case 3:
						strcpy(Weekday ,"三");
						break;
					case 4:
						strcpy(Weekday ,"四");
						break;
					case 5:
						strcpy(Weekday ,"五");
						break;
					case 6:
						strcpy(Weekday ,"六");
						break;
				}
				sprintf(Str, "现在是%d年%d月%d日，星期%s", MyRTC_Time.Year, MyRTC_Time.Month, MyRTC_Time.Day, Weekday);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				sprintf(Str, "北京时间%d时%d分%d秒", MyRTC_Time.Hour, MyRTC_Time.Minute, MyRTC_Time.Second);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				
				Serial_ClearTxBuffer(); // 清空发送缓冲区
				// 【数据包内容： 0x31 + 年高 + 年低 + 月 + 日 + 星期 + 时 + 分 + 秒】
				Serial_SetTxDataPacket(9, 0x31, (MyRTC_Time.Year >> 8) & 0xFF, MyRTC_Time.Year & 0xFF,
					MyRTC_Time.Month, MyRTC_Time.Day, MyRTC_Time.Weekday, 
					MyRTC_Time.Hour, MyRTC_Time.Minute, MyRTC_Time.Second);
				Serial_SendPacket(1, 9);
				break;
			case 0x41:
				for (int i = 0; i < 5; i++) {
					uint8_t temp_int = Serial2_RxDataPacket[1 + i * 2];   // 整数部分
					uint8_t temp_dec = Serial2_RxDataPacket[2 + i * 2];   // 小数部分
					
					float temperature = temp_int + temp_dec / 10.0;
					TempThreshold_Set(i+1, temperature);
				}
				Serial_SetTxDataPacket(1, 0x41);
				Serial_SendPacket(1, 1); 
				Serial_SendString(2, "温度阈值已设置");
                Serial_SendByte(2, '\n');
				break;
			case 0x46:
				Volume_Set(Serial2_RxDataPacket[1]);
				sprintf(Str, "语音音量已调整为:%d", Serial2_RxDataPacket[1]);
				Serial_SendString(2, Str);
                Serial_SendByte(2, '\n');
				break;
			case 0x7E:
				if(Serial2_RxDataPacket[3] == 0x0D){//播放命令
					Music_IsOn = 1;
				}else if(Serial2_RxDataPacket[3] == 0x0E){//暂停命令
					Music_IsOn = 0;
				}else if(Serial2_RxDataPacket[3] == 0x01){//下一首
					Music_IsOn = 1;
				}else if(Serial2_RxDataPacket[3] == 0x02){//上一首
					Music_IsOn = 1;
				}
				for(uint8_t i = 0; i < 8; i++){
					Serial_TxDataPacket[i] = Serial2_RxDataPacket[i];
				}
				Serial_SendPacket(1, 8);
				Serial_SendString(2, "收到音乐操作指令");
                Serial_SendByte(2, '\n');
				break;
			case 0xEE:	//蓝牙已断开
				Serial_TxDataPacket[0] = 0xEE;
				Serial_SendPacket(1, 1); 
				Serial_SendString(2, "蓝牙已断开");
                Serial_SendByte(2, '\n');
				break;
			case 0xEF:	//蓝牙已连接
				Serial_TxDataPacket[0] = 0xEF;
				Serial_SendPacket(1, 1); 
				Serial_SendString(2, "蓝牙已连接");
                Serial_SendByte(2, '\n');
				break;
		}
	}
}
