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
#define DELAY_MS		10000


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


/*SHUTDOWN_TIME_VALUE*/
#define SHUTDOWN_8s   0
#define SHUTDOWN_32s  1
#define SHUTDOWN_16s  2
#define SHUTDOWN_64s  3

/*CUT-OFF VOLTAGE RANGE*/
#define CUT_OFF_VOLTAGE_0   0     // 4.14/4.26/4.305/4.35  V
#define CUT_OFF_VOLTAGE_1   1     // 4.17/4.275/4.32/4.365 V
#define CUT_OFF_VOLTAGE_2   2     // 4.185/4.29/4.335/4.38 V
#define CUT_OFF_VOLTAGE_3   3     // 4.2/4.305/4.35/4.395  V

/*BATTERY STOP CHARGING CURRENT*/
#define CURRENT_200   0
#define CURRENT_400   1
#define CURRENT_500   2
#define CURRENT_600   3

/*VOLTAGE SETTING FOR CHARGING*/
#define VOUT_0  0   //4.45
#define VOUT_1  1   //4.5
#define VOUT_2  2   //4.55
#define VOUT_3  3   //4.6
#define VOUT_4  4   //4.65
#define VOUT_5  5   //4.7
#define VOUT_6  6   //4.75
#define VOUT_7  7   //4.8

/*Battery Voltage */
#define BATT_VOLTAGE_3  3  //4.4
#define BATT_VOLTAGE_2  2  //4.35
#define BATT_VOLTAGE_1  1  //4.3
#define BATT_VOLTAGE_0  0  //4.2

/*Voltage Pressure setting*/
#define Pressurized_42   3
#define Pressurized_28   2
#define Pressurized_14   1
#define Not_pressurized  0




 void IP5306_test(void);



#endif /* MAIN_IP5306_H_ */
