/*
 * app.c
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#include "app.h"

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin,
			UART_HandleTypeDef huart, DAC_HandleTypeDef hdac,
			UART_HandleTypeDef huartDebug, ADC_HandleTypeDef hadc)
{
	// ======== LED =========== //
	blinkLedInit(&app->blinkLed, ledPort, ledPin, PATTERN_TOGGLE_EACH_100_MS);

	// ======== Comm ======== //
	commInit(&app->comm, huart, huartDebug);

	// ======== Filter =========== //
	movingAverageInit(&app->movingAverageFilter, 128);

	// ======== Sampling =========== //
	samplingInit(&app->sampling, hadc);

	// ======== Controller =========== //
	controllerInit(&app->controller, hdac);

	// ======== Calibration =========== //
	uint32_t y_Imeasured_mA[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45,
								50, 55, 60, 65, 70, 75, 80, 85, 90, 95,
								100, 105, 110, 115, 120, 125, 130, 135, 140, 145,
								150, 155, 160, 165, 170, 175, 180, 185, 190, 195,
								200, 205, 210, 215, 220, 225, 230, 235, 240, 245,
								250, 255, 260, 265, 270, 275, 280, 285, 290, 295,
								300};

	uint32_t x_Icalculated_mA[] = {0, 4, 9, 14, 19, 24, 29, 35, 40, 46,
								  51, 56, 61, 66, 72, 77, 82, 87, 93, 98,
								  103, 108, 113, 118, 124, 129, 134, 139, 144, 149,
								  155, 160, 165, 171, 176, 181, 186, 191, 197, 202,
								  206, 212, 217, 222, 227, 233, 238, 243, 248, 254,
								  259, 264, 269, 274, 280, 285, 290, 295, 300, 305,
								  310};

	uint32_t sizeof_y = sizeof(y_Imeasured_mA) / sizeof(y_Imeasured_mA[0]);
	uint32_t sizeof_x = sizeof(x_Icalculated_mA) / sizeof(x_Icalculated_mA[0]);

	memcpy(app->currentReferenceInMiliAmpsY, y_Imeasured_mA, 4*61);
	memcpy(app->currentCalculatedInMiliAmpsX, x_Icalculated_mA, 4*61);
}

// ======== LED =========== //
void appExecuteBlinkLed(App *app)
{
	blinkLedExecuteBlink(&app->blinkLed);
}

// ======== Controller =========== //
void appRunController(App *app)
{
	controllerRunPidController(&app->controller);
}

// ======== App Calculations =========== //
uint32_t appGetCurrentInMiliAmps(App *app, uint16_t adcValue)
{
	uint32_t electronicCircuitGain = 0;
	float voltageDividerRate = 0;

	if (samplingGetCurrentMagnitudeOrder(&app->sampling) == LOW_CURRENT)
	{
		electronicCircuitGain = 100;
		voltageDividerRate = 0.877;
	}
	else
	{
		electronicCircuitGain = 10;
		voltageDividerRate = 0.955;
	}

	uint32_t shuntResistorInOhms = 1;
  	float measuredSignalInVolts = ((3.3 * adcValue) / 4095);
  	float conditionedSignalInVolts = measuredSignalInVolts / (electronicCircuitGain * voltageDividerRate);
  	float calculatedCurrentInAmps = conditionedSignalInVolts / shuntResistorInOhms;
  	float calculatedCurrentInMiliAmpsAux = 1000 * calculatedCurrentInAmps;
  	uint32_t calculatedCurrentInMiliAmps = (uint32_t) calculatedCurrentInMiliAmpsAux;
  	return calculatedCurrentInMiliAmps;
}

// ======== Sampling =========== //
void appExecuteSampling(App *app)
{
	samplingExecuteAdcRead(&app->sampling);

	uint16_t readAdcValue = samplingGetAdcValue(&app->sampling);
	uint32_t calculatedCurrentInMiliAmps = appGetCurrentInMiliAmps(app, readAdcValue);
	appAddNewValueToFilter(app, calculatedCurrentInMiliAmps);
	uint32_t filteredCurrentInMiliAmps = appGetFilterResult(app);

	if (filteredCurrentInMiliAmps < app->currentCalculatedInMiliAmpsX[0])
	{
		filteredCurrentInMiliAmps = app->currentCalculatedInMiliAmpsX[0];
	}
	else if (filteredCurrentInMiliAmps > app->currentCalculatedInMiliAmpsX[60])
	{
		filteredCurrentInMiliAmps = app->currentCalculatedInMiliAmpsX[60];
	}

	int8_t mainIndex = 0;
	int8_t prevIndex = 0;
	int8_t nextIndex = 0;

	for (mainIndex = 0; mainIndex < 60; mainIndex++)
	{
		if ((filteredCurrentInMiliAmps >= app->currentCalculatedInMiliAmpsX[mainIndex]) && (filteredCurrentInMiliAmps <= app->currentCalculatedInMiliAmpsX[mainIndex+1]))
		{
			prevIndex = mainIndex - 1;
			nextIndex = mainIndex + 1;

			if (prevIndex < 0)
			{
				prevIndex = 0;
			}

			if (nextIndex > 60)
			{
				nextIndex = 60;
			}
			break;
		}
	}

	float x  = (float) filteredCurrentInMiliAmps;
	float x0 = (float) app->currentCalculatedInMiliAmpsX[prevIndex];
	float x1 = (float) app->currentCalculatedInMiliAmpsX[nextIndex];
	float y0 = (float) app->currentReferenceInMiliAmpsY[prevIndex];
	float y1 = (float) app->currentReferenceInMiliAmpsY[nextIndex];
	float y  = (((x - x0) / (x1 - x0)) * (y1 - y0)) + y0;

	uint32_t calibratedCurrentInMiliAmps = (uint32_t) y;

	appSetPidProcessVariable(app, calibratedCurrentInMiliAmps);
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
	if (status == TRUE)
	{
		blinkLedSetBlinkPattern(&app->blinkLed, PATTERN_TOGGLE_EACH_250_MS);
	}
	else
	{
		blinkLedSetBlinkPattern(&app->blinkLed, PATTERN_TOGGLE_EACH_100_MS);
	}

	controllerSetRunPidControllerStatus(&app->controller, status);
}

uint16_t appGetSamplingInterval(App *app)
{
	return samplingGetSamplingInterval(&app->sampling);
}

void appSetSamplingInterval(App *app, uint16_t samplingInterval)
{
	if ((samplingInterval >= 0) && (samplingInterval <= DELAY_5000_MILISECONDS))
	{
		samplingSetSamplingInterval(&app->sampling, samplingInterval);
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

uint32_t appGetBlinkDelay(App *app)
{
	return blinkLedGetBlinkDelay(&app->blinkLed);
}
