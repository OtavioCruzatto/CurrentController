/*
 * movingAverage.h
 *
 *  Created on: Sep 23, 2023
 *      Author: Otavio
 */

#ifndef INC_MOVINGAVERAGE_H_
#define INC_MOVINGAVERAGE_H_

//#include "main.h"
#include <stdint.h>
#include <string.h>

#define MAX_QTY_OF_ELEMENTS	300

typedef struct
{
	int16_t index;
	int16_t window;
	int32_t buffer[MAX_QTY_OF_ELEMENTS];
	int32_t sum;
	int32_t movingAverage;
} MovingAverage;

void movingAverageInit(MovingAverage *movingAverage, int16_t window);
void movingAverageAddValue(MovingAverage *movingAverage, int32_t newValue);
int32_t movingAverageGetMean(MovingAverage *movingAverage);

#endif /* INC_MOVINGAVERAGE_H_ */
