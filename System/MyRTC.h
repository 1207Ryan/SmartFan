#ifndef __MYRTC_H
#define __MYRTC_H

typedef struct{
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Weekday;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
}MyTime;
extern MyTime MyRTC_Time;

void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);

void MyRTC_Add_1Year(void);
void MyRTC_Sub_1Year(void);
void MyRTC_Add_1Month(void);
void MyRTC_Sub_1Month(void);
void MyRTC_Add_1Day(void);
void MyRTC_Sub_1Day(void);
void MyRTC_Add_1Hour(void);
void MyRTC_Sub_1Hour(void);
void MyRTC_Add_1Minute(void);
void MyRTC_Sub_1Minute(void);
void MyRTC_Add_1Second(void);
void MyRTC_Sub_1Second(void);
uint32_t MyRTC_GetCurrentSec(void);
uint8_t MyRTC_CalcWeekday(uint16_t year, uint8_t month, uint8_t day);
uint8_t MyRTC_GetWeekday(void);

#endif
