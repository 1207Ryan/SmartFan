#include "stm32f10x.h"                  // Device header
#include "MyRTC.h"
#include <time.h>

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
		MyRTC_Time.Day = 24;
		MyRTC_Time.Hour = 0;
		MyRTC_Time.Minute = 0;
		MyRTC_Time.Second = 0;
		
		MyRTC_SetTime();
		
		MyRTC_Time.Weekday = MyRTC_GetWeekday(); 
		
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
	MyRTC_Time.Weekday = MyRTC_CalcWeekday(MyRTC_Time.Year, MyRTC_Time.Month, MyRTC_Time.Day);
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

static uint8_t MyRTC_IsLeapYear(uint16_t year)
{
    return (year%4==0 && year%100!=0) || (year%400==0);
}

/**
  * @brief  基姆拉尔森公式计算星期几（适配1=周一，7=周日）
  * @param  year：4位年份（如2026）
  * @param  month：月份（1-12）
  * @param  day：日期（1-31）
  * @retval 1=周一，2=周二...0=周日
  */
uint8_t MyRTC_CalcWeekday(uint16_t year, uint8_t month, uint8_t day)
{
    // 1. 基础合法性校验
    if(year < 1970 || month < 1 || month > 12 || day < 1 || day > 31)
    {
        return 0; // 无效日期
    }
    
    // 2. 月份天数校验（含闰年）
    uint8_t max_day[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if(month == 2 && MyRTC_IsLeapYear(year))
    {
        if(day > 29) return 0;
    }
    else if(day > max_day[month])
    {
        return 0;
    }
    
    // 3. 修正1/2月为上一年的13/14月（公式要求）
    uint16_t y = year;
    uint8_t m = month;
    if(m == 1 || m == 2)
    {
        y -= 1;
        m += 12;
    }
    
    // 4. 基姆拉尔森公式计算（原结果0=周一，6=周日）
    int w = (day + 2*m + 3*(m+1)/5 + y + y/4 - y/100 + y/400 + 1) % 7;
    
    // 5. 映射为1=周一，2=周二...7=周日
    switch(w)
    {
        case 0: return 0;  // 周日
        case 1: return 1;  // 周一
        case 2: return 2;  // 周二
        case 3: return 3;  // 周三
        case 4: return 4;  // 周四
        case 5: return 5;  // 周五
        case 6: return 6;  // 周六
        default: return 9; // 异常情况返回0
    }
}

// 封装：读取当前RTC时间并计算星期几
uint8_t MyRTC_GetWeekday(void)
{
    MyRTC_ReadTime(); // 先更新MyRTC_Time的年月日
    return MyRTC_CalcWeekday(MyRTC_Time.Year, MyRTC_Time.Month, MyRTC_Time.Day);
}


