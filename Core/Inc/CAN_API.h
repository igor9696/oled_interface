/**
 * @brief
 * This is header of CAN API for Organ Pump. In this file all macros are defined.
 * It also contains function prototypes of API and type definitions.
 *
 * @file 		CAN_API.c
 * @date		Apr 16, 2021
 * @author		Michał Pałka
 *
 * @details
 * <pre>
 * API tested on NUCLEO-L476RG and NUCLEO-L433RC-P.
 * In CubeMX:
 * CAN1:							X Activated
 * NVIC:							X CAN1 RX0 Interrupt
 * Prescaler:						20
 * Time Quanta inBit Segment 1:		13 Times
 * Time Quanta inBit Segment 2:		2 Times
 *
 * </pre>
 */

#ifndef INC_CAN_API_H_
#define INC_CAN_API_H_

#include "main.h"
#include <string.h>

#define CIRC_BUFF_SIZE	16						//!< size of circular buffer for CAN RX messages

#define CAN_TIMEOUT		100						//!< CAN TX timeout

#define LOW_ADDRESS_LIMIT			0x1A		//!< lower component addresses boundary
#define HIGH_ADDRESS_LIMIT			0x1E		//!< upper component addresses boundary

#define ERR_MASK		0x600					//!< error address mask
#define TYP_MASK		0x1C0					//!< type address mask
#define CMP_MASK		0x03F					//!< component address mask

#define ERR_SHIFT		9						//!< shift for error code
#define TYP_SHIFT		6						//!< shift for message type code
#define CMP_SHIFT		0						//!< shift for component code

#define TYP_ACPT		0x05					//!< confirmation code

#define ERR_CERR		0x00					//!< critical error code
#define ERR_FERR		0x01					//!< component error code,
#define ERR_WARN		0x02					//!< warning code
#define ERR_NORM		0x03					//!< normal code

#define ID_MASK			0x7FF					//!< mask for standard CAN ID, 11-bit's

/**
 * CAN_RX_message represents incoming CAN message. It store CAN_RxHeaderTypeDef and data field of message.
 */
typedef struct {
	CAN_RxHeaderTypeDef RX_header;	//!< is needed to recognize incoming CAN request.
	uint8_t RX_data[8];				//!< currently not used.
}CAN_RX_message;
/**
 * CAN_TX_message represents outcoming CAN message. It store CAN_TxHeaderTypeDef, data field of message and TX_mailbox.
 */
typedef struct {
	CAN_TxHeaderTypeDef TX_header;	//!< is needed to specify header of outcoming CAN message.
	uint8_t TX_data[8];				//!< represents data field. It is up to 8 bytes but doesn't have to be fully used.
	uint32_t TX_mailbox;		//!< specifies TX mailbox.
}CAN_TX_message;
/**
 * CAN_message_ID is used for the purpose of identifying type of request.
 */


//////////////////////////////////////////////////////////////////////////////////////////////

void CAN_init(CAN_HandleTypeDef *phcan);
uint8_t CAN_get_message(CAN_RX_message **msg);
uint8_t CAN_send_message(void(*TX_handler)(void));

#endif /* INC_CAN_API_H_ */
