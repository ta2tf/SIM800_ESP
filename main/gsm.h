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
#define LED_GPIO             13
#define LED_ON               HIGH
#define LED_OFF              LOW

#define HI_LEVEL             1
#define LO_LEVEL             0


#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

 void GSM_TX_Task(void *arg);
 void GSM_RX_Task(void *arg);
 void GSM_INT_Task(void *arg);
 void led_blink_task(void *arg);

 void GSM_PowerInit(void);
 void GSM_PINInit(void);
 void GSM_UART_Init(void) ;


 void configure_led(void);




#endif /* MAIN_GSM_H_ */
