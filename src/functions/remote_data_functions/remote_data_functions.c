#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <mongoc/mongoc.h>
#include <bson/bson.h>

#include "define_macro_functions.h"
#include "define_macros.h"
#include "initialize_and_reset_structs_define_macros.h"
#include "structures.h"
#include "variables.h"
#include "define_macros_test.h"

#include "blockchain_functions.h"
#include "block_verifiers_server_functions.h"
#include "block_verifiers_synchronize_server_functions.h"
#include "block_verifiers_synchronize_functions.h"
#include "database_functions.h"
#include "shared_delegate_website_thread_server_functions.h"
#include "insert_database_functions.h"
#include "read_database_functions.h"
#include "update_database_functions.h"
#include "delete_database_functions.h"
#include "count_database_functions.h"
#include "remote_data_functions.h"
#include "network_daemon_functions.h"
#include "network_functions.h"
#include "network_security_functions.h"
#include "network_wallet_functions.h"
#include "server_functions.h"
#include "string_functions.h"
#include "thread_functions.h"
#include "convert.h"
#include "vrf.h"
#include "crypto_vrf.h"
#include "VRF_functions.h"
#include "sha512EL.h"

/*
-----------------------------------------------------------------------------------------------------------
Functions
-----------------------------------------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------------------------------------
Name: validate_name
Description: Checks if the name is valid with the timestamp and tx_hash
Parameters:
  MESSAGE - The name
Return: 0 if an error has occured or invalid, 1 if valid
-----------------------------------------------------------------------------------------------------------
*/

int validate_name(const char* MESSAGE)
{
  // Variables
  char data[SMALL_BUFFER_SIZE];
  char data2[SMALL_BUFFER_SIZE];
  char data3[SMALL_BUFFER_SIZE];
  int name_settings_length;
  long int registration_length;

  // define macros
  #define DATABASE_COLLECTION "remote_data"

  memset(data,0,sizeof(data));
  memset(data2,0,sizeof(data2));
  memset(data3,0,sizeof(data3));

  // check if its just the name or the name and the settings
  name_settings_length = strstr(MESSAGE,".xcash") != NULL ? 6 : strstr(MESSAGE,".sxcash") != NULL ? 6 : strstr(MESSAGE,".pxcash") != NULL ? 6 : strstr(MESSAGE,".website") != NULL ? 6 : 0;

  // create the message
  memcpy(data,"{\"name\":\"",9);
  memcpy(data+strlen(data),MESSAGE,strlen(MESSAGE)-name_settings_length);
  memcpy(data+strlen(data),"\"}",2);

  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,data,"timestamp",data2) == 1 && read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,data,"tx_hash",data3) == 1)
  {
    // check if the name has a valid registration and a valid tx_hash
    sscanf(data2,"%ld", &registration_length);
    if ((registration_length + REMOTE_DATA_REGISTRATION_LENGTH) < time(NULL) || strlen(data3) != TRANSACTION_HASH_LENGTH)
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
  return 1;

  #undef DATABASE_COLLECTION
}



/*
-----------------------------------------------------------------------------------------------------------
Name: server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_address_settings
Description: Runs the code when the server receives the NODE_TO_NETWORK_DATA_NODES_REMOTE_DATA_GET_ADDRESS_SETTINGS message
Parameters:
  CLIENT_SOCKET - The socket to send data to
  MESSAGE - The message
-----------------------------------------------------------------------------------------------------------
*/

