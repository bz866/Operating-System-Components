//
// HEader File
// // PageReplacementAlgos.hpp
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

- We explore the implementation of several page replacement algorithms. Page replacement implies the 
identification of a victim frame according to the algorithm’s policy. This should be implemented as 
a subclass of a general Pager class with one virtual function “frame_t* select_victim_frame();” that
 returns a victim frame. Once a victim frame has been determined, the victim frame must be unmapped 
 from its user ( <address space:vpage>), i.e. its entry in the owning process’s page_table must be 
 removed (“UNMAP”), however you must inspect the state of the R and M bits. If the page was modified, 
 then the page frame must be paged out to the swap device (“OUT”) or in case it was file mapped written 
 back to the file (“FOUT”). Now the frame can be reused for the faulting instruction. 
 First the PTE must be reset (note once the PAGEDOUT flag is set it will never be reset as it indicates 
 there is content on the swap device) and then the PTE’s frame must be set.

- The page replacement should be generic and the algorithms should be special instances of the 
page replacement class to avoid “switch/case statements” in the simulation of instructions. 
Use object oriented programming and inheritance.

- Algorithms to implement
FIFO
f
Random
r
Clock
c
Enhanced Second Chance / NRU
e
Aging
a
Working Set
w

*/
//


#ifndef PageReplacementAlgos_HPP
#define PageReplacementAlgos_HPP

#include "Process.hpp"
#include "FrameTable.hpp"
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
#include <climits>


using namespace std;

class PageReplacementAlgos
{
public:
	list<Frame*> framesPhysical; // OS maintains a list of the pages currently in memory

	PageReplacementAlgos();
	~PageReplacementAlgos();
	virtual Frame* select_victim_frame(FrameTable* frameTable, vector<Process*> processlist, unsigned long instructionId);
	virtual void getRandomNumber(string randFilePath);
};

class FIFOPager : public PageReplacementAlgos
{
public:
	FIFOPager();
	~FIFOPager();
	Frame* select_victim_frame(FrameTable* frameTable, vector<Process*> processlist, unsigned long instructionId);
	
};

class RandomPager : public PageReplacementAlgos
{
public: 
	bool randomNumListGet_flg; // default false
	std::vector<int> randNumsPreDefined; // use vector to enable suscription operation
	int ofs;

	RandomPager();
	// RandomPager(string* randFilePath);
	~RandomPager();
	Frame* select_victim_frame(FrameTable* frameTable, vector<Process*> processlist, unsigned long instructionId);
	void getRandomNumber(string randFilePath);
};


class ClockPager : public PageReplacementAlgos
{
public:
	ClockPager();
	~ClockPager();
	
	Frame* select_victim_frame(FrameTable* frameTable, vector<Process*> processlist, unsigned long instructionId);
};


class ESCNRUPager : public PageReplacementAlgos
{
	// Enhanced Second Chance / Not Recently Used Pager
public:
	list<Frame*> framesPhysical; // OS maintains a list of the pages currently in memory
	// first frame in four NRU classes; 0, 0; 0, 1; 1, 0; 1, 1;
	// vector<Frame*> framesFourNRUClasses[4]; //  only have to remember the first frame that falls into each class as it would be the one picked for that class.
	// vector<Frame*> framesFourNRUClasses; //  only have to remember the first frame that falls into each class as it would be the one picked for that class.
	Frame* framesFourNRUClasses[4];
	// Frame* framesFourNRUClasses[4];
	unsigned long instrNumLastRefBitReset = 0; // Reset reference bit if instructionId - instrNumLastRefBitReset >= 50
	int hand; // initalize as zero with no effect. Once a victim frame is determined, the hand is set to the next position after the victim frame for the next select_victim_frame() invocation.

	ESCNRUPager(); // overloaded constructor with all null pointers in list<Frame*>framesFourNRUClasses
	~ESCNRUPager();
	Frame* select_victim_frame(FrameTable* frameTable, vector<Process*> processlist, unsigned long instructionId);
};


class AgingPager : public PageReplacementAlgos
{
public:
	int hand; // initialize as zero with no effect.  Once the victim frame is determined, the hand is set to the next position after the victim frame for the next select_victim() invocation.

	AgingPager();
	~AgingPager();	
	Frame* select_victim_frame(FrameTable* frameTable, vector<Process*> processlist, unsigned long instructionId);	
};

class WorkingSetPager : public PageReplacementAlgos
{
public:
	int hand;// initialize as zero with no effect.  Once the victim frame is determined, the hand is set to the next position after the victim frame for the next select_victim() invocation.

	WorkingSetPager();
	~WorkingSetPager();
	Frame* select_victim_frame(FrameTable* frameTable, vector<Process*> processlist, unsigned long instructionId);	
};



#endif
