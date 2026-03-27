#ifndef __WIFI_H
#define __WIFI_H

extern long long int time;
extern uint16_t code;
extern uint8_t weather_temp_u8;
extern char climate[10];

void WIFI_Init(void);
void WIFI_GetTime(void);
void WIFI_GetWeather(void);
void Weather_Parse(void);


#endif
