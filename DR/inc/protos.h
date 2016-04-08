/*
*	Filename:		protos.h
*	Programmer:		Kyle Jensen
*	Description:	This is the header file for all the prototypes that the Data Reader application uses.
*	Date: 			April 7, 2016	
*/

#ifndef __PROTOS__
#define __PROTOS__

//List of system includes
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>


//Personal include files
#include "../../inc/masterlist.h"
#include "../../inc/message_struct.h"



//File definitions
#define kLogFile "/tmp/dataReader.log" 
#define kSharedFile "/tmp/machineStatus.db"

//Size definitions
#define kLogBufferSize 20
#define kMaxMessageLength 1024

//Time definitions
#define kMaxMessageWaitTime 30
#define kAlarmTime 35

//Error code definitions
#define kNoError 0
#define kMessageKeyError 1
#define kMessageQueueError 2
#define kSharedMemoryError 3
#define kSemaphoreError 4
#define kCriticalRegionError 5
#define kSharedFileError 6


//Empty information struct for DCInfo
static const DCInfo emptyDCInfo;

//Data creator functions
int getIndexOfDC(pid_t pid);
void removeDCFromList(pid_t pid);

//Shared Memory Functions
bool allocateSharedMemory(pid_t* sharedMemoryID, int sharedMemoryKey);

//Watchdog functions
void checkMasterList(int signal);
void startWatchDog(int alarmTime);

//Semaphore Control
pid_t initializeSemaphore(void);
int writeToSharedFile(pid_t semaphoreID, pid_t machineID, int statusCode);

//Message functions
void handleMessage(StatusMessage message, MasterList* masterList);
bool createMessageQueue(pid_t* messageID, int messageKey);

//Logging functions
int logMessage(const char* format, ...);
void logDCStatus(int dc, int pid, int newStatus);
void logErrorStatus(char* message, char* file, int line);


#endif