void server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_address_settings(const int CLIENT_SOCKET, const char* MESSAGE)
{
  // Variables
  char public_address[XCASH_WALLET_LENGTH+1];
  char message[SMALL_BUFFER_SIZE];
  int count;
  int count2;
  size_t data_size;

  // define macros
  #define DATABASE_COLLECTION "remote_data"
  #define REMOTE_DATA_GET_ADDRESS_SETTINGS_ERROR \
  if (debug_settings == 1) \
  { \
    memcpy(error_message.function[error_message.total],"server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_address_settings",87); \
    memcpy(error_message.data[error_message.total],"Error: Could not get the address settings",41); \
    error_message.total++; \
  } \
  memset(message,0,strlen(message)); \
  memcpy(message,"Error: Could not get the address settings}",42); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,0,0,""); \
  return;

  #define REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS(settings) \
  memset(message,0,strlen(message)); \
  memcpy(message,settings,strlen(settings)); \
  memcpy(message+strlen(message),"}",1); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,0,0,""); \
  return;
  
  memset(public_address,0,sizeof(public_address));
  memset(message,0,sizeof(message));

  // parse the message
  for (count = 0, count2 = 0; count < REMOTE_DATA_GET_ADDRESS_SETTINGS_PARAMETER_AMOUNT; count++)
  {
    if (count == 1)
    {
      if ((data_size = strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) - count2) != XCASH_WALLET_LENGTH)
      {
        REMOTE_DATA_GET_ADDRESS_SETTINGS_ERROR;
      }
      memcpy(public_address,&MESSAGE[count2],data_size);
    }
    count2 = (int)(strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) + 1);
  }

  // the public address can only be in address, saddress, paddress, saddress_list or paddress_list. If not their its address

  // create the message
  memcpy(message,"{\"saddress\":\"",13);
  memcpy(message+strlen(message),public_address,XCASH_WALLET_LENGTH);
  memcpy(message+strlen(message),"\"}",2);

  if (count_documents_in_collection(remote_data_database_name,DATABASE_COLLECTION,message) > 0)
  {    
    REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("saddress");
  }

  // create the message
  memset(message,0,sizeof(message));
  memcpy(message,"{\"paddress\":\"",13);
  memcpy(message+strlen(message),public_address,XCASH_WALLET_LENGTH);
  memcpy(message+strlen(message),"\"}",2);

  if (count_documents_in_collection(remote_data_database_name,DATABASE_COLLECTION,message) > 0)
  {    
    REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("paddress");
  }

  // create the message
  memset(message,0,sizeof(message));
  memcpy(message,"{\"saddress_list\":{\"$regex\":\"",28);
  memcpy(message+strlen(message),public_address,XCASH_WALLET_LENGTH);
  memcpy(message+strlen(message),"|\"}}",4);

  if (count_documents_in_collection(remote_data_database_name,DATABASE_COLLECTION,message) > 0)
  {    
    REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("saddress");
  }

  // create the message
  memset(message,0,sizeof(message));
  memcpy(message,"{\"paddress_list\":{\"$regex\":\"",28);
  memcpy(message+strlen(message),public_address,XCASH_WALLET_LENGTH);
  memcpy(message+strlen(message),"|\"}}",4);

  if (count_documents_in_collection(remote_data_database_name,DATABASE_COLLECTION,message) > 0)
  {    
    REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("paddress");
  }

  // the public address is either address or not in the database. either way return address
  REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("address");

  #undef DATABASE_COLLECTION
  #undef REMOTE_DATA_GET_ADDRESS_SETTINGS_ERROR
  #undef REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS
}



/*
-----------------------------------------------------------------------------------------------------------
Name: server_receive_data_socket_remote_data_get_address_settings
Description: Runs the code when the server receives /remotedatagetaddresssettings
Parameters:
  CLIENT_SOCKET - The socket to send data to
  DATA - The data
Return: 0 if an error has occured, 1 if successfull
-----------------------------------------------------------------------------------------------------------
*/

