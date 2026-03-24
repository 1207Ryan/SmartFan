#ifndef __MOTOR_H
#define __MOTOR_H

#define MOTOR_SPEED_0  0       // 0档对应电机转速
#define MOTOR_SPEED_1  20      // 1档对应电机转速
#define MOTOR_SPEED_2  40      // 2档对应电机转速
#define MOTOR_SPEED_3  60      // 3档对应电机转速
#define MOTOR_SPEED_4  80      // 4档对应电机转速
#define MOTOR_SPEED_5  100     // 5档对应电机转速

void Motor_Init(void);
void Motor_SetSpeed(int8_t speed);
void Motor_SetGear(uint8_t Gear);
void Motor_Stop(void);

#endif
