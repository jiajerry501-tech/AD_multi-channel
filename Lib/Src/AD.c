#include "AD.h"

extern ADC_HandleTypeDef hadc1;

/**
  * @brief  ADC 硬件自校准
  * @note   STM32F103 系列在转换前必须进行自校准以减小内部参考偏置误差
  */
void AD_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
}

/**
  * @brief  获取指定规则通道的 12 位 ADC 转换原始值 (0 ~ 4095)
  * @param  ADC_Channel: HAL 库通道宏 (如 ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3)
  * @retval uint16_t: 12 位原始 ADC 转换数据
  */
uint16_t AD_GetValue(uint32_t ADC_Channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    /* 动态重构规则组 Rank 1 通道 */
    sConfig.Channel = ADC_Channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5; // 采样周期 55.5 Cycles 保证阻抗匹配与信号稳定

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        return 0;
    }

    /* 启动单次软件触发转换 */
    HAL_ADC_Start(&hadc1);

    /* 轮询等待转换完成（超时阈值设为 10ms） */
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
        return (uint16_t)HAL_ADC_GetValue(&hadc1);
    }

    return 0;
}