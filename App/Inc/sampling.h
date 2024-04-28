/*
 * sampling.h
 *
 *  Created on: Apr 22, 2024
 *      Author: Otavio
 */

#ifndef INC_SAMPLING_H_
#define INC_SAMPLING_H_

#include "stm32f4xx_hal.h"
#include "defs.h"

typedef struct
{
	// ======== Sampling ============ //
	uint16_t samplingInterval;
	uint16_t adcValue;
	uint16_t adcHysteresisCriticalValue;
	uint16_t adcHysteresisThreshold;
	uint16_t adcValueHigh;
	uint16_t adcValueLow;
	uint8_t currentMagnitudeOrder;

	// ======== ADC ============ //
	ADC_HandleTypeDef hadc;
} Sampling;

typedef enum CURRENT_MAGNITUDE_ORDER
{
	LOW_CURRENT = 0x00,
	HIGH_CURRENT = 0x01
} CurrentMagnitudeOrder;

// ======== Init ======== //
void samplingInit(Sampling *sampling, ADC_HandleTypeDef hdac);

// ======== Sampling ============ //
void samplingExecuteAdcRead(Sampling *sampling);

// ======= Getters and Setters ======== //
uint16_t samplingGetSamplingInterval(Sampling *sampling);
void samplingSetSamplingInterval(Sampling *sampling, uint16_t samplingInterval);
uint16_t samplingGetAdcValue(Sampling *sampling);
void samplingSetAdcValue(Sampling *sampling, uint16_t adcValue);
uint8_t samplingGetCurrentMagnitudeOrder(Sampling *sampling);
void samplingSetCurrentMagnitudeOrder(Sampling *sampling, uint8_t currentMagnitudeOrder);

#endif /* INC_SAMPLING_H_ */
