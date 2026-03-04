/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "lptim.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ir_gp2y0a21yk0f.h"
#include "hcsr04.h"
#include "adcrd.h"
#include "motor.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* Private macro --------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #endif /* __GNUC__ */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float ldr1_voltage;
float ldr2_voltage;
float ir1_distance;
float ir2_distance;
float ultrasound1_distance;
float ultrasound2_distance;
uint32_t i = 0;
uint32_t pulse_counter = 0;
uint32_t pwm_tim4_duty_cycle = 0;

uint32_t debug=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief  print character to a console based on UART2
 * @param ch pointer to the character to print
 * @retval value of the character
 */
PUTCHAR_PROTOTYPE {
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit (&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);

  return ch;
}
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_LPTIM1_Init();
  /* USER CODE BEGIN 2 */
  if (debug) printf ("STM32 example - ETN Polytech Nantes\r\n");

  /* configuration for HCSR04 */
  HAL_TIM_Base_Start (&htim2); /* start Timer2 to get a 1µs counter */
  hcsr04_set_trig_pin (HCSR04_INSTANCE_0, D6_OUT_HCSR04_TRIG1_GPIO_Port,
  D6_OUT_HCSR04_TRIG1_Pin);
  hcsr04_set_echo_pin (HCSR04_INSTANCE_0, D10_IN_HCSR04_ECHO1_GPIO_Port,
  D10_IN_HCSR04_ECHO1_Pin);
  hcsr04_set_trig_pin (HCSR04_INSTANCE_1, D12_OUT_HCSR04_TRIG2_GPIO_Port,
  D12_OUT_HCSR04_TRIG2_Pin);
  hcsr04_set_echo_pin (HCSR04_INSTANCE_1, D9_IN_HCSR04_ECHO2_GPIO_Port,
  D9_IN_HCSR04_ECHO2_Pin);

  /* configuration for infrared ranging sensors */
  ir_set_adc_channel (IR_INSTANCE_0, ADCRD_CHAN2_A3_ADC2_IN17_IR1);
  ir_set_adc_channel (IR_INSTANCE_1, ADCRD_CHAN3_A5_ADC2_IN3_IR2);
  HAL_Delay (IR_SETUP_DELAY_MS);

  /* ADC configuration */
  adcrd_init ();

  /* start TIM4 for PWM generation */
  HAL_TIM_Base_Start (&htim3);
  HAL_TIM_Base_Start (&htim4);
  HAL_TIM_PWM_Start (&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start (&htim4, TIM_CHANNEL_2);

  /* start counting pulse on LPTIM1 */
  HAL_LPTIM_Counter_Start (&hlptim1, 65535);

  int light_found = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (debug) printf ("loop %ld\r\n", ++i);

    /* Blink LD2 */
    if (debug) printf ("\tLD2 on\r\n");
    HAL_GPIO_WritePin (GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_Delay (100);

    /* Read and print ADC values */
    ldr1_voltage = adcrd_get_voltage (ADCRD_CHAN0_A0_ADC1_IN0_LDR1);
    if (debug) printf ("\tldr1 %6.2f V\r\n", ldr1_voltage);
    ldr2_voltage = adcrd_get_voltage (ADCRD_CHAN1_A1_ADC1_IN2_LDR2);
    if (debug) printf ("\tldr2 %6.2f V\r\n", ldr2_voltage);

    /* Read and print distances from IR ranging sensors */
    ir1_distance = ir_get_distance (IR_INSTANCE_0);
    if (debug) printf ("\tir1 %6.1f cm\r\n", ir1_distance);
    ir2_distance = ir_get_distance (IR_INSTANCE_1);
    if (debug) printf ("\tir2 %6.1f cm\r\n", ir2_distance);

    /* Read and print distances from ultrasound sensors */
    ultrasound1_distance = hcsr04_get_distance (HCSR04_INSTANCE_0);
    if (debug) printf ("\tultrasound1 %6.1f cm\r\n", ultrasound1_distance);
    ultrasound2_distance = hcsr04_get_distance (HCSR04_INSTANCE_1);
    if (debug) printf ("\tultrasound2 %6.1f cm\r\n", ultrasound2_distance);

    /* Read pulse counter that is fed by PWM (1 pulse every 50ms)*/
    pulse_counter = HAL_LPTIM_ReadCounter (&hlptim1);
    if (debug) printf ("\tpulse count = %ld\r\n", pulse_counter);
    bool pwm_increase = false;
    /* change duty cycle */
    if (pwm_increase == true) {
      if (pwm_tim4_duty_cycle >= 10) {
		pwm_increase = false;
		pwm_tim4_duty_cycle -= 10;
      }
      else {
		pwm_tim4_duty_cycle += 10;
      }
    }
    else {
      if (pwm_tim4_duty_cycle <= 0) {
		pwm_increase = true;
		pwm_tim4_duty_cycle += 10;
      }
      else {
		pwm_tim4_duty_cycle -= 10;
      }
    }
    if (debug) printf ("\tduty cycle = %ld\r\n", pwm_tim4_duty_cycle);

    /* Blink LD2 */
    HAL_GPIO_WritePin (GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    if (debug) printf ("\tLD2 off\r\n");

    /* testing */
    motor_reverse(MOTOR_INSTANCE_2, 80);
	motor_reverse(MOTOR_INSTANCE_1, 80);
    /* testing */

    if (!light_found && 0){
    	aller_vers_la_lumiere(&light_found);
    }
//    if (!maze_complete){
//    	do_maze();
//    }
    if (debug) {
    	HAL_Delay (1000);
    } else {
    	HAL_Delay(1);
    }
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
