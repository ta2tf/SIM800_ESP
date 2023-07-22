/*
 * can.c
 *
 *  Created on: 14 Jun 2023
 *      Author: MertechArge014
 */





#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "can.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "sys/time.h"

//
//struct tm {
//   int tm_sec;         /* seconds,  range 0 to 59          */
//   int tm_min;         /* minutes, range 0 to 59           */
//   int tm_hour;        /* hours, range 0 to 23             */
//   int tm_mday;        /* day of the month, range 1 to 31  */
//   int tm_mon;         /* month, range 0 to 11             */
//   int tm_year;        /* The number of years since 1900   */
//   int tm_wday;        /* day of the week, range 0 to 6    */
//   int tm_yday;        /* day in the year, range 0 to 365  */
//   int tm_isdst;       /* daylight saving time             */
//};



/* --------------------- CAN Definitions and static variables ------------------ */

//CAN Configurations


/*
 TX GPIO number This option selects the GPIO pin used for the TX signal. Connect the TX signal to your transceiver.
          2    IDF_TARGET_ESP32C3
          20   IDF_TARGET_ESP32S2
          21   IDF_TARGET_ESP32
          4    IDF_TARGET_ESP32S3

 RX GPIO number This option selects the GPIO pin used for the RX signal. Connect the RX signal to your transceiver.
          3    IDF_TARGET_ESP32C3
          21   IDF_TARGET_ESP32S2
          22   IDF_TARGET_ESP32
          5    IDF_TARGET_ESP32S3
*/

//  TARGET ESP32
 #define TX_GPIO_NUM             22
 #define RX_GPIO_NUM             21

////  TARGET ESP32 S3
//#define TX_GPIO_NUM             4
//#define RX_GPIO_NUM             5


#define RX_TASK_PRIO            8     //Receiving task priority
#define TX_TASK_PRIO            9
#define FILTER_MSG_ID                  0x1FFFFFFF   //11 bit standard format ID
#define FILTER_MSG_MASK                0x1FFFFFFF   //11 bit standard format ID


#define CAN_TAG             "CAN:"

static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

//Filter all other IDs except MSG_ID
//static const twai_filter_config_t f_config = {.acceptance_code = (FILTER_MSG_ID << 3),
//                                             .acceptance_mask = ~(FILTER_MSG_MASK<<3),
//                                             .single_filter = true};
//Set to NO_ACK mode due to self testing with single module
static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);
static char canXML[256];
static char canJSON[256];
static char canDec[256];

#define TWAI_EXTD_ID_MASK               0x1FFFFFFF  /**< Bit mask for 29 bit Extended Frame Format ID */
#define TWAI_STD_ID_MASK                0x7FF       /**< Bit mask for 11 bit Standard Frame Format ID */




QueueHandle_t can_rx_queue = NULL;


enum e_pre_filter
{
  Filter_always,
  filter_mask,
  filter_whitelist,
  filter_blacklist
};



/* --------------------------- CAN Tasks and Functions -------------------------- */



static void twai_transmit_task(void *arg)
{


	//Configure message to transmit
	twai_message_t message;
	message.identifier = 0x18FD0900;
	message.extd = 1;
	message.data_length_code = 8;

	for (int i = 0; i < 8; i++) {
	    message.data[i] = i;
	}


	while (1) {


            twai_transmit(&message, 10000);
            ESP_LOGI(CAN_TAG, "Transmitted ping ");
            vTaskDelay(pdMS_TO_TICKS(5000));

        }

    vTaskDelete(NULL);
}

