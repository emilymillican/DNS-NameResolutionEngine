#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include <pthread.h>
#include <semaphore.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>

#define BUFFERSIZE 10

struct thread_info {    
    pthread_t*          tid;
    char*               namefiles[10];
    int                 namefilesstat[10];  
    int                 numfiles;    
    pthread_mutex_t*    namearraylockptr;
    char*               buffer[BUFFERSIZE];
    pthread_mutex_t*    bufferlockptr;
    pthread_mutex_t*    servicedlockptr;              
    pthread_mutex_t*    resultslockptr;   
    sem_t*              space_availptr;
    sem_t*              items_availptr; 
    int                 bufferindexput;
    int                 bufferindexget;
    pthread_mutex_t*    bufferindexlockptr;
    pthread_mutex_t*    filelockptr;
    FILE*               file[10]; 
    FILE*               resultsfileptr;
    FILE*               servicedfileptr;               
    //need condition variables? or semaphores?  
};
void* requesterFunction(void* info);
void* resolverFunction(void* info);

#endif