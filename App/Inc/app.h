/*
 * app.h
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "controller.h"
#include "enums.h"
#include "defs.h"
#include "movingAverage.h"
#include "blinkLed.h"
#include "sampling.h"

#define MIN_CURRENT_IN_MICRO_AMPS	0
#define MAX_CURRENT_IN_MICRO_AMPS	300000

typedef struct App App;

#include "comm.h"

struct App
{
	// ======== LED =========== //
	BlinkLed blinkLed;

	// ======== Comm ======== //
	Comm comm;

	// ======== Filter =========== //
	MovingAverage movingAverageFilter;

	// ======== Sampling =========== //
	Sampling sampling;

	// ======== Controller =========== //
	Controller controller;
};

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin,
			UART_HandleTypeDef huart, DAC_HandleTypeDef hdac,
			UART_HandleTypeDef huartDebug, ADC_HandleTypeDef hadc);

// ======== LED =========== //
void appExecuteBlinkLed(App *app);

// ======== Controller =========== //
void appRunController(App *app);

// ======== App Calculations =========== //
uint32_t appGetCurrentInMiliAmps(App *app, uint16_t adcValue);

// ======== Sampling =========== //
void appExecuteSampling(App *app);

// ======== Filter =========== //
void appAddNewValueToFilter(App *app, uint32_t newValue);
uint32_t appGetFilterResult(App *app);

// ======== Data Packet Rx =========== //
void appAppendReceivedByte(App *app, uint8_t receivedByte);
void appDecodeReceivedData(App *app);
void appExtractCommandAndPayloadFromDecodedData(App *app);
void appDecodeExtractedCommand(App *app);

// ======== Data Packet Tx =========== //
void appSendData(App *app);

// ======= Getters and Setters ======== //
float appGetPidKp(App *app);
void appSetPidKp(App *app, float kp);
float appGetPidKi(App *app);
void appSetPidKi(App *app, float ki);
float appGetPidKd(App *app);
void appSetPidKd(App *app, float kd);
uint16_t appGetPidInterval(App *app);
void appSetPidInterval(App *app, uint16_t pidInterval);
uint16_t appGetSamplingInterval(App *app);
void appSetSamplingInterval(App *app, uint16_t samplingInterval);
uint16_t appGetMovingAverageFilterWindow(App *app);
void appSetMovingAverageFilterWindow(App *app, uint16_t movingAverageFilterWindow);
int32_t appGetPidMinSumOfErrors(App *app);
void appSetPidMinSumOfErrors(App *app, int32_t minSumOfErrors);
int32_t appGetPidMaxSumOfErrors(App *app);
void appSetPidMaxSumOfErrors(App *app, int32_t maxSumOfErrors);
int32_t appGetPidMinControlledVariable(App *app);
void appSetPidMinControlledVariable(App *app, int32_t minControlledVariable);
int32_t appGetPidMaxControlledVariable(App *app);
void appSetPidMaxControlledVariable(App *app, int32_t maxControlledVariable);
float appGetPidOffset(App *app);
void appSetPidOffset(App *app, float offset);
float appGetPidBias(App *app);
void appSetPidBias(App *app, float bias);
float appGetPidSetpoint(App *app);
void appSetPidSetpoint(App *app, float setpoint);
float appGetPidProcessVariable(App *app);
void appSetPidProcessVariable(App *app, uint32_t value);
Bool appGetRunPidControllerStatus(App *app);
void appSetRunPidControllerStatus(App *app, Bool status);
Bool appGetEnableSendKeepAliveMessage(App *app);
void appSetEnableSendKeepAliveMessage(App *app, Bool status);
uint32_t appGetBlinkDelay(App *app);

#endif /* INC_APP_H_ */
