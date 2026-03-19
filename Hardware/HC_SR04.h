#ifndef __HC_SR04_H
#define __HC_SR04_H

// 固定引脚：Trig=PA11，Echo=PA12
#define TRIG_PIN     GPIO_Pin_11
#define TRIG_PORT    GPIOA
#define ECHO_PIN     GPIO_Pin_12
#define ECHO_PORT    GPIOA

#define SATE_DISTANCE 5.0f

// 函数声明
void HC_SR04_Init(void);        // 初始化HC-SR04
float HC_SR04_GetDistance(void); // 获取测距结果（单位：cm）
void Distance_Detect_Start(void);
void Distance_Detect_Stop(void);

#endif
