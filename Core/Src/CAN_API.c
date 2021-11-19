/**
 * @brief
 * In this file there is implementation of CAN API for pump.
 *
 * @file 		CAN_API.c
 * @date		Apr 16, 2021
 * @author		Michał Pałka
 */

#include "CAN_API.h"

static CAN_HandleTypeDef *hcan;		//!< pointer to CAN_HandleTypeDef structure of using CAN

CAN_TX_message CAN_message;			//!< CAN_TX_message instance. Need to be filled in TX_handler passed to CAN_send_message function.

CAN_RX_message circ_RX_buff[CIRC_BUFF_SIZE]; 	//!< CAN_RX_message circular buffer
uint8_t buff_head = 0;							//!< head of RX circular buffer
uint8_t buff_tail = 0;							//!< tail of RX circular buffer


/**
 * CAN_filter_init initialize filter for CAN RX messages.
 * Message filtering not at this level in current implementation.
 * For more information refer to <a
 * href="https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf#page=2049&zoom=100,89,118"
 * >reference manual fig. 609</a>
 * and <a
 * href="https://www.st.com/resource/en/user_manual/dm00173145-description-of-stm32l4l4-hal-and-lowlayer-drivers-stmicroelectronics.pdf#page=127&zoom=100,0,0"
 * >HAL description section 9.1.2</a>.
 *
 */
static void CAN_filter_init(void){
	CAN_FilterTypeDef sFilterConfig;

	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0;
	sFilterConfig.FilterIdLow = 0;
	sFilterConfig.FilterMaskIdHigh = 0;
	sFilterConfig.FilterMaskIdLow = 0;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 0;

	if (HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK) {
		Error_Handler();
	}

}
/**
 * CAN_send_message is static function which sends previously prepared CAN_message.
 * @param [in]	void(*TX_handler)(void) function pointer
 * @param [out]	uint8_t	1 - sent successfully, 0 - timeout.
 */
uint8_t CAN_send_message(void(*TX_handler)(void)){
	uint32_t tick_start;
	tick_start = HAL_GetTick();
	while(HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3){
		if(HAL_GetTick()-tick_start > CAN_TIMEOUT){		//if sending time exceed timeout
			return 0;									//return 0 (failure)
		}
	}

	memset(CAN_message.TX_data, 0, 8);

	TX_handler();

	if(HAL_CAN_AddTxMessage(hcan, &CAN_message.TX_header, CAN_message.TX_data, &CAN_message.TX_mailbox) != HAL_OK){
		Error_Handler();
	}
	return 1;											//success, return 1
}
/**
 * CAN_init initialize CAN API.
 * @param [in]	phcan 	pointer to CAN_HandleTypeDef
 * @param [in]	id		Unique device id for CAN BUS
 */
void CAN_init(CAN_HandleTypeDef *phcan){
	hcan=phcan;
	CAN_message.TX_mailbox = 0;

	CAN_filter_init();

	if (HAL_CAN_Start(hcan) != HAL_OK) {
		Error_Handler();
	}

	if(HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK){
		Error_Handler();
	}
}

/**
 * CAN_get_message checks if there is an message in buffer which is not handle. If there is it returns number of messages in buffer and
 * assign equivalent value to given msg pointer.
 *
 * @param [in]	msg	pointer to CAN_RX_message.
 * @param [out]	uint8_t	number of messages in buffer.
 *
 * 	Recommended usage:
 * 	@code
while(CAN_get_message(&msg)){
	handle(&msg);
	...
}
 * 	@endcode
 */
uint8_t CAN_get_message(CAN_RX_message **msg){
	uint8_t msg_count = 0;

	if(buff_head==buff_tail){
		return msg_count;					//if head == tail there is nothing in buffer
	}else if(buff_head>buff_tail){
		msg_count = buff_head-buff_tail;
	}else{
		msg_count = CIRC_BUFF_SIZE - (buff_tail - buff_head);
	}

	*msg = circ_RX_buff+buff_tail;

	buff_tail=(buff_tail+1)%CIRC_BUFF_SIZE;		//message read, move tail forward

	return msg_count;
}

/**
 * HAL_CAN_RxFifo0MsgPendingCallback handles the CAN RX interrupt.
 *
 * @param [in] hcan	pointer to CAN_HandleTypeDef structure.
 */
void HAL_CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef * hcan){
	memset(circ_RX_buff[buff_head].RX_data, 0, 8);
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &circ_RX_buff[buff_head].RX_header, circ_RX_buff[buff_head].RX_data) != HAL_OK){
		Error_Handler();
	}
	//if (((circ_RX_buff[buff_head].RX_header.StdId & CMP_MASK) >= LOW_ADDRESS_LIMIT) && ((circ_RX_buff[buff_head].RX_header.StdId & CMP_MASK) <= HIGH_ADDRESS_LIMIT)) {

	if((buff_head + 1) % CIRC_BUFF_SIZE == buff_tail)
	{	//if buffer is overflowed
		buff_tail = (buff_tail + 1) % CIRC_BUFF_SIZE;		//move tail and lose the oldest message
	}

	buff_head = (buff_head + 1) % CIRC_BUFF_SIZE;			//move head forward

}



