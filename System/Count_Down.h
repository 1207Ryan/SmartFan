#ifndef __COUNT_DOWN_H
#define __COUNT_DOWN_H

void Count_Down_Init(void);
void Count_Set_0(void);
void Count_Add_1s(void);
void Count_Sub_1s(void);
void Count_Add_Second(uint8_t Second);
void Count_Add_1m(void);
void Count_Sub_1m(void);
void Count_Add_Minute(uint8_t Minute);
void Count_Add_1h(void);
void Count_Sub_1h(void);
void Count_Add_Hour(uint8_t Hour);
void Count_Start(void);
void Count_Stop(void);
uint32_t Get_Count(void);
void Count_Down_Over(void);

#endif
