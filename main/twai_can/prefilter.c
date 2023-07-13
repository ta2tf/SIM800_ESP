/*
 * prefilter.c
 *
 *  Created on: 11 Jul 2023
 *      Author: MertechArge014
 */



#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "esp_log.h"

enum filter_sta_e
{ status_not_active,
  status_active
};


enum pre_filter_e
{ pre_filter_not_set,
  pre_filter_id_blacklist,
  pre_filter_id_blackmask,
  pre_filter_id_whitelist,
  pre_filter_id_whitemask
};



typedef struct filter {
	uint8_t            pre_filter_id;
	enum filter_sta_e  pre_filter_status;
	enum pre_filter_e  pre_filter_type;
	uint32_t           pre_filter_parm;
	uint32_t           pre_filter_mask;
	uint32_t           num_of_filtered_msg;
}filter_t;




typedef struct pre_filter_item {
	struct filter           filter;
    struct pre_filter_item *next;
}pre_filter_item_t;




/* --------------------- Linked List  Definitions and  variables ------------------ */
struct pre_filter_item * head = NULL;
struct pre_filter_item * current = NULL;
struct pre_filter_item * prev = NULL;



/* ---------------------------- Linked List  Functions -------------------------------- */

//==========================================================================================
// display  display the list
//==========================================================================================
void ListFilters(void) {
   struct pre_filter_item *ptr = head;

   printf("\nList Filters \n");
   //start from the beginning
   while(ptr != NULL) {

	  printf("ID  : %d   \n", ptr->filter.pre_filter_id);
	  printf("Sta : %d   \n", ptr->filter.pre_filter_status);
      printf("Num : %d   \n", ptr->filter.num_of_filtered_msg);
      printf("Type: %d   \n", ptr->filter.pre_filter_type);
      printf("Parm: 0x%X \n\n", ptr->filter.pre_filter_parm);

      ptr = ptr->next;
   }

   printf("\n");
}





//==========================================================================================
// size_of_list
//==========================================================================================
void size_of_list() {
   int size = 0;

   if( head == NULL) {
      printf("List size : %d \r\n", size);
      return;
   }

   current = head;
   size = 1;
   while( current->next != NULL) {
	    current =  current->next;
      size++;
   }
   printf("List size : %d \r\n", size);
}


//==========================================================================================
// insert    insert link at the first location
//==========================================================================================
void insert_filter(filter_t data) {
   //create a link
   struct pre_filter_item *prefilter = (struct pre_filter_item*) malloc(sizeof(struct pre_filter_item));


   //link->key = key;
   if (head == NULL)
   {
     prefilter->filter.pre_filter_id = 1;
   }
   else
   {
	   prefilter->filter.pre_filter_id = head->filter.pre_filter_id+1;
   }

   prefilter->filter.pre_filter_status = status_active;
   prefilter->filter.pre_filter_parm = data.pre_filter_parm;
   prefilter->filter.pre_filter_mask = data.pre_filter_mask;
   prefilter->filter.pre_filter_type = data.pre_filter_type;
   prefilter->filter.num_of_filtered_msg = 0;



   //point it to old first node
   prefilter->next =  head;

   //point first to new first node
   head = prefilter;

   printf("Insert:%d  inserted \n" , prefilter->filter.pre_filter_id);

}



