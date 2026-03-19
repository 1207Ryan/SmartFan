#ifndef __MENU_H
#define __MENU_H

#define TotalSelect	    5
#define TEMP_GEAR_1    22.0f  // 1档温度阈值
#define TEMP_GEAR_2    23.0f  // 2档温度阈值
#define TEMP_GEAR_3    24.0f  // 3档温度阈值
#define TEMP_GEAR_4    25.0f  // 4档温度阈值
#define TEMP_GEAR_5    26.0f  // 5档温度阈值

extern uint8_t CurrSelect1;
extern uint8_t CurrSelect2;
extern uint8_t CurrSelect3;
extern uint8_t CurrState;

uint8_t Menu1(void);
void Menu2_Temp(void);
void Menu2_Fan(void);
void Menu2_Music(void);
void Menu2_Clock(void);
void Menu2_Debug(void);


#endif
