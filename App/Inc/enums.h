/*
 * enums.h
 *
 *  Created on: Aug 7, 2023
 *      Author: Otavio
 */

#ifndef INC_ENUMS_H_
#define INC_ENUMS_H_

typedef enum FLAG
{
	INACTIVE = 0x00,
	ACTIVE
} Flag;

typedef enum BOOL
{
	FALSE = 0x00,
	TRUE
} Bool;

typedef enum CONTROLLER_STATUS
{
	HALT = 0x00,
	RUN
} ControllerStatus;

typedef enum PROCESS_VAR_SEND_STATUS
{
	NOT_SEND = 0x00,
	SEND
} ProcessVarSendStatus;

#endif /* INC_ENUMS_H_ */
