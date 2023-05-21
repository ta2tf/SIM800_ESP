/*
 * httpsota.c
 *
 *  Created on: 19 May 2023
 *      Author: SYA
 */



#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "protocol_examples_common.h"
#include "httpsota.h"


#define TAG "O T A"
SemaphoreHandle_t ota_semaphore;

const int software_version = 1;
extern const uint8_t server_cert_pem_start[] asm("_binary_google_crt_start");

esp_err_t  client_event_handler(esp_http_client_event_t *evt)
{
  return ESP_OK;
}

void run_ota(void *params)
{
  while (true)
  {
	   ESP_LOGI(TAG, "wait OTA");
    xSemaphoreTake(ota_semaphore, portMAX_DELAY);
    ESP_LOGI(TAG, "Invoking OTA");

    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    esp_http_client_config_t clientConfig = {
      .url = "https://drive.google.com/u/0/uc?id=19lcX5Bgy4qlicdO1WXenlflAlzlAHl0Z&export=download", // our ota location
      .event_handler = client_event_handler,
      .cert_pem = (char *)server_cert_pem_start
    };

    if(esp_https_ota(&clientConfig) == ESP_OK)
    {
      ESP_LOGI(TAG,"OTA flash succsessfull for version %d.",software_version);
      printf("restarting in 5 seconds\n");
      vTaskDelay(pdMS_TO_TICKS(5000));
      esp_restart();
    }
    ESP_LOGE(TAG,"Failed to update firmware");
  }

}

void on_button_pushed(void *params)
{
  xSemaphoreGiveFromISR(ota_semaphore, pdFALSE);
}

void ota_app(void)
{
  printf("OLAAA HAY!!! This is a new feature\n");
  ESP_LOGI("SOFTWARE VERSION", "we are running %d",software_version);
  gpio_config_t gpioConfig = {
      .pin_bit_mask = 1ULL << GPIO_NUM_36,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLUP_DISABLE,
      .intr_type = GPIO_INTR_NEGEDGE};
  gpio_config(&gpioConfig);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(GPIO_NUM_36, on_button_pushed, NULL);

  ota_semaphore = xSemaphoreCreateBinary();
  xTaskCreate(run_ota, "run_ota", 1024 * 8, NULL, 2, NULL);
}
