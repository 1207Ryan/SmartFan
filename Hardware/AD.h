#ifndef __AD_H
#define __AD_H

extern uint16_t AD_Value;

void AD_Init(void);
void AD_Collect_Start(void);
void AD_Collect_Stop(void);
float AD_to_Temp(void);

#endif
