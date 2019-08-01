// Scheduler.hpp
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

Use OOP(inheritance) to implement 

one interface for different scheduling algorithms

*/
//
//


#include "Scheduler.hpp"

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

// const unsigned int Scheduler::MAXPRIOS = assigned_MAXPRIOS; 

Scheduler::Scheduler()
{
	//Default Constructor
	finishTimeLastEvent = 0;
	totalCPUUtilization = 0;
	pctCPUUtilization = 0.0;
	totalIOUtilization = 0;
	pctIOUtilization = 0.0;
	totalTurnaroundTime = 0;
	avgTurnaroundTime = 0.0;
	totalCPUWaitingTime = 0;
	avgCPUWaitingTime = 0.0;
	throughoutNumProcsPerHundredTimeUnits = 0.0;

}

Scheduler::~Scheduler() {}

void Scheduler::add_to_queue(Process *p)
{
	// redefined in all scheduling algorithm derived classes 
	// no action in base class
	// point to objects of derived classes
}

Process* Scheduler::get_next_process()
{
	// redefined in all scheduling algorithm derived classes 
	// no action in base class
	// point to objects of derived classes
	return nullptr;
}


void FCFS::add_to_queue(Process *p) // from 1, 4
{
	if (isVerbose)
	{
		if (p->timeStampPrevState == p->AT) // CREATE 1
		{
			p->verboseInfo_CREATEtoREADY();
		}
		else // BLOCKED to READY
		{
			p->verboseInfo_BLOCKEDtoREADY();
		}
	}
	// if (p->remainCPUBurst == 0)
	// {
	// 	p->timeInPrevState = 0;
	// }

	p->timeStampPrevState += p->timeInPrevState;
	// add to readyqueue
	readyQueue.push_back(p);
}

Process* FCFS::get_next_process()
{
	//get next process from ready queue
	//FCFS // FIFO
	if (readyQueue.empty())
	{
		return nullptr;
	}
	else
	{
		Process* nextProcess = readyQueue.front();
		return nextProcess;
	}
}

void LCFS::add_to_queue(Process *p) // from 1, 4
{
	//add to ready queue
	//same order to put in, when running, pop

	if (isVerbose)
	{
		if (p->timeStampPrevState == p->AT) // CREATE 1
		{
			p->verboseInfo_CREATEtoREADY();
		}
		else // BLOCKED to READY
		{
			p->verboseInfo_BLOCKEDtoREADY();
		}
	}

	// if (p->remainCPUBurst == 0)
	// {
	// 	p->timeInPrevState = 0;
	// }

	p->timeStampPrevState += p->timeInPrevState;
	readyQueue.push_front(p);
}

Process* LCFS::get_next_process()
{
	//get next process from ready queue
	// LCFS //LIFO
	if (readyQueue.empty())
	{
		return nullptr;
	}
	else 
	{
		Process* nextProcess = readyQueue.front();
		return nextProcess;
	}
}

void SRTF::add_to_queue(Process *p) // from 1, 4
{
	//add to ready queue
	//page2: SRTF: schedule is based on the shortest remaining execution time, not shortest CPU burst and is non-preemptive
	if (isVerbose)
	{
		if (p->timeStampPrevState == p->AT) // CREATE 1
		{
			p->verboseInfo_CREATEtoREADY();
		}
		else // BLOCKED to READY
		{
			p->verboseInfo_BLOCKEDtoREADY();
		}
	}

	if (readyQueue.empty())
	{
		readyQueue.push_back(p);
		return;
	}

	std::list<Process*>::iterator iter = readyQueue.begin();
	for (; iter != readyQueue.end(); iter++)
	{
		if ((*iter)->remainExecTime <= p->remainExecTime)
		{
			continue;
		}
		else
		{
			readyQueue.insert(iter, p);
			break;
		}
	}
	if (iter ==readyQueue.end())
		readyQueue.push_back(p);

	// std::deque<Process*> containerQ;
	// while (!readyQueue.empty())
	// {
	// 	Process* ori_proc = readyQueue.front();
	// 	if (p->remainExecTime > ori_proc->remainExecTime)
	// 	{
	// 		containerQ.push_back(ori_proc);
	// 		readyQueue.pop_front();
	// 	}
	// 	else
	// 	{
	// 		containerQ.push_back(p);
	// 	}
	// }

	// if (readyQueue.empty())
	// {
	// 	containerQ.push_back(p);
	// }
	// else 
	// {
	// 	while (!readyQueue.empty())
	// 	{
	// 		Process* ori_proc = readyQueue.front();
	// 		containerQ.push_back(ori_proc);
	// 		readyQueue.pop_front();
	// 	}
	// }
	// readyQueue = containerQ; // sorted readyQueue by ascending remain execution time
	// containerQ.clear(); // clear content to save storage
}

