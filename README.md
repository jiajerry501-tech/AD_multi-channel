# STM32 Multi-Channel ADC Acquisition Example (AD_multi-channel)

本工程是一个基于 **STM32F103** 芯片（使用 STM32CubeMX + HAL 库）实现的 **多通道 ADC 软件轮询采集示例**。

通过在单次转换模式下**动态重构规则通道 (Rank 1)**，实现了单个 ADC 外设对多路模拟传感器信号的依次采样与换算，并结合 OLED 显示屏实时呈现采集到的 12 位数字量和对应的模拟电压值。

---

## 🌟 项目特点 (Features)

1. **动态通道切换**：利用 `HAL_ADC_ConfigChannel()` 在单次转换时重新配置规则组 Rank 1 通道，无需开辟复杂的 DMA 循环模式或多通道连续扫描即可实现多路模拟量采集。
2. **硬件自校准**：初始化时自动调用 `HAL_ADCEx_Calibration_Start()` 对 STM32内部 ADC 进行校准，有效减小内部参考偏置误差。
3. **直观数据可视化**：通过 0.96 寸 OLED 显示屏实时显示各个通道的：
   - 通道编号及含义标签
   - 12 位 ADC 原始数字量 (`0 ~ 4095`)
   - 换算后的实际模拟电压值 (`0.00V ~ 3.30V`)

---

## 🛠️ 硬件连接与引脚定义 (Hardware Connections)

| 外设/传感器模块 | STM32F103 引脚 | 功能说明 |
| :--- | :--- | :--- |
| **光敏传感器 (Light)** | `PA1` | ADC1 Channel 1 (`ADC_CHANNEL_1`) |
| **热敏传感器 (Temp)** | `PA2` | ADC1 Channel 2 (`ADC_CHANNEL_2`) |
| **红外反射传感器 (IR)** | `PA3` | ADC1 Channel 3 (`ADC_CHANNEL_3`) |
| **OLED 显示屏** | 根据 `Lib/Inc/oled.h` 配置 | 显示 ADC 采集结果与电压数据 |

> 💡 **采样电压计算公式**：
> $$\text{Voltage (V)} = \frac{\text{ADC\_Value}}{4095.0} \times 3.3\text{V}$$

---

## 📁 目录结构 (Project Structure)

```text
AD_multi-channel/
├── AD_multi-channel.ioc   # STM32CubeMX 工程配置文件
├── CMakeLists.txt         # CMake 构建配置文件
├── CMakePresets.json      # CMake Preset 配置
├── Core/                  # CubeMX 生成的核心代码
│   ├── Inc/               # 系统头文件 (main.h, adc.h, gpio.h 等)
│   └── Src/               # 主程序入口 (main.c) 及外设初始化
├── Lib/                   # 自定义扩展驱动库
│   ├── Inc/
│   │   ├── AD.h           # 多通道 ADC 驱动头文件
│   │   ├── oled.h         # OLED 屏幕驱动头文件
│   │   └── oled_font.h    # OLED 字库头文件
│   └── Src/
│       ├── AD.c           # 多通道 ADC 动态切换采样实现
│       ├── oled.c         # OLED 屏幕驱动实现
│       └── oled_font.c    # OLED 字库数据
└── Drivers/               # STM32F1xx HAL 库与 CMSIS 驱动
```

---

## 🚀 核心代码逻辑 (Key Implementation)

### 1. 多通道 ADC 采集函数 (`Lib/Src/AD.c`)

```c
uint16_t AD_GetValue(uint32_t ADC_Channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    // 动态重构规则组 Rank 1 通道
    sConfig.Channel = ADC_Channel;
    sConfig.Rank = ADC_REGULAR_Rank_1; // ADC_REGULAR_RANK_1
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5; // 55.5 周期采样

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        return 0;
    }

    // 启动单次软件触发转换
    HAL_ADC_Start(&hadc1);

    // 轮询等待转换完成
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
        return (uint16_t)HAL_ADC_GetValue(&hadc1);
    }

    return 0;
}
```

### 2. 主循环调度采集 (`Core/Src/main.c`)

```c
// 依次切换通道采集
AD0_Light = AD_GetValue(AD_CHANNEL_LIGHT); // PA1
AD1_Temp  = AD_GetValue(AD_CHANNEL_TEMP);  // PA2
AD2_IR    = AD_GetValue(AD_CHANNEL_IR);    // PA3

// 标幺化换算电压
Voltage_Light = (float)AD0_Light / 4095.0f * 3.3f;
Voltage_Temp  = (float)AD1_Temp  / 4095.0f * 3.3f;
Voltage_IR    = (float)AD2_IR    / 4095.0f * 3.3f;
```

---

## 💻 编译与开发环境 (Development Environment)

- **开发工具**：VS Code / STM32CubeIDE / Keil MDK
- **构建工具**：CMake + GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`)
- **配置工具**：STM32CubeMX (可打开 `.ioc` 文件调整外设参数)

---

## 📜 许可证 (License)

本项目采用 [MIT License](LICENSE) 开源。
