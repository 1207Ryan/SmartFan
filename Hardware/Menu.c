#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Menu.h"
#include "Motor.h"
#include "MyRTC.h"
#include "AD.h"
#include "HC_SR04.h"
#include "Count_Down.h"
#include "Timer.h"
#include "Serial.h"
#include "ESP8266.h"
#include "Voice_Recognition.h"

uint8_t CurrSelect1 = 1;
uint8_t CurrSelect2 = 1;
uint8_t CurrSelect3 = 1;
uint8_t CurrSelect4 = 1;
uint8_t CurrState = 0;

float Temp_Gear_1 = 20.0f;  // 1档温度阈值
float Temp_Gear_2 = 28.0f;  // 2档温度阈值
float Temp_Gear_3 = 36.0f;  // 3档温度阈值
float Temp_Gear_4 = 44.0f;  // 4档温度阈值
float Temp_Gear_5 = 52.0f;  // 5档温度阈值

volatile uint8_t Working = 0;
volatile uint8_t Gear;
volatile uint8_t Last_Gear;
volatile float Temp;
volatile uint8_t Temp2Gear = 0;
volatile uint8_t VoiceVolume = 1;
volatile uint8_t MusicVolume = 10;
volatile uint8_t Music_IsOn = 0;
extern volatile uint8_t IsSafe;
extern uint32_t cnt;
extern uint8_t Count_Started;

uint8_t Menu1(void){
	uint8_t Menu1_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			if(CurrSelect1 >= 1 && CurrSelect1 <= 4){
				OLED_ShowString(0, 0,  "温度调节档位          	", OLED_8X16);
				OLED_ShowString(0, 16, "风扇档位控制          	", OLED_8X16);
				OLED_ShowString(0, 32, "倒计时               	", OLED_8X16);
				OLED_ShowString(0, 48, "时间显示            	", OLED_8X16);
			}
			else if(CurrSelect1 >= 5 && CurrSelect1 <= 8){
				OLED_ShowString(0, 0,  "天气显示        	  ", OLED_8X16);
				OLED_ShowString(0, 16, "调整温度阈值     	  ", OLED_8X16);
				OLED_ShowString(0, 32, "音乐               ", OLED_8X16);
				OLED_ShowString(0, 48, "调整语音音量         ", OLED_8X16);
			}else if(CurrSelect1 >= 9 && CurrSelect1 <= 9){
				OLED_ShowString(0, 0,  "调试                   ", OLED_8X16);
				OLED_ShowString(0, 16, "                      ", OLED_8X16);
				OLED_ShowString(0, 32, "                      ", OLED_8X16);
				OLED_ShowString(0, 48, "                      ", OLED_8X16);
			}

			OLED_ReverseArea(0, ((CurrSelect1-1)*16)%64, 128, 16);
			OLED_Update();
			CurrState = 1;
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect1-1)*16)%64, 128, 16);
			CurrSelect1--;
			if(CurrSelect1 <= 0){
				CurrSelect1 = TotalSelect;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect1-1)*16)%64, 128, 16);
			CurrSelect1++;
			if(CurrSelect1 > TotalSelect){
				CurrSelect1 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect1-1)*16)%64, 128, 16);
			OLED_Clear();
			OLED_Update();
			Menu1_Select = CurrSelect1;
		}
		
		switch(Menu1_Select){
			case 1:
				Menu1_Select = 0;
				Menu2_Temp();
				break;
			case 2:
				Menu1_Select = 0;
				Menu2_Fan();
				break;
			case 3:
				Menu1_Select = 0;
				Menu2_CountDown();
				break;
			case 4:
				Menu1_Select = 0;
				Menu2_Clock();
				break;
			case 5:
				Menu1_Select = 0;
				Menu2_Weather();
				break;
			case 6:
				Menu1_Select = 0;
				Menu2_SetTempThreshold();
				break;
			case 7:
				Menu1_Select = 0;
				Menu2_Music();
				break;
			case 8:
				Menu1_Select = 0;
				Menu2_SetVoiceVolume();
				break;
			case 9:
				Menu1_Select = 0;
				Menu2_Debug();
				break;
		}
	}
}

/**
  * @brief 温度控制风扇档位二级菜单
  * @param	无
  * @retval 无
  */