Process* SRTF::get_next_process()
{
	//get next process from readyqueue
	//readyqueue is sorted during adding in the order of ascending remaining execution time
	if (readyQueue.empty())
	{
		return nullptr;
	} 
	else
	{
		Process* nextProcess = readyQueue.front();
		return nextProcess;
	}
}

void RR::add_to_queue(Process *p) // from 1, 4, 5
{
	// add to ready queue
	// In RR, process to Process to STATE_READY can come from transition 4 & 5
	// 4: from STATE_BLOCKEd; 5: from STATE_RUNNING (preemptive)
	// 4: the previous CPU Burst exhausted, remainCPUBurst == 0
	// 4: IOs are independent from each other, i.e. they can commensurate concurrently without affecting each other’s IO burst time.
	// 5: Preemptive; the previous CPU Burst was not yet preemptive, remainCPUBurst != 0
	// 5: CPU RUNNING cannot commensurate concurrently

	if (isVerbose)
	{
		if (p->timeStampPrevState == p->AT) // CREATE 1
		{
			p->verboseInfo_CREATEtoREADY();
		}
		else if (p->remainCPUBurst == 0)// BLOCKED to READY
		{
			p->verboseInfo_BLOCKEDtoREADY();
		}
		else // PREEMPTION, RUNNING to READY
		{
			p->verboseInfo_RUNNINGtoREADY();
		}
	}

	if (p->remainCPUBurst == 0) //from STATE_BLOCKED & STATE_CREATED
	{
		p->timeInPrevState = 0; //independent
	}
	// from STATE_RUNNING pass 
	p->timeStampPrevState += p->timeInPrevState;
	readyQueue.push_back(p);
}

Process* RR::get_next_process()
{
	//get next process from readyqueue
	if (readyQueue.empty())
	{
		return nullptr;
	}
	else
	{
		Process* nextProcess = readyQueue.front();
		return nextProcess;
	}
}

void PRIO::add_to_queue(Process *p)
{
	// add to ready queue by dynamic priority
	// possible transitions 1, 4 , 5
	// 1: from STATE_READY 4: from STATE_BLOCKED; 5: from STATE_RUNNING
	if (p->timeStampPrevState + p->timeInPrevState == p->AT) // from STATE_CREATE p->timeStampPrevState + p->timeInPrevState == p->AT
	{
		if (isVerbose) p->verboseInfo_CREATEtoREADY();
		p->timeStampPrevState += p->timeInPrevState;

		std::list<Process*>::iterator iter = active.begin();
		for(; iter != active.end(); iter++)
		{
			if ((*iter)->dynamic_priority >= p->dynamic_priority)
			{
				active.insert(iter, p);
				break;
			}
		}
		if(iter == active.end())
		{
			active.push_back(p);
		}
	}
	else if (p->remainCPUBurst == 0) // from STATE_BLOCKED
	{
		if (isVerbose) p->verboseInfo_BLOCKEDtoREADY();

		p->dynamic_priority = p->static_priority - 1; //When a process returns from I/O its dynamic priority is reset (to (static_priority-1).
		p->timeInPrevState = 0;
		std::list<Process*>::iterator iter = active.begin();
		for(; iter != active.end(); iter++)
		{
			if ((*iter)->dynamic_priority >= p->dynamic_priority)
			{
				active.insert(iter, p);
				break;
			}
		}
		if(iter == active.end())
		{
			active.push_back(p);
		}
	}
	//else if (p->remainCPUBurst != 0 && p->timeStampPrevState + p->timeInPrevState > p->AT) // from STATE_RUNNING, preemptive
	else
	{
		if (isVerbose) p->verboseInfo_RUNNINGtoREADY();

		p->timeInPrevState = QUANTUM;
		p->dynamic_priority -= 1; // With every QUANTUM expiration the dynamic priority decreases by one. 
		if (p->dynamic_priority == -1) // When “-1” is reached the prio is reset to (static_priority-1).
		{
			p->dynamic_priority = p->static_priority - 1; //reset
			std::list<Process*>::iterator iter = expire.begin();
			for(; iter != expire.end(); iter++)
			{
				if ((*iter)->dynamic_priority >= p->dynamic_priority)
				{
					expire.insert(iter, p);
					break;
				}
			}
			if (iter == expire.end())
			{
				expire.push_back(p);
			}
		}
		else 
		{
			std::list<Process*>::iterator iter = active.begin();
			for(; iter != active.end(); iter++)
			{
				if ((*iter)->dynamic_priority >= p->dynamic_priority)
				{	
					active.insert(iter, p);
					break;
				}
			}
			if (iter == active.end())
			{
				active.push_back(p);
			}
		}

	}
}

