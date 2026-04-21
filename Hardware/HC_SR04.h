#ifndef __HC_SR04_H
#define __HC_SR04_H

// 固定引脚：Trig=PA5，Echo=PA4
#define TRIG_PIN     GPIO_Pin_5
#define TRIG_PORT    GPIOA
#define ECHO_PIN     GPIO_Pin_4
#define ECHO_PORT    GPIOA

// 函数声明
void HC_SR04_Init(void);        // 初始化HC-SR04
float HC_SR04_GetDistance(void); // 获取测距结果（单位：cm）
void Safe_Distance_Add(void);
void Safe_Distance_Sub(void);
void Safe_Distance_Set(float Distance);

#endif
