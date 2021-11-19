/*
 * CAN_Utils.h
 *
 *  Created on: Nov 18, 2021
 *      Author: DELL
 */

#ifndef INC_CAN_UTILS_H_
#define INC_CAN_UTILS_H_

#include "main.h"
#include "CAN_API.h"


typedef enum module_type
{
	MCU = 0,
	BBOX,
	PMP,
	INFUS,
	COOL,
	CSTT,
	USRI,

}module_type;


typedef enum error_type
{
	FATAL_ERROR = 0,
	MODULE_ERROR,
	WARNING,
	NO_ERROR,

}error_type;

typedef enum msg_type
{
	REQUEST,
	ACK,
	DATA,

}msg_type;


typedef struct message_t
{
	module_type 		module_type;
	error_type 			error_type;
	msg_type 			msg_type;

	uint8_t				data_counter; // number of bytes in buffer for given message
	uint8_t			data[8];
}message_t;



void CAN_Handler(message_t *parsed_ID);
void Parse_CAN_ID(message_t *parsed_ID, uint8_t module_type, uint8_t msg_type, uint8_t error_type);




#endif /* INC_CAN_UTILS_H_ */
