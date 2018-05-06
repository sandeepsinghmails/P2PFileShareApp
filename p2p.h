/* FILE: discover.h
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION : Header File for p2p main()
 */
#ifndef __P2P_H__
#define __P2P_H__
extern void print_menu (char* myip);
extern void delegate_to_worker_thread (int client_sock);
extern void create_thread_pool ();

extern void init_p2p_service(char* myip);
extern void init_discovery_service(char* myip);
// Device Log Functionality
extern ofstream logger;

#endif