int server_receive_data_socket_remote_data_get_address_settings(const int CLIENT_SOCKET, const char* DATA)
{
  // Variables
  char public_address[XCASH_WALLET_LENGTH+1];
  char message[SMALL_BUFFER_SIZE];

  // define macros
  #define DATABASE_COLLECTION "remote_data"
  #define REMOTE_DATA_GET_ADDRESS_SETTINGS_ERROR(settings) \
  if (debug_settings == 1) \
  { \
    memcpy(error_message.function[error_message.total],"server_receive_data_socket_remote_data_get_address_settings",59); \
    memcpy(error_message.data[error_message.total],settings,sizeof(settings)-1); \
    error_message.total++; \
  } \
  memset(message,0,strlen(message)); \
  memcpy(message,"{\"Error\":\"Could not get the address settings\"}",46); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,strlen(message),400,"application/json"); \
  return 0;

  #define REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS(settings) \
  memset(message,0,strlen(message)); \
  memcpy(message,"{\"address_settings\":\"",21); \
  memcpy(message+strlen(message),settings,strlen(settings)); \
  memcpy(message+strlen(message),"\"}",2); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,strlen(message),200,"application/json"); \
  return 1;
  
  memset(public_address,0,sizeof(public_address));
  memset(message,0,sizeof(message));

  // get the parameter1
  memcpy(public_address,&DATA[45],(strnlen(DATA,sizeof(public_address)) - strnlen(strstr(DATA," HTTP/"),sizeof(public_address)))-45);

  // error check
  if (strncmp(public_address,"",BUFFER_SIZE) == 0 || strlen(public_address) != XCASH_WALLET_LENGTH)
  {
    REMOTE_DATA_GET_ADDRESS_SETTINGS_ERROR("Invalid parameters");
  } 

  // the public address can only be in address, saddress, paddress, saddress_list or paddress_list. If not their its address

  // create the message
  memcpy(message,"{\"saddress\":\"",13);
  memcpy(message+strlen(message),public_address,XCASH_WALLET_LENGTH);
  memcpy(message+strlen(message),"\"}",2);

  if (count_documents_in_collection(remote_data_database_name,DATABASE_COLLECTION,message) > 0)
  {    
    REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("saddress");
  }

  // create the message
  memset(message,0,sizeof(message));
  memcpy(message,"{\"paddress\":\"",13);
  memcpy(message+strlen(message),public_address,XCASH_WALLET_LENGTH);
  memcpy(message+strlen(message),"\"}",2);

  if (count_documents_in_collection(remote_data_database_name,DATABASE_COLLECTION,message) > 0)
  {    
    REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("paddress");
  }

  // create the message
  memset(message,0,sizeof(message));
  memcpy(message,"{\"saddress_list\":{\"$regex\":\"",28);
  memcpy(message+strlen(message),public_address,XCASH_WALLET_LENGTH);
  memcpy(message+strlen(message),"|\"}}",4);

  if (count_documents_in_collection(remote_data_database_name,DATABASE_COLLECTION,message) > 0)
  {    
    REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("saddress");
  }

  // create the message
  memset(message,0,sizeof(message));
  memcpy(message,"{\"paddress_list\":{\"$regex\":\"",28);
  memcpy(message+strlen(message),public_address,XCASH_WALLET_LENGTH);
  memcpy(message+strlen(message),"|\"}}",4);

  if (count_documents_in_collection(remote_data_database_name,DATABASE_COLLECTION,message) > 0)
  {    
    REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("paddress");
  }

  // the public address is either address or not in the database. either way return address
  REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS("address");

  #undef DATABASE_COLLECTION
  #undef REMOTE_DATA_GET_ADDRESS_SETTINGS_ERROR
  #undef REMOTE_DATA_GET_ADDRESS_SETTINGS_STATUS
}



/*
-----------------------------------------------------------------------------------------------------------
Name: server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_address_from_name
Description: Runs the code when the server receives the NODE_TO_NETWORK_DATA_NODES_REMOTE_DATA_GET_ADDRESS_FROM_NAME message
Parameters:
  CLIENT_SOCKET - The socket to send data to
  MESSAGE - The message
-----------------------------------------------------------------------------------------------------------
*/

