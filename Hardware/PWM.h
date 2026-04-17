#ifndef __PWM_H
#define	__PWM_H

#define GPIO_PIN_PWMA GPIO_Pin_7
#define GPIO_PORT_PWMA GPIOA

void PWM_Init(void);
void PWM_SetCompare2(uint16_t Compare);

#endif
