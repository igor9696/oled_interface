/*
 * CAN_Utils.c
 *
 *  Created on: Nov 18, 2021
 *      Author: DELL
 */

#include "CAN_Utils.h"

extern CAN_RX_message *rx_msg_ptr;


//static int16_t get_value_16_signed(message_t *parsed_ID)
//{
//	if(parsed_ID->data_counter == 2)
//	{
//		return (int16_t)(rx_msg_ptr->RX_data[1] << 8) | rx_msg_ptr->RX_data[0];
//	}
//
//	return 0;
//}

static uint16_t get_value_16_unsigned(message_t *parsed_ID)
{
	if(parsed_ID->data_counter == 2)
	{
		return (uint16_t)(rx_msg_ptr->RX_data[1] << 8) | rx_msg_ptr->RX_data[0];
	}

	return 0;
}


static uint8_t get_value_8(message_t *parsed_ID)
{
	if(parsed_ID->data_counter == 1)
	{
		uint8_t tmp = rx_msg_ptr->RX_data[0];
		return tmp;
	}

	return 0;
}


static uint8_t get_data_counter(CAN_RX_message *rx_msg_ptr)
{
	uint8_t data_len = 0;

	for(int i = 0; i < 8; i++)
	{
		if(rx_msg_ptr->RX_data[i] != 0)
		{
			data_len++;
		}
	}

	return data_len;
}


static void get_value_32(message_t *parsed_ID)
{
	parsed_ID->data[0] = (rx_msg_ptr->RX_data[1] << 8) | (rx_msg_ptr->RX_data[0]);
	parsed_ID->data[1] = (rx_msg_ptr->RX_data[3] << 8) | (rx_msg_ptr->RX_data[2]);
}



void Parse_CAN_ID(message_t *parsed_ID, uint8_t module_type, uint8_t msg_type, uint8_t error_type)
{
	switch(module_type)
	{
	case 0:
		parsed_ID->module_type = MCU;
		break;
	case 1:
		parsed_ID->module_type = BBOX;
		break;
	case 2:
		parsed_ID->module_type = PMP;
		break;
	case 3:
		parsed_ID->module_type = INFUS;
		break;
	case 4:
		parsed_ID->module_type = COOL;
		break;
	case 5:
		parsed_ID->module_type = CSTT;
		break;
	case 6:
		parsed_ID->module_type = USRI;
		break;
	}

	switch(msg_type)
	{
	case 4:
		parsed_ID->msg_type = REQUEST;
		break;
	case 5:
		parsed_ID->msg_type = ACK;
		break;
	case 6:
		parsed_ID->msg_type = DATA;
		break;
	}


	switch(error_type)
	{
	case 0:
		parsed_ID->error_type = FATAL_ERROR;
		break;
	case 1:
		parsed_ID->error_type = MODULE_ERROR;
		break;
	case 2:
		parsed_ID->error_type = WARNING;
		break;
	case 3:
		parsed_ID->error_type = NO_ERROR;
		break;
	}
}


void CAN_Handler(message_t *parsed_ID)
{
	// check if any message available
	if(CAN_get_message(&rx_msg_ptr))
	{
		memset(parsed_ID->data, 0, 8); // clear data buffer
		//char buff[32];
		//uint8_t msg_len;
		//uint8_t data_len = 0;

		uint16_t module_type = 7;
		uint16_t msg_type;
		uint8_t error_type;

		// get data counter from rx buffer
		parsed_ID->data_counter = get_data_counter(rx_msg_ptr);
		parsed_ID->data_counter = rx_msg_ptr->RX_header.DLC;


		// parse message ID
		module_type = (rx_msg_ptr->RX_header.StdId) & 0x0F;
		msg_type = (rx_msg_ptr->RX_header.StdId >> 6) & 0x07;
		error_type = rx_msg_ptr->RX_header.StdId >> 9;

		// set message_t struct
		Parse_CAN_ID(parsed_ID, module_type, msg_type, error_type);

		// get data
		memcpy(parsed_ID->data, rx_msg_ptr->RX_data, 8);


// 		send received data to terminal [DEBUG]
//		for(int i = 0; i < data_len; i++)
//		{
//			msg_len = sprintf(buff, "Received: %d\n\r", rx_msg_ptr->RX_data[i]);
//			HAL_UART_Transmit(&huart2, buff, msg_len, 1000);
//		}
	}
}




