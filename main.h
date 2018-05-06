/* FILE: discover.h
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION : Common Header File - for all modules
 */
#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>
#include <fstream>
#include <sstream>


#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

// For server
#include <netdb.h>
#define SERVICE_PORT 8888
#define DISCOVERY_PORT 9999

using namespace std;

// File Receive Buffer Size
#define BUF_SIZE 256
#define ERR_NOSUCHFILE 2 // Wrong Filename (File Not Present/cannot be shared with Peer)

// Receive Failure Auto-retries
#define MAX_RETRIES 3

// General 
#define SUCCESS 0
#define FAILURE 1

#endif // #ifndef __MAIN_H__
