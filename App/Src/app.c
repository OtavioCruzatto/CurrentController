/*
 * app.c
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#include "app.h"

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin, UART_HandleTypeDef huart, DAC_HandleTypeDef hdac, UART_HandleTypeDef huartDebug)
{
	// ======== LED =========== //
	app->blinkDelay = DELAY_100_MILISECONDS;
	app->ledPort = ledPort;
	app->ledPin = ledPin;

	// ======== UART =========== //
	app->huart = huart;
	app->huartDebug = huartDebug;

	// ======== DAC ============ //
	app->hdac = hdac;

	// ======== Controller =========== //
	pidInit(&app->pid, 50, 2, 100, 2, 0, PID_CONTROLLER);
	pidSetSetpoint(&app->pid, 0);
	app->samplingInterval = DELAY_5_MILISECONDS;
	app->runPidController = FALSE;
	HAL_DAC_SetValue(&app->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);

	// ======== Filter =========== //
	movingAverageInit(&app->movingAverageFilter, 128);

	// ======== Data Packet Tx =========== //
	dataPacketTxInit(&app->dataPacketTx, 0xAA, 0x55);
	app->processVariableReadyToSend = FALSE;
	app->enableSendProcessVariable = FALSE;
	app->enableSendCurrentConfigDataValues = FALSE;
	app->enableSendKeepAliveMessage = FALSE;

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
void appRunController(App *app)
{
	pidCompute(&app->pid);
	uint32_t controlledVariable = (uint32_t) pidGetControlledVariable(&app->pid);
	HAL_DAC_SetValue(&app->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, controlledVariable);
}

void appSetProcessVariable(App *app, uint32_t value)
{
	pidSetProcessVariable(&app->pid, (float) value);
}

float appGetProcessVariable(App *app)
{
	return pidGetProcessVariable(&app->pid);
}

uint32_t appGetCurrentInMiliAmps(uint16_t adcValue)
{
	uint32_t electronicCircuitGain = 10;
	uint32_t shuntResistorInOhms = 1;
  	float measuredSignalInVolts = ((3.3 * adcValue) / 4095);
  	float conditionedSignalInVolts = measuredSignalInVolts / electronicCircuitGain;
  	float calculatedCurrentInAmps = conditionedSignalInVolts / shuntResistorInOhms;
  	float calculatedCurrentInMiliAmpsAux = 1000 * calculatedCurrentInAmps;
  	uint32_t calculatedCurrentInMiliAmps = (uint32_t) calculatedCurrentInMiliAmpsAux;
  	return calculatedCurrentInMiliAmps;
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

	switch (app->command)
	{
		case CMD_RX_SET_CONFIG_DATA_VALUES:

			/************* Kp *************/
			pidKpTimes1000 = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
			pidKp = ((float) pidKpTimes1000) / 1000;
			app->pid.kp = pidKp;

			/************* Ki *************/
			pidKiTimes1000 = (app->data[4] << 24) + (app->data[5] << 16) + (app->data[6] << 8) + app->data[7];
			pidKi = ((float) pidKiTimes1000) / 1000;
			app->pid.ki = pidKi;

			/************* Kd *************/
			pidKdTimes1000 = (app->data[8] << 24) + (app->data[9] << 16) + (app->data[10] << 8) + app->data[11];
			pidKd = ((float) pidKdTimes1000) / 1000;
			app->pid.kd = pidKd;

			/************* Pid Interval *************/
			receivedPidInterval = (app->data[12] << 8) + app->data[13];
			if ((receivedPidInterval >= 0) && (receivedPidInterval <= 50000))
			{
				appSetPidInterval(app, receivedPidInterval);
			}

			/************* Sampling Interval *************/
			receivedSamplingInterval = (app->data[14] << 8) + app->data[15];
			if ((receivedSamplingInterval >= 0) && (receivedSamplingInterval <= 50000))
			{
				app->samplingInterval = receivedSamplingInterval;
			}

			/************* Moving Average Window *************/
			receivedMovingAverageWindow = (app->data[16] << 8) + app->data[17];
			if (receivedMovingAverageWindow > MOV_AVG_FIL_MAX_QTY_OF_ELEMENTS)
			{
				receivedMovingAverageWindow = MOV_AVG_FIL_MAX_QTY_OF_ELEMENTS;
			}
			app->movingAverageFilter.window = receivedMovingAverageWindow;

			/************* Min Sum Of Errors *************/
			receivedPidMinSumOfErrors = (app->data[18] << 24) + (app->data[19] << 16) + (app->data[20] << 8) + app->data[21];
			receivedPidMinSumOfErrors -= 1000000000;
			if (receivedPidMinSumOfErrors < MIN_SUM_OF_ERRORS_ALLOWED)
			{
				receivedPidMinSumOfErrors = MIN_SUM_OF_ERRORS_ALLOWED;
			}
			else if (receivedPidMinSumOfErrors > MAX_SUM_OF_ERRORS_ALLOWED)
			{
				receivedPidMinSumOfErrors = MAX_SUM_OF_ERRORS_ALLOWED;
			}
			app->pid.minSumOfErrors = receivedPidMinSumOfErrors;

			/************* Max Sum Of Errors *************/
			receivedPidMaxSumOfErrors = (app->data[22] << 24) + (app->data[23] << 16) + (app->data[24] << 8) + app->data[25];
			receivedPidMaxSumOfErrors -= 1000000000;
			if (receivedPidMaxSumOfErrors < MIN_SUM_OF_ERRORS_ALLOWED)
			{
				receivedPidMaxSumOfErrors = MIN_SUM_OF_ERRORS_ALLOWED;
			}
			else if (receivedPidMaxSumOfErrors > MAX_SUM_OF_ERRORS_ALLOWED)
			{
				receivedPidMaxSumOfErrors = MAX_SUM_OF_ERRORS_ALLOWED;
			}
			app->pid.maxSumOfErrors = receivedPidMaxSumOfErrors;

			/************* Min Controlled Variable *************/
			receivedPidMinControlledVariable = (app->data[26] << 24) + (app->data[27] << 16) + (app->data[28] << 8) + app->data[29];
			receivedPidMinControlledVariable -= 1000000000;
			if (receivedPidMinControlledVariable < MIN_CONTROLLED_VARIABLE_ALLOWED)
			{
				receivedPidMinControlledVariable = MIN_CONTROLLED_VARIABLE_ALLOWED;
			}
			else if (receivedPidMinControlledVariable > MAX_CONTROLLED_VARIABLE_ALLOWED)
			{
				receivedPidMinControlledVariable = MAX_CONTROLLED_VARIABLE_ALLOWED;
			}
			app->pid.minControlledVariable = receivedPidMinControlledVariable;

			/************* Max Controlled Variable *************/
			receivedPidMaxControlledVariable = (app->data[30] << 24) + (app->data[31] << 16) + (app->data[32] << 8) + app->data[33];
			receivedPidMaxControlledVariable -= 1000000000;
			if (receivedPidMaxControlledVariable < MIN_CONTROLLED_VARIABLE_ALLOWED)
			{
				receivedPidMaxControlledVariable = MIN_CONTROLLED_VARIABLE_ALLOWED;
			}
			else if (receivedPidMaxControlledVariable > MAX_CONTROLLED_VARIABLE_ALLOWED)
			{
				receivedPidMaxControlledVariable = MAX_CONTROLLED_VARIABLE_ALLOWED;
			}
			app->pid.maxControlledVariable = receivedPidMaxControlledVariable;

			/************* Pid Offset *************/
			receiveidPidOffset = (app->data[34] << 24) + (app->data[35] << 16) + (app->data[36] << 8) + app->data[37];
			pidOffset = (((float) receiveidPidOffset) - 1000000) / 1000;
			app->pid.offset = pidOffset;

			/************* Pid Bias *************/
			receiveidPidBias = (app->data[38] << 24) + (app->data[39] << 16) + (app->data[40] << 8) + app->data[41];
			pidBias = (((float) receiveidPidBias) - 1000000) / 1000;
			app->pid.bias = pidBias;

			/************* Send The New Config Data Values *************/
			app->enableSendCurrentConfigDataValues = TRUE;
			break;

		case CMD_RX_ASK_FOR_CURRENT_CONFIG_DATA_VALUES:
			app->enableSendCurrentConfigDataValues = TRUE;
			break;

		case CMD_RX_SET_PID_SETPOINT:
			receivedPidSetpointTimes1000 = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
			receivedPidSetpoint = ((float) receivedPidSetpointTimes1000) / 1000;
			if ((receivedPidSetpoint >= 0) && (receivedPidSetpoint <= 300000))
			{
				app->pid.setpoint = receivedPidSetpoint;
				appSetEnableSendCurrentPidSetpointValue(app, TRUE);
			}
			break;

		case CMD_RX_SET_RUN_PID_CONTROLLER_STATUS:
			if (app->data[0] == 0x00)
			{
				app->runPidController = FALSE;
				app->pid.controlledVariable = 0;
				app->pid.currentError = 0;
				app->pid.differenceOfErrors = 0;
				app->pid.previousError = 0;
				app->pid.processVariable = 0;
				app->pid.sumOfErrors = 0;
				HAL_DAC_SetValue(&app->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
			}
			else if (app->data[0] == 0x01)
			{
				app->runPidController = TRUE;
			}
			break;

		case CMD_RX_SET_SEND_PROCESS_VARIABLE_STATUS:
			if (app->data[0] == 0x00)
			{
				app->enableSendProcessVariable = FALSE;
			}
			else if (app->data[0] == 0x01)
			{
				app->enableSendProcessVariable = TRUE;
			}
			break;

		case CMD_RX_ASK_FOR_CURRENT_PID_SETPOINT_VALUE:
			appSetEnableSendCurrentPidSetpointValue(app, TRUE);
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
void appSendCurrentConfigDataValues(App *app)
{
	uint8_t qtyOfBytes = 42;
	uint8_t bytes[qtyOfBytes];
	uint32_t kpTimes1000 = (uint32_t)(1000 * app->pid.kp);
	uint32_t kiTimes1000 = (uint32_t)(1000 * app->pid.ki);
	uint32_t kdTimes1000 = (uint32_t)(1000 * app->pid.kd);
	uint16_t pidInterval = (uint16_t) (10000 * pidGetInterval(&app->pid));
	uint16_t movingAverageWindow = app->movingAverageFilter.window;
	uint32_t minSumOfErrors = (uint32_t) (app->pid.minSumOfErrors + 1000000000);
	uint32_t maxSumOfErrors = (uint32_t) (app->pid.maxSumOfErrors + 1000000000);
	uint32_t minControlledVariable = (uint32_t) (app->pid.minControlledVariable + 1000000000);
	uint32_t maxControlledVariable = (uint32_t) (app->pid.maxControlledVariable + 1000000000);
	uint32_t offset = (uint32_t) ((app->pid.offset * 1000) + 1000000);
	uint32_t bias = (uint32_t) ((app->pid.bias * 1000) + 1000000);

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
	bytes[14] = ((app->samplingInterval >> 8) & 0x00FF);
	bytes[15] = (app->samplingInterval & 0x00FF);

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

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_CURRENT_CONFIG_DATA_VALUES);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appSendCurrentPidSetpointValue(App *app)
{
	uint8_t qtyOfBytes = 4;
	uint8_t bytes[qtyOfBytes];
	uint32_t setpointTimes1000 = (uint32_t)(1000 * app->pid.setpoint);

	bytes[0] = ((setpointTimes1000 >> 24) & 0x000000FF);
	bytes[1] = ((setpointTimes1000 >> 16) & 0x000000FF);
	bytes[2] = ((setpointTimes1000 >> 8) & 0x000000FF);
	bytes[3] = (setpointTimes1000 & 0x000000FF);

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_CURRENT_PID_SETPOINT);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appSendProcessVariable(App *app)
{
	uint32_t processVariableValue = (uint32_t) appGetProcessVariable(app);
	uint8_t qtyOfBytes = 4;
	uint8_t bytes[qtyOfBytes];
	bytes[0] = ((processVariableValue >> 24) & 0x000000FF);
	bytes[1] = ((processVariableValue >> 16) & 0x000000FF);
	bytes[2] = ((processVariableValue >> 8) & 0x000000FF);
	bytes[3] = (processVariableValue & 0x000000FF);

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_CURRENT_PROCESS_VARIABLE_VALUE);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appSendKeepAliveMessage(App *app)
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

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_KEEP_ALIVE_MESSAGE);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appTrySendData(App *app)
{
	if (appGetEnableSendCurrentConfigDataValues(app) == TRUE)
	{
		appSendCurrentConfigDataValues(app);
		appSetEnableSendCurrentConfigDataValues(app, FALSE);
	}
	else if (appGetEnableSendCurrentPidSetpointValue(app) == TRUE)
	{
		appSendCurrentPidSetpointValue(app);
		appSetEnableSendCurrentPidSetpointValue(app, FALSE);
	}
	else if (appGetEnableSendKeepAliveMessage(app) == TRUE)
	{
		appSendKeepAliveMessage(app);
		appSetEnableSendKeepAliveMessage(app, FALSE);
	}
	else if (appGetEnableSendProcessVariable(app) == TRUE)
	{
		appSendProcessVariable(app);
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

Bool appGetEnableSendCurrentConfigDataValues(App *app)
{
	return app->enableSendCurrentConfigDataValues;
}

void appSetEnableSendCurrentConfigDataValues(App *app, Bool status)
{
	app->enableSendCurrentConfigDataValues = status;
}

void appSetSamplingInterval(App *app, uint16_t samplingInterval)
{
	app->samplingInterval = samplingInterval;
}

uint16_t appGetSamplingInterval(App *app)
{
	return app->samplingInterval;
}

void appSetPidInterval(App *app, uint16_t pidInterval)
{
	pidSetInterval(&app->pid, ((float) pidInterval) / 10000);
}

uint16_t appGetPidInterval(App *app)
{
	return (uint16_t) (10000 * pidGetInterval(&app->pid));
}

Bool appGetRunPidControllerStatus(App *app)
{
	return app->runPidController;
}

void appSetRunPidControllerStatus(App *app, Bool status)
{
	app->runPidController = status;
}

Bool appGetEnableSendCurrentPidSetpointValue(App *app)
{
	return app->enableSendCurrentPidSetpointValue;
}

void appSetEnableSendCurrentPidSetpointValue(App *app, Bool status)
{
	app->enableSendCurrentPidSetpointValue = status;
}

Bool appGetEnableSendKeepAliveMessage(App *app)
{
	return app->enableSendKeepAliveMessage;
}

void appSetEnableSendKeepAliveMessage(App *app, Bool status)
{
	app->enableSendKeepAliveMessage = status;
}
