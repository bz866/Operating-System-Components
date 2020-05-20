// Utils.cpp
//
//
//
//
//
//
//
// Author: Binqian Zeng
//
// start: 07/06/2019
//
//
/*

The File with Utilities functions

All functions are global

*/
//
//


#include "Utils.hpp"
// #include "Process.hpp"
// #include "DiscreteEventSimulator.hpp"

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

#include <locale>
#include <algorithm>
#include <iterator>
#include <cstring>

using namespace std;

//extern std::deque<EVENT*> eventQueue; // queue of events, push all process features as first event, then insert remained process events features by evtTimeStamp
extern std::list<EVENT*> eventQueue;
extern std::vector<int> randvals;

int myrandom(int burst)
{
	//Generate a random number between [1, burst]
	//Can be used to generate random CPUBurst, IOBurst, and static_priority

	int res_rand;
	if (ofs == randnums_num - 1) {ofs = 0;} //wrap around when we run out of numbers
	// cout << "MYRANDOM" << ofs << randvals[ofs] << " " << burst << " " << endl;
	res_rand = 1 + (int)(randvals[ofs] % burst); // [1, burst]
	ofs++;
	return res_rand;
}


void put_event_to_eventQueue(EVENT *evt)
{
	//put event into the eventqueue by timestamp
	if (eventQueue.empty()) // Empty eventQueue
	{
		eventQueue.push_back(evt);
		return; 
	}

	std::list<EVENT*> containerQ;
	while (!eventQueue.empty()) // non-empty eventQueue
	{
		EVENT* ori_evt = eventQueue.front();
		if (evt->evtTimeStamp >= ori_evt->evtTimeStamp)
		{
			containerQ.push_back(ori_evt);
			eventQueue.pop_front();
		}
		else
		{
			containerQ.push_back(evt); // insert evt into queue by timestamp
			break;
		}
	}
	if (eventQueue.empty())
	{
		containerQ.push_back(evt); // the new to the end 
	}
	else
	{
		while (!eventQueue.empty()) // remained events in eventQueue
		{
			EVENT* ori_evt = eventQueue.front();
			containerQ.push_back(ori_evt);
			eventQueue.pop_front();
		}
	}

	eventQueue = containerQ; // inserted eventQueue , sorted by evtTimeStamp
	containerQ.clear(); // clear content to save storage 
}

void put_event_to_eventQueue(Process* evtProcess, int evtTimeStamp, TRANSITION_TYPE transition)
{
	//Ref: https://www.tutorialspoint.com/cplusplus/cpp_this_pointer.htm
	//The this pointer is an implicit parameter to all member functions. Therefore, inside a member function, this may be used to refer to the invoking object.
	put_event_to_eventQueue(new EVENT(evtProcess, evtTimeStamp, transition));
}

EVENT* get_event_from_eventQueue()
{
	//Get event from the eventQueue
	if (eventQueue.empty())
	{
		return nullptr;
	}
	//else
	EVENT* res_evt;
	res_evt = eventQueue.front();
	eventQueue.pop_front();
	return res_evt;
}

int get_next_event_time()
{
	//get the evtTimeStamp of the next event(the very front one) from the event queue
	if (eventQueue.empty())
	{
		return -1;
	}
	//else
	int next_evt_timestamp = eventQueue.front()->evtTimeStamp;
	return next_evt_timestamp;
}

std::string extract_ints(std::ctype_base::mask category, std::string str, std::ctype<char> const& facet)
{
    using std::strlen;

    char const *begin = &str.front(),
               *end   = &str.back();

    auto res = facet.scan_is(category, begin, end);

    begin = &res[0];
    end   = &res[strlen(res)];

    return std::string(begin, end);
}

std::string extract_ints(std::string str)
{
    return extract_ints(std::ctype_base::digit, str,
         std::use_facet<std::ctype<char>>(std::locale("")));
}

int has_any_digits(const std::string& s)
{
    
    if (std::any_of(s.begin(), s.end(), ::isdigit))
    {
    	return 1;
    }
    else
    {
    	return 0;
    }
}

bool contains_number(const std::string &c)
{
    if (c.find('0') != std::string::npos ||
        c.find('1') != std::string::npos ||
        c.find('2') != std::string::npos ||
        c.find('3') != std::string::npos ||
        c.find('4') != std::string::npos ||
        c.find('5') != std::string::npos ||
        c.find('6') != std::string::npos ||
        c.find('7') != std::string::npos ||
        c.find('8') != std::string::npos ||
        c.find('9') != std::string::npos)
    {
        return true;
    }

    return false;
}





















