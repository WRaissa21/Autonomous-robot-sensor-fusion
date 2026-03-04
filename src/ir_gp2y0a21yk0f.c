/**
 * @file ir_gp2y0a21yk0f.c
 * @brief implementation for infrared ranging sensor GP2Y0A21YK0F
 *
 * @author Anonymous
 * @date Sep 16, 2023
 */

#define IR_GP2Y0A21YK0F_C

#include "ir_gp2y0a21yk0f.h"

/* global variables */
ir_config_t ir[IR_NUMBER_OF_INSTANCES];

/**
 * @brief  Associate a port/pin to a given instance
 * @param[in] instance: must be IR_INSTANCE_1 or IR_INSTANCE_2
 * @param[in] GPIOx: must be one of GPIOA, GPIOB, ..., GPIOG
 * @param[in] GPIO_Pin: must be one of GPIO_PIN_0, ..., GPIO_PIN_15
 * @retval -1 in case of error, 0 otherwise
 */
int32_t ir_set_adc_channel(uint32_t instance, uint32_t channel)
{
	/* check instance number */
	if ((instance != IR_INSTANCE_0) && (instance != IR_INSTANCE_1)) {
		return -1;
	}
	/* memorize channel */
	ir[instance].channel = channel;
	return 0;
}

/**
 * @brief  Read distance of a given instance
 * @param[in] instance: must be IR_INSTANCE_1 or IR_INSTANCE_2
 * @retval measured distance or -1 in case of error
 */
float ir_get_distance(uint32_t instance)
{
	float distance;
	uint32_t adc_val;

	/* check instance number */
	if ((instance != IR_INSTANCE_0) && (instance != IR_INSTANCE_1)) {
		return -1;
	}

	/* read ADC */
	adc_val = adcrd_get_value(ir[instance].channel);

	/* convert adc value to distance */
	distance = ir_adcval2distance(adc_val);
	return distance;
}


/**
 * @brief  Convert ADC value to a distance in cm, using polynomial regression equation
 * d = 7.7494V^6 - 86.744V^5 + 391.13V^4 - 911.57V^3 + 1171.5V^2 -816.24V + 273.27
 * @retval distance in cm
 */
float ir_adcval2distance(uint32_t adc_val)
{
	float distance = -1;
	float v, v2, v3, v4, v5, v6;

	v = (float) adc_val * 3.3 / 4095.0;
	if ((v > 0.35) && (v<3.3)) {
		v2 = v * v;
		v3 = v * v2;
		v4 = v2 * v2;
		v5 = v2 * v3;
		v6 = v3 * v3;
		distance = (7.7494 * v6) - (86.744 * v5) + (391.13 * v4) - (911.57 * v3) + (1171.5 * v2) - (816.24 * v) + 273.27;
	}

	return distance;
}

#undef IR_GP2Y0A21YK0F_C

