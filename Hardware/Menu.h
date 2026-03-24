#ifndef __MENU_H
#define __MENU_H

#define TotalSelect	    6
#define TEMP_GEAR_1    20.0f  // 1档温度阈值
#define TEMP_GEAR_2    22.0f  // 2档温度阈值
#define TEMP_GEAR_3    24.0f  // 3档温度阈值
#define TEMP_GEAR_4    26.0f  // 4档温度阈值
#define TEMP_GEAR_5    28.0f  // 5档温度阈值

extern uint8_t CurrSelect1;
extern uint8_t CurrSelect2;
extern uint8_t CurrSelect3;
extern uint8_t CurrSelect4;
extern uint8_t CurrState;

uint8_t Menu1(void);
void Menu2_Temp(void);
void Menu2_Fan(void);
void Menu2_CountDown(void);
void Menu2_Clock(void);
void Menu2_Debug(void);

void Menu3_CountDown(void);
void Menu3_SetDateAndTime(void);

void Menu4_SetDate(void);
void Menu4_SetTime(void);

#endif
