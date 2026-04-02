#ifndef __MENU_H
#define __MENU_H

#define TotalSelect 8

extern uint8_t CurrSelect1;
extern uint8_t CurrSelect2;
extern uint8_t CurrSelect3;
extern uint8_t CurrSelect4;
extern uint8_t CurrState;

extern float Temp_Gear_1;
extern float Temp_Gear_2;
extern float Temp_Gear_3;
extern float Temp_Gear_4;
extern float Temp_Gear_5;

uint8_t Menu1(void);
void Menu2_Temp(void);
void Menu2_Fan(void);
void Menu2_CountDown(void);
void Menu2_Clock(void);
void Menu2_Weather(void);
void Menu2_SetTempThreshold(void);
void Menu2_Music(void);
void Menu2_Debug(void);

void Menu3_CountDown(void);
void Menu3_SetDateAndTime(void);
void Menu3_SetTempThreshold(uint8_t Temp_Gear_x);
void Menu3_SetMusicVolume(void);
void Menu3_SetMusicOrder(void);
void Menu3_SetMusic(void);

void Menu4_SetDate(void);
void Menu4_SetTime(void);

#endif
