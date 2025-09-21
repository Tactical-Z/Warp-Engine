#pragma once

#include "Logger.h"

#include <pthread.h>

#define MaxThreads 20

struct ThreadData{
    pthread_t threadID[MaxThreads];
};

enum ThreadStatus{
    TS_ACTIVE,
    TS_SHUTTINGDOWN,
    TS_DEACTIVE
}; 
volatile extern enum ThreadStatus threadStatus;

extern int threadEnabled[MaxThreads];

int InitThreads(struct ThreadData* _threadData);
void RunThread(struct ThreadData* _threadData, int _thredId, void* (*_func)(void*), void* _arg);
int CleanupThreads(struct ThreadData** _threadData);