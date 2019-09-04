// shcedIO.cpp
//
//
// main File
//
//
// Author: Binqian Zeng
//
// start: 07/06/2019
//
//
/*
The main file of the IO Scheduler

 The schedulers to be implemented are FIFO (i), SSTF (j), LOOK (s), CLOOK (c), and FLOOK (f) 
 (the letters in bracket define which parameter must be given in the –s program flag).

Ref: 

To Run:
Invocation is a follows:
./iosched –s<schedalgo> [options] <inputfile> // options are -v -q -f (see below)


Note:
loop that increments simulation time by one and checks whether any action is to be taken. In that case you have to check in the following order.
1) Did a new I/O arrive to the system at this time, if so add to IO-queue ?
2) If an IO is active and completed at this time, if so compute relevant info and store in IO request for final summary
3) If an IO is active but did not yet complete, move the head by one sector/track/unit in the direction it is going (to
simulate seek)
4) If no IO request active now (after (2)) but IO requests are pending? Fetch the next requiest and start the new IO.
5) Increment time by 1
When switching queues in FLOOK you always continue in the direction you were going from the current position, until the queue is empty. 
Then you switch direction until empty and then switch the queues continuing into that direction and so forth. While other variants are
possible, I simply chose this one this time though other variants make also perfect sense.

*/
//
//


#include "Request.hpp"
#include "Scheduler.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <cstdint>



using namespace std;