void server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_address_from_name(const int CLIENT_SOCKET, const char* MESSAGE)
{
  // Variables
  char name[REMOTE_DATA_NAME_MAXIMUM_LENGTH+1];
  char message[BUFFER_SIZE];
  char data2[BUFFER_SIZE];
  int count;
  int count2;
  int name_settings_length;
  size_t data_size;

  // define macros
  #define DATABASE_COLLECTION "remote_data"
  #define REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR \
  if (debug_settings == 1) \
  { \
    memcpy(error_message.function[error_message.total],"server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_address_from_name",88); \
    memcpy(error_message.data[error_message.total],"Error: Could not get the address settings",41); \
    error_message.total++; \
  } \
  memset(message,0,strlen(message)); \
  memcpy(message,"Error: Could not get the address settings}",42); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,0,0,""); \
  return;

  #define REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(settings) \
  memset(message,0,strlen(message)); \
  memcpy(message,settings,strlen(settings)); \
  memcpy(message+strlen(message),"}",1); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,0,0,""); \
  return;
  
  memset(name,0,sizeof(name));
  memset(data2,0,sizeof(data2));
  memset(message,0,sizeof(message));

  // parse the message
  for (count = 0, count2 = 0; count < REMOTE_DATA_GET_ADDRESS_FROM_NAME_PARAMETER_AMOUNT; count++)
  {
    if (count == 1)
    {
      if ((data_size = strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) - count2) > REMOTE_DATA_NAME_MAXIMUM_LENGTH)
      {
        REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR;
      }
      memcpy(name,&MESSAGE[count2],data_size);
    }
    count2 = (int)(strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) + 1);
  }

  // validate the name settings
  name_settings_length = strstr(MESSAGE,".xcash") != NULL ? 6 : strstr(MESSAGE,".sxcash") != NULL ? 6 : strstr(MESSAGE,".pxcash") != NULL ? 6 : strstr(MESSAGE,".website") != NULL ? 6 : 0;
  if (name_settings_length == 0)
  {
    REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR;
  }
  
  // check if the tx_hash is empty or the name is expired
  if (validate_name(data2) == 0)
  {
    REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR;
  }

  // create the message
  memcpy(message,"{\"name\":\"",9);
  memcpy(message+strlen(message),name,strlen(name)-name_settings_length);
  memcpy(message+strlen(message),"\"}",2);

  // get the remote data
  memset(data2,0,sizeof(data2));
  if (strstr(name,".xcash") != NULL)
  {
    if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"address",data2) == 1)
    {
      REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(data2);
    }
  }
  else if (strstr(name,".sxcash") != NULL)
  {
    if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"saddress",data2) == 1)
    {
      REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(data2);
    }
  }
  else if (strstr(name,".pxcash") != NULL)
  {
    if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"paddress",data2) == 1)
    {
      REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(data2);
    }
  }
  else if (strstr(name,".website") != NULL)
  {
    if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"website",data2) == 1)
    {
      REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(data2);
    }
  }
  
  REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR;

  #undef DATABASE_COLLECTION
  #undef REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR
  #undef REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS
}



/*
-----------------------------------------------------------------------------------------------------------
Name: server_receive_data_socket_remote_data_get_address_from_name
Description: Runs the code when the server receives /remotedatagetaddressfromname
Parameters:
  CLIENT_SOCKET - The socket to send data to
  DATA - The data
Return: 0 if an error has occured, 1 if successfull
-----------------------------------------------------------------------------------------------------------
*/

