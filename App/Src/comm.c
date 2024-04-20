/*
 * comm.c
 *
 *  Created on: Apr 19, 2024
 *      Author: Otavio
 */

#include "comm.h"

// ======== Init ======== //
void commInit(Comm *comm, UART_HandleTypeDef huart, UART_HandleTypeDef huartDebug)
{
	// ======== UART =========== //
	comm->huart = huart;
	comm->huartDebug = huartDebug;

	// ======== Data Packet Tx =========== //
	dataPacketTxInit(&comm->dataPacketTx, 0xAA, 0x55);
	commSetEnableSendProcessVariable(comm, FALSE);
	commSetEnableSendCurrentConfigDataValues(comm, FALSE);
	commSetEnableSendCurrentPidSetpointValue(comm, FALSE);
	commSetEnableSendKeepAliveMessage(comm,  FALSE);

	// ======== Data Packet Rx =========== //
	dataPacketRxInit(&comm->dataPacketRx, 0xAA, 0x55);
	commSetDecodeCommandStatus(comm, FALSE);
	commSetCommand(comm, 0x00);
	commClearData(comm);
	commSetDataLenght(comm, 0);
}

// ======== TX - Data Packet =========== //
void commTrySendData(Comm *comm, App *app)
{
	if (commGetEnableSendCurrentConfigDataValues(comm) == TRUE)
	{
		commSendCurrentConfigDataValues(comm, app);
		commSetEnableSendCurrentConfigDataValues(comm, FALSE);
	}
	else if (commGetEnableSendCurrentPidSetpointValue(comm) == TRUE)
	{
		commSendCurrentPidSetpointValue(comm, app);
		commSetEnableSendCurrentPidSetpointValue(comm, FALSE);
	}
	else if (commGetEnableSendKeepAliveMessage(comm) == TRUE)
	{
		commSendKeepAliveMessage(comm, app);
		commSetEnableSendKeepAliveMessage(comm, FALSE);
	}
	else if (commGetEnableSendProcessVariable(comm) == TRUE)
	{
		commSendProcessVariable(comm, app);
	}
}

