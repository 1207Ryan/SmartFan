#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Menu.h"
#include "LED.h"
#include "Motor.h"
#include "MyRTC.h"
#include "AD.h"
#include "HC_SR04.h"
#include "Timer.h"

uint8_t CurrSelect1 = 1;
uint8_t CurrSelect2 = 1;
uint8_t CurrSelect3 = 1;
uint8_t CurrState = 0;
volatile uint8_t Gear;

volatile float Temp;
volatile uint8_t Temp2Gear = 0;
extern volatile uint8_t IsSafe;

uint8_t Menu1(void){
	while(1){
		if(CurrState == 0){
			OLED_Clear();
			if(CurrSelect1>=1 && CurrSelect1<=4){
				OLED_ShowString(0, 0,  "温度调节档位          ", OLED_8X16);
				OLED_ShowString(0, 16, "风扇档位控制          ", OLED_8X16);
				OLED_ShowString(0, 32, "时间显示             ", OLED_8X16);
				OLED_ShowString(0, 48, "设置                ", OLED_8X16);
			}
			else if(CurrSelect1 == 5 ){
				OLED_ShowString(0, 0,  "调试             ", OLED_8X16);
				OLED_ShowString(0, 16, "                ", OLED_8X16);
				OLED_ShowString(0, 32, "                ", OLED_8X16);
				OLED_ShowString(0, 48, "                ", OLED_8X16);
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
				CurrSelect1=TotalSelect;
			}
		}else if(Key_Check(KEY_2, KEY_SINGLE)){//下一项
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect1-1)*16)%64, 128, 16);
			CurrSelect1++;
			if(CurrSelect1 > TotalSelect){
				CurrSelect1=1;
			}
		}else if(Key_Check(KEY_3, KEY_SINGLE)){//确认
			CurrState = 0;
			OLED_ReverseArea(0, ((CurrSelect1-1)*16)%64, 128, 16);
			OLED_Clear();
			OLED_Update();
			return CurrSelect1;
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
			OLED_ShowString(0, 0, "<-         ℃ 档", OLED_8X16);
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
				break;
			case 2:		//启动温度调节档位
				AD_Collect_Start();
				Temp2Gear = 1;
				Last_Gear = 0;
				Menu2_Select = 0;
				break;
			case 3:		//停止风扇
				AD_Collect_Stop();
				Temp = 0;
				Temp2Gear = 0;
				Gear = 0;
				Last_Gear = 0;
				Motor_Stop();
				Menu2_Select = 0;
				break;
//			case 4:		
//				Gear = 0;
//				Motor_SetSpeed(0);
//				Menu2_Select = 0;
//				break;
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
				return;
				Menu2_Select = 0;
				break;
			case 2:		//升档
				Temp2Gear = 0;
				if(Gear < 5){
					Gear++;
				}
				Motor_SetSpeed(Gear * 20);
				Menu2_Select = 0;
				break;
			case 3:		//降档
				Temp2Gear = 0;
				if(Gear > 0){
					Gear--;
				}
				Motor_SetSpeed(Gear * 20);
				Menu2_Select = 0;
				break;
			case 4:		//停止
				Temp2Gear = 0;
				Gear = 0;
				Last_Gear = 0;
				Motor_Stop();
				Menu2_Select = 0;
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
			OLED_ShowString(0, 0, "<-               ", OLED_8X16);
			OLED_ShowString(0, 16, "Date:XXXX-XX-XX", OLED_8X16);
			OLED_ShowString(0, 32, "Time:XX:XX:XX  ", OLED_8X16);
			OLED_ShowString(0, 48, "            设置", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect2 - 1)*48, 128, 16);
			OLED_Update();
			CurrState = 1;
		}else{
			OLED_ReverseArea(0, ((CurrSelect2 - 1)*48)%64, 128, 16);
			OLED_ClearArea(40, 16, 32, 16);
			OLED_ClearArea(80, 16, 16, 16);
			OLED_ClearArea(104, 16, 16, 16);
			OLED_ClearArea(40, 32, 16, 16);
			OLED_ClearArea(64, 32, 16, 16);
			OLED_ClearArea(88, 32, 16, 16); 
			MyRTC_ReadTime();
			
			OLED_ShowNum(40, 16, MyRTC_Time.Year, 4, OLED_8X16);
			OLED_ShowNum(80, 16, MyRTC_Time.Month, 2, OLED_8X16);
			OLED_ShowNum(104, 16, MyRTC_Time.Day, 2, OLED_8X16);
			OLED_ShowNum(40, 32, MyRTC_Time.Hour, 2, OLED_8X16);
			OLED_ShowNum(64, 32, MyRTC_Time.Minute, 2, OLED_8X16);
			OLED_ShowNum(88, 32, MyRTC_Time.Second, 2, OLED_8X16);

			OLED_ReverseArea(0, ((CurrSelect2 - 1)*48)%64, 128, 16);
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
				return;
				Menu2_Select = 0;
				break;
			case 2:
				Menu2_Select = 0;
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
				return;
				Menu2_Select = 0;
				break;
		}
	}
}
