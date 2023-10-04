/*
 * pid.c
 *
 *  Created on: Sep 17, 2023
 *      Author: Otavio
 */

#include "pid.h"

void pidInit(PidController *pidController, uint8_t kp, uint8_t ki, uint8_t kd, ControllerTopology controllerTopology)
{
	pidController->kp = kp;
	pidController->ki = ki;
	pidController->kd = kd;
	pidController->controllerTopology = controllerTopology;
	pidController->currentError = 0;
	pidController->previousError = 0;
	pidController->sumOfErrors = 0;
	pidController->setpoint = 0;
	pidController->controlledVariable = 0;
	pidController->processVariable = 0;
	pidController->differenceOfErrors = 0;
}

void pidCompute(PidController *pidController)
{
	pidController->currentError = pidController->setpoint - pidController->processVariable;
	pidController->sumOfErrors += pidController->currentError;
	pidController->differenceOfErrors = pidController->currentError - pidController->previousError;

	if (pidController->sumOfErrors > MAX_SUM_OF_ERRORS)
	{
		pidController->sumOfErrors = MAX_SUM_OF_ERRORS;
	}
	else if (pidController->sumOfErrors < MIN_SUM_OF_ERRORS)
	{
		pidController->sumOfErrors = MIN_SUM_OF_ERRORS;
	}

	if (pidController->controllerTopology == P_CONTROLLER)
	{
		pidController->controlledVariable = (pidController->kp * pidController->currentError);
	}
	else if (pidController->controllerTopology == PI_CONTROLLER)
	{
		pidController->controlledVariable = (pidController->kp * pidController->currentError) + (pidController->ki * pidController->sumOfErrors);
	}
	else if (pidController->controllerTopology == PID_CONTROLLER)
	{
		pidController->controlledVariable = (pidController->kp * pidController->currentError) + (pidController->ki * pidController->sumOfErrors) + (pidController->kd * pidController->differenceOfErrors);
	}
	else
	{
		pidController->controlledVariable = 0;
	}

	if (pidController->controlledVariable > MAX_CONTROLLED_VARIABLE)
	{
		pidController->controlledVariable = MAX_CONTROLLED_VARIABLE;
	}
	else if (pidController->controlledVariable < MIN_CONTROLLED_VARIABLE)
	{
		pidController->controlledVariable = MIN_CONTROLLED_VARIABLE;
	}

	pidController->previousError = pidController->currentError;
}

void pidSetSetpoint(PidController *pidController, int32_t setpoint)
{
	pidController->setpoint = setpoint;
}

int32_t pidGetSetpoint(PidController *pidController)
{
	return pidController->setpoint;
}

void pidSetProcessVariable(PidController *pidController, int32_t processVariable)
{
	pidController->processVariable = processVariable;
}

int32_t pidGetProcessVariable(PidController *pidController)
{
	return pidController->processVariable;
}

int32_t pidGetControlledVariable(PidController *pidController)
{
	return pidController->controlledVariable;
}
