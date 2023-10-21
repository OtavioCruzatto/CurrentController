/*
 * app.h
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "main.h"

#define QTY_DATA_BYTES	25

typedef enum APP_RX_COMMANDS
{
	CMD_RX_ASK_FOR_SEND_PROCESS_VARIABLE = 0x22,
	CMD_RX_ASK_FOR_PID_KS_PARAMETERS = 0x13,
	CMD_RX_ASK_FOR_PID_CONTROLLER_PARAMETERS = 0x16,
	CMD_RX_SET_PID_KP_PARAMETER = 0x10,
	CMD_RX_SET_PID_KI_PARAMETER = 0x11,
	CMD_RX_SET_PID_KD_PARAMETER = 0x12,
	CMD_RX_SET_SAMPLING_INTERVAL = 0x18,
	CMD_RX_SET_PID_INTERVAL = 0x19,
	CMD_RX_SET_PID_SETPOINT = 0x20,
	CMD_RX_ASK_FOR_RUN_PID_CONTROLLER = 0x15
} CommandsFromComputer;

typedef enum APP_TX_COMMANDS
{
	CMD_TX_PROCESS_VARIABLE_VALUE = 0x21,
	CMD_TX_PID_KS_PARAMETER_VALUES = 0x14,
	CMD_TX_PID_CONTROLLER_PARAMETER_VALUES = 0x17,
} CommandsToComputer;

typedef struct
{
	// ======== LED =========== //
	uint32_t blinkDelay;
	GPIO_TypeDef* ledPort;
	uint16_t ledPin;

	// ======== UART =========== //
	UART_HandleTypeDef huart;

	// ======== DAC ============ //
	DAC_HandleTypeDef hdac;

	// ======== Controller =========== //
	PidController pid;
	uint16_t samplingInterval;
	uint16_t pidInterval;
	Bool runPidController;

	// ======== Filter =========== //
	MovingAverage movingAverageFilter;

	// ======== Data Packet Tx =========== //
	DataPacketTx dataPacketTx;
	Bool processVariableReadyToSend;
	Bool enableSendProcessVariable;
	Bool enableSendPidKsParameterValues;
	Bool enableSendPidControllerParameterValues;

	// ======== Data Packet Rx =========== //
	DataPacketRx dataPacketRx;
	Bool decodeCommandStatus;
	uint8_t command;
	uint8_t data[QTY_DATA_BYTES];
	uint8_t dataLenght;
} App;

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin, UART_HandleTypeDef huart, DAC_HandleTypeDef hdac);

// ======== LED =========== //
void appExecuteBlinkLed(App *app);
uint32_t appGetBlinkDelay(App *app);

// ======== Controller =========== //
void appRunController(App *app);
void appSetProcessVariable(App *app, uint32_t value);
uint32_t appGetProcessVariable(App *app);
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
void appSendPidKsParameterValues(App *app);
void appTrySendData(App *app);
void appSetProcessVariableReadyToSend(App *app, Bool status);
Bool appGetProcessVariableReadyToSend(App *app);
Bool appGetEnableSendProcessVariable(App *app);
void appSetEnableSendProcessVariable(App *app, Bool status);
Bool appGetEnableSendPidKsParameterValues(App *app);
void appSetEnableSendPidKsParameterValues(App *app, Bool status);
Bool appGetEnableSendPidControllerParameterValues(App *app);
void appSetEnableSendPidControllerParameterValues(App *app, Bool status);

#endif /* INC_APP_H_ */
