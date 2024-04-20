/*
 * movingAverage.h
 *
 *  Created on: Sep 23, 2023
 *      Author: Otavio
 */

#ifndef INC_MOVINGAVERAGE_H_
#define INC_MOVINGAVERAGE_H_

#include <stdint.h>
#include <string.h>

#define MOV_AVG_FIL_MAX_QTY_OF_ELEMENTS	300

typedef struct
{
	int16_t index;
	uint16_t window;
	int32_t buffer[MOV_AVG_FIL_MAX_QTY_OF_ELEMENTS];
	int32_t sum;
	int32_t movingAverage;
} MovingAverage;

void movingAverageInit(MovingAverage *movingAverage, uint16_t window);
void movingAverageAddValue(MovingAverage *movingAverage, int32_t newValue);
int32_t movingAverageGetMean(MovingAverage *movingAverage);
uint16_t movingAverageGetWindow(MovingAverage *movingAverage);
void movingAverageSetWindow(MovingAverage *movingAverage, uint16_t window);

#endif /* INC_MOVINGAVERAGE_H_ */
