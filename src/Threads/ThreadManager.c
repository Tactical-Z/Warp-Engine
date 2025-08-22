
#include "ThreadManager.h"

int threadEnabled[MaxThreads];
volatile enum ThreadStatus threadStatus = TS_DEACTIVE;

int InitThreads(struct ThreadData* _threadData){

    for(int i = 0; i < MaxThreads; i++){
        _threadData->threadID[i] = 0;
        threadEnabled[i] = 0; 
    };
   
    return 1;
};

void RunThread(struct ThreadData* _threadData, int _id, void* (*_func)(void*), void* _arg){

    // Check thread already active
    if(threadEnabled[_id]){
        LOG_ERROR("Thread %lu is already taken, cannot create new thread.", _threadData->threadID[_id]);
        return ;
    }

    // Activeate thread status, create new thread
    threadStatus = TS_ACTIVE;
    threadEnabled[_id] = 1;
    pthread_create(&_threadData->threadID[_id], NULL, _func, _arg);
    LOG_INFO("Started thread %i", _threadData->threadID[_id]);
}

int CleanupThreads(struct ThreadData** _threadData){
    threadStatus = TS_SHUTTINGDOWN;

    for(int i = 0; i < MaxThreads; i++){
        
        if(threadEnabled[i]){
            LOG_INFO("Joining Thread %lu with %lu", (*_threadData)->threadID[i],pthread_self());
            pthread_join((*_threadData)->threadID[i], NULL);
            threadEnabled[i] = 0;
        }
    };
    threadStatus = TS_DEACTIVE;

    free(*_threadData);
    *_threadData = NULL;

    return 0;
};