void commSendCurrentConfigDataValues(Comm *comm, App *app)
{
	uint8_t qtyOfBytes = 42;
	uint8_t bytes[qtyOfBytes];
	uint32_t kpTimes1000 = (uint32_t)(1000 * appGetPidKp(app));
	uint32_t kiTimes1000 = (uint32_t)(1000 * appGetPidKi(app));
	uint32_t kdTimes1000 = (uint32_t)(1000 * appGetPidKd(app));
	uint16_t pidInterval = (uint16_t) appGetPidInterval(app);
	uint16_t samplingInterval = appGetSamplingInterval(app);
	uint16_t movingAverageWindow = appGetMovingAverageFilterWindow(app);
	uint32_t minSumOfErrors = (uint32_t) (appGetPidMinSumOfErrors(app) + 1000000000);
	uint32_t maxSumOfErrors = (uint32_t) (appGetPidMaxSumOfErrors(app) + 1000000000);
	uint32_t minControlledVariable = (uint32_t) (appGetPidMinControlledVariable(app) + 1000000000);
	uint32_t maxControlledVariable = (uint32_t) (appGetPidMaxControlledVariable(app) + 1000000000);
	uint32_t offset = (uint32_t) ((appGetPidOffset(app) * 1000) + 1000000);
	uint32_t bias = (uint32_t) ((appGetPidBias(app) * 1000) + 1000000);

	/************* Kp *************/
	bytes[0] = ((kpTimes1000 >> 24) & 0x000000FF);
	bytes[1] = ((kpTimes1000 >> 16) & 0x000000FF);
	bytes[2] = ((kpTimes1000 >> 8) & 0x000000FF);
	bytes[3] = (kpTimes1000 & 0x000000FF);

	/************* Ki *************/
	bytes[4] = ((kiTimes1000 >> 24) & 0x000000FF);
	bytes[5] = ((kiTimes1000 >> 16) & 0x000000FF);
	bytes[6] = ((kiTimes1000 >> 8) & 0x000000FF);
	bytes[7] = (kiTimes1000 & 0x000000FF);

	/************* Kd *************/
	bytes[8] = ((kdTimes1000 >> 24) & 0x000000FF);
	bytes[9] = ((kdTimes1000 >> 16) & 0x000000FF);
	bytes[10] = ((kdTimes1000 >> 8) & 0x000000FF);
	bytes[11] = (kdTimes1000 & 0x000000FF);

	/************* Pid Interval *************/
	bytes[12] = ((pidInterval >> 8) & 0x00FF);
	bytes[13] = (pidInterval & 0x00FF);

	/************* Sampling Interval *************/
	bytes[14] = ((samplingInterval >> 8) & 0x00FF);
	bytes[15] = (samplingInterval & 0x00FF);

	/************* Moving Average Window *************/
	bytes[16] = ((movingAverageWindow >> 8) & 0x00FF);
	bytes[17] = (movingAverageWindow & 0x00FF);

	/************* Min Sum Of Errors *************/
	bytes[18] = ((minSumOfErrors >> 24) & 0x000000FF);
	bytes[19] = ((minSumOfErrors >> 16) & 0x000000FF);
	bytes[20] = ((minSumOfErrors >> 8) & 0x000000FF);
	bytes[21] = (minSumOfErrors & 0x000000FF);

	/************* Max Sum Of Errors *************/
	bytes[22] = ((maxSumOfErrors >> 24) & 0x000000FF);
	bytes[23] = ((maxSumOfErrors >> 16) & 0x000000FF);
	bytes[24] = ((maxSumOfErrors >> 8) & 0x000000FF);
	bytes[25] = (maxSumOfErrors & 0x000000FF);

	/************* Min Controlled Variable *************/
	bytes[26] = ((minControlledVariable >> 24) & 0x000000FF);
	bytes[27] = ((minControlledVariable >> 16) & 0x000000FF);
	bytes[28] = ((minControlledVariable >> 8) & 0x000000FF);
	bytes[29] = (minControlledVariable & 0x000000FF);

	/************* Max Controlled Variable *************/
	bytes[30] = ((maxControlledVariable >> 24) & 0x000000FF);
	bytes[31] = ((maxControlledVariable >> 16) & 0x000000FF);
	bytes[32] = ((maxControlledVariable >> 8) & 0x000000FF);
	bytes[33] = (maxControlledVariable & 0x000000FF);

	/************* Offset *************/
	bytes[34] = ((offset >> 24) & 0x000000FF);
	bytes[35] = ((offset >> 16) & 0x000000FF);
	bytes[36] = ((offset >> 8) & 0x000000FF);
	bytes[37] = (offset & 0x000000FF);

	/************* Bias *************/
	bytes[38] = ((bias >> 24) & 0x000000FF);
	bytes[39] = ((bias >> 16) & 0x000000FF);
	bytes[40] = ((bias >> 8) & 0x000000FF);
	bytes[41] = (bias & 0x000000FF);

	dataPacketTxSetCommand(&comm->dataPacketTx, CMD_TX_CURRENT_CONFIG_DATA_VALUES);
	dataPacketTxSetPayloadData(&comm->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&comm->dataPacketTx);
	dataPacketTxUartSend(&comm->dataPacketTx, comm->huart);
	dataPacketTxPayloadDataClear(&comm->dataPacketTx);
	dataPacketTxClear(&comm->dataPacketTx);
}

void commSendCurrentPidSetpointValue(Comm *comm, App *app)
{
	uint8_t qtyOfBytes = 4;
	uint8_t bytes[qtyOfBytes];
	uint32_t setpointTimes1000 = (uint32_t)(1000 * appGetPidSetpoint(app));

	bytes[0] = ((setpointTimes1000 >> 24) & 0x000000FF);
	bytes[1] = ((setpointTimes1000 >> 16) & 0x000000FF);
	bytes[2] = ((setpointTimes1000 >> 8) & 0x000000FF);
	bytes[3] = (setpointTimes1000 & 0x000000FF);

	dataPacketTxSetCommand(&comm->dataPacketTx, CMD_TX_CURRENT_PID_SETPOINT);
	dataPacketTxSetPayloadData(&comm->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&comm->dataPacketTx);
	dataPacketTxUartSend(&comm->dataPacketTx, comm->huart);
	dataPacketTxPayloadDataClear(&comm->dataPacketTx);
	dataPacketTxClear(&comm->dataPacketTx);
}

