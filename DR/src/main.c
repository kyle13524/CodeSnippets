/*
*	Filename:		main.c
*	Programmer:		Kyle Jensen
*	Description:	The purpose of this program is to take incoming messages from the Data Creator (DC) application
*					and parse the values and send them to the Data Monitor (DM), aswell as logging the values. When a DC
*					application goes offline, it will remove it from the masterlist. When all DCs are gone, it will
*					send a final message to the DM indicating that the DR has gone offline.
*	Date: 			April 7, 2016	
*/

#include "../inc/protos.h"


//Don't tell Carlo but I am using a global variable for the master list so the signal handler can access it.
MasterList *masterList;

int main(void)
{
	//Determines whether the watchdog should be started (true when first message comes in)
	bool messagesStarted = false;

	//ID numbers
	pid_t messageID = -1;
	pid_t sharedMemoryID = -1;
	pid_t semaphoreID = -1;

	//Keys for message queue and shared memory
	key_t messageKey = 0;
	key_t sharedMemoryKey = 0;

	//Status message struct to fill with information from incoming messages
	StatusMessage statusMessage;

	//Get a message key
	messageKey = ftok ("/tmp", 4578);

	//Get a shared memory key
	sharedMemoryKey = ftok(".", 16535);

	//Check return values
	if (messageKey == -1 || sharedMemoryKey == -1) 
	{ 
		printf ("(SERVER) Cannot allocate key\n");
		logErrorStatus("Cannot allocate message/memory keys.", __FILE__, __LINE__);
		return kMessageKeyError;
	}

	//Create message queue if not already created
	if(!createMessageQueue(&messageID, messageKey))
	{
		printf("(SERVER) Cannot allocate a new message queue.\n");
		logErrorStatus("Cannot allocate a new message queue.", __FILE__, __LINE__);
		return kMessageQueueError;
	}

	//Allocate memory for the master list
	if(!allocateSharedMemory(&sharedMemoryID, sharedMemoryKey))
	{
		printf("(SERVER) Cannot allocate new shared memory.\n");
		logErrorStatus("Cannot allocate new shared memory.", __FILE__, __LINE__);
		return kSharedMemoryError;
	}

	//Allocate memory to the master list
	masterList = (MasterList*) shmat(sharedMemoryID, NULL, 0);

	//Make sure memory has been successfully allocated
	if(masterList == NULL)
	{
		printf("(SERVER) Cannot allocate to shared memory.\n");
		logErrorStatus("Cannot allocate to shared memory.", __FILE__, __LINE__);
		return kSharedMemoryError;
	}

	// Set the message queue ID for the Master List
	masterList->msgQueueID = messageID;

	//Sleep for 15 seconds
	sleep(15);

	semaphoreID = initializeSemaphore();
	if(semaphoreID == -1)
	{
		return kSharedMemoryError;
	}

	//Enter main processing loop
	do
	{
		//Size of message
		size_t size = sizeof(StatusMessage) - sizeof(long);

		//Receive a message from the message queue
		if(msgrcv(messageID, &statusMessage, size, 0, 0) != -1)
		{
			if(!messagesStarted)
			{
				startWatchDog(kAlarmTime);
				messagesStarted = true;
			}

			handleMessage(statusMessage, masterList);
			writeToSharedFile(semaphoreID, statusMessage.machineID, statusMessage.status_code);
		}

	} while(masterList->numberOfDCs > 0);

	writeToSharedFile(semaphoreID, 0x00000000, 0xFFFFFFFF);

	//**Need to close all IPC schemes here!**

	semctl(semaphoreID, 0, IPC_RMID);

	return kNoError;
}


void startWatchDog(int alarmTime)
{
	signal(SIGALRM, checkMasterList);
	alarm(alarmTime);
}


void checkMasterList(int signal)
{
	if(masterList->numberOfDCs > 0)
	{
		int i = 0;
		for(i = 0; i < masterList->numberOfDCs; i++)
		{
			int timeDifference = (int) time(NULL) - masterList->dc[i].lastTimeHeardFrom;
			if(timeDifference > kMaxMessageWaitTime)
			{
				printf("(SERVER) PID $%d hasn't been heard from in the past 30 seconds. Removing from master list.\n", masterList->dc[i].dcProcessID);
				removeDCFromList(masterList->dc[i].dcProcessID);
				i -= 1;
			}
		}
	}

	startWatchDog(kAlarmTime);
}


bool allocateSharedMemory(pid_t* sharedMemoryID, int sharedMemoryKey)
{
	bool success = true;

	//Once a message queue is available, create shared memory for MasterList
	if((*sharedMemoryID = shmget(sharedMemoryKey, sizeof(masterList), 0)) == -1)
	{
		printf("(SERVER) No shared memory available for MasterList. Creating memory.\n");
		*sharedMemoryID = shmget(sharedMemoryKey, sizeof(masterList), IPC_CREAT | 0660);
		if(*sharedMemoryID == -1)
		{
			success = false;		
		}
	}

	return success;
}


int getIndexOfDC(pid_t pid)
{
	int index = -1;
	int i = 0;
	for(i = 0; i < kMaxDCCount; i++)
	{
		if(pid == masterList->dc[i].dcProcessID)
		{
			index = i;
			break;
		}
	}

	return index;
}


void removeDCFromList(pid_t pid)
{
	bool found = false;

	int i = 0;
	for(i = 0; i < kMaxDCCount; i++)
	{
		if(masterList->dc[i].dcProcessID == pid)
		{
			//Remove the dc
			masterList->dc[i] = emptyDCInfo;
			masterList->numberOfDCs--;
			found = true;

			continue;
		}


		//Collapse the array so all values are in adjacent elements
		if(found)
		{
			if(masterList->dc[i].dcProcessID != 0)
			{
				masterList->dc[i - 1].dcProcessID = masterList->dc[i].dcProcessID;
				masterList->dc[i - 1].lastTimeHeardFrom = masterList->dc[i].lastTimeHeardFrom;
				masterList->dc[i] = emptyDCInfo;
			}
		}
	}
}


int logMessage(const char* format, ...)
{
	int done;

	FILE* file = NULL;

	char buffer[kLogBufferSize] = { 0 };
	struct tm *sTm;

	time_t now = time(NULL);
	sTm = localtime(&now);

	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", sTm);

	file = fopen(kLogFile, "a");

	if(file == NULL)
	{
		printf("Error opening log file.\n");
	}
	else
	{
		va_list arg;

		fprintf(file, "%s", buffer);

		va_start(arg, format);
		done = vfprintf(file, format, arg);
		va_end(arg);

		fprintf(file, "\n");
	}

	if(fclose(file) != 0)
	{
		printf("Error closing log file.\n");
	}

	return done;
}




void logDCStatus(int dc, int pid, int newStatus)
{
	logMessage("DC #%d (pid %d) registered a new status of %d", dc, pid, newStatus);
}




void logErrorStatus(char* message, char* file, int line)
{
	logMessage("ERROR >> %s (%s, %d)", message, file, line);
}



