#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "../Inc/gpio.h"
#include "../Inc/adc.h"
#include "../Inc/usart.h"
#include <cstring>
#include <cstdio>
#include "I2C.h"
#include "MPU6050.h"
#include "basic_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

void set_pwm_value(uint16_t val);

void SystemClock_Config(void);

void itoa(int num, unsigned char* buffer, int base);

#define RX_SZ 3
uint8_t rx_buf[RX_SZ];

//es mapear de una recta a otra no?
int map(int m, int ol, int oh, int tl, int th)
{
  if (m<=ol)
    return tl;
  if (m>=oh)
    return th;
  int orange = oh-ol;
  float prop = (m-ol)/orange;
  int trange = th-tl;
  int ret = trange*prop+tl;
  return ret;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
#define RESP_SZ 16
  uint8_t resp_buf[RESP_SZ] = "got: ";
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  for (int i = 0; i<RX_SZ; ++i)
    resp_buf[i+5] = rx_buf[i];

  HAL_UART_Transmit(huart, resp_buf, RESP_SZ, 10);
  const volatile size_t pwm_val = (rx_buf[0]-'0')*100+(rx_buf[1]-'0')*10+(rx_buf[2]-'0');
  set_pwm_value(pwm_val);
}

/* Independent watch dog*/
namespace mIWDG{
  constexpr size_t base = 0x40003000;
  constexpr size_t KR = (base); //limit 0x400033FF
  constexpr size_t PR = (base + 0x4);
  constexpr size_t RLR = (base + 0x8);
  constexpr size_t SR = (base + 0xC);
  constexpr size_t WINR = (base + 0x10);
  /*************************************/
  void enable_watchdog() { *reinterpret_cast<size_t*>(KR) = 0x0000CCCC;}
  void enable_regwrite() { *reinterpret_cast<size_t*>(KR) = 0x00005555;}
  void write_prescaler(const size_t val) { *reinterpret_cast<size_t*>(PR) = (val&0x7); }//values from 0 to 7
  void write_reload(const size_t val) { *reinterpret_cast<size_t*>(RLR) = (val&0xFFF); }
  void wait() {
    size_t status = 1;
    while(status != 0) { status = *reinterpret_cast<size_t*>(SR); };
  }
  void refresh() { *reinterpret_cast<size_t*>(KR) = 0x0000AAAA; }
  void set_and_go(const size_t prescaler, const size_t reload) {
    enable_watchdog();
    enable_regwrite();
    write_prescaler(prescaler);
    write_reload(reload);
    wait();
  }
};

void print(const char* str, int val)
{
  char buf[64];
  std::sprintf(buf, "%s%d\r\n", str, val);
  HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 10);
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
  pinMode(GPIOA, GPIO_PIN_5, OUTPUT);
  pinMode(GPIOA, GPIO_PIN_6, OUTPUT);
  pinMode(GPIOA, GPIO_PIN_7, OUTPUT);
  //MX_DMA_Init();
  MX_TIM14_Init();
  MX_USART2_UART_Init();

  set_pwm_value(1000);

  uint8_t tx_buf[32] = "---\n";
  uint8_t greetings[32] = "Hey I just reset\n";

  basic_timer t6(BasicTimer::TIM6, 0x1800, 0x800);
  t6.configure_interrupts(1);
  t6.enable();

  //mIWDG::set_and_go(6, 0xFF);
  I2C i2c2(I2C::Peripheral::I2C1);
  i2c2.enable(I2C::Timing::Standard);
  MPU6050 mpu(i2c2); //pasa una referencia al objeto i2c
  I2C::Status res = mpu.set_sampling_rate();
  print("setting_sampling_rate: ", static_cast<size_t>(res));



  HAL_UART_Transmit(&huart2, greetings, strlen((const char*)greetings), 10);
  uint8_t buf[16] = {0};
  float acc[3] = {0};

  /* HAL_GetTick regresa en milisegundos. ;/ */
  while (1) {

    if(readPin(GPIOC, GPIO_PIN_13)==0) {
      mpu.posicionar_en_registro_ax();
      mpu.leer(buf, 6);
      mpu.convert_to_float(acc, buf, 3);

      std::sprintf((char*)tx_buf, "ax=%.2f\t ay=%.2f\t az=%.2f\n\r", acc[0], acc[1], acc[2]);

      HAL_UART_Transmit(&huart2, tx_buf, std::strlen((const char*)tx_buf), 10);
    }

    if (false) {
/*
      writePin(GPIOA, 4, 1); //Ultrasonic
      HAL_Delay(1);
      writePin(GPIOA, 4, 0); //Ultrasonic
      //pulseIn(GPIOA, GPIO_PIN_1, HIGH)

      while (readPin(GPIOA, GPIO_PIN_1)==0);

      */
    }
    else {


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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!=HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0)!=HAL_OK) {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit)!=HAL_OK) {
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{

  if (htim->Instance==TIM3) {
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
  while (1);
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

#ifdef __cplusplus
}
#endif