/*
 * controller.c
 *
 *  Created on: Apr 21, 2024
 *      Author: Otavio
 */

#include "controller.h"

// ======== Init ======== //
void controllerInit(Controller *controller, DAC_HandleTypeDef hdac)
{
	controller->hdac = hdac;

	pidInit(&controller->pid, 50, 2, 100, 0, 0, PID_CONTROLLER);
	pidSetSetpoint(&controller->pid, 0);
	controllerSetRunPidControllerStatus(controller, FALSE);
	HAL_DAC_SetValue(&controller->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
}

// ======== Controller =========== //
void controllerRunPidController(Controller *controller)
{
	pidCompute(&controller->pid);
	uint32_t controlledVariable = (uint32_t) pidGetControlledVariable(&controller->pid);
	HAL_DAC_SetValue(&controller->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, controlledVariable);
}

// ======= Getters and Setters ======== //
float controllerGetPidKp(Controller *controller)
{
	return pidGetKp(&controller->pid);
}

void controllerSetPidKp(Controller *controller, float kp)
{
	pidSetKp(&controller->pid, kp);
}

float controllerGetPidKi(Controller *controller)
{
	return pidGetKi(&controller->pid);
}

void controllerSetPidKi(Controller *controller, float ki)
{
	pidSetKi(&controller->pid, ki);
}

float controllerGetPidKd(Controller *controller)
{
	return pidGetKd(&controller->pid);
}

void controllerSetPidKd(Controller *controller, float kd)
{
	pidSetKd(&controller->pid, kd);
}

float controllerGetPidInterval(Controller *controller)
{
	return pidGetInterval(&controller->pid);
}

void controllerSetPidInterval(Controller *controller, float pidInterval)
{
	pidSetInterval(&controller->pid, pidInterval);
}

int32_t controllerGetPidMinSumOfErrors(Controller *controller)
{
	return pidGetMinSumOfErrors(&controller->pid);
}

void controllerSetPidMinSumOfErrors(Controller *controller, int32_t minSumOfErrors)
{
	pidSetMinSumOfErrors(&controller->pid, minSumOfErrors);
}

int32_t controllerGetPidMaxSumOfErrors(Controller *controller)
{
	return pidGetMaxSumOfErrors(&controller->pid);
}

void controllerSetPidMaxSumOfErrors(Controller *controller, int32_t maxSumOfErrors)
{
	pidSetMaxSumOfErrors(&controller->pid, maxSumOfErrors);
}

int32_t controllerGetPidMinControlledVariable(Controller *controller)
{
	return pidGetMinControlledVariable(&controller->pid);
}

void controllerSetPidMinControlledVariable(Controller *controller, int32_t minControlledVariable)
{
	pidSetMinControlledVariable(&controller->pid, minControlledVariable);
}

int32_t controllerGetPidMaxControlledVariable(Controller *controller)
{
	return pidGetMaxControlledVariable(&controller->pid);
}

void controllerSetPidMaxControlledVariable(Controller *controller, int32_t maxControlledVariable)
{
	pidSetMaxControlledVariable(&controller->pid, maxControlledVariable);
}

float controllerGetPidOffset(Controller *controller)
{
	return pidGetOffset(&controller->pid);
}

void controllerSetPidOffset(Controller *controller, float offset)
{
	pidSetOffset(&controller->pid, offset);
}

float controllerGetPidBias(Controller *controller)
{
	return pidGetBias(&controller->pid);
}

void controllerSetPidBias(Controller *controller, float bias)
{
	pidSetBias(&controller->pid, bias);
}

float controllerGetPidSetpoint(Controller *controller)
{
	return pidGetSetpoint(&controller->pid);
}

void controllerSetPidSetpoint(Controller *controller, float setpoint)
{
	pidSetSetpoint(&controller->pid, setpoint);
}

float controllerGetPidProcessVariable(Controller *controller)
{
	return pidGetProcessVariable(&controller->pid);
}

void controllerSetPidProcessVariable(Controller *controller, float value)
{
	pidSetProcessVariable(&controller->pid, value);
}

Bool controllerGetRunPidControllerStatus(Controller *controller)
{
	return controller->runPidController;
}

void controllerSetRunPidControllerStatus(Controller *controller, Bool status)
{
	if (status == FALSE)
	{
		pidClearParameters(&controller->pid);
		HAL_DAC_SetValue(&controller->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
	}

	controller->runPidController = status;
}
