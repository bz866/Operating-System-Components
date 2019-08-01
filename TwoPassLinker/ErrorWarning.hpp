// ErrorWarning.hpp
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
You should continue processing after encountering an error/warning (other than a syntax error) and you should be able to detect multiple errors in the same run.
1. You should stop processing if a syntax error is detected in the input, print a syntax error message with the line number and the character offset in the input file where observed. A syntax error is defined as a missing token (e.g. 4 used symbols are defined but only 3 are given) or an unexpected token. Stop processing and exit.
2. If a symbol is defined multiple times, print an error message and use the value given in the first definition. Error message to appear as part of printing the symbol table (following symbol=value printout on the same line)
3. If a symbol is used in an E-instruction but not defined anywhere, print an error message and use the value zero.
4. If a symbol is defined but not used, print a warning message and continue.
5. If an address appearing in a definition exceeds the size of the module, print a warning message and treat the address
given as 0 (relative to the module).
6. If an external address is too large to reference an entry in the use list, print an error message and treat the address as
immediate.
7. If a symbol appears in a use list but is not actually used in the module (i.e., not referred to in an E-type address),
print a warning message and continue.
8. If an absolute address exceeds the size of the machine, print an error message and use the absolute value zero.
9. If a relative address exceeds the size of the module, print an error message and use the module relative value zero
(that means you still need to remap “0” that to the correct absolute address).
10. If an illegal immediate value (I) is encountered (i.e. more than 4 numerical digits, aka >= 10000), print an error and
convert the value to 9999.
11. If an illegal opcode is encountered (i.e. more than 4 numerical digits, aka >= 10000), print an error and convert the
<opcode,operand> to 9999.
*/
//
//


#ifndef ErrorWarning_HPP
#define ErrorWarning_HPP
#include <sstream>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <fstream> //Input/output stream class to operate on files.
#include <ctype.h>
#include <map>

using namespace std;

class ErrorWarning
{
	
private:

	int code;
	int idx_module;
	string name;
	int address;
	int val_constraint;

public:
	ErrorWarning();
	~ErrorWarning();

	//Extractors
	int Code() const;
	int IdxModule() const;
	string Name() const;
	int Address() const;
	int ValConstraint() const;

	// Modifiers
	void Code(int newCode);

	void IdxModule(int new_idx_module);

	void Name(string newName);

	void Address(int newAddress);

	void ValConstraint(int new_val_constraint);

	//Error OR Warning msg
	void getMessage();

		
};


#endif

