//==========================================================================================
// find_data
//==========================================================================================
int find_filter(uint8_t item) {
   int pos = 0;

   if(head == NULL) {
    //  printf("Find Data: Linked List not initialized");
      return -1;
   }

   current = head;

   while(current != NULL) {

	   if(current->filter.pre_filter_id == item) {
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
void update_filter(uint8_t filterID, filter_t new) {
   int pos = 0;

   char topicID[32];
   char dataCAN[300];
   char databytes[64];
   char dbyte[3];


   if(head == NULL) {
  //    printf("Update: Linked List not initialized");
      return;
   }

   current = head;

   while(current != NULL) {



      if(current->filter.pre_filter_id == filterID) {

    	 current->filter.pre_filter_status = new.pre_filter_status;
    	 current->filter.pre_filter_parm = new.pre_filter_parm;
    	 current->filter.pre_filter_type = new.pre_filter_type;
    	 current->filter.num_of_filtered_msg = 0;

      }

      current = current->next;
      pos++;

   }

   printf("Update: %X does not exist in the list\n", filterID);
}



//==========================================================================================
// remove_data
//==========================================================================================
void remove_data(uint8_t FilterID) {
   int pos = 0;



   if(head == NULL) {
      printf("Remove Data: Linked List not initialized\n");
      return;
   }

   if(head->filter.pre_filter_id == FilterID) {
      if(head->next != NULL) {
         head = head->next;
         return;
      } else {
         head = NULL;
         printf("List is empty now");
         return;
      }
   } else if( (head->filter.pre_filter_id != FilterID) && (head->next == NULL) ) {
      printf("Remove Data:  %X not found in the list\n", FilterID);
      return;
   }

   //prev = head;
   current = head;

   while(current->next != NULL && current->filter.pre_filter_id != FilterID) {
      prev = current;
      current = current->next;
   }

   if(current->filter.pre_filter_id == FilterID) {
      prev->next = prev->next->next;
      free(current);
   } else
      printf("Remove Data: %X not found in the list.\n", FilterID);


}

//==========================================================================================
// do pre filter
//==========================================================================================
int DoPreFilter(uint32_t ID)
{
	   uint32_t tmp_parm;
	   uint32_t tmp_id;

	 printf("Start Filter: \r\n");

	   if( head == NULL) {
	      return 0;
	   }

	   current = head;

	   while( current->next != NULL) {

		if (current->filter.pre_filter_status == status_active)
		{
		   // Black List
		   if (current->filter.pre_filter_type == pre_filter_id_blacklist)
		   {
		     if (current->filter.pre_filter_parm == ID)
		      {
			    current->filter.num_of_filtered_msg++;
			     printf("return Black List [%d]\r\n",  current->filter.pre_filter_id);
			    return 0;
		      }
		   } else
		   //black Mask
		   if (current->filter.pre_filter_type == pre_filter_id_blackmask)
		   {
			 if (current->filter.pre_filter_parm & ID)
			  {
				current->filter.num_of_filtered_msg++;
				 printf("return Black Mask [%d]\r\n",  current->filter.pre_filter_id);
				return 0;
			  }
		   } else
		   //white List
		   if (current->filter.pre_filter_type == pre_filter_id_whitelist)
		   {
			 if (current->filter.pre_filter_parm == ID)
			  {
				current->filter.num_of_filtered_msg++;
				 printf("return White List [%d]\r\n",  current->filter.pre_filter_id);
				return 1;
			  }
		   } else
		   //white Mask
		   if (current->filter.pre_filter_type == pre_filter_id_whitemask)
		   {

			   tmp_parm = current->filter.pre_filter_parm & current->filter.pre_filter_mask;
			     tmp_id = ID & current->filter.pre_filter_mask;

			   printf("[%x] %x\r\n",  tmp_parm, tmp_id);

			   printf("[%x] %x\r\n",  current->filter.pre_filter_parm, current->filter.pre_filter_mask);

			 if (  tmp_parm == tmp_id)
			  {
				current->filter.num_of_filtered_msg++;
				 printf("return White Mask [%d]\r\n",  current->filter.pre_filter_id);
				return 1;
			  }
		   }

		}


		    current =  current->next;

	   }

	   printf("End Do Filter without filtered: \r\n");
	   return 1;

}

//==========================================================================================
// test_linklist
//==========================================================================================
int test_can_prefilter(void)
{

	filter_t test_filter;

	printf("Start pre filter test\n");

	test_filter.pre_filter_status = status_active;
	test_filter.pre_filter_parm   = 0x18FFFF00;
	test_filter.pre_filter_type   = pre_filter_id_whitelist;

    insert_filter(test_filter);


    test_filter.pre_filter_status = status_active;
    test_filter.pre_filter_parm   = 0x18FFFFAA;
    test_filter.pre_filter_type   = pre_filter_id_whitelist;

    insert_filter(test_filter);

	test_filter.pre_filter_status = status_active;
	test_filter.pre_filter_mask   = 0x000000FF;
	test_filter.pre_filter_parm   = 0x000000AC;
	test_filter.pre_filter_type   = pre_filter_id_whitemask;

    insert_filter(test_filter);


    test_filter.pre_filter_status = status_active;
    test_filter.pre_filter_parm   = 0x18FFFFBB;
    test_filter.pre_filter_type   = pre_filter_id_whitelist;

    insert_filter(test_filter);




   size_of_list();
   ListFilters();
   DoPreFilter(0x18FFFFAA);
   DoPreFilter(0x18FFFFAC);

   ListFilters();

   printf("End pre filter test\n");

   return 0;
}



