/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "AD.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* 传感器 12 位数字量缓存 (0 ~ 4095) */
uint16_t AD0_Light = 0;
uint16_t AD1_Temp  = 0;
uint16_t AD2_IR    = 0;

/* 转换后的模拟电压 (单位: V) */
float Voltage_Light = 0.0f;
float Voltage_Temp  = 0.0f;
float Voltage_IR    = 0.0f;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  /* 初始化屏幕与 ADC 校准 */
    OLED_Init();
    AD_Init();

    /*绘制静态文本标签 (行号: 1~4，列号: 1~16) */
    OLED_ShowString(1, 1, "AD0_L:");
    OLED_ShowString(2, 1, "AD1_T:");
    OLED_ShowString(3, 1, "AD2_R:");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* --- 1. 依次切换通道采集原始数据 --- */
        AD0_Light = AD_GetValue(AD_CHANNEL_LIGHT); // PA1 (ADC1_IN1)
        AD1_Temp  = AD_GetValue(AD_CHANNEL_TEMP);  // PA2 (ADC1_IN2)
        AD2_IR    = AD_GetValue(AD_CHANNEL_IR);    // PA3 (ADC1_IN3)

        /* --- 2. 标幺化换算为实际电压 (V = AD_Val / 4095.0 * 3.3V) --- */
        Voltage_Light = (float)AD0_Light / 4095.0f * 3.3f;
        Voltage_Temp  = (float)AD1_Temp  / 4095.0f * 3.3f;
        Voltage_IR    = (float)AD2_IR    / 4095.0f * 3.3f;

        /* --- 3. 刷新 12 位 AD 数字量 (4位宽度) --- */
        OLED_ShowNum(1, 7, AD0_Light, 4);
        OLED_ShowNum(2, 7, AD1_Temp,  4);
        OLED_ShowNum(3, 7, AD2_IR,    4);

        /* --- 4. 刷新计算后的电压值 (格式: X.XX V) --- */
        // 光敏传感器电压
        OLED_ShowNum(1, 12, (uint32_t)Voltage_Light, 1);
        OLED_ShowChar(1, 13, '.');
        OLED_ShowNum(1, 14, (uint32_t)(Voltage_Light * 100) % 100, 2);
        OLED_ShowChar(1, 16, 'V');

        // 热敏传感器电压
        OLED_ShowNum(2, 12, (uint32_t)Voltage_Temp, 1);
        OLED_ShowChar(2, 13, '.');
        OLED_ShowNum(2, 14, (uint32_t)(Voltage_Temp * 100) % 100, 2);
        OLED_ShowChar(2, 16, 'V');

        // 红外反射传感器电压
        OLED_ShowNum(3, 12, (uint32_t)Voltage_IR, 1);
        OLED_ShowChar(3, 13, '.');
        OLED_ShowNum(3, 14, (uint32_t)(Voltage_IR * 100) % 100, 2);
        OLED_ShowChar(3, 16, 'V');

        /* 刷新延时 (100ms) */
        HAL_Delay(100);
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
