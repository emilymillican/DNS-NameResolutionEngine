#include "multi-lookup.h"
#include "util.c"
#include "util.h"


int main(int argc, char* argv[])
{
    struct thread_info info;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    char* end1;
    char* end2;
    long int requesters = strtol(argv[1], &end1, 10);
    long int resolvers = strtol(argv[2], &end2, 10);
    if(requesters > 10 || resolvers <1)
    {
        printf("error - the maximum number of requester threads supported is 10\n");
        return -1;
    }
    if(resolvers  > 5 || resolvers < 1)
    {
        printf("error - the maximum number of resolver threads supported is 5\n");
        return -1;
    }
    if((argc-5) > 10)
    {
        printf("error - the maximum number of input files supported is 10\n");
        return -1;
    }
    char* serviced = argv[3];
    char* results = argv[4];
    for(int numfiles = 0; numfiles < 10; numfiles++)
    //for(int numfiles = 0; numfiles < (argc-5); numfiles++)
    {
        if (numfiles < (argc-5)) {
           info.namefiles[numfiles] = (char*)malloc(1025*sizeof(char));
           strcpy(info.namefiles[numfiles], argv[numfiles+5]);
           info.namefilesstat[numfiles] = 0;
        }   
        else {
           info.namefiles[numfiles] = NULL;
           info.namefilesstat[numfiles] = 2;
        }     
        info.file[numfiles] = NULL;
    }

    info.numfiles = argc-5;
    pthread_t tid[requesters+resolvers];
    info.tid = tid;
    info.bufferindexput = 0;
    info.bufferindexget = 0;
    int i;
    int j;

    for(i = 0; i < BUFFERSIZE; i++)
    {
        info.buffer[i] = (char*)malloc(1025*sizeof(char));
        *info.buffer[i] = '\0';
    }

    pthread_mutex_t namearraylock;
    pthread_mutex_t bufferlock;
    pthread_mutex_t servicedlock;
    pthread_mutex_t resultslock;
    pthread_mutex_t bufferindexlock;
    pthread_mutex_t filelock;


    sem_t space_avail;
    sem_t items_avail;


    if (pthread_mutex_init(&namearraylock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    }
    if (pthread_mutex_init(&bufferlock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    if (pthread_mutex_init(&servicedlock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    if (pthread_mutex_init(&resultslock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    if (pthread_mutex_init(&bufferindexlock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    if (pthread_mutex_init(&filelock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

  
    info.namearraylockptr = &namearraylock;
    info.bufferlockptr = &bufferlock;
    info.servicedlockptr = &servicedlock;
    info.resultslockptr = &resultslock;
    info.bufferindexlockptr = &bufferindexlock;
    info.filelockptr = &filelock;

    sem_init(&space_avail, 0, BUFFERSIZE);
    sem_init(&items_avail, 0, 0);

    info.space_availptr = &space_avail;
    info.items_availptr = &items_avail;


    info.resultsfileptr = fopen(results, "w");
    if(info.resultsfileptr == NULL)
    {
        fprintf(stderr, "bad output file path: %s\n", results);
        return -1;

    }
    info.servicedfileptr = fopen(serviced, "w");
    if(info.servicedfileptr == NULL)
    {
        fprintf(stderr, "bad output file path: %s\n", serviced);
        return -1;
    }


    for(i = 0; i < requesters; i++)
    {
        pthread_create(&info.tid[i], NULL, &requesterFunction, &info);
    }

    for(j = 0; j < resolvers; j++)
    {
        pthread_create(&info.tid[j+i], NULL, &resolverFunction, &info);
    }

    for(i = 0; i < requesters+resolvers; i++)
    {
        pthread_join(tid[i],NULL);
    }

    for(i = 0; i < info.numfiles; i++)
    {
        fclose(info.file[i]);
    } 

    fclose(info.resultsfileptr);
    fclose(info.servicedfileptr);

    for(int i = 0; i < BUFFERSIZE; i++)
    {
        free(info.buffer[i]);
    }

    pthread_mutex_destroy(&namearraylock);
    pthread_mutex_destroy(&bufferlock);
    pthread_mutex_destroy(&servicedlock);  
    pthread_mutex_destroy(&resultslock);
    pthread_mutex_destroy(&bufferindexlock);  
    pthread_mutex_destroy(&filelock);  
    sem_destroy(&space_avail); 
    sem_destroy(&items_avail); 
    for(int numfiles = 0; numfiles < 10; numfiles++)
    {
        free(info.namefiles[numfiles]);
    }

    gettimeofday(&end, NULL);
    double time_taken;
    time_taken = (end.tv_sec - start.tv_sec) * 1e6;
    time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6;
    printf("time taken = %f seconds\n", time_taken);
  
//use malloc when generating variables -> dont need shared memory, threads already share memory. 
}

void* requesterFunction(void* arg)
{
    char* processfile;
    int processfileindex;
    char* hostname = (char*)malloc(1026*sizeof(char));
    int count = 0;
    bool flag = true;
    struct thread_info* info = arg;
    
    while(1)
    {
        pthread_mutex_lock(info->namearraylockptr);
        for(int i = (info->numfiles-1); i >=0; i--)
        {
            if(info->namefilesstat[i] == 0)
            {
                processfile = info->namefiles[i];
                processfileindex = i;
                count++;
                break;
            }
            else if(i == 0)
            {
                for(int j = (info->numfiles-1); j>=0; j--)
                {
                    if(info->namefilesstat[j] == 1)
                    {
                        processfile = info->namefiles[j];
                        processfileindex = j;
                        count++;
                        break;
                    }
                    else if(j == 0)
                    {
                        flag = false;
                    }
                }
            }
        }
        pthread_mutex_unlock(info->namearraylockptr);
        if(flag == true)
        {
            pthread_mutex_lock(info->namearraylockptr);
            if(info->namefilesstat[processfileindex] == 0)
            {
                pthread_mutex_lock(info->filelockptr);
                info->file[processfileindex] = fopen (processfile,"r");
                if(info->file[processfileindex] == NULL)
                {
                    fprintf(stderr, "error accessing name file: %s\n", processfile);
                    return NULL;
                }
                pthread_mutex_unlock(info->filelockptr);
            }
            pthread_mutex_unlock(info->namearraylockptr);
            size_t len = 1026;
            int index;
            pthread_mutex_lock(info->namearraylockptr);
            info->namefilesstat[processfileindex] = 1;
            pthread_mutex_unlock(info->namearraylockptr);
            while (1)
            {
                pthread_mutex_lock(info->filelockptr);
                ssize_t read = getline(&hostname, &len, info->file[processfileindex]);
                pthread_mutex_unlock(info->filelockptr);
                if(read > 1025)
                {
                    printf("hostname %s too large. limit is 1024 characters.\n", hostname);
                    return NULL;
                }
                if(read == -1)
                {
                    break;
                }
                sem_wait(info->space_availptr);
                pthread_mutex_lock(info->bufferindexlockptr);
                index = info->bufferindexput;
                pthread_mutex_lock(info->bufferlockptr);
                strcpy(info->buffer[index], hostname);
                if(info->bufferindexput == BUFFERSIZE-1)
                {
                    info->bufferindexput = 0;
                }
                else
                {
                    info->bufferindexput++;
                }
                pthread_mutex_unlock(info->bufferlockptr);
                pthread_mutex_unlock(info->bufferindexlockptr);
                sem_post(info->items_availptr);
            }
            pthread_mutex_lock(info->namearraylockptr);
            info->namefilesstat[processfileindex] = 2;
            pthread_mutex_unlock(info->namearraylockptr);
        }
        else
        {
            break;
        }
    }
    pthread_mutex_lock(info->servicedlockptr);
    if (info->servicedfileptr!=NULL)
    {
        fprintf(info->servicedfileptr, "Thread %lu serviced %d files.\n", pthread_self(), count);
    }
    pthread_mutex_unlock(info->servicedlockptr);
    free(hostname);
    return 0;
}

void* resolverFunction(void* arg)
{
    char* hostname;
    bool flag = true;
    char* IP;
    struct thread_info* info = arg;
    int index;
    while(1)
    {
        pthread_mutex_lock(info->namearraylockptr);
        for(int i = info->numfiles; i >=0; i--)
        {
            if(info->namefilesstat[i] < 2)
            {
                break;
            }
            else if(i == 0)
            {
                flag = false;
            }
        }
        pthread_mutex_unlock(info->namearraylockptr);
        int val;
        sem_getvalue(info->items_availptr, &val);
        if(flag == true || val != 0)
        {
            hostname = (char*)malloc(1025*sizeof(char));
            sem_wait(info->items_availptr);
            pthread_mutex_lock(info->bufferindexlockptr);
            index = info->bufferindexget;
            pthread_mutex_lock(info->bufferlockptr);
            strcpy(hostname, info->buffer[index]);
            hostname[strlen(info->buffer[index]) - 1] = '\0';
            pthread_mutex_unlock(info->bufferlockptr);
            if(info->bufferindexget == BUFFERSIZE-1)
            {
                info->bufferindexget = 0;
            }
            else
            {
                info->bufferindexget++;
            }
            pthread_mutex_unlock(info->bufferindexlockptr);
            sem_post(info->space_availptr);
            IP = (char*)malloc(INET6_ADDRSTRLEN*sizeof(char));
            if(dnslookup(hostname, IP, INET6_ADDRSTRLEN) != 0)
            {
                fprintf(stderr, "dnslookup error - invalid hostname %s\n", hostname);
                *IP = '\0';
            }

            pthread_mutex_lock(info->resultslockptr);
            if (info->resultsfileptr!=NULL)
            {
                if (*IP == '\0') {
                   fprintf(info->resultsfileptr, "%s,\n", hostname);
                }  
                else {
                   fprintf(info->resultsfileptr, "%s, %s\n", hostname, IP);
                }   
            }
            pthread_mutex_unlock(info->resultslockptr);

            free(hostname);
            free(IP);
        }
        else
        {
            break;
        }
    }
    return 0;
}
