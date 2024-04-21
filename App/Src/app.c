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

	// ======== Comm ======== //
	commInit(&app->comm, huart, huartDebug);

	// ======== Controller =========== //
	controllerInit(&app->controller, hdac);

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
	controllerRunPidController(&app->controller);
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

void appDecodeReceivedData(App *app)
{
	commTryDecodeReceivedDataPacket(&app->comm);
}

void appExtractCommandAndPayloadFromDecodedData(App *app)
{
	commTryExtractCommandAndPayloadFromDecodedDataPacket(&app->comm);
}

void appDecodeExtractedCommand(App *app)
{
	commTryDecodeExtractedCommand(&app->comm, app);
}

// ======== Data Packet Tx =========== //
void appSendData(App *app)
{
	commTrySendData(&app->comm, app);
}

// ======= Getters and Setters ======== //
float appGetPidKp(App *app)
{
	return controllerGetPidKp(&app->controller);
}

void appSetPidKp(App *app, float kp)
{
	controllerSetPidKp(&app->controller, kp);
}

float appGetPidKi(App *app)
{
	return controllerGetPidKi(&app->controller);
}

void appSetPidKi(App *app, float ki)
{
	controllerSetPidKi(&app->controller, ki);
}

float appGetPidKd(App *app)
{
	return controllerGetPidKd(&app->controller);
}

void appSetPidKd(App *app, float kd)
{
	controllerSetPidKd(&app->controller, kd);
}

uint16_t appGetPidInterval(App *app)
{
	return (uint16_t) (10000 * controllerGetPidInterval(&app->controller));
}

void appSetPidInterval(App *app, uint16_t pidInterval)
{
	if ((pidInterval >= 0) && (pidInterval <= DELAY_5000_MILISECONDS))
	{
		controllerSetPidInterval(&app->controller, ((float) pidInterval) / 10000);
	}
}

int32_t appGetPidMinSumOfErrors(App *app)
{
	return controllerGetPidMinSumOfErrors(&app->controller);
}

void appSetPidMinSumOfErrors(App *app, int32_t minSumOfErrors)
{
	controllerSetPidMinSumOfErrors(&app->controller, minSumOfErrors);
}

int32_t appGetPidMaxSumOfErrors(App *app)
{
	return controllerGetPidMaxSumOfErrors(&app->controller);
}

void appSetPidMaxSumOfErrors(App *app, int32_t maxSumOfErrors)
{
	controllerSetPidMaxSumOfErrors(&app->controller, maxSumOfErrors);
}

int32_t appGetPidMinControlledVariable(App *app)
{
	return controllerGetPidMinControlledVariable(&app->controller);
}

void appSetPidMinControlledVariable(App *app, int32_t minControlledVariable)
{
	controllerSetPidMinControlledVariable(&app->controller, minControlledVariable);
}

int32_t appGetPidMaxControlledVariable(App *app)
{
	return controllerGetPidMaxControlledVariable(&app->controller);
}

void appSetPidMaxControlledVariable(App *app, int32_t maxControlledVariable)
{
	controllerSetPidMaxControlledVariable(&app->controller, maxControlledVariable);
}

float appGetPidOffset(App *app)
{
	return controllerGetPidOffset(&app->controller);
}

void appSetPidOffset(App *app, float offset)
{
	controllerSetPidOffset(&app->controller, offset);
}

float appGetPidBias(App *app)
{
	return controllerGetPidBias(&app->controller);
}

void appSetPidBias(App *app, float bias)
{
	controllerSetPidBias(&app->controller, bias);
}

float appGetPidSetpoint(App *app)
{
	return controllerGetPidSetpoint(&app->controller);
}

void appSetPidSetpoint(App *app, float setpoint)
{
	if ((setpoint >= MIN_CURRENT_IN_MICRO_AMPS) && (setpoint <= MAX_CURRENT_IN_MICRO_AMPS))
	{
		controllerSetPidSetpoint(&app->controller, setpoint);
		commSetEnableSendCurrentPidSetpointValue(&app->comm, TRUE);
	}
}

float appGetPidProcessVariable(App *app)
{
	return controllerGetPidProcessVariable(&app->controller);
}

void appSetPidProcessVariable(App *app, uint32_t value)
{
	controllerSetPidProcessVariable(&app->controller, (float) value);
}

Bool appGetRunPidControllerStatus(App *app)
{
	return controllerGetRunPidControllerStatus(&app->controller);
}

void appSetRunPidControllerStatus(App *app, Bool status)
{
	controllerSetRunPidControllerStatus(&app->controller, status);
}

uint16_t appGetSamplingInterval(App *app)
{
	return controllerGetSamplingInterval(&app->controller);
}

void appSetSamplingInterval(App *app, uint16_t samplingInterval)
{
	if ((samplingInterval >= 0) && (samplingInterval <= DELAY_5000_MILISECONDS))
	{
		controllerSetSamplingInterval(&app->controller, samplingInterval);
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

Bool appGetEnableSendKeepAliveMessage(App *app)
{
	return commGetEnableSendKeepAliveMessage(&app->comm);
}

void appSetEnableSendKeepAliveMessage(App *app, Bool status)
{
	commSetEnableSendKeepAliveMessage(&app->comm, status);
}
