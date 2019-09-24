#ifndef SERVER_FUNCTIONS_H_   /* Include guard */
#define SERVER_FUNCTIONS_H_

/*
-----------------------------------------------------------------------------------------------------------
Function prototypes
-----------------------------------------------------------------------------------------------------------
*/

int start_new_round(void);
int start_current_round_start_blocks(void);
int data_network_node_create_block(void);
int start_part_4_of_round(void);
int update_block_verifiers_list(void);
int update_databases(void);
int add_block_verifiers_round_statistics(const char* BLOCK_HEIGHT);
int add_round_statistics(void);
int calculate_main_nodes_roles(void);
int server_received_data_XCASH_DPOPS_test_data(const int CLIENT_SOCKET, const char* MESSAGE);
int server_receive_data_socket_node_to_block_verifiers_add_reserve_proof(const int CLIENT_SOCKET, const char* MESSAGE);
int server_receive_data_socket_block_verifiers_to_block_verifiers_invalid_reserve_proofs(const char* MESSAGE);
int server_receive_data_socket_nodes_to_block_verifiers_register_delegates(const int CLIENT_SOCKET, const char* MESSAGE);
int server_receive_data_socket_nodes_to_block_verifiers_remove_delegates(const int CLIENT_SOCKET, const char* MESSAGE);
int server_receive_data_socket_nodes_to_block_verifiers_update_delegates(const int CLIENT_SOCKET, const char* MESSAGE);
int server_receive_data_socket_block_verifiers_to_network_data_nodes_block_verifiers_current_time(const int CLIENT_SOCKET, const char* MESSAGE);
int server_receive_data_socket_main_network_data_node_to_block_verifier_start_block(const char* MESSAGE);
int server_receive_data_socket_main_network_data_node_to_block_verifier_create_new_block(const int CLIENT_SOCKET, const char* MESSAGE);
int server_receive_data_socket_main_node_to_node_message_part_4(const char* MESSAGE);
int server_receive_data_socket_node_to_node(const char* MESSAGE);
int server_receive_data_socket_block_verifiers_to_block_verifiers_vrf_data(const char* MESSAGE);
int server_receive_data_socket_block_verifiers_to_block_verifiers_block_blob_signature(const char* MESSAGE);
int create_server(const int MESSAGE_SETTINGS);
int new_socket_thread(void);
int socket_thread(int client_socket);
#endif