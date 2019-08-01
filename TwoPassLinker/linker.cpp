

//============== tokenizer ==================//
/*
#include "Module.hpp"
#include "Symbol.hpp"
#include "Instruction.hpp"
#include "ErrorWarning.hpp"
*/

#include "Tokenizer_optimized.hpp"
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
#include <algorithm>

//#include <regex>
//std::regex decimal("[0-9]+", std::regex_constants::basic); //integer for readInt()
//std::regex alphanumerical("([A-Z]|[a-z])([A-Z]|[a-z]|[0-9])*", regex_constants::extended); //symbol name for readSymbol()

using namespace std;

int MACHINE_MEMONRY = 512;
int MAX_NUM_DEF = 16;
int MAX_NUM_USE = 16;
int MAX_SYM_LEN = 16;
bool END_OF_MODULE = false;


int idx_instr = 0; // the index of the instruction globally
int idx_module = 0; //if fileopen succeed, ++, 1-based counting
int total_instr = 0; // Total number of instructions 
//std::map<string, int> defCountPairs; // defcount pairs(S, R), symbol and its relative word address
std::map<string, int> globalDefCountPairsList; //Global defcount pairs(S, R), Global defined symbols and their absolute word address
std::map<string, int> globalSymbolUsedList; //Global defined Symbol and their number of module where they are used 
std::map<int, int> baseAddByModuleList; // pairs of module index and their base address
std::map<string, bool> globalSymbolMultiDefinedList; //record all symbols that are defined multiple times
std::map<string, int> globalSymbolDefinedModuleIdxList; //pairs of defined symbols and their index of module where they are defined 

std::map<string, int> moduleDefCountPairsList; //relative word address should < incoming codecount of the module - 1 //Cleared in the end of a module
std::vector<string> moduleSymbol2UseList; // Use vector to store all symbols will be called within the module //Cleared in the end of a module
std::map<string, bool> moduleSymbolUsedList;  //The vector to record used symbol in module level  //Cleared in the end of a module

bool IN_PASS_ONE = false;
bool IN_PASS_TWO = false;

//Integer pattern
bool isNumber(string s)
{
	for (int i; i < s.length(); i++) 
        if (isdigit(s[i]) == false) 
            return false; 
  
    return true; 
}

//alphanumerical pattern
bool isAlnumPattern(string s)
{
	if (!(isalpha(s[0])))
	{
		return false; // symbol must start with a letter
	}

	int i = 1; // the rest part must follow "([A-Z]|[a-z])([A-Z]|[a-z]|[0-9])*"
	while (i < s.length())
	{
		if (isalpha(s[i]))
			i++;
		else
			break;
	}
	while (i < s.length())
	{
		if (isdigit(s[i]))
			i++;
		else
			return false;
	}
	return true;
}


//format output table line number as 3 digits
void printTableLineNum(int idx_instr)
{
	cout << std::setfill('0') << std::setw(3) << idx_instr << ": ";
}

//format instruction number as 4 digits
void printInstrNum(int instr_num)
{
	cout << std::setfill('0') << std::setw(4) << instr_num;
}

void printTableHeadMeta()
{
	ErrorWarning ErrorWarningTrigger;

	cout << "Symbol Table" << endl;
	for (std::map<string, int>::iterator iter = globalDefCountPairsList.begin(); iter != globalDefCountPairsList.end(); iter++)
	{
		cout << iter-> first << "=" << iter->second;
		if (globalSymbolMultiDefinedList[iter->first])
		{
			ErrorWarningTrigger.Code(2);
			ErrorWarningTrigger.getMessage();
		}
		cout << endl;
	}
	cout << endl;
	
	// Start of the output table
	cout << "Memory Map" << endl;

}


//parse errors (given in Note)
void __parseerror(int errcode, int num_lines, int line_offset)
{
	const char* errstr[] = 
	{
		"NUM_EXPECTED",		//0 // Number expected
		"SYM_EXPECTED",		//1 // Symbol Expected
		"ADDR_EXPECTED", 	//2 // Addressing Expected which is A/E/I/R
		"SYM_TOLONG", 		//3 // Symbol name is too long
		"TOO_MANY_DEF_IN_MODULE",	//4 // > 16
		"TOO_MANY_USE_IN_MODULE",	//5 // >16
		"TOO_MANY_INSTR"		//6 // total num_instr exceeds memory size 512
	};

	cout << "Parse Error line " << num_lines << " offset " << line_offset << ": " << errstr[errcode] << endl;
	exit(1); // if parse error raised, exit
}

