/*	File Name:		protos.h
*	Description:	This file holds all of my prototypes, constants, and includes that are used within this program.
*	Programmer:		Kyle Jensen & Devon Tomlin
*	Date:			March 2, 2016		
*/	


#ifndef __PROTOS__
#define __PROTOS__

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <errno.h>

//Integer Constants
#define kInitialAddressField 0
#define kAddressByteSize 2
#define kCheckSumSize 1
#define kMaxSRecordSize 50
#define kMaxASMRecordSize 90
#define kMaxLineSize 16

//Switch Constants
#define kOutputSwitch "-o"
#define kInputSwitch "-i"
#define kSrecSwitch "-srec"
#define kHelpSwitch "-h"

//File Extensions
#define kSrecExtension ".srec"
#define kAsmExtension ".asm"

const unsigned char* MY_NAME = "KYLEDEVON";


//Function Prototypes
int getIntFromHex(char c);
char getHexadecimalEquivalent(int i);
void clearCRLF(char* string);
int isValidSwitch(char* s);
void displayHelpFile();

int checkSum(int count, int address, const unsigned char* data, int size);
void buildSRecord(const char* recordType, int addressField, char sRecord[], const unsigned char* data, int size);
void buildAssemblyRecord(char asmRecord[], const unsigned char* data, int size);

#endif