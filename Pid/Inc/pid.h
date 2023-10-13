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

#define MAX_SUM_OF_ERRORS			10000
#define MIN_SUM_OF_ERRORS			-10000
#define MAX_CONTROLLED_VARIABLE		4095
#define MIN_CONTROLLED_VARIABLE		0

typedef enum CONTROLLER_TOPOLOGY
{
	P_CONTROLLER = 0x00,
	PI_CONTROLLER,
	PID_CONTROLLER
} ControllerTopology;

typedef struct
{
	uint8_t kp;
	uint8_t ki;
	uint8_t kd;
	int32_t currentError;
	int32_t previousError;
	int32_t sumOfErrors;
	int32_t setpoint;
	int32_t controlledVariable;
	int32_t processVariable;
	int32_t differenceOfErrors;
	ControllerTopology controllerTopology;
} PidController;

void pidInit(PidController *pidController, uint8_t kp, uint8_t ki, uint8_t kd, ControllerTopology controllerTopology);
void pidCompute(PidController *pidController);
void pidSetSetpoint(PidController *pidController, int32_t setpoint);
int32_t pidGetSetpoint(PidController *pidController);
void pidSetProcessVariable(PidController *pidController, int32_t processVariable);
int32_t pidGetProcessVariable(PidController *pidController);
int32_t pidGetControlledVariable(PidController *pidController);

#endif /* INC_PID_H_ */
