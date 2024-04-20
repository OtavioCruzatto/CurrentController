/*
 * app.c
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#include "app.h"

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin, UART_HandleTypeDef huart, DAC_HandleTypeDef hdac, UART_HandleTypeDef huartDebug)
{
	// ======== LED =========== //
	app->blinkDelay = DELAY_100_MILISECONDS;
	app->ledPort = ledPort;
	app->ledPin = ledPin;

	// ======== DAC ============ //
	app->hdac = hdac;

	// ======== Comm ======== //
	commInit(&app->comm, huart, huartDebug);

	// ======== Controller =========== //
	pidInit(&app->pid, 50, 2, 100, 2, 0, PID_CONTROLLER);
	pidSetSetpoint(&app->pid, 0);
	appSetSamplingInterval(app, DELAY_5_MILISECONDS);
	appSetRunPidControllerStatus(app, FALSE);
	HAL_DAC_SetValue(&app->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);

	// ======== Filter =========== //
	movingAverageInit(&app->movingAverageFilter, 128);
}

// ======== LED =========== //
void appExecuteBlinkLed(App *app)
{
	HAL_GPIO_TogglePin(app->ledPort, app->ledPin);
}

uint32_t appGetBlinkDelay(App *app)
{
	return app->blinkDelay;
}

// ======== Controller =========== //
void appRunController(App *app)
{
	pidCompute(&app->pid);
	uint32_t controlledVariable = (uint32_t) pidGetControlledVariable(&app->pid);
	HAL_DAC_SetValue(&app->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, controlledVariable);
}

void appSetProcessVariable(App *app, uint32_t value)
{
	pidSetProcessVariable(&app->pid, (float) value);
}

float appGetProcessVariable(App *app)
{
	return pidGetProcessVariable(&app->pid);
}

uint32_t appGetCurrentInMiliAmps(uint16_t adcValue)
{
	uint32_t electronicCircuitGain = 10;
	uint32_t shuntResistorInOhms = 1;
  	float measuredSignalInVolts = ((3.3 * adcValue) / 4095);
  	float conditionedSignalInVolts = measuredSignalInVolts / electronicCircuitGain;
  	float calculatedCurrentInAmps = conditionedSignalInVolts / shuntResistorInOhms;
  	float calculatedCurrentInMiliAmpsAux = 1000 * calculatedCurrentInAmps;
  	uint32_t calculatedCurrentInMiliAmps = (uint32_t) calculatedCurrentInMiliAmpsAux;
  	return calculatedCurrentInMiliAmps;
}

void appSetRunControllerStatus(App *app, Bool status)
{
	if (status == FALSE)
	{
		appSetRunPidControllerStatus(app, FALSE);
		pidClearParameters(&app->pid);
		HAL_DAC_SetValue(&app->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
	}
	else
	{
		appSetRunPidControllerStatus(app, TRUE);
	}
}

// ======== Filter =========== //
void appAddNewValueToFilter(App *app, uint32_t newValue)
{
	movingAverageAddValue(&app->movingAverageFilter, newValue);
}

uint32_t appGetFilterResult(App *app)
{
	return movingAverageGetMean(&app->movingAverageFilter);
}

// ======== Data Packet Rx =========== //
void appAppendReceivedByte(App *app, uint8_t receivedByte)
{
	commAppendReceivedByte(&app->comm, receivedByte);
}

void appTryDecodeReceivedDataPacket(App *app)
{
	commTryDecodeReceivedDataPacket(&app->comm);
}

void appTryExtractCommandAndPayloadFromDecodedDataPacket(App *app)
{
	commTryExtractCommandAndPayloadFromDecodedDataPacket(&app->comm);
}

void appTryDecodeExtractedCommand(App *app)
{
	commTryDecodeExtractedCommand(&app->comm, app);
}

// ======== Data Packet Tx =========== //
void appTrySendData(App *app)
{
	commTrySendData(&app->comm, app);
}

// ======= Getters and Setters ======== //
float appGetPidKp(App *app)
{
	return pidGetKp(&app->pid);
}

void appSetPidKp(App *app, float kp)
{
	pidSetKp(&app->pid, kp);
}

float appGetPidKi(App *app)
{
	return pidGetKi(&app->pid);
}

void appSetPidKi(App *app, float ki)
{
	pidSetKi(&app->pid, ki);
}

float appGetPidKd(App *app)
{
	return pidGetKd(&app->pid);
}

void appSetPidKd(App *app, float kd)
{
	pidSetKd(&app->pid, kd);
}

uint16_t appGetPidInterval(App *app)
{
	return (uint16_t) (10000 * pidGetInterval(&app->pid));
}

void appSetPidInterval(App *app, uint16_t pidInterval)
{
	if ((pidInterval >= 0) && (pidInterval <= 50000))
	{
		pidSetInterval(&app->pid, ((float) pidInterval) / 10000);
	}
}

uint16_t appGetSamplingInterval(App *app)
{
	return app->samplingInterval;
}

void appSetSamplingInterval(App *app, uint16_t samplingInterval)
{
	if ((samplingInterval >= 0) && (samplingInterval <= 50000))
	{
		app->samplingInterval = samplingInterval;
	}
}

uint16_t appGetMovingAverageFilterWindow(App *app)
{
	return movingAverageGetWindow(&app->movingAverageFilter);
}

void appSetMovingAverageFilterWindow(App *app, uint16_t movingAverageFilterWindow)
{
	movingAverageSetWindow(&app->movingAverageFilter, movingAverageFilterWindow);
}

int32_t appGetPidMinSumOfErrors(App *app)
{
	return pidGetMinSumOfErrors(&app->pid);
}

void appSetPidMinSumOfErrors(App *app, int32_t minSumOfErrors)
{
	pidSetMinSumOfErrors(&app->pid, minSumOfErrors);
}

int32_t appGetPidMaxSumOfErrors(App *app)
{
	return pidGetMaxSumOfErrors(&app->pid);
}

void appSetPidMaxSumOfErrors(App *app, int32_t maxSumOfErrors)
{
	pidSetMaxSumOfErrors(&app->pid, maxSumOfErrors);
}

int32_t appGetPidMinControlledVariable(App *app)
{
	return pidGetMinControlledVariable(&app->pid);
}

void appSetPidMinControlledVariable(App *app, int32_t minControlledVariable)
{
	pidSetMinControlledVariable(&app->pid, minControlledVariable);
}

int32_t appGetPidMaxControlledVariable(App *app)
{
	return pidGetMaxControlledVariable(&app->pid);
}

void appSetPidMaxControlledVariable(App *app, int32_t maxControlledVariable)
{
	pidSetMaxControlledVariable(&app->pid, maxControlledVariable);
}

float appGetPidOffset(App *app)
{
	return pidGetOffset(&app->pid);
}

void appSetPidOffset(App *app, float offset)
{
	pidSetOffset(&app->pid, offset);
}

float appGetPidBias(App *app)
{
	return pidGetBias(&app->pid);
}

void appSetPidBias(App *app, float bias)
{
	pidSetBias(&app->pid, bias);
}

float appGetPidSetpoint(App *app)
{
	return pidGetSetpoint(&app->pid);
}

void appSetPidSetpoint(App *app, float setpoint)
{
	if ((setpoint >= 0) && (setpoint <= 300000))
	{
		pidSetSetpoint(&app->pid, setpoint);
		commSetEnableSendCurrentPidSetpointValue(&app->comm, TRUE);
	}
}

Bool appGetRunPidControllerStatus(App *app)
{
	return app->runPidController;
}

void appSetRunPidControllerStatus(App *app, Bool status)
{
	app->runPidController = status;
}

Bool appGetEnableSendKeepAliveMessage(App *app)
{
	return commGetEnableSendKeepAliveMessage(&app->comm);
}

void appSetEnableSendKeepAliveMessage(App *app, Bool status)
{
	commSetEnableSendKeepAliveMessage(&app->comm, status);
}
