/*
 * IP5306.c
 *
 *  Created on: 9 May 2023
 *      Author: MertechArge014
 */


#include "IP5306.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

static const char *TAG = "IP5306";

#define TIMEOUT_MS		10000
#define DELAY_MS		1000

void IP5306_Init()
{

	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = 21,
		.scl_io_num = 22,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 400000,
	};
	i2c_param_config(I2C_NUM_0, &conf);
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

void IP5306_Read_task()
{
	uint8_t rx_data[5];

	while (1) {




		i2c_master_read_from_device(I2C_NUM_0, IP5306_ADDR, rx_data, 1, TIMEOUT_MS/portTICK_PERIOD_MS);
		ESP_LOG_BUFFER_HEX(TAG, rx_data, 1);
		vTaskDelay(DELAY_MS/portTICK_PERIOD_MS);
	}
}


//==========================================================================================================
//
//==========================================================================================================
 void IP5306_test(void)
{
	 IP5306_Init();
  xTaskCreate(IP5306_Read_task, "IP5306_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);
}
