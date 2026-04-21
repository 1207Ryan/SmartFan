#ifndef __MENU_H
#define __MENU_H

#define TotalSelect 7

extern uint8_t CurrSelect1;
extern uint8_t CurrSelect2;
extern uint8_t CurrSelect3;
extern uint8_t CurrSelect4;
extern uint8_t CurrState;

extern uint8_t Motor_Speed[6];      // 各档对应电机转速
extern float Temp_Threshold[6];
extern float Safe_Distance;

uint8_t Menu1(void);
void Menu2_Temp(void);
void Menu2_Fan(void);
void Menu2_CountDown(void);
void Menu2_Clock(void);
void Menu2_Weather(void);
void Menu2_Music(void);
void Menu2_Settings(void);
void Menu2_Debug(void);

void Menu3_CountDown(void);
void Menu3_SetDateAndTime(void);

void Menu3_SetMusicVolume(void);
void Menu3_SetMusicOrder(void);
void Menu3_SetMusic(void);
void Menu3_SetMotorSpeed(void);
void Menu3_SetTempThreshold(void);
void Menu3_SetVoiceVolume(void);
void Menu3_SetSafeDistance(void);

void Menu4_SetDate(void);
void Menu4_SetTime(void);
void Menu4_SetMotorSpeed(uint8_t Gear_x);
void Menu4_SetTempThreshold(uint8_t Temp_Gear_x);

#endif
