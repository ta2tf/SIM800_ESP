/*
 * panel.c
 *
 *  Created on: 21 Jun 2023
 *      Author: MertechArge014
 */

#include "panel.h"



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"


#include "driver/gpio.h"
#include "esp_check.h"

static const char *TAG = "MAIN";


//  Pinout
// ┌─────────────┐
// │  R1 o o G1  │
// │  B1 o o GND │
// │  R2 o o G2  │
// │  B2 o o GND │
// │  A  o o B   │
// │  C  o o GND │
// │  S  o o L   │
// │  OE o o GND │
// └─────────────┘


#define PIN_R1  33

#define PIN_A  4
#define PIN_B  2
#define PIN_C  15

#define PIN_S  25
#define PIN_L  26
#define PIN_OE 27






//==========================================================================================================
//
//==========================================================================================================
 void configure_pins(void)
{


	gpio_reset_pin(PIN_R1);
	gpio_set_direction(PIN_R1, GPIO_MODE_OUTPUT);


    gpio_reset_pin(PIN_A);
    gpio_set_direction(PIN_A, GPIO_MODE_OUTPUT);

    gpio_reset_pin(PIN_B);
    gpio_set_direction(PIN_B, GPIO_MODE_OUTPUT);

    gpio_reset_pin(PIN_C);
    gpio_set_direction(PIN_C, GPIO_MODE_OUTPUT);

    gpio_reset_pin(PIN_S);
    gpio_set_direction(PIN_S, GPIO_MODE_OUTPUT);

    gpio_reset_pin(PIN_L);
    gpio_set_direction(PIN_L, GPIO_MODE_OUTPUT);

    gpio_reset_pin(PIN_OE);
    gpio_set_direction(PIN_OE, GPIO_MODE_OUTPUT);

}

 //==========================================================================================================
 //
 //==========================================================================================================
 void PinSet(gpio_num_t gpio_num)
 {
     gpio_set_level(gpio_num, 1);
 }


 //==========================================================================================================
  //
  //==========================================================================================================
  void Pinclock(void)
  {
      gpio_set_level(PIN_S, 0);
      vTaskDelay(1 / portTICK_PERIOD_MS);

      gpio_set_level(PIN_S, 1);
      vTaskDelay(2 / portTICK_PERIOD_MS);

      gpio_set_level(PIN_S, 0);
      vTaskDelay(1 / portTICK_PERIOD_MS);

  }


 //==========================================================================================================
 //
 //==========================================================================================================
 void PinReset(gpio_num_t gpio_num)
 {
     gpio_set_level(gpio_num, 0);
 }


 void latch(int line){

		PinSet(PIN_OE);
		PinSet(PIN_L);

 		if(line==0)
 		{
 			PinReset(PIN_A);
 			PinReset(PIN_B);
 			PinReset(PIN_C);
 		}
 		if(line==1)
 		{
 			PinSet(PIN_A);
 			PinReset(PIN_B);
 			PinReset(PIN_C);
 		}
 		if(line==2){
 			PinReset(PIN_A);
 			PinSet(PIN_B);
 			PinReset(PIN_C);
 		}
 		if(line==3){
 			PinSet(PIN_A);
 			PinSet(PIN_B);
 			PinReset(PIN_C);
 		}
 		if(line==4){
 			PinReset(PIN_A);
 			PinReset(PIN_B);
 			PinSet(PIN_C);
 		}
 		if(line==5){
 			PinSet(PIN_A);
 			PinReset(PIN_B);
 			PinSet(PIN_C);
 		}
 		if(line==6){
 			PinReset(PIN_A);
 			PinSet(PIN_B);
 			PinSet(PIN_C);
 		}
 		if(line==7){
 			PinSet(PIN_A);
 			PinSet(PIN_B);
 			PinSet(PIN_C);
 		}

 		PinReset(PIN_L);
 		PinReset(PIN_OE);

 }


void Paneltest(void)
{
	configure_pins();

while(1)
 {
	PinReset(PIN_A);
	PinReset(PIN_B);
	PinReset(PIN_C);



	PinSet(PIN_OE);
	PinReset(PIN_L);

	for(int j=0;j<64;j++)
	 {
		latch((j%7));

		PinSet(PIN_R1);
		Pinclock();

	 }


	PinSet(PIN_L);
	PinReset(PIN_OE);


	ESP_LOGI(TAG, "Panel");

	  vTaskDelay(10 / portTICK_PERIOD_MS);

 }
}



