/*
 * can_buf.c
 *
 *  Created on: 14 Jun 2023
 *      Author: MertechArge014
 *
 *
 *
 */




// +--------------+                       +----------+
// |              |                       |          |
// |              |                       |          |
// |              |                       |          |
// |              +--+---+---+--+---+--+--+          |
// |   TWAI       |  |   |   |  |   |  |  |  Linked  |
// |              +--+---+---+--+---+--+--+  List    |
// |              |     can_rx_queue      |          |
// |              |                       |          |
// |              |                       |          |
// |              |                       |          |
// +--------------+                       +----------+
//   twai_receive_task                   can_buffer_task
//
//
//                                         +----------+                    +----------+
//                                         |          |                    |          |
//                                         |          |                    |          |
//                                         |  Linked  +-+--+---+--+---+--+-+ Reporter |
//                                         |  List    | |  |   |  |   |  | |          |
//                                         |          +-+--+---+--+---+--+-+          |
//                                         |          |                    |          |
//                                         |          | report_can_rx_queue|          |
//                                         |          |                    |          |
//                                         |          |                    |          |
//                                         +----------+                    +----------+



// ┌──────────────┐                       ┌──────────┐
// │              │                       │          │
// │              │                       │          │
// │              │                       │          │
// │              ├──┬───┬───┬──┬───┬──┬──┤          │
// │   TWAI       │  │   │   │  │   │  │  │  Linked  │
// │              ├──┴───┴───┴──┴───┴──┴──┤  List    │
// │              │     can_rx_queue      │          │
// │              │                       │          │
// │              │                       │          │
// │              │                       │          │
// └──────────────┘                       └──────────┘
//   twai_receive_task                   can_buffer_task
//
//
//                                         ┌──────────┐                    ┌──────────┐
//                                         │          │                    │          │
//                                         │          │                    │          │
//                                         │  Linked  ├─┬──┬───┬──┬───┬──┬─┤ reporter │
//                                         │  List    │ │  │   │  │   │  │ │          │
//                                         │          ├─┴──┴───┴──┴───┴──┴─┤          │
//                                         │          │                    │          │
//                                         │          │ report_can_rx_queue│          │
//                                         │          │                    │          │
//                                         │          │                    │          │
//                                         └──────────┘                    └──────────┘

//  https://asciiflow.com/#/


#include <stdio.h>
#include <string.h>


#include "esp_wifi.h"
#include "esp_event.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"



#include "esp_log.h"
#include "driver/twai.h"
#include "can_buf.h"
#include "prefilter.h"

#include "can.h"
#include "time.h"
#include "aws.h"

extern QueueHandle_t can_rx_queue;

enum e_node_status
{
  node_unknown,
  node_new,
  node_updated,
  node_refreshed
};



enum post_filter_e
{
  post_filter_onchange,
  post_filter_ontime,
  post_filter_oncount
};




typedef struct node {
	Ltime_t        recTime;
	twai_message_t canMsg;
	uint32_t       cntMsg;
	enum e_node_status NodeStatus;
    struct node *next;
}node_t;



/* --------------------- Linked List  Definitions and  variables ------------------ */
struct node *head = NULL;
struct node *current = NULL;
struct node *prev = NULL;

uint16_t countList;

QueueHandle_t can_update_queue = NULL;
QueueHandle_t can_refresh_queue = NULL;


extern int32_t MQTT_CONNECTED;
extern esp_mqtt_client_handle_t client;

// Globals
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;



/* ---------------------------- Linked List  Functions -------------------------------- */

//==========================================================================================
// display  display the list
//==========================================================================================
void display(uint8_t detail) {
   struct node *ptr = head;

   printf("\n[head] =>");
   //start from the beginning
   while(ptr != NULL) {
      printf(" 0x%X =>",ptr->canMsg.identifier);

      if (detail == 1)
      {  printf("\n");
    	   for(int j=0;j< ptr->canMsg.data_length_code;j++)
    		   printf("CAN Data[%d]: 0x%x \r\n",j,ptr->canMsg.data[j]);
      }

      ptr = ptr->next;
   }

   printf(" [null]\n");
}


