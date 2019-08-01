// Process.hpp
//
//
// Header File
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



#ifndef Process_HPP
#define Process_HPP

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
#include <vector>
#include <list>

using namespace std;

//Global variables
extern int isVerbose; // if print support trace information
extern int QUANTUM; // For RR, PRIO, PREPRIO
extern int MAXPRIOS; // optional for PRIO PREPRIO
extern int DEFAULT_MAXPRIOS; //if the addition is omitted then MAXPRIOS=4 by default (page 4)
extern int DEFAULT_QUANTUM; //deal with schedulers is to treat non- preemptive scheduler as preemptive with very large quantum 10K
extern int randnums_num; // total number of random numbers in randFile
extern int ofs; // the index of the ith random number 
extern std::vector<int> randvals; //

extern int  CURRENT_TIME;


enum STATE_TYPE {STATE_CREATE, STATE_READY, STATE_RUNNING, STATE_BLOCKED}; //define types of states

//========================== Process =============================

class Process
{
// private:
// 	static int processCounter = 0; // the counter of processes, 0-counting base

public:

	static int processCounter; // the counter of processes, 0-counting base

	// features, get from the inputFile
	int processID; // the index of ith process, increase simutaniously with the processCounter
	int AT; //arrival time
	int TC; // Total CPU time
	int CB; // CPU burst 
	int IO; // I/O burst
	// meaningful for PRIO/PREPRIO cases ONLY
	int static_priority; //Priority(PRIO), distinguished with PRIO scheduling algorithm, random number in [1, maxprio]

	//dynamic features
	//STATE_TYPE state; // current state of the procerss
	STATE_TYPE state;
	int dynamic_priority; //defined between [ 0 .. (static_priority-1) ]. set as static_priority-1, can be updated during processes running
	int timeStampPrevState; //  the last time a state transition was done 
	int timeInPrevState; // Time spent in the previous state
	int remainExecTime; // remain Execution Time 
	int remainCPUBurst; // remain CPU burst
	
	// Output Statistics
	int FT; //Finishing time
	int TT; //Turnaround time (finishing time - AT)
	int IT; //I/O Time(Time in blocked state) // use to calculate the CW
	int IT_ALO; // IO utilization; ALO for 'At Least One'; (i.e. percentage (0.0 – 100.0) of time at least one process is performing IO
	int CW; //CPU Waiting time

	Process(); //Default 
	Process(int arrivalTime, int totalCPUTime, int CPUBurst, int IOBurst, int static_priority); //Constructor by Value 	
	~Process(); //Default 

	void verboseInfo_CREATEtoREADY(); //STATE_CREATED -> STATEREADY, transition 1
	void verboseInfo_READYtoRUNNING(); //STATE_READY -> RUNNING, transition 2
	void verboseInfo_RUNNINGtoBLOCKED(int randIO); //STATE_RUNNING -> STATE_BLOCKED, transition 3
	void verboseInfo_BLOCKEDtoREADY(); //STATE_BLOCKED -> STATE_READY, transition 4
	void verboseInfo_RUNNINGtoREADY(); //STATE_RUNNING -> STATE_READY, transition 5
	void verboseInfo_ProcessAllDone(); //The process is finished, total CPU time exhausted

	void print(); // print out features and stats in desired format
};



#endif