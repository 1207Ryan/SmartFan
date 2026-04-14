#include "stm32f10x.h"                  // Device header
#include "HC_SR04.h"
#include "Delay.h"

/*
Trig - PA5
Echo - PA4
*/

// 全局变量（仅本文件可见）
static uint32_t g_Echo_Time = 0;  // Echo脉冲宽度（us，扩大为32位防溢出）
static uint16_t g_Tim4MsCount = 0;// TIM4 1ms中断计数（兼容辰哥逻辑，防溢出）
static uint8_t  g_Echo_Flag = 0;  // 测距完成标志

// 滑动窗口相关变量（3次滑动平均）
#define WINDOW_SIZE 3  
static float g_DistanceWindow[WINDOW_SIZE] = {0}; 
static uint8_t g_WindowIndex = 0; 

// 声速校准：根据实际室温调整（20℃=0.01717，25℃=0.0173）
#define SOUND_SPEED_COEFF 0.01717f
// 有效测距范围（us）：1cm≈29us，1m≈5800us
#define MIN_ECHO_TIME 29
#define MAX_ECHO_TIME 5800

/**
 * @brief TIM4中断服务函数（1ms累加）
 */
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        g_Tim4MsCount++;                // 每1ms计数+1
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

/**
 * @brief 初始化TIM4（1us计数+1ms中断，双重计时防溢出）
 */
static void TIM4_TimeInit(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1; // 1ms中断
    TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1; // 
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

    // 使能TIM4更新中断
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    // 禁用通道输出，避冲突
    TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Disable);
    TIM_CCxCmd(TIM4, TIM_Channel_2, TIM_CCx_Disable);
    TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Disable);
    TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Disable);

    TIM_Cmd(TIM4, DISABLE);
}

/**
 * @brief 初始化Echo引脚外部中断（PA12 → EXTI12）
 */
static void ECHO_EXTI_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = ECHO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(ECHO_PORT, &GPIO_InitStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);

    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = EXTI_Line4;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXTI_InitStruct);

    // NVIC配置
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//已有
	
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);

    // TIM4中断优先级（次高）
    NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
}

/**
 * @brief 初始化HC-SR04（核心修复：分组移到main，校准参数）
 */
void HC_SR04_Init(void)
{
    // 1. Trig引脚初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = TRIG_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TRIG_PORT, &GPIO_InitStruct);
    GPIO_ResetBits(TRIG_PORT, TRIG_PIN);

    // 2. TIM4初始化（含中断）
    TIM4_TimeInit();

    // 3. EXTI初始化
    ECHO_EXTI_Init();

    // 4. 滑动窗口初始化
    for(uint8_t i=0; i<WINDOW_SIZE; i++)
    {
        g_DistanceWindow[i] = 0.0f;
    }
    g_WindowIndex = 0;
}

/**
 * @brief EXTI4中断服务函数（修复：累加计时防溢出）
 */
void EXTI4_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        if (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == 1)
        {
            // 上升沿：重置计时
            g_Tim4MsCount = 0;
            TIM_SetCounter(TIM4, 0);
            TIM_Cmd(TIM4, ENABLE);
            g_Echo_Flag = 0;
        }
        else
        {
            // 下降沿：计算总时间（ms*1000 + us）
            TIM_Cmd(TIM4, DISABLE);
            g_Echo_Time = (uint32_t)g_Tim4MsCount * 1000 + TIM_GetCounter(TIM4);
            g_Echo_Flag = 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

/**
 * @brief 滑动窗口平均值计算
 */
static float CalcWindowAverage(void)
{
    float sum = 0.0f;
    uint8_t validCount = 0;
    for(uint8_t i=0; i<WINDOW_SIZE; i++)
    {
        if(g_DistanceWindow[i] >= 0.0f)
        {
            sum += g_DistanceWindow[i];
            validCount++;
        }
    }
    return validCount == 0 ? 0.0f : (sum / validCount);
}

/**
 * @brief 获取单次测距结果（核心修复：校准+过滤）
 */
static float GetSingleDistance(void)
{
    float distance = 0.0f;
    g_Echo_Time = 0;
    g_Echo_Flag = 0;

    // 1. 触发脉冲改为20us，提高稳定性
    GPIO_SetBits(TRIG_PORT, TRIG_PIN);
    Delay_us(20);
    GPIO_ResetBits(TRIG_PORT, TRIG_PIN);

    // 2. 缩短超时到10ms，减少阻塞
    uint32_t timeout = 0;
    while (g_Echo_Flag == 0 && timeout < 10000)
    {
        timeout++;
        Delay_us(1);
    }

    // 3. 过滤无效值+超近距离校准
    if (g_Echo_Time >= MIN_ECHO_TIME && g_Echo_Time <= MAX_ECHO_TIME && g_Echo_Flag){
        distance = g_Echo_Time * SOUND_SPEED_COEFF;
        // 超近距离校准（1~2cm减0.2cm，2~5cm减0.1cm）
        if(distance < 2.0f)
        {
            distance -= 0.2f;
        }
        else if(distance < 5.0f)
        {
            distance -= 0.1f;
        }
    }else if (g_Echo_Time < MIN_ECHO_TIME){
        distance = 0.0f;
    }else if (g_Echo_Time > MAX_ECHO_TIME){
        distance = MAX_ECHO_TIME * SOUND_SPEED_COEFF;
    }else{
        distance = MAX_ECHO_TIME * SOUND_SPEED_COEFF;
    }

    return distance;
}

/**
 * @brief 对外接口
 */
float HC_SR04_GetDistance(void)
{
    float singleDist = GetSingleDistance();
    if(singleDist >= 0.0f)
    {
        g_DistanceWindow[g_WindowIndex] = singleDist;
        g_WindowIndex = (g_WindowIndex + 1) % WINDOW_SIZE;
    }
    return CalcWindowAverage();
}