//==========================================================================================
// display  display the list
//==========================================================================================
void displayData(twai_message_t *data) {
   struct node *ptr = head;

   printf("CAN ID: 0x%x \r\n",data->identifier);
   for(int j=0;j< data->data_length_code;j++)
	   printf("CAN Data[%d]: 0x%x \r\n",j,data->data[j]);


}



//==========================================================================================
// size_of_list
//==========================================================================================
void size_of_list() {
   int size = 0;

   if(head==NULL) {
      printf("List size : %d \r\n", size);
      return;
   }

   current = head;
   size = 1;
   while(current->next!=NULL) {
      current = current->next;
      size++;
   }
   printf("List size : %d \r\n", size);
}


//==========================================================================================
// insert    insert link at the first location
//==========================================================================================
void insert(rx_message_t data) {
   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
   char topicID[32];
   char dataCAN[300];
   char databytes[64];
   char dbyte[3];


   //link->key = key;
   link->canMsg.identifier = data.can.identifier;
   link->canMsg.data_length_code = data.can.data_length_code;
   for(int j=0;j< data.can.data_length_code;j++)
	   link->canMsg.data[j] = data.can.data[j];


   link->recTime.tm_mday = data.rtm.tm_mday;
   link->recTime.tm_mon  = data.rtm.tm_mon;
   link->recTime.tm_year = data.rtm.tm_year;

   link->recTime.tm_hour = data.rtm.tm_hour;
   link->recTime.tm_min = data.rtm.tm_min;
   link->recTime.tm_sec = data.rtm.tm_sec;
   link->recTime.tm_msec = data.rtm.tm_msec;
   link->cntMsg = 1;
   countList++;

   link->NodeStatus = node_new;


   if(MQTT_CONNECTED)
   {

	   memset(topicID,0,sizeof(topicID));
	   sprintf(topicID,"/insert/%08X",link->canMsg.identifier);

	   // ["Ford", "BMW", "Fiat"]
	   memset(databytes,0,sizeof(databytes));
	   for (int j=0;j<link->canMsg.data_length_code;j++)
	    {
		   memset(dbyte,0,sizeof(dbyte));
		   if (j== link->canMsg.data_length_code-1)
		    sprintf(dbyte,"\"%02X\"",link->canMsg.data[j]);
		   else
			sprintf(dbyte,"\"%02X\",",link->canMsg.data[j]);

		   strcat(databytes,dbyte);
	    }

	   memset(dataCAN,0,sizeof(dataCAN));
	   sprintf(dataCAN,"{\"Date\": \"%02d/%02d/%04d\", \"Time\": \"%02d:%02d:%02d.%03d\", \"Counter\": \"%d\" , \"Length\": \"%d\", \"Data\": [%s] }",
			   link->recTime.tm_mday, link->recTime.tm_mon, link->recTime.tm_year,
			   link->recTime.tm_hour, link->recTime.tm_min, link->recTime.tm_sec, link->recTime.tm_msec,
			   link->cntMsg,
			   link->canMsg.data_length_code,
			   databytes
	   );

       esp_mqtt_client_publish(client, &topicID , &dataCAN, 0, 0, 0);
   }



   //point it to old first node
   link->next = head;

   //point first to new first node
   head = link;

   printf("Insert:%d listed  %X inserted \n" , countList, data.can.identifier);

}



//==========================================================================================
// find_data
//==========================================================================================
int find_data(uint32_t item) {
   int pos = 0;

   if(head==NULL) {
    //  printf("Find Data: Linked List not initialized");
      return -1;
   }

   current = head;

   while(current != NULL) {

	   if(current->canMsg.identifier == item) {
       //  printf("Find Data: 0x%X found at position %d\n", item, pos);
         return pos;
      }

      current = current->next;
      pos++;
   }




 //  printf("Find Data: %X does not exist in the list", item);
   return -1;
}


