// tokenizer_optimized.hpp
//
// Header File
//
// Lab 1 exercise Operating System 2019 Summer
//
// Author: Binqian Zeng
//
// start: 06/17/2019
//
//
/*
Parse the input, prints the token and the position in the file found, at the end prints the 
final position in the file. 
*/
//
// Optimized based on the old one 
// Struct the tokenizer as a class instead of a function
// Use private variable to access the current token
// Use Extractor methods to access the current token and line from the outside
//

/*
Parse the input, prints the token and the position in the file found, at the end prints the 
final position in the file. 
*/


#ifndef Tokenizer_optimized_HPP
#define Tokenizer_optimized_HPP

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <fstream> //Input/output stream class to operate on files.
#include <stdint.h> 
#include <vector>
#include <sstream>
#include <map>
#include <iomanip>

using namespace std;


class Tokenizer_optimized
{

private:

	std::ifstream input_file;
	int num_lines; //count the number of lines of the input file
	int line_offset; //the line offset
	int token_offset; //the length of a token
	std::string line; // the current line
	std::string token; //the current token
	std::istringstream issline; 
	//returns a string object with a copy of the current contents
	//Or sets str as the contents of the stream

public:
	
	Tokenizer_optimized(); // Default Constructor
	Tokenizer_optimized(char* filename); // Constructor by loading input file
	~Tokenizer_optimized();
	void CloseFile(); //close the opened file

	//Value Extractor
	int NumLines() const;
	int LineOffset() const;
	int TokenOffset() const;
	std::string Line() const;
	std::string getToken();

	//Modifiers
	//The Tokenizer_optimized works as a loader, no need to be mannually modified

	//WalkThrough the text file
	bool Walk();
	//If meet a valid token like char/string, return true, "\t\n "return false
	//which means EOF reached

};

#endif


















