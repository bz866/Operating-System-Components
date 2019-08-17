//
// HEader File
// // Process.hpp
//
//
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

 - processes, each with its array/vector/list of vmas and a pageTable 
 that represents the translations from virtual pages to physical frames for that process.

- a specification for each process’ address space is comprised of
i. the number of virtual memory areas / segments (aka VMAs)
ii. specification for each said VMA comprises of 4 numbers:
“starting_virtual_page ending_virtual_page write_protected[0/1] filemapped[0/1]”


*/
//

#ifndef Process_HPP
#define Process_HPP

#include "VirtualMemoryArea.hpp"
#include "PageTableEntry.hpp"

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

extern int PAGE_TABLE_SIZE;

// typedef struct ProcessStats
struct ProcessStats
{
	unsigned long unmaps;
	unsigned long maps;
	unsigned long ins;
	unsigned long outs;
	unsigned long fins;
	unsigned long fouts;
	unsigned long zeros;
	unsigned long segv;
	unsigned long segprot;

	ProcessStats(); // Overloaded constructor with initial values all as zeros 
};

// typedef struct Process
struct Process
{
	int processID;
	ProcessStats* processStats;
	// Note: Use vector to enable subscript operator 
	// std::list<VirtualMemoryArea*> vmas;
	// std::list<PageTableEntry*> pageTable; // 64 PTEs maximum
	std::vector<VirtualMemoryArea*> vmas;
	std::vector<PageTableEntry*> pageTable; // 64 PTEs maximum
	
	Process(int PID); // constructor with ProcessID
	bool is_write_protected(int vpageIdx); // correspond to @write_protected in VMA: binary whether the VMA is write protected or not
	bool is_file_mapped(int vpageIdx); // correspond to @file_mapped in VMA: binary to indicated whether the VMA is mapped to a file or not
	bool not_a_hole(int vpageIdx); // check if the requested virtual page is a hole 
	void printProcessStats(); // print the Process statistics
};


#endif


