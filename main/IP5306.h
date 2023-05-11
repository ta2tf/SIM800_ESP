/*
 * IP5306.h
 *
 *  Created on: 9 May 2023
 *      Author: MertechArge014
 */

#ifndef MAIN_IP5306_H_
#define MAIN_IP5306_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define I2C_SDA              21
#define I2C_SCL              22


#define IP5306_REG_SYS_CTL0  0x00



#define TIMEOUT_MS		10000
#define DELAY_MS		1000


/*macros definition*/
#define ENABLE    1
#define DISABLE   0

#define IP5306_ADDRESS   0x75            //7 bit slave address


/******************************************************************
*****************************Registers*****************************
*******************************************************************
*/
#define SYS_CTL0       0x00
#define SYS_CTL1       0x01
#define SYS_CTL2       0x02

#define Charger_CTL0   0x20
#define Charger_CTL1   0x21
#define Charger_CTL2   0x22
#define Charger_CTL3   0x23

#define CHG_DIG_CTL0   0x24

#define REG_READ0      0x70
#define REG_READ1      0x71
#define REG_READ2      0x72
#define REG_READ3      0x77
#define REG_READ4      0x78


#define BATTERY_75_BIT  0x80
#define BATTERY_50_BIT  0x40
#define BATTERY_25_BIT  0x20
#define BATTERY_0_BIT   0x10


 void IP5306_test(void);



#endif /* MAIN_IP5306_H_ */
