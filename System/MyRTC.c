#include "stm32f10x.h"                  // Device header
#include <time.h>

void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);

typedef struct{
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
}MyTime;

MyTime MyRTC_Time;

void MyRTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_BKPEN, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWREN, ENABLE);

	PWR_BackupAccessCmd(ENABLE);
	
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xDEAD){
		RCC_LSEConfig(RCC_LSE_ON);
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		
		RTC_SetPrescaler(32768-1);
		RTC_WaitForLastTask();
		
		/*
		RTC_SetCounter(1740758400);
		RTC_WaitForLastTask();
		*/

		MyRTC_Time.Year = 2026;
		MyRTC_Time.Month = 3;
		MyRTC_Time.Day = 15;
		MyRTC_Time.Hour = 12;
		MyRTC_Time.Minute = 0;
		MyRTC_Time.Second = 0;
		
		MyRTC_SetTime();
		
		BKP_WriteBackupRegister(BKP_DR1, 0xDEAD);
	}else{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
}

void MyRTC_SetTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_date.tm_year = MyRTC_Time.Year - 1900;
	time_date.tm_mon = MyRTC_Time.Month - 1;
	time_date.tm_mday = MyRTC_Time.Day;
	time_date.tm_hour = MyRTC_Time.Hour;
	time_date.tm_min = MyRTC_Time.Minute;
	time_date.tm_sec = MyRTC_Time.Second;

	time_cnt = mktime(&time_date) - 8*60*60;
	RTC_SetCounter(time_cnt);
	RTC_WaitForLastTask();
}

void MyRTC_ReadTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
}

void MyRTC_Add_1Year(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900 + 1;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Sub_1Year(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900 - 1;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Add_1Month(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1 + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Sub_1Month(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1 - 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Add_1Day(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday + 1;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Sub_1Day(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday - 1;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Add_1Hour(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour + 1;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Sub_1Hour(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour - 1;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Add_1Minute(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min + 1;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Sub_1Minute(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min - 1;
	MyRTC_Time.Second = time_date.tm_sec;
	
	MyRTC_SetTime();
}

void MyRTC_Add_1Second(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec + 1;
	
	MyRTC_SetTime();
}

void MyRTC_Sub_1Second(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_cnt += 8*60*60;
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.Year = time_date.tm_year + 1900;
	MyRTC_Time.Month= time_date.tm_mon + 1;
	MyRTC_Time.Day = time_date.tm_mday;
	MyRTC_Time.Hour = time_date.tm_hour;
	MyRTC_Time.Minute = time_date.tm_min;
	MyRTC_Time.Second = time_date.tm_sec - 1;
	
	MyRTC_SetTime();
}

uint32_t MyRTC_GetCurrentSec(void)
{
    // RTC_GetCounter()返回的是减8小时后的秒数，加8小时还原东八区
    return RTC_GetCounter() + 8 * 60 * 60;
}

