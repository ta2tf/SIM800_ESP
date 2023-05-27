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

    IDX_CHAR_D,
    IDX_CHAR_VAL_D,

    HRS_IDX_NB,
};



#define CHR_SYSTEM_ID             0x2A23
#define CHR_MODEL_NUMBER          0x2A24
#define CHR_SERIAL_NUMBER         0x2A25
#define CHR_FIRMWARE_REVISION     0x2A26
#define CHR_HARDWARE_REVISION     0x2A27
#define CHR_SOFTWARE_REVISION     0x2A28
#define CHR_MANUFACTURER_NAME     0x2A29
#define CHR_IEEE11073_20601       0x2A2A



void BLE_Init(void);

#endif /* MAIN_BLE_H_ */
