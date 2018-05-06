/* FILE: threadpool.cpp
 * AUTHOR: Sandeep Singh
 * DATE: May 6, 2018
 * DESCRIPTION: Thread Pooling Functionality
 */
#include "main.h"
#include "logger.h"
#include "threadpool.h"

// =============================================================================================================================
// 						Thread Pooling Starts
// =============================================================================================================================

void delegate_to_worker_thread (int client_sock)
{
	pthread_mutex_lock(&clifd_mutex);
	clifd[iput] = client_sock;
	
    if (++iput == MAXNCLI)
    {
		iput = 0;
    }
	if (iput == iget)
    {
		logger << LOG_MSG ("Error: iput = iget = ") << iput << endl;
    }
	   
    pthread_cond_signal(&clifd_cond);
    pthread_mutex_unlock(&clifd_mutex);
}

void worker_request_handler(int connfd)
{
    logger << LOG_MSG ("Processing Client Request") << endl;
	
    int read_size = 0;
    char client_message[100];


     // clear the message buffer
	 memset(client_message, 0, 100);

	 // Receive a message from client
     // Client Message Format: ACTION#FILENAME
     // Examples: 
     // [1] get#test.txt
     //     worker_request_handler should RECEIVE a file named "text.txt.rcv" on connfd
     
     // [2] put#test.txt 
     //     worker_request_handler should SEND a file named "text.txt" on connfd
     read_size = recv(connfd , client_message, 100 , 0);

     // Parse the Client Request
     string action;
     string filename;
     string message(client_message);
     stringstream tmp(message);
     getline (tmp, action, '#');  
     getline (tmp, filename, '#');  
     
        
     logger << LOG_MSG ("Client Message: ") << client_message << endl;
     logger << LOG_MSG ("ACTION: ") << action << endl;
     logger << LOG_MSG ("FILENAME: ") << filename << endl;
     sleep(1);
    
     if (0 == action.compare("put"))
     {
     	logger << LOG_MSG ("ACTION = PUT") << endl;
     	// Send File to the Client
     	// Create an Input Stream
     	ifstream input(filename.c_str());
     	if (input.fail())
     	{
         	logger << LOG_MSG ("ERROR: Invalid Filename") << endl;
         	write(connfd, "P2P_ERR_NOSUCHFILE", strlen("P2P_ERR_NOSUCHFILE"));
     	}
	 	else
	 	{
            // Calculate Checksum
            unsigned int checksum = find_checksum(filename);
            char tmp[11];
            sprintf (tmp, "%u", checksum);
            string md5(tmp);

            logger << LOG_MSG ("checksum of outgoing file: ") << md5 << endl;
            write(connfd, tmp, 11);
            
            sleep(1);
     		/* Read data from file and send it */
     		while (!input.eof())        
     		{
        	 	/* First read file in chunks of BUF_SIZE bytes */
         		char buff[BUF_SIZE]={0};
         		input.read(buff, BUF_SIZE);
		 		int nread = input.gcount();

         		// logger << LOG_MSG ("Bytes read: ") << nread << endl;
		 		// logger << LOG_MSG ("Data read: ") << buff << endl;

         		// If read was success, send data
         		if(nread > 0)
         		{
                	logger << LOG_MSG ("Sending File: ") << client_message << " to Peer" << endl;
                	write(connfd, buff, nread);
            	}
         	}
     	 	input.close();
     	}
	 }
	 else if (0 == action.compare("get"))
	 {
     	logger << LOG_MSG ("ACTION = GET") << endl;
        /* Create file where data will be stored */
        string rcv_filename = filename+".rcv";
        /* Receive data in chunks of BUF_SIZE bytes */
        int bytesReceived = 0;
        char buff[BUF_SIZE];
        memset(buff, '0', sizeof(buff));
        bool isFirstByte = true;
        while((bytesReceived = read(connfd, buff, BUF_SIZE)) > 0)
        {
            if (true == isFirstByte)
            {
                if (0 == strncmp (buff, "P2P_ERR_NOSUCHFILE", strlen("P2P_ERR_NOSUCHFILE")))
                {
                    logger << LOG_MSG ("ERROR: Wrong Filename requested") << endl;
                    // return ERR_NOSUCHFILE;
                    close(connfd);
                    return;
                }
                isFirstByte = false;
            }
            ofstream output(rcv_filename.c_str());

            if (output.fail())
            {
               logger << LOG_MSG ("Failed to Create Output File on Receiver") << endl;
               close(connfd);
               return;
            }
            output.write(buff, bytesReceived);
        }

        if(bytesReceived < 0)
        {
            logger << LOG_MSG ("Error: No data received") << endl;
            close(connfd);
            return;
        }
     }
     close(connfd);
}


void* worker_thread_main (void *arg)
{
	// Socketfd for Client Requests (accepted by Service Thread)
    int connfd;
    int index = *((int*)arg);

    logger << LOG_MSG ("WORKER THREAD: ") << index << " is UP" << endl;
    for ( ; ; ) 
    {
        pthread_mutex_lock(&clifd_mutex);

        while (iget == iput)
        {
            pthread_cond_wait(&clifd_cond, &clifd_mutex);
        }
        connfd = clifd[iget];    // connected socket to service
        if (++iget == MAXNCLI)
        {
            iget = 0;
        }
         
        pthread_mutex_unlock(&clifd_mutex);
        tptr[index].thread_count++;

        worker_request_handler (connfd);      // process request
        close(connfd);
     }
}

void thread_make(int i)
{
    int *arg = new int (i);
    pthread_create(&tptr[i].thread_tid, NULL, &worker_thread_main, (void *) arg);
    return;     
}

void create_thread_pool ()
{
    tptr = new Thread[MAX_THREADS*sizeof(Thread)];

    for (int i = 0; i < MAX_THREADS; i++)
    {
        thread_make(i);
    }
}
