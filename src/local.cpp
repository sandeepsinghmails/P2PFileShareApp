/* FILE: local.cpp
 * AUTHOR: Sandeep Singh
 * DATE: May 6, 2018
 * DESCRIPTION : Client Side Functionality
 */

#include "main.h"
#include "logger.h"
#include "local.h"

unsigned int receive_file (string filename, string peer_ip)
{

    //client(filename+".rcv", peer_ip);
    string rcv_filename = filename+".rcv";

    int portno, n;
    /* Create a socket first */
    int sockfd = 0;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        logger << LOG_MSG("ERROR: Failed to create client socket \n");
        return FAILURE;
    }

    /* Initialize sockaddr_in data structure */
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVICE_PORT); // port
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_addr.s_addr = inet_addr(peer_ip.c_str());

    /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        logger << LOG_MSG ("ERROR: Client Connect Failed") << endl;
        return FAILURE;
    }

    /* send the message line to the server */
    string request = "put#" + filename;
    logger << LOG_MSG ("TRACE: filename:: ") << filename << endl;
    logger << LOG_MSG ("TRACE: request:: ") << request << endl;
    n = write(sockfd, request.c_str(), strlen(request.c_str()));
    if (n < 0)
    { 
       logger << LOG_MSG ("ERROR: Failed to send command to Server") << endl;
       return FAILURE;
    }
    
    sleep(1);
   
    // Receive Checksum for upcoming file
    unsigned int pre_checksum = 0;
    char md5[11];	
    n = read(sockfd, md5, 11);
    if (n>0)
	{
		md5[sizeof md5 - 1] = '\0';
        stringstream strValue;
        strValue << md5;
        strValue >> pre_checksum;
        logger << LOG_MSG ("Checksum of upcoming file: ") << pre_checksum << endl;
	}
    else
    { 
        logger << LOG_MSG ("ERROR: Checksum not received") << endl;
        return FAILURE;
    }
	
	if (pre_checksum <= 0)
	{
    	logger << LOG_MSG ("ERROR: Wrong Filename requested") << endl;
        return ERR_NOSUCHFILE;
	}
	
    /* Create file where data will be stored */
    ofstream output(rcv_filename.c_str());

    if (output.fail())
    {
       logger << LOG_MSG ("Failed to Create Output File on Receiver") << endl;
       return FAILURE;
    }
    /* Receive data in chunks of BUF_SIZE bytes */
    int bytesReceived = 0;
    char buff[BUF_SIZE];
    memset(buff, '0', sizeof(buff));
    while((bytesReceived = read(sockfd, buff, BUF_SIZE)) > 0)
    {
        output.write(buff, bytesReceived);
    }

    if(bytesReceived < 0)
    {
        logger << LOG_MSG ("Error: No data received") << endl;
        return FAILURE;
    }
    close(sockfd);
    
    return pre_checksum;
}

unsigned int send_file (string filename, string peer_ip)
{

    int portno, n;
    /* Create a socket first */
    int sockfd = 0;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        logger << LOG_MSG("ERROR: Failed to create client socket \n");
        return FAILURE;
    }

    /* Initialize sockaddr_in data structure */
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVICE_PORT); // port
    serv_addr.sin_addr.s_addr = inet_addr(peer_ip.c_str());

    /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        logger << LOG_MSG ("ERROR: Client Connect Failed") << endl;
        return FAILURE;
    }

    /* send the message line to the peer */
    string request = "get#" + filename;
    logger << LOG_MSG ("TRACE: filename:: ") << filename << endl;
    logger << LOG_MSG ("TRACE: request:: ") << request << endl;
    n = write(sockfd, request.c_str(), strlen(request.c_str()));
    if (n < 0)
    { 
       logger << LOG_MSG ("ERROR: Failed to send data to Server") << endl;
    }
    sleep(1);
    // Create an Input Stream
    ifstream input(filename.c_str());
    if (input.fail())
    {
       logger << LOG_MSG ("ERROR: Invalid Filename") << endl;
       return ERR_NOSUCHFILE;
    }
        
    /* Read data from file and send it */
    while (!input.eof())        
    {
        /* First read file in chunks of BUF_SIZE bytes */
        char buff[BUF_SIZE]={0};
        input.read(buff, BUF_SIZE);
		int nread = input.gcount();

        logger << LOG_MSG ("Bytes read for sending: ") << nread << endl;
		//logger << LOG_MSG ("Data read: ") << buff << endl;

        /* If read was success, send data. */
        if(nread > 0)
        {
            logger << LOG_MSG ("Sending Bytes: ") << buff << endl;
            write(sockfd, buff, nread);
        }
    }
	input.close();
    close(sockfd);

    return SUCCESS;
}

void discover_peers (char* myip)
{   
    logger << LOG_MSG ("Discovering Peers ...") << endl;
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    struct sockaddr_in client;
    unsigned short broadcastPort;     /* Server port */
    string sendString;                 /* String to broadcast */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLen;       /* Length of string to broadcast */
    unsigned int clientlen;

    struct timeval read_timeout;
    read_timeout.tv_sec  = 0;
    read_timeout.tv_usec = 10;

    string tmp1(myip);
    string tmp3;
    string bcast;
    // Parse the IP Address
    stringstream tmp2(tmp1);
    getline (tmp2, tmp3, '.'); 
    bcast.append(tmp3+".");
    getline (tmp2, tmp3, '.'); 
    bcast.append(tmp3+".");
    getline (tmp2, tmp3, '.'); 
    bcast.append(tmp3+".");
    bcast.append("255");

    logger << LOG_MSG ("Broadcast Address: ") << bcast << endl;
    
    broadcastPort = DISCOVERY_PORT;      /* broadcast port */
    sendString = "discover";             /* string to broadcast */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        logger << LOG_MSG ("socket() failed") << endl;

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
        logger << LOG_MSG ("setsockopt() failed for broadcast") << endl;
   

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void *) &read_timeout, 
          sizeof(read_timeout)) < 0)
        logger << LOG_MSG ("setsockopt() failed for read timeout") << endl;
    

    // Construct local address structure 
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   
    broadcastAddr.sin_family = AF_INET;                 
    broadcastAddr.sin_addr.s_addr = inet_addr(bcast.c_str());
    broadcastAddr.sin_port = htons(broadcastPort);         

    sendStringLen = sendString.length();  
    // Broadcast sendString in datagram to clients 
    if (sendto(sock, sendString.c_str(), sendStringLen, 0, (struct sockaddr *) 
               &broadcastAddr, sizeof(broadcastAddr)) != sendStringLen)
    	logger << LOG_MSG ("sendto() sent a different number of bytes than expected") << endl;

    int attempts = MAX_DISCOVERABLE_PEERS;
    while (attempts)
    { 
        clientlen = sizeof(client);
        char buf[BUF_SIZE];
        bzero(buf, BUF_SIZE);
        int n = recvfrom(sock, buf, BUF_SIZE, 0,
		             (struct sockaddr *) &client, &clientlen);
        string str(buf);
     
        if (n < 0)
        {
            logger << LOG_MSG ("ERROR in recvfrom") << endl;
        }
        else if (str.compare(tmp1)!=0)
        {
	        logger << LOG_MSG ("Discovered Peer: ") << str << endl;
	        cout << "Discovered Peer: " << str << endl;
        }
        attempts--;
    }
}

