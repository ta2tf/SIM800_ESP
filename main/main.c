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
****************************************************************************/


#include "main.h"
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "esp_check.h"
#include "driver/gpio.h"

#include "gsm.h"
#include "ble.h"
#include "led.h"
#include "IP5306.h"
#include "battery.h"
#include "aws.h"

static const char *TAG = "MAIN";

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

    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", (int) esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());


    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );


    LED_Init();
    BLE_Init();
    GSM_Init();

    IP5306_test();
    bat_Init();
     
	aws_main();
	
	
    while (1)
     {

         vTaskDelay(pdMS_TO_TICKS(100));
     }
}