Process* PRIO::get_next_process()
{
	if (active.empty()) //When the active queue is empty, active and expired are switched
	{
		//switch
		// std::deque<Process*> *temp;
		std::list<Process*> temp;
		temp = this->active;
		this->active = this->expire;
		this->expire = temp;
		// temp = nullptr; // clear()
		temp.clear();
	}

	if (!active.empty())
	{
		Process* nextProcess = active.back();
		return nextProcess;
	}

 	return nullptr;
}

void PREPRIO::add_to_queue(Process *p)
{
	// add to ready queue by dynamic priority
	// possible transitions 1, 4 , 5
	// 1: from STATE_READY 4: from STATE_BLOCKED; 5: from STATE_RUNNING
	// When a process arrives at the ready queue with a higher priority than the running process, the new
	// one PREEMPTS the running process
	// Need extra part to alternate the running process with the highest priority process 
	if (p->timeStampPrevState + p->timeInPrevState == p->AT) // from STATE_CREATE // p->timeStampPrevState + p->timeInPrevState == p->AT
	{
		if (isVerbose) p->verboseInfo_CREATEtoREADY();

		p->timeStampPrevState += p->timeInPrevState;

		if (CURRENT_RUNNING_PROCESS != nullptr &&
			p->dynamic_priority > CURRENT_RUNNING_PROCESS->dynamic_priority &&
			p->timeStampPrevState + p->timeInPrevState <= CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState)
		{
			//Preemption
			//remove the future event for the currently running process
			//Two pointers to iterate the event queue
			//std::deque<EVENT*> temp_eventQueue = DES.get_eventQueue();
			std::list<EVENT*>::iterator currevt = eventQueue.begin();
			std::list<EVENT*>::iterator nextevt = eventQueue.end();
			TRANSITION_TYPE currevt_transition;
			for (; currevt != eventQueue.end(); currevt = nextevt)
			{
				nextevt = ++currevt;
				currevt--;
				if ((*currevt)->evtProcess->processID == CURRENT_RUNNING_PROCESS->processID)
				{
					currevt_transition = (*currevt)->transition;
					eventQueue.erase(currevt);
					break;
				}
			}
			//add a preemtion event for the current time stamp
			// switch((*currevt)->transition)
			if (currevt_transition == TRANS_TO_BLOCK)
				CURRENT_RUNNING_PROCESS->remainExecTime -= p->timeStampPrevState + p->timeInPrevState - CURRENT_RUNNING_PROCESS->timeStampPrevState;
			else
				CURRENT_RUNNING_PROCESS->remainExecTime += CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState - (p->timeStampPrevState + p->timeInPrevState);
				// case TRANS_TO_PREEMPT:
				// 	CURRENT_RUNNING_PROCESS->remainExecTime += CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState - (p->timeStampPrevState + p->timeInPrevState);
				// case TRANS_TO_READY:
				// 	CURRENT_RUNNING_PROCESS->remainExecTime += p->timeStampPrevState + p->timeInPrevState - CURRENT_RUNNING_PROCESS->timeStampPrevState;
				// default:
				// 	std::cerr << "Pending event can only be a BLOCK or a PREEMPT event" << endl;
				// 	abort();
			
			CURRENT_RUNNING_PROCESS->remainCPUBurst += CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState - (p->timeStampPrevState + p->timeInPrevState);
			CURRENT_RUNNING_PROCESS->timeInPrevState -= CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState - (p->timeStampPrevState + p->timeInPrevState);

			put_event_to_eventQueue(CURRENT_RUNNING_PROCESS, p->timeStampPrevState + p->timeInPrevState, TRANS_TO_RUN);
		}
		std::list<Process*>::iterator iter = active.begin();
		for(; iter != active.end(); iter++)
		{
			if ((*iter)->dynamic_priority >= p->dynamic_priority)
			{
				active.insert(iter, p);
				break;
			}
		}
		if (iter == active.end())
		{
			active.push_back(p);
		}
	}
	else if(p->remainCPUBurst == 0)
	{
		// Preemption only if 
		//1) the unblocking process's dynamic_priority is higher than the currently running process dynamic_prioirity
		//2) the currently running process does not have an event pending for the same time stamp
		//Non-preemption; event pending exists
		//|-------current running-------|>>>>>>>>event pending>>>>>>>>
		//********|>>>>>>>>unblocking process>>>>>>>>>>>>>>>>>>>>>>>>>>
		// We dont force a preemption at this point as the pending event will be picked up before the scheduler is called
		//
		//Preemption; event pending doesn't exists
		//|-------current running-------|>>>>>>>>event pending>>>>>>>>
		//********|>>>>>unclocking>>|OR-| (<=)
		// Preemption happens, you have to remove the future event for the currently running process and add a preemtion
		// event for the current time stamp(ensure that the event is properly ordered in the event Q)
		if (isVerbose) p->verboseInfo_BLOCKEDtoREADY();

		p->dynamic_priority = p->static_priority - 1;
		if (CURRENT_RUNNING_PROCESS != nullptr &&
			p->dynamic_priority > CURRENT_RUNNING_PROCESS->dynamic_priority &&
			p->timeStampPrevState + p->timeInPrevState <= CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState)
		{
			//Preemption
			//remove the future event for the currently running process
			//Two pointers to iterate the event queue
			//std::deque<EVENT*> temp_eventQueue = DES.get_eventQueue();
			std::list<EVENT*>::iterator currevt = eventQueue.begin();
			std::list<EVENT*>::iterator nextevt = eventQueue.end();
			TRANSITION_TYPE currevt_transition;
			for (; currevt != eventQueue.end(); currevt = nextevt)
			{
				nextevt = ++currevt;
				currevt--;
				if ((*currevt)->evtProcess->processID == CURRENT_RUNNING_PROCESS->processID)
				{
					currevt_transition = (*currevt)->transition;
					eventQueue.erase(currevt);
					break;
				}
			}
			//add a preemtion event for the current time stamp
			if  (currevt_transition == TRANS_TO_BLOCK)
				CURRENT_RUNNING_PROCESS->remainExecTime -= p->timeStampPrevState + p->timeInPrevState - CURRENT_RUNNING_PROCESS->timeStampPrevState;
			else
				CURRENT_RUNNING_PROCESS->remainExecTime += CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState - (p->timeStampPrevState + p->timeInPrevState);

			CURRENT_RUNNING_PROCESS->remainCPUBurst += CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState - (p->timeStampPrevState + p->timeInPrevState);
			CURRENT_RUNNING_PROCESS->timeInPrevState -= CURRENT_RUNNING_PROCESS->timeStampPrevState + CURRENT_RUNNING_PROCESS->timeInPrevState - (p->timeStampPrevState + p->timeInPrevState);

			put_event_to_eventQueue(CURRENT_RUNNING_PROCESS, p->timeStampPrevState + p->timeInPrevState, TRANS_TO_RUN);
		}

		p->timeInPrevState = 0;
		std::list<Process*>::iterator iter = active.begin();
		for(; iter != active.end(); iter++)
		{
			if ((*iter)->dynamic_priority >= p->dynamic_priority)
			{	
				active.insert(iter, p);
				break;
			}
		}
		if (iter == active.end())
		{
			active.push_back(p);
		}
	}
	else
	{
		if (isVerbose) p->verboseInfo_RUNNINGtoREADY();

		p->timeInPrevState = QUANTUM;
		p->dynamic_priority -= 1; // With every QUANTUM expiration the dynamic priority decreases by one. 
		if (p->dynamic_priority == -1) // When “-1” is reached the prio is reset to (static_priority-1).
		{
			p->dynamic_priority = p->static_priority - 1; //reset
			std::list<Process*>::iterator iter = expire.begin();
			for(; iter != expire.end(); iter++)
			{
				if ((*iter)->dynamic_priority >= p->dynamic_priority)
				{
					expire.insert(iter, p);
					break;
				}
			}
			if (iter == expire.end())
			{
				expire.push_back(p);
			}
		}
		else
		{
			std::list<Process*>::iterator iter = active.begin();
			for(; iter != active.end(); iter++)
			{
				if ((*iter)->dynamic_priority >= p->dynamic_priority)
				{
					active.insert(iter, p);
					break;
				}
			}
			if (iter == active.end())
			{
				active.push_back(p);
			}
		}
	}
}

Process* PREPRIO::get_next_process()
{
	if (active.empty()) //When the active queue is empty, active and expired are switched
	{
		//switch
		// std::deque<Process*> *temp;
		std::list<Process*> temp;
		temp = this->active;
		this->active = this->expire;
		this->expire = temp;
		temp.clear();
	}

	if (!active.empty())
	{
		Process* nextProcess = active.back();
		return nextProcess;
	}

	return nullptr;
}





