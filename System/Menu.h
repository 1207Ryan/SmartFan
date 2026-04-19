#ifndef __MENU_H
#define __MENU_H

#define TotalSelect 7

extern uint8_t CurrSelect1;
extern uint8_t CurrSelect2;
extern uint8_t CurrSelect3;
extern uint8_t CurrSelect4;
extern uint8_t CurrState;

extern uint8_t Motor_Speed_0;      // 0档对应电机转速
extern uint8_t Motor_Speed_1;      // 1档对应电机转速
extern uint8_t Motor_Speed_2;      // 2档对应电机转速
extern uint8_t Motor_Speed_3;      // 3档对应电机转速
extern uint8_t Motor_Speed_4;      // 4档对应电机转速
extern uint8_t Motor_Speed_5;      // 5档对应电机转速

extern float Temp_Gear_1;
extern float Temp_Gear_2;
extern float Temp_Gear_3;
extern float Temp_Gear_4;
extern float Temp_Gear_5;

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