int main(int argc, char *argv[])
{
	/* code */
	Scheduler* THE_SCHEDULER; // {FIFO (i), SSTF (j), LOOK (s), CLOOK (c), and FLOOK (f) }
	string schedspec; // The scheduler specification
	string inputFilePath; 
	int c = 0; // for argument parsing
	int optindex; // for get inputFilePath from the CML
	
	// ==========================================Argument Parsing =============================================
	while ((c = getopt(argc, argv, "s:")) != -1)
	{
		switch(c)
		{
			case 's':
			{
				schedspec = optarg;
				switch (schedspec[0])
				{
					case 'i':
						THE_SCHEDULER = new class FIFO();
						break;

					case 'j':
						THE_SCHEDULER = new class SSTF();
						break;

					case 's':
						THE_SCHEDULER = new class LOOK();
						break;

					case 'c':
						THE_SCHEDULER = new class CLOOK();
						break;

					case 'f':
						THE_SCHEDULER = new class FLOOK();
						break;
				}// end of scheduler category 
			} // end case 's'
			break;
		}
	} // end parsing arguments

	// =================================== InputFile ========================================
	optindex = optind;
	inputFilePath = argv[optindex];

	// =================================== Get content from the inputFile =====================
	ifstream inputFile;
	string line;
	int operationIdx = 0;
	// list<IOOperationRequest*> operationsList;
	// int N = 15000;
	vector<IOOperationRequest*> operationsVec;

	// cout << "FilePath" << " " << inputFilePath << endl;
	inputFile.open(inputFilePath);
	if (!inputFile.good()) // check if open sucessfully 
	{
		std::cerr << "Fail to open the input file. " << endl;
		abort();
	}

	while (getline(inputFile, line)) // get all operations and store into a vector
	{
		istringstream linestream(line);
		char linehead;
		int timeStamp = 0;
		int track = 0;
		linestream >> linehead;
		if (linehead == '#') {continue;} // ignore lines starting with '#'
		linestream.str(line);
		linestream.clear(); // set error flags
		linestream >> timeStamp >> track;
		IOOperationRequest* newOperation = new IOOperationRequest(operationIdx, timeStamp, track);
		operationsVec.push_back(newOperation);
		operationIdx++;
	}


	// ================================= Simulate the scheduling of IO operations ===========================
	int currentTime = 0; // current time 
	int currentOperationIdx = 0; // the index of the current operation
	IOOperationRequest* currOperation; // current operation
	int totalTime = 0; // total simulated time, i.e. until the last I/O request has completed.
	int totalMovement = 0; // total number of tracks the head had to be moved
	int totalTurnAround = 0; // total turnaround time of all operations
	double avgTurnAround = 0; // average turnaround time per operation from time of submission to time of completion
	int totalWaitTime = 0; // total wait time of all operations
	double avgWaitTime = 0; // average wait time per operation (time from submission to issue of IO request to start disk operation)
	int maxWaitTime = 0; // maximum wait time for any IO operation.
	int head = 0; // The head is initially positioned at track=0 at time=0. //  moving the head to the correct track as seeking
	bool active_flg = false; // The indicator of if the IO is active
	// Note:
	// 1) 1) Did a new I/O arrive to the system at this time, if so add to IO-queue ?
	// 2) If an IO is active and completed at this time, if so compute relevant info and store in IO request for final summary
	// 3) If an IO is active but did not yet complete, move the head by one sector/track/unit in the direction it is going (to simulate seek)
	// 4) If no IO request active now (after (2)) but IO requests are pending? Fetch the next requiest and start the new IO.
	while (!THE_SCHEDULER->IOQueue.empty() || currentOperationIdx < operationsVec.size() || active_flg)
	{
		// 1)
		if (currentOperationIdx < operationsVec.size() && operationsVec[currentOperationIdx]->arriveTime == currentTime)
		{
			//  1) new I/O arrive to the system at this time, add to IO-queue
			THE_SCHEDULER->add_request(operationsVec[currentOperationIdx]);
			currentOperationIdx++;
		}

		// 2) & 3) 
		if (active_flg) 
		{
			// 2) 
			if (currOperation->track == head) // 2) n IO is active and completed at this time, if so compute relevant info and store in IO request for final summary
			{
				currOperation->endTime = currentTime;
				// totalTurnAround += (currOperation->endTime - currOperation->arriveTime);
				active_flg = false;
			}
			// 3) 
			else if (currOperation->track > head) // 3)a. If an IO is active but did not yet complete, move the head by one sector/track/unit in the direction it is going (to simulate seek)
			{
				head++;
				totalMovement++;
				THE_SCHEDULER->postiveDirection = true;
				THE_SCHEDULER->reverseDirection = false;
			}
			else if (currOperation->track < head) // 3)b. If an IO is active but did not yet complete, move the head by one sector/track/unit in the direction it is going (to simulate seek)
			{
				head--;
				totalMovement++;
				THE_SCHEDULER->postiveDirection = false;
				THE_SCHEDULER->reverseDirection = true;
			}
		}

		// 4) no IO request active now (after (2)) but IO requests are pending, Fetch the next requiest and start the new IO.
		if (!active_flg && !THE_SCHEDULER->IOQueue.empty())
		{
			currOperation = THE_SCHEDULER->get_next_IOrequest(head); // fetch
			currOperation->startTime = currentTime;
			if (maxWaitTime < currentTime - currOperation->arriveTime) // update max wait time among all operations
			{
				maxWaitTime = currentTime - currOperation->arriveTime;
			}
			active_flg = true;
			continue;
		}

		currentTime++;
	} // end simulation

	// ======================== Print =============================================
	// For each IO request create an info line
	vector<IOOperationRequest*>::iterator opIter = operationsVec.begin();
	for(; opIter != operationsVec.end(); opIter++)
	{
		totalWaitTime += (*opIter)->startTime - (*opIter)->arriveTime;
		totalTurnAround += (*opIter)->endTime - (*opIter)->arriveTime;
		printf("%5d: %5d %5d %5d\n", (*opIter)->id, (*opIter)->arriveTime, (*opIter)->startTime, (*opIter)->endTime);
	}

	// for the complete run a SUM line:
	avgWaitTime = (double)totalWaitTime / currentOperationIdx;
	avgTurnAround = (double)totalTurnAround / currentOperationIdx;
	totalTime = currentTime - 1;
	printf("SUM: %d %d %.2lf %.2lf %d\n", totalTime, totalMovement, avgTurnAround, avgWaitTime, maxWaitTime);

	return 0; // end
}





