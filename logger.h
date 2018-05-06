/* FILE: logger.h
 * AUTHOR: Sandeep Singh
 * DATE: Dec 31, 2017
 * DESCRIPTION : Header File for Logging Module
 */
#ifndef __LOGGER_H__
#define __LOGGER_H__
// =================================================================================================================================
// 										           Logging
// =================================================================================================================================

#define LOG_MSG(x) "[" << __TIME__ << "] " << __FUNCTION__ << "():: " << x

void config_device ();

#endif
