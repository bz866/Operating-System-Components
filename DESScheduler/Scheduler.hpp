// Scheduler.hpp
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


#ifndef Scheduler_HPP
#define Scheduler_HPP

#include "Utils.hpp"
// #include "Utils.cpp"
#include "Process.hpp"
// #include "Process.cpp"
#include "EVENT.hpp"
// #include "EVENT.cpp"
//Ref: Debug: https://arstechnica.com/civis/viewtopic.php?t=918219
/*
You have to include the *.cpp file because it's the file that includes both the class heading and its member function implementations.

#include is something called a preprocessor directive. It literally takes the text and sticks it into the file at compile-time.

You can be a -real- pimp and just put your function implementations in the *.h file :P
*/

/*
NOTE: !!!!!!!!!!!!!
Avoid circlular dependency
*/



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

using namespace std;

//Global variables
extern int isVerbose; // if print support trace information
extern int QUANTUM; // For RR, PRIO, PREPRIO
extern int MAXPRIOS; // optional for PRIO PREPRIO
extern int assigned_MAXPRIOS;
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

enum SCHEDULE_ALGO {FCFS, LCFS, SRTF, RR, PRIO, PREPRIO}; //defined scheduling algorithm


class Scheduler
{
protected: // accessable in derived classes

	// static const int unsigned MAXPRIOS;

public:

	//Queue 
	//Use deque to access front and back easily to meet requirement of all Scheduling Algorithm
	//std::deque<Process*> readyQueue; // Processes in STATE_READY, from 1, 4, 5 
	std::list<Process*> readyQueue;
	// std::deque<Process*> finishedQueue; // store processes which finish their total CPU Time; with calculated output statistics
	std::list<Process*> finishedQueue;
	//the scheduler has exactly MAXPRIOS priority levels [0, MAXPRIOS-1], MAXPRIOS -1 being the highest
	//USe the concept of {an active} and {an expire} runqueue
	//Utilize independent process lists/queues of each level 
	//When the active queue is empty, active and expired are switched 
	// std::deque<Process*> prior_readyQueue[2][MAXPRIOS];
	// std::deque<Process*> *active = &prior_readyQueue[0][0];
	// std::deque<Process*> *expire = &prior_readyQueue[1][0];
	// std::deque<Process*> prior_readyQueue;
	// std::list<Process*> prior_readyQueue[2][MAXPRIOS];
	// std::list<Process*> *active = &prior_readyQueue[0][0];
	// std::list<Process*> *expire = &prior_readyQueue[1][0];
	std::list<Process*> active;
	std::list<Process*> expire;

	int finishTimeLastEvent;
	int totalCPUUtilization;
	double pctCPUUtilization;
	int totalIOUtilization;
	double pctIOUtilization;
	int totalTurnaroundTime;
	double avgTurnaroundTime;
	int totalCPUWaitingTime;
	double avgCPUWaitingTime;
	double throughoutNumProcsPerHundredTimeUnits;
	// throughoutNumProcsPerHundredTimeUnits = 0.0; // assign value in constructor is only available in C++11

	Scheduler();
	~Scheduler();

	virtual void add_to_queue(Process *p);
	virtual Process* get_next_process();
};

class FCFS: public Scheduler
{
public:

	//inheritance
	void add_to_queue(Process *p);
	Process* get_next_process();
};

class LCFS: public Scheduler
{
public:

	//inheritance
	void add_to_queue(Process *p);
	Process* get_next_process();
};

class SRTF: public Scheduler
{
public:

	//inheritance
	void add_to_queue(Process *p);
	Process* get_next_process();
};

class RR: public Scheduler
{
public:

	//inheritance
	void add_to_queue(Process *p);
	Process* get_next_process();
};

class PRIO: public Scheduler
{
public:

	//inheritance
	void add_to_queue(Process *p);
	Process* get_next_process();
};

class PREPRIO: public Scheduler
{
public:

	//inheritance
	void add_to_queue(Process *p);
	Process* get_next_process();
};

#endif