void __stageError(int stage_num)
{
	switch (stage_num)
	{
		case 1:
			cout << "functions for stage 1 can only be called when IN_PASS_ONE=TRUE and IN_PASS_TWO=FALSE. " << endl;
			exit(1);
		case 2:
			cout << "functions for stage 2 can only be called when IN_PASS_ONE=TRUE and IN_PASS_TWO=FALSE. " << endl;
			exit(1);
	}
}

bool intErrorDetected(int cnt, int int_category)
//Determine if the count number meets requirements based on its category
//defcount <= 16
//usecount <= 16
//codecount <= 512
{
	switch (int_category)
	{
		case 4: 
			return cnt > MAX_NUM_DEF;
		case 5:
			return cnt > MAX_NUM_USE;
		case 6:
			total_instr += cnt;
			return total_instr > MACHINE_MEMONRY;
		default:
			return false;
	}
	return false;
}

bool symbol2UseDefined(string symbol2Use)
{
	return (globalDefCountPairsList.find(symbol2Use) != globalDefCountPairsList.end());
}

bool symbolMultiDefined(string symbol)
{
	return (globalDefCountPairsList.find(symbol) != globalDefCountPairsList.end());
}

bool symbolNotUsedInGlobal(string symbol)
{
	return (globalSymbolUsedList.find(symbol) == globalSymbolUsedList.end());
}



int readInt(Tokenizer_optimized &Tokenizer, int int_category) 
//Take the reference of the Tokenizer without modifing or initializing it
//Three possible integer categories: 4. defcount 5. usecount 6. codecount
//4, 5, 6 corresponds to __parseError 4, 5, 6
{
	if (Tokenizer.Walk()) //Valid token
	{
		string num_str = Tokenizer.getToken();
		char num_char_arr[num_str.size() + 1];
		strcpy(num_char_arr, num_str.c_str());
		//if (regex_match(num_str, decimal)) //integer for sure
		if (isNumber(num_str))
		{
			//int num = std::stoi(Tokenizer.getToken()); // convert string integer to int
			int num = strtol(num_char_arr, NULL, 10);
			if (intErrorDetected(num, int_category)) //not within constraints
			{
				__parseerror(int_category, Tokenizer.NumLines(), Tokenizer.LineOffset());
			}
			else //within constraint
			{
				return num;
			}
		}
		else //Not pure integer
		{	
			__parseerror(0, Tokenizer.NumLines(), Tokenizer.LineOffset());
		}
	}
	else // get Invalid Token
	{
		if (END_OF_MODULE)
		{
			//current module successfully loaded, 
			// though hit the end of the file so no valid defcount can be got
			//invalid token is got, but not a mistake
			return -1; 
		}
		else 
		{
			__parseerror(0, Tokenizer.NumLines(), Tokenizer.LineOffset());
		}
	}
	return -999999; // foo compiler to avoid warning
}

std::string readSymbol(Tokenizer_optimized &Tokenizer)
{
	if (Tokenizer.Walk()) //Valid Token
	{
		string symbol = Tokenizer.getToken();
		//if (regex_match(symbol, alphanumerical)) //Symbol must be alphanumerical 
		if (isAlnumPattern(symbol))
		{
			if (symbol.length() > MAX_SYM_LEN) //Symbol Length constraint
			{
				__parseerror(3, Tokenizer.NumLines(), Tokenizer.LineOffset());
			}
			return symbol;
		}	
		else // Not valud symbol string, __parseerror 
		{
			__parseerror(1, Tokenizer.NumLines(), Tokenizer.LineOffset());
		}
	}
	else
	{
		__parseerror(1, Tokenizer.NumLines(), Tokenizer.LineOffset()); //Expecting Valid string as Symbol
	}
	return "-999999"; // foo compiler to avoid warning
}

void readIAER_PASS_ONE(Tokenizer_optimized &Tokenizer)
{
	if (Tokenizer.Walk()) //Valid Token
	{
		string instrtype = Tokenizer.getToken();
		//std::vector<string> valid_instrtype_vec {"I", "A", "E", "R"};
		//static const string valid_instrtype_arr = {"I", "A", "E", "R"};
		std::vector<string> valid_instrtype_vec;
		valid_instrtype_vec.push_back("I");
		valid_instrtype_vec.push_back("A");
		valid_instrtype_vec.push_back("E");
		valid_instrtype_vec.push_back("R");

		std::vector<string>::iterator it = std::find(valid_instrtype_vec.begin(), valid_instrtype_vec.end(), instrtype); // if token is IAER
		
		if (IN_PASS_ONE && !IN_PASS_TWO)
		{
			if (it != valid_instrtype_vec.end()) // IAER
			{
				// IN PASS ONE only checking
				// no storing or returing needed
			}
			else
			{
				__parseerror(2, Tokenizer.NumLines(), Tokenizer.LineOffset()); // Not IAER
			}
		}
		else
		{
			__stageError(1);
		}
	}
	else
	{
		__parseerror(2, Tokenizer.NumLines(), Tokenizer.LineOffset()); // Expecting valid char as Instruction type IAER 
	}
}

std::string readIAER_PASS_TWO(Tokenizer_optimized &Tokenizer)
{
	if (Tokenizer.Walk()) //Valid Token
	{
		string instrtype = Tokenizer.getToken();

		if (!IN_PASS_ONE && IN_PASS_TWO)
		{
			return instrtype;
		}
		else
		{
			__stageError(2);
		}
	}
	else
	{
		__parseerror(2, Tokenizer.NumLines(), Tokenizer.LineOffset()); // Expecting valid char as Instruction type IAER 
	}
	return "-999999"; // foo compiler to avoid warning
}

void readInstrNum_PASS_ONE(Tokenizer_optimized &Tokenizer)
{
	if (Tokenizer.Walk()) //Valid Token
	{
		if (IN_PASS_ONE && !IN_PASS_TWO)
		{
			//IN PASS ONE, only detect if there is parse error not storing or printing
			int instr_num = readInt(Tokenizer, 0); // 0 means not constrained by defcount usecount or codecount
		}
		else
		{
			__stageError(1);
		}
	}
	else
	{
		__parseerror(0, Tokenizer.NumLines(), Tokenizer.LineOffset());
	}
}

void readInstrNum_PASS_TWO(Tokenizer_optimized &Tokenizer, string &last_instrtype)
{
	ErrorWarning ErrorWarningTrigger;

	if (Tokenizer.Walk()) //Valid Token
	{
		if (!IN_PASS_ONE && IN_PASS_TWO)
		// IN PASS TWO, we need to print program text as table 
		{
			int instr_num = readInt(Tokenizer, 0);
			int abs_instr_num; 

			printTableLineNum(idx_instr); // Global instruments index
			//Constraint & Error of InstrNum given instrtype
			if (last_instrtype == "I") // Immidiate address
			{
				if (instr_num >= 10000)
				{
					instr_num = 9999;
					printInstrNum(instr_num);
					ErrorWarningTrigger.Code(10);
					ErrorWarningTrigger.getMessage();
				}
				else
				{
					printInstrNum(instr_num);
				}
			}
			else if(last_instrtype == "E")
			{
				if (instr_num >= 10000)
				{
					instr_num = 9999;
					printInstrNum(instr_num);
					ErrorWarningTrigger.Code(11);
					ErrorWarningTrigger.getMessage();
				}
				else if (instr_num % 1000 >= moduleSymbol2UseList.size())
				{
					printInstrNum(instr_num);
					ErrorWarningTrigger.Code(6);
					ErrorWarningTrigger.getMessage();
				}
				else
				{
					int opcode = instr_num / 1000;
					int operand = instr_num % 1000;
					string curSymbol2Use = moduleSymbol2UseList[operand];
					if (symbol2UseDefined(curSymbol2Use))
					{	

						abs_instr_num = opcode * 1000 + globalDefCountPairsList[curSymbol2Use];
						printInstrNum(abs_instr_num);
						globalSymbolUsedList[curSymbol2Use] = true;
						moduleSymbolUsedList[curSymbol2Use] = true;
					}
					else //symbol to use not defined 
					{
						abs_instr_num = opcode * 1000 + 0;
						moduleSymbolUsedList[curSymbol2Use] = true; //still mark as used
						printInstrNum(abs_instr_num);
						ErrorWarningTrigger.Code(3);
						ErrorWarningTrigger.Name(curSymbol2Use);
						ErrorWarningTrigger.getMessage();
					}

				}
			}
			else if(last_instrtype == "A")
			{
				if (instr_num >= 10000)
				{
					instr_num = 9999;
					printInstrNum(instr_num);
					ErrorWarningTrigger.Code(11);
					ErrorWarningTrigger.getMessage();
				}
				else
				{
					int opcode = instr_num / 1000;
					int operand = instr_num % 1000;
					if (operand > MACHINE_MEMONRY)
					{
						abs_instr_num = opcode * 1000;
						printInstrNum(abs_instr_num);
						ErrorWarningTrigger.Code(8);
						ErrorWarningTrigger.getMessage();
					}
					else
					{
						printInstrNum(instr_num);
					}
				}
			}
			else if(last_instrtype == "R")
			{
				if (instr_num >= 10000)
				{
					instr_num = 9999;
					printInstrNum(instr_num);
					ErrorWarningTrigger.Code(11);
					ErrorWarningTrigger.getMessage();
				}
				else
				{
					int opcode = instr_num / 1000;
					int operand = instr_num % 1000;
					if (operand >= baseAddByModuleList[idx_module + 1] - baseAddByModuleList[idx_module])
					{
						abs_instr_num = opcode * 1000 + baseAddByModuleList[idx_module];
						printInstrNum(abs_instr_num);
						ErrorWarningTrigger.Code(9);
						ErrorWarningTrigger.getMessage();
					}
					else
					{
						abs_instr_num = instr_num + baseAddByModuleList[idx_module];
						printInstrNum(abs_instr_num);
					}
				}

			}
		}
		else
		{
			__stageError(2);
		}

		cout << endl; //table printing line break
	}
	else 
	{
		__parseerror(0, Tokenizer.NumLines(), Tokenizer.LineOffset());
	}
}

