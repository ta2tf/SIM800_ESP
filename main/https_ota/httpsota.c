/*
 * httpsota.c
 *
 *  Created on: 19 May 2023
 *      Author: SYA
 */

#include "httpsota.h"

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

#include "esp_ota_ops.h"
#include "wifi_connect.h"

#define TAG "O T A"
xSemaphoreHandle ota_semaphore;


extern const uint8_t server_cert_pem_start[] asm("_binary_google_crt_start");



//==========================================================================================
// validateVersion
//==========================================================================================
esp_err_t validateVersion(char *inversion)
{

	char version[32+1];
	char Family[2+1];
	char Major[2+1];
	char Minor[2+1];
	char Release[2+1];


	memset(version,0,sizeof(version));
	memset(Family,0,sizeof(Family));
	memset(Major,0,sizeof(Major));
	memset(Minor,0,sizeof(Minor));
	memset(Release,0,sizeof(Release));


	// 01234567890
	// 00.00.00.00
	memcpy(version,inversion,32);

	memcpy(Family, &version[0],2);
	memcpy(Major,  &version[3],2);
	memcpy(Minor,  &version[6],2);
	memcpy(Release,&version[9],2);

	ESP_LOGI(TAG, " Family is %s", Family);
	ESP_LOGI(TAG, " Major is %s", Major);
	ESP_LOGI(TAG, " Minor is %s", Minor);
	ESP_LOGI(TAG, " Release is %s", Release);

   return ESP_OK;
}


//==========================================================================================
// validate_image_header
//==========================================================================================
esp_err_t validate_image_header2(esp_app_desc_t *incoming_ota_desc)
{
  const esp_partition_t *running_partition = esp_ota_get_running_partition();
  esp_app_desc_t running_partition_description;
  esp_ota_get_partition_description(running_partition, &running_partition_description);

  ESP_LOGI(TAG, "current version is %s", running_partition_description.version);



  ESP_LOGI(TAG, "new version is     %s", incoming_ota_desc->version);
  ESP_LOGI(TAG, "new version date   %s", incoming_ota_desc->date);
  ESP_LOGI(TAG, "new version name   %s", incoming_ota_desc->project_name);
  ESP_LOGI(TAG, "new version time   %s", incoming_ota_desc->time);


  if (strcmp(running_partition_description.version, incoming_ota_desc->version) == 0)
  {
    ESP_LOGW(TAG, "NEW VERSION IS THE SAME AS CURRENT VERSION. ABORTING");
    return ESP_FAIL;
  }
  else
  {

	  return ESP_OK;
  }
  return ESP_OK;
}



//==========================================================================================
// DumpPartition
//==========================================================================================
void DumpPartition()
{
  const esp_partition_t *running_partition = esp_ota_get_running_partition();
  const esp_partition_t *boot_partition    = esp_ota_get_boot_partition();

  printf("\nRunning partition: %s\n", running_partition->label);
  printf("Boot    partition: %s\n", boot_partition->label);
  printf("Total Partition Count is: %d\n", esp_ota_get_app_partition_count()+1);


   const esp_partition_t *factory_partition;
   const esp_partition_t *ota0_partition;
   const esp_partition_t *ota1_partition;

   esp_app_desc_t         partition_description;


   factory_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY  , "factory");
   esp_ota_get_partition_description(factory_partition, &partition_description);

   printf("\nfactory partition: %s\n", factory_partition->label);
   printf("factory firmware version is: %s\n", partition_description.version);
   printf("factory firmware project_name is: %s\n", partition_description.project_name);

   validateVersion(partition_description.version);


   ota0_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY  , "ota_0");
   esp_ota_get_partition_description(ota0_partition, &partition_description);

   printf("\nOTA 0 partition: %s\n", ota0_partition->label);
   printf("OTA 0 firmware version is: %s\n", partition_description.version);
   printf("OTA 0 firmware project_name is: %s\n", partition_description.project_name);


   ota1_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY  , "ota_1");
   esp_ota_get_partition_description(ota1_partition, &partition_description);

   printf("\nOTA 1 partition: %s\n", ota1_partition->label);
   printf("OTA 1 firmware version is: %s\n",  partition_description.version);
   printf("OTA 1 firmware project_name is: %s\n",  partition_description.project_name);


   //esp_ota_set_boot_partition(ota1_partition);
   }





