//
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
 from its user ( <address space:vpage>), i.e. its entry in the owning process’s pageTable must be 
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

#include "PageReplacementAlgos.hpp"

// #include <iostream>
// #include <sstream>
// #include <fstream>

using namespace std;
extern int FRAME_TABLE_SIZE;

PageReplacementAlgos::PageReplacementAlgos(){}

PageReplacementAlgos::~PageReplacementAlgos(){}

Frame* PageReplacementAlgos::select_victim_frame(FrameTable* frameTable, vector<Process*> processVec, unsigned long instructionId)
{
	std::cerr << "Pager need to be specified to a page replacement algorithm. It should not be called directly.";
	abort();
	return nullptr;
}

void PageReplacementAlgos::getRandomNumber(string randFilePath){}

FIFOPager::FIFOPager(){}

FIFOPager::~FIFOPager(){}

Frame* FIFOPager::select_victim_frame(FrameTable* frameTable, vector<Process*> processVec, unsigned long instructionId)
{
	// First In First Out
	// The most recent arrival at the tail
	// On a page fault, the page at the head is removed 
	// Here, we implement FIFO with a simple round robin using a "hand" == pointer 
	if (!this->framesPhysical.empty())
	{
		Frame* frameToAssign = this->framesPhysical.front();
		this->framesPhysical.pop_front();
		return frameToAssign;
	}
	else
	{
		return nullptr;
	}
}

RandomPager::RandomPager(){}

RandomPager::~RandomPager()
{
	this->randomNumListGet_flg = false;
}

void RandomPager::getRandomNumber(string randFilePath)
{
	// Randomly remove a page based on pre-specified random numbers file
	// MUST get load the random numbers file before paging
	ifstream randFile;
	int randnums_num;
	randFile.open(randFilePath);
	//====================== Extract all random numbers from the randFile ==========================
	if (!randFile.good())
	{
		std::cerr << "Random Number file cannot be opend. " << endl;
		abort();
	}
	randFile >> randnums_num; // First line of the file
	this->randNumsPreDefined.reserve(randnums_num); // reserve necessary spaces
	int lineint;
	while (randFile >> lineint)
	{	
		// randvals[i] = lineint;
		this->randNumsPreDefined.push_back(lineint);
		// randFile >> randvals[i]; //append one random number at a time
	}
	randFile.close();

	this->randomNumListGet_flg = true;
}

Frame* RandomPager::select_victim_frame(FrameTable* frameTable, vector<Process*> processVec, unsigned long instructionId)
{
	// Randomly remove a page based on pre-specified random numbers file
	if (!this->randomNumListGet_flg)
	{
		std::cerr << "Random Numbers list should be specified before call the pager. " << endl;
		abort();
	}

	if (!this->framesPhysical.empty())
	{
		// if (this->ofs == this->randNumsPreDefined.size()) {this->ofs = 0;}
		int randIdx = this->randNumsPreDefined[this->ofs % this->randNumsPreDefined.size()] % frameTable->frameAll.size();
		this->ofs++;

		Frame* frameToAssign = frameTable->frameAll[randIdx];
		return frameToAssign;
	}
	else
	{
		return nullptr;
	}
}

ClockPager::ClockPager(){}

ClockPager::~ClockPager(){}

Frame* ClockPager::select_victim_frame(FrameTable* frameTable, vector<Process*> processVec, unsigned long instructionId)
{
	// The Clock Page Replacement Policy
	// - Keep page frames on a circular list in the form of a clock
	// - The hand points to the oldest uninspected page
	// - When page fault occurs
	// -- The page pointed to by the hand is inspected
	// -- If R == 0:
	//		1. page evicted
	//		2. new page inserted into its place
	//		3. hand is advanced
	// -- If R == 1:
	// 		1. R is set to 0
	// 		2. hand is advanced
	// Ref: https://github.com/alizhiyu46/OS-LAB3/blob/master/pager.cpp 
	// Line 49-73; Rewrite without looking 
	Frame* frameOnHand;
	Process* curProcess;
	PageTableEntry* curPTE;

	if (!this->framesPhysical.empty())
	{
		frameOnHand = this->framesPhysical.front();
		curProcess = processVec[frameOnHand->reverse_mapping.first];
		curPTE = curProcess->pageTable[frameOnHand->reverse_mapping.second];

		// if R == 1
		while (curPTE->REFERENCED == 1) 
		{
			// 1. R is set to 0
			curPTE->REFERENCED = 0;
			// 2. hand is advanced
			this->framesPhysical.pop_front();
			this->framesPhysical.push_back(frameOnHand);
			frameOnHand = this->framesPhysical.front(); // reset the page pointed
			curProcess = processVec[frameOnHand->reverse_mapping.first];
			curPTE = curProcess->pageTable[frameOnHand->reverse_mapping.second];
		}

		// if R == 0
		this->framesPhysical.pop_front(); // page is evicted
		while (frameOnHand->exited_flg) // The hand points to the oldest uninspected page 
		{
			frameOnHand->exited_flg = false; // inspected
			frameOnHand = select_victim_frame(frameTable, processVec, instructionId); // Circle through all pages/frames // The hand points to the oldest uninspected page 
		}

		return frameOnHand;
	}
	else 
	{
		return nullptr;
	}
}

