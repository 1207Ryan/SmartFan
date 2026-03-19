#include "stm32f10x.h"                  // Device header
#include "math.h"
#include "Timer.h"

// ===== 硬件参数配置（根据你的电路修改）=====
#define NTC_R_REF    10000.0f  // 分压电阻值
#define NTC_B_VALUE  3950.0f   // NTC的B值
#define NTC_R_25     10000.0f  // 25℃时NTC阻值
#define ADC_MAX      4095.0f   // 12位ADC最大值
#define VCC          3.3f      // 供电电压
#define WINDOW_SIZE  8         // 滑动窗口长度（建议8-12，越小响应越快）


extern volatile uint8_t Gear;
uint16_t AD_Value;			  	//定义用于存放AD转换结果的全局数组
float Temp_Value = 0.0f;      	// 转换后的温度值（单位：℃）
uint8_t AD_Collect_Status = 0;	// 采集状态：0=关闭，1=开启
static float temp_window[WINDOW_SIZE]; // 滑动窗口缓存
static uint8_t window_count = 0;       // 窗口内有效数据个数
static uint8_t window_index = 0;       // 当前写入索引


/**
  * 函    数：AD初始化
  * 参    数：无
  * 返 回 值：无
  */
void AD_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//开启ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		//开启DMA1的时钟
	
	/*设置ADC时钟*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);						//选择时钟6分频，ADCCLK = 72MHz / 6 = 12MHz
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA1引脚初始化为模拟输入
	
	/*规则组通道配置*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);	//规则组序列1的位置，配置为通道0
	
	/*ADC初始化*/
	ADC_InitTypeDef ADC_InitStructure;											//定义结构体变量
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;							//模式，选择独立模式，即单独使用ADC1
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//数据对齐，选择右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;			//外部触发，使用软件触发，不需要外部触发
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//连续转换,每转换一次规则组序列后立刻开始下一次转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;								//扫描模式，失能
	ADC_InitStructure.ADC_NbrOfChannel = 1;										//通道数，为1，扫描规则组的1个通道
	ADC_Init(ADC1, &ADC_InitStructure);											//将结构体变量交给ADC_Init，配置ADC1
	
	/*DMA初始化*/
	DMA_InitTypeDef DMA_InitStructure;											//定义结构体变量
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;				//外设基地址，给定形参AddrA
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度，选择半字，对应16为的ADC数据寄存器
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址自增，选择失能，始终以ADC数据寄存器为源
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&AD_Value;					//存储器基地址，给定存放AD转换结果的全局数组AD_Value
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//存储器数据宽度，选择半字，与源数据宽度对应
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;					//存储器地址自增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//数据传输方向，选择由外设到存储器，ADC数据寄存器转到数组
	DMA_InitStructure.DMA_BufferSize = 1;										//转运的数据大小（转运次数），与ADC通道数一致
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//模式，选择循环模式，与ADC的连续转换一致
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//存储器到存储器，选择失能，数据由ADC外设触发转运到存储器
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级，选择中等
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);								//将结构体变量交给DMA_Init，配置DMA1的通道1
	
	/*DMA和ADC使能*/
	DMA_Cmd(DMA1_Channel1, DISABLE);							//DMA1的通道1失能
	ADC_DMACmd(ADC1, DISABLE);								//ADC1触发DMA1的信号失能
	ADC_Cmd(ADC1, DISABLE);									//ADC1失能
	
	/*ADC校准*/
	ADC_ResetCalibration(ADC1);								//固定流程，内部有电路会自动执行校准
	uint16_t timeout = 100;
	while (ADC_GetResetCalibrationStatus(ADC1) == SET && timeout > 0)
	{
		timeout--;
		__NOP(); // 空操作，仅占CPU周期，不阻塞
	}
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	/*ADC触发*/
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);	
}

/**
  * @brief  开启温度采集
  * @retval 无
  */
