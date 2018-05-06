/* FILE: logger.cpp
 * AUTHOR: Sandeep Singh
 * DATE: May 6, 2018
 * DESCRIPTION : Logging Functionality
 */
#include "main.h"
#include "logger.h"

// Device Log Functionality
ofstream logger;

void config_device ()
{
    srand((unsigned int) time(NULL));
    unsigned int dev_id = rand();
	string filename = "log." + to_string(dev_id);
    cout << "Logfile Name: " << filename << endl;
	logger.open( filename.c_str(), ios::out | ios::app );
    
    if(!logger.is_open())
    {
        logger << "Failed to Create Log-File" << endl;
    }
}