void commSendKeepAliveMessage(Comm *comm, App *app)
{
	uint8_t qtyOfBytes = 1;
	uint8_t bytes[qtyOfBytes];

	if (appGetRunPidControllerStatus(app) == TRUE)
	{
		bytes[0] = 0x01;
	}
	else
	{
		bytes[0] = 0x00;
	}

	dataPacketTxSetCommand(&comm->dataPacketTx, CMD_TX_KEEP_ALIVE_MESSAGE);
	dataPacketTxSetPayloadData(&comm->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&comm->dataPacketTx);
	dataPacketTxUartSend(&comm->dataPacketTx, comm->huart);
	dataPacketTxPayloadDataClear(&comm->dataPacketTx);
	dataPacketTxClear(&comm->dataPacketTx);
}

void commSendProcessVariable(Comm *comm, App *app)
{
	uint32_t processVariableValue = (uint32_t) appGetProcessVariable(app);
	uint8_t qtyOfBytes = 4;
	uint8_t bytes[qtyOfBytes];
	bytes[0] = ((processVariableValue >> 24) & 0x000000FF);
	bytes[1] = ((processVariableValue >> 16) & 0x000000FF);
	bytes[2] = ((processVariableValue >> 8) & 0x000000FF);
	bytes[3] = (processVariableValue & 0x000000FF);

	dataPacketTxSetCommand(&comm->dataPacketTx, CMD_TX_CURRENT_PROCESS_VARIABLE_VALUE);
	dataPacketTxSetPayloadData(&comm->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&comm->dataPacketTx);
	dataPacketTxUartSend(&comm->dataPacketTx, comm->huart);
	dataPacketTxPayloadDataClear(&comm->dataPacketTx);
	dataPacketTxClear(&comm->dataPacketTx);
}

// ======== RX - Data Packet =========== //
void commSetData(Comm *comm, uint8_t *data, uint8_t dataLength)
{
	if (dataLength <= QTY_DATA_BYTES)
	{
		comm->dataLenght = dataLength;
		memcpy(comm->data, data, dataLength);
	}
}

void commClearData(Comm *comm)
{
	memset(comm->data, 0x00, QTY_DATA_BYTES);
}

void commAppendReceivedByte(Comm *comm, uint8_t receivedByte)
{
	dataPacketRxAppend(&comm->dataPacketRx, receivedByte);
}

void commTryDecodeReceivedDataPacket(Comm *comm)
{
	dataPacketRxDecode(&comm->dataPacketRx);
}

void commTryExtractCommandAndPayloadFromDecodedDataPacket(Comm *comm)
{
	if (dataPacketRxGetDataPacketStatus(&comm->dataPacketRx) == VALID_RX_DATA_PACKET)
	{
		uint8_t receivedCmd = dataPacketRxGetCommand(&comm->dataPacketRx);
		uint8_t receivedPayloadDataLength = dataPacketRxGetPayloadDataLength(&comm->dataPacketRx);

		if (receivedPayloadDataLength > 0)
		{
			uint8_t *receivedPayloadData = dataPacketRxGetPayloadData(&comm->dataPacketRx);
			commSetData(comm, receivedPayloadData, receivedPayloadDataLength);
		}

		commSetCommand(comm, receivedCmd);
		commSetDecodeCommandStatus(comm, TRUE);
		dataPacketRxClear(&comm->dataPacketRx);
	}
}

void commTryDecodeExtractedCommand(Comm *comm, App *app)
{
	if (commGetDecodeCommandStatus(comm) == TRUE)
	{
		commDecodeReceivedCommand(comm, app);
		commSetDecodeCommandStatus(comm, FALSE);
	}
}

