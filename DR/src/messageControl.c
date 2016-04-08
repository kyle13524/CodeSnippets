#include "../inc/protos.h"
#include "../../inc/message_struct.h"

bool createMessageQueue(int* messageID, int messageKey)
{
	bool success = true;
	//Check for existence of message queue
	if((*messageID = msgget(messageKey, 0)) == -1)
	{
		printf("(SERVER) No message queue. Creating one.\n");

		//If none are created, create a new message queue
		*messageID = msgget(messageKey, IPC_CREAT | 0660);
		if(*messageID == -1)
		{
			success = false;
		}
	}

	return success;
}



void handleMessage(StatusMessage message, MasterList* masterList)
{
	pid_t machineID = message.machineID;
	int statusCode = message.status_code;

	printf("(SERVER) Message Data (ID #%ld)  =  %d  :  %s\n", message.machineID, message.status_code, message.status);
			
	int dcIndex = getIndexOfDC(machineID);

	//dcIndex is -1 if the index does not exist
	if(dcIndex == -1 && masterList->numberOfDCs <= 10)
	{
		dcIndex = masterList->numberOfDCs;

		masterList->dc[dcIndex].dcProcessID = (pid_t) message.machineID;
		masterList->numberOfDCs++;

		logMessage("New data creator (PID: %d) seen by the message queue", message.machineID);
	}						

	if(message.status_code == MACHINE_IS_OFFLINE)
	{
		removeDCFromList(message.machineID);
	}
	else
	{
		masterList->dc[dcIndex].lastTimeHeardFrom = time(NULL);
	}

	logDCStatus(dcIndex, message.machineID, message.status_code);
}