void Menu2_Temp(){
	uint8_t Menu2_Select = 0;

	while(1){
		if(CurrState == 0){
			OLED_Clear();
			OLED_ShowString(0, 0,  "<-         ℃ 档", OLED_8X16);
			OLED_ShowString(0, 16, "启动温度调节档位  ", OLED_8X16);
			OLED_ShowString(0, 32, "停止风扇        ", OLED_8X16);
			OLED_ShowString(0, 48, "              ", OLED_8X16);
			
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_ClearArea(24, 0, 16, 16);
			OLED_ClearArea(44, 0, 40, 16);  // 清空温度
			OLED_ClearArea(104, 0, 8, 16);
			if(Temp2Gear){
				OLED_ShowFloatNum(44, 0, Temp, 2, 1, OLED_8X16);  // 实时刷新温度
				OLED_ShowString(24, 0, "√", OLED_8X16);
			}	
			else{
				OLED_ShowString(52, 0, "--.-", OLED_8X16);
				OLED_ShowString(24, 0, "×", OLED_8X16);
			}
			OLED_ShowNum(104, 0, Gear, 1, OLED_8X16);
			if(!IsSafe)
				OLED_DrawRectangle(104, 0, 8, 16, OLED_UNFILLED);
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_UpdateArea(24, 0, 16, 16);
			OLED_UpdateArea(44, 0, 40, 16);  // 更新温度区域
			OLED_UpdateArea(104, 0, 8, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 3;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			CurrSelect2++;
			if(CurrSelect2 > 3){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				return;
			case 2:		//启动温度调节档位
				AD_Collect_Start();
				Working = 1;
				Temp2Gear = 1;
				Last_Gear = 0;
				Fan_On();
				//Serial_SendByte(1, 0x01);
				Menu2_Select = 0;
				break;
			case 3:		//停止风扇
				AD_Collect_Stop();
				Working = 0;
				Temp = 0;
				Temp2Gear = 0;
				Gear = 0;
				Last_Gear = 0;
				Motor_Stop();
				Fan_Off();
				//Serial_SendByte(1, 0x02);
				Menu2_Select = 0;
				break;
		}
	}
}

/**
  * @brief 风扇档位控制二级菜单
  * @param 无
  * @retval 无
  */
void Menu2_Fan(void){
	uint8_t Menu2_Select = 0;

	while(1){
		if(CurrState == 0){
			OLED_Clear();
			OLED_ShowString(0, 0, "<-     当前:  档", OLED_8X16);
			OLED_ShowString(0, 16, "升档               ", OLED_8X16);
			OLED_ShowString(0, 32, "降档               ", OLED_8X16);
			OLED_ShowString(0, 48, "停止               ", OLED_8X16);
			
			
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_ClearArea(24, 0, 32, 16);
			OLED_ClearArea(104, 0, 8, 16);
			
			OLED_ShowNum(104, 0, Gear, 1, OLED_8X16);
			if(!IsSafe)
				OLED_DrawRectangle(104, 0, 8, 16, OLED_UNFILLED);
			
			if(Temp2Gear){
				OLED_ShowString(24, 0, "自动", OLED_8X16);
			}else{
				OLED_ShowString(24, 0, "手动", OLED_8X16);
			}
			
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_UpdateArea(24, 0, 32, 16);
			OLED_UpdateArea(104, 0, 8, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 4;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			CurrSelect2++;
			if(CurrSelect2 > 4){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				return;
			case 2:		//升档
				Temp2Gear = 0;
				Working = 1;
				AD_Collect_Stop();
				if(Gear < 5){
					Gear++;
					Fan_Gear_Up();
				}else if(Gear >= 5){
					Fan_Gear_Max();
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				Menu2_Select = 0;
				break;
			case 3:		//降档
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
				Menu2_Select = 0;
				break;
			case 4:		//停止
				AD_Collect_Stop();
				Temp2Gear = 0;
				Gear = 0;
				Last_Gear = 0;
				Motor_Stop();
				Working = 0;
				Fan_Off();
				Menu2_Select = 0;
				break;
		}
	}
}

/**
  * @brief 倒计时二级菜单
  * @param 无
  * @retval 无
  */
void Menu2_CountDown(void){
	uint8_t Menu2_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			
			if(CurrSelect2>=1 && CurrSelect2<=3){
				OLED_ShowString(0, 0,  "<-            档", OLED_8X16);
				OLED_ShowString(0, 16, "设置倒计时         ", OLED_8X16);
				OLED_ShowString(0, 32, "XXXX倒计时          ", OLED_8X16);
				OLED_ShowString(0, 48, "倒计时:            ", OLED_8X16);
			}
			OLED_ShowChar(72, 48, 'h', OLED_8X16);
			OLED_ShowChar(96, 48, 'm', OLED_8X16);
			OLED_ShowChar(120, 48, 's', OLED_8X16);
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%48, 128, 16);
			OLED_ClearArea(0, 32, 32, 16);
			OLED_ClearArea(56, 48, 16, 16);
			OLED_ClearArea(80, 48, 16, 16);
			OLED_ClearArea(104, 48, 16, 16);
			OLED_ClearArea(104, 0, 8, 16);
			
			cnt = Get_Count();
			OLED_ShowNum(56, 48, cnt/3600, 2, OLED_8X16);
			OLED_ShowNum(80, 48, cnt%3600/60, 2, OLED_8X16);
			OLED_ShowNum(104, 48, cnt%3600%60, 2, OLED_8X16);
			if(!Count_Started)
				OLED_ShowString(0, 32, "开始", OLED_8X16);
			else
				OLED_ShowString(0, 32, "停止", OLED_8X16);
			OLED_ShowNum(104, 0, Gear, 1, OLED_8X16);
			
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%48, 128, 16);
			OLED_UpdateArea(0, 32, 32, 16);
			OLED_UpdateArea(56, 48, 16, 16);
			OLED_UpdateArea(80, 48, 16, 16);
			OLED_UpdateArea(104, 48, 16, 16);
			OLED_UpdateArea(104, 0, 8, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 3;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect2++;
			if(CurrSelect2 > 3){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				return;
			case 2:
				Menu2_Select = 0;
				CurrState = 0;
				Menu3_CountDown();
				break;
			case 3:
				Menu2_Select = 0;
				if(!Count_Started)
					Count_Start();
				else
					Count_Stop();
				break;
		}
	}
}

/**
  * @brief 倒计时三级菜单
  * @param 无
  * @retval 无
  */
void Menu3_CountDown(void){
	uint8_t Menu3_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			
			if(CurrSelect3 >= 1 && CurrSelect3 <= 3){
				OLED_ShowString(0, 0,  "<-                ", OLED_8X16);
				OLED_ShowString(0, 16, "加1秒钟            ", OLED_8X16);
				OLED_ShowString(0, 32, "减1秒钟            ", OLED_8X16);
			}else if(CurrSelect3 >= 4 && CurrSelect3 <= 6){
				OLED_ShowString(0, 0,  "<-                ", OLED_8X16);
				OLED_ShowString(0, 16, "加1分钟            ", OLED_8X16);
				OLED_ShowString(0, 32, "减1分钟            ", OLED_8X16);
			}else if(CurrSelect3 >= 7 && CurrSelect3 <= 9){
				OLED_ShowString(0, 0,  "<-                ", OLED_8X16);
				OLED_ShowString(0, 16, "加1小时            ", OLED_8X16);
				OLED_ShowString(0, 32, "减1小时            ", OLED_8X16);
			}
			OLED_ShowString(0, 48, "倒计时:            ", OLED_8X16);
			OLED_ShowChar(72, 48, 'h', OLED_8X16);
			OLED_ShowChar(96, 48, 'm', OLED_8X16);
			OLED_ShowChar(120, 48, 's', OLED_8X16);
			OLED_ReverseArea(0, ((CurrSelect3 - 1)*16)%48, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%48, 128, 16);
			OLED_ClearArea(56, 48, 16, 16);
			OLED_ClearArea(80, 48, 16, 16);
			OLED_ClearArea(104, 48, 16, 16);
			
			OLED_ShowNum(56, 48, Get_Count()/3600, 2, OLED_8X16);
			OLED_ShowNum(80, 48, Get_Count()%3600/60, 2, OLED_8X16);
			OLED_ShowNum(104, 48, Get_Count()%3600%60, 2, OLED_8X16);

			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%48, 128, 16);
			OLED_UpdateArea(56, 48, 16, 16);
			OLED_UpdateArea(80, 48, 16, 16);
			OLED_UpdateArea(104, 48, 16, 16);
			
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect3--;
			if(CurrSelect3 <= 0){
				CurrSelect3 = 9;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect3++;
			if(CurrSelect3 > 9){
				CurrSelect3 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE | KEY_REPEAT)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu3_Select = CurrSelect3;
		}
		
		switch(Menu3_Select){
			case 1:
				Menu3_Select = 0;
				CurrSelect3 = 1;
				return;
			case 2:
				Menu3_Select = 0;
				Count_Add_1s();
				break;
			case 3:
				Menu3_Select = 0;
				Count_Sub_1s();
				break;
			case 4:
				Menu3_Select = 0;
				CurrSelect3 = 1;
				return;
			case 5:
				Menu3_Select = 0;
				Count_Add_1m();
				break;
			case 6:
				Menu3_Select = 0;
				Count_Sub_1m();
				break;
			case 7:
				Menu3_Select = 0;
				CurrSelect3 = 1;
				return;
			case 8:
				Menu3_Select = 0;
				Count_Add_1h();
				break;
			case 9:
				Menu3_Select = 0;
				Count_Sub_1h();
				break;
		}
	}
}

/**
  * @brief 时钟控制二级菜单
  * @param 无
  * @retval 无
  */
void Menu2_Clock(void){
	uint8_t Menu2_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			OLED_ShowString(0, 0, "<-              ", OLED_8X16);
			OLED_ShowString(0, 16, "Date:XXXX-XX-XX", OLED_8X16);
			OLED_ShowString(0, 32, "Time:XX:XX:XX  ", OLED_8X16);
			OLED_ShowString(0, 48, "设置日期和时间    ", OLED_8X16);
			OLED_ShowString(80, 0, "星期", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect2 - 1)*48, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*48)%64, 128, 16);
			OLED_ClearArea(112, 0, 16, 16);
			OLED_ClearArea(40, 16, 32, 16);
			OLED_ClearArea(80, 16, 16, 16);
			OLED_ClearArea(104, 16, 16, 16);
			OLED_ClearArea(40, 32, 16, 16);
			OLED_ClearArea(64, 32, 16, 16);
			OLED_ClearArea(88, 32, 16, 16); 
			MyRTC_ReadTime();
			
			switch(MyRTC_Time.Weekday)
			{
				case 0:
					OLED_ShowString(112, 0, "日", OLED_8X16);
					break;
				case 1:
					OLED_ShowString(112, 0, "一", OLED_8X16);
					break;
				case 2:
					OLED_ShowString(112, 0, "二", OLED_8X16);
					break;
				case 3:
					OLED_ShowString(112, 0, "三", OLED_8X16);
					break;
				case 4:
					OLED_ShowString(112, 0, "四", OLED_8X16);
					break;
				case 5:
					OLED_ShowString(112, 0, "五", OLED_8X16);
					break;
				case 6:
					OLED_ShowString(112, 0, "六", OLED_8X16);
					break;
			}
			
			OLED_ShowNum(40, 16, MyRTC_Time.Year, 4, OLED_8X16);
			OLED_ShowNum(80, 16, MyRTC_Time.Month, 2, OLED_8X16);
			OLED_ShowNum(104, 16, MyRTC_Time.Day, 2, OLED_8X16);
			OLED_ShowNum(40, 32, MyRTC_Time.Hour, 2, OLED_8X16);
			OLED_ShowNum(64, 32, MyRTC_Time.Minute, 2, OLED_8X16);
			OLED_ShowNum(88, 32, MyRTC_Time.Second, 2, OLED_8X16);

			OLED_ReverseArea(0, ((CurrSelect2 - 1)*48)%64, 128, 16);
			OLED_UpdateArea(112, 0, 16, 16);
			OLED_UpdateArea(40, 16, 32, 16);
			OLED_UpdateArea(80, 16, 16, 16);
			OLED_UpdateArea(104, 16, 16, 16);
			OLED_UpdateArea(40, 32, 16, 16);
			OLED_UpdateArea(64, 32, 16, 16);
			OLED_UpdateArea(88, 32, 16, 16); 
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 2;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect2++;
			if(CurrSelect2 > 2){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				return;
			case 2:
				Menu2_Select = 0;
				Menu3_SetDateAndTime();
				break;
		}
	}
}

