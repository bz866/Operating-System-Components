
// Request.hpp
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


Ref: 

To Run:


Note:

*/
//
//

#ifndef Request_HPP
#define Request_HPP

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

using namespace std;

struct IOOperationRequest
{
	int id;
	int arriveTime;
	int startTime;
	int endTime;
	int track;

	IOOperationRequest(); //default constructor with initial values for all members
	IOOperationRequest(int operationID_, int arriveTime_, int track_); // overload constructor with given operationID, arriveTime, and track
};


#endif 