int server_receive_data_socket_remote_data_get_address_from_name(const int CLIENT_SOCKET, const char* DATA)
{
  // Variables
  char name[REMOTE_DATA_NAME_MAXIMUM_LENGTH+1];
  char data2[BUFFER_SIZE];
  char message[BUFFER_SIZE];
  int name_settings_length;

  // define macros
  #define DATABASE_COLLECTION "remote_data"
  #define REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR \
  if (debug_settings == 1) \
  { \
    memcpy(error_message.function[error_message.total],"server_receive_data_socket_remote_data_get_address_from_name",60); \
    memcpy(error_message.data[error_message.total],"Invalid parameters",18); \
    error_message.total++; \
  } \
  memset(message,0,strlen(message)); \
  memcpy(message,"{\"Error\":\"Could not get the remote_data\"}",41); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,strlen(message),400,"application/json"); \
  return 0;

  #define REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(settings) \
  memset(message,0,strlen(message)); \
  memcpy(message,"{\"remote_data\":\"",16); \
  memcpy(message+strlen(message),settings,strlen(settings)); \
  memcpy(message+strlen(message),"\"}",2); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,strlen(message),200,"application/json"); \
  return 1;
  
  memset(name,0,sizeof(name));
  memset(data2,0,sizeof(data2));
  memset(message,0,sizeof(message));

  // get the parameter1
  memcpy(name,&DATA[45],(strnlen(DATA,sizeof(name)) - strnlen(strstr(DATA," HTTP/"),sizeof(name)))-45);

  // error check
  if (strncmp(name,"",BUFFER_SIZE) == 0 || strlen(name) > REMOTE_DATA_NAME_MAXIMUM_LENGTH)
  {
    REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR;
  } 

  // validate the name settings
  name_settings_length = strstr(name,".xcash") != NULL ? 6 : strstr(name,".sxcash") != NULL ? 6 : strstr(name,".pxcash") != NULL ? 6 : strstr(name,".website") != NULL ? 6 : 0;
  if (name_settings_length == 0)
  {
    REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR;
  }

  // check if the tx_hash is empty or the name is expired
  if (validate_name(name) == 0)
  {
    REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR;
  }

  // create the message
  memcpy(message,"{\"name\":\"",9);
  memcpy(message+strlen(message),name,strlen(name)-name_settings_length);
  memcpy(message+strlen(message),"\"}",2);

  // get the remote data
  memset(data2,0,sizeof(data2));
  if (strstr(name,".xcash") != NULL)
  {
    if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"address",data2) == 1)
    {
      REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(data2);
    }
  }
  else if (strstr(name,".sxcash") != NULL)
  {
    if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"saddress",data2) == 1)
    {
      REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(data2);
    }
  }
  else if (strstr(name,".pxcash") != NULL)
  {
    if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"paddress",data2) == 1)
    {
      REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(data2);
    }
  }
  else if (strstr(name,".website") != NULL)
  {
    if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"website",data2) == 1)
    {
      REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS(data2);
    }
  }
  
  REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR;

  #undef DATABASE_COLLECTION
  #undef REMOTE_DATA_GET_ADDRESS_FROM_NAME_ERROR
  #undef REMOTE_DATA_GET_ADDRESS_FROM_NAME_STATUS
}



/*
-----------------------------------------------------------------------------------------------------------
Name: server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_information_from_name
Description: Runs the code when the server receives the NODE_TO_NETWORK_DATA_NODES_REMOTE_DATA_GET_INFORMATION_FROM_NAME message
Parameters:
  CLIENT_SOCKET - The socket to send data to
  MESSAGE - The message
-----------------------------------------------------------------------------------------------------------
*/

void server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_information_from_name(const int CLIENT_SOCKET, const char* MESSAGE)
{
  // Variables
  char name[REMOTE_DATA_NAME_MAXIMUM_LENGTH+1];
  char message[BUFFER_SIZE];
  char data2[BUFFER_SIZE];
  int count;
  int count2;
  size_t data_size;

  // define macros
  #define DATABASE_COLLECTION "remote_data"
  #define REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR \
  if (debug_settings == 1) \
  { \
    memcpy(error_message.function[error_message.total],"server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_information_from_name",92); \
    memcpy(error_message.data[error_message.total],"Error: Could not get the address settings",41); \
    error_message.total++; \
  } \
  memset(message,0,strlen(message)); \
  memcpy(message,"Error: Could not get the information}",37); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,0,0,""); \
  return;
  
  memset(name,0,sizeof(name));
  memset(data2,0,sizeof(data2));
  memset(message,0,sizeof(message));

  // parse the message
  for (count = 0, count2 = 0; count < REMOTE_DATA_GET_ADDRESS_FROM_NAME_PARAMETER_AMOUNT; count++)
  {
    if (count == 1)
    {
      if ((data_size = strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) - count2) > REMOTE_DATA_NAME_MAXIMUM_LENGTH)
      {
        REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
      }
      memcpy(name,&MESSAGE[count2],data_size);
    }
    count2 = (int)(strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) + 1);
  }

  // check if the tx_hash is empty or the name is expired
  if (validate_name(name) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }

  // get the remote data
  memset(message,0,sizeof(message));  
  memcpy(message,"Information for ",16);
  memcpy(message+strlen(message),name,strlen(name));
  memcpy(message+strlen(message),":\r\naddress: ",12);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"address",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\r\nprivate only address (saddress): ",35);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"saddress",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\r\npublic only address (paddress): ",34);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"paddress",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\r\nwebsite: ",11);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"website",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\r\ntimestamp: ",13);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"timestamp",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\r\nregister delegate: ",21);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"reserve_delegate_address",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\r\nregister amount: ",19);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"reserve_delegate_amount",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\r\ntx_hash: ",11);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"tx_hash",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\r\n\r\n}",5);

  send_data(CLIENT_SOCKET,(unsigned char*)message,0,0,"");
  return;

  #undef DATABASE_COLLECTION
  #undef REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR
}