//esp_err_t client_event_handler(esp_http_client_event_t *evt)
//{
//  return ESP_OK;
//}

esp_err_t client_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read

#define TAG "OTA HTTPS"

    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
        	ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
        	ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
        	ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            break;
        case HTTP_EVENT_ON_DATA:
        	ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA");
            break;
        case HTTP_EVENT_ON_FINISH:
        	ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

            break;
        case HTTP_EVENT_REDIRECT:
        	 ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}


esp_err_t validate_image_header(esp_app_desc_t *incoming_ota_desc)
{
  const esp_partition_t *running_partition = esp_ota_get_running_partition();
  esp_app_desc_t running_partition_description;
  esp_ota_get_partition_description(running_partition, &running_partition_description);

  ESP_LOGI(TAG, "current version is %s\n", running_partition_description.version);
  ESP_LOGI(TAG, "new version is %s\n", incoming_ota_desc->version);

  if (strcmp(running_partition_description.version, incoming_ota_desc->version) == 0)
  {
    ESP_LOGW(TAG, "NEW VERSION IS THE SAME AS CURRENT VERSION. ABORTING");
    return ESP_FAIL;
  }
  return ESP_OK;
}

void run_ota(void *params)
{


  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  while (true)
  {
    xSemaphoreTake(ota_semaphore, portMAX_DELAY);
    ESP_LOGI(TAG, "Invoking OTA");


    example_wifi_connect();

    esp_http_client_config_t clientConfig = {
        .url = "https://drive.google.com/u/0/uc?id=19lcX5Bgy4qlicdO1WXenlflAlzlAHl0Z&export=download", // our ota location
        .event_handler = client_event_handler,
        .cert_pem = (char *)server_cert_pem_start,
		.timeout_ms = 5000
    };



    esp_https_ota_config_t ota_config = {
        .http_config = &clientConfig};

    esp_https_ota_handle_t ota_handle = NULL;

    if (esp_https_ota_begin(&ota_config, &ota_handle) != ESP_OK)
    {
      ESP_LOGE(TAG, "esp_https_ota_begin failed");
      example_disconnect();
      continue;
    }

    esp_app_desc_t incoming_ota_desc;
    if (esp_https_ota_get_img_desc(ota_handle, &incoming_ota_desc) != ESP_OK)
    {
      ESP_LOGE(TAG, "esp_https_ota_get_img_desc failed");
      esp_https_ota_finish(ota_handle);
      example_disconnect();
      continue;
    }

    if (validate_image_header(&incoming_ota_desc) != ESP_OK)
    {
      ESP_LOGE(TAG, "validate_image_header failed");
      esp_https_ota_finish(ota_handle);
      example_disconnect();
      continue;
    }

    while (true)
    {
      esp_err_t ota_result = esp_https_ota_perform(ota_handle);

      if (ota_result != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
        break;
    }

    if (esp_https_ota_finish(ota_handle) != ESP_OK)
    {
      ESP_LOGE(TAG, "esp_https_ota_finish failed");
      example_disconnect();
      continue;
    }
    else
    {
      printf("restarting in 5 seconds\n");
      vTaskDelay(pdMS_TO_TICKS(5000));
      esp_restart();
    }
    ESP_LOGE(TAG, "Failed to update firmware");
  }
}


void on_button_pushed(void *params)
{
  xSemaphoreGiveFromISR(ota_semaphore, pdFALSE);
}


void ota_app(void)
{
  printf("hay I'm a new feature\n");
  const esp_partition_t *running_partition = esp_ota_get_running_partition();
  esp_app_desc_t running_partition_description;
  esp_ota_get_partition_description(running_partition, &running_partition_description);
  printf("current firmware version is: %s\n", running_partition_description.version);

  gpio_config_t gpioConfig = {
      .pin_bit_mask = 1ULL << GPIO_NUM_36,
      .mode = GPIO_MODE_DEF_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLUP_DISABLE,
      .intr_type = GPIO_INTR_NEGEDGE};
  gpio_config(&gpioConfig);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(GPIO_NUM_36, on_button_pushed, NULL);

  DumpPartition();

  ota_semaphore = xSemaphoreCreateBinary();
  xTaskCreate(run_ota, "run_ota", 1024 * 8, NULL, 2, NULL);
}
