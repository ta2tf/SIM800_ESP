/*
 * IP5306.h
 *
 *  Created on: 9 May 2023
 *      Author: MertechArge014
 */

#ifndef MAIN_IP5306_H_
#define MAIN_IP5306_H_


#define I2C_SDA              21
#define I2C_SCL              22

#define IP5306_ADDR          0xEA
#define IP5306_REG_SYS_CTL0  0x00


 void IP5306_test(void);



#endif /* MAIN_IP5306_H_ */
