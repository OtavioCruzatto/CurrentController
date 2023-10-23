/*
 * app.c
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#include "app.h"

// ======== Init =========== //
void appInit(App *app, GPIO_TypeDef* ledPort, uint16_t ledPin, UART_HandleTypeDef huart, DAC_HandleTypeDef hdac)
{
	// ======== LED =========== //
	app->blinkDelay = DELAY_100_MILISECONDS;
	app->ledPort = ledPort;
	app->ledPin = ledPin;

	// ======== UART =========== //
	app->huart = huart;

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
	app->enableSendPidKsParameterValues = FALSE;
	app->enableSendPidControllerParameterValues = FALSE;
	app->enableSendPidMinAndMaxSumOfErrors = FALSE;
	app->enableSendPidMinAndMaxControlledVariable = FALSE;

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

		case CMD_RX_ASK_FOR_PID_CONTROLLER_PARAMETERS:
			app->enableSendPidControllerParameterValues = TRUE;
			break;

		case CMD_RX_SET_PID_KP_PARAMETER:
			pidKpTimes1000 = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
			pidKp = ((float) pidKpTimes1000) / 1000;
			app->pid.kp = pidKp;
			break;

		case CMD_RX_SET_PID_KI_PARAMETER:
			pidKiTimes1000 = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
			pidKi = ((float) pidKiTimes1000) / 1000;
			app->pid.ki = pidKi;
			break;

		case CMD_RX_SET_PID_KD_PARAMETER:
			pidKdTimes1000 = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
			pidKd = ((float) pidKdTimes1000) / 1000;
			app->pid.kd = pidKd;
			break;

		case CMD_RX_SET_SAMPLING_INTERVAL:
			receivedSamplingInterval = (app->data[0] << 8) + app->data[1];
			if ((receivedSamplingInterval >= 0) && (receivedSamplingInterval <= 50000))
			{
				app->samplingInterval = receivedSamplingInterval;
			}
			break;

		case CMD_RX_SET_PID_INTERVAL:
			receivedPidInterval = (app->data[0] << 8) + app->data[1];
			if ((receivedPidInterval >= 0) && (receivedPidInterval <= 50000))
			{
				appSetPidInterval(app, receivedPidInterval);
			}
			break;

		case CMD_RX_SET_PID_SETPOINT:
			receivedPidSetpointTimes1000 = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
			receivedPidSetpoint = ((float) receivedPidSetpointTimes1000) / 1000;
			if ((receivedPidSetpoint >= 0) && (receivedPidSetpoint <= 300000))
			{
				app->pid.setpoint = receivedPidSetpoint;
			}
			break;

		case CMD_RX_ASK_FOR_RUN_PID_CONTROLLER:
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

		case CMD_RX_SET_MOVING_AVERAGE_WINDOW:
			receivedMovingAverageWindow = (app->data[0] << 8) + app->data[1];
			if (receivedMovingAverageWindow > MOV_AVG_FIL_MAX_QTY_OF_ELEMENTS)
			{
				receivedMovingAverageWindow = MOV_AVG_FIL_MAX_QTY_OF_ELEMENTS;
			}
			app->movingAverageFilter.window = receivedMovingAverageWindow;
			break;

		case CMD_RX_ASK_FOR_PID_MIN_AND_MAX_SUM_OF_ERRORS:
			app->enableSendPidMinAndMaxSumOfErrors = TRUE;
			break;

		case CMD_RX_ASK_FOR_PID_MIN_AND_MAX_CONTROLLED_VARIABLE:
			app->enableSendPidMinAndMaxControlledVariable = TRUE;
			break;

		case CMD_RX_SET_PID_MIN_SUM_OF_ERRORS:
			receivedPidMinSumOfErrors = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
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
			break;

		case CMD_RX_SET_PID_MAX_SUM_OF_ERRORS:
			receivedPidMaxSumOfErrors = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
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
			break;

		case CMD_RX_SET_PID_MIN_CONTROLLED_VARIABLE:
			receivedPidMinControlledVariable = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
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
			break;

		case CMD_RX_SET_PID_MAX_CONTROLLED_VARIABLE:
			receivedPidMaxControlledVariable = (app->data[0] << 24) + (app->data[1] << 16) + (app->data[2] << 8) + app->data[3];
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
	uint32_t processVariableValue = (uint32_t) appGetProcessVariable(app);
	uint8_t qtyOfBytes = 4;
	uint8_t bytes[qtyOfBytes];
	bytes[0] = ((processVariableValue >> 24) & 0x000000FF);
	bytes[1] = ((processVariableValue >> 16) & 0x000000FF);
	bytes[2] = ((processVariableValue >> 8) & 0x000000FF);
	bytes[3] = (processVariableValue & 0x000000FF);

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_PROCESS_VARIABLE_VALUE);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appSendPidKsParameterValues(App *app)
{
	uint8_t qtyOfBytes = 12;
	uint8_t bytes[qtyOfBytes];
	uint32_t kpTimes1000 = (uint32_t)(1000 * app->pid.kp);
	uint32_t kiTimes1000 = (uint32_t)(1000 * app->pid.ki);
	uint32_t kdTimes1000 = (uint32_t)(1000 * app->pid.kd);

	bytes[0] = ((kpTimes1000 >> 24) & 0x000000FF);
	bytes[1] = ((kpTimes1000 >> 16) & 0x000000FF);
	bytes[2] = ((kpTimes1000 >> 8) & 0x000000FF);
	bytes[3] = (kpTimes1000 & 0x000000FF);

	bytes[4] = ((kiTimes1000 >> 24) & 0x000000FF);
	bytes[5] = ((kiTimes1000 >> 16) & 0x000000FF);
	bytes[6] = ((kiTimes1000 >> 8) & 0x000000FF);
	bytes[7] = (kiTimes1000 & 0x000000FF);

	bytes[8] = ((kdTimes1000 >> 24) & 0x000000FF);
	bytes[9] = ((kdTimes1000 >> 16) & 0x000000FF);
	bytes[10] = ((kdTimes1000 >> 8) & 0x000000FF);
	bytes[11] = (kdTimes1000 & 0x000000FF);

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_PID_KS_PARAMETER_VALUES);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appSendPidControllerParameterValues(App *app)
{
	uint8_t qtyOfBytes = 10;
	uint8_t bytes[qtyOfBytes];
	uint32_t setpointTimes1000 = (uint32_t)(1000 * app->pid.setpoint);
	uint16_t pidInterval = (uint16_t) (10000 * pidGetInterval(&app->pid));
	uint16_t movingAverageWindow = app->movingAverageFilter.window;

	bytes[0] = ((app->samplingInterval >> 8) & 0x00FF);
	bytes[1] = (app->samplingInterval & 0x00FF);
	bytes[2] = ((pidInterval >> 8) & 0x00FF);
	bytes[3] = (pidInterval & 0x00FF);
	bytes[4] = ((setpointTimes1000 >> 24) & 0x000000FF);
	bytes[5] = ((setpointTimes1000 >> 16) & 0x000000FF);
	bytes[6] = ((setpointTimes1000 >> 8) & 0x000000FF);
	bytes[7] = (setpointTimes1000 & 0x000000FF);
	bytes[8] = ((movingAverageWindow >> 8) & 0x00FF);
	bytes[9] = (movingAverageWindow & 0x00FF);

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_PID_CONTROLLER_PARAMETER_VALUES);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appSendPidMinAndMaxSumOfErrorsValues(App *app)
{
	uint8_t qtyOfBytes = 8;
	uint8_t bytes[qtyOfBytes];
	uint32_t minSumOfErrors = (uint32_t) (app->pid.minSumOfErrors + 1000000000);
	uint32_t maxSumOfErrors = (uint32_t) (app->pid.maxSumOfErrors + 1000000000);

	bytes[0] = ((minSumOfErrors >> 24) & 0x000000FF);
	bytes[1] = ((minSumOfErrors >> 16) & 0x000000FF);
	bytes[2] = ((minSumOfErrors >> 8) & 0x000000FF);
	bytes[3] = (minSumOfErrors & 0x000000FF);

	bytes[4] = ((maxSumOfErrors >> 24) & 0x000000FF);
	bytes[5] = ((maxSumOfErrors >> 16) & 0x000000FF);
	bytes[6] = ((maxSumOfErrors >> 8) & 0x000000FF);
	bytes[7] = (maxSumOfErrors & 0x000000FF);

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_PID_MIN_AND_MAX_SUM_OF_ERRORS);
	dataPacketTxSetPayloadData(&app->dataPacketTx, bytes, qtyOfBytes);
	dataPacketTxMount(&app->dataPacketTx);
	dataPacketTxUartSend(&app->dataPacketTx, app->huart);
	dataPacketTxPayloadDataClear(&app->dataPacketTx);
	dataPacketTxClear(&app->dataPacketTx);
}

void appSendPidMinAndMaxControlledVariableValues(App *app)
{
	uint8_t qtyOfBytes = 8;
	uint8_t bytes[qtyOfBytes];
	uint32_t minControlledVariable = (uint32_t) (app->pid.minControlledVariable + 1000000000);
	uint32_t maxControlledVariable = (uint32_t) (app->pid.maxControlledVariable + 1000000000);

	bytes[0] = ((minControlledVariable >> 24) & 0x000000FF);
	bytes[1] = ((minControlledVariable >> 16) & 0x000000FF);
	bytes[2] = ((minControlledVariable >> 8) & 0x000000FF);
	bytes[3] = (minControlledVariable & 0x000000FF);

	bytes[4] = ((maxControlledVariable >> 24) & 0x000000FF);
	bytes[5] = ((maxControlledVariable >> 16) & 0x000000FF);
	bytes[6] = ((maxControlledVariable >> 8) & 0x000000FF);
	bytes[7] = (maxControlledVariable & 0x000000FF);

	dataPacketTxSetCommand(&app->dataPacketTx, CMD_TX_PID_MIN_AND_MAX_CONTROLLED_VARIABLE);
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
	else if (appGetEnableSendPidMinAndMaxSumOfErrorsValues(app) == TRUE)
	{
		appSendPidMinAndMaxSumOfErrorsValues(app);
		appSetEnableSendPidMinAndMaxSumOfErrorsValues(app, FALSE);
	}
	else if (appGetEnableSendPidMinAndMaxControlledVariableValues(app) == TRUE)
	{
		appSendPidMinAndMaxControlledVariableValues(app);
		appSetEnableSendPidMinAndMaxControlledVariableValues(app, FALSE);
	}
	else if (appGetEnableSendPidControllerParameterValues(app) == TRUE)
	{
		appSendPidControllerParameterValues(app);
		appSetEnableSendPidControllerParameterValues(app, FALSE);
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

Bool appGetEnableSendPidKsParameterValues(App *app)
{
	return app->enableSendPidKsParameterValues;
}

void appSetEnableSendPidKsParameterValues(App *app, Bool status)
{
	app->enableSendPidKsParameterValues = status;
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

Bool appGetEnableSendPidControllerParameterValues(App *app)
{
	return app->enableSendPidControllerParameterValues;
}

void appSetEnableSendPidControllerParameterValues(App *app, Bool status)
{
	app->enableSendPidControllerParameterValues = status;
}

Bool appGetRunPidControllerStatus(App *app)
{
	return app->runPidController;
}

void appSetRunPidControllerStatus(App *app, Bool status)
{
	app->runPidController = status;
}

Bool appGetEnableSendPidMinAndMaxSumOfErrorsValues(App *app)
{
	return app->enableSendPidMinAndMaxSumOfErrors;
}

void appSetEnableSendPidMinAndMaxSumOfErrorsValues(App *app, Bool status)
{
	app->enableSendPidMinAndMaxSumOfErrors = status;
}

Bool appGetEnableSendPidMinAndMaxControlledVariableValues(App *app)
{
	return app->enableSendPidMinAndMaxControlledVariable;
}

void appSetEnableSendPidMinAndMaxControlledVariableValues(App *app, Bool status)
{
	app->enableSendPidMinAndMaxControlledVariable = status;
}
