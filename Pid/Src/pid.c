/*
 * pid.c
 *
 *  Created on: Sep 17, 2023
 *      Author: Otavio
 */

#include "pid.h"
#include "defs.h"

void pidInit(PidController *pidController, float kp, float ki, float kd, float offset, float bias, ControllerTopology controllerTopology)
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
	pidController->minSumOfErrors = -10000;
	pidController->maxSumOfErrors = 10000;
	pidController->minControlledVariable = 0;
	pidController->maxControlledVariable = 4095;
	pidController->offset = offset;
	pidController->bias = bias;
	pidController->interval = ((float) DELAY_25_MILISECONDS) / 10000;
}

void pidCompute(PidController *pidController)
{
	pidController->currentError = pidController->setpoint - pidController->processVariable;
	pidController->sumOfErrors += pidController->currentError;
	pidController->differenceOfErrors = pidController->currentError - pidController->previousError;

	if (pidController->sumOfErrors > pidController->maxSumOfErrors)
	{
		pidController->sumOfErrors = pidController->maxSumOfErrors;
	}
	else if (pidController->sumOfErrors < pidController->minSumOfErrors)
	{
		pidController->sumOfErrors = pidController->minSumOfErrors;
	}

	if (pidController->controllerTopology == P_CONTROLLER)
	{
		float proportionalTerm = pidController->kp * pidController->currentError;
		pidController->controlledVariable = proportionalTerm + pidController->bias;
	}
	else if (pidController->controllerTopology == PI_CONTROLLER)
	{
		float proportionalTerm = pidController->kp * pidController->currentError;
		float integralTerm = pidController->ki * pidController->sumOfErrors;
		pidController->controlledVariable = proportionalTerm + integralTerm + pidController->bias;
	}
	else if (pidController->controllerTopology == PID_CONTROLLER)
	{
		float proportionalTerm = pidController->kp * pidController->currentError;
		float integralTerm = pidController->ki * pidController->sumOfErrors;
		float derivativeTerm = pidController->kd * pidController->differenceOfErrors;
		pidController->controlledVariable = proportionalTerm + integralTerm + derivativeTerm + pidController->bias;
	}
	else
	{
		pidController->controlledVariable = 0;
	}

	if (pidController->controlledVariable > pidController->maxControlledVariable)
	{
		pidController->controlledVariable = pidController->maxControlledVariable;
	}
	else if (pidController->controlledVariable < pidController->minControlledVariable)
	{
		pidController->controlledVariable = pidController->minControlledVariable;
	}

	pidController->previousError = pidController->currentError;
}

void pidClearParameters(PidController *pidController)
{
	pidController->controlledVariable = 0;
	pidController->currentError = 0;
	pidController->differenceOfErrors = 0;
	pidController->previousError = 0;
	pidController->processVariable = 0;
	pidController->sumOfErrors = 0;
}

float pidGetKp(PidController *pidController)
{
	return pidController->kp;
}

void pidSetKp(PidController *pidController, float kp)
{
	pidController->kp = kp;
}

float pidGetKi(PidController *pidController)
{
	return pidController->ki;
}

void pidSetKi(PidController *pidController, float ki)
{
	pidController->ki = ki;
}

float pidGetKd(PidController *pidController)
{
	return pidController->kd;
}

void pidSetKd(PidController *pidController, float kd)
{
	pidController->kd = kd;
}

float pidGetInterval(PidController *pidController)
{
	return pidController->interval;
}

void pidSetInterval(PidController *pidController, float interval)
{
	pidController->interval = interval;
}

float pidGetSetpoint(PidController *pidController)
{
	return pidController->setpoint;
}

void pidSetSetpoint(PidController *pidController, float setpoint)
{
	pidController->setpoint = setpoint;
}

float pidGetProcessVariable(PidController *pidController)
{
	return pidController->processVariable;
}

void pidSetProcessVariable(PidController *pidController, float processVariable)
{
	pidController->processVariable = processVariable + pidController->offset;
}

float pidGetControlledVariable(PidController *pidController)
{
	return pidController->controlledVariable;
}

float pidGetOffset(PidController *pidController)
{
	return pidController->offset;
}

void pidSetOffset(PidController *pidController, float offset)
{
	pidController->offset = offset;
}

float pidGetBias(PidController *pidController)
{
	return pidController->bias;
}

void pidSetBias(PidController *pidController, float bias)
{
	pidController->bias = bias;
}

int32_t pidGetMinSumOfErrors(PidController *pidController)
{
	return pidController->minSumOfErrors;
}

void pidSetMinSumOfErrors(PidController *pidController, int32_t minSumOfErrors)
{
	if (minSumOfErrors < MIN_SUM_OF_ERRORS_ALLOWED)
	{
		minSumOfErrors = MIN_SUM_OF_ERRORS_ALLOWED;
	}
	else if (minSumOfErrors > MAX_SUM_OF_ERRORS_ALLOWED)
	{
		minSumOfErrors = MAX_SUM_OF_ERRORS_ALLOWED;
	}
	pidController->minSumOfErrors = minSumOfErrors;
}

int32_t pidGetMaxSumOfErrors(PidController *pidController)
{
	return pidController->maxSumOfErrors;
}

void pidSetMaxSumOfErrors(PidController *pidController, int32_t maxSumOfErrors)
{
	if (maxSumOfErrors < MIN_SUM_OF_ERRORS_ALLOWED)
	{
		maxSumOfErrors = MIN_SUM_OF_ERRORS_ALLOWED;
	}
	else if (maxSumOfErrors > MAX_SUM_OF_ERRORS_ALLOWED)
	{
		maxSumOfErrors = MAX_SUM_OF_ERRORS_ALLOWED;
	}
	pidController->maxSumOfErrors = maxSumOfErrors;
}

int32_t pidGetMinControlledVariable(PidController *pidController)
{
	return pidController->minControlledVariable;
}

void pidSetMinControlledVariable(PidController *pidController, int32_t minControlledVariable)
{
	if (minControlledVariable < MIN_CONTROLLED_VARIABLE_ALLOWED)
	{
		minControlledVariable = MIN_CONTROLLED_VARIABLE_ALLOWED;
	}
	else if (minControlledVariable > MAX_CONTROLLED_VARIABLE_ALLOWED)
	{
		minControlledVariable = MAX_CONTROLLED_VARIABLE_ALLOWED;
	}
	pidController->minControlledVariable = minControlledVariable;
}

int32_t pidGetMaxControlledVariable(PidController *pidController)
{
	return pidController->maxControlledVariable;
}

void pidSetMaxControlledVariable(PidController *pidController, int32_t maxControlledVariable)
{
	if (maxControlledVariable < MIN_CONTROLLED_VARIABLE_ALLOWED)
	{
		maxControlledVariable = MIN_CONTROLLED_VARIABLE_ALLOWED;
	}
	else if (maxControlledVariable > MAX_CONTROLLED_VARIABLE_ALLOWED)
	{
		maxControlledVariable = MAX_CONTROLLED_VARIABLE_ALLOWED;
	}
	pidController->maxControlledVariable = maxControlledVariable;
}
