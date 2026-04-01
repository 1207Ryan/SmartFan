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
0xFE:蓝牙已连接	0xFF:蓝牙已断开
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
char Weekday[2];

void HC_04_Init(void){
	Serial_Init(2);
}

void HC_04_Detect(void){
	if(Serial_GetRxFlag(2) == 1){
		RxData2 =Serial_GetRxData(2);
		//Serial_SendByte(RxData);
		switch(RxData2){
			case 0x01:	//开启温度调节
				AD_Collect_Start();
				Working = 1;
				Temp2Gear = 1;
				Last_Gear = 0;
				Serial_SendByte(1, 0x01);
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
				Serial_SendByte(1, 0x02);
				Serial_SendString(2, "风扇已关闭");
                Serial_SendByte(2, '\n');
				break;
			case 0x03:	//报温度
				// 1. 提取整数位（直接强制类型转换）
				temp_int = (uint8_t)Temp; // 25.6 → 25
                // 2. 提取小数位（放大10倍后取余，保留1位）
                temp_dec = (uint8_t)((Temp - temp_int) * 10); // 25.6-25=0.6 → 0.6×10=6
				
				Serial_SendByte(1, 0x03);
				Delay_ms(1);  
				Serial_SendByte(1, temp_int);  // 第一次发送：整数位（25 → 0x19）
                Delay_ms(1);                // 短暂延时，避免接收端粘包（可选）
                Serial_SendByte(1, temp_dec);  // 第二次发送：小数位（6 → 0x06）
			
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
					TxData2 = 0x15;
				}else if(Gear < 5){
					Gear++;
					TxData2 = 0x11;
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				
				Serial_SendByte(1, TxData2);
				sprintf(Str, "当前档位：%d档", Gear);
				Serial_SendString(2, Str);
				Serial_SendByte(2, '\n');
				break;
			case 0x12:
				Temp2Gear = 0;
				AD_Collect_Stop();
				if(Gear > 0){
					Gear--;
					TxData2 = 0x12;
				}else if(Gear == 0){
					Working = 0;
					TxData2 = 0x16;
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				
				Serial_SendByte(1, TxData2);
				sprintf(Str, "当前档位：%d档", Gear);
				Serial_SendString(2, Str);
				Serial_SendByte(2, '\n');
				break;
			case 0x13:
				Serial_SendByte(1, 0x13); 
                Serial_SendByte(1, Gear);  
			
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
				Serial_SendByte(1, 0x27);
				
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
				
				switch(MyRTC_Time.Weekday)
				{
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
				
				Serial_SendByte(1, 0x31);
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
					case 0x41:
						break
			case 0xFE:	//蓝牙已连接
				Serial_SendByte(1, 0xFE);
				Serial_SendString(2, "蓝牙已开启");
                Serial_SendByte(2, '\n');
				break;
			case 0xFF:	//蓝牙已连接
				Serial_SendByte(1, 0xFF);
				Serial_SendString(2, "蓝牙已断开");
                Serial_SendByte(2, '\n');
				break;
		}
	}
}
