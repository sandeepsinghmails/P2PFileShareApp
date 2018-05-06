/* FILE: checksum.cpp
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION: Checksum Functionality
 */
#include "main.h"
#include "logger.h"
#include "checksum.h"

unsigned int find_checksum(string filename)  
{
    ifstream file(filename);
   
    unsigned int checksum = 0;
    unsigned int shift = 0;
    for (int ch = file.get(); file; ch = file.get()) 
    {
        checksum += (ch << shift);
        shift += 8;
        if (shift == 32) {
            shift = 0;
        }
    }
	logger << LOG_MSG ("TRACE: filename = ") << filename << " & checksum = " << checksum << endl;
    return checksum;
}