/**
  * @brief 时钟控制三级菜单
  * @param 无
  * @retval 无
  */
void Menu3_SetDateAndTime(void){
	uint8_t Menu3_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			OLED_ShowString(0, 0,  "<-            ", OLED_8X16);
			OLED_ShowString(0, 16, "自动校准        ", OLED_8X16);
			OLED_ShowString(0, 32, "设置日期        ", OLED_8X16);
			OLED_ShowString(0, 48, "设置时间        ", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect3 - 1)*16, 128, 16);
			OLED_Update();
			CurrState = 1;
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect3--;
			if(CurrSelect3 <= 0){
				CurrSelect3 = 4;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect3++;
			if(CurrSelect3 > 4){
				CurrSelect3 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu3_Select = CurrSelect3;
		}
		
		switch(Menu3_Select){
			case 1:
				Menu3_Select = 0;
				return;
			case 2:
				Menu3_Select = 0;
				OLED_ShowString(0, 0, "正在获取中……", OLED_8X16);
				OLED_Update();
				ESP8266_GetTime();
				OLED_Clear();
				OLED_Update();
				break;
			case 3:
				Menu3_Select = 0;
				Menu4_SetDate();
				break;
			case 4:
				Menu3_Select = 0;
				Menu4_SetTime();
				break;
		}
	}
}

