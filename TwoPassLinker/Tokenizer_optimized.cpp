// tokenizer_optimized.hpp
//
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
// Use istringstream to get character instead of Two Pointers

/*
Parse the input, prints the token and the position in the file found, at the end prints the 
final position in the file. 
*/

#include "Tokenizer_optimized.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <fstream> 
#include <stdint.h> 
#include <vector>
#include <sstream>
#include <map>



using namespace std;

Tokenizer_optimized::Tokenizer_optimized() // Default Constructor
{
	//
}

Tokenizer_optimized::Tokenizer_optimized(char* filename) // Constructor by loading input file
{
	input_file.open(filename, ios::in); // Assume that the argv[1] is the file name, read-only
	if (input_file.fail()) // file not successfully opened
	{
		printf("Could not open the file."); // Check if the file is opened successfully
		exit(1);
	}
	num_lines = 0;
	line_offset = 1; //1-counting base 
	token_offset = token.length(); //default string constructor has length 0
}

Tokenizer_optimized::~Tokenizer_optimized()
{
	// Default Destructor
}

//Extractors
void Tokenizer_optimized::CloseFile() //close the opened file
{
	input_file.close();
}

//Value Extractor
int Tokenizer_optimized::NumLines() const
{
	return num_lines;
}

int Tokenizer_optimized::LineOffset() const
{
	return line_offset;
}

int Tokenizer_optimized::TokenOffset() const
{
	return token.length();
}

std::string Tokenizer_optimized::Line() const
{
	return line;
}

std::string Tokenizer_optimized::getToken()
{
	if (Walk()) // the current token is char or string
	{	
		line_offset += token.length();
		issline >> token; // Yield string to variable name until whites reached
		//Ref: http://www.cplusplus.com/reference/sstream/istringstream/istringstream/
		//tokenizer check
		//cout << num_lines << ":" << line_offset << " " << token << endl;
		return token;

	}
	else
	{
		cout << "Get invalid token" << "Line: " << num_lines << " Offset: " << line_offset << endl;
		exit(1);
	}
}

//Walk Through
bool Tokenizer_optimized::Walk()
// Ref: https://github.com/begumcig/linker/blob/master/Tokenizer.cpp
// See this github by chance in Googling
// Rewrite without looking at it
{
	if (token.length() + line_offset > line.length()) // the last char/token reached, jump to the newline
	{
		if (num_lines != 0) // One line finished
		{
			line_offset += token.length();
		}
		else // The first line just getline
		{}

		if (getline(input_file, line, '\n'))// parse the stringstream with getline(), count lines
		{
			num_lines++; // newline 
			line_offset = 1; //newline new line_offset, 1-based counting
			token.clear();// newline erase all content in the last token string
			issline.clear(); //All current value are overwritten and cleared
			issline.str(line); //set new value to the iss
		}
		else //Empty File
		{
			return false;
		}
	}
	

	//Trucate whites in the istringstream
	const string delimeters = "\t "; //delimeters to split tokens
	while (delimeters.find(issline.peek()) != std::string::npos) //token is space or tab //Handle beginning and middle
	{
		issline.get(); //Extracts characters from the stream, as unformatted input:
		line_offset++;
		if (line_offset > line.length()) //Handle whites at the end //The last char is space or tab
		{
			line_offset -= token.length();
			return Walk();
		}
	}

	return true; //the token right now is char or string, use Token() extractor to get it
}











