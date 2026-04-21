#ifndef __AD_H
#define __AD_H

#define GPIO_ADC_PIN GPIO_Pin_0
#define GPIO_ADC_PORT GPIOA
#define ADC_CHANNEL ADC_Channel_0

// ===== 硬件参数配置=====
#define NTC_R_REF    10000.0f  // 分压电阻值
#define NTC_B_VALUE  3950.0f   // NTC的B值
#define NTC_R_25     10000.0f  // 25℃时NTC阻值
#define ADC_MAX      4095.0f   // 12位ADC最大值
#define VCC          3.3f      // 供电电压
#define WINDOW_SIZE  8         // 滑动窗口长度（建议8-12，越小响应越快）

extern uint16_t AD_Value;

extern float Temp_Threshold[6];

void AD_Init(void);
void AD_Collect_Start(void);
void AD_Collect_Stop(void);
float AD_to_Temp(void);
void TempThreshold_Add(uint8_t TempThresHold_x);
void TempThreshold_Sub(uint8_t TempThresHold_x);
void TempThreshold_Set(uint8_t TempThresHold_x, float Threshold);

#endif
