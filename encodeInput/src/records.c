/*	File Name:		records.c
*	Description:	This file holds all of the record building functions for the assignment.
*	Programmer:		Kyle Jensen & Devon Tomlin
*	Date:			March 2, 2016		
*/	

#include "../inc/protos.h"



/*	Function Name:	buildAssemblyRecord()
*	Purpose:		The purpose of this function is to build an assembly record given data from the file read.
*					The function parses the data into the asmRecord string which is passed in by reference.
*
*	Parameters:		asmRecord (char[]) = reference to the character array to hold the assembly record.
*					data (const char*) = the data that is passed into the function
*	Returns:		void.
*/
void buildAssemblyRecord(char asmRecord[], const unsigned char* data, int size)
{
	//Create a pointer to the asmRecord array
	char* record = asmRecord;

	//Add the dc.b instruction to the record, and increment the counter to point to the next open space.
	record += sprintf(record, "dc.b\t");

	//For all characters in the data, print the value of the ASCII character in hexadecimal format
	int i = 0;
	for(i = 0; i < size; i++)
	{
		int byte = data[i];
		record += sprintf(record, "$%.2X, ", byte);
	}

	//Reset the character pointer
	record = asmRecord;

	//Remove trailing comma
	char* whereComma = strrchr(record, ',');
	if(whereComma != NULL)
	{
		*whereComma = '\0';
	}
}



/*	Function Name:	buildSRecord()
*	Purpose:		The purpose of this function is to take the data from the read and parse it into Motorola S-Records.
*				
*	Parameters:		recordType (const char*) = the type of record to build(S0, S1, S5, S9)
*					addressField (int) = the value of the current address field (where this will exist in memory)
*					sRecord (char[]) = a reference to the record we want to fill with S-Record data.
*					data (const char*) = the data that is passed into the function
*	Returns:		void.
*/
void buildSRecord(const char* recordType, int addressField, char sRecord[], const unsigned char* data, int size)
{
	//Create a pointer to the sRecord array
	char* record = sRecord;

	//Get the count by adding the address bytes, the length of the data string, and the checksum
	int CC = kAddressByteSize + size + kCheckSumSize;

	//Add the record type, count, and address field to the S-Record.
	record += sprintf(record, "%s%.2X%.4X", recordType, CC, addressField);

	//For all bytes of data, add their hexadecimal values to the S-Record
	int i = 0;
	for(i = 0; i < size; i++)
	{
		unsigned int byte = data[i];
		record += sprintf(record, "%.2X", byte);

	}

	//Add the checksum value to the S-Record
	record += sprintf(record, "%.2X", checkSum(CC, addressField, data, size));
}



/*	Function Name:	checkSum()
*	Purpose:		The purpose of this function is to create a checksum value for all S-Records. The function takes
*					the count (CC), the address field, and data and adds up all of their values. It then uses the bit-flip
*					operator (~) to flip all of the bits giving the 1's complement. Then it does a binary AND (&) with 0xFF
*					to get the least significant byte of the value. This is our checkSum.
*
*	Parameters:		count (int) = the count (CC) field from the S-Record.
*					address (int) = the current value of the address field.
*					data (const char*) = the data being passed into the function.
*	Returns:		checksum (int) = the checksum calculated by the function.
*/
int checkSum(int count, int address, const unsigned char* data, int size)
{
	int i = 0;
	int dataTotal = 0;
	unsigned int checksum = 0;

	//For all letters in data stream
	for(i = 0; i < size; i++)
	{	
		unsigned int byte = data[i];
		dataTotal += byte;
	}

	checksum = ~(count + address + dataTotal) & 0xFF;

	return checksum;
}



