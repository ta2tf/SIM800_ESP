/*
 * battery.c
 *
 *  Created on: 10 May 2023
 *      Author: MertechArge014
 */



#include <battery.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_err.h"


static const char *BAT_TAG = "BAT_TASK";

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling


QueueHandle_t BatteryQueue;


static esp_adc_cal_characteristics_t *adc_chars;

static const adc_channel_t channel = ADC_CHANNEL_7;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;

static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;


static void check_efuse(void)
{

    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
    	ESP_LOGI(BAT_TAG, "eFuse Two Point: Supported\n");
    } else {
    	ESP_LOGI(BAT_TAG, "eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
    	ESP_LOGI(BAT_TAG, "eFuse Vref: Supported\n");
    } else {
    	ESP_LOGI(BAT_TAG, "eFuse Vref: NOT supported\n");
    }

}


static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    	ESP_LOGI(BAT_TAG, "Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    	ESP_LOGI(BAT_TAG, "Characterized using eFuse Vref\n");
    } else {
    	ESP_LOGI(BAT_TAG, "Characterized using Default Vref\n");
    }
}


void bat_task(void *arg)
{

	esp_log_level_set(BAT_TAG, ESP_LOG_INFO);

    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    //Continuously sample ADC1
    while (1) {
        int adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        int voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        ESP_LOGI(BAT_TAG, "Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);

        xQueueSend(BatteryQueue, &voltage, NULL);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



//==========================================================================================================
 //
 //==========================================================================================================
  void bat_Init(void)
 {

   BatteryQueue = xQueueCreate(10, sizeof(int));
   xTaskCreate(bat_task, "Battery_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);

 }

