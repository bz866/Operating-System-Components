// EVENT.cpp
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

EVENT::EVENT() {}

EVENT::~EVENT() {}

EVENT::EVENT(Process* proc_, int evtTimeStamp_, TRANSITION_TYPE trans_type_) //Constructor with values
{
	evtProcess = proc_;
	evtTimeStamp = evtTimeStamp_;
	transition = trans_type_;
}