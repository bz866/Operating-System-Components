// ErrorWarning.hpp
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


#include "ErrorWarning.hpp"
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
#include <iomanip>

using namespace std;

ErrorWarning::ErrorWarning(){}

ErrorWarning::~ErrorWarning(){}

//Access Value
int ErrorWarning::Code() const
{
	return code;
}

int ErrorWarning::IdxModule() const
{
	return idx_module;
}

string ErrorWarning::Name() const
{
	return name;
}

int ErrorWarning::Address() const
{
	return address;
}

int ErrorWarning::ValConstraint() const
{
	return val_constraint;
}

// Modifiers
void ErrorWarning::Code(int newCode)
{
	code = newCode;
}

void ErrorWarning::IdxModule(int new_idx_module)
{
	idx_module = new_idx_module;
}

void ErrorWarning::Name(string newName)
{
	name = newName;
}

void ErrorWarning::Address(int newAddress)
{
	address = newAddress;
}

void ErrorWarning::ValConstraint(int new_val_constraint)
{
	val_constraint = new_val_constraint;
}

//return Error OR Warning
void ErrorWarning::getMessage()
{
	//string msg;

	switch (code)
	{
		case 8: cout << " Error: Absolute address exceeds machine size; zero used";
		break;

		case 9: cout << " Error: Relative address exceeds module size; zero used";
		break;

		case 6: cout << " Error: External address exceeds length of uselist; treated as immediate";
		break;

		case 3: cout << " Error: " << name << " is not defined; zero used";
		break;

		case 2: cout << " Error: This variable is multiple times defined; first value used";
		break;

		case 10: cout << " Error: Illegal immediate value; treated as 9999";
		break;

		case 11: cout << " Error: Illegal opcode; treated as 9999";
		break;

		case 5: cout << "Warning: Module " << idx_module << ": " << name << " too big " << address <<
		" (max=" <<  val_constraint << ") assume zero relative";
		break;

		case 7: cout << "Warning: Module " << idx_module << ": " << name << " appeared in the uselist but was not actually used";
		break;

		case 4: cout << "Warning: Module " << idx_module << ": " << name << " was defined but never used";
		break;

	}

	//return msg;
}