/* MQTT Mutual Authentication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "aws.h"


extern SemaphoreHandle_t s_semph_get_ip_addrs;

static const char *TAG = "MQTTS_EXAMPLE";

// this is for code embedded certs
//extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
//extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");
//
//extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
//extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");
//
//extern const uint8_t server1_cert_pem_start[] asm("_binary_awsrootca_crt_start");
//extern const uint8_t server1_cert_pem_end[] asm("_binary_awsrootca_crt_end");



char * private_crt;
char * private_key;
char * certificate;

size_t value_size_private_crt;
size_t value_size_private_key;
size_t value_size_certificate;



char * nvs_load_value_if_exist(nvs_handle handle, const char* key, size_t *value_size )
{
	static const char TAG[] = "NVS-Test";

    // Try to get the size of the item

    if(nvs_get_str(handle, key, NULL, value_size) != ESP_OK){
        ESP_LOGE(TAG, "Failed to get size of key: %s", key);
        return NULL;
    }

    char* value = malloc(*value_size);
    if(nvs_get_str(handle, key, value, value_size) != ESP_OK){
        ESP_LOGE(TAG, "Failed to load key: %s", key);
        return NULL;
    }

    return value;
}



void NVS_certificate_get(void)
{
	static const char TAG[] = "NVS-GET";

	char logg[128];

    // Initialize NVS
    ESP_LOGI(TAG, "test start  NVS");

    // Open the "certs" namespace in read-only mode
    nvs_handle handle;
    ESP_ERROR_CHECK( nvs_open("certs", NVS_READONLY, &handle));

    // Load the private key & certificate
    ESP_LOGI(TAG, "Loading private key & certificate");

    ESP_LOGI(TAG, "HEAP FREE SIZE %d",esp_get_free_heap_size());


     private_crt = nvs_load_value_if_exist(handle, "priv_crt", &value_size_private_crt);
     private_key = nvs_load_value_if_exist(handle, "priv_key", &value_size_private_key);
     certificate = nvs_load_value_if_exist(handle, "certificate", &value_size_certificate);

    // We're done with NVS
    nvs_close(handle);

    ESP_LOGI(TAG, "HEAP FREE SIZE %d",esp_get_free_heap_size());

    // Check if both items have been correctly retrieved
    if(private_crt == NULL || private_key == NULL || certificate == NULL){
        ESP_LOGE(TAG, "Private key or cert could not be loaded");
        return; // You might want to handle this in a better way
    }
    else
    	ESP_LOGE(TAG, "Private key or cert  be loaded");

    memset(logg,0,sizeof(logg));
    memcpy(logg,private_crt,32);
    ESP_LOGI(TAG, "private_crt NVS %d  %s", value_size_private_crt, logg);

    memset(logg,0,sizeof(logg));
    memcpy(logg,private_key,32);
    ESP_LOGI(TAG, "private_key NVS %d  %s", value_size_private_key, logg);

    memset(logg,0,sizeof(logg));
    memcpy(logg,certificate,32);
    ESP_LOGI(TAG, "certificate NVS  %d  %s", value_size_certificate,  logg);

    ESP_LOGI(TAG, "test stop  NVS");

//    free(private_crt);
//    free(private_key);
//    free(certificate);

    ESP_LOGI(TAG, "HEAP FREE SIZE %d",esp_get_free_heap_size());

}



static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
       ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
       ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
       ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
       ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
       ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
       ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
       ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
       ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
       ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
       ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
       ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
           ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
       ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{

// this is for code embedded certs
//  const esp_mqtt_client_config_t mqtt_cfg = {
//    .broker.address.uri = "mqtts://asiqb1fo39ypl-ats.iot.eu-central-1.amazonaws.com",
//    .broker.verification.certificate = (const char *)server1_cert_pem_start,
//    .credentials = {
//      .authentication = {
//        .certificate = (const char *)client_cert_pem_start,
//        .key = (const char *)client_key_pem_start,
//      },
//    }
//  };
//

  const esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtts://asiqb1fo39ypl-ats.iot.eu-central-1.amazonaws.com",
    .broker.verification.certificate = (const char *)certificate,
    .credentials = {
      .authentication = {
        .certificate = (const char *)private_crt,
        .key = (const char *)private_key,
      },
    }
  };

   ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void aws_main(void)
{
   ESP_LOGI(TAG, "[APP] Startup..");
   ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
   ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    NVS_certificate_get();

   // esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_INFO);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_INFO);
    esp_log_level_set("TRANSPORT", ESP_LOG_INFO);
    esp_log_level_set("OUTBOX", ESP_LOG_INFO);

    ESP_ERROR_CHECK(nvs_flash_init());


    if (s_semph_get_ip_addrs) {
         xSemaphoreTake(s_semph_get_ip_addrs,500000);
    mqtt_app_start();
    }
}