//==========================================================================================
// update_data
//==========================================================================================
void update_data(uint32_t CanID, rx_message_t new) {
   int pos = 0;

   char topicID[32];
   char dataCAN[300];
   char databytes[64];
   char dbyte[3];


   if(head==NULL) {
  //    printf("Update: Linked List not initialized");
      return;
   }

   current = head;

   while(current != NULL) {



      if(current->canMsg.identifier == CanID) {

    	 current->NodeStatus = node_refreshed;


    	 current->recTime.tm_mday = new.rtm.tm_mday;
    	 current->recTime.tm_mon  = new.rtm.tm_mon;
    	 current->recTime.tm_year = new.rtm.tm_year;

    	 current->recTime.tm_hour = new.rtm.tm_hour;
    	 current->recTime.tm_min  = new.rtm.tm_min;
    	 current->recTime.tm_sec  = new.rtm.tm_sec;
    	 current->recTime.tm_msec = new.rtm.tm_msec;
    	 current->cntMsg++;

         current->canMsg.data_length_code = new.can.data_length_code;
         for(int j=0;j< new.can.data_length_code;j++)
         {
        	 if (current->canMsg.data[j] != new.can.data[j])
        	 {
        	   current->canMsg.data[j] = new.can.data[j];
        	   current->NodeStatus = node_updated;
        	 }
         }



         if (current->NodeStatus == node_refreshed)
         {
           printf("Refreshed:    0x%08X  [%d]\n",current->canMsg.identifier, current->cntMsg);
           // add que refreshed

           if(MQTT_CONNECTED)
           {

        	   memset(topicID,0,sizeof(topicID));
        	   sprintf(topicID,"/refresh/%08X",current->canMsg.identifier);

        	   // ["Ford", "BMW", "Fiat"]
        	   memset(databytes,0,sizeof(databytes));
        	   for (int j=0;j<current->canMsg.data_length_code;j++)
        	    {
        		   memset(dbyte,0,sizeof(dbyte));
        		   if (j== current->canMsg.data_length_code-1)
        		    sprintf(dbyte,"\"%02X\"",current->canMsg.data[j]);
        		   else
        			sprintf(dbyte,"\"%02X\",",current->canMsg.data[j]);

        		   strcat(databytes,dbyte);
        	    }

        	   memset(dataCAN,0,sizeof(dataCAN));
        	   sprintf(dataCAN,"{\"Date\": \"%02d/%02d/%04d\", \"Time\": \"%02d:%02d:%02d.%03d\", \"Counter\": \"%d\" , \"Length\": \"%d\", \"Data\": [%s] }",
        			   current->recTime.tm_mday, current->recTime.tm_mon, current->recTime.tm_year,
					   current->recTime.tm_hour, current->recTime.tm_min, current->recTime.tm_sec, current->recTime.tm_msec,
					   current->cntMsg,
					   current->canMsg.data_length_code,
					   databytes
        	   );

               esp_mqtt_client_publish(client, &topicID , &dataCAN, 0, 0, 0);
           }


         }
         else
         {
           printf("Updated:%d  0x%X \n", current->cntMsg,  current->canMsg.identifier);
           // add que updated
           if(MQTT_CONNECTED)
           {
        	   memset(topicID,0,sizeof(topicID));
        	   sprintf(topicID,"/update/%08X",current->canMsg.identifier);

        	   // ["Ford", "BMW", "Fiat"]
        	   memset(databytes,0,sizeof(databytes));
        	   for (int j=0;j<current->canMsg.data_length_code;j++)
        	    {
        		   memset(dbyte,0,sizeof(dbyte));
        		   if (j== current->canMsg.data_length_code-1)
        		    sprintf(dbyte,"\"%02X\"",current->canMsg.data[j]);
        		   else
        			sprintf(dbyte,"\"%02X\",",current->canMsg.data[j]);

        		   strcat(databytes,dbyte);
        	    }

        	   memset(dataCAN,0,sizeof(dataCAN));
        	   sprintf(dataCAN,"{\"Date\": \"%02d/%02d/%04d\", \"Time\": \"%02d:%02d:%02d.%03d\", \"Counter\": \"%d\" , \"Length\": \"%d\", \"Data\": [%s] }",
        			   current->recTime.tm_mday, current->recTime.tm_mon, current->recTime.tm_year,
					   current->recTime.tm_hour, current->recTime.tm_min, current->recTime.tm_sec, current->recTime.tm_msec,
					   current->cntMsg,
					   current->canMsg.data_length_code,
					   databytes
        	   );



               esp_mqtt_client_publish(client, &topicID , &dataCAN, 0, 0, 0);
           }

         }

         return;
      }

      current = current->next;
      pos++;


   }

   printf("Update: %X does not exist in the list\n", CanID);
}