void commDecodeReceivedCommand(Comm *comm, App *app)
{
	uint16_t receivedSamplingInterval = 0;
	uint16_t receivedPidInterval = 0;
	uint16_t receivedMovingAverageWindow = 0;
	int32_t receivedPidMinSumOfErrors = 0;
	int32_t receivedPidMaxSumOfErrors = 0;
	int32_t receivedPidMinControlledVariable = 0;
	int32_t receivedPidMaxControlledVariable = 0;

	uint32_t receivedPidSetpointTimes1000 = 0;
	float receivedPidSetpoint = 0;

	uint32_t pidKpTimes1000 = 0;
	float pidKp = 0;

	uint32_t pidKiTimes1000 = 0;
	float pidKi = 0;

	uint32_t pidKdTimes1000 = 0;
	float pidKd = 0;

	uint32_t receiveidPidOffset = 0;
	float pidOffset = 0;

	uint32_t receiveidPidBias = 0;
	float pidBias = 0;

	switch (commGetCommand(comm))
	{
		case CMD_RX_SET_CONFIG_DATA_VALUES:

			// ************* Kp ************* //
			pidKpTimes1000 = (comm->data[0] << 24) + (comm->data[1] << 16) + (comm->data[2] << 8) + comm->data[3];
			pidKp = ((float) pidKpTimes1000) / 1000;
			appSetPidKp(app, pidKp);

			// ************* Ki ************* //
			pidKiTimes1000 = (comm->data[4] << 24) + (comm->data[5] << 16) + (comm->data[6] << 8) + comm->data[7];
			pidKi = ((float) pidKiTimes1000) / 1000;
			appSetPidKi(app, pidKi);

			// ************* Kd ************* //
			pidKdTimes1000 = (comm->data[8] << 24) + (comm->data[9] << 16) + (comm->data[10] << 8) + comm->data[11];
			pidKd = ((float) pidKdTimes1000) / 1000;
			appSetPidKd(app, pidKd);

			// ************* Pid Interval ************* //
			receivedPidInterval = (comm->data[12] << 8) + comm->data[13];
			appSetPidInterval(app, receivedPidInterval);

			// ************* Sampling Interval ************* //
			receivedSamplingInterval = (comm->data[14] << 8) + comm->data[15];
			appSetSamplingInterval(app, receivedSamplingInterval);

			// ************* Moving Average Window ************* //
			receivedMovingAverageWindow = (comm->data[16] << 8) + comm->data[17];
			appSetMovingAverageFilterWindow(app, receivedMovingAverageWindow);

			// ************* Min Sum Of Errors ************* //
			receivedPidMinSumOfErrors = (comm->data[18] << 24) + (comm->data[19] << 16) + (comm->data[20] << 8) + comm->data[21];
			receivedPidMinSumOfErrors -= 1000000000;
			appSetPidMinSumOfErrors(app, receivedPidMinSumOfErrors);

			// ************* Max Sum Of Errors ************* //
			receivedPidMaxSumOfErrors = (comm->data[22] << 24) + (comm->data[23] << 16) + (comm->data[24] << 8) + comm->data[25];
			receivedPidMaxSumOfErrors -= 1000000000;
			appSetPidMaxSumOfErrors(app, receivedPidMaxSumOfErrors);

			// ************* Min Controlled Variable ************* //
			receivedPidMinControlledVariable = (comm->data[26] << 24) + (comm->data[27] << 16) + (comm->data[28] << 8) + comm->data[29];
			receivedPidMinControlledVariable -= 1000000000;
			appSetPidMinControlledVariable(app, receivedPidMinControlledVariable);

			// ************* Max Controlled Variable ************* //
			receivedPidMaxControlledVariable = (comm->data[30] << 24) + (comm->data[31] << 16) + (comm->data[32] << 8) + comm->data[33];
			receivedPidMaxControlledVariable -= 1000000000;
			appSetPidMaxControlledVariable(app, receivedPidMaxControlledVariable);

			// ************* Pid Offset ************* //
			receiveidPidOffset = (comm->data[34] << 24) + (comm->data[35] << 16) + (comm->data[36] << 8) + comm->data[37];
			pidOffset = (((float) receiveidPidOffset) - 1000000) / 1000;
			appSetPidOffset(app, pidOffset);

			// ************* Pid Bias ************* //
			receiveidPidBias = (comm->data[38] << 24) + (comm->data[39] << 16) + (comm->data[40] << 8) + comm->data[41];
			pidBias = (((float) receiveidPidBias) - 1000000) / 1000;
			appSetPidBias(app, pidBias);

			// ************* Send The New Config Data Values ************* //
			commSetEnableSendCurrentConfigDataValues(comm, TRUE);
			break;

		case CMD_RX_ASK_FOR_CURRENT_CONFIG_DATA_VALUES:
			commSetEnableSendCurrentConfigDataValues(comm, TRUE);
			break;

		case CMD_RX_SET_PID_SETPOINT:
			receivedPidSetpointTimes1000 = (comm->data[0] << 24) + (comm->data[1] << 16) + (comm->data[2] << 8) + comm->data[3];
			receivedPidSetpoint = ((float) receivedPidSetpointTimes1000) / 1000;
			appSetPidSetpoint(app, receivedPidSetpoint);
			break;

		case CMD_RX_SET_RUN_PID_CONTROLLER_STATUS:
			if (comm->data[0] == 0x00)
			{
				appSetRunControllerStatus(app, FALSE);
			}
			else if (comm->data[0] == 0x01)
			{
				appSetRunControllerStatus(app, TRUE);
			}
			break;

		case CMD_RX_SET_SEND_PROCESS_VARIABLE_STATUS:
			if (comm->data[0] == 0x00)
			{
				commSetEnableSendProcessVariable(comm, FALSE);
			}
			else if (comm->data[0] == 0x01)
			{
				commSetEnableSendProcessVariable(comm, TRUE);
			}
			break;

		case CMD_RX_ASK_FOR_CURRENT_PID_SETPOINT_VALUE:
			commSetEnableSendCurrentPidSetpointValue(comm, TRUE);
			break;

		default:
			break;
	}
}