/**
  * @brief 时钟控制四级菜单，设置日期
  * @param 无
  * @retval 无
  */
void Menu4_SetDate(void){
	uint8_t Menu4_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			if(CurrSelect4 >= 1 && CurrSelect4 <= 3){
				OLED_ShowString(0, 0,  "<-           ", OLED_8X16);
				OLED_ShowString(0, 16, "年份+1        ", OLED_8X16);
				OLED_ShowString(0, 32, "年份-1        ", OLED_8X16);
			}else if(CurrSelect4 >= 4 && CurrSelect4 <= 6){
				OLED_ShowString(0, 0,  "<-           ", OLED_8X16);
				OLED_ShowString(0, 16, "月份+1        ", OLED_8X16);
				OLED_ShowString(0, 32, "月份-1        ", OLED_8X16);
			}else if(CurrSelect4 >= 7 && CurrSelect4 <= 9){
				OLED_ShowString(0, 0,  "<-           ", OLED_8X16);
				OLED_ShowString(0, 16, "日期+1        ", OLED_8X16);
				OLED_ShowString(0, 32, "日期-1        ", OLED_8X16);
			}
			OLED_ShowString(0, 48, "Date:XXXX-XX-XX", OLED_8X16);
			
			OLED_ReverseArea(0, ((CurrSelect4 - 1)*16)%48, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect4 - 1)*16)%48, 128, 16);
			OLED_ClearArea(40, 48, 32, 16);
			OLED_ClearArea(80, 48, 16, 16);
			OLED_ClearArea(104, 48, 16, 16);
			MyRTC_ReadTime();
			
			OLED_ShowNum(40, 48, MyRTC_Time.Year, 4, OLED_8X16);
			OLED_ShowNum(80, 48, MyRTC_Time.Month, 2, OLED_8X16);
			OLED_ShowNum(104, 48, MyRTC_Time.Day, 2, OLED_8X16);

			OLED_ReverseArea(0, ((CurrSelect4 - 1)*48)%48, 128, 16);
			OLED_UpdateArea(40, 48, 32, 16);
			OLED_UpdateArea(80, 48, 16, 16);
			OLED_UpdateArea(104, 48, 16, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect4--;
			if(CurrSelect4 <= 0){
				CurrSelect4 = 9;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect4++;
			if(CurrSelect4 > 9){
				CurrSelect4 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE | KEY_REPEAT)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu4_Select = CurrSelect4;
		}
		
		switch(Menu4_Select){
			case 1:
				Menu4_Select = 0;
				CurrSelect4 = 1;
				return;
			case 2:
				Menu4_Select = 0;
				MyRTC_Add_1Year();
				break;
			case 3:
				Menu4_Select = 0;
				MyRTC_Sub_1Year();
				break;
			case 4:
				Menu4_Select = 0;
				CurrSelect4 = 1;
				return;
			case 5:
				Menu4_Select = 0;
				MyRTC_Add_1Month();
				break;
			case 6:
				Menu4_Select = 0;
				MyRTC_Sub_1Month();
				break;
			case 7:
				Menu4_Select = 0;
				CurrSelect4 = 1;
				return;
			case 8:
				Menu4_Select = 0;
				MyRTC_Add_1Day();
				break;
			case 9:
				Menu4_Select = 0;
				MyRTC_Sub_1Day();
				break;
		}
	}
}

/**
  * @brief 时钟控制四级菜单，设置时间
  * @param 无
  * @retval 无
  */
