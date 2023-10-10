/*
 * app.c
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#include "app.h"

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin, UART_HandleTypeDef huart)
{
	// ======== LED =========== //
	app->blinkDelay = DELAY_100_MILISECONDS;
	app->ledPort = ledPort;
	app->ledPin = ledPin;

	// ======== UART =========== //
	app->huart = huart;

	// ======== Controller =========== //
	pidInit(&app->pid, 1, 2, 3, PID_CONTROLLER);
	pidSetSetpoint(&app->pid, 500);

	// ======== Data Packet Tx =========== //
	dataPacketTxInit(&app->dataPacketTx, 0xAA, 0x55);
	app->processVariableReadyToSend = FALSE;
	app->enableSendProcessVariable = FALSE;
	app->enableSendPidKsParameterValues = FALSE;

	// ======== Data Packet Rx =========== //
	dataPacketRxInit(&app->dataPacketRx, 0xAA, 0x55);
	app->decodeCommandStatus = FALSE;
	app->command = 0;
	memset(app->data, 0x00, QTY_DATA_BYTES);
	app->dataLenght = 0;
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
void appRunController(App *app, DAC_HandleTypeDef hdac)
{
	// pidSetProcessVariable(&app->pid, app->pid->processVariable);
	pidCompute(&app->pid);
	uint32_t controlledVariable = pidGetControlledVariable(&app->pid);
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, controlledVariable);
}

void appSetProcessVariable(App *app, uint16_t value)
{
	pidSetProcessVariable(&app->pid, value);
}

uint16_t appGetProcessVariable(App *app)
{
	return pidGetProcessVariable(&app->pid);
}

// ======== Data Packet Rx =========== //
void appAppendReceivedByte(App *app, uint8_t receivedByte)
{
	dataPacketRxAppend(&app->dataPacketRx, receivedByte);
}

void appTryDecodeReceivedDataPacket(App *app)
{
	dataPacketRxDecode(&app->dataPacketRx);
}

void appTryExtractCommandAndPayloadFromDecodedDataPacket(App *app)
{
	if (dataPacketRxGetDataPacketStatus(&app->dataPacketRx) == VALID_RX_DATA_PACKET)
	{
		uint8_t receivedCmd = dataPacketRxGetCommand(&app->dataPacketRx);
		uint8_t receivedPayloadDataLength = dataPacketRxGetPayloadDataLength(&app->dataPacketRx);

		if (receivedPayloadDataLength > 0)
		{
			uint8_t *receivedPayloadData = dataPacketRxGetPayloadData(&app->dataPacketRx);
			appSetData(app, receivedPayloadData, receivedPayloadDataLength);
		}

		appSetCommand(app, receivedCmd);
		appSetDecodeStatus(app, TRUE);
		dataPacketRxClear(&app->dataPacketRx);
	}
}

void appTryDecodeExtractedCommand(App *app)
{
	if (appGetDecodeStatus(app) == TRUE)
	{
		appDecodeReceivedCommand(app);
		appSetDecodeStatus(app, FALSE);
	}
}

void appDecodeReceivedCommand(App *app)
{
	switch (app->command)
	{
		case CMD_RX_ASK_FOR_SEND_PROCESS_VARIABLE:
			if (app->data[0] == 0x00)
			{
				app->enableSendProcessVariable = FALSE;
			}
			else if (app->data[0] == 0x01)
			{
				app->enableSendProcessVariable = TRUE;
			}
			break;

		case CMD_RX_ASK_FOR_PID_KS_PARAMETERS:
			app->enableSendPidKsParameterValues = TRUE;
			break;

		default:
			break;
	}
}

void appSetCommand(App *app, uint8_t command)
{
	app->command = command;
}

void appSetDecodeStatus(App *app, Bool status)
{
	app->decodeCommandStatus = status;
}

Bool appGetDecodeStatus(App *app)
{
	return app->decodeCommandStatus;
}

void appSetData(App *app, uint8_t *data, uint8_t dataLength)
{
	if (dataLength <= QTY_DATA_BYTES)
	{
		app->dataLenght = dataLength;
		memcpy(app->data, data, dataLength);
	}
}

// ======== Data Packet Tx =========== //
void appSendProcessVariable(App *app)
{
	uint16_t processVariableValue = appGetProcessVariable(app);
	uint8_t qtyOfBytes = 2;
	uint8_t bytes[qtyOfBytes];
	bytes[0] = ((processVariableValue >> 8) & 0x00FF);
	bytes[1] = (processVariableValue & 0x00FF);

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_PROCESS_VARIABLE_VALUE);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appSendPidKsParameterValues(App *app)
{
	uint8_t qtyOfBytes = 3;
	uint8_t bytes[qtyOfBytes];
	bytes[0] = app->pid.kp;
	bytes[1] = app->pid.ki;
	bytes[2] = app->pid.kd;

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_PID_KS_PARAMETER_VALUES);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appTrySendData(App *app)
{
	if (appGetEnableSendPidKsParameterValues(app) == TRUE)
	{
		appSendPidKsParameterValues(app);
		appSetEnableSendPidKsParameterValues(app, FALSE);
	}
	else if (appGetProcessVariableReadyToSend(app) == TRUE)
	{
		if (appGetEnableSendProcessVariable(app) == TRUE)
		{
			appSendProcessVariable(app);
		}
		appSetProcessVariableReadyToSend(app, FALSE);
	}
}

void appSetProcessVariableReadyToSend(App *app, Bool status)
{
	app->processVariableReadyToSend = status;
}

Bool appGetProcessVariableReadyToSend(App *app)
{
	return app->processVariableReadyToSend;
}

Bool appGetEnableSendProcessVariable(App *app)
{
	return app->enableSendProcessVariable;
}

void appSetEnableSendProcessVariable(App *app, Bool status)
{
	app->enableSendProcessVariable = status;
}

Bool appGetEnableSendPidKsParameterValues(App *app)
{
	return app->enableSendPidKsParameterValues;
}

void appSetEnableSendPidKsParameterValues(App *app, Bool status)
{
	app->enableSendPidKsParameterValues = status;
}
