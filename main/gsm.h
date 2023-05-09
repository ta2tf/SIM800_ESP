/*
 * gsm.h
 *
 *  Created on: 9 May 2023
 *      Author: MertechArge014
 */

#ifndef MAIN_GSM_H_
#define MAIN_GSM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define MODEM_RST             5
#define MODEM_PWRKEY          4
#define MODEM_POWER_ON       23

#define MODEM_TX             27
#define MODEM_RX             26

#define MODEM_DTR            32
#define MODEM_RI             33

#define I2C_SDA              21
#define I2C_SCL              22

#define HI_LEVEL             1
#define LO_LEVEL             0


#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00


 void GSM_Init(void);


#endif /* MAIN_GSM_H_ */
