//
// HEader File
// // VirtualMemoryArea.hpp
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

- Virtual memory area (VMA)
The kernel uses virtual memory areas to keep track of the process's memory mappings; for example, 
a process has one VMA for its code, one VMA for each type of data, one VMA for each distinct memory 
mapping (if any), and so on. VMAs are processor-independent structures, with permissions and access 
control flags. Each VMA has a start address, a length, and their sizes are always a multiple of the 
page size (PAGE_SIZE). A VMA consists of a number of pages, each of which has an entry in the page 
table.

Note: 
a specification for each process’ address space is comprised of
i. the number of virtual memory areas / segments (aka VMAs)
ii. specification for each said VMA comprises of 4 numbers:
	“starting_virtual_page ending_virtual_page write_protected[0/1] filemapped[0/1]”

- Each VMA is comprised of 4 numbers.
start_vpage:
end_vpage: (note the VMA has (end_vpage – start_vpage + 1) virtual pages ) write_protected: binary whether the VMA is write protected or not
file_mapped: binary to indicated whether the VMA is mapped to a file or not



*/
//

#ifndef VirtualMemoryArea_HPP
#define VirtualMemoryArea_HPP

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

// typedef struct VirtualMemoryArea
struct VirtualMemoryArea
{
	int start_vpage;
	int end_vpage;
	int write_protected;
	int file_mapped;

	// Overload constructor with given initial values 
	VirtualMemoryArea(int start_vpg, int end_vpg, int write_prtctd, int file_mppd);
};




#endif