void AD_Collect_Start(void)
{
    if(AD_Collect_Status == 1) return; // 已开启则直接返回，避免重复操作
    
    // 1. 使能DMA和ADC
    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    
    // 2. 启动ADC连续转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);//软件触发ADC开始工作，由于ADC处于连续转换模式，故触发一次后ADC就可以一直连续不断地工作
    
	// 启动中断计时器
	TIM_Cmd(TIM3, ENABLE);
	
    // 3. 更新采集状态
    AD_Collect_Status = 1;
}

/**
  * @brief  关闭温度采集
  * @retval 无
  */
void AD_Collect_Stop(void)
{
    if(AD_Collect_Status == 0) return; // 已关闭则直接返回
    
    // 1. 停止ADC转换
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    
    // 2. 关闭ADC和DMA
    ADC_Cmd(ADC1, DISABLE);
    ADC_DMACmd(ADC1, DISABLE);
    DMA_Cmd(DMA1_Channel1, DISABLE);
	
	// 关闭中断计时器
	if(Gear == 0)
		TIM_Cmd(TIM3, DISABLE);
    
    // 3. 清空采集值（可选，避免关闭后残留旧值）
    AD_Value = 0;
    Temp_Value = 0.0f;
    
    // 4. 更新采集状态
    AD_Collect_Status = 0;
}

/**
  * @brief  滑动窗口平均值计算（核心：只算最近WINDOW_SIZE个有效数据）
  * @param  new_temp 新采集的温度值
  * @retval 窗口平均后的温度值
  */
static float Temp_SlidingWindow(float new_temp)
{
    // 1. 新值写入窗口（覆盖最旧的数据）
    temp_window[window_index++] = new_temp;
    if(window_index >= WINDOW_SIZE) window_index = 0;
    
    // 2. 记录有效数据个数（启动阶段：不满窗口时按实际个数算）
    if(window_count < WINDOW_SIZE) window_count++;
    
    // 3. 计算窗口内有效数据的平均值（核心：滑动窗口）
    float sum = 0.0f;
    for(uint8_t i=0; i<window_count; i++)
    {
        sum += temp_window[i];
    }
    return sum / window_count;
}

/**
  * @brief  重置滑动窗口（关闭采集时调用）
  */
static void Temp_Window_Reset(void)
{
    window_count = 0;
    window_index = 0;
    // 清空窗口缓存（可选，防止旧值残留）
    for(uint8_t i=0; i<WINDOW_SIZE; i++)
    {
        temp_window[i] = 0.0f;
    }
}

/**
  * @brief  AD值转温度（滑动窗口版+反向修正+快速启动）
  * @retval 平滑后的温度值（℃）
  */
float AD_to_Temp(void)
{
    // 关闭采集时重置窗口，返回0
    if(AD_Collect_Status == 0) 
    {
        Temp_Window_Reset();
        Temp_Value = 0.0f;
        return 0.0f;
    }
    
    // 1. ADC值转电压（防异常）
    float voltage = (AD_Value / ADC_MAX) * VCC;
    if(voltage <= 0.01f) voltage = 0.01f;
    if(voltage >= VCC - 0.01f) voltage = VCC - 0.01f;
    
    // 2. 反向计算NTC阻值（适配温度越高ADC值越低）
    float ntc_r = NTC_R_REF * (voltage / (VCC - voltage));
    if(ntc_r < 500.0f) ntc_r = 500.0f;
    if(ntc_r > 50000.0f) ntc_r = 50000.0f;
    
    // 3. 阻值转温度（标准B值公式）
    float temp_k = 1.0f / ( (1.0f/298.15f) + (1.0f/NTC_B_VALUE)*log(ntc_r/NTC_R_25) );
    float raw_temp = temp_k - 273.15f;
    
    // 4. 温度校准（根据实测微调）
    raw_temp += 0.0f;
    
    // 5. 滑动窗口平均（启动无延迟，运行平滑）
    Temp_Value = Temp_SlidingWindow(raw_temp);
    
    // 6. 温度范围限制
    if(Temp_Value < 0.0f) Temp_Value = 0.0f;
    if(Temp_Value > 100.0f) Temp_Value = 100.0f;
    
    return Temp_Value;
}
