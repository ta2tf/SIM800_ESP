/*
 * can.h
 *
 *  Created on: 14 Jun 2023
 *      Author: MertechArge014
 */

#ifndef MAIN_TWAI_CAN_CAN_H_
#define MAIN_TWAI_CAN_CAN_H_


#define TWAI_FRAME_MAX_DLC              8           /**< Max data bytes allowed in TWAI */
#define TWAI_FRAME_EXTD_ID_LEN_BYTES    4           /**< EFF ID requires 4 bytes (29bit) */
#define TWAI_FRAME_STD_ID_LEN_BYTES     2           /**< SFF ID requires 2 bytes (11bit) */
#define TWAI_ERR_PASS_THRESH            128         /**< Error counter threshold for error passive */

typedef struct {
    uint32_t identifier;                /**< 11 or 29 bit identifier */
    uint8_t data_length_code;           /**< Data length code */
    uint8_t data[TWAI_FRAME_MAX_DLC];    /**< Data bytes (not relevant in RTR frame) */
    uint32_t counter;
} can_message_t;




void CAN_Test(void);


#endif /* MAIN_TWAI_CAN_CAN_H_ */
