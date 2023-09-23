/*
 * movingAverage.c
 *
 *  Created on: Sep 23, 2023
 *      Author: Otavio
 */

#include "movingAverage.h"

void movingAverageInit(MovingAverage *movingAverage, int16_t length)
{
	movingAverage->index = 0;
	movingAverage->length = length;
	memset(movingAverage->buffer, 0x00, MAX_QTY_OF_ELEMENTS);
	movingAverage->sum = 0;
	movingAverage->movingAverage = 0;
}

void movingAverageAddValue(MovingAverage *movingAverage, int32_t newValue)
{
	movingAverage->sum = movingAverage->sum - movingAverage->buffer[movingAverage->index] + newValue;
	movingAverage->buffer[movingAverage->index] = newValue;
	movingAverage->movingAverage = movingAverage->sum / movingAverage->length;
	movingAverage->index = (movingAverage->index + 1) % movingAverage->length;
}

int32_t movingAverageGetMean(MovingAverage *movingAverage)
{
	return movingAverage->movingAverage;
}

