// utils.cpp
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
#include "Utils.hpp"

using namespace std;

bool get_next_instruction(ifstream& inputFile, string& operation, string& vpage)
{
	// cout << "IN GET NEXT instruction" << endl;
	if (inputFile.eof())
	{
		return false;
	}
	char linehead;
	string line;

    getline(inputFile, line);
    istringstream linestream(line);
    linestream >> linehead;
    while (linehead == '#')
    {
        if(inputFile.eof()) // end with # line
        {
            return false;
        }
        getline(inputFile, line);
        linestream.clear();
        linestream.str(line);
        linestream >> linehead;
    }

    operation = linehead;
	if (VALID_OPERATIONS.find(operation) == std::string::npos)
	{
		return false;
	}
	if (line.size() > 0)
	{
		linestream >> vpage;
	}
	return true;
}

// bool get_next_instruction(ifstream& inputFile_, string& operation_, string& vpage_)
// {
// 	if (inputFile_.eof()) // empty file
// 	{
// 		return false;
// 	}

// 	string line
// }