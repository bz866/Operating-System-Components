// Process.cpp
//
//
//
//
//
//
// Lab 2 exercise Operating System 2019 Summer
//
// Author: Binqian Zeng
//
// start: 07/06/2019
//
//
/*



*/
//
//


#include "Process.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <istream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <string>
#include <cstring>
#include <deque>
#include <list>


using namespace std;

int Process::processCounter = 0;

Process::Process() {}

Process::~Process() {}

Process::Process(int arrivalTime, int totalCPUTime, int CPUBurst, int IOBurst, int static_priority_ori) //Constructor by values
{
	processID = processCounter++;
	//features
	AT = arrivalTime;
	TC = totalCPUTime;
	CB = CPUBurst;
	IO = IOBurst;
	static_priority = static_priority_ori;
	//dynamic features
	dynamic_priority = static_priority - 1;
	timeStampPrevState = AT;
	timeInPrevState = 0;
	remainExecTime = TC;
	remainCPUBurst = 0;
	state = STATE_READY;
	//Output stats
	FT = 0;
	TT = 0;
	IT = 0;
	IT_ALO = 0;
	CW = 0;
}

void Process::verboseInfo_CREATEtoREADY() //STATE_CREATED -> STATEREADY, transition 1
{
	cout << this->timeInPrevState + this->timeStampPrevState << " " << this->processID << " " << this->timeInPrevState << ": " 
	<< "CREATED -> READY" << endl;
}

void Process::verboseInfo_READYtoRUNNING() //STATE_READY -> RUNNING, transition 2
{
	cout << CURRENT_TIME << " " << this->processID << " " << this->timeInPrevState << ": "
	<< "READY -> RUNNG" << " " << "cb=" << this->remainCPUBurst << " " << "rem=" << this->remainExecTime << " "
	<< "prio=" << this->dynamic_priority << endl;
}

void Process::verboseInfo_RUNNINGtoBLOCKED(int randIO) //STATE_RUNNING -> STATE_BLOCKED, transition 3
{
	cout << CURRENT_TIME << " " << this->processID << " " << this->timeInPrevState << ": " 
	<< "RUNNG -> BLOCK" << " " << "ib=" << randIO << " " << "rem=" << this->remainExecTime << endl;
}

void Process::verboseInfo_BLOCKEDtoREADY() //STATE_BLOCKED -> STATE_READY, transition 4
{
	cout << this->timeInPrevState + this->timeStampPrevState << " " << this->processID << " " << this->timeInPrevState << ": " 
	<< "BLOCK -> READY" << endl;
}

void Process::verboseInfo_RUNNINGtoREADY() //STATE_RUNNING -> STATE_READY, transition 5
{
	cout << this->timeInPrevState + this->timeStampPrevState << " " << this->processID << " " << this->timeInPrevState << ": " 
	<< "RUNNG -> READY" << " " << "cb=" << this->remainCPUBurst << " " << "rem=" << this->remainExecTime << " " << "prio=" << this->dynamic_priority << endl;
}

void Process::verboseInfo_ProcessAllDone() //The process is finished, total CPU time exhausted
{
	cout << CURRENT_TIME << " " << this->processID << " " << this->timeInPrevState << ": Done" << endl;
}

void Process::print()
{
	//print features and statistics information as expected formatting (including precision)
	printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
			this->processID, // :
			this->AT, 
			this->TC, 
			this->CB,
			this->IO, 
			this->static_priority, // |
			this->FT, 
			this->TT, 
			this->IT,
			this->CW
	);

}