/*
 * application.c
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#include "application.h"

void applicationInit(Application *application, GPIO_TypeDef* ledPort, uint16_t ledPin)
{
	application->ledPort = ledPort;
	application->ledPin = ledPin;
	application->blinkDelay = DELAY_100_MILISECONDS;
	application->blinkStatus = TRUE;
	application->command = 0;
	application->decodeCommandStatus = FALSE;
	application->dataLenght = 0;
	memset(application->data, 0x00, QTY_DATA_BYTES);

	application->adcReadComplete = FALSE;
	application->enableSendAdcRead = FALSE;
	application->adcValue = 0;
}

void applicationDecodeCommand(Application *application)
{
	switch (application->command)
	{
		case CMD_TEMPORIZATION_1:
			application->blinkDelay = DELAY_100_MILISECONDS;
			application->blinkStatus = TRUE;
			break;

		case CMD_TEMPORIZATION_2:
			application->blinkDelay = DELAY_250_MILISECONDS;
			application->blinkStatus = TRUE;
			break;

		case CMD_TEMPORIZATION_3:
			application->blinkDelay = DELAY_500_MILISECONDS;
			application->blinkStatus = TRUE;
			break;

		case CMD_SET_SEND_ADC_READ_STATUS:
			if (application->data[0] == 0x00)
			{
				application->enableSendAdcRead = FALSE;
			}
			else if (application->data[0] == 0x01)
			{
				application->enableSendAdcRead = TRUE;
			}
			break;

		default:
			application->blinkDelay = DELAY_100_MILISECONDS;
			application->blinkStatus = TRUE;
			break;
	}
}

void applicationExecuteBlinkLed(Application *application)
{
	if (application->blinkStatus == TRUE)
	{
	  HAL_GPIO_TogglePin(application->ledPort, application->ledPin);
	}
	else
	{
	  HAL_GPIO_WritePin(application->ledPort, application->ledPin, GPIO_PIN_RESET);
	}
}

uint32_t applicationGetBlinkDelay(Application *application)
{
	return application->blinkDelay;
}

void applicationSetCommand(Application *application, uint8_t command)
{
	application->command = command;
}

void applicationSetDecodeStatus(Application *application, Bool status)
{
	application->decodeCommandStatus = status;
}

Bool applicationGetDecodeStatus(Application *application)
{
	return application->decodeCommandStatus;
}

void applicationSetData(Application *application, uint8_t *data, uint8_t dataLength)
{
	if (dataLength <= QTY_DATA_BYTES)
	{
		application->dataLenght = dataLength;
		memcpy(application->data, data, dataLength);
	}
}

void applicationSetAdcReadCompleteStatus(Application *application, Bool status)
{
	application->adcReadComplete = status;
}

Bool applicationGetAdcReadCompleteStatus(Application *application)
{
	return application->adcReadComplete;
}

Bool applicationGetEnableSendAdcRead(Application *application)
{
	return application->enableSendAdcRead;
}

void applicationSetAdcValue(Application *application, uint16_t value)
{
	application->adcValue = value;
}

uint16_t applicationGetAdcValue(Application *application)
{
	return application->adcValue;
}
