#ifndef __AD_H
#define __AD_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* 传感器引脚对应 ADC1 规则通道定义 */
#define AD_CHANNEL_LIGHT     ADC_CHANNEL_1  // PA1: 光敏传感器
#define AD_CHANNEL_TEMP      ADC_CHANNEL_2  // PA2: 热敏传感器
#define AD_CHANNEL_IR        ADC_CHANNEL_3  // PA3: 反射式红外传感器

/* 函数声明 */
void AD_Init(void);
uint16_t AD_GetValue(uint32_t ADC_Channel);

#endif /* __AD_H */