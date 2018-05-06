/* FILE: discover.h
 * AUTHOR: Sandeep Singh
 * DATE: May 6, 2018
 * DESCRIPTION : Header File for Server Thread Pooling
 */
#ifndef __THREADPOOLING_H_
#define __THREADPOOLING_H_

// =============================================================================================================================
// 										           Thread Pooling 
// =============================================================================================================================
#define MAX_THREADS 3
#define MAXNCLI     3
pthread_mutex_t		clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t		clifd_cond  = PTHREAD_COND_INITIALIZER;

typedef struct {
  pthread_t		thread_tid;		// thread ID 
  long			thread_count;	// # connections handled
} Thread;
Thread	*tptr;		            // array of Thread structures; calloc'ed for run-time configuration

int clifd[MAXNCLI];  		    // array in which service thread will store the connected socket descriptors 
int iput; 					    // index of clifd[] where the next clientSockfd will be stored by the service thread
int iget; 					    // index of clifd[] from where a new clientSockfd will be fetched by one of the threads in the pool

extern unsigned int find_checksum(string filename);
// Device Log Functionality
extern ofstream logger;
#endif // #ifndef __THREADPOOLING_H_
