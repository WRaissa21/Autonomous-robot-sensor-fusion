/*
 * hcsr04.c
 *
 */

#define HCSR04_C

#include "hcsr04.h"
#include "tim.h"

/* global variables */
hcsr04_config_t hcsr04[HCSR04_NUMBER_OF_INSTANCES];

/**
 * @brief  This function allocate trig signal to a port/pin for a given instance.
 * @param[in] GPIOx: must be one of GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
 * @param[in] GPIO_Pin: must be one of GPIO_PIN_0, ..., GPIO_PIN_15
 * @retval -1 in case of error, 0 otherwise
 */
int32_t
hcsr04_set_trig_pin (uint32_t instance, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  /* check instance number */
  if ((instance != HCSR04_INSTANCE_0) && (instance != HCSR04_INSTANCE_1))
    {
      return -1;
    }
  hcsr04[instance].trig_port = GPIOx;
  hcsr04[instance].trig_pin = GPIO_Pin;
  return 0;
}

/**
 * @brief  This function allocate echo signal to a port/pin for a given instance.
 * @param[in] GPIOx: must be one of GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
 * @param[in] GPIO_Pin: must be one of GPIO_PIN_0, ..., GPIO_PIN_15
 * @retval -1 in case of error, 0 otherwise
 */
int32_t
hcsr04_set_echo_pin (uint32_t instance, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  /* check instance number */
  if ((instance != HCSR04_INSTANCE_0) && (instance != HCSR04_INSTANCE_1))
    {
      return -1;
    }
  hcsr04[instance].echo_port = GPIOx;
  hcsr04[instance].echo_pin = GPIO_Pin;
  return 0;
}
/**
 * @brief  This function returns the distance measured by an ultrasonic sensor.
 * @param[in] instance must be HCSR04_INSTANCE1 or HCSR04_INSTANCE2
 * @retval distance in cm or a negative number in case of error
 */

/*------------------------------------------------------------------------------------------------------*
 *	On 23 Sept. 2024										*
 *	Revised by Salah to  to improve timeout handling and implement checks for timer overflow	*
 *													*
 *------------------------------------------------------------------------------------------------------*
 */

float
hcsr04_get_distance (uint32_t instance)
{
  uint32_t duration = 0, start_tick = 0, stop_tick = 0, echo = 0;
  float distance;

  // Check for valid instance
  if ((instance != HCSR04_INSTANCE_0) && (instance != HCSR04_INSTANCE_1))
    {
      return -1;
    }

  // Trigger the ultrasonic sensor
  HAL_GPIO_WritePin (hcsr04[instance].trig_port, hcsr04[instance].trig_pin,
		     GPIO_PIN_SET);
  hcsr04_delay_us (10);
  HAL_GPIO_WritePin (hcsr04[instance].trig_port, hcsr04[instance].trig_pin,
		     GPIO_PIN_RESET);

  // Wait for the rising edge of the Echo pin or a timeout of 50 ms

  start_tick = __HAL_TIM_GetCounter(&htim2);
  do
    {
      echo = HAL_GPIO_ReadPin (hcsr04[instance].echo_port,
			       hcsr04[instance].echo_pin);
      stop_tick = __HAL_TIM_GetCounter(&htim2);
    }
  while ((echo != 1) && ((stop_tick - start_tick) < 50000));

  // Timeout check
  if ((stop_tick - start_tick) >= 50000)
    {

      return -1;
    }

  // Start timing the echo pulse

  uint32_t max_duration = 50000; // Timeout duration (50 ms)
  start_tick = __HAL_TIM_GetCounter(&htim2);
  do
    {
      echo = HAL_GPIO_ReadPin (hcsr04[instance].echo_port,
			       hcsr04[instance].echo_pin);
      stop_tick = __HAL_TIM_GetCounter(&htim2);
      // Check if the timeout has been reached
      //Timeout: Echo pin stuck high, no falling edge detected.
      if ((stop_tick - start_tick) >= max_duration)
	{

	  return -1; // Timeout
	}
    }
  while (echo != 0);

  // Calculate the duration of the echo pulse
  if (stop_tick < start_tick)
    {
      duration = (0xFFFF - start_tick) + stop_tick;
    }
  else
    {
      duration = stop_tick - start_tick;
    }

  // Calculate distance based on the duration (duration * speed of sound / 2)
  distance = (float) duration * 0.034 / 2;
  //printf("Measured distance: %6.1f cm\n", distance);

  return distance;
}
/**
 * @brief Active wait for the duration passed in argument
 * @param delay duration in µs
 * @retval none
 */
void
hcsr04_delay_us (uint32_t delay)
{
  uint32_t tick_start = __HAL_TIM_GetCounter(&htim2);
  uint32_t tick_stop;

  if (delay > 0xFFFFFFFF - tick_start)
    {
      tick_stop = delay - (0xFFFFFFFF - tick_start);
    }
  else
    {
      tick_stop = tick_start + delay;
    }

  while ((__HAL_TIM_GetCounter(&htim2) <= tick_stop))
    {
      /* wait */
    }
}

#undef HCSR04_C

