#ifndef __HC_04_H
#define __HC_04_H

void HC_04_Init(void);
void HC_04_Detect(void);
void BlueTooth_Fan_On(void);
void BlueTooth_Fan_Off(void);
void BlueTooth_Fan_Gear_Up(void);
void BlueTooth_Fan_Gear_Down(void);
void BlueTooth_Set_Countdown(void);
void BlueTooth_Start_Countdown(void);
void BlueTooth_Stop_Countdown(void);
void BlueTooth_SetMotorSpeed(void);
void BlueTooth_SetTempThreshold(void);
void BlueTooth_SetMusicVolume(uint8_t MusicVolume);
void BlueTooth_SendString(char *Str);

#endif
