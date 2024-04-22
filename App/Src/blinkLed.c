/*
 * blinkLed.c
 *
 *  Created on: Apr 22, 2024
 *      Author: Otavio
 */

#include "blinkLed.h"

// ======== Init =========== //
void blinkLedInit(BlinkLed *blinkLed, GPIO_TypeDef* ledPort, uint16_t ledPin, uint8_t blinkPattern)
{
	blinkLed->ledPort = ledPort;
	blinkLed->ledPin = ledPin;
	blinkLed->blinkPattern = blinkPattern;
	blinkLed->counter = 0;
	HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_RESET);
	blinkLedSetBlinkDelay(blinkLed, DELAY_100_MILISECONDS);
}

// ======== LED =========== //
void blinkLedExecuteBlink(BlinkLed *blinkLed)
{
	switch (blinkLedGetBlinkPattern(blinkLed))
	{
		case PATTERN_ALWAYS_OFF:
			HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_RESET);
			blinkLedSetBlinkDelay(blinkLed, DELAY_500_MILISECONDS);
			break;

		case PATTERN_ALWAYS_ON:
			HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_SET);
			blinkLedSetBlinkDelay(blinkLed, DELAY_500_MILISECONDS);
			break;

		case PATTERN_TOGGLE_EACH_100_MS:
			HAL_GPIO_TogglePin(blinkLed->ledPort, blinkLed->ledPin);
			blinkLedSetBlinkDelay(blinkLed, DELAY_100_MILISECONDS);
			break;

		case PATTERN_TOGGLE_EACH_250_MS:
			HAL_GPIO_TogglePin(blinkLed->ledPort, blinkLed->ledPin);
			blinkLedSetBlinkDelay(blinkLed, DELAY_250_MILISECONDS);
			break;

		case PATTERN_SHORT_SHORT_LONG:
			switch (blinkLedGetCounter(blinkLed))
			{
				case 0:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_SET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_250_MILISECONDS);
					blinkLedSetCounter(blinkLed, 1);
					break;

				case 1:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_RESET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_250_MILISECONDS);
					blinkLedSetCounter(blinkLed, 2);
					break;

				case 2:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_SET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_250_MILISECONDS);
					blinkLedSetCounter(blinkLed, 3);
					break;

				case 3:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_RESET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_250_MILISECONDS);
					blinkLedSetCounter(blinkLed, 4);
					break;

				case 4:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_SET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_750_MILISECONDS);
					blinkLedSetCounter(blinkLed, 5);
					break;

				case 5:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_RESET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_500_MILISECONDS);
					blinkLedSetCounter(blinkLed, 0);
					break;
			}
			break;

		case PATTERN_HEART_BEAT:
			switch (blinkLedGetCounter(blinkLed))
			{
				case 0:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_SET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_200_MILISECONDS);
					blinkLedSetCounter(blinkLed, 1);
					break;

				case 1:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_RESET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_150_MILISECONDS);
					blinkLedSetCounter(blinkLed, 2);
					break;

				case 2:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_SET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_500_MILISECONDS);
					blinkLedSetCounter(blinkLed, 3);
					break;

				case 3:
					HAL_GPIO_WritePin(blinkLed->ledPort, blinkLed->ledPin, GPIO_PIN_RESET);
					blinkLedSetBlinkDelay(blinkLed, DELAY_400_MILISECONDS);
					blinkLedSetCounter(blinkLed, 0);
					break;
			}
			break;
	}
}

// ======= Getters and Setters ======== //
uint32_t blinkLedGetBlinkDelay(BlinkLed *blinkLed)
{
	return blinkLed->blinkDelay;
}

void blinkLedSetBlinkDelay(BlinkLed *blinkLed, uint32_t blinkDelay)
{
	blinkLed->blinkDelay = blinkDelay;
}

uint8_t blinkLedGetCounter(BlinkLed *blinkLed)
{
	return blinkLed->counter;
}

void blinkLedSetCounter(BlinkLed *blinkLed, uint8_t counter)
{
	blinkLed->counter = counter;
}

uint8_t blinkLedGetBlinkPattern(BlinkLed *blinkLed)
{
	return blinkLed->blinkPattern;
}

void blinkLedSetBlinkPattern(BlinkLed *blinkLed, uint8_t blinkPattern)
{
	blinkLed->blinkPattern = blinkPattern;
	blinkLedSetCounter(blinkLed, 0);
}
