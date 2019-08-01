// DiscreteEventSimulator.cpp
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


#include "DiscreteEventSimulator.hpp"

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

DiscreteEventSimulator::DiscreteEventSimulator() {}

DiscreteEventSimulator::~DiscreteEventSimulator() {}

// DiscreteEventSimulator::DiscreteEventSimulator(std::ifstream &inputFile, std::ifstream &randFile, SCHEDULE_ALGO schedalgo)
DiscreteEventSimulator::DiscreteEventSimulator(string inputFilePath_, string randFilePath_, SCHEDULE_ALGO schedalgo_)
{
	//Constructor with values 
	// inputFile = inputFile;
	// randFile = randFile;
	schedalgo = schedalgo_;
	inputFile.open(inputFilePath_);
	randFile.open(randFilePath_);

	//====================== Define Scheduler based on specified schedalgo ========================
	switch(schedalgo)
	{
		// The 'new' operatior returns a unique pointer
		//Ref: https://docs.microsoft.com/en-us/cpp/cpp/new-operator-cpp?view=vs-2019
		// When class names conflict with enum type names, add class type before initializing a new class 
		// new class CLASSNAME()
		// instead of 
		// new CLASSNAME()
		case FCFS:
		{
			THE_SCHEDULER = new class FCFS();
			break;
		}
		case LCFS:
		{
			THE_SCHEDULER = new class LCFS();
			break;
		}
		case SRTF:
		{
			THE_SCHEDULER = new class SRTF();
			break;
		}
		case RR:
		{
			THE_SCHEDULER = new class RR();
			break;
		}
		case PRIO:
		{
			THE_SCHEDULER = new class PRIO();
			break;
		}
		case PREPRIO:
		{
			THE_SCHEDULER = new class PREPRIO();
			break;
		}
		default:
		{
			std::cerr << "Invalid scheduling algorithm type" << endl;
			abort();
		}
	}

	//====================== Extract all random numbers from the randFile ==========================
	if (!randFile.good())
	{
		std::cerr << "Random Number file cannot be opend. " << endl;
		abort();
	}
	randFile >> randnums_num; // First line of the file
	randvals.reserve(randnums_num); // reserve necessary spaces
	int lineint;
	while (randFile >> lineint)
	{	
		// randvals[i] = lineint;
		randvals.push_back(lineint);
		// randFile >> randvals[i]; //append one random number at a time
	}
	randFile.close();


	//===================== Parse inputFile and get all processes ==================================
	string line;
	std::vector<int> processFeaturesVec; // AT TC CB IO
	processFeaturesVec.reserve(4);
	if (!inputFile.good())
	{
		std::cerr << "Input file cannot be opend. " << endl;
		abort();
	}
	int i = 0;
	while ( getline(inputFile, line, '\n'))
	{
		std::istringstream linestream(line);  // assign a line as stringstream
		do // parse integers to AT TC CB IO
		{	
			for (int featureNum; linestream >> featureNum;)
			{
				processFeaturesVec.push_back(featureNum); // insert to vec one by one 
			}
			if (!linestream.eof() && linestream.fail()) // consume and discard tokens from the stream
			{
				linestream.clear(); //set error flags
				string token;
				linestream >> token; 
				cout << token << " " << "appear in the inputFile. Unexpected." << endl;
				abort();
			}	
		}
		while (!linestream.eof());

		int static_priority = myrandom(MAXPRIOS); // get static_priority
		processFeaturesVec.push_back(static_priority);

		// unpack features vector as arguments to initialize a process // STATE_CREATE
		Process* proc = new Process(processFeaturesVec[0], processFeaturesVec[1], processFeaturesVec[2], processFeaturesVec[3], processFeaturesVec[4]);
		// initialize event directly to STATE_READY (Process* proc, int evtTimeStamp, TRANSITION_TYPE trans_type)
		EVENT* evt = new EVENT(proc, processFeaturesVec[0], TRANS_TO_READY);
		put_event_to_eventQueue(evt); // push back to eventQueue
		processFeaturesVec.clear();
	}
}

