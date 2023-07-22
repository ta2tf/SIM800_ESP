/*
 * prefilter.h
 *
 *  Created on: 11 Jul 2023
 *      Author: MertechArge014
 */

#ifndef MAIN_TWAI_CAN_PREFILTER_H_
#define MAIN_TWAI_CAN_PREFILTER_H_

#include "can.h"

int test_can_prefilter(void);
int DoPreFilter(rx_message_t *msg);
uint8_t get_filter_datamask(uint8_t item) ;


#endif /* MAIN_TWAI_CAN_PREFILTER_H_ */
