
// Scheduler.cpp
//
//
//
//
//
//
// Lab 4 exercise Operating System 2019 Summer
//
// Author: Binqian Zeng
//
// start: 07/06/2019
//
//
/*

The schedulers to be implemented are FIFO (i), SSTF (j), LOOK (s), CLOOK (c), and FLOOK (f) 
(the letters in bracket define which parameter must be given in the –s program flag).


Ref: 

To Run:


Note:

*/
//
//

// #include "Request.hpp"
#include "Scheduler.hpp"

using namespace std;

Scheduler::Scheduler(){}

Scheduler::~Scheduler(){}

IOOperationRequest* Scheduler::get_next_IOrequest(int head)
{
	std::cerr << "Scheduler must be specified with scheduling algorithm." << endl;
	abort();
	return nullptr;
}

void Scheduler::add_request(IOOperationRequest* newRequest){}

IOOperationRequest* FIFO::get_next_IOrequest(int head)
{
	if (!IOQueue.empty())
	{
		IOOperationRequest* nextRequest = IOQueue.front();
		IOQueue.erase(IOQueue.begin());
		return nextRequest;
	}
	else // empty
	{
		return nullptr;
	}
}

void FIFO::add_request(IOOperationRequest* newRequest)
{
	this->IOQueue.push_back(newRequest);
}

IOOperationRequest* SSTF::get_next_IOrequest(int head)
{
	if (!IOQueue.empty())
	{
		IOOperationRequest* nextRequest = IOQueue.front();
		int indexSST = 0;
		for (int i = 0; i < IOQueue.size(); i++)
		{
			if(abs(head - IOQueue[i]->track) < abs(head - nextRequest->track)) // get the closest request
			{
				nextRequest = IOQueue[i];
				indexSST = i;
			}
		}
		IOQueue.erase(IOQueue.begin() + indexSST);
		return nextRequest;
	}
	else //empty 
	{
		return nullptr;
	}
}

void SSTF::add_request(IOOperationRequest* newRequest)
{
	this->IOQueue.push_back(newRequest);
}

LOOK::LOOK()
{
	this->postiveDirection = true;
	this->reverseDirection = false;
}

IOOperationRequest* LOOK::get_next_IOrequest(int head)
{
	if (!IOQueue.empty())
	{
		IOOperationRequest* nextRequest = IOQueue.front();
		int indexSST = 0;
		// int SST = IOQueue.front()->track - head;
		int SST = INT_MAX;
		bool closerOneFound = false;
		if (postiveDirection && !reverseDirection) //search start in postive direction
		{
			for (int i = 0; i < IOQueue.size(); i++)
			{
				if (0 <= IOQueue[i]->track - head && IOQueue[i]->track - head < SST) // found closer one
				{
					SST = IOQueue[i]->track - head;
					indexSST = i;
					nextRequest = IOQueue[i];
					closerOneFound = true;
				}
			}
		}
		else if (reverseDirection && !postiveDirection) // search start in reverse direction
		{
			for (int i = 0; i < IOQueue.size(); i++)
			{
				if (0 >= IOQueue[i]->track - head && abs(IOQueue[i]->track - head) < SST) // found closer one 
				{
					SST = abs(IOQueue[i]->track - head);
					indexSST = i;
					nextRequest = IOQueue[i];
					closerOneFound = true;
				}
			}
		}
		else
		{
			std::cerr << "postiveDirection and reverseDirection must be [true/false] or [false/true]." << endl;
			abort();
		}
		if(!closerOneFound) // if not found closer one search in another direction
		{
			// swap searching direction
			postiveDirection = !postiveDirection;
			reverseDirection = !reverseDirection;

			if(reverseDirection && !postiveDirection) // start in postive direction then search in reverse direction
			{
				for (int i = 0; i < IOQueue.size(); i++)
				{
					if (0 >= IOQueue[i]->track - head && abs(IOQueue[i]->track - head) < SST) // found closer one 
					{
						SST = abs(IOQueue[i]->track - head);
						indexSST = i;
						nextRequest = IOQueue[i];
						closerOneFound = true;
					}
				}
			}
			else if (postiveDirection && !reverseDirection) // start in reverse direction then search in postive direction
			{
				for (int i = 0; i < IOQueue.size(); i++)
				{
					if (0 <= IOQueue[i]->track - head && IOQueue[i]->track - head < SST) // found closer one
					{
						SST = IOQueue[i]->track - head;
						indexSST = i;
						nextRequest = IOQueue[i];
						closerOneFound = true;
					}
				}
			}
			else
			{
				std::cerr << "postiveDirection and reverseDirection must be [true/false] or [false/true]." << endl;
				abort();
			}
		}
		IOQueue.erase(IOQueue.begin() + indexSST);
		return nextRequest;
	}
	else // empty
	{
		return nullptr;
	}
}

