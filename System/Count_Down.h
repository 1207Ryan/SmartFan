#ifndef __COUNT_DOWN_H
#define __COUNT_DOWN_H

void Count_Down_Init(void);
void Count_Add_1s(void);
void Count_Sub_1s(void);
void Count_Add_1m(void);
void Count_Sub_1m(void);
void Count_Add_1h(void);
void Count_Sub_1h(void);
void Count_Start(void);
void Count_Stop(void);
uint32_t Get_Count(void);

#endif
