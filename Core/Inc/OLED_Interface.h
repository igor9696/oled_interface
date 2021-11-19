/*
 * OLED_Interface.h
 *
 *  Created on: Nov 18, 2021
 *      Author: DELL
 */

#ifndef INC_OLED_INTERFACE_H_
#define INC_OLED_INTERFACE_H_

#include "main.h"
#include "CAN_Utils.h"
#include "SSD1322_API.h"
#include "SSD1322_HW_Driver.h"

//typedef struct param_monitor
//{
//	// TODO: parameters to print on screen
//
//
//
//}param_monitor;


typedef struct OLED_t
{
	// SPI_Handler
	//



}OLED_t;



// function prototypes
void Interface_Handler(message_t *parsed_ID);
void OLED_Update(OLED_t screen);
void OLED_PrintStartScreen(OLED_t screen);

#endif /* INC_OLED_INTERFACE_H_ */
