#include "main.h"
#include <cstring>
#include <cstdio>
#include "I2C.h"
#include "MPU6050.h"
#include "basic_timer.h"
#include "GPIO_Port.h"
#include "RCC.h"

#undef TIM15
#undef TIM16
#undef TIM17
#undef USART1
#undef USART2
#undef USART3
#undef USART4

#include "general_timer.h"
#include "UART.h"

#ifdef __cplusplus
extern "C" {
#endif

void set_pwm_value(uint16_t val);

void SystemClock_Config(void);

void itoa(int num, unsigned char* buffer, int base);

//const volatile size_t pwm_val = (rx_buf[0]-'0')*100+(rx_buf[1]-'0')*10+(rx_buf[2]-'0');
// set_pwm_value(pwm_val);

void test_callback(void)
{
  GPIO::PORTA.toggle(5);
}

volatile uint8_t glb_flag=0;
void callback1(void)
{
  ++glb_flag;
}

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  //mIWDG::set_and_go(6, 0xFF);

  /* Initialize all configured peripherals */
  /************************************/
  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::C);
  GPIO::PORTA.entrada(1);
  GPIO::PORTA.salida(5);
  auto blue_btn = GPIO::PORTC.entrada(13);
  /************************************/

  UART uart2(UART::Peripheral::USART2, 115200);
  uart2.enable_fifo().enable();

  /** Hasta que no encuentre un mejor mecanismo para hacer callbacks más sofisticados,
   * seguiré haciendo cochinadas en las interrupciones. Hay que buscar una solución. */
  UART uart3(UART::Peripheral::USART3, 9600);
  uart3.enable_interrupt_rx(nullptr).enable();

  uint8_t tx_buf[64] = "---\n";
  uint8_t greetings[32] = "Hey I just reset\n";

  auto led_callback = [](void) -> void {
    GPIO::PORTA.toggle(5);
  };
  basic_timer t6(BasicTimer::TIM6, basic_timer::Mode::Periodic, 0x1800, 0x800);
  t6.enable_interrupt(led_callback);
  t6.start();

  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::OnePulseMode, 0x2000, 0x800);
  t17.enable_interrupt(callback1);

  I2C i2c2(I2C::Peripheral::I2C1);
  i2c2.enable(I2C::Timing::Standard);

  MPU6050 mpu(i2c2); //pasa una referencia al objeto i2c
  I2C::Status res = mpu.set_sampling_rate();

  uart2.transmitq(greetings, strlen((const char*)greetings));
  uint8_t buf[16] = {0};
  float acc[3] = {0};

  /* HAL_GetTick regresa en milisegundos. ;/ */
  while (1) {

    if(glb_flag%2 == 1) {
      mpu.posicionar_en_registro_ax();
      mpu.leer(buf, 6);
      mpu.convert_to_float(acc, buf, 3);

      std::sprintf((char*)tx_buf, "ax=%.2f\t ay=%.2f\t az=%.2f\n\r", acc[0], acc[1], acc[2]);

      uart2.transmitq(tx_buf, std::strlen((const char*)tx_buf));
      glb_flag = 0;
    }

    if(blue_btn.read_input() == 0) {
      t17.start();
    }

    /** Forwardeo del UART3 al UART2 */
    if(uart3.available()) {
      uart2.write_byte(uart3.read_byte());
    }

    /** Pass through to UART 3 */
    if(uart2.available()) {
      uart3.write_byte(uart2.read_byte());
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