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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_check.h"
#include "driver/gpio.h"
#include "gsm.h"
#include "ble.h"


extern QueueHandle_t interputQueue;


//==========================================================================================================
//==========================================================================================================
//
//
//==========================================================================================================
//==========================================================================================================
void app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );


     configure_led();

     GSM_PowerInit();
     GSM_UART_Init();

     xTaskCreate(GSM_RX_Task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
     xTaskCreate(GSM_TX_Task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);

     xTaskCreate(led_blink_task, "led_blink_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);

     interputQueue = xQueueCreate(10, sizeof(int));
     xTaskCreate(GSM_INT_Task, "uart_INT_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);


     GSM_PINInit();

     BLE_Init();
}
