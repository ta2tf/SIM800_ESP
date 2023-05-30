/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases creating a GATT database using a predefined attribute table.
* It acts as a GATT server and can send adv data, be connected by client.
* Run the gatt_client demo, the client demo will automatically connect to the gatt_server_service_table demo.
* Client demo will enable GATT server's notify after connection. The two devices will then exchange
* data.
* add github now
* add home adition
*
*
*
* to build cert NVS file
* cd .\build\
python C:\Espressif\frameworks\esp-idf-v5.0.1\components\nvs_flash\nvs_partition_generator\nvs_partition_gen.py generate  "C:\Espressif\frameworks\esp-idf-v5.0.1\workspace2\SIM800_ESP\nvs.csv" certs.bin 16384

python C:\Espressif\frameworks\esp-idf-v5.0.1-3\components\nvs_flash\nvs_partition_generator\nvs_partition_gen.py generate  "C:\Espressif\frameworks\esp-idf-v5.0.1-3\workspace\SIM800_ESP\nvs.csv" certs.bin 16384


Creating NVS binary with version: V2 - Multipage Blob Support Enabled
Created NVS binary: ===> C:\Espressif\frameworks\esp-idf-v5.0.1\workspace2\SIM800_ESP\build\certs.bin


# ESP-IDF Partition Table
# Name, Type, SubType, Offset, Size, Flags
nvs,data,nvs,0x9000,16K,
otadata,data,ota,0xd000,8K,
phy_init,data,phy,0xf000,4K,
spiffs,data,spiffs,0x10000,1M,
factory,app,factory,0x400000,4M,
ota_0,app,ota_0,0x800000,4M,
ota_1,app,ota_1,0xc00000,4M,
*
****************************************************************************/


#include "main.h"
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"cd
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"

#include "esp_check.h"
#include "driver/gpio.h"

#include "gsm.h"
#include "ble.h"
#include "led.h"
#include "IP5306.h"
#include "battery.h"
#include "aws.h"
#include "httpsota.h"
#include "wifi_connect.h"
#include "uart.h"

#include "nvs.h"
#include "nvs_flash.h"



static const char *TAG = "MAIN";


void second_nvs_test(void)
{

    ESP_LOGI(TAG, "S T O R A G E start");

    char issid[32];
    char ipass[32];

    sprintf(issid,"%s","ola mola");
    sprintf(ipass,"%s","noli la");

    nvs_handle_t storage;

   ESP_ERROR_CHECK( nvs_flash_init());
   ESP_ERROR_CHECK( nvs_open("storage", NVS_READWRITE, &storage));
   ESP_ERROR_CHECK( nvs_set_str(storage, "cpyssid", issid));
   ESP_ERROR_CHECK( nvs_set_str(storage, "cpypass", ipass));
   nvs_close(storage);



         size_t ssidLen, passLen;
         char *ssid = NULL, *pass = NULL;



             nvs_open("storage", NVS_READWRITE, &storage);

             if (nvs_get_str(storage, "cpyssid", NULL, &ssidLen) == ESP_OK)
             {
               if (ssidLen > 0)
               {
                 ssid = malloc(ssidLen);
                 nvs_get_str(storage, "cpyssid", ssid, &ssidLen);
               }
             }
             else
           	  ESP_LOGI(TAG, "S T O R A G E fail 1");

             if (nvs_get_str(storage, "cpypass", NULL, &passLen) == ESP_OK)
             {
               if (passLen > 0)
               {
                 pass = malloc(passLen);
                 nvs_get_str(storage, "cpypass", pass, &passLen);
               }
             } else
           	  ESP_LOGI(TAG, "S T O R A G E fail 2");


             nvs_close(storage);


             if (ssid != NULL && pass != NULL)
              {
            	    ESP_LOGI(TAG, "S T O R A G E  [%s]  :  [%s]", ssid, pass);

                  if (ssid != NULL)
                    free(ssid);
                  if (pass != NULL)
                    free(pass);

              }

             ESP_LOGI(TAG, "S T O R A G E End");


}




//==========================================================================================================
//==========================================================================================================
//
//
//==========================================================================================================
//==========================================================================================================
void app_main(void)
{
    esp_err_t ret;

	esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "[APP] Startup.....");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", (int) esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());


    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

        char issid[32];
        char ipass[32];

     //   sprintf(issid,"%s","MERLIN");
     //   sprintf(ipass,"%s","narnia1523");

        sprintf(issid,"%s","Mertech_2_4");
        sprintf(ipass,"%s","MeR0TecH_2");


        ESP_ERROR_CHECK( nvs_flash_init());
        nvs_handle_t nvs;
        ESP_ERROR_CHECK( nvs_open("wifiCreds", NVS_READWRITE, &nvs));
        ESP_ERROR_CHECK( nvs_set_str(nvs, "ssid", issid));
        ESP_ERROR_CHECK( nvs_set_str(nvs, "pass", ipass));
        nvs_close(nvs);


        second_nvs_test();


      //  uart_echo_test();

    LED_Init();


  //   BLE_Init();
   //   GSM_Init();

   //  IP5306_test();
   //  bat_Init();


   // example_wifi_connect();


   //  ota_app();

   // aws_main();

    while (1)
     {

         vTaskDelay(pdMS_TO_TICKS(100));
     }
}