/*
-----------------------------------------------------------------------------------------------------------
Name: server_receive_data_socket_remote_data_get_information_from_name
Description: Runs the code when the server receives /remotedatagetinformationfromname
Parameters:
  CLIENT_SOCKET - The socket to send data to
  DATA - The data
Return: 0 if an error has occured, 1 if successfull
-----------------------------------------------------------------------------------------------------------
*/

int server_receive_data_socket_remote_data_get_information_from_name(const int CLIENT_SOCKET, const char* DATA)
{
  // Variables
  char name[REMOTE_DATA_NAME_MAXIMUM_LENGTH+1];
  char message[BUFFER_SIZE];
  char data2[BUFFER_SIZE];

  // define macros
  #define DATABASE_COLLECTION "remote_data"
  #define REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR \
  if (debug_settings == 1) \
  { \
    memcpy(error_message.function[error_message.total],"server_receive_data_socket_nodes_to_network_data_nodes_remote_data_get_information_from_name",92); \
    memcpy(error_message.data[error_message.total],"Error: Could not get the address settings",41); \
    error_message.total++; \
  } \
  memset(message,0,strlen(message)); \
  memcpy(message,"{\"Error\":\"Could not get the information\"}",41); \
  send_data(CLIENT_SOCKET,(unsigned char*)message,strlen(message),400,"application/json"); \
  return 0;
  
  memset(name,0,sizeof(name));
  memset(data2,0,sizeof(data2));
  memset(message,0,sizeof(message));

  // get the parameter1
  memcpy(name,&DATA[49],(strnlen(DATA,sizeof(name)) - strnlen(strstr(DATA," HTTP/"),sizeof(name)))-49);

  // error check
  if (strncmp(name,"",BUFFER_SIZE) == 0 || strlen(name) > REMOTE_DATA_NAME_MAXIMUM_LENGTH)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  } 

  // check if the tx_hash is empty or the name is expired
  if (validate_name(name) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }

  // get the remote data
  memset(message,0,sizeof(message));  
  memcpy(message,"{\"name\":\"",9);
  memcpy(message+strlen(message),name,strlen(name));
  memcpy(message+strlen(message),"\",\"address\":\"",13);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"address",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\",\"saddress\":\"",14);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"saddress",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\",\"paddress\":\"",14);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"paddress",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\",\"website\":\"",13);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"website",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\",\"timestamp\":\"",15);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"timestamp",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\",\"reserve_delegate_address\":\"",30);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"reserve_delegate_address",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\",\"reserve_delegate_amount\":\"",29);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"reserve_delegate_amount",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\",\"tx_hash\":\"",13);
  memset(data2,0,sizeof(data2));
  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,message,"tx_hash",data2) == 0)
  {
    REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR;
  }
  memcpy(message+strlen(message),data2,strlen(data2));
  memcpy(message+strlen(message),"\"}",2);

  send_data(CLIENT_SOCKET,(unsigned char*)message,strlen(message),200,"application/json");
  return 1;

  #undef DATABASE_COLLECTION
  #undef REMOTE_DATA_GET_INFORMATION_FROM_NAME_ERROR
}



/*
-----------------------------------------------------------------------------------------------------------
Name: server_receive_data_socket_remote_data_nodes_to_block_verifiers_update_remote_data
Description: Runs the code when the server receives the NODES_TO_BLOCK_VERIFIERS_UPDATE_REMOTE_DATA message
Parameters:
  CLIENT_SOCKET - The socket to send data to
  MESSAGE - The message
-----------------------------------------------------------------------------------------------------------
*/

