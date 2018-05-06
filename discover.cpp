/* FILE: discover.cpp
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION : Functionality for Discovery Module
 */

#include "main.h"
#include "logger.h"
#include "discover.h"

void* discovery_request_handler(void *arg)
{
	struct sockaddr_in server;
    struct sockaddr_in client;
    int sockfd;
    unsigned int clientlen;
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    char *myip = (char*)arg;
    char buf[BUFSIZE];
     
    //Create socket
    sockfd = socket(AF_INET , SOCK_DGRAM , 0);
    if (sockfd == -1)
    {
        logger << LOG_MSG ("ERROR: Failed to create SERVER THREAD Socket") << endl;
    }
    else
    {
        logger << LOG_MSG ("DISCOVERY THREAD Socket created") << endl;
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    
    //server.sin_addr.s_addr = inet_addr(myip);
    server.sin_port = htons( DISCOVERY_PORT );

    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR , (void *) &reuse, 
          sizeof(reuse)) < 0)
        logger << LOG_MSG ("setsockopt() failed") << endl;

    //Bind
    if( bind(sockfd,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        logger << LOG_MSG ("ERROR: DISCOVERY THREAD Bind Failed");
    }
    else
    {
        logger << LOG_MSG ("DISCOVERY THREAD Bind Successful") << endl;
    }
     

    //Accept incoming connection
    logger << LOG_MSG ("DISCOVERY THREAD Listening to Client Requests") << endl;
    clientlen = sizeof(client);

    while(true)
    {    
        /*
         * recvfrom: receive a UDP datagram from a client
         */
        bzero(buf, BUFSIZE);
        int n = recvfrom(sockfd, buf, BUFSIZE, 0,
		             (struct sockaddr *) &client, &clientlen);

        if (n < 0)
        {
          logger << LOG_MSG ("ERROR in recvfrom") << endl;
        }
        else
        {
            string str(buf);
	        logger << LOG_MSG ("server received datagram: ") << str << endl;
            
            string sendString(myip);
            int sendStringLen = sendString.length();  /* Find length of sendString */
            /* Broadcast sendString in datagram to client */
            if (sendto(sockfd, sendString.c_str(), sendStringLen, 0, (struct sockaddr *) 
                   &client, sizeof(client)) != sendStringLen)
                logger << LOG_MSG ("sendto() sent a different number of bytes than expected") << endl;

        }
    }
} 

void init_discovery_service(char* myip)
{
	pthread_t thread_id;
	
    if( pthread_create(&thread_id, NULL,  discovery_request_handler, (void*) myip) < 0)
    {
            logger << LOG_MSG ("ERROR: Failed to create DISCOVERY THREAD, RESTART THE APPLICATION");
            return;
    }
}