void Menu4_SetTime(void){
	uint8_t Menu4_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			if(CurrSelect4>=1 && CurrSelect4<=3){
				OLED_ShowString(0, 0,  "<-           ", OLED_8X16);
				OLED_ShowString(0, 16, "时钟+1        ", OLED_8X16);
				OLED_ShowString(0, 32, "时钟-1        ", OLED_8X16);
			}else if(CurrSelect4>=4 && CurrSelect4<=6){
				OLED_ShowString(0, 0,  "<-           ", OLED_8X16);
				OLED_ShowString(0, 16, "分钟+1        ", OLED_8X16);
				OLED_ShowString(0, 32, "分钟-1        ", OLED_8X16);
			}else if(CurrSelect4>=7 && CurrSelect4<=9){
				OLED_ShowString(0, 0,  "<-           ", OLED_8X16);
				OLED_ShowString(0, 16, "秒钟+1        ", OLED_8X16);
				OLED_ShowString(0, 32, "秒钟-1        ", OLED_8X16);
			}
			OLED_ShowString(0, 48, "Time:XX:XX:XX  ", OLED_8X16);
			
			OLED_ReverseArea(0, ((CurrSelect4 - 1)*16)%48, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect4 - 1)*16)%48, 128, 16);
			OLED_ClearArea(40, 48, 16, 16);
			OLED_ClearArea(64, 48, 16, 16);
			OLED_ClearArea(88, 48, 16, 16); 
			MyRTC_ReadTime();
			
			OLED_ShowNum(40, 48, MyRTC_Time.Hour, 2, OLED_8X16);
			OLED_ShowNum(64, 48, MyRTC_Time.Minute, 2, OLED_8X16);
			OLED_ShowNum(88, 48, MyRTC_Time.Second, 2, OLED_8X16);

			OLED_ReverseArea(0, ((CurrSelect4 - 1)*48)%48, 128, 16);
			OLED_UpdateArea(40, 48, 16, 16);
			OLED_UpdateArea(64, 48, 16, 16);
			OLED_UpdateArea(88, 48, 16, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect4--;
			if(CurrSelect4 <= 0){
				CurrSelect4 = 9;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect4++;
			if(CurrSelect4 > 9){
				CurrSelect4 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE | KEY_REPEAT)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu4_Select = CurrSelect4;
		}
		
		switch(Menu4_Select){
			case 1:
				Menu4_Select = 0;
				CurrSelect4 = 1;
				return;
			case 2:
				Menu4_Select = 0;
				MyRTC_Add_1Hour();
				break;
			case 3:
				Menu4_Select = 0;
				MyRTC_Sub_1Hour();
				break;
			case 4:
				Menu4_Select = 0;
				CurrSelect4 = 1;
				return;
			case 5:
				Menu4_Select = 0;
				MyRTC_Add_1Minute();
				break;
			case 6:
				Menu4_Select = 0;
				MyRTC_Sub_1Minute();
				break;
			case 7:
				Menu4_Select = 0;
				CurrSelect4 = 1;
				return;
			case 8:
				Menu4_Select = 0;
				MyRTC_Add_1Second();
				break;
			case 9:
				Menu4_Select = 0;
				MyRTC_Sub_1Second();
				break;
		}
	}
}

/**
  * @brief 时钟控制二级菜单
  * @param 无
  * @retval 无
  */
void Menu2_Weather(void){
	uint8_t Menu2_Select = 0;
	
	OLED_ShowString(0, 0, "正在获取中……", OLED_8X16);
	OLED_Update();
	ESP8266_GetWeather();
	OLED_Clear();
	OLED_Update();
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			OLED_ShowString(0, 0,  "<-            ", OLED_8X16);
			OLED_ShowString(0, 16, "地点:福州市     ", OLED_8X16);
			OLED_ShowString(0, 32, "温度:   ℃     ", OLED_8X16);
			OLED_ShowString(0, 48, "天气:         ", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect2 - 1)*64, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*48)%64, 128, 16);
			OLED_ClearArea(40, 32, 16, 16);
			OLED_ClearArea(40, 48, 32, 16);
			
			OLED_ShowNum(40, 32, weather_temp_u8, 2, OLED_8X16);
			OLED_ShowString(40, 48, climate, OLED_8X16);

			OLED_ReverseArea(0, ((CurrSelect2 - 1)*48)%64, 128, 16);
			OLED_UpdateArea(40, 32, 16, 16);
			OLED_UpdateArea(40, 48, 32, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect2++;
			if(CurrSelect2 > 1){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				return;
		}
	}
}

/**
  * @brief 温度阈值设置二级菜单
  * @param 无
  * @retval 无
  */