//==========================================================================================
// remove_data
//==========================================================================================
void remove_data(uint32_t data) {
   int pos = 0;



   if(head==NULL) {
      printf("Remove Data: Linked List not initialized\n");
      return;
   }

   if(head->canMsg.identifier == data) {
      if(head->next != NULL) {
         head = head->next;
         return;
      } else {
         head = NULL;
         printf("List is empty now");
         return;
      }
   } else if(head->canMsg.identifier != data && head->next == NULL) {
      printf("Remove Data:  %X not found in the list\n", data);
      return;
   }

   //prev = head;
   current = head;

   while(current->next != NULL && current->canMsg.identifier != data) {
      prev = current;
      current = current->next;
   }

   if(current->canMsg.identifier == data) {
      prev->next = prev->next->next;
      free(current);
   } else
      printf("Remove Data: %X not found in the list.\n", data);


}

//==========================================================================================
// test_linklist
//==========================================================================================
int test_can_linklist() {

	rx_message_t test_msg;


	test_msg.can.data_length_code = 8;
	test_msg.can.data[0] = 0xA0;
	test_msg.can.data[1] = 0xA1;
	test_msg.can.data[2] = 0xA2;
	test_msg.can.data[3] = 0xA3;
	test_msg.can.data[4] = 0xB0;
	test_msg.can.data[5] = 0xB1;
	test_msg.can.data[6] = 0xB2;
	test_msg.can.data[7] = 0xB3;


	printf("[APP] Free Mem: %d \n\n", (int) esp_get_free_heap_size());
	test_msg.can.identifier = 0x1234;
    insert(test_msg);

	test_msg.can.identifier = 0x5678;
    insert(test_msg);

	test_msg.can.identifier = 0xABCD;
    insert(test_msg);

    printf("[APP] Free Mem: %d \n\n", (int) esp_get_free_heap_size());

	test_msg.can.identifier = 0x1234;
    insert(test_msg);

	test_msg.can.identifier = 0x5678;
    insert(test_msg);

	test_msg.can.identifier = 0xABCD;
    insert(test_msg);

    printf("[APP] Free Mem: %d \n\n", (int) esp_get_free_heap_size());



   size_of_list();

   find_data(0x5678);
   find_data(0x5679);

     display(1);
 	test_msg.can.data[0] = 0xEF;
     update_data(0x5678, test_msg);
     display(1);

   printf("Before Removal : ");
   display(1);
   remove_data(0x5678);
   printf("After Removal  : ");
   display(0);

   remove_data(0xABCD);
   printf("After Removal  : ");
   display(0);

   remove_data(0x12c4);
   printf("After Removal  : ");
   display(0);



   return 0;
}



/* --------------------------- Tasks and Functions -------------------------- */

static void can_buffer_task(void *arg)
{
	rx_message_t RXPackage;

	countList = 0;


    while (1)
     {

    	if (can_rx_queue != NULL)
    	 {

    		xQueueReceive(can_rx_queue, &RXPackage, portMAX_DELAY);

    		if (DoPreFilter(RXPackage.can.identifier))
    		{
				if ( find_data(RXPackage.can.identifier) != -1)
				{
					update_data(RXPackage.can.identifier, RXPackage);
				}
				else
				{
					insert(RXPackage);
				}
    		}

    	 }

    	//vTaskMissedYield();
    	 vTaskDelay(pdMS_TO_TICKS(10));
     }

}


/* --------------------------- Tasks and Functions -------------------------- */

static void periodik_task(void *arg)
{

	time_t now;
	char strftime_buf[64];
	struct tm timeinfo;

    while (1)
     {
    	time(&now);
    	localtime_r(&now, &timeinfo);
    	strftime(strftime_buf, sizeof(strftime_buf), "%d/%m/%Y - %X", &timeinfo);
    	printf ("The current date/time in istanbul is: %s\n", strftime_buf);

    	 vTaskDelay(pdMS_TO_TICKS(5000));
     }

}



void can_buffer_run(void)
{

  xTaskCreatePinnedToCore(&can_buffer_task, "can_buffer_task", 1024*6, NULL, 8, NULL,0);
  xTaskCreatePinnedToCore(&periodik_task,   "periodicr_task",  1024*4, NULL, 5, NULL,0);

}

