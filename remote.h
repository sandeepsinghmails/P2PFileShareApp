/* FILE: discover.h
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION : Header File for Server Functionality
 */
#ifndef __REMOTE_H__
#define __REMOTE_H__

// Message Queue
#define QUEUE_SIZE 5 
void init_p2p_service(char* myip);
void* remote_request_handler(void *arg);

extern void delegate_to_worker_thread (int client_sock);

// Device Log Functionality
extern ofstream logger;
#endif
