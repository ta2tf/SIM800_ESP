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

Creating NVS binary with version: V2 - Multipage Blob Support Enabled
Created NVS binary: ===> C:\Espressif\frameworks\esp-idf-v5.0.1\workspace2\SIM800_ESP\build\certs.bin

*
****************************************************************************/


#include "main.h"
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
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

#include "nvs.h"
#include "nvs_flash.h"



static const char *TAG = "MAIN";





char * nvs_load_value_if_exist(nvs_handle handle, const char* key)
{
	static const char TAG[] = "NVS-Test";

    // Try to get the size of the item
    size_t value_size;
    if(nvs_get_str(handle, key, NULL, &value_size) != ESP_OK){
        ESP_LOGE(TAG, "Failed to get size of key: %s", key);
        return NULL;
    }

    char* value = malloc(value_size);
    if(nvs_get_str(handle, key, value, &value_size) != ESP_OK){
        ESP_LOGE(TAG, "Failed to load key: %s", key);
        return NULL;
    }

    return value;
}

void NVS_app_main(void)
{
	static const char TAG[] = "NVS-Test";

	char logg[128];

    // Initialize NVS
    ESP_LOGI(TAG, "test start  NVS");

    // Open the "certs" namespace in read-only mode
    nvs_handle handle;
    ESP_ERROR_CHECK( nvs_open("certs", NVS_READONLY, &handle));

    // Load the private key & certificate
    ESP_LOGI(TAG, "Loading private key & certificate");
    char * private_key = nvs_load_value_if_exist(handle, "priv_key");
    char * certificate = nvs_load_value_if_exist(handle, "certificate");

    // We're done with NVS
    nvs_close(handle);


    // Check if both items have been correctly retrieved
    if(private_key == NULL || certificate == NULL){
        ESP_LOGE(TAG, "Private key or cert could not be loaded");
        return; // You might want to handle this in a better way
    }
    else
    	ESP_LOGE(TAG, "Private key or cert  be loaded");


    memset(logg,0,sizeof(logg));
    memcpy(logg,private_key,32);
    ESP_LOGI(TAG, "private_key NVS %s", logg);

    memset(logg,0,sizeof(logg));
    memcpy(logg,certificate,32);
    ESP_LOGI(TAG, "certificate NVS %s", logg);

    ESP_LOGI(TAG, "test stop  NVS");




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

    //    char issid[32];
    //    char ipass[32];
    //
    //    sprintf(issid,"%s","MERLIN");
    //    sprintf(ipass,"%s","narnia1523");
    //
    //    nvs_flash_init();
    //    nvs_handle_t nvs;
    //    nvs_open("wifiCreds", NVS_READWRITE, &nvs);
    //    nvs_set_str(nvs, "ssid", issid);
    //    nvs_set_str(nvs, "pass", ipass);
    //    nvs_close(nvs);


    NVS_app_main();


    LED_Init();


  //   BLE_Init();
   //   GSM_Init();

   //  IP5306_test();
   //  bat_Init();



     //example_wifi_connect();


   //  ota_app();

   //  aws_main();

    while (1)
     {

         vTaskDelay(pdMS_TO_TICKS(100));
     }
}
