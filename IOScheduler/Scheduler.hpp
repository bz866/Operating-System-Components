
//
// Scheduler.hpp
//
// Author: Binqian Zeng
//
// start: 07/06/2019
//
//
/*

The schedulers to be implemented are FIFO (i), SSTF (j), LOOK (s), CLOOK (c), and FLOOK (f) 
(the letters in bracket define which parameter must be given in the –s program flag).

Note:

IO Scheduler algorithms
- FIFO(i)
- SSTF(j)
- LOOK(s)
- CLOOK(c)
- FLOOK(f)

*/
//
//

#ifndef Scheduler_HPP
#define Scheduler_HPP

#include "Request.hpp"

#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <algorithm>
#include <climits>


using namespace std;

class Scheduler
{
public:
	std::vector<IOOperationRequest*> IOQueue;
	// std::vector<IOOperationRequest*> requestsQueue[2];
	// std::vector<IOOperationRequest*> *activeQueue = &requestsQueue[0];
	// std::vector<IOOperationRequest*> *addOnQueue = &requestsQueue[1];
	std::vector<IOOperationRequest*> activeQueue;
	std::vector<IOOperationRequest*> addOnQueue;
	bool postiveDirection;
	bool reverseDirection;

	Scheduler(); 
	~Scheduler();
	virtual IOOperationRequest* get_next_IOrequest(int head);
	virtual void add_request(IOOperationRequest* newRequest);
};

class FIFO : public Scheduler
{
public:
	IOOperationRequest* get_next_IOrequest(int head);
	void add_request(IOOperationRequest* newRequest);
};

class SSTF : public Scheduler
{
public:
	IOOperationRequest* get_next_IOrequest(int head);
	void add_request(IOOperationRequest* newRequest);
};

class LOOK : public Scheduler
{
public:
	LOOK(); // initialize the direction = false
 	IOOperationRequest* get_next_IOrequest(int head);
 	void add_request(IOOperationRequest* newRequest);
};

class CLOOK : public Scheduler
{
public:
	IOOperationRequest* get_next_IOrequest(int head);
 	void add_request(IOOperationRequest* newRequest);
};

class FLOOK : public Scheduler
{
public:
	FLOOK(); // initlize the direction = false
	IOOperationRequest* get_next_IOrequest(int head);
 	void add_request(IOOperationRequest* newRequest);
};


#endif /* The IO Schduler defined */
