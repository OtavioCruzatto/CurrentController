/*
 * app.h
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "main.h"

#define QTY_DATA_BYTES	QTY_PAYLOAD_RX_DATA_BYTES

typedef enum APP_RX_COMMANDS
{
	CMD_RX_SET_CONFIG_DATA_VALUES = 0x01,
	CMD_RX_ASK_FOR_CURRENT_CONFIG_DATA_VALUES = 0x02,
	CMD_RX_SET_PID_SETPOINT = 0x03,
	CMD_RX_SET_RUN_PID_CONTROLLER_STATUS = 0x04,
	CMD_RX_SET_SEND_PROCESS_VARIABLE_STATUS = 0x05,
	CMD_RX_ASK_FOR_CURRENT_PID_SETPOINT_VALUE = 0x06
} CommandsFromComputer;

typedef enum APP_TX_COMMANDS
{
	CMD_TX_CURRENT_CONFIG_DATA_VALUES = 0x80,
	CMD_TX_CURRENT_PID_SETPOINT = 0x81,
	CMD_TX_CURRENT_PROCESS_VARIABLE_VALUE = 0x82
} CommandsToComputer;

typedef struct
{
	// ======== LED =========== //
	uint32_t blinkDelay;
	GPIO_TypeDef* ledPort;
	uint16_t ledPin;

	// ======== UART =========== //
	UART_HandleTypeDef huart;
	UART_HandleTypeDef huartDebug;

	// ======== DAC ============ //
	DAC_HandleTypeDef hdac;

	// ======== Controller =========== //
	PidController pid;
	uint16_t samplingInterval;
	Bool runPidController;

	// ======== Filter =========== //
	MovingAverage movingAverageFilter;

	// ======== Data Packet Tx =========== //
	DataPacketTx dataPacketTx;
	Bool processVariableReadyToSend;
	Bool enableSendProcessVariable;
	Bool enableSendCurrentConfigDataValues;
	Bool enableSendCurrentPidSetpointValue;

	// ======== Data Packet Rx =========== //
	DataPacketRx dataPacketRx;
	Bool decodeCommandStatus;
	uint8_t command;
	uint8_t data[QTY_DATA_BYTES];
	uint8_t dataLenght;
} App;

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin, UART_HandleTypeDef huart, DAC_HandleTypeDef hdac, UART_HandleTypeDef huartDebug);

// ======== LED =========== //
void appExecuteBlinkLed(App *app);
uint32_t appGetBlinkDelay(App *app);

// ======== Controller =========== //
void appRunController(App *app);
void appSetProcessVariable(App *app, uint32_t value);
float appGetProcessVariable(App *app);
void appSetSamplingInterval(App *app, uint16_t samplingInterval);
uint16_t appGetSamplingInterval(App *app);
void appSetPidInterval(App *app, uint16_t pidInterval);
uint16_t appGetPidInterval(App *app);
Bool appGetRunPidControllerStatus(App *app);
void appSetRunPidControllerStatus(App *app, Bool status);
uint32_t appGetCurrentInMiliAmps(uint16_t adcValue);

// ======== Filter =========== //
void appAddNewValueToFilter(App *app, uint32_t newValue);
uint32_t appGetFilterResult(App *app);

// ======== Data Packet Rx =========== //
void appAppendReceivedByte(App *app, uint8_t receivedByte);
void appTryDecodeReceivedDataPacket(App *app);
void appTryExtractCommandAndPayloadFromDecodedDataPacket(App *app);
void appTryDecodeExtractedCommand(App *app);
void appDecodeReceivedCommand(App *app);
void appSetCommand(App *app, uint8_t command);
void appSetDecodeStatus(App *app, Bool status);
Bool appGetDecodeStatus(App *app);
void appSetData(App *app, uint8_t *data, uint8_t dataLength);

// ======== Data Packet Tx =========== //
void appSendProcessVariable(App *app);
void appSendCurrentConfigDataValues(App *app);
void appSendCurrentPidSetpointValue(App *app);
void appTrySendData(App *app);
Bool appGetProcessVariableReadyToSend(App *app);
void appSetProcessVariableReadyToSend(App *app, Bool status);
Bool appGetEnableSendProcessVariable(App *app);
void appSetEnableSendProcessVariable(App *app, Bool status);
Bool appGetEnableSendCurrentConfigDataValues(App *app);
void appSetEnableSendCurrentConfigDataValues(App *app, Bool status);
Bool appGetEnableSendCurrentPidSetpointValue(App *app);
void appSetEnableSendCurrentPidSetpointValue(App *app, Bool status);

#endif /* INC_APP_H_ */