void LOOK::add_request(IOOperationRequest* newRequest)
{
	this->IOQueue.push_back(newRequest);
}

IOOperationRequest* CLOOK::get_next_IOrequest(int head)
{
	if (!IOQueue.empty())
	{
		IOOperationRequest* nextRequest = IOQueue.front();
		int indexSST = 0;
		// int SST = IOQueue.front()->track - head;
		int SST = INT_MAX;
		bool closerOneFound = false;
		for (int i = 0; i < IOQueue.size(); i++) // search in any direction
		{
			if (0 <= IOQueue[i]->track - head && IOQueue[i]->track - head < SST) // found closer one
			{
				SST = IOQueue[i]->track - head;
				indexSST = i;
				nextRequest = IOQueue[i];
				closerOneFound = true;
			}
		}
		if (!closerOneFound) // if not found closer one search in another direction
		{
			for (int i = 0; i < IOQueue.size(); i++)
			{
				if (0 >= IOQueue[i]->track - head && abs(IOQueue[i]->track - 0) < SST) // found closer one //wrap around
				{
					SST = abs(IOQueue[i]->track - 0);
					indexSST = i;
					nextRequest = IOQueue[i];
					closerOneFound = true;
				}
			}
		} // end of serach in another direction
		IOQueue.erase(IOQueue.begin() + indexSST);
		return nextRequest;
	}// 
	else // empty
	{
		return nullptr;
	}
}

void CLOOK::add_request(IOOperationRequest* newRequest)
{
	this->IOQueue.push_back(newRequest);
}

FLOOK::FLOOK()
{
	this->postiveDirection = true;
	this->reverseDirection = false;
}

IOOperationRequest* FLOOK::get_next_IOrequest(int head)
{
	if (!IOQueue.empty())
	{	
		if(activeQueue.empty()) // swap activeQueue and addOnQueue if active is empty
		{
			std::vector<IOOperationRequest*> temp;
			temp = activeQueue;
			activeQueue = addOnQueue;
			addOnQueue = temp;
		}

		if(activeQueue.empty()) // if still empty, no requests can be selected
		{
			return nullptr;
		}

		IOOperationRequest* nextRequest = activeQueue.front();
		int indexSST = 0;
		// int SST = activeQueue.front()->track - head;
		int SST = INT_MAX;
		bool closerOneFound = false;
		if (postiveDirection && !reverseDirection) // then same as SCAN(LOOK)
		{
			for (int i = 0; i < activeQueue.size(); i++)
			{
				if (0 <= activeQueue[i]->track - head && activeQueue[i]->track - head < SST)
				{
					SST = activeQueue[i]->track - head;
					indexSST = i;
					nextRequest = activeQueue[i];
					closerOneFound = true;
				}
			}
		}
		else if (reverseDirection && !postiveDirection)
		{
			for (int i = 0; i < activeQueue.size(); i++)
			{
				if (0 >= activeQueue[i]->track - head && abs(activeQueue[i]->track - head) < SST)
				{
					SST = abs(activeQueue[i]->track - head);
					indexSST = i;
					nextRequest = activeQueue[i];
					closerOneFound = true;
				}
			}
		}
		else
		{
			std::cerr << "postiveDirection and reverseDirection must be [true/false] or [false/true]." << endl;
			abort();
		}
		if (!closerOneFound)
		{
			postiveDirection = !postiveDirection;
			reverseDirection = !reverseDirection;

			if (reverseDirection && !postiveDirection)
			{
				for (int i = 0; i < activeQueue.size(); i++)
				{
					if (0 >= activeQueue[i]->track - head && abs(activeQueue[i]->track - head) < SST)
					{
						SST = abs(activeQueue[i]->track - head);
						indexSST = i;
						nextRequest = activeQueue[i];
						closerOneFound = true;
					}
				}
			}
			else if (postiveDirection && !reverseDirection)
			{
				for (int i = 0; i < activeQueue.size(); i++)
				{
					if (0 <= activeQueue[i]->track - head && activeQueue[i]->track - head < SST)
					{
						SST = activeQueue[i]->track - head;
						indexSST = i;
						nextRequest = activeQueue[i];
						closerOneFound = true;
					}
				}
			}
			else 
			{
				std::cerr << "postiveDirection and reverseDirection must be [true/false] or [false/true]." << endl;
				abort();
			}
		} // end of serach in another direction
		activeQueue.erase(activeQueue.begin() + indexSST);
		IOQueue.erase(IOQueue.begin());
		return nextRequest;
	}// end of IOQueue check
	else // IO empty
	{
		return nullptr;
	}
}

void FLOOK::add_request(IOOperationRequest* newRequest)
{
	this->IOQueue.push_back(newRequest);
	this->addOnQueue.push_back(newRequest);
}








