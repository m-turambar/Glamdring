#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"

void set_pwm_value(uint16_t val);
void SystemClock_Config(void);

#define RX_SZ 3
uint8_t rx_buf[RX_SZ];

//es mapear de una recta a otra no?
int map(int m, int ol, int oh, int tl, int th)
{
    if(m <= ol)
        return tl;
    if(m >= oh)
        return th;
    int orange = oh-ol;
    float prop = (m-ol)/orange;
    int trange = th-tl;
    int ret = trange*prop + tl;
    return ret;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	#define RESP_SZ 16
	uint8_t resp_buf[RESP_SZ] = "got: ";
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	for(int i=0; i<RX_SZ; ++i)
		resp_buf[i + 5] = rx_buf[i];
	
	HAL_UART_Transmit(huart, resp_buf, RESP_SZ, 10);
	const volatile size_t pwm_val = (rx_buf[0]-'0')*100 + (rx_buf[1]-'0')*10 + (rx_buf[2]-'0');
	set_pwm_value(pwm_val);
}

void delayMicroseconds(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(uwTickFreq);
  }

  while ((HAL_GetTick() - tickstart) < wait)
  {
  }
}

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /************************************/
  ADC1_Init();
  pinMode(GPIOA, GPIO_PIN_1, INPUT);
  pinMode(GPIOC, GPIO_PIN_13, INPUT);
  /************************************/
  pinMode(GPIOA, GPIO_PIN_4, OUTPUT);
  pinMode(GPIOA, GPIO_PIN_6, OUTPUT);
  pinMode(GPIOA, GPIO_PIN_7, OUTPUT);
  //MX_DMA_Init();
  MX_TIM14_Init();
  MX_USART2_UART_Init();

  /* Mike */
  //HAL_UART_Receive_DMA(&huart2, rx_buf, RX_SZ);

  set_pwm_value(1000);

  uint8_t tx_buf[16] = {0};
  
  int t_on = 1;
  
  HAL_UART_Transmit(&huart2, tx_buf, 16, 10);
  
  while (1)
  {
    if(readPin(GPIOA, GPIO_PIN_1) == 0)
    {
        //HAL_UART_Transmit(&huart2, tx_buf, 4, 10);
        for(int i = 4; i<8; ++i)
        {        
            writePin(GPIOA, i, 1);
            HAL_Delay(t_on);
            writePin(GPIOA, i, 0);
        }
    }
    if(readPin(GPIOC, GPIO_PIN_13) == 0)
    {
        //HAL_UART_Transmit(&huart2, tx_buf, 4, 10);
        for(int i = 7; i>=4; --i)
        {        
            writePin(GPIOA, i, 1);
            HAL_Delay(t_on);
            writePin(GPIOA, i, 0);
        }
    }    
  }

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

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */

}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */

