// DiscreteEventSimulator.hpp
//
//
// Header File
//
//
//
//
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


#ifndef DiscreteEventSimulator_HPP
#define DiscreteEventSimulator_HPP

#include "Utils.hpp"
#include "Scheduler.hpp"
#include "Process.hpp"
#include "EVENT.hpp"

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

//Global variables
extern int isVerbose; // if print support trace information
extern int QUANTUM; // For RR, PRIO, PREPRIO
extern int MAXPRIOS; // optional for PRIO PREPRIO
extern int DEFAULT_MAXPRIOS; //if the addition is omitted then MAXPRIOS=4 by default (page 4)
extern int DEFAULT_QUANTUM; //deal with schedulers is to treat non- preemptive scheduler as preemptive with very large quantum 10K
extern int randnums_num; // total number of random numbers in randFile
extern int ofs; // the index of the ith random number 
extern std::vector<int> randvals; //

//Queue 
//Use deque to access front and back easily to meet requirement of all Scheduling Algorithm
//extern std::deque<EVENT*> eventQueue; // queue of events, push all process features as first event, then insert remained process events features by evtTimeStamp
extern std::list<EVENT*> eventQueue;
//extern std::deque<Process*> readyQueue; // 
//extern std::deque<Process*> finishedQueue;
extern Process* CURRENT_RUNNING_PROCESS;
extern int  CURRENT_TIME;

//====================================== DiscreteEventSimulator ==========================

class DiscreteEventSimulator
{
private:

	// File Stream
	std::ifstream inputFile;
	std::ifstream randFile;

public:

	//friend class Scheduler;

	SCHEDULE_ALGO schedalgo;

	bool CALL_SCHEDULER; //if there is a process is running
	bool flag_ALO_IO; // The indicator that 'at least one' process is performing IO

	Scheduler* THE_SCHEDULER; //the scheduler based on specified schedalgo
	//std::deque<EVENT*> eventQueue; // queue of events, push all process features as first event, then insert remained process events features by evtTimeStamp

	DiscreteEventSimulator();
	~DiscreteEventSimulator();

	//Pass fstream object to function; Ref: http://www.cplusplus.com/forum/beginner/84043/
	// DiscreteEventSimulator(std::ifstream &inputFile, std::ifstream &randFile, SCHEDULE_ALGO schedalgo); // Constructor with values 
	DiscreteEventSimulator(string inputFilePath_, string randFilePath_, SCHEDULE_ALGO schedalgo_); // Constructor with values 

	//std::deque<EVENT*> get_eventQueue();
	//void assign_eventQueue(std::deque<EVENT*>);

	void Simulation(); //Simulation all processes based on 

	void collectSummaryInformation(); // calculate summary information

	void printOutputInformation(); //print summary information 
};

#endif