void passOne(char* filename) //pointer to an array of chars
{
	ErrorWarning ErrorWarningTrigger;

	Tokenizer_optimized Tokenizer(filename); //Initialize Tokenizer for pass one
	//Module defcount pairs(S, R), defined symbols and their relative word address
	//std::map<string, int> moduleDefCountPairsList; //relative word address should < incoming codecount of the module - 1
	//std::vector<string> moduleSymbol2UseList; // Use vector to store all symbols will be called within the module
	//std::vector<string, bool> moduleSymbolUsedList; // vector of boolean indicators to record if a symbol in a module is used

	while (Tokenizer.Walk())
	{
		idx_module++; //fileopen successed, idx_module 1-based counting

		//get defcount
		int defcount = readInt(Tokenizer, 4); //read defcount, can be __parseerror(4)
		if (defcount == -1) 
		{
			//the latest moduel successfully loaded 
			//hit the end of module ,then no defcount can be got
			//invalid token is got, but not a mistake
			//handle file end with newlines
			Tokenizer.CloseFile();
			break;
		}

		//get defcount pairs
		for (int i=0; i < defcount; i++) // number of defcount defcount pairs
		{
			string symbol = readSymbol(Tokenizer); //symbol defined
			int symbol_rel_add = readInt(Tokenizer, 0); //symbol relative address, no constraint
			//cout << Tokenizer.NumLines() << " " << Tokenizer.LineOffset() << " " << symbol_rel_add << endl;

			if (symbolMultiDefined(symbol)) //check if the symbol is multi defined
			{
				globalSymbolMultiDefinedList[symbol] = idx_module;
			}
			else
			{
				moduleDefCountPairsList[symbol] = symbol_rel_add;
				globalDefCountPairsList[symbol] = symbol_rel_add + baseAddByModuleList[idx_module];
				globalSymbolDefinedModuleIdxList[symbol] = idx_module;
			}
		}

		//get usecount
		int usecount = readInt(Tokenizer, 5); //read usecount, can be __parseerror(4)

		//get symbols 
		for (int i=0; i < usecount; i++) // number of usecount symbols
		{
			string symbol_2_use = readSymbol(Tokenizer); //
		}

		//get codecount
		int codecount = readInt(Tokenizer, 6); //read codecount, can be __parseerror(6)

		//Warnings Print
		//Constraint: relative address in defcount pairs should < codecount - 1
		for (std::map<string, int>::iterator iter = moduleDefCountPairsList.begin(); iter != moduleDefCountPairsList.end(); iter++)
		{
			if (iter->second > codecount - 1)
			{
				ErrorWarningTrigger.Code(5);
				ErrorWarningTrigger.IdxModule(idx_module);
				ErrorWarningTrigger.Name(iter->first);
				ErrorWarningTrigger.Address(iter->second);
				int val_constraint = codecount-1;
				ErrorWarningTrigger.ValConstraint(val_constraint);
				ErrorWarningTrigger.getMessage();
				globalDefCountPairsList[iter->first] = baseAddByModuleList[idx_module];
				cout << endl;
			}
		}


		//get instruction type and instruction number(address)
		for (int i = 0; i < codecount; i++)
		{
			readIAER_PASS_ONE(Tokenizer);
			readInstrNum_PASS_ONE(Tokenizer);
		}

		//set base address of the next module based on current base and codecount
		baseAddByModuleList[idx_module + 1] = baseAddByModuleList[idx_module] + codecount;
		//module loading end
		END_OF_MODULE = true; 

		moduleDefCountPairsList.clear();
	}

	//Close the file
	Tokenizer.CloseFile();

	//print table head metadata
	printTableHeadMeta();

	//clear to reopen & rewalk in pass two
	//globalDefCountPairsList.clear();
	//globalSymbolMultiDefinedList.clear();
	//moduleDefCountPairsList.clear();

}