void server_receive_data_socket_remote_data_nodes_to_block_verifiers_update_remote_data(const int CLIENT_SOCKET, const char* MESSAGE)
{
  // Variables
  char data[4096];
  char data2[4096];
  char delegate_public_address[XCASH_WALLET_LENGTH+1];
  char item[100];
  char value[100];
  int count;
  int count2;
  size_t data_size;

  // define macros
  #define DATABASE_COLLECTION "remote_data"
  #define REMOTE_DATA_UPDATE_DATA_ERROR \
  if (debug_settings == 1) \
  { \
    memcpy(error_message.function[error_message.total],"server_receive_data_socket_remote_data_nodes_to_block_verifiers_update_remote_data",82); \
    memcpy(error_message.data[error_message.total],"Could not update the remote data",32); \
    error_message.total++; \
  } \
  send_data(CLIENT_SOCKET,(unsigned char*)"Could not update the remote data}",0,0,""); \
  return;

  memset(data,0,sizeof(data));
  memset(data2,0,sizeof(data2));
  memset(delegate_public_address,0,sizeof(delegate_public_address));
  memset(item,0,sizeof(item));
  memset(value,0,sizeof(value));

  // verify the message
  if (verify_data(MESSAGE,0) == 0 || string_count(MESSAGE,"|") != REMOTE_DATA_UPDATE_DATA_PARAMETER_AMOUNT || check_for_invalid_strings(MESSAGE) == 0)
  {   
    REMOTE_DATA_UPDATE_DATA_ERROR;
  }

  // parse the message
  for (count = 0, count2 = 0; count < REMOTE_DATA_UPDATE_DATA_PARAMETER_AMOUNT; count++)
  {
    if (count == 1)
    {
      if ((data_size = strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) - count2) >= sizeof(item))
      {
        REMOTE_DATA_UPDATE_DATA_ERROR;
      }
      memcpy(item,&MESSAGE[count2],data_size);
    }
    if (count == 2)
    {
      if ((data_size = strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) - count2) >= sizeof(value))
      {
        REMOTE_DATA_UPDATE_DATA_ERROR;
      }
      memcpy(value,&MESSAGE[count2],data_size);
    }
    if (count == 3)
    {
      if ((data_size = strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) - count2) != XCASH_WALLET_LENGTH)
      {
        REMOTE_DATA_UPDATE_DATA_ERROR;
      }
      memcpy(delegate_public_address,&MESSAGE[count2],data_size);
    }
    count2 = (int)(strlen(MESSAGE) - strlen(strstr(MESSAGE+count2,"|")) + 1);
  }

  // check if the data is valid
  if (strlen(delegate_public_address) != XCASH_WALLET_LENGTH || strncmp(delegate_public_address,XCASH_WALLET_PREFIX,sizeof(XCASH_WALLET_PREFIX)-1) != 0)
  {
    REMOTE_DATA_UPDATE_DATA_ERROR;
  }

  // check if the item is valid
  if (strncmp(item,"website",BUFFER_SIZE) != 0 && strncmp(item,"smart_contract_hash",BUFFER_SIZE) != 0)
  {    
    REMOTE_DATA_UPDATE_DATA_ERROR;
  }

  // create the message
  memcpy(data,"{\"address\":\"",12);
  memcpy(data+19,delegate_public_address,XCASH_WALLET_LENGTH);
  memcpy(data+strlen(data),"\"}",2);

  if (read_document_field_from_collection(remote_data_database_name,DATABASE_COLLECTION,data,"name",data2) == 0 || validate_name(data2) == 0)
  {
    REMOTE_DATA_UPDATE_DATA_ERROR;
  }

  memset(data2,0,sizeof(data2));
  memcpy(data2,"{\"",2);
  memcpy(data2+2,item,strnlen(item,sizeof(item)));
  memcpy(data2+strlen(data2),"\":\"",3);
  memcpy(data2+strlen(data2),value,strnlen(value,sizeof(value)));
  memcpy(data2+strlen(data2),"\"}",2);

  // update the delegate in the database
  if (update_document_from_collection(remote_data_database_name,DATABASE_COLLECTION,data,data2) == 0)
  {
    REMOTE_DATA_UPDATE_DATA_ERROR;
  }

  send_data(CLIENT_SOCKET,(unsigned char*)"Updated the remote data}",0,0,"");
  return;

  #undef DATABASE_COLLECTION
  #undef REMOTE_DATA_UPDATE_DATA_ERROR
}