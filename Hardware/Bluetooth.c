#include "stm32f10x.h"                  // Device header
#include "HC_04.h"
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
#include "stdio.h"
#include <string.h> 


/*
HC-04 Tx - Rx PA3
HC-04 Rx - Tx PA2
0x01:开启温度调节	0x02:关闭温度调节	0x03:报温度
0x11:升档	0x12:降档	0x13:报档位	0x14:距离过近提示
0x2C:设置倒计时	依次说 小时：分钟：秒钟 每个块对应一个时间	0x2D:开始倒计时
0x20 ~ 0x2B 对应0 ~ 55
0x31:报时
*/

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

void Bluetooth_Init(void){
	Serial2_Init();
}

void Bluetooth(void){
	if(Serial2_GetRxFlag() == 1)
	{
		RxData2 =Serial2_GetRxData();
		//Serial_SendByte(RxData);
		switch(RxData2){
			case 0x01:	//开启温度调节
				AD_Collect_Start();
				Working = 1;
				Temp2Gear = 1;
				Last_Gear = 0;
				Serial_SendByte(0x01);
				break;
			case 0x02:	//关闭温度调节
				AD_Collect_Stop();
				Working = 0;
				Temp = 0;
				Temp2Gear = 0;
				Gear = 0;
				Last_Gear = 0;
				Motor_Stop();
				Serial_SendByte(0x02);
				break;
			case 0x03:	//报温度
				// 1. 提取整数位（直接强制类型转换）
				temp_int = (uint8_t)Temp; // 25.6 → 25
                // 2. 提取小数位（放大10倍后取余，保留1位）
                temp_dec = (uint8_t)((Temp - temp_int) * 10); // 25.6-25=0.6 → 0.6×10=6
				
				Serial_SendByte(0x03);
				Delay_ms(1);  
				Serial_SendByte(temp_int);  // 第一次发送：整数位（25 → 0x19）
                Delay_ms(1);                // 短暂延时，避免接收端粘包（可选）
                Serial_SendByte(temp_dec);  // 第二次发送：小数位（6 → 0x06）
			
				// 合成一条完整字符串：当前温度：25.6℃
				sprintf(Str, "当前温度：%d.%d℃", temp_int, temp_dec);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
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
				
				Serial_SendByte(0x11);
				
				sprintf(Str, "当前档位：%d档", Gear);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
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
				Serial_SendByte(0x12);
				sprintf(Str, "当前档位：%d档", Gear);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
				break;
			case 0x13:
				Serial_SendByte(0x13); 
                Serial_SendByte(Gear);  
			
				sprintf(Str, "当前档位：%d档", Gear);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
				break;
			case 0x21:
				Count_Add_1h();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
				break;
			case 0x22:
				Count_Sub_1h();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
				break;
			case 0x23:
				Count_Add_1m();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
				break;
			case 0x24:
				Count_Sub_1m();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
				break;
			case 0x25:
				Count_Add_1s();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
				break;
			case 0x26:
				Count_Sub_1s();
				cnt = Get_Count();
				sprintf(Str, "倒计时：%d时:%d分:%d秒", cnt/3600, cnt%3600/60, cnt%3600%60);
				Serial2_SendString(Str);
                Serial2_SendByte('\n');
				break;
			case 0x27:
				if(!Count_Started)
					Count_Start();
				Serial_SendByte(0x27);
				
				Serial2_SendString("倒计时已开始");
				Serial2_SendByte('\n');
				break;
			case 0x28:
				if(Count_Started)
					Count_Stop();
				Serial2_SendString("倒计时已停止");
				Serial2_SendByte('\n');
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
				Serial2_SendString(Str);
				Serial2_SendByte('\n');
				sprintf(Str, "北京时间%d时%d分%d秒", MyRTC_Time.Hour, MyRTC_Time.Minute, MyRTC_Time.Second);
				Serial2_SendString(Str);
				Serial2_SendByte('\n');
				
				Serial_SendByte(0x31);
				memset(Serial_TxPacket, 0, Serial_SizeofTxPacket);// 清空发送缓冲区
				
				// 【数据包内容： 年高 + 年低 + 月 + 日 + 星期 + 时 + 分 + 秒】
				Serial_TxPacket[0] = (MyRTC_Time.Year >> 8) & 0xFF; // 年高8位
				Serial_TxPacket[1] = MyRTC_Time.Year & 0xFF;        // 年低8位
				Serial_TxPacket[2] = MyRTC_Time.Month;
				Serial_TxPacket[3] = MyRTC_Time.Day;
				Serial_TxPacket[4] = MyRTC_Time.Weekday;
				Serial_TxPacket[5] = MyRTC_Time.Hour;
				Serial_TxPacket[6] = MyRTC_Time.Minute;
				Serial_TxPacket[7] = MyRTC_Time.Second;
				
				Serial_SendPacket();
				break;
		}
	}
}
