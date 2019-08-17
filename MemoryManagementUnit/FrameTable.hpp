//
// HEader File
// // FrameTable.hpp
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

- Frame Table
- a **GLOBAL** frame_table that each operating system maintains to describe the usage of each of 
its physical frames and where you maintain reverse mappings to the process and the vpage that 
maps a particular frame (In this assignment a frame can only be mapped by at most one PTE at a time).

*/
//

#ifndef FrameTable_HPP
#define FrameTable_HPP

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

// typedef struct Frame
struct Frame
{
	// Note:
	// @age: For aging
	// The aging algorithm is a descendant of the NFU algorithm, with modifications to make it aware of the 
	// time span of use. Instead of just incrementing the counters of pages referenced, putting equal 
	// emphasis on page references regardless of the time, the reference counter on a page is first 
	// shifted right (divided by 2), before adding the referenced bit to the left of that binary number. 
	// @tau: For working set; the amount of memory that a process requires in a given time interval.
	// If the referenced bit is zero, the value in time of last use field is compared to the 
	//current total time value, and if the difference is greater than some value, tau, this page is removed.
	int frameID;
	unsigned long age; // For aging
	unsigned long tau; // For working set
	bool exited_flg; 
	std::pair <int, int> reverse_mapping; // Use reverse mapping to access the to the PTEs to maintain the reference and modified bits

	Frame(int fID); // 
};

// typedef struct FrameTable
struct FrameTable
{
	vector<Frame*> frameAll; // All frames that are assumed existing in the hardware
	list<Frame*> frameFreePool; // frames that are free to use

	FrameTable(int frameTableSize); // overloaded constructor with given frameTableSize, frame table list will reserve given size for potential usage
	Frame* allocate_frame_from_free_list(); // Get a free frame to use
	void free_one_frame(int frameIdx); // record that the frameIdx frame is free now
};


#endif