void passTwo(char* filename)
{
	ErrorWarning ErrorWarningTrigger;
	idx_module = 0;
	idx_instr = 0;
	END_OF_MODULE = false;

	//Reopen the file 
	Tokenizer_optimized Tokenizer(filename);
	//Initialize Tokenizer for pass one
	//Module defcount pairs(S, R), defined symbols and their relative word address
	//std::vector<string, int> moduleDefCountPairsList; //relative word address should < incoming codecount of the module - 1
	//std::vector<string> moduleSymbol2UseList; // Use vector to store all symbols will be called within the module
	bool final_warning_printed = false;

	while (Tokenizer.Walk())
	{
		idx_module++; //fileopen successed, idx_module 1-based counting

		//get defcount
		int defcount = readInt(Tokenizer, 4); //read defcount, can be __parseerror(4)
		if (defcount == -1) 
		{
			//When the end of file is hit
			//PASS two should print required warnings
			Tokenizer.CloseFile();
			idx_module -= 1; //Whites are not new module

			cout << endl;
			//Warning print
			for (std::map<string, int>::iterator iter = globalDefCountPairsList.begin(); iter != globalDefCountPairsList.end(); iter++)
			{
				if (symbolNotUsedInGlobal(iter->first))
				{
					ErrorWarningTrigger.Code(4);
					ErrorWarningTrigger.IdxModule(idx_module);
					ErrorWarningTrigger.Name(iter->first);
					ErrorWarningTrigger.getMessage();
					cout << endl;
				}
			}
			final_warning_printed = true;
			cout << endl;
			break;
		}

		//get defcount pairs
		for (int i = 0; i < defcount; i++)
		{
			string symbol = readSymbol(Tokenizer); //symbol defined
			int symbol_rel_add = readInt(Tokenizer, 4);
			if (symbolMultiDefined(symbol)) //check if the symbol is multi defined
			{
				globalSymbolMultiDefinedList[symbol] = idx_module;
			}
			else
			{
				moduleDefCountPairsList[symbol] = symbol_rel_add;
				globalDefCountPairsList[symbol] = symbol_rel_add + baseAddByModuleList[idx_module];
				globalSymbolDefinedModuleIdxList[symbol] = idx_module;
			}
		}

		//get usecount
		int usecount = readInt(Tokenizer, 5);
		//get symbols storing to uselist 
		for (int i = 0; i < usecount; i++)
		{
			string symbol_2_use = readSymbol(Tokenizer); //get
			moduleSymbol2UseList.push_back(symbol_2_use); //store
		}

		//get codecount
		int codecount = readInt(Tokenizer, 6);

		//Constraint: relative address in defcount pairs should < codecount - 1
		for (std::map<string, int>::iterator iter = moduleDefCountPairsList.begin(); iter != moduleDefCountPairsList.end(); iter++)
		{
			if (iter->second > codecount - 1)
			{
				ErrorWarningTrigger.Code(5);
				ErrorWarningTrigger.IdxModule(idx_module);
				ErrorWarningTrigger.Name(iter->first);
				ErrorWarningTrigger.Address(iter->second);
				int val_constraint = codecount-1;
				ErrorWarningTrigger.ValConstraint(val_constraint);
				//ErrorWarningTrigger.getMessage();
				globalDefCountPairsList[iter->first] = baseAddByModuleList[idx_module];
			}
		}


		//get instruction
		for (int i = 0; i < codecount; i++)
		{
			string cur_instrtype = readIAER_PASS_TWO(Tokenizer); //read type
			readInstrNum_PASS_TWO(Tokenizer, cur_instrtype); //read num and print
			idx_instr++;
		}


		//Warning print
		// Symbol appeared in uselist but not used
		for (int i = 0; i < moduleSymbol2UseList.size(); i++)
			//(std::vector<string>::iterator sym = moduleSymbol2UseList.begin(); sym != moduleSymbol2UseList.end(); sym++)
		{

			//if (moduleSymbolUsedList.find(moduleSymbol2UseList[i]) == moduleSymbolUsedList.end())
			if (moduleSymbolUsedList.find(moduleSymbol2UseList[i]) == moduleSymbolUsedList.end())
			{
				ErrorWarningTrigger.Code(7);
				ErrorWarningTrigger.IdxModule(idx_module);
				ErrorWarningTrigger.Name(moduleSymbol2UseList[i]);
				ErrorWarningTrigger.getMessage();
				cout << endl;
			}
		}

		END_OF_MODULE = true;

		// module record clear
		moduleSymbol2UseList.clear();
		moduleSymbolUsedList.clear();
		moduleDefCountPairsList.clear();

	}

	Tokenizer.CloseFile();

	// Warning Print
	// symbol defined not used
	if (!final_warning_printed)
	{
		cout << endl;
		for (std::map<string, int>::iterator iter = globalDefCountPairsList.begin(); iter != globalDefCountPairsList.end(); iter++)
		{
			if (symbolNotUsedInGlobal(iter->first))
			{
				ErrorWarningTrigger.Code(4);
				ErrorWarningTrigger.IdxModule(globalSymbolDefinedModuleIdxList[iter->first]);
				ErrorWarningTrigger.Name(iter->first);
				ErrorWarningTrigger.getMessage();
				cout << endl;
			}
		}
		final_warning_printed = true;
		cout << endl;
	}

}






//================== main ==========================//

int main(int argc, char* argv[])
{
	// Invalid input file path
	if (argc > 1) // argc should be two, take one file a time as input
	{
		if (argc > 2)
		{
			cout << "One text file at a time. Not a valid input file path: " << "arguments length %s " << argc << "!= 2" << endl;
			return 1;
		}
		else {}
	}
	else
	{
		cout << "Value Error. Not given a input file path" << endl;
		return 1;
	}

	
	char* filename = argv[1]; //Valid inputfile path argument
	//ErrorWarning ErrorWarningTrigger(); //Initialize the ErrorWarning module 

	//PASS ONE
	IN_PASS_ONE = true;
	IN_PASS_TWO = false;
	passOne(filename);

	//PASS Two
	IN_PASS_ONE = false;
	IN_PASS_TWO = true;
	passTwo(filename);

	return 0;
}























