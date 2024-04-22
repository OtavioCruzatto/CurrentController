/*
 * blinkLed.h
 *
 *  Created on: Apr 22, 2024
 *      Author: Otavio
 */

#ifndef INC_BLINKLED_H_
#define INC_BLINKLED_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "defs.h"

typedef struct
{
	uint32_t blinkDelay;
	uint8_t counter;
	uint8_t blinkPattern;
	GPIO_TypeDef* ledPort;
	uint16_t ledPin;
} BlinkLed;

typedef enum BLINK_PATTERNS
{
	PATTERN_ALWAYS_OFF = 0x00,
	PATTERN_TOGGLE_EACH_100_MS = 0x01,
	PATTERN_SHORT_SHORT_LONG = 0x02,
	PATTERN_HEART_BEAT = 0x03,
	PATTERN_TOGGLE_EACH_250_MS = 0x04,
	PATTERN_ALWAYS_ON = 0x05
} BlinkPatterns;

// ======== Init =========== //
void blinkLedInit(BlinkLed *blinkLed, GPIO_TypeDef* ledPort, uint16_t ledPin, uint8_t blinkPattern);

// ======== LED =========== //
void blinkLedExecuteBlink(BlinkLed *blinkLed);

// ======= Getters and Setters ======== //
uint32_t blinkLedGetBlinkDelay(BlinkLed *blinkLed);
void blinkLedSetBlinkDelay(BlinkLed *blinkLed, uint32_t blinkDelay);
uint8_t blinkLedGetCounter(BlinkLed *blinkLed);
void blinkLedSetCounter(BlinkLed *blinkLed, uint8_t counter);
uint8_t blinkLedGetBlinkPattern(BlinkLed *blinkLed);
void blinkLedSetBlinkPattern(BlinkLed *blinkLed, uint8_t blinkPattern);

#endif /* INC_BLINKLED_H_ */
