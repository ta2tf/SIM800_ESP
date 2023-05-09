/*
 * led.c
 *
 *  Created on: 9 May 2023
 *      Author: MertechArge014
 */



#include "led.h"



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"


#include "driver/gpio.h"
#include "esp_check.h"



static uint8_t s_led_state = 0;

//==========================================================================================================
//
//==========================================================================================================
 void configure_led(void)
{
    gpio_reset_pin(LED_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}


//==========================================================================================================
//
//==========================================================================================================
 void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(LED_GPIO, s_led_state);
}

//==========================================================================================================
//
//==========================================================================================================
 void led_blink_task(void *arg)
{

	configure_led();

    while (1) {

    	blink_led();
    	s_led_state = !s_led_state;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

 //==========================================================================================================
 //
 //==========================================================================================================
  void LED_Init(void)
 {
   configure_led();
   xTaskCreate(led_blink_task, "led_blink_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);
 }
