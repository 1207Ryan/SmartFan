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

uint8_t CurrSelect1 = 1;
uint8_t CurrSelect2 = 1;
uint8_t CurrSelect3 = 1;
uint8_t CurrSelect4 = 1;
uint8_t CurrState = 0;

volatile uint8_t Working = 0;
volatile uint8_t Gear;
volatile uint8_t Last_Gear;
volatile float Temp;
volatile uint8_t Temp2Gear = 0;
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
			else if(CurrSelect1 >=5 && CurrSelect1 <= 6){
				OLED_ShowString(0, 0,  "调试             ", OLED_8X16);
				OLED_ShowString(0, 16, "设置             ", OLED_8X16);
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
				Serial_SendByte(0x01);
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
				Serial_SendByte(0x02);
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
				Menu2_Select = 0;
				return;
			case 2:		//升档
				Temp2Gear = 0;
				Working = 1;
				AD_Collect_Stop();
				if(Gear < 5){
					Gear++;
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				Serial_SendByte(0x11);
				Menu2_Select = 0;
				break;
			case 3:		//降档
				Temp2Gear = 0;
				AD_Collect_Stop();
				if(Gear > 0){
					Gear--;
				}
				Last_Gear = Gear;
				Motor_SetGear(Gear);
				if(Gear == 0) Working = 0;
				Serial_SendByte(0x12);
				Menu2_Select = 0;
				break;
			case 4:		//停止
				AD_Collect_Stop();
				Temp2Gear = 0;
				Gear = 0;
				Last_Gear = 0;
				Motor_Stop();
				Working = 0;
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
				OLED_ShowString(0, 32, "    倒计时          ", OLED_8X16);
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
			OLED_ShowString(0, 16, "设置日期        ", OLED_8X16);
			OLED_ShowString(0, 32, "设置时间        ", OLED_8X16);
			OLED_ShowString(0, 48, "              ", OLED_8X16);
			
			OLED_ReverseArea(0, (CurrSelect3 - 1)*16, 128, 16);
			OLED_Update();
			CurrState = 1;
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
				Menu4_SetDate();
				break;
			case 3:
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
				return;
		}
	}
}
