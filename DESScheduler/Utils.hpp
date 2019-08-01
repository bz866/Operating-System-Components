// Utils.hpp
//
//
// main File
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
All global functions 
*/
//
//
#ifndef Utils_HPP
#define Utils_HPP

// #include "DiscreteEventSimulator.hpp"
// #include "DiscreteEventSimulator.cpp"
#include "EVENT.hpp"
#include "Process.hpp"

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
#include <cctype>
#include <string>
#include <cstring>
#include <deque>
#include <vector>
#include <list>

using namespace std;

int myrandom(int burst); //generate random number

void put_event_to_eventQueue(EVENT* evt); // put event into the eventqueue by timestamp
void put_event_to_eventQueue(Process* evtProcess, int evtTimeStamp, TRANSITION_TYPE transition); //Put event with constructor with values 

EVENT* get_event_from_eventQueue(); // Get event from the eventQueue

int get_next_event_time(); //get the evtTimeStamp of the next event(the very front one) from the event queue

std::string extract_ints(std::ctype_base::mask category, std::string str, std::ctype<char> const& facet);

std::string extract_ints(std::string str);

int has_any_digits(std::string& s);

bool contains_number(const std::string &c);

#endif