ESCNRUPager::ESCNRUPager()
{
	// overloaded constructor
	// initialize with empty class records, all as null pointers
	instrNumLastRefBitReset = 0;
	hand = 0;
	// this->this->framesFourNRUClasses.reserve(4);
	for (int i = 0; i < 4; i++)
	{
		this->framesFourNRUClasses[i] = nullptr; // initialize empty record list
	}
}

ESCNRUPager::~ESCNRUPager(){}

Frame* ESCNRUPager::select_victim_frame(FrameTable* frametable, vector<Process*> processes, unsigned long instructionidx)
{	
	// The Not Recently Used(NRU) Replacement; a.k.a. Enhanced Second Chance(ESC) Replacement Algorithm
	// Two status bits with each page
	// - R: set whenever the page is referenced (used)
	// - M: set when the page is written/dirty
	// R and M bits are available in most computers implementing virtual memory
	// Those bits are updated with each memory reference 
	// - Must be updated by hardware
	// - Reset only by the OS
	// Periodically (e.g. on each clock interrupt) the R bit is cleared 
	// simulate the periodic time inside the select_victim_frame function.
	// If [50] or more instructions have passed since the last time the reference bits were reset, then the reference bit of the pte’s reached by the frame traversal should be cleared after you considered the class of that frame/page. 
	// - to distinguish pages that have been referenced recently

	// R(2) * M(2) 4 classes
	// ** NRU Algo removes a page at random from the lowest numbered un-empty **
	// Note: but here we select the first page encountered in a class
	// so we dont have to track all of them, we also use a clock like algorighm  to circle through pages/frames
	// Note: Ref-Bit set doesn't happen each and every search. 
	// Typically done at specific times through a daemon 
	// Note: class_index = 2*R+M

    Frame* frameOnHand;
    Frame* victimFrame = nullptr;
    Process* curProc;
    PageTableEntry* curPTE;
    int class_index;
    //has to clear again, every time
    for (int i = 0; i < 4; i++) 
    {
        this->framesFourNRUClasses[i] = nullptr;
    }
    
    for (int i = 0; i < frametable->frameAll.size(); i++) 
    {
        //to get curPTE
        int fidx = (i + this->hand) % frametable->frameAll.size(); // frame idx
        frameOnHand = frametable->frameAll[fidx];
        // cout << "------frameOnHand ID " << frameOnHand->frameID << endl;
        // cout << "FIRST SECOND " << procID << ":" << curPTEIdx << endl;
        curProc = processes[frameOnHand->reverse_mapping.first];
        curPTE = curProc->pageTable[frameOnHand->reverse_mapping.second];
        class_index = curPTE->MODIFIED + curPTE->REFERENCED * 2;
        // cout << "REFERENCED MODIFIED " << curPTE->REFERENCED << ":" << curPTE->MODIFIED << endl;


        // cout << "----- FIDX CALCULATION "<< calRes << endl;
        // cout << "----- class index " << class_index << endl;

        
        if (this->framesFourNRUClasses[class_index] == nullptr)
        {
            if (class_index == 0) 
            {
                victimFrame = frameOnHand;
                //set hand to victimFrame next
                this->hand = (fidx + 1) % frametable->frameAll.size();
                // cout << "------ reset hand " << this->hand << endl;
                break;
            }
            this->framesFourNRUClasses[class_index] = frameOnHand;
        }
    }
    if (!victimFrame) {
        for (int i = 1; i < 4; i++) 
        {
            if (this->framesFourNRUClasses[i] != nullptr) 
            {
                victimFrame = this->framesFourNRUClasses[i];
                this->hand = (victimFrame->frameID + 1) % frametable->frameAll.size(); // update hand
                break;
            }
        }
    }
    
    //reset after consider the page, after 50 instructions,
    if (instructionidx - instrNumLastRefBitReset >= 50) 
    {
        for (int i = 0; i < frametable->frameAll.size(); i++) 
        {
            frameOnHand = frametable->frameAll[i];
            int procID = frameOnHand->reverse_mapping.first;
            if (procID != -1) 
            {
                processes[procID]->pageTable[frameOnHand->reverse_mapping.second]->REFERENCED = 0;
            }
        }
        instrNumLastRefBitReset = instructionidx;
    }
    // cout << "SELECTED FREAME ID " << victimFrame->frameID << endl;
    return victimFrame;
}

AgingPager::AgingPager()
{
	hand = 0;
}

AgingPager::~AgingPager(){}

