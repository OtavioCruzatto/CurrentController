/*
 * app.h
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "pid.h"
#include "enums.h"
#include "defs.h"
#include "movingAverage.h"

typedef struct App App;

#include "comm.h"

struct App
{
	// ======== LED =========== //
	uint32_t blinkDelay;
	GPIO_TypeDef* ledPort;
	uint16_t ledPin;

	// ======== DAC ============ //
	DAC_HandleTypeDef hdac;

	// ======== Controller =========== //
	PidController pid;
	uint16_t samplingInterval;
	Bool runPidController;

	// ======== Filter =========== //
	MovingAverage movingAverageFilter;

	// ======== Comm ======== //
	Comm comm;
};

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin, UART_HandleTypeDef huart, DAC_HandleTypeDef hdac, UART_HandleTypeDef huartDebug);

// ======== LED =========== //
void appExecuteBlinkLed(App *app);
uint32_t appGetBlinkDelay(App *app);

// ======== Controller =========== //
void appRunController(App *app);
void appSetProcessVariable(App *app, uint32_t value);
float appGetProcessVariable(App *app);
Bool appGetRunPidControllerStatus(App *app);
void appSetRunPidControllerStatus(App *app, Bool status);
uint32_t appGetCurrentInMiliAmps(uint16_t adcValue);
void appSetRunControllerStatus(App *app, Bool status);

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
Bool appGetEnableSendKeepAliveMessage(App *app);
void appSetEnableSendKeepAliveMessage(App *app, Bool status);

#endif /* INC_APP_H_ */
