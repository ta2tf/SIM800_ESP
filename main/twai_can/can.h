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




void CAN_Test(void);


#endif /* MAIN_TWAI_CAN_CAN_H_ */
