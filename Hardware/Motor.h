#ifndef __MOTOR_H
#define __MOTOR_H

#define GPIO_PIN_AIN1 GPIO_Pin_1
#define GPIO_PIN_AIN2 GPIO_Pin_0
#define GPIO_PORT_AIN GPIOB

extern uint8_t Motor_Speed[6];

void Motor_Init(void);
void Motor_SetSpeed(int8_t speed);
void Motor_SetGear(uint8_t Gear);
void Motor_Stop(void);
void Motor_Speed_Add(uint8_t Gear_x);
void Motor_Speed_Sub(uint8_t Gear_x);
void Motor_Speed_Set(uint8_t Gear_x, uint8_t Speed);

#endif
