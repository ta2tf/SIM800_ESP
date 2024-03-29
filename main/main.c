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
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

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
#include "esp_wifi.h"

#include "whatsapp.h"

#include "driver/rmt.h"
#include "led_strip.h"
#include "http_server.h"
#include "can.h"
#include "can_buf.h"
#include "prefilter.h"

#include "sntp.h"

#include "panel.h"
#include "libGSM.h"



static const char *TAG = "MAIN";

#define CUSTOM_NVS_PART_NAME "storage"


#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define EXAMPLE_CHASE_SPEED_MS (5)
#define CONFIG_EXAMPLE_RMT_TX_GPIO 13
#define CONFIG_EXAMPLE_STRIP_LED_NUMBER 100

static TaskHandle_t ledtaskHandler = NULL;

uint32_t red = 0;
uint32_t green = 0;
uint32_t blue = 0;
uint32_t blink = 2;


xSemaphoreHandle report_semaphore;
static char  TaskList[1024];
char * ptrTaskList = &TaskList[0];

void second_nvs_test(void)
{

    ESP_LOGI(TAG, "S T O R A G E start");

    nvs_handle_t storage;
    esp_err_t ret;

    char issid[32];
    char ipass[32];

    const esp_partition_t* partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition);

    printf("partition \"%s\" (0x%" PRIx32 " bytes at 0x%" PRIx32 ")\n", partition->label, partition->size, partition->address);



    /* Initialize the custom NVS partition */
    ret = nvs_flash_init_partition(CUSTOM_NVS_PART_NAME);
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase_partition(CUSTOM_NVS_PART_NAME));
        ret = nvs_flash_init_partition(CUSTOM_NVS_PART_NAME);
    }
    ESP_ERROR_CHECK(ret);

    uint8_t data_size = 128;
    uint8_t read_data[data_size];
    printf("Reading with esp_partition_read:\n");
    ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, data_size));
    ESP_LOG_BUFFER_HEXDUMP(TAG, read_data, data_size, ESP_LOG_INFO);


