/*
 * pid.h
 *
 *  Created on: Sep 17, 2023
 *      Author: Otavio
 */

#ifndef INC_PID_H_
#define INC_PID_H_

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
void pidClearParameters(PidController *pidController);
float pidGetKp(PidController *pidController);
void pidSetKp(PidController *pidController, float kp);
float pidGetKi(PidController *pidController);
void pidSetKi(PidController *pidController, float ki);
float pidGetKd(PidController *pidController);
void pidSetKd(PidController *pidController, float kd);
float pidGetInterval(PidController *pidController);
void pidSetInterval(PidController *pidController, float interval);
float pidGetSetpoint(PidController *pidController);
void pidSetSetpoint(PidController *pidController, float setpoint);
float pidGetProcessVariable(PidController *pidController);
void pidSetProcessVariable(PidController *pidController, float processVariable);
float pidGetControlledVariable(PidController *pidController);
float pidGetOffset(PidController *pidController);
void pidSetOffset(PidController *pidController, float offset);
float pidGetBias(PidController *pidController);
void pidSetBias(PidController *pidController, float bias);
int32_t pidGetMinSumOfErrors(PidController *pidController);
void pidSetMinSumOfErrors(PidController *pidController, int32_t minSumOfErrors);
int32_t pidGetMaxSumOfErrors(PidController *pidController);
void pidSetMaxSumOfErrors(PidController *pidController, int32_t maxSumOfErrors);
int32_t pidGetMinControlledVariable(PidController *pidController);
void pidSetMinControlledVariable(PidController *pidController, int32_t minControlledVariable);
int32_t pidGetMaxControlledVariable(PidController *pidController);
void pidSetMaxControlledVariable(PidController *pidController, int32_t maxControlledVariable);

#endif /* INC_PID_H_ */
