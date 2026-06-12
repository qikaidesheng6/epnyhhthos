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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern volatile uint8_t mode;  // 模式编号：0=呼吸 1=流水 2=逐个亮→全亮→逐个灭 3=流水+闪烁 4=全亮
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

LED_Init();

    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5); // ✅ 清上电误触发

static uint8_t auto_sub_mode = 1;
static uint16_t flow_cnt = 0;  // 流水灯执行计数器
#define FLOW_RUN_TIMES 24
// 新增：每个模式保持时长(ms)，按需修改
#define MODE_HOLD_TIME 800    
static uint16_t hold_tick = 0;  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		

 if(mode == 0)
    {
        // mode=0：自动循环 1~5 所有模式
        switch(auto_sub_mode)
        {
            case 1:
                LED_Breath();
                // 每个模式跑完切下一个子模式
						 hold_tick++;
            // 延时结束再切换模式
            if(hold_tick >= MODE_HOLD_TIME)
            {
                auto_sub_mode = 2;
                hold_tick = 0;    // 清空计时
                flow_cnt = 0;     // 流水计数清零
            }
            HAL_Delay(1); 
                break;
            case 2:
                LED_Flow();
                HAL_Delay(100);
						 flow_cnt++; 
             if(flow_cnt >= FLOW_RUN_TIMES)
                {
                    auto_sub_mode = 3;
                    flow_cnt = 0; // 计数清零，留给后续模式使用
                }
                break;
            case 3:
                LED_FlowOn_AllOn_FlowOff();
                auto_sub_mode = 4;
                break;
            case 4:
                LED_FlowAndFlash();
                auto_sub_mode = 5;
                break;
            case 5:
                for(int i = 0; i < LED_NUM; i++)
                    HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_ON);
             hold_tick++;
            if(hold_tick >= MODE_HOLD_TIME)
            {
                auto_sub_mode = 1;
                hold_tick = 0;
            }
            HAL_Delay(1);
                break;
        }
    }
    else
    {
    // 主循环只管执行当前模式，中断负责加模式数
    switch(mode)
    {
		
        case 1: LED_Breath(); break;//呼吸
        case 2: LED_Flow();HAL_Delay(100); break;//流水
        case 3: LED_FlowOn_AllOn_FlowOff(); break;//完全点亮并完全消失
        case 4: LED_FlowAndFlash(); break;//闪烁三次
        case 5:
            for(int i = 0; i < LED_NUM; i++)
                HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_ON);//保持全亮
            HAL_Delay(100);
            break;
    }
	}
	/*	switch(current_mode)
    {
        case 0:
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
            HAL_Delay(2000);
            break;

        case 1:
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
            HAL_Delay(1000);
            break;

        default:
            break;
			}*/
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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