void Menu2_SetTempThreshold(void){
	uint8_t Menu2_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			if(CurrSelect2 >= 1 && CurrSelect2 <= 4){
				OLED_ShowString(0, 0,  "<-             ", OLED_8X16);
				OLED_ShowString(0, 16, "5档阈值: XX.X ℃   ", OLED_8X16);
				OLED_ShowString(0, 32, "4档阈值: XX.X ℃   ", OLED_8X16);
				OLED_ShowString(0, 48, "3档阈值: XX.X ℃   ", OLED_8X16);
			}else if(CurrSelect2 >= 5 && CurrSelect2 <= 7){
				OLED_ShowString(0, 0,  "<-             ", OLED_8X16);
				OLED_ShowString(0, 16, "2档阈值: XX.X ℃   ", OLED_8X16);
				OLED_ShowString(0, 32, "1档阈值: XX.X ℃   ", OLED_8X16);
			}
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			if(CurrSelect2 >= 1 && CurrSelect2 <= 4){
				OLED_ClearArea(64, 16, 48, 16);
				OLED_ClearArea(64, 32, 48, 16);
				OLED_ClearArea(64, 48, 48, 16);
				
				OLED_ShowFloatNum(64, 16, Temp_Gear_5, 2, 1, OLED_8X16);
				OLED_ShowFloatNum(64, 32, Temp_Gear_4, 2, 1, OLED_8X16);
				OLED_ShowFloatNum(64, 48, Temp_Gear_3, 2, 1, OLED_8X16);
				
				OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
				OLED_UpdateArea(64, 16, 48, 16);
				OLED_UpdateArea(64, 32, 48, 16);
				OLED_UpdateArea(64, 48, 48, 16);
			}else if(CurrSelect2 >= 5 && CurrSelect2 <= 7){
				OLED_ClearArea(64, 16, 48, 16);
				OLED_ClearArea(64, 32, 48, 16);
				
				OLED_ShowFloatNum(64, 16, Temp_Gear_2, 2, 1, OLED_8X16);
				OLED_ShowFloatNum(64, 32, Temp_Gear_1, 2, 1, OLED_8X16);
				
				OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
				OLED_UpdateArea(64, 16, 48, 16);
				OLED_UpdateArea(64, 32, 48, 16);
			}
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 7;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect2++;
			if(CurrSelect2 > 7){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE | KEY_REPEAT)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				CurrSelect2 = 1;
				return;
			case 2:
				Menu2_Select = 0;
				Menu3_SetTempThreshold(5);
				break;
			case 3:
				Menu2_Select = 0;
				Menu3_SetTempThreshold(4);
				break;
			case 4:
				Menu2_Select = 0;
				Menu3_SetTempThreshold(3);
				return;
			case 5:
				Menu2_Select = 0;
				CurrSelect2 = 1;
				return;
			case 6:
				Menu2_Select = 0;
				Menu3_SetTempThreshold(2);
				break;
			case 7:
				Menu2_Select = 0;
				Menu3_SetTempThreshold(1);
				return;
		}
	}
}

/**
  * @brief 温度阈值设置三级菜单
  * @param 设置的温度阈值
  * @retval 无
  */
void Menu3_SetTempThreshold(uint8_t TempThresHold_x){
	uint8_t Menu3_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			if(CurrSelect3 >= 1 && CurrSelect3 <= 3){
				OLED_ShowString(0, 0,  "<-           ", OLED_8X16);
				OLED_ShowString(0, 16, "阈值+0.5        ", OLED_8X16);
				OLED_ShowString(0, 32, "阈值-0.5        ", OLED_8X16);
			}
			OLED_ShowNum(0, 48, TempThresHold_x, 1, OLED_8X16);
			OLED_ShowString(8, 48, "档阈值: XX.X ℃  ", OLED_8X16);
			
			OLED_ReverseArea(0, ((CurrSelect3 - 1)*16)%64, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect3 - 1)*16)%64, 128, 16);
			OLED_ClearArea(64, 48, 48, 16); 
			
			switch(TempThresHold_x){
				case 1:OLED_ShowFloatNum(64, 48, Temp_Gear_1, 2, 1, OLED_8X16);break;
				case 2:OLED_ShowFloatNum(64, 48, Temp_Gear_2, 2, 1, OLED_8X16);break;
				case 3:OLED_ShowFloatNum(64, 48, Temp_Gear_3, 2, 1, OLED_8X16);break;
				case 4:OLED_ShowFloatNum(64, 48, Temp_Gear_4, 2, 1, OLED_8X16);break;
				case 5:OLED_ShowFloatNum(64, 48, Temp_Gear_5, 2, 1, OLED_8X16);break;
			}

			OLED_ReverseArea(0, ((CurrSelect3 - 1)*16)%64, 128, 16);
			OLED_UpdateArea(64, 48, 48, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect3--;
			if(CurrSelect3 <= 0){
				CurrSelect3 = 3;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect3++;
			if(CurrSelect3 > 3){
				CurrSelect3 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE | KEY_REPEAT)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu3_Select = CurrSelect3;
		}
		
		switch(Menu3_Select){
			case 1:
				Menu3_Select = 0;
				CurrSelect3 = 1;
				SetTempThreshold();
				return;
			case 2:
				Menu3_Select = 0;
				TempThreshold_Add(TempThresHold_x);
				break;
			case 3:
				Menu3_Select = 0;
				TempThreshold_Sub(TempThresHold_x);
				break;
		}
	}
}

/**
  * @brief 音乐播放二级菜单
  * @param	无
  * @retval 无
  */
