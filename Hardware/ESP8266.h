#ifndef __ESP8266_H
#define __ESP8266_H

extern uint8_t weather_code;
extern uint8_t weather_temp_u8;
extern char climate[10];

void ESP8266_Init(void);
void ESP8266_GetTime(void);
void ESP8266_GetWeather(void);
void Weather_Parse(void);

#endif
