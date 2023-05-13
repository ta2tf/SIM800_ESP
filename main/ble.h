/*
 * ble.h
 *
 *  Created on: 9 May 2023
 *      Author: MertechArge014
 */

#ifndef MAIN_BLE_H_
#define MAIN_BLE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Attributes State Machine */
enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,
	IDX_CHAR_CFG_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    HRS_IDX_NB,
};



void BLE_Init(void);

#endif /* MAIN_BLE_H_ */
