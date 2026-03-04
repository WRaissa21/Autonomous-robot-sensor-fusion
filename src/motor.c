/*
 * motor.c
 *
 *  Created on: Sep 28, 2023
 *      Author: frq09465
 */

#define MOTOR_C

#include <stdbool.h>
#include "motor.h"
#include "tim.h"
#include "main.h"

/**
 * @brief  Activate motor of a given instance at a given percentage of maximum speed in forward direction
 * @param[in] instance: must be MOTOR_INSTANCE_1 or MOTOR_INSTANCE_2
 * @param[in] speed_percent: must be an unsigned integer between 0 and 100
 * @retval -1 in case of error, 0 otherwise
 */

int32_t motor_forward ( uint32_t instance, uint32_t speed_percent ) {
  /* check instance number */
  if ((instance != MOTOR_INSTANCE_1) && (instance != MOTOR_INSTANCE_2)) {
    return -1;
  }

  if (speed_percent>100){speed_percent = 100;}
  else if (speed_percent<0){speed_percent = 0;}

  if (instance == MOTOR_INSTANCE_1) {
	/* set motor1 in forward at a given speed */
	HAL_GPIO_WritePin (D2_OUT_MOTOR1_IN1_GPIO_Port, D2_OUT_MOTOR1_IN1_Pin, GPIO_PIN_RESET); // pin in1 = 0 : avance roue droite relie a in4
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,speed_percent*htim3.Init.Period/100); // relie a in3
  }
  else {
	HAL_GPIO_WritePin (A6_OUT_MOTOR2_IN4_GPIO_Port, A6_OUT_MOTOR2_IN4_Pin, GPIO_PIN_RESET); // pin in4 = 0 : avance roue gauche relie a in1
	__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2,speed_percent*htim4.Init.Period/100); // relie a in2

  }
  return 0;
}
/**
 * @brief  Activate motor of a given instance at a given percentage of maximum speed in reverse direction
 * @param[in] instance: must be MOTOR_INSTANCE_1 or MOTOR_INSTANCE_2
 * @param[in] speed_percent: must be an unsigned integer between 0 and 100
 * @retval -1 in case of error, 0 otherwise
 */
int32_t motor_reverse ( uint32_t instance, uint32_t speed_percent ) {
  /* check instance number */
  if ((instance != MOTOR_INSTANCE_1) && (instance != MOTOR_INSTANCE_2)) {
    return -1;
  }

  if (speed_percent>100){speed_percent = 100;}
  else if (speed_percent<0){speed_percent = 0;}

  if (instance == MOTOR_INSTANCE_1) {
    /* set motor1 in forward at a given speed */
    HAL_GPIO_WritePin (D2_OUT_MOTOR1_IN1_GPIO_Port, D2_OUT_MOTOR1_IN1_Pin, GPIO_PIN_SET);  // pin in1 = 0 : avance roue droite relie a in4
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,(100-speed_percent)*htim3.Init.Period/100); // relie a in3
  }
  else {
	HAL_GPIO_WritePin (A6_OUT_MOTOR2_IN4_GPIO_Port, A6_OUT_MOTOR2_IN4_Pin, GPIO_PIN_SET); // pin in4 = 0 : avance roue gauche relie a in1
	__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2,(100-speed_percent)*htim4.Init.Period/100); // relie a in2
  }

  return 0;
}

/**
 * @brief  Put the motor of a given instance in free wheeling
 * @param[in] instance: must be MOTOR_INSTANCE_1 or MOTOR_INSTANCE_2
 * @retval -1 in case of error, 0 otherwise
 */
int32_t motor_freewheeling ( uint32_t instance ) {
  /* check instance number */
  if ((instance != MOTOR_INSTANCE_1) && (instance != MOTOR_INSTANCE_2)) {
    return -1;
  }

  /* complete the code here */

  return 0;
}

/**
 * @brief  Strongly brake and stop motor of a given instance
 * @param[in] instance: must be MOTOR_INSTANCE_1 or MOTOR_INSTANCE_2
 * @retval -1 in case of error, 0 otherwise
 */
int32_t motor_brake_stop ( uint32_t instance ) {
  /* check instance number */
  if ((instance != MOTOR_INSTANCE_1) && (instance != MOTOR_INSTANCE_2)) {
    return -1;
  }

  if (instance == MOTOR_INSTANCE_1) {
    /* set motor1 in forward at a given speed */
	    HAL_GPIO_WritePin (D2_OUT_MOTOR1_IN1_GPIO_Port, D2_OUT_MOTOR1_IN1_Pin, GPIO_PIN_RESET);  // pin in1 = 0 : avance roue droite relie a in4
	    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,0*htim3.Init.Period/100); // relie a in3
  }
  else {
		HAL_GPIO_WritePin (A6_OUT_MOTOR2_IN4_GPIO_Port, A6_OUT_MOTOR2_IN4_Pin, GPIO_PIN_RESET); // pin in4 = 0 : avance roue gauche relie a in1
	  __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2,0*htim4.Init.Period/100); // relie a in2
  }

  return 0;
}

#undef MOTOR_C
