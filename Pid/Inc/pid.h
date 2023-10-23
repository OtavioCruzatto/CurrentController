/*
 * pid.h
 *
 *  Created on: Sep 17, 2023
 *      Author: Otavio
 */

#ifndef INC_PID_H_
#define INC_PID_H_

//#include "main.h"
#include <stdint.h>

#define MIN_SUM_OF_ERRORS_ALLOWED	-1000000000
#define MAX_SUM_OF_ERRORS_ALLOWED	1000000000

#define MIN_CONTROLLED_VARIABLE_ALLOWED	0
#define MAX_CONTROLLED_VARIABLE_ALLOWED	4095

typedef enum CONTROLLER_TOPOLOGY
{
	P_CONTROLLER = 0x00,
	PI_CONTROLLER,
	PID_CONTROLLER
} ControllerTopology;

typedef struct
{
	float kp;
	float ki;
	float kd;
	float currentError;
	float previousError;
	float sumOfErrors;
	float setpoint;
	float controlledVariable;
	float processVariable;
	float differenceOfErrors;
	float interval;
	float offset;
	float bias;
	int32_t minSumOfErrors;
	int32_t maxSumOfErrors;
	int32_t minControlledVariable;
	int32_t maxControlledVariable;
	ControllerTopology controllerTopology;
} PidController;

void pidInit(PidController *pidController, float kp, float ki, float kd, float offset, float bias, ControllerTopology controllerTopology);
void pidCompute(PidController *pidController);
void pidSetSetpoint(PidController *pidController, float setpoint);
float pidGetSetpoint(PidController *pidController);
void pidSetProcessVariable(PidController *pidController, float processVariable);
float pidGetProcessVariable(PidController *pidController);
float pidGetControlledVariable(PidController *pidController);
float pidGetInterval(PidController *pidController);
void pidSetInterval(PidController *pidController, float interval);
float pidGetOffset(PidController *pidController);
void pidSetOffset(PidController *pidController, float offset);
float pidGetBias(PidController *pidController);
void pidSetBias(PidController *pidController, float bias);

#endif /* INC_PID_H_ */
