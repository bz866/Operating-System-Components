// EVENT.hpp
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
#ifndef EVENT_HPP
#define EVENT_HPP

#include "Process.hpp"
// #include "Process.cpp"

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

//============================== Event ==========================================
enum TRANSITION_TYPE {TRANS_TO_READY, TRANS_TO_RUN, TRANS_TO_BLOCK, TRANS_TO_PREEMPT}; //defined transition types

struct EVENT
{
	// 
	Process* evtProcess; 
	int evtTimeStamp;
	TRANSITION_TYPE transition;

	EVENT();
	EVENT(Process* proc_, int evtTimeStamp_, TRANSITION_TYPE trans_type_); // Constructor with values
	~EVENT();

/*
NOTE:

Avoid assign same input variable names as member variable names

*/
	// EVENT(Process* proc, int evtTimeStamp, TRANSITION_TYPE trans_type)
	// {
	// 	evtProcess = proc;
	// 	evtTimeStamp = evtTimeStamp;
	// 	transition = trans_type;
	// } // Constructor with values
};

#endif