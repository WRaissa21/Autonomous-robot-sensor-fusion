/**
 * @file adcread.c
 * @brief implementation for analog read
 *
 * @author Anonymous
 * @date Sep 16, 2023
 */

#define ADCRD_C

#include "adc.h"
#include "adcrd.h"

/* global variables */
uint16_t adc1_buffer[ADCRD_ADC1_NB_CHANNELS] = {11, 12};
uint16_t adc2_buffer[ADCRD_ADC2_NB_CHANNELS] = {21, 22};

/**
 * @brief Initialize ADC
 * @retval none
 */
void adcrd_init(void)
{
  /* Calibrate The ADC On Power-Up For Better Accuracy */
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
  /* start ADC in DMA mode */
  HAL_ADC_Start_DMA(&hadc1,(uint32_t*)adc1_buffer,2);
  HAL_ADC_Start_DMA(&hadc2,(uint32_t*)adc2_buffer,2);
  /* Start ADC Conversion */
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Start(&hadc2);
}

/**
 * @brief read ADC value
 * @param[in] value: channel number (between 0 and 3)
 * @retval 12-bit ADC value, -1 if channel is incorrect
 */
int32_t adcrd_get_value(uint32_t channel)
{
	int32_t value;
	switch (channel)
	{
	case ADCRD_CHAN0_A0_ADC1_IN0_LDR1:
		value = adc1_buffer[0];
		break;
	case ADCRD_CHAN1_A1_ADC1_IN2_LDR2:
		value = adc1_buffer[1];
		break;
	case ADCRD_CHAN2_A3_ADC2_IN17_IR1:
		value = adc2_buffer[0];
		break;
	case ADCRD_CHAN3_A5_ADC2_IN3_IR2:
		value = adc2_buffer[1];
		break;
	default:
		value = -1;
	}
	return value;
}

/**
 * @brief read voltage value
 * @param[in] value: channel number (between 0 and 3)
 * @retval voltage, -1 if channel is incorrect
 */
float adcrd_get_voltage(uint32_t channel)
{
	uint32_t value;
	float voltage;

	value = adcrd_get_value(channel);
	if (value < 0) {
		voltage = -1;
	} else {
		voltage = acdrd_adcval2volt(value);
	}

	return voltage;
}

/**
 * @brief Convert an ADC value to voltage
 * @param[in] value: 12-bit ADC value
 * @retval voltage
 */
float acdrd_adcval2volt(uint32_t value)
{
	float voltage;
	if (value > ADCRD_MAX_ADC_VALUE) {
		return -1;
	}
	voltage = 3.3 * (float)value / 4095;
	return voltage;
}

#undef ADCRD_C
