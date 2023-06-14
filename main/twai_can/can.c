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

#include "can.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "hal/twai_types.h"




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
// #define TX_GPIO_NUM             21
// #define RX_GPIO_NUM             22

//  TARGET ESP32 S3
#define TX_GPIO_NUM             4
#define RX_GPIO_NUM             5


#define RX_TASK_PRIO            9            //Receiving task priority
#define FILTER_MSG_ID                  0x18FD0900   //11 bit standard format ID
#define FILTER_MSG_MASK                0x18000000   //11 bit standard format ID


#define CAN_TAG             "CAN:"

static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
//Filter all other IDs except MSG_ID
static const twai_filter_config_t f_config = {.acceptance_code = (FILTER_MSG_ID << 3),
                                             .acceptance_mask = ~(FILTER_MSG_MASK<<3),
                                             .single_filter = true};
//Set to NO_ACK mode due to self testing with single module
static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
static char canXML[256];
static char canJSON[256];
static char canXMLlist[6*1024];

#define TWAI_EXTD_ID_MASK               0x1FFFFFFF  /**< Bit mask for 29 bit Extended Frame Format ID */
#define TWAI_STD_ID_MASK                0x7FF       /**< Bit mask for 11 bit Standard Frame Format ID */




QueueHandle_t can_rx_queue = NULL;



/* --------------------------- CAN Tasks and Functions -------------------------- */

//==========================================================================================
//==========================================================================================
// twai_receive_task
//==========================================================================================
//==========================================================================================
static void twai_receive_task(void *arg)
{
    twai_message_t rx_message;

   while (1)
        {
            //Receive message and print message data
            ESP_ERROR_CHECK(twai_receive(&rx_message, portMAX_DELAY));

                    if (!(rx_message.rtr))
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


						sprintf(canJSON, "{\"MSG\": {\"ID\": \"%08X\",\"D0\": \"%02X\",\"D1\": \"%02X\",\"D2\": \"%02X\",\"D3\": \"%02X\",\"D4\": \"%02X\",\"D5\": \"%02X\",\"D6\": \"%02X\",\"D7\": \"%02X\"}}",
                    			rx_message.identifier,
                    			rx_message.data[0],
								rx_message.data[1],
								rx_message.data[2],
								rx_message.data[3],
								rx_message.data[4],
								rx_message.data[5],
								rx_message.data[6],
								rx_message.data[7]
                    	);



                        ESP_LOGI(CAN_TAG,"%s", canXML);



//                        if(MQTT_CONNEECTED)
//                        {
//                            esp_mqtt_client_publish(client, "/topic/can", canJSON, 0, 0, 0);
//                        }
//


                        can_message_t InCanMsg;


                        InCanMsg.identifier = rx_message.identifier;
                        InCanMsg.data_length_code = rx_message.data_length_code;

                        for(int j=0;j< InCanMsg.data_length_code;j++)
                        	InCanMsg.data[j] = rx_message.data[j];

                        xQueueSend(can_rx_queue, &InCanMsg, portMAX_DELAY);





                    }
                    vTaskDelay(pdMS_TO_TICKS(1));
         }
}



void CAN_Test(void)
{

	  //Install TWAI driver
	  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
	  ESP_LOGI(CAN_TAG, "Can Driver installed");

	  ESP_ERROR_CHECK(twai_start());
	  ESP_LOGI(CAN_TAG, "Can Driver started");

	  can_rx_queue = xQueueCreate(100, sizeof(can_message_t));


}
