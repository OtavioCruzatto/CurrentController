/*
 * controller.h
 *
 *  Created on: Apr 21, 2024
 *      Author: Otavio
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include "enums.h"
#include "defs.h"
#include "stm32f4xx_hal.h"
#include "pid.h"

typedef struct
{
	// ======== Controller ============ //
	PidController pid;
	Bool runPidController;

	// ======== DAC ============ //
	DAC_HandleTypeDef hdac;
} Controller;

// ======== Init ======== //
void controllerInit(Controller *controller, DAC_HandleTypeDef hdac);

// ======== Controller =========== //
void controllerRunPidController(Controller *controller);

// ======= Getters and Setters ======== //
float controllerGetPidKp(Controller *controller);
void controllerSetPidKp(Controller *controller, float kp);
float controllerGetPidKi(Controller *controller);
void controllerSetPidKi(Controller *controller, float ki);
float controllerGetPidKd(Controller *controller);
void controllerSetPidKd(Controller *controller, float kd);
float controllerGetPidInterval(Controller *controller);
void controllerSetPidInterval(Controller *controller, float pidInterval);
int32_t controllerGetPidMinSumOfErrors(Controller *controller);
void controllerSetPidMinSumOfErrors(Controller *controller, int32_t minSumOfErrors);
int32_t controllerGetPidMaxSumOfErrors(Controller *controller);
void controllerSetPidMaxSumOfErrors(Controller *controller, int32_t maxSumOfErrors);
int32_t controllerGetPidMinControlledVariable(Controller *controller);
void controllerSetPidMinControlledVariable(Controller *controller, int32_t minControlledVariable);
int32_t controllerGetPidMaxControlledVariable(Controller *controller);
void controllerSetPidMaxControlledVariable(Controller *controller, int32_t maxControlledVariable);
float controllerGetPidOffset(Controller *controller);
void controllerSetPidOffset(Controller *controller, float offset);
float controllerGetPidBias(Controller *controller);
void controllerSetPidBias(Controller *controller, float bias);
float controllerGetPidSetpoint(Controller *controller);
void controllerSetPidSetpoint(Controller *controller, float setpoint);
float controllerGetPidProcessVariable(Controller *controller);
void controllerSetPidProcessVariable(Controller *controller, float value);
Bool controllerGetRunPidControllerStatus(Controller *controller);
void controllerSetRunPidControllerStatus(Controller *controller, Bool status);

#endif /* INC_CONTROLLER_H_ */
