/*
 * gsm.c
 *
 *  Created on: 9 May 2023
 *      Author: MertechArge014
 */






#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>

#include "driver/gpio.h"
#include "driver/uart.h"

#include "gsm.h"


 void GSM_TX_Task(void *arg);
 void GSM_RX_Task(void *arg);
 void GSM_INT_Task(void *arg);


 void GSM_PowerInit(void);
 void GSM_PINInit(void);
 void GSM_UART_Init(void);



static const int GSM_RX_BUF_SIZE = 1024;

QueueHandle_t GSM_RX_Queue;
QueueHandle_t interputQueue;





//==========================================================================================================
//
//==========================================================================================================
 void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}




//==========================================================================================================
//
//==========================================================================================================
 void GSM_PINInit(void)
{
	// RI pin Ring interrupt enable
	gpio_reset_pin(MODEM_RI);
	gpio_set_direction(MODEM_RI, GPIO_MODE_INPUT);



	gpio_reset_pin(MODEM_DTR);
	gpio_set_direction(MODEM_DTR, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MODEM_PWRKEY);
    gpio_set_direction(MODEM_PWRKEY, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MODEM_POWER_ON);
    gpio_set_direction(MODEM_POWER_ON, GPIO_MODE_OUTPUT);



    // GSM power supply Enable
    gpio_set_level(MODEM_POWER_ON, HI_LEVEL);

}


 //==========================================================================================================
 //
 //==========================================================================================================
  void GSM_RINGInit(void)
 {
	gpio_intr_enable(MODEM_RI);
	gpio_set_intr_type(MODEM_RI,GPIO_INTR_ANYEDGE);


	gpio_install_isr_service(0);
	gpio_isr_handler_add(MODEM_RI, gpio_interrupt_handler, (void *)MODEM_RI);

 }

//==========================================================================================================
//
//==========================================================================================================
 void GSM_PowerUp(void)
{

    // toggle SIM800 Powerkey
    gpio_set_level(MODEM_PWRKEY, HI_LEVEL);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(MODEM_PWRKEY, LO_LEVEL);
    vTaskDelay(1000/ portTICK_PERIOD_MS);
    gpio_set_level(MODEM_PWRKEY, HI_LEVEL);

}


//==========================================================================================================
//
//==========================================================================================================
void GSM_UART_Init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, GSM_RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, MODEM_TX, MODEM_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

//==========================================================================================================
//
//==========================================================================================================
int GSM_SendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

//==========================================================================================================
//
//==========================================================================================================
 void GSM_TX_Task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);

    GSM_SendData(TX_TASK_TAG, "AT+CMIC=0,15\r\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    GSM_SendData(TX_TASK_TAG, "AT+CMIC=1,15\r\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    GSM_SendData(TX_TASK_TAG, "AT+CMIC=2,15\r\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    GSM_SendData(TX_TASK_TAG, "AT+CMIC=3,15\r\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);


    GSM_SendData(TX_TASK_TAG, "AT+CEXTERNTONE=0\r\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);


    GSM_SendData(TX_TASK_TAG, "AT+CMICBIAS=0\r\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    while (1) {
    	GSM_SendData(TX_TASK_TAG, "AT+CSQ\r\n");
        vTaskDelay(20000 / portTICK_PERIOD_MS);

//        GSM_SendData(TX_TASK_TAG, "AT+CADC?\r\n");
//        vTaskDelay(20000 / portTICK_PERIOD_MS);


    }
}


//==========================================================================================================
//
//==========================================================================================================
 void GSM_RX_Task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(GSM_RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, GSM_RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
    }
    free(data);
}

 //==========================================================================================================
 //
 //==========================================================================================================
  void GSM_INT_Task(void *arg)
 {
     static const char *INT_TASK_TAG = "INT_TASK";
     esp_log_level_set(INT_TASK_TAG, ESP_LOG_INFO);
     while (1) {
     	int pinNumber = 0;
         if (xQueueReceive(interputQueue, &pinNumber, portMAX_DELAY))
         {
         	ESP_LOGI(INT_TASK_TAG,"RING Received GPIO %d ", pinNumber);
         	GSM_SendData(INT_TASK_TAG, "ATA\r\n");

         }
     }
 }


 //==========================================================================================================
 //
 //==========================================================================================================
  void GSM_Init(void)
 {

	 GSM_PINInit();

	 GSM_PowerUp();

	 GSM_UART_Init();

	  vTaskDelay(5000 / portTICK_PERIOD_MS);

	 xTaskCreate(GSM_RX_Task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
	 xTaskCreate(GSM_TX_Task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);

	 interputQueue = xQueueCreate(10, sizeof(int));
	 xTaskCreate(GSM_INT_Task, "uart_INT_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);

	 GSM_RINGInit();
 }
