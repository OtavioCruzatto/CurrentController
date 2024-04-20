/*
 * comm.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Otavio
 */

#ifndef INC_COMM_H_
#define INC_COMM_H_

#include "dataPacketTx.h"
#include "dataPacketRx.h"

#define QTY_DATA_BYTES	QTY_PAYLOAD_RX_DATA_BYTES

typedef struct
{
	// ======== UART =========== //
	UART_HandleTypeDef huart;
	UART_HandleTypeDef huartDebug;

	// ======== Data Packet Tx =========== //
	DataPacketTx dataPacketTx;
	Bool enableSendProcessVariable;
	Bool enableSendCurrentConfigDataValues;
	Bool enableSendCurrentPidSetpointValue;
	Bool enableSendKeepAliveMessage;

	// ======== Data Packet Rx =========== //
	DataPacketRx dataPacketRx;
	Bool decodeCommandStatus;
	uint8_t command;
	uint8_t data[QTY_DATA_BYTES];
	uint8_t dataLenght;
} Comm;

typedef enum APP_COMM_RX_COMMANDS
{
	CMD_RX_SET_CONFIG_DATA_VALUES = 0x01,
	CMD_RX_ASK_FOR_CURRENT_CONFIG_DATA_VALUES = 0x02,
	CMD_RX_SET_PID_SETPOINT = 0x03,
	CMD_RX_SET_RUN_PID_CONTROLLER_STATUS = 0x04,
	CMD_RX_SET_SEND_PROCESS_VARIABLE_STATUS = 0x05,
	CMD_RX_ASK_FOR_CURRENT_PID_SETPOINT_VALUE = 0x06
} CommandsFromComputer;

typedef enum APP_COMM_TX_COMMANDS
{
	CMD_TX_CURRENT_CONFIG_DATA_VALUES = 0x80,
	CMD_TX_CURRENT_PID_SETPOINT = 0x81,
	CMD_TX_CURRENT_PROCESS_VARIABLE_VALUE = 0x82,
	CMD_TX_KEEP_ALIVE_MESSAGE = 0x83
} CommandsToComputer;

#include "app.h"

// ======== Init =========== //
void commInit(Comm *comm, UART_HandleTypeDef huart, UART_HandleTypeDef huartDebug);

// ======== TX - Data Packet =========== //
void commTrySendData(Comm *comm, App *app);
void commSendCurrentConfigDataValues(Comm *comm, App *app);
void commSendCurrentPidSetpointValue(Comm *comm, App *app);
void commSendKeepAliveMessage(Comm *comm, App *app);
void commSendProcessVariable(Comm *comm, App *app);

// ======== RX - Data Packet =========== //
void commSetData(Comm *comm, uint8_t *data, uint8_t dataLength);
void commClearData(Comm *comm);
void commAppendReceivedByte(Comm *comm, uint8_t receivedByte);
void commTryDecodeReceivedDataPacket(Comm *comm);
void commTryExtractCommandAndPayloadFromDecodedDataPacket(Comm *comm);
void commTryDecodeExtractedCommand(Comm *comm, App *app);
void commDecodeReceivedCommand(Comm *comm, App *app);

// ======== TX - Getters and Setters ======== //
Bool commGetEnableSendProcessVariable(Comm *comm);
void commSetEnableSendProcessVariable(Comm *comm, Bool status);
Bool commGetEnableSendCurrentConfigDataValues(Comm *comm);
void commSetEnableSendCurrentConfigDataValues(Comm *comm, Bool status);
Bool commGetEnableSendCurrentPidSetpointValue(Comm *comm);
void commSetEnableSendCurrentPidSetpointValue(Comm *comm, Bool status);
Bool commGetEnableSendKeepAliveMessage(Comm *comm);
void commSetEnableSendKeepAliveMessage(Comm *comm, Bool status);

// ======== RX - Getters and Setters ======== //
Bool commGetDecodeCommandStatus(Comm *comm);
void commSetDecodeCommandStatus(Comm *comm, Bool status);
uint8_t commGetCommand(Comm *comm);
void commSetCommand(Comm *comm, uint8_t command);
uint8_t commGetDataLenght(Comm *comm);
void commSetDataLenght(Comm *comm, uint8_t dataLenght);

#endif /* INC_COMM_H_ */