// ======== TX - Getters and Setters ======== //
Bool commGetEnableSendProcessVariable(Comm *comm)
{
	return comm->enableSendProcessVariable;
}

void commSetEnableSendProcessVariable(Comm *comm, Bool status)
{
	comm->enableSendProcessVariable = status;
}

Bool commGetEnableSendCurrentConfigDataValues(Comm *comm)
{
	return comm->enableSendCurrentConfigDataValues;
}

void commSetEnableSendCurrentConfigDataValues(Comm *comm, Bool status)
{
	comm->enableSendCurrentConfigDataValues = status;
}

Bool commGetEnableSendCurrentPidSetpointValue(Comm *comm)
{
	return comm->enableSendCurrentPidSetpointValue;
}

void commSetEnableSendCurrentPidSetpointValue(Comm *comm, Bool status)
{
	comm->enableSendCurrentPidSetpointValue = status;
}

Bool commGetEnableSendKeepAliveMessage(Comm *comm)
{
	return comm->enableSendKeepAliveMessage;
}

void commSetEnableSendKeepAliveMessage(Comm *comm, Bool status)
{
	comm->enableSendKeepAliveMessage = status;
}

// ======== RX - Getters and Setters ======== //
Bool commGetDecodeCommandStatus(Comm *comm)
{
	return comm->decodeCommandStatus;
}

void commSetDecodeCommandStatus(Comm *comm, Bool status)
{
	comm->decodeCommandStatus = status;
}

uint8_t commGetCommand(Comm *comm)
{
	return comm->command;
}

void commSetCommand(Comm *comm, uint8_t command)
{
	comm->command = command;
}

uint8_t commGetDataLenght(Comm *comm)
{
	return comm->dataLenght;
}

void commSetDataLenght(Comm *comm, uint8_t dataLenght)
{
	comm->dataLenght = dataLenght;
}
