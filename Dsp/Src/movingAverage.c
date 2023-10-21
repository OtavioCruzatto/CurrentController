/*
 * movingAverage.c
 *
 *  Created on: Sep 23, 2023
 *      Author: Otavio
 */

#include "movingAverage.h"

void movingAverageInit(MovingAverage *movingAverage, uint16_t window)
{
	movingAverage->index = 0;
	movingAverage->window = window;
	memset(movingAverage->buffer, 0x00, MOV_AVG_FIL_MAX_QTY_OF_ELEMENTS * sizeof(int32_t));
	movingAverage->sum = 0;
	movingAverage->movingAverage = 0;
}

void movingAverageAddValue(MovingAverage *movingAverage, int32_t newValue)
{
	movingAverage->sum = movingAverage->sum - movingAverage->buffer[movingAverage->index] + newValue;
	movingAverage->buffer[movingAverage->index] = newValue;
	movingAverage->movingAverage = movingAverage->sum / movingAverage->window;
	movingAverage->index = (movingAverage->index + 1) % movingAverage->window;
}

int32_t movingAverageGetMean(MovingAverage *movingAverage)
{
	return movingAverage->movingAverage;
}

