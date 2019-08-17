
// Request.cpp
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

#include "Request.hpp"

IOOperationRequest::IOOperationRequest()
{
	//default constructor with inital values as all zeros
	id = 0;
	arriveTime = 0;
	startTime = 0;
	endTime = 0;
	track = 0;
}

IOOperationRequest::IOOperationRequest(int operationID_, int arriveTime_, int track_)
{
	// overload constructor with given operationID, arriveTime, and track
	id = operationID_;
	arriveTime = arriveTime_;
	startTime = 0;
	endTime = 0;
	track = track_;
}