//==========================================================================================
//==========================================================================================
// twai_receive_task
//==========================================================================================
//==========================================================================================
static void twai_receive_task(void *arg)
{
	rx_message_t rx_message;


	time_t now;
	char strftime_buf[64];
	struct tm timeinfo;


	struct timeval tv_now;
	int     time_ms;
	int64_t time_ms_full;

    ESP_LOGI(CAN_TAG,"%s", "twai_receive_task STARTED");

   while (1)
        {
            //Receive message and print message data
            ESP_ERROR_CHECK(twai_receive(&rx_message.can, portMAX_DELAY));



         	time(&now);
			localtime_r(&now, &timeinfo);

		//	strftime(strftime_buf, sizeof(strftime_buf), "%d/%m/%Y - %X", &timeinfo);

            gettimeofday(&tv_now, NULL);

        	time_ms = tv_now.tv_usec;
        	time_ms = time_ms / 1000;

        	rx_message.rtm.tm_hour = timeinfo.tm_hour;
        	rx_message.rtm.tm_min  = timeinfo.tm_min;
        	rx_message.rtm.tm_sec  = timeinfo.tm_sec;
        	rx_message.rtm.tm_msec = time_ms;

        	rx_message.rtm.tm_mday = timeinfo.tm_mday;
        	rx_message.rtm.tm_mon =  timeinfo.tm_mon+1;
        	rx_message.rtm.tm_year = timeinfo.tm_year+1900;



            xQueueSend(can_rx_queue, &rx_message, portMAX_DELAY);

                    if (!(rx_message.can.rtr))
                    {
//                    	sprintf(canXML,"<CANLOG><MSG><ID>%08X</ID><D0>%02X</D0><D1>%02X</D1><D2>%02X</D2><D3>%02X</D3><D4>%02X</D4><D5>%02X</D5><D6>%02X</D6><D7>%02X</D7></MSG></CANLOG>",
//                    			rx_message.identifier,
//                    			rx_message.data[0],
//								rx_message.data[1],
//								rx_message.data[2],
//								rx_message.data[3],
//								rx_message.data[4],
//								rx_message.data[5],
//								rx_message.data[6],
//								rx_message.data[7]
//                    	);


//						sprintf(canJSON, "{\"MSG\": {\"ID\": \"%08X\",\"D0\": \"%02X\",\"D1\": \"%02X\",\"D2\": \"%02X\",\"D3\": \"%02X\",\"D4\": \"%02X\",\"D5\": \"%02X\",\"D6\": \"%02X\",\"D7\": \"%02X\"}}",
//                    			rx_message.identifier,
//                    			rx_message.data[0],
//								rx_message.data[1],
//								rx_message.data[2],
//								rx_message.data[3],
//								rx_message.data[4],
//								rx_message.data[5],
//								rx_message.data[6],
//								rx_message.data[7]
//                    	);
//




						sprintf(canDec, "[%02d/%02d/%04d %02d:%02d:%02d-%03d] 0x%08X %02X %02X %02X %02X %02X %02X %02X %02X",

								rx_message.rtm.tm_mday,
								rx_message.rtm.tm_mon,
								rx_message.rtm.tm_year,

								rx_message.rtm.tm_hour,
								rx_message.rtm.tm_min,
								rx_message.rtm.tm_sec,
								rx_message.rtm.tm_msec,

                    			rx_message.can.identifier,
                    			rx_message.can.data[0],
								rx_message.can.data[1],
								rx_message.can.data[2],
								rx_message.can.data[3],
								rx_message.can.data[4],
								rx_message.can.data[5],
								rx_message.can.data[6],
								rx_message.can.data[7]
                    	);




                    //    ESP_LOGI(CAN_TAG,"%s", canDec);



//                        if(MQTT_CONNEECTED)
//                        {
//                            esp_mqtt_client_publish(client, "/topic/can", canJSON, 0, 0, 0);
//                        }
//









                     }
                   // vTaskDelay(pdMS_TO_TICKS(1));
         }
}




void CAN_Test(void)
{
	 ESP_LOGI(CAN_TAG, "Can Driver start");
	  //Install TWAI driver
	  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
	  ESP_LOGI(CAN_TAG, "Can Driver installed");

	  ESP_ERROR_CHECK(twai_start());
	  ESP_LOGI(CAN_TAG, "Can Driver started");

	  can_rx_queue = xQueueCreate(100, sizeof(rx_message_t));
	  xTaskCreatePinnedToCore(twai_receive_task, "TWAI_rx", 4096, NULL, RX_TASK_PRIO, NULL, tskNO_AFFINITY);
	  xTaskCreatePinnedToCore(twai_transmit_task, "TWAI_tx", 4096, NULL, TX_TASK_PRIO, NULL, tskNO_AFFINITY);
}
