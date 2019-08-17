// utils.hpp
//
//
// main File
//
//
//
//
// Lab 3 exercise Operating System 2019 Summer
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

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <list>
#include <istream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <utility>
#include <unistd.h>

using namespace std;

extern string VALID_OPERATIONS;

bool get_next_instruction(ifstream& inputFile, string& operation, string& vpage);

// enum OPTION_INDICATOR
// {
// 	O, P, F, S // 
// };

// class Simulator
// {
// public:

// 	std::ifstream inputFile; // the input file of instructions
// 	int frameTableSize; // the default size of frame table which is limited by the hardware
// 	bool O_optFlg, P_optFlg, F_optFlg, S_optFlg, // -o options arguments indicator

// 	Simulator();
// 	~Simulator();
	
// };

#endif 