/*	File Name:		encodeInput.c
*	Description:	This file is the main file of the program. This program takes in input through command line arguments
*					for data to be parsed into either S-Records or ASM files. 
*	Programmer:		Kyle Jensen & Devon Tomlin
*	Date:			March 2, 2016		
*/	


#include "../inc/protos.h"

//Main Function
int main(int argc, char* argv[])
{
	FILE* inputFile = NULL;
	FILE* outputFile = NULL;

	char* inputFileName = NULL;
	char* outputFileName = NULL;
 
	int addressField = kInitialAddressField;
	int s1Count = 0;

	//Output format 0 = ASM, 1 = SREC
	int outputFormat = 0;

	int i = 0;
	for(i = 0; i < argc; i++)
	{
		char* argument = argv[i];

		//If the argument starts with a dash, a switch is incoming.. Not necessarily a valid switch though!
		if(strncmp(argument, "-", 1) == 0)
		{
			//Check if switch is valid
			int switchNum = isValidSwitch(argument);
			if(switchNum != 0)
			{
				switch(switchNum)
				{
					case 1:
						//If there is anything after the switch, it is supposedly a file
						if(strlen(argument) > strlen(kOutputSwitch))
						{
							//Remove the switch from the argument
							argument += strlen(kInputSwitch);

							//Allocate memory for the name of the input file.
							inputFileName = (char*) malloc(sizeof(char) * (strlen(argument) + strlen(kSrecExtension)) + 1);

							if((inputFile = fopen(argument, "rb")) == NULL)
							{
								printf("Error Opening Read File.\n");
								return 1;
							}
							else
							{
								strcpy(inputFileName, argument);
							}
						}

						break;
					case 2:
						//If there is anything after the switch, it is supposedly a file
						if(strlen(argument) > strlen(kOutputSwitch))
						{
							//Remove the switch from the argument
							argument += strlen(kOutputSwitch);

							//Allocate memory for the name of the output file.
							outputFileName = (char*) malloc(sizeof(char) * (strlen(argument) + strlen(kSrecExtension)) + 1);

							if((outputFile = fopen(argument, "w")) == NULL)
							{
								/*
								if(errno == EACCES)
								{
									printf("Error Opening Write File. User may have invalid write permissions.\n");
								}
								*/
								
							}
							else
							{
								strcpy(outputFileName, argument);
							}
						}

						break;
					case 3:
						//Set the format to SREC
						outputFormat = 1;
						break;
					case 4:
						//Display help file and quit program.
						displayHelpFile();
						return -1;
						break;
				}
			}
			else
			{
				printf("\n\"%s\" is not a valid switch\n", argument);
				printf("Heres some help on how to properly use this program..\n\n");
				displayHelpFile();
				return -1;
			}
		}
	}

	//If there is an input file and no output switch, we want to create an output file
	//with the file extension we want
	if(inputFile != NULL && outputFile == NULL)
	{
		//Allocate memory for name of output file.
		outputFileName = (char*) malloc(sizeof(char) * (strlen(inputFileName) + strlen(kSrecExtension)) + 1);
		strcpy(outputFileName, inputFileName);

		//If format is srec, append the srec file extension to the current file name and open a file for write
		if(outputFormat == 1)
		{
			strcat(outputFileName, kSrecExtension);
			outputFile = fopen(outputFileName, "w");
		}
		//If format is asm, append the asm file extension to the current file name and open a file for write
		else
		{
			strcat(outputFileName, kAsmExtension);
			outputFile = fopen(outputFileName, "w");
		}
	}


	//If the file handles have not been set to anything else, default them to standard in/out
	if(inputFile == NULL)
	{
		inputFile = stdin;
	}
	if(outputFile == NULL)
	{
		outputFile = stdout;
	}


	//If input file and output file are not NULL, we are good to go!
	if(inputFile != NULL && outputFile != NULL)
	{
		//Print s0 record if we are doing s-rec format
		if(outputFormat == 1)
		{
			//S0 Record
			char s0Record[kMaxSRecordSize] = { 0 };
			buildSRecord("S0", addressField, s0Record, MY_NAME, strlen(MY_NAME));
			fprintf(outputFile, "%s\n", s0Record);
		}

		//Buffer to hold the data we want to store.
		unsigned char buffer[kMaxLineSize + 1] = { '0' };
		int bytesRead = 0;

		while((bytesRead = fread(buffer, sizeof(char), kMaxLineSize, inputFile)))
		{
			//If we are using s-rec format, build an s1 record.
			if(outputFormat == 1)
			{
				char s1Record[kMaxSRecordSize] = { 0 };
				buildSRecord("S1", addressField, s1Record, buffer, bytesRead);

				fprintf(outputFile, "%s\n", s1Record);

				addressField += bytesRead;
				s1Count++;
			}
			else
			{
				char asmRecord[kMaxASMRecordSize] = { 0 };
				buildAssemblyRecord(asmRecord, buffer, bytesRead);

				fprintf(outputFile, "%s\n", asmRecord);
			}
		}

		//If we are using s-rec format, print s5 and s9 records.
		if(outputFormat == 1)
		{
			//S5 Record
			char s5Record[kMaxSRecordSize] = { 0 };
			buildSRecord("S5", addressField, s5Record, "", 0);
			fprintf(outputFile, "%s\n", s5Record);

			//S9 Record
			char s9Record[kMaxSRecordSize] = { 0 };
			buildSRecord("S9", kInitialAddressField, s9Record, "", 0);
			fprintf(outputFile, "%s\n", s9Record);
		}
	
	}
	else
	{
		printf("Error opening file\n");
	}

	//Free memory for file names.
	if(outputFileName != NULL)
	{
		free(outputFileName);
	}
	if(inputFileName != NULL)
	{
		free(inputFileName);
	}



	return 1;
}



/*	Function Name:	isValidSmith()
*	Purpose:		Return whether the switch is a valid switch.
*	Parameters:		s (char*) = the name of the switch we are checking.
*	Returns:		switch (int) = the status code of the switch.
*/
int isValidSwitch(char* s) 
{
	int switchType = 0;

	if(strncmp(s, kInputSwitch, 2) == 0)
	{
		switchType = 1;
	}
	else if(strncmp(s, kOutputSwitch, 2) == 0)
	{
		switchType = 2;
	}
	else if(strcmp(s, kSrecSwitch) == 0)
	{
		switchType = 3;
	}
	else if(strcmp(s, kHelpSwitch) == 0)
	{
		switchType = 4;
	} 

	return switchType;
}



/*	Function Name:	displayHelpFile()
*	Purpose:		The purpose of the function is to display the help file to the user.
*	Parameters:		none.
*	Returns:		void.
*/
void displayHelpFile()
{
	FILE* helpFile = fopen("help.txt", "r");

	if(helpFile != NULL)
	{
		while(!feof(helpFile))
		{
			char buffer[121] = { 0 };
			fgets(buffer, 121, helpFile);
			printf("%s", buffer);
			
		}
	}
	else
	{
		printf("Could not locate help file.\n");
	}
}