void Menu2_Music(){
	uint8_t Menu2_Select = 0;

	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			if(CurrSelect2 >= 1 && CurrSelect2 <= 4){
				OLED_ShowString(0, 0,  "<-              ", OLED_8X16);
				OLED_ShowString(0, 16, "播放             ", OLED_8X16);
				OLED_ShowString(0, 32, "下一首             ", OLED_8X16);
				OLED_ShowString(0, 48, "上一首           	", OLED_8X16);
			}else if(CurrSelect2 >= 5 && CurrSelect2 <= 8){
				OLED_ShowString(0, 0,  "<-              ", OLED_8X16);
				OLED_ShowString(0, 16, "设置音量          ", OLED_8X16);
				OLED_ShowString(0, 32, "设置播放顺序      ", OLED_8X16);
				OLED_ShowString(0, 48, "设置播放歌曲       ", OLED_8X16);
			}else if(CurrSelect2 >= 9 && CurrSelect2 <= 10){
				OLED_ShowString(0, 0,  "<-              ", OLED_8X16);
				OLED_ShowString(0, 16, "       ", OLED_8X16);
				OLED_ShowString(0, 32, "                ", OLED_8X16);
				OLED_ShowString(0, 48, "                ", OLED_8X16);
			}
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
			OLED_Update();
			CurrState = 1;
		}
		else{
			if(CurrSelect2 >= 1 && CurrSelect2 <= 4){
				OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
				OLED_ClearArea(0, 16, 32, 16);
				if(Music_IsOn == 1){//正在播放
					OLED_ShowString(0, 16, "暂停             ", OLED_8X16);
				}else{//正在暂停
					OLED_ShowString(0, 16, "播放             ", OLED_8X16);
				}
				OLED_ReverseArea(0, ((CurrSelect2 - 1)*16)%64, 128, 16);
				OLED_UpdateArea(0, 16, 32, 16);
			}
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 8;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			CurrSelect2++;
			if(CurrSelect2 > 8){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect2-1)*16)%64, 128, 16);
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				CurrSelect2 = 1;
				return;
			case 2:	
				Serial_ClearTxBuffer();
				if(Music_IsOn == 1){//正在播放
					Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x0E, 0x00, 0x00, 0x00, 0xEF);//暂停
					Music_IsOn = 0;
				}else{//未在播放
					Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x0D, 0x00, 0x00, 0x00, 0xEF);//播放
					Music_IsOn = 1;
				}
				Serial_SendPacket(1, 8);
				Menu2_Select = 0;
				break;
			case 3:	//下一首
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x01, 0x00, 0x00, 0x00, 0xEF);//下一首
				Serial_SendPacket(1, 8);
				Menu2_Select = 0;
				break;
			case 4:	//上一首
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x02, 0x00, 0x00, 0x00, 0xEF);//上一首
				Serial_SendPacket(1, 8);
				Menu2_Select = 0;
				break;
			case 5:
				Menu2_Select = 0;
				CurrSelect2 = 1;
			case 6:
				Menu2_Select = 0;
				Menu3_SetMusicVolume();
				break;
			case 7:
				Menu2_Select = 0;
				Menu3_SetMusicOrder();
				break;
			case 8:
				Menu2_Select = 0;
				Menu3_SetMusic();
				break;
		}
	}
}

/**
  * @brief 音乐音量设置三级菜单
  * @param 无
  * @retval 无
  */
void Menu3_SetMusicVolume(void){
	uint8_t Menu3_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			OLED_ShowString(0, 0,  "<-  当前音量:      ", OLED_8X16);
			OLED_ShowString(0, 16, "音量+1            ", OLED_8X16);
			OLED_ShowString(0, 32, "音量-1            ", OLED_8X16);
			OLED_ShowString(0, 48, "                 ", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect3 - 1)*16, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, (CurrSelect3 - 1)*16, 128, 16);
			OLED_ClearArea(112, 0, 16, 16);
			OLED_ShowNum(112, 0, MusicVolume, 2, OLED_8X16);
			OLED_ReverseArea(0, (CurrSelect3 - 1)*16, 128, 16);
			OLED_UpdateArea(112, 0, 16, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect3--;
			if(CurrSelect3 <= 0){
				CurrSelect3 = 4;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect3++;
			if(CurrSelect3 > 4){
				CurrSelect3 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE | KEY_REPEAT)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu3_Select = CurrSelect3;
		}
		
		switch(Menu3_Select){
			case 1:
				Menu3_Select = 0;
				CurrSelect3 = 1;
				return;
			case 2:
				Menu3_Select = 0;
				if(MusicVolume < 30){
					MusicVolume++;
				}
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, MusicVolume, 0xEF);
				Serial_SendPacket(1, 8);
				break;
			case 3:
				Menu3_Select = 0;
				if(MusicVolume > 1){
					MusicVolume--;
				}
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, MusicVolume, 0xEF);
				Serial_SendPacket(1, 8);
				break;
		}
	}
}

/**
  * @brief 音乐播放顺序设置三级菜单
  * @param 无
  * @retval 无
  */
void Menu3_SetMusicOrder(void){
	uint8_t Menu3_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			OLED_ShowString(0, 0,  "<-            ", OLED_8X16);
			OLED_ShowString(0, 16, "单曲循环        ", OLED_8X16);
			OLED_ShowString(0, 32, "顺序播放        ", OLED_8X16);
			OLED_ShowString(0, 48, "随机播放        ", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect3 - 1)*16, 128, 16);
			OLED_Update();
			CurrState = 1;
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect3--;
			if(CurrSelect3 <= 0){
				CurrSelect3 = 4;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect3++;
			if(CurrSelect3 > 4){
				CurrSelect3 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu3_Select = CurrSelect3;
		}
		
		switch(Menu3_Select){
			case 1:
				Menu3_Select = 0;
				CurrSelect3 = 1;
				return;
			case 2:
				Menu3_Select = 0;
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x19, 0x00, 0x00, 0x00, 0xEF);//单曲循环开启
				Serial_SendPacket(1, 8);
				break;
			case 3:
				Menu3_Select = 0;
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x11, 0x00, 0x00, 0x01, 0xEF);//顺序播放
				Serial_SendPacket(1, 8);
				break;
			case 4:
				Menu3_Select = 0;
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x18, 0x00, 0x00, 0x00, 0xEF);//随机播放
				Serial_SendPacket(1, 8);
				break;
		}
	}
}

