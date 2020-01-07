#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "define_macro_functions.h"
#include "structures.h"
#include "define_macros_test.h"

#include "insert_database_functions.h"
#include "read_database_functions.h"
#include "delete_database_functions.h"
#include "thread_functions.h"

#include "variables_test.h"
#include "read_database_functions_test.h"

/*
-----------------------------------------------------------------------------------------------------------
Functions
-----------------------------------------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------------------------------------
Name: read_database_functions_test
Description: Test the read database functions
Return: The number of passed read database functions test
-----------------------------------------------------------------------------------------------------------
*/

int read_database_functions_test(void)
{   
  // Variables  
  int count;
  int counter;
  pthread_t thread_id;
  struct database_document_fields database_data;
  struct database_multiple_documents_fields database_multiple_documents_fields;

  // define macros
  #define READ_DATABASE_FUNCTIONS_TEST 11
  #define DATABASE_FIELDS_COUNT 7
  #define MESSAGE "{\"message_settings\": \"XCASH_DPOPS_TEST_DATA\"}"
  #define RESET_DATABASE_DATA \
  for (count = 0; count < DATABASE_FIELDS_COUNT; count++) \
  { \
    memset(database_data.item[count],0,strlen(database_data.item[count])); \
    memset(database_data.value[count],0,strlen(database_data.value[count])); \
  } \
  database_data.count = 0;

  #define RESET_DATABASE_DATA_MULTIPLE_DOCUMENTS_FIELDS \
  for (count = 0; count < 2; count++) \
  { \
    for (counter = 0; counter < DATABASE_FIELDS_COUNT; counter++) \
    { \
      memset(database_multiple_documents_fields.item[count][counter],0,strlen(database_multiple_documents_fields.item[count][counter])); \
      memset(database_multiple_documents_fields.value[count][counter],0,strlen(database_multiple_documents_fields.value[count][counter])); \
    } \
  } \
  database_multiple_documents_fields.document_count = 0; \
  database_multiple_documents_fields.database_fields_count = 0;
  

  // write the start test message
  color_print(TEST_OUTLINE,"blue");
  fprintf(stderr,"\033[1;34mread database functions test - Total test: %d\033[0m\n",READ_DATABASE_FUNCTIONS_TEST);
  color_print(TEST_OUTLINE,"blue");
  fprintf(stderr,"\n");

  delete_collection_from_database(database_name,DATABASE_COLLECTION_TEST,0);

  // reset the variables
  memset(data_test,0,sizeof(data_test));
  count_test = 0;

  // initialize the database_document_fields struct 
  for (count = 0; count < DATABASE_FIELDS_COUNT; count++)
  {
    database_data.item[count] = (char*)calloc(BUFFER_SIZE,sizeof(char));
    database_data.value[count] = (char*)calloc(BUFFER_SIZE,sizeof(char));

    if (database_data.item[count] == NULL || database_data.value[count] == NULL)
    {
      color_print("Could not allocate the memory needed on the heap","red");
      exit(0);
    }
  }
  database_data.count = DATABASE_FIELDS_COUNT;

  // initialize the database_multiple_documents_fields struct 
  for (count = 0; count < 2; count++)
  {
    for (counter = 0; counter < DATABASE_FIELDS_COUNT; counter++)
    {
      database_multiple_documents_fields.item[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
      database_multiple_documents_fields.value[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));

      if (database_multiple_documents_fields.item[count][counter] == NULL || database_multiple_documents_fields.value[count][counter] == NULL)
      {
        color_print("Could not allocate the memory needed on the heap","red");
        exit(0);
      }
    }
  }
  database_multiple_documents_fields.document_count = 0;
  database_multiple_documents_fields.database_fields_count = DATABASE_FIELDS_COUNT;

  // read a document in the collection
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,MESSAGE,0);
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  if (read_document_from_collection(database_name,DATABASE_COLLECTION_TEST,MESSAGE,data_test,0) == 1)
  {
    if (strstr(data_test,"XCASH_DPOPS_TEST_DATA") != NULL)
    {
      color_print("PASSED! Test for read_document_from_collection","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_document_from_collection","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for read_document_from_collection","red");
  }

  // read a document in the collection on a separate thread
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  struct read_document_from_collection_thread_parameters read_document_from_collection_thread_parameters = {database_name,DATABASE_COLLECTION_TEST,MESSAGE,data_test};
  pthread_create(&thread_id, NULL, &read_document_from_collection_thread,(void *)&read_document_from_collection_thread_parameters);
  if (thread_settings(thread_id) == 1)
  {
    if (strstr(data_test,"XCASH_DPOPS_TEST_DATA") != NULL)
    {
      color_print("PASSED! Test for read_document_from_collection_thread","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_document_from_collection_thread","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for read_document_from_collection_thread","red");
  }

  // read a document in the collection and parse a field
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  if (read_document_field_from_collection(database_name,DATABASE_COLLECTION_TEST,MESSAGE,"message_settings",data_test,0) == 1)
  {
    if (strncmp(data_test,"XCASH_DPOPS_TEST_DATA",BUFFER_SIZE) == 0)
    {
      color_print("PASSED! Test for read_document_field_from_collection","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_document_field_from_collection","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for read_document_field_from_collection","red");
  }

  // read a document in the collection and parse a field on a separate thread
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  struct read_document_field_from_collection_thread_parameters read_document_field_from_collection_thread_parameters = {database_name,DATABASE_COLLECTION_TEST,MESSAGE,"message_settings",data_test};
  pthread_create(&thread_id, NULL, &read_document_field_from_collection_thread,(void *)&read_document_field_from_collection_thread_parameters);
  if (thread_settings(thread_id) == 1)
  {
    if (strncmp(data_test,"XCASH_DPOPS_TEST_DATA",BUFFER_SIZE) == 0)
    {
      color_print("PASSED! Test for read_document_field_from_collection_thread","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_document_field_from_collection_thread","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for read_document_field_from_collection_thread","red");
  }

  // read a document in the collection and parse all fields
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  delete_collection_from_database(database_name,DATABASE_COLLECTION_TEST,0);
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,DATABASE_COLLECTION_STATISTICS_TEST_DATA,0);

  if (read_document_all_fields_from_collection(database_name,DATABASE_COLLECTION_TEST,"{\"username\":\"XCASH\"}",&database_data,0) == 1)
  {
    if (strncmp(database_data.item[0],"username",BUFFER_SIZE) == 0 && strncmp(database_data.value[0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_data.value[2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_data.value[4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_data.value[6],"15",BUFFER_SIZE) == 0)
    {
      color_print("PASSED! Test for read_document_all_fields_from_collection","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_document_all_fields_from_collection","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for read_document_all_fields_from_collection","red");
  }

  // read a document in the collection and parse all fields on a separate thread
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  delete_collection_from_database(database_name,DATABASE_COLLECTION_TEST,0);
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,DATABASE_COLLECTION_STATISTICS_TEST_DATA,0);
  
  // reset the database_document_fields struct 
  RESET_DATABASE_DATA;

  struct read_document_all_fields_from_collection_thread_parameters read_document_all_fields_from_collection_thread_parameters = {database_name,DATABASE_COLLECTION_TEST,"{\"username\":\"XCASH\"}",&database_data};
  pthread_create(&thread_id, NULL, &read_document_all_fields_from_collection_thread,(void *)&read_document_all_fields_from_collection_thread_parameters);
  if (thread_settings(thread_id) == 1)
  {
    if (strncmp(database_data.item[0],"username",BUFFER_SIZE) == 0 && strncmp(database_data.value[0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_data.value[2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_data.value[4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_data.value[6],"15",BUFFER_SIZE) == 0)
    {
      color_print("PASSED! Test for read_document_all_fields_from_collection_thread","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_document_all_fields_from_collection_thread","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for read_document_all_fields_from_collection_thread","red");
  }
  
  // read multiple documents in the collection and parse all fields
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  delete_collection_from_database(database_name,DATABASE_COLLECTION_TEST,0);
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,DATABASE_COLLECTION_STATISTICS_TEST_DATA,0);
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,DATABASE_COLLECTION_STATISTICS_TEST_DATA,0);

  if (read_multiple_documents_all_fields_from_collection(database_name,DATABASE_COLLECTION_TEST,"",&database_multiple_documents_fields,1,2,0,"",0) == 1)
  {
    if (strncmp(database_multiple_documents_fields.item[0][0],"username",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][6],"15",BUFFER_SIZE) == 0 &&        
        strncmp(database_multiple_documents_fields.item[1][0],"username",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][6],"15",BUFFER_SIZE) == 0)
    {
      color_print("PASSED! Test for read_multiple_documents_all_fields_from_collection","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_multiple_documents_all_fields_from_collection","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for read_multiple_documents_all_fields_from_collection","red");
  }

  // read multiple documents in the collection using the sort document option and parse all fields
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  delete_collection_from_database(database_name,DATABASE_COLLECTION_TEST,0);
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,DATABASE_COLLECTION_STATISTICS_TEST_DATA,0);
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,DATABASE_COLLECTION_STATISTICS_TEST_DATA,0);

  // reset the database_multiple_documents_fields struct 
  RESET_DATABASE_DATA_MULTIPLE_DOCUMENTS_FIELDS;

  if (read_multiple_documents_all_fields_from_collection(database_name,DATABASE_COLLECTION_TEST,"",&database_multiple_documents_fields,1,2,1,"data",0) == 1)
  {
     if (strncmp(database_multiple_documents_fields.item[0][0],"username",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][6],"15",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][0],"username",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][6],"15",BUFFER_SIZE) == 0)
    {
      color_print("PASSED! Test for read_multiple_documents_all_fields_from_collection","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_multiple_documents_all_fields_from_collection","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for read_multiple_documents_all_fields_from_collection","red");
  }

  // read multiple documents in the collection and parse all fields on a separate thread
  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  delete_collection_from_database(database_name,DATABASE_COLLECTION_TEST,0);
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,DATABASE_COLLECTION_STATISTICS_TEST_DATA,0);
  insert_document_into_collection_json(database_name,DATABASE_COLLECTION_TEST,DATABASE_COLLECTION_STATISTICS_TEST_DATA,0);

  // reset the database_multiple_documents_fields struct 
  RESET_DATABASE_DATA_MULTIPLE_DOCUMENTS_FIELDS;

  struct read_multiple_documents_all_fields_from_collection_thread_parameters read_multiple_documents_all_fields_from_collection_thread_parameters = {database_name,DATABASE_COLLECTION_TEST,"",&database_multiple_documents_fields,1,2,0,""};
  pthread_create(&thread_id, NULL, &read_multiple_documents_all_fields_from_collection_thread,(void *)&read_multiple_documents_all_fields_from_collection_thread_parameters);
  if (thread_settings(thread_id) == 1)
  { 
     if (strncmp(database_multiple_documents_fields.item[0][0],"username",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[0][6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][6],"15",BUFFER_SIZE) == 0 &&        
        strncmp(database_multiple_documents_fields.item[1][0],"username",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_multiple_documents_fields.item[1][6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][6],"15",BUFFER_SIZE) == 0)
    {
      color_print("PASSED! Test for read_multiple_documents_all_fields_from_collection_thread","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for read_multiple_documents_all_fields_from_collection_thread","red");
    } 
  }
  else
  {
    color_print("FAILED! Test for read_multiple_documents_all_fields_from_collection_thread","red");
  }

  // database_document_parse_json_data

  // reset the database_document_fields struct 
  RESET_DATABASE_DATA;

  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  memcpy(data_test,DATABASE_COLLECTION_STATISTICS_PARSE_JSON_TEST_DATA,sizeof(DATABASE_COLLECTION_STATISTICS_PARSE_JSON_TEST_DATA)-1);
  database_data.count = 7;

  if (database_document_parse_json_data(data_test,&database_data) == 1)
  {
    if (strncmp(database_data.item[0],"username",BUFFER_SIZE) == 0 && strncmp(database_data.value[0],"XCASH",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_data.value[2],"5",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_data.value[4],"10",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_data.value[5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
        strncmp(database_data.item[6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_data.value[6],"15",BUFFER_SIZE) == 0)
    {
      color_print("PASSED! Test for database_document_parse_json_data","green");
      count_test++;
    }
    else
    {
      color_print("FAILED! Test for database_document_parse_json_data","red");
    }    
  }
  else
  {
    color_print("FAILED! Test for database_document_parse_json_data","red");
  }
  
  // reset the database_document_fields struct 
  RESET_DATABASE_DATA_MULTIPLE_DOCUMENTS_FIELDS;

  memset(data_test,0,strnlen(data_test,BUFFER_SIZE));
  memcpy(data_test,DATABASE_COLLECTION_STATISTICS_PARSE_JSON_TEST_DATA,sizeof(DATABASE_COLLECTION_STATISTICS_PARSE_JSON_TEST_DATA)-1);
  database_multiple_documents_fields.document_count = 0;
  database_multiple_documents_fields.database_fields_count = 7;

  if (database_multiple_documents_parse_json_data(data_test,&database_multiple_documents_fields,0) == 1)
  {
    database_multiple_documents_fields.document_count = 1;
    database_multiple_documents_fields.database_fields_count = 7;
    if (database_multiple_documents_parse_json_data(data_test,&database_multiple_documents_fields,1) == 1)
    {
      if (strncmp(database_multiple_documents_fields.item[0][0],"username",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][0],"XCASH",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[0][1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[0][2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][2],"5",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[0][3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[0][4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][4],"10",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[0][5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[0][6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[0][6],"15",BUFFER_SIZE) == 0 &&        
          strncmp(database_multiple_documents_fields.item[1][0],"username",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][0],"XCASH",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[1][1],"most_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][1],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[1][2],"most_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][2],"5",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[1][3],"best_block_verifier_online_percentage_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][3],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[1][4],"best_block_verifier_online_percentage",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][4],"10",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[1][5],"most_block_producer_total_rounds_delegate_name",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][5],"DELEGATE_NAME",BUFFER_SIZE) == 0 &&
          strncmp(database_multiple_documents_fields.item[1][6],"most_block_producer_total_rounds",BUFFER_SIZE) == 0 && strncmp(database_multiple_documents_fields.value[1][6],"15",BUFFER_SIZE) == 0)
      {
        color_print("PASSED! Test for database_multiple_documents_parse_json_data","green");
        count_test++;
      }
      else
      {
        color_print("FAILED! Test for database_multiple_documents_parse_json_data","red");
      }    
    }
    else
    {
      color_print("FAILED! Test for database_multiple_documents_parse_json_data","red");
    }
  }
  else
  {
    color_print("FAILED! Test for database_multiple_documents_parse_json_data","red");
  }

  for (count = 0; count < DATABASE_FIELDS_COUNT; count++)
  {
    pointer_reset(database_data.item[count]);
    pointer_reset(database_data.value[count]);
  }

  for (count = 0; count < 2; count++)
  {
    for (counter = 0; counter < DATABASE_FIELDS_COUNT; counter++)
    {
      pointer_reset(database_multiple_documents_fields.item[count][counter]);
      pointer_reset(database_multiple_documents_fields.value[count][counter]);
    }
  }

  // write the end test message
  if (count_test == READ_DATABASE_FUNCTIONS_TEST)
  {
    fprintf(stderr,"\n");
    color_print(TEST_OUTLINE,"green");
    fprintf(stderr,"\033[1;32mread database functions test - Passed test: %d, Failed test: 0\033[0m\n",READ_DATABASE_FUNCTIONS_TEST);
    color_print(TEST_OUTLINE,"green");
    fprintf(stderr,"\n\n");
  }
  else
  {
    fprintf(stderr,"\n");
    color_print(TEST_OUTLINE,"red");
    fprintf(stderr,"\033[1;31mread database functions test - Passed test: %d, Failed test: %d\033[0m\n",count_test,READ_DATABASE_FUNCTIONS_TEST-count_test);
    color_print(TEST_OUTLINE,"red");
    fprintf(stderr,"\n\n");
  } 
  return count_test;

  #undef READ_DATABASE_FUNCTIONS_TEST
  #undef DATABASE_FIELDS_COUNT
  #undef MESSAGE
  #undef RESET_DATABASE_DATA
  #undef RESET_DATABASE_DATA_MULTIPLE_DOCUMENTS_FIELDS
}