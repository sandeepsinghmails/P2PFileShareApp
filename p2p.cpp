/* FILE: p2p.cpp
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION : p2p device's main() 
 */
#include "main.h"
#include "logger.h"
#include "p2p.h"

int main (int argc, char **argv)
{
    if (argc != 2)
    {
		cout << "Usage: ./" << argv[0] << " <IP Address>" << endl;
        return 1;
	}

    // Step-1: Set Logging, generate Device-id 
    config_device ();

    // Step-2: Initialize the Main Service Thread  
    char* myip = argv[1];
    init_p2p_service(myip);

    // Step-3: Initialize the Peer Discovery Thread 
    init_discovery_service(myip);

    // Step-4: Create Thread Pool
    create_thread_pool();

    // Step-5: Print User Menu Options
    print_menu(myip);

    return 0;
}