/**
  * @brief 播放歌曲设置三级菜单
  * @param 无
  * @retval 无
  */
void Menu3_SetMusic(void){
	uint8_t Menu3_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			OLED_ShowString(0, 0,  "<-             ", OLED_8X16);
			OLED_ShowString(0, 16, "周杰伦          ", OLED_8X16);
			OLED_ShowString(0, 32, "林俊杰          ", OLED_8X16);
			OLED_ShowString(0, 48, "王力宏          ", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect3 - 1)*16, 128, 16);
			OLED_Update();
			CurrState = 1;
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect3--;
			if(CurrSelect3 <= 0){
				CurrSelect3 = 4;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect3++;
			if(CurrSelect3 > 4){
				CurrSelect3 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu3_Select = CurrSelect3;
		}
		
		switch(Menu3_Select){
			case 1:
				Menu3_Select = 0;
				CurrSelect3 = 1;
				return;
			case 2:
				Menu3_Select = 0;
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x17, 0x00, 0x00, 0x01, 0xEF);//周杰伦
				Serial_SendPacket(1, 8);
				break;
			case 3:
				Menu3_Select = 0;
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x17, 0x00, 0x00, 0x02, 0xEF);//林俊杰
				Serial_SendPacket(1, 8);
				break;
			case 4:
				Menu3_Select = 0;
				Serial_ClearTxBuffer();
				Serial_SetTxDataPacket(8, 0x7E, 0xFF, 0x06, 0x17, 0x00, 0x00, 0x03, 0xEF);//王力宏
				Serial_SendPacket(1, 8);
				break;
		}
	}
}

/**
  * @brief 设置语音音量二级菜单
  * @param 无
  * @retval 无
  */
void Menu2_SetVoiceVolume(void){
	uint8_t Menu2_Select = 0;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			
			OLED_ShowString(0, 0,  "<-   当前音量:     ", OLED_8X16);
			OLED_ShowString(0, 16, "音量+1            ", OLED_8X16);
			OLED_ShowString(0, 32, "音量-1            ", OLED_8X16);
			OLED_ShowString(0, 48, "                 ", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect2 - 1)*16, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, (CurrSelect2 - 1)*16, 128, 16);
			OLED_ClearArea(120, 0, 8, 16);
			OLED_ShowNum(120, 0, VoiceVolume, 1, OLED_8X16);
			OLED_ReverseArea(0, (CurrSelect2 - 1)*16, 128, 16);
			OLED_UpdateArea(120, 0, 8, 16);
		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 3;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect2++;
			if(CurrSelect2 > 3){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE | KEY_REPEAT)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				CurrSelect2 = 1;
				return;
			case 2://音量+1
				Menu2_Select = 0;
				if(VoiceVolume < 7){
					VoiceVolume++;
					Volume_Up();
				}else{
					Volume_Max();
				}
				break;
			case 3://音量-1
				Menu2_Select = 0;
				if(VoiceVolume > 1){
					VoiceVolume--;
					Volume_Down();
				}else{
					Volume_Min();
				}
				break;
		}
	}
}

/**
  * @brief 调试二级菜单
  * @param 无
  * @retval 无
  */
void Menu2_Debug(void){
	uint8_t Menu2_Select = 0;
	float distance;
	
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			
			OLED_ShowString(0, 0,  "<-               ", OLED_8X16);
			OLED_ShowString(0, 16, "Temp:XX.X℃      ", OLED_8X16);
			OLED_ShowString(0, 32, "Distance:XX.Xcm  ", OLED_8X16);
			OLED_ShowString(0, 48, "Gear:            ", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect2 - 1)*16, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, (CurrSelect2 - 1)*16, 128, 16);
			OLED_ClearArea(40, 16, 32, 16);
			OLED_ClearArea(64, 32, 32, 16);
			
			if(Temp2Gear){
				OLED_ShowFloatNum(40, 16, Temp, 2, 1, OLED_8X16);  // 实时刷新温度
				OLED_ShowString(24, 0, "√", OLED_8X16);
			}	
			else{
				OLED_ShowString(40, 16, "--.-", OLED_8X16);
				OLED_ShowString(24, 0, "×", OLED_8X16);
			}
			OLED_ShowNum(40, 48, Gear, 1, OLED_8X16);
			
			distance = HC_SR04_GetDistance();
			OLED_ShowFloatNum(64, 32, distance, 2, 1, OLED_8X16);
			OLED_ShowChar(64, 32, ':', OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect2 - 1)*16, 128, 16);

			
			OLED_Update();
			
			Delay_ms(200);

		}
		
		if(Key_Check(KEY_1, KEY_SINGLE)){//上一项
			CurrState = 0;
			CurrSelect2--;
			if(CurrSelect2 <= 0){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			CurrSelect2++;
			if(CurrSelect2 > 1){
				CurrSelect2 = 1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_Clear();
			OLED_Update();
			Menu2_Select = CurrSelect2;
		}
		
		switch(Menu2_Select){
			case 1:
				Menu2_Select = 0;
				CurrSelect2 = 1;
				return;
		}
	}
}