void* local_request_handler (void *arg)
{
     logger << LOG_MSG ("Processing Client Request") << endl;

     unsigned int retVal = FAILURE;	
     int retries = 0;
     unsigned int post_checksum=0;
     char* cli = (char*)arg;
    
     // Parse the Arguments
     string action;
     string filename;
     string peer_ip;

     string str(cli);
     logger << LOG_MSG ("str:: ") << str << endl;
     stringstream tmp(str);
     getline (tmp, action, '#');  
     getline (tmp, filename, '#');  
     getline (tmp, peer_ip, '#');  
     
     logger << LOG_MSG ("ACTION: ") << action << endl;
     logger << LOG_MSG ("FILENAME: ") << filename << endl;
     logger << LOG_MSG ("Peer IP: ") << peer_ip << endl;

     if (0 == action.compare("rcv"))
     {
         do
		 {
             retVal = receive_file(filename, peer_ip);
             string rcv_filename = filename+".rcv";
    	     // Calculate Checksum of received file
    	     post_checksum = find_checksum (rcv_filename);
    	     logger << LOG_MSG ("Checksum of downloaded file: ") << post_checksum << endl;
             retries++;
		 } while ((FAILURE != retVal) && (ERR_NOSUCHFILE != retVal) && 
			  (retVal != post_checksum) && (retries < MAX_RETRIES));

			
   		 if (FAILURE == retVal)
		 {
			logger << LOG_MSG ("ERROR: Failed to Receive File") << endl;
		 }
		 else if (post_checksum == retVal)
		 {	
			logger << LOG_MSG ("File Received Successfully") << endl;
		 }
		 else if (ERR_NOSUCHFILE == retVal)
		 {
			logger << LOG_MSG ("ERROR: Invalid File Requested") << endl;
	     }

     }
     else if (0 == action.compare("send"))
     {
         retVal = send_file(filename, peer_ip);
		 if (ERR_NOSUCHFILE == retVal)
		 {
		     logger << LOG_MSG ("ERROR: Invalid File Requested") << endl;
		 }
		 else if (FAILURE == retVal)
		 {
		     logger << LOG_MSG ("ERROR: Failed to Receive File") << endl;
         }
         else
		 {
		     logger << LOG_MSG ("File Sent Successfully") << endl;
	     }
     }
}

void print_menu (char* myip)
{
    char menu_option;
    int retVal = FAILURE;
    string filename;
    string peer_ip;
    
    do
	{
        cout << "------------------------------------------\n" << endl;
        cout << "        P2P File Sharing App\n" << endl;
        cout << "------------------------------------------\n" << endl;
    	cout << "r -- Receive a file" << endl;
    	cout << "s -- Send a file" << endl;
    	cout << "d -- Discover peers" << endl;
    	cout << "q -- Quit this application" << endl;
    	cout << "Please enter an option from the main menu: ";
    	cin >> menu_option;		

    	switch(menu_option)
    	{
    		case 'r':
            {
				cout << "Activating Receive Module....\n" << endl;
    			cout << "Enter File to be Received: ";
    			cin  >> filename;
    			cout << "Enter Peer IP Address: ";
    			cin  >> peer_ip;
				
				pthread_t thread_id;
                string str =  "rcv#" + filename + '#' + peer_ip;
                char* arg = strdup(str.c_str());
                logger << LOG_MSG ("CLI receive:: string is: ") << str << endl;

                if( pthread_create(&thread_id , NULL ,  local_request_handler , (void*) arg) < 0)
    			{
            		logger << LOG_MSG ("ERROR: Failed to create CLIENT THREAD for receive");
            		return;
    			}
        		break;
            }
            case 's':
            {
               	cout << "Activating Send Module....\n" << endl;
    			cout << "Enter File to be Sent: ";
    			cin  >> filename;
    			cout << "Enter Peer IP Address: ";
    			cin  >> peer_ip;

                pthread_t thread_id;
                string str = "send#" + filename + '#' + peer_ip;
                char* arg = strdup(str.c_str());
                logger << LOG_MSG ("CLI send:: string is: ") << str << endl;

                if( pthread_create(&thread_id , NULL ,  local_request_handler , (void*) arg) < 0)
    			{
            		logger << LOG_MSG ("ERROR: Failed to create CLIENT THREAD for receive");
            		return;
    			}
        		break;
 
            }
    		case'd':
        	    discover_peers(myip);
        		break;
    		case'q':
        		break;
    		default:
        		cout << "invalid input" << endl;
            	break;
    	}
    }while(menu_option !='q'); 
}
