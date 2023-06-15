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
#include "can.h"
#include "time.h"

extern QueueHandle_t can_rx_queue;

enum e_node_status
{
  node_unknown,
  node_new,
  node_updated,
  node_refreshed,
  node_reported
};



struct node {
	twai_message_t CanMsg;
	enum e_node_status NodeStatus;
   struct node *next;
};



/* --------------------- Linked List  Definitions and  variables ------------------ */
struct node *head = NULL;
struct node *current = NULL;
struct node *prev = NULL;


/* ---------------------------- Linked List  Functions -------------------------------- */

//==========================================================================================
// display  display the list
//==========================================================================================
void display(uint8_t detail) {
   struct node *ptr = head;

   printf("\n[head] =>");
   //start from the beginning
   while(ptr != NULL) {
      printf(" 0x%X =>",ptr->CanMsg.identifier);

      if (detail == 1)
      {  printf("\n");
    	   for(int j=0;j< ptr->CanMsg.data_length_code;j++)
    		   printf("CAN Data[%d]: 0x%x \r\n",j,ptr->CanMsg.data[j]);
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
void insert(twai_message_t data) {
   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));



   //link->key = key;
   link->CanMsg.identifier = data.identifier;
   link->CanMsg.data_length_code = data.data_length_code;
   for(int j=0;j< data.data_length_code;j++)
	   link->CanMsg.data[j] = data.data[j];
   link->NodeStatus = node_new;



   //point it to old first node
   link->next = head;

   //point first to new first node
   head = link;

   printf("Insert: %X inserted \n" , data.identifier);

}



//==========================================================================================
// find_data
//==========================================================================================
int find_data(uint32_t item) {
   int pos = 0;

   if(head==NULL) {
      printf("Find Data: Linked List not initialized");
      return -1;
   }

   current = head;

   while(current != NULL) {

	   if(current->CanMsg.identifier == item) {
         printf("Find Data: 0x%X found at position %d\n", item, pos);
         return pos;
      }

      current = current->next;
      pos++;
   }




   printf("Find Data: %X does not exist in the list", item);
   return -1;
}


//==========================================================================================
// update_data
//==========================================================================================
void update_data(uint32_t CanID, twai_message_t new) {
   int pos = 0;


   if(head==NULL) {
      printf("Update: Linked List not initialized");
      return;
   }

   current = head;

   while(current != NULL) {
      if(current->CanMsg.identifier == CanID) {

    	 current->NodeStatus = node_refreshed;

         current->CanMsg.data_length_code = new.data_length_code;
         for(int j=0;j< new.data_length_code;j++)
         {
        	 if (current->CanMsg.data[j] != new.data[j])
        	 {
        	   current->CanMsg.data[j] = new.data[j];
        	   current->NodeStatus = node_updated;
        	 }
         }

         if (current->NodeStatus != node_reported)
        	 printf("\n Update: Over Run Error\n");


         if (current->NodeStatus == node_refreshed)
          printf("Refreshed: 0x%X \n", current->CanMsg.identifier);
         else
          printf("Updated: 0x%X \n", current->CanMsg.identifier);

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

   if(head->CanMsg.identifier == data) {
      if(head->next != NULL) {
         head = head->next;
         return;
      } else {
         head = NULL;
         printf("List is empty now");
         return;
      }
   } else if(head->CanMsg.identifier != data && head->next == NULL) {
      printf("Remove Data:  %X not found in the list\n", data);
      return;
   }

   //prev = head;
   current = head;

   while(current->next != NULL && current->CanMsg.identifier != data) {
      prev = current;
      current = current->next;
   }

   if(current->CanMsg.identifier == data) {
      prev->next = prev->next->next;
      free(current);
   } else
      printf("Remove Data: %X not found in the list.\n", data);


}

//==========================================================================================
// test_linklist
//==========================================================================================
int test_can_linklist() {

	twai_message_t test_msg;


	test_msg.data_length_code = 8;
	test_msg.data[0] = 0xA0;
	test_msg.data[1] = 0xA1;
	test_msg.data[2] = 0xA2;
	test_msg.data[3] = 0xA3;
	test_msg.data[4] = 0xB0;
	test_msg.data[5] = 0xB1;
	test_msg.data[6] = 0xB2;
	test_msg.data[7] = 0xB3;


	printf("[APP] Free Mem: %d \n\n", (int) esp_get_free_heap_size());
	test_msg.identifier = 0x1234;

    insert(test_msg);

	test_msg.identifier = 0x5678;

    insert(test_msg);

	test_msg.identifier = 0xABCD;

    insert(test_msg);

    printf("[APP] Free Mem: %d \n\n", (int) esp_get_free_heap_size());

	test_msg.identifier = 0x1234;

    insert(test_msg);

	test_msg.identifier = 0x5678;

    insert(test_msg);

	test_msg.identifier = 0xABCD;

    insert(test_msg);

    printf("[APP] Free Mem: %d \n\n", (int) esp_get_free_heap_size());



   size_of_list();

   find_data(0x5678);
   find_data(0x5679);

     display(1);
 	test_msg.data[0] = 0xEF;
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
	  twai_message_t CanMsg;

    while (1)
     {

    	if (can_rx_queue != NULL)
    	 {

    		xQueueReceive(can_rx_queue, &CanMsg, portMAX_DELAY);

			if ( find_data(CanMsg.identifier) != -1)
			{
				update_data(CanMsg.identifier, CanMsg);
			}
			else
			{
				insert(CanMsg);
			}


    	 }

    	 vTaskDelay(pdMS_TO_TICKS(1));
     }

}


/* --------------------------- Tasks and Functions -------------------------- */

static void periodicr_task(void *arg)
{
	time_t now;
	char strftime_buf[64];
	struct tm timeinfo;

	time(&now);
	// Set timezone to China Standard Time
	setenv("TZ", "CST-8", 1);
	tzset();



    while (1)
     {
    	localtime_r(&now, &timeinfo);
    	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    	printf("The current date/time in Shanghai is: %s", strftime_buf);

    	 vTaskDelay(pdMS_TO_TICKS(15000));
     }

}



void can_buffer_run(void)
{

  xTaskCreatePinnedToCore(&can_buffer_task, "can_buffer_task", 1024*4, NULL, 5, NULL,0);
  xTaskCreatePinnedToCore(&periodicr_task, "periodicr_task", 1024*4, NULL, 5, NULL,0);

}

