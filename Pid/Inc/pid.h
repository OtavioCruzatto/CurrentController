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
	int32_t minSumOfErrors;
	int32_t maxSumOfErrors;
	int32_t minControlledVariable;
	int32_t maxControlledVariable;
	ControllerTopology controllerTopology;
} PidController;

void pidInit(PidController *pidController, float kp, float ki, float kd, ControllerTopology controllerTopology);
void pidCompute(PidController *pidController);
void pidSetSetpoint(PidController *pidController, float setpoint);
float pidGetSetpoint(PidController *pidController);
void pidSetProcessVariable(PidController *pidController, float processVariable);
float pidGetProcessVariable(PidController *pidController);
float pidGetControlledVariable(PidController *pidController);

#endif /* INC_PID_H_ */