Frame* AgingPager::select_victim_frame(FrameTable* frameTable, vector<Process*> processVec, unsigned long instructionId)
{
	// In order to solve the issue that NRU never forgets anything
	// Modification: - Called **Aging**
	// - shift counter right by 1
	// - add R bit as the left most bit 
	// - reset R bit
	// The Page whose counter is lowest is replaced at page replacement
	Frame* frameOnHand;
	Frame* victimFrame = frameTable->frameAll[hand];
	Process* curProcess;
	PageTableEntry* curPTE;

	// for (int i = 0; i < frameTable->frameAll.size(); i++)
	for (int i = 0; i < FRAME_TABLE_SIZE; i++)
	{
		// frameOnHand = frameTable->frameAll[(i + this->hand) % frameTable->frameAll.size()]; // Get frame by reading index into the frame table
		frameOnHand = frameTable->frameAll[(i + this->hand) % frameTable->frameAll.size()]; // Get frame by reading index into the frame table
		int calRes = (i + this->hand) % frameTable->frameAll.size();
		curProcess = processVec[frameOnHand->reverse_mapping.first];
		curPTE = curProcess->pageTable[frameOnHand->reverse_mapping.second];

		frameOnHand->age = frameOnHand->age >> 1; // right shift to the right
		if (curPTE->REFERENCED) // currently mapped
		{
			// frameOnHand->age = (frameOnHand->age | 0x80000000); // set the leading bit to one
			frameOnHand->age = frameOnHand->age | ((unsigned long)1 << 31);
			curPTE->REFERENCED = 0;
		}
		if (frameOnHand->age < victimFrame->age)
		{
			victimFrame = frameOnHand;
		}
	}

 	// int index = (i + this->hand) % frameTable->frameAll.size();
	// for (int i = 0; i < frameTable->frameAll.size(); i++)
	// for (int i = 0; i < FRAME_TABLE_SIZE; i++)
	// {
	// 	if (curPTE->REFERENCED == 1)
	// 	{
	// 		frameOnHand->age += curPTE->REFERENCED << 31;
	// 		curPTE->REFERENCED = 0; // reset reference bit after getting the victim
	// 	}

	// 	if (frameOnHand->age >= frameTable->frameAll[0]->age)
	// 	{
	// 		victimFrame = frameTable->frameAll[0];
	// 	}
	// 	else if(frameOnHand->age < frameTable->frameAll[0]->age)
	// 	{
	// 		victimFrame = frameOnHand;
	// 	}
	// 	else {}
	// }
	// this->hand = (victimFrame->frameID + 1) % frameTable->frameAll.size();  //set hand to victim next
	this->hand = (victimFrame->frameID + 1) % FRAME_TABLE_SIZE;  //set hand to victim next
	// cout << "SELECTED FREAME ID " << victimFrame->frameID << endl;
	return victimFrame;
}

WorkingSetPager::WorkingSetPager()
{
	hand = 0;
}

WorkingSetPager::~WorkingSetPager(){}

Frame* WorkingSetPager::select_victim_frame(FrameTable* frameTable, vector<Process*> processVec, unsigned long instructionId)
{
	// Working Set: the set of pages that a process is currently using
	// Thrashing: a program causing page faults at high rates(e.g. pagefaults/instructions metric)
	// We assume TAU = 50; so if 50 or more instructions have passed since the time of "last use" was recorded in the frame and the reference bit is not set, then this frame will be selected (see algo).
	Frame* frameOnHand;
	// Frame* victimFrame = frameTable->frameAll[hand];
	Frame* victimFrame = nullptr;
	Process* curProcess;
	PageTableEntry* curPTE;
	int maxDiff = INT_MIN; // instructionId - frameOnHand->tau - 1

	// for (int i = 0; i < frameTable->frameAll.size(); i++)
	for (int i = 0; i < FRAME_TABLE_SIZE; i++)
	{
		frameOnHand = frameTable->frameAll[(i + this->hand) % frameTable->frameAll.size()];// Get frame by reading index into the frame table
		int calRes = (i + this->hand) % frameTable->frameAll.size();
		curProcess = processVec[frameOnHand->reverse_mapping.first];
		curPTE = curProcess->pageTable[frameOnHand->reverse_mapping.second];
		

		if (curPTE->REFERENCED == 0)
		{	
			int diff = instructionId - frameOnHand->tau - 1;
			// if (instructionId - frameOnHand->tau - 1 >= 50)
			if (diff >= 50)
			{
				victimFrame = frameOnHand;
				break;
			}
			// else if (instructionId - frameOnHand->tau - 1 < 50)
			else if (diff < 50)
			{
				// if (instructionId - frameOnHand->tau - 1 > maxDiff)
				if (diff > maxDiff)
				{
					// maxDiff = instructionId - frameOnHand->tau - 1;
					maxDiff = diff;
					victimFrame = frameOnHand;
				}
			}
			// cout << "DIFF " << diff << endl;
		}
		else // if (curPTE->REFERENCED == 1) // the ones that are currently mapped
		{
			curPTE->REFERENCED = 0; // reset the reference bit 
			frameOnHand->tau = instructionId - 1;
		}
	}

	if (victimFrame == nullptr)
	{
		victimFrame = frameTable->frameAll[hand];
	}

	this->hand = (victimFrame->frameID + 1) % frameTable->frameAll.size();  //set hand to victim next
	// cout << "SELECTED FREAME ID " << victimFrame->frameID << endl;
	return victimFrame;
}




