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
	CMD_RX_ASK_FOR_SEND_PROCESS_VARIABLE = 0x22
} CommandsFromComputer;

typedef enum APP_TX_COMMANDS
{
	CMD_TX_PROCESS_VARIABLE_VALUE = 0x21
} CommandsToComputer;

typedef struct
{
	// ======== LED =========== //
	uint32_t blinkDelay;
	GPIO_TypeDef* ledPort;
	uint16_t ledPin;

	// ======== Controller =========== //
	PidController pid;

	// ======== Data Packet Tx =========== //
	DataPacketTx dataPacketTx;
	Bool processVariableReadyToSend;
	Bool enableSendProcessVariable;

	// ======== Data Packet Rx =========== //
	DataPacketRx dataPacketRx;
	Bool decodeCommandStatus;
	uint8_t command;
	uint8_t data[QTY_DATA_BYTES];
	uint8_t dataLenght;
} App;

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin);

// ======== LED =========== //
void appExecuteBlinkLed(App *app);
uint32_t appGetBlinkDelay(App *app);

// ======== Controller =========== //
void appRunController(App *app, DAC_HandleTypeDef hdac);
void appSetProcessVariable(App *app, uint16_t value);
uint16_t appGetProcessVariable(App *app);

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
void appSendProcessVariable(App *app, UART_HandleTypeDef huart);
void appTrySendData(App *app, UART_HandleTypeDef huart);
void appSetProcessVariableReadyToSend(App *app, Bool status);
Bool appGetProcessVariableReadyToSend(App *app);
Bool appGetEnableSendProcessVariable(App *app);

#endif /* INC_APP_H_ */
