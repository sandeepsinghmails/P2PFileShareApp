/* FILE: local.h
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION : Header File for Client (Command Line) Functionality
 */
#ifndef __LOCAL_H__
#define __LOCAL_H__

#define MAX_DISCOVERABLE_PEERS 100

unsigned int receive_file (string filename, string peer_ip);
unsigned int send_file (string filename, string peer_ip);
void list_peers ();
void* local_request_handler (void *arg);
void print_menu ();

extern unsigned int find_checksum(string filename);
// Device Log Functionality
extern ofstream logger;
#endif
