/*
 * wifi_sta.h
 *
 *  Created on: 21 May 2023
 *      Author: SYA
 */

#ifndef MAIN_WIFI_WIFI_STA_H_
#define MAIN_WIFI_WIFI_STA_H_


//#define CONFIG_ESP_WIFI_SSID       "MERLIN"
//#define CONFIG_ESP_WIFI_PASSWORD   "narnia1523"

#define CONFIG_ESP_WIFI_SSID       "Mertech_2_4"
#define CONFIG_ESP_WIFI_PASSWORD   "MeR0TecH_2"

#define CONFIG_ESP_MAXIMUM_RETRY   5

//#define CONFIG_ESP_WIFI_AUTH_OPEN
//#define CONFIG_ESP_WIFI_AUTH_WEP
//#define CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define CONFIG_ESP_WIFI_AUTH_WPA2_PSK 1
//#define CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
//#define CONFIG_ESP_WIFI_AUTH_WPA3_PSK
//#define CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
//#define CONFIG_ESP_WIFI_AUTH_WAPI_PSK

void Wifi_Station(void);


#endif /* MAIN_WIFI_WIFI_STA_H_ */