//    sprintf(issid,"%s","ola mola");
//    sprintf(ipass,"%s","noli la");
//
//
//
//   ESP_ERROR_CHECK( nvs_flash_init());
//   ESP_ERROR_CHECK( nvs_open("storage", NVS_READWRITE, &storage));
//   ESP_ERROR_CHECK( nvs_set_str(storage, "cpyssid", issid));
//   ESP_ERROR_CHECK( nvs_set_str(storage, "cpypass", ipass));
//   nvs_close(storage);



         size_t ssidLen, passLen;
         char *ssid = NULL, *pass = NULL;

           nvs_open_from_partition(partition->label, "second", NVS_READWRITE, &storage);

             if (nvs_get_str(storage, "myid", NULL, &ssidLen) == ESP_OK)
             {
               if (ssidLen > 0)
               {
                 ssid = malloc(ssidLen);
                 nvs_get_str(storage, "myid", ssid, &ssidLen);
               }
             }
             else
           	  ESP_LOGI(TAG, "S T O R A G E fail 1");

             if (nvs_get_str(storage, "mypass", NULL, &passLen) == ESP_OK)
             {
               if (passLen > 0)
               {
                 pass = malloc(passLen);
                 nvs_get_str(storage, "mypass", pass, &passLen);
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

#define MAC_ADDR_SIZE 6

static void example_print_chip_info(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;

    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);

    uint32_t size_flash_chip;
    esp_flash_get_size(NULL, &size_flash_chip);
    printf("size_flash_chip=%d\n", size_flash_chip);

     uint8_t mac[MAC_ADDR_SIZE];

     esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
     ESP_LOGI("MAC STA address", "MAC address: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

}

void report_task(void *parms)
{


   while(1)
   {
 	        xSemaphoreTake(report_semaphore, portMAX_DELAY);
	    	     vTaskList(ptrTaskList);
	    	      ESP_LOGI("REPORT", "\nTask\t\tState\tPrio\tStack\tNum\n%s",ptrTaskList);
	    	     vTaskGetRunTimeStats(ptrTaskList);
	    	      ESP_LOGI("REPORT", "\nTask\t\tRunTime\t\tTimePercent\n%s",ptrTaskList);
	    	     vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

int pos=3;

static void colorled(void *pvParameters)
{


	 uint state;

    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(CONFIG_EXAMPLE_RMT_TX_GPIO, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(CONFIG_EXAMPLE_STRIP_LED_NUMBER, (led_strip_dev_t)config.channel);
    led_strip_t *strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip) {
        ESP_LOGE(TAG, "install WS2812 driver failed");
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));

    ESP_LOGI(TAG, "LED Color Start");

    while (true) {

    	 xTaskNotifyWait(0xffffffff, 0, &state, portMAX_DELAY);
    	    ESP_LOGI(TAG,"received state %d flags\n", state);





		if (state == 0 ) // clear
		{
		  strip->clear(strip, 50);
		  vTaskDelay(pdMS_TO_TICKS(EXAMPLE_CHASE_SPEED_MS));
		}
		else if (state == 1 ) // bink
		{ ESP_ERROR_CHECK(strip->set_pixel(strip, pos, red, green, blue));
	      ESP_ERROR_CHECK(strip->refresh(strip, 100));
		  vTaskDelay(pdMS_TO_TICKS(EXAMPLE_CHASE_SPEED_MS));
		  strip->clear(strip, 50);
		  vTaskDelay(pdMS_TO_TICKS(EXAMPLE_CHASE_SPEED_MS));
		  xTaskNotify(ledtaskHandler, (1), eSetBits);

		}
		else if (state == 2 ) // one shot
		{ ESP_ERROR_CHECK(strip->set_pixel(strip, pos, red, green, blue));
	      ESP_ERROR_CHECK(strip->refresh(strip, 100));
		  vTaskDelay(pdMS_TO_TICKS(EXAMPLE_CHASE_SPEED_MS));
		  strip->clear(strip, 50);
		  vTaskDelay(pdMS_TO_TICKS(EXAMPLE_CHASE_SPEED_MS));
		}
		else if (state == 3 ) // continuous
		{ ESP_ERROR_CHECK(strip->set_pixel(strip, pos, red, green, blue));
	      ESP_ERROR_CHECK(strip->refresh(strip, 100));
	     // vTaskDelay(pdMS_TO_TICKS(50));


		}
		else
			vTaskDelay(pdMS_TO_TICKS(50));




    }
}





// Get the string name of type enum values used in this example
static const char* get_type_str(esp_partition_type_t type)
{
    switch(type) {
        case ESP_PARTITION_TYPE_APP:
            return "ESP_PARTITION_TYPE_APP";
        case ESP_PARTITION_TYPE_DATA:
            return "ESP_PARTITION_TYPE_DATA";
        default:
            return "UNKNOWN_PARTITION_TYPE"; // type not used in this example
    }
}

// Get the string name of subtype enum values used in this example
static const char* get_subtype_str(esp_partition_subtype_t subtype)
{
    switch(subtype) {
        case ESP_PARTITION_SUBTYPE_DATA_NVS:
            return "ESP_PARTITION_SUBTYPE_DATA_NVS";
        case ESP_PARTITION_SUBTYPE_DATA_PHY:
            return "ESP_PARTITION_SUBTYPE_DATA_PHY";
        case ESP_PARTITION_SUBTYPE_APP_FACTORY:
            return "ESP_PARTITION_SUBTYPE_APP_FACTORY";
        case ESP_PARTITION_SUBTYPE_DATA_FAT:
            return "ESP_PARTITION_SUBTYPE_DATA_FAT";
        default:
            return "UNKNOWN_PARTITION_SUBTYPE"; // subtype not used in this example
    }
}

// Find the partition using given parameters
static void find_partition(esp_partition_type_t type, esp_partition_subtype_t subtype, const char* name)
{

    ESP_LOGI(TAG, "Find partition with type %s, subtype %s, label %s...", get_type_str(type), get_subtype_str(subtype),
                        name == NULL ? "NULL (unspecified)" : name);

    const esp_partition_t * part  = esp_partition_find_first(type, subtype, name);

    if (part != NULL) {
        ESP_LOGI(TAG, "\tfound partition '%s' at offset 0x%x with size 0x%x", part->label, part->address, part->size);
    } else {
        ESP_LOGE(TAG, "\tpartition not found!");
    }
}

//void Partition_test(void)
//{
//    /*
//    * This example uses the partition table from ../partitions_example.csv. For reference, its contents are as follows:
//    *
//    *  nvs,        data, nvs,      0x9000,  0x6000,
//    *  phy_init,   data, phy,      0xf000,  0x1000,
//    *  factory,    app,  factory,  0x10000, 1M,
//    *  storage1,   data, fat,             , 0x40000,
//    *  storage2,   data, fat,             , 0x40000,
//    *
//    * Display the partition table to the user for reference.
//    */
//    extern const char csv_start[] asm("_binary_partitions_example_csv_start");
//    extern const char csv_end[]   asm("_binary_partitions_example_csv_end");
//
//    ESP_LOGI(TAG, "Printing partition table csv file contents for reference...\n\n%.*s", csv_end - csv_start + 1, csv_start);
//
//    /* First Part - Finding partitions using esp_partition_find_first. */
//
//    ESP_LOGI(TAG, "----------------Find partitions---------------");
//
//    // Find partitions using esp_partition_find_first(). This returns the first partition matching the passed constraints.
//    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
//    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_PHY, NULL);
//    find_partition(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
//
//    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
//    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
//    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
//    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
//    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
//
//    ESP_LOGI(TAG, "Find second FAT partition by specifying the label");
//    // In case of multiple matches, `esp_partition_find_first` returns the first match.
//    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs_ext");
//
//    /* Second Part - Iterating over partitions */
//
//    ESP_LOGI(TAG, "----------------Iterate through partitions---------------");
//
//    esp_partition_iterator_t it;
//
//    ESP_LOGI(TAG, "Iterating through app partitions...");
//    it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
//
//    // Loop through all matching partitions, in this case, all with the type 'data' until partition with desired
//    // label is found. Verify if its the same instance as the one found before.
//    for (; it != NULL; it = esp_partition_next(it)) {
//        const esp_partition_t *part = esp_partition_get(it);
//        ESP_LOGI(TAG, "\tfound partition '%s' at offset 0x%x with size 0x%x", part->label, part->address, part->size);
//    }
//    // Release the partition iterator to release memory allocated for it
//    esp_partition_iterator_release(it);
//
//    ESP_LOGI(TAG, "Iterating through data partitions...");
//    it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
//
//    // Loop through all matching partitions, in this case, all with the type 'data' until partition with desired
//    // label is found. Verify if its the same instance as the one found before.
//    for (; it != NULL; it = esp_partition_next(it)) {
//        const esp_partition_t *part = esp_partition_get(it);
//        ESP_LOGI(TAG, "\tfound partition '%s' at offset 0x%x with size 0x%x", part->label, part->address, part->size);
//    }
//
//    // Release the partition iterator to release memory allocated for it
//    esp_partition_iterator_release(it);
//
//    ESP_LOGI(TAG, "Example end");
//}


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

//        sprintf(issid,"%s","MERLIN");
//        sprintf(ipass,"%s","narnia1523");

         sprintf(issid,"%s","Mertech_2_4");
         sprintf(ipass,"%s","MeR0TecH_2");


        ESP_ERROR_CHECK( nvs_flash_init());
        nvs_handle_t nvs;
        ESP_ERROR_CHECK( nvs_open("wifiCreds", NVS_READWRITE, &nvs));
        ESP_ERROR_CHECK( nvs_set_str(nvs, "ssid", issid));
        ESP_ERROR_CHECK( nvs_set_str(nvs, "pass", ipass));
        nvs_close(nvs);

        LED_Init();

      //  test_can_prefilter();

     //    app_main_GSM_test();

   example_wifi_connect();
    stntp_set();

       // test_can_linklist();


      //  Partition_test();

       // example_print_chip_info();

        //second_nvs_test();


      //  uart_echo_test();



  //  whatsapp_main();


   // BLE_Init();
    // GSM_Init();

   //  IP5306_test();
   //  bat_Init();


   //  ota_app();

// aws_main();

//     report_semaphore = xSemaphoreCreateBinary();
//
//	 xTaskCreate(report_task, "report", 1024*2, NULL, configMAX_PRIORITIES, NULL);
//	 xTaskCreatePinnedToCore(&colorled, "color_led", 1024*3, NULL, 5, &ledtaskHandler,1);
//
//	 red = 100;
//	 vTaskDelay(pdMS_TO_TICKS(EXAMPLE_CHASE_SPEED_MS));
//	 xTaskNotify(ledtaskHandler, (3), eSetBits);

 //   xTaskCreatePinnedToCore(&http_server_task, "http_server", 1024*4, NULL, 5, NULL,0);

  test_can_prefilter();
  CAN_Test();
  can_buffer_run();

 // Paneltest();
    while (1)
     {

    	   vTaskDelay(pdMS_TO_TICKS(10));
     }
}
