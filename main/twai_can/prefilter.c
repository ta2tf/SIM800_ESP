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
struct pre_filter_item * pre_head = NULL;
struct pre_filter_item * pre_current = NULL;
struct pre_filter_item * pre_prev = NULL;



/* ---------------------------- Linked List  Functions -------------------------------- */

//==========================================================================================
// display  display the list
//==========================================================================================
void ListFilters(void) {
   struct pre_filter_item *ptr = pre_head;

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
void size_of_preFilter() {
   int size = 0;

   if( pre_head == NULL) {
      printf("List size : %d \r\n", size);
      return;
   }

   pre_current = pre_head;
   size = 1;
   while( pre_current->next != NULL) {
	    pre_current =  pre_current->next;
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
   if (pre_head == NULL)
   {
     prefilter->filter.pre_filter_id = 1;
   }
   else
   {
	   prefilter->filter.pre_filter_id = pre_head->filter.pre_filter_id+1;
   }

   prefilter->filter.pre_filter_status = status_active;
   prefilter->filter.pre_filter_parm = data.pre_filter_parm;
   prefilter->filter.pre_filter_mask = data.pre_filter_mask;
   prefilter->filter.pre_filter_type = data.pre_filter_type;
   prefilter->filter.num_of_filtered_msg = 0;



   //point it to old first node
   prefilter->next =  pre_head;

   //point first to new first node
   pre_head = prefilter;

   printf("Insert:%d  inserted \n" , prefilter->filter.pre_filter_id);

}



//==========================================================================================
// find_data
//==========================================================================================
int find_filter(uint8_t item) {
   int pos = 0;

   if(pre_head == NULL) {
    //  printf("Find Data: Linked List not initialized");
      return -1;
   }

   pre_current = pre_head;

   while(pre_current != NULL) {

	   if(pre_current->filter.pre_filter_id == item) {
         printf("Find Data: 0x%X found at position %d\n", item, pos);
         return pos;
      }

      pre_current = pre_current->next;
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


   if(pre_head == NULL) {
  //    printf("Update: Linked List not initialized");
      return;
   }

   pre_current = pre_head;

   while(pre_current != NULL) {



      if(pre_current->filter.pre_filter_id == filterID) {

    	 pre_current->filter.pre_filter_status = new.pre_filter_status;
    	 pre_current->filter.pre_filter_parm = new.pre_filter_parm;
    	 pre_current->filter.pre_filter_type = new.pre_filter_type;
    	 pre_current->filter.num_of_filtered_msg = 0;

      }

      pre_current = pre_current->next;
      pos++;

   }

   printf("Update: %X does not exist in the list\n", filterID);
}



//==========================================================================================
// remove_data
//==========================================================================================
void remove_filter(uint8_t FilterID) {
   int pos = 0;



   if(pre_head == NULL) {
      printf("Remove Data: Linked List not initialized\n");
      return;
   }

   if(pre_head->filter.pre_filter_id == FilterID) {
      if(pre_head->next != NULL) {
         pre_head = pre_head->next;
         return;
      } else {
         pre_head = NULL;
         printf("List is empty now");
         return;
      }
   } else if( (pre_head->filter.pre_filter_id != FilterID) && (pre_head->next == NULL) ) {
      printf("Remove Data:  %X not found in the list\n", FilterID);
      return;
   }

   //pre_prev = pre_head;
   pre_current = pre_head;

   while(pre_current->next != NULL && pre_current->filter.pre_filter_id != FilterID) {
      pre_prev = pre_current;
      pre_current = pre_current->next;
   }

   if(pre_current->filter.pre_filter_id == FilterID) {
      pre_prev->next = pre_prev->next->next;
      free(pre_current);
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

//	 printf("Start Filter: \r\n");

	   if( pre_head == NULL) {
	      return 0;
	   }

	   pre_current = pre_head;

	   while( pre_current->next != NULL) {

		if (pre_current->filter.pre_filter_status == status_active)
		{
		   // Black List
		   if (pre_current->filter.pre_filter_type == pre_filter_id_blacklist)
		   {
		     if (pre_current->filter.pre_filter_parm == ID)
		      {
			    pre_current->filter.num_of_filtered_msg++;
			     printf("Black List [%d,%d]\r\n",  pre_current->filter.pre_filter_id,pre_current->filter.num_of_filtered_msg );
			    return 0;
		      }
		   } else
		   //black Mask
		   if (pre_current->filter.pre_filter_type == pre_filter_id_blackmask)
		   {
			   tmp_parm = pre_current->filter.pre_filter_parm & pre_current->filter.pre_filter_mask;
			     tmp_id = ID & pre_current->filter.pre_filter_mask;

			 if (  tmp_parm == tmp_id)
			  {
				pre_current->filter.num_of_filtered_msg++;
				 printf("White Mask [%d,%d]\r\n",  pre_current->filter.pre_filter_id,pre_current->filter.num_of_filtered_msg );
				return 0;
			  }
		   } else
		   //white List
		   if (pre_current->filter.pre_filter_type == pre_filter_id_whitelist)
		   {


			 if (pre_current->filter.pre_filter_parm == ID)
			  {
				pre_current->filter.num_of_filtered_msg++;
				 printf("White List:     0x%08X [%d,%d]\r\n",ID,  pre_current->filter.pre_filter_id,pre_current->filter.num_of_filtered_msg );
				return 1;
			  }
		   } else
		   //white Mask
		   if (pre_current->filter.pre_filter_type == pre_filter_id_whitemask)
		   {
			   tmp_parm = pre_current->filter.pre_filter_parm & pre_current->filter.pre_filter_mask;
			     tmp_id = ID & pre_current->filter.pre_filter_mask;

			 if (  tmp_parm == tmp_id)
			  {
				pre_current->filter.num_of_filtered_msg++;
				 printf("White Mask [%d,%d]\r\n",  pre_current->filter.pre_filter_id,pre_current->filter.num_of_filtered_msg );
				return 1;
			  }
		   }

		}
		    pre_current =  pre_current->next;
	   }

	   printf("NOT Filtered: 0x%X \r\n",ID);
	   return 0;

}

//==========================================================================================
// test_linklist
//==========================================================================================
int test_can_prefilter(void)
{

	filter_t test_filter;

	printf("Start pre filter test\n");




    test_filter.pre_filter_status = status_active;
    test_filter.pre_filter_parm   = 0x18FFFFAA;
    test_filter.pre_filter_type   = pre_filter_id_whitelist;

    insert_filter(test_filter);
    test_filter.pre_filter_mask   = 0x000000FF;
	test_filter.pre_filter_status = status_active;
	test_filter.pre_filter_parm   = 0x0CF0E431;

	test_filter.pre_filter_type   = pre_filter_id_whitemask;

    insert_filter(test_filter);





//
//	test_filter.pre_filter_status = status_active;
//	test_filter.pre_filter_mask   = 0x000000FF;
//	test_filter.pre_filter_parm   = 0x000000AC;
//	test_filter.pre_filter_type   = pre_filter_id_whitemask;
//
//    insert_filter(test_filter);
//
//
//    test_filter.pre_filter_status = status_active;
//    test_filter.pre_filter_parm   = 0x18FFFFBB;
//    test_filter.pre_filter_type   = pre_filter_id_whitelist;
//
//    insert_filter(test_filter);





   size_of_preFilter();
   ListFilters();
//    DoPreFilter(0x0CF00431);
//   DoPreFilter(0x18FFFFAC);
//   DoPreFilter(0x18FF11AC);
//   DoPreFilter(0x18FF10AC);


   ListFilters();

   printf("End pre filter test\n");

   return 0;
}



