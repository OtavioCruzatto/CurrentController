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

typedef enum APP_COMMANDS
{
	CMD_SET_SEND_ADC_READ_STATUS = 0x40
} AppCommands;

typedef struct
{
	// ======== LED =========== //
	uint32_t blinkDelay;
	GPIO_TypeDef* ledPort;
	uint16_t ledPin;

	// ======== Decode received commands =========== //
	Bool decodeCommandStatus;
	uint8_t command;
	uint8_t data[QTY_DATA_BYTES];
	uint8_t dataLenght;

	// ======== Controller =========== //
	PidController pid;

	// ======== Data Packet Tx =========== //
	DataPacketTx dataPacketTx;

	// ======== Data Packet Rx =========== //
	DataPacketRx dataPacketRx;

	// ========  =========== //
	Bool adcReadComplete;
	Bool enableSendAdcRead;
	uint16_t adcValue;
} App;

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin);

// ======== LED =========== //
void appExecuteBlinkLed(App *app);
uint32_t appGetBlinkDelay(App *app);

// ======== Decode received commands =========== //
void appDecodeReceivedCommand(App *app);
void appSetCommand(App *app, uint8_t command);
void appSetDecodeStatus(App *app, Bool status);
Bool appGetDecodeStatus(App *app);
void appSetData(App *app, uint8_t *data, uint8_t dataLength);

// ======== Controller =========== //
void appRunController(App *app, DAC_HandleTypeDef hdac);

// ======== Data Packet Tx =========== //
void appSendAdcRead(App *app, UART_HandleTypeDef huart);
void appTrySendData(App *app, UART_HandleTypeDef huart);

// ======== Data Packet Rx =========== //
void appAppendReceivedByte(App *app, uint8_t receivedByte);
void appTryDecodeReceivedDataPacket(App *app);
void appTryExtractCommandAndPayloadFromDecodedDataPacket(App *app);
void appTryDecodeExtractedCommand(App *app);

// ========  =========== //
void appSetAdcReadCompleteStatus(App *app, Bool status);
Bool appGetAdcReadCompleteStatus(App *app);
Bool appGetEnableSendAdcRead(App *app);
void appSetAdcValue(App *app, uint16_t value);
uint16_t appGetAdcValue(App *app);

#endif /* INC_APP_H_ */
