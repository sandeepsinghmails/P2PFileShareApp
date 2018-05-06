/* FILE: remote.cpp
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION : Server Functionality (Handling of remote requests)
 */
#include "main.h"
#include "logger.h"
#include "remote.h"

void* remote_request_handler(void *arg)
{
	struct sockaddr_in server;
    struct sockaddr_in client;
    int socket_desc;
    int client_sock;
    int c;
    char *myip = (char*)arg;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        logger << LOG_MSG ("ERROR: Failed to create SERVER THREAD Socket") << endl;
    }
    else
    {
        logger << LOG_MSG ("SERVICE THREAD Socket created") << endl;
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(myip);
    server.sin_port = htons( SERVICE_PORT );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        logger << LOG_MSG ("ERROR: SERVICE THREAD Bind Failed");
    }
    else
    {
        logger << LOG_MSG ("SERVICE THREAD Bind Successful") << endl;
    }
     
    //Listen
    listen(socket_desc , QUEUE_SIZE);

    //Accept incoming connection
    logger << LOG_MSG ("SERVICE THREAD Listening to Client Requests") << endl;
    c = sizeof(struct sockaddr_in);

    // Get Client Connections & hand them over to Worker Threads
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {    
       if (client_sock < 0)
       {
           logger << LOG_MSG ("FAILED To accept Client Connection");
       }
       else
       {
           logger << LOG_MSG ("ACCEPTED Client Connection") << endl;
			
           // Hand-over this connection to a Worker-Thread
           delegate_to_worker_thread (client_sock);
       }
    }
} 

void init_p2p_service(char* myip)
{
	pthread_t thread_id;
	
    if( pthread_create(&thread_id , NULL ,  remote_request_handler , (void*) myip) < 0)
    {
            logger << LOG_MSG ("ERROR: Failed to create SERVICE THREAD, RESTART THE APPLICATION");
            return;
    }
}
