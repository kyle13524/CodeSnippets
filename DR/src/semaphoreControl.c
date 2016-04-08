/*
*	Filename:		semaphoreControl.c
*	Programmer:		Kyle Jensen
*	Description:	This file holds the definitions of functions to handle the semaphores. There is a function
*					for initializing the semaphore and writing a shared file.
*	Date: 			April 7, 2016	
*/

#include "../inc/protos.h"
#include "../../inc/semStruct.h"


pid_t initializeSemaphore(void)
{
	pid_t semaphoreID = -1;

	semaphoreID = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if(semaphoreID == -1)
	{
		printf("(SERVER) Cannot create semaphore.\n");
		logErrorStatus("Cannot create semaphore.", __FILE__, __LINE__);
	}

	printf("(SERVER) Semaphore ID is: %d\n", semaphoreID);

	//Initialize semaphore to a known value
	if(semctl(semaphoreID, 0, SETALL, init_values) == -1)
	{
		printf("(SERVER) Cannot initialize semaphores.\n");
		logErrorStatus("Cannot initialize semaphores.", __FILE__, __LINE__);
		semaphoreID = -1;
	}

	return semaphoreID;
}



int writeToSharedFile(pid_t semaphoreID, pid_t machineID, int statusCode)
{
	int success = kNoError;
	FILE* sharedFilePointer = NULL;

	//Enter the critical region (gain access to the "talking stick")
	if(semop (semaphoreID, &acquire_operation, 1) == -1)
	{
		printf("(SERVER) Cannot start critical region\n");
		logErrorStatus("Cannot start critical region", __FILE__, __LINE__);
		success = kCriticalRegionError;
	}

	//Open the shared file for appending in binary
	if((sharedFilePointer = fopen(kSharedFile, "ab+")) == NULL)
	{
		printf("(SERVER) Cannot write to shared file.\n");
		logErrorStatus("Cannot write to shared file.", __FILE__, __LINE__);
		success = kSharedFileError;
	}

	//Write the machineID and statusCode to the shared file
	fwrite(&machineID, sizeof(int), 1, sharedFilePointer);
	fwrite(&statusCode, sizeof(int), 1, sharedFilePointer);

	//Exit the critical region (make access to the "talking stick" available to use) 
	if(semop(semaphoreID, &release_operation, 1) == -1)
	{
		printf("(SERVER) Cannot exit critical region.\n");
		logErrorStatus("Cannot exit critical region.", __FILE__, __LINE__);
		success = kCriticalRegionError;
	}

	//Close the shared file
	if(fclose(sharedFilePointer) != 0)
	{
		printf("(SERVER) Cannot close shared file.\n");
		logErrorStatus("Cannot close shared file.", __FILE__, __LINE__);
		success = kSharedFileError;
	}

	return success;
}