void DiscreteEventSimulator::Simulation()
{
	EVENT* evt;
	flag_ALO_IO = false; // The indicator that 'at least one' process is performing IO
	CALL_SCHEDULER = false;
	int CNTEVT = -1;
	while( (evt = get_event_from_eventQueue()) )
	{	
		// cout << "EVENT SIZE: "<< eventQueue.size() << endl;
		// CALL_SCHEDULER = false;
		flag_ALO_IO = false;
		CNTEVT++;
		Process* proc = evt->evtProcess; //this is the process the event works on
		CURRENT_TIME = evt->evtTimeStamp;
		// evt->evtProcess->timeInPrevState = CURRENT_TIME - proc->timeStampPrevState;
		// cout << "CURRENT TIME " << CURRENT_TIME << endl;
		if(CURRENT_RUNNING_PROCESS != nullptr && 
			CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState <= CURRENT_TIME)
		{
			// cout << "UPdate no running" << CURRENT_RUNNING_PROCESS->timeStampPrevState << " " <<
			// CURRENT_RUNNING_PROCESS->timeInPrevState << " " <<
			 // CURRENT_TIME << endl;
			CURRENT_RUNNING_PROCESS = nullptr;
		}
		if (CURRENT_TIME > proc->timeStampPrevState)
		{
			proc->timeInPrevState = CURRENT_TIME - proc->timeStampPrevState;
			
			// cout << "COCURRENCY : " << proc->processID << " " << CURRENT_TIME << " " 
			// << proc->timeStampPrevState << " " << proc->timeInPrevState << endl;
		}
		else
		{
			proc->timeInPrevState = 0;
		}

		switch(evt->transition)  // which state to transition to?
		{
			case TRANS_TO_READY:
			{
				// cout << "TRANS_TO_READY" << endl;
				// take the incoming processes and put into ready queue
				// must come from BLOCKED or from PREEMPTION
				// must add to ready queue
				// processes from STATE_CREATE are put into the ready queue during parsing the inputFile
				// incoming processes to the STATE_READY/ready queue can only from 4(from STATE_BLOCKED) or 5(from PREEMPTION)
				THE_SCHEDULER->add_to_queue(evt->evtProcess);
				CALL_SCHEDULER = true; // conditional on whether something is run 
				break;
			}

			case TRANS_TO_RUN:
			{
				// cout << "TRANS_TO_RUN" << endl;
				// create event for either preemption or blocking
				// processes go to STATE_RUNNING can only go out through 3(to STATE_BLOCKED) or 5(through PREEMPTION to STATE_READY)
				put_event_to_eventQueue(evt->evtProcess, CURRENT_TIME, TRANS_TO_READY);
				break;
			}

			case TRANS_TO_BLOCK:
			{
				// cout << "TRANS_TO_BLOCK" << endl;
				//create an event for when process becomes CALL_SCHEDULER = true;
				if (proc->timeInPrevState != proc->remainExecTime) //enters into a period of IO(transition 3), it has not yet reached its toal CPU time
				{
					proc->remainExecTime -= proc->timeInPrevState; //update, end of the last state
					proc->timeStampPrevState = CURRENT_TIME; // begin new state
					int randIO = myrandom(proc->IO); //IO burst is a random number between [1, IO]
					proc->IT += randIO; //IT Time gets increament during each IO, BLOCKED
					if (isVerbose) proc->verboseInfo_RUNNINGtoBLOCKED(randIO);
					EVENT* newevt = new EVENT(proc, CURRENT_TIME + randIO, TRANS_TO_READY);
					put_event_to_eventQueue(newevt);
					CALL_SCHEDULER = true;

					// Accumulate the time that 'AT Least One Process is performing IO/in BLOCKED'
					if (eventQueue.size() == 1)
					{
						evt->evtProcess->IT_ALO += randIO;
						CALL_SCHEDULER = true;
						break;
					}
					else
					{
						std::list<EVENT*>::iterator iter = eventQueue.end();
						iter--; // DEBUG: .end() is end maker, not an element, use directly to access member variable will raise Segmentation fault (core dumped)
						for(; iter != eventQueue.begin() && (*iter)->evtProcess != proc; iter--)
						{
							if ((*iter)->transition == TRANS_TO_READY && 
								(*iter)->evtTimeStamp > (*iter)->evtProcess->AT) // IO event in event queue
							{	
								flag_ALO_IO = true;
								CALL_SCHEDULER = true;
								break;
							}
						}
						if(!flag_ALO_IO)
						{
							if (iter == eventQueue.begin())
							{
								proc->IT_ALO += randIO;
								flag_ALO_IO = true;
								CALL_SCHEDULER = true;
								break;
							}
							iter--;
							for(; iter != eventQueue.begin(); iter--)
							{
								if ((*iter)->transition == TRANS_TO_READY && 
									(*iter)->evtTimeStamp > (*iter)->evtProcess->AT &&
									(*iter)->evtTimeStamp > CURRENT_TIME) // IO event in event queue
								{
									proc->IT_ALO += CURRENT_TIME + randIO - (*iter)->evtTimeStamp;
									flag_ALO_IO = true;
									CALL_SCHEDULER = true;
									break; 
								}
							}
							if(iter == eventQueue.begin())
							{
								if ((*iter)->transition == TRANS_TO_READY && 
									(*iter)->evtTimeStamp > (*iter)->evtProcess->AT &&
									(*iter)->evtTimeStamp > CURRENT_TIME) // IO event in event queue
								{
									proc->IT_ALO += CURRENT_TIME + randIO - (*iter)->evtTimeStamp;
									CALL_SCHEDULER = true;
									break; 
								}
								else
								{
									proc->IT_ALO += randIO;
									CALL_SCHEDULER = true;
									break;
								}
							}
						}
					}
				}
				else //total CPU time reached, process finished 
				{
					if (isVerbose) proc->verboseInfo_ProcessAllDone();
					// Calculate Output Statistics for the finished process
					proc->FT = CURRENT_TIME;
					proc->TT = proc->FT - proc->AT;
					// proc->IT gets increment during each IO
					proc->CW = proc->TT - proc->TC -proc->IT; //STATE CREATE 0 time, READY + BLOCKED + RUNNING = Turnaround Time

					//insert the finished processes to the finished queue by processID
					std::list<Process*>::iterator iter = THE_SCHEDULER->finishedQueue.begin();
					for(; iter != THE_SCHEDULER->finishedQueue.end(); iter++)
					{
						if ((*iter)->processID >= proc->processID)
						{
							THE_SCHEDULER->finishedQueue.insert(iter, proc); //insert back to begin() after insertion
							CALL_SCHEDULER = true;
							break;
						} 
					}
					if (iter == THE_SCHEDULER->finishedQueue.end())
					{
						THE_SCHEDULER->finishedQueue.push_back(proc);
						CALL_SCHEDULER = true;
						break;
					}
				}	
				CALL_SCHEDULER = true;
				break;
			}

			case TRANS_TO_PREEMPT:
			{
				// cout << "TRANS_TO_PREEMPT" << endl;
				// add to runqueue (no event is generated) 
				THE_SCHEDULER->add_to_queue(proc);
				CALL_SCHEDULER = true;
				break;
			}
		}
		//remove current event object from Memory delete evt;
		evt = nullptr;

		if (CALL_SCHEDULER)
		{	
			// cout << "FORMAL SIMULATION" << endl;
			// cout << CNTEVT << " : " << proc->processID << " " << proc->CB << endl;
			// cout << "FORMAL SIMULATE" << endl;
			// cout << "NEXT EVENT TIME: " << get_next_event_time() << "  CURRENT TIME: " << CURRENT_TIME << endl;
			if (get_next_event_time() == CURRENT_TIME)
			{	
				continue; //process next event from Event queue
			}
			CALL_SCHEDULER = false; //reset global flag
			if (CURRENT_RUNNING_PROCESS == nullptr) // no process is running, get the next process 
			{
				CURRENT_RUNNING_PROCESS = THE_SCHEDULER->get_next_process();
				if (CURRENT_RUNNING_PROCESS == nullptr)
				{
					continue; // create event to make this process runable for same time
				}

				if (CURRENT_RUNNING_PROCESS->remainCPUBurst == 0) //from STATE_BLOCKED
				{
					//previous CPU Burst exhausted // assign new CPU burst
					// cout << "New RANDCB GOT" << endl;
					// cout << "CURRENET PROCESSID " << CURRENT_RUNNING_PROCESS->processID << endl;
					int randCB = myrandom(CURRENT_RUNNING_PROCESS->CB); // random [1, CB]
					if (randCB > CURRENT_RUNNING_PROCESS->remainExecTime) //If the remaining execution time is smaller than the random number chosen, reduce the random integer number to the remaining execution time.
					{
						randCB = CURRENT_RUNNING_PROCESS->remainExecTime;
					}
					CURRENT_RUNNING_PROCESS->remainCPUBurst = randCB; //assign new remain CPU burst
				}
				//else from STATE_CREATED already have CPU Burst; from PREEMPTION previous CPU Burst not exhausted

				if (QUANTUM >= CURRENT_RUNNING_PROCESS->remainCPUBurst) // enough QUATNUM, sent to STATE_BLOCKED, exhauste remain CPU Burst
				{
					// cout << "QUANTUM SUFFICIENT" << QUANTUM << endl;
					if (isVerbose) CURRENT_RUNNING_PROCESS->verboseInfo_READYtoRUNNING();
					//RUN and update; 
					CURRENT_RUNNING_PROCESS->timeStampPrevState = CURRENT_TIME; 
					CURRENT_RUNNING_PROCESS->timeInPrevState = CURRENT_RUNNING_PROCESS->remainCPUBurst; // end of running & the last event
					// To STATE_BLOCKED // new EVENT to meet independent requirement of IO
					// cout << "RUN EVENT setup: " << CURRENT_RUNNING_PROCESS->processID << " " <<  CURRENT_RUNNING_PROCESS->remainCPUBurst << endl;

					put_event_to_eventQueue(CURRENT_RUNNING_PROCESS, CURRENT_TIME + CURRENT_RUNNING_PROCESS->remainCPUBurst, TRANS_TO_BLOCK);
					if(THE_SCHEDULER->readyQueue.empty()) //Preemptive Scheduling algo
					{
						THE_SCHEDULER->active.pop_back();
					}
					else // Preeemptive 
					{
						THE_SCHEDULER->readyQueue.pop_front();
					}
					CURRENT_RUNNING_PROCESS->remainCPUBurst = 0; //CPU burst exhausted
				}
				else // insufficient QUANTUM, sent to STATE_READY, PREEMPTION
				{
					if (isVerbose) CURRENT_RUNNING_PROCESS->verboseInfo_READYtoRUNNING();

					CURRENT_RUNNING_PROCESS->remainCPUBurst -= QUANTUM;
					CURRENT_RUNNING_PROCESS->remainExecTime -= QUANTUM;
					CURRENT_RUNNING_PROCESS->timeInPrevState = QUANTUM;
					CURRENT_RUNNING_PROCESS->timeStampPrevState = CURRENT_TIME;
					put_event_to_eventQueue(CURRENT_RUNNING_PROCESS, CURRENT_TIME + QUANTUM, TRANS_TO_PREEMPT);
					if(THE_SCHEDULER->readyQueue.empty()) //Preemptive Scheduling algo
					{
						THE_SCHEDULER->active.pop_back();
					}
					else // Preeemptive 
					{
						THE_SCHEDULER->readyQueue.pop_front();
					}
				}
			}
		}
	}
}

void DiscreteEventSimulator::collectSummaryInformation() // calculate summary information
{
	std::list<Process*>::iterator iter_proc;
	for (iter_proc = THE_SCHEDULER->finishedQueue.begin(); iter_proc != THE_SCHEDULER->finishedQueue.end(); iter_proc++)
	{	
		THE_SCHEDULER->totalCPUUtilization += (*iter_proc)->TC;
		THE_SCHEDULER->totalIOUtilization += (*iter_proc)->IT_ALO;
		// THE_SCHEDULER->totalIOUtilization += (*iter_proc)->IT;
		THE_SCHEDULER->totalTurnaroundTime += (*iter_proc)->TT;
		THE_SCHEDULER->totalCPUWaitingTime += (*iter_proc)->CW;

		if (THE_SCHEDULER->finishTimeLastEvent < (*iter_proc)->FT)
		{
			THE_SCHEDULER->finishTimeLastEvent = (*iter_proc)->FT;
		}
	}

	THE_SCHEDULER->pctCPUUtilization = (double)THE_SCHEDULER->totalCPUUtilization / THE_SCHEDULER->finishTimeLastEvent * 100;
	THE_SCHEDULER->pctIOUtilization = (double)THE_SCHEDULER->totalIOUtilization / THE_SCHEDULER->finishTimeLastEvent * 100;
	THE_SCHEDULER->avgTurnaroundTime = (double)THE_SCHEDULER->totalTurnaroundTime / Process::processCounter;
	THE_SCHEDULER->avgCPUWaitingTime = (double)THE_SCHEDULER->totalCPUWaitingTime / Process::processCounter;
	THE_SCHEDULER->throughoutNumProcsPerHundredTimeUnits = (double)Process::processCounter / THE_SCHEDULER->finishTimeLastEvent * 100;

}

void DiscreteEventSimulator::printOutputInformation() //print summary information 
{
	//Process Inforamtion 
	std::list<Process*>::iterator iter_proc;
	for (iter_proc = THE_SCHEDULER->finishedQueue.begin(); iter_proc != THE_SCHEDULER->finishedQueue.end(); iter_proc++)
	{
		(*iter_proc)->print();
	}
	//Summary Information
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
			THE_SCHEDULER->finishTimeLastEvent,
			THE_SCHEDULER->pctCPUUtilization,
			THE_SCHEDULER->pctIOUtilization,
			THE_SCHEDULER->avgTurnaroundTime,
			THE_SCHEDULER->avgCPUWaitingTime,
			THE_SCHEDULER->throughoutNumProcsPerHundredTimeUnits
	);
}



