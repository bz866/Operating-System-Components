//
// // FrameTable.cpp
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

-Frame
- each frame has corresponding mapping
Meta Data includes
– reverse mapping ( pid, vpage) of user
– Helper data for algorithm (e.g. age, tau)

*/
//


#include "FrameTable.hpp"

using namespace std;



Frame::Frame(int fID) //Overloaded constructor with initial values and given frameID
{
	this->frameID = fID;
	this->age = 0;
	this->tau = 0;
	this->exited_flg = false;
	this->reverse_mapping = pair<int, int>(-1, -1);
}

FrameTable::FrameTable(int frameTableSize)
{
	// this->frameAll.reserve(frameTableSize);
	// this->frameFreePool.reserve(frameTableSize);
	// this->frameFreePool.resize(frameTableSize);
	//this->frameFreePool.reserve(frameTableSize);
	for (int i = 0; i < frameTableSize; i++)
	{
		this->frameAll.push_back(new Frame(i)); // initialize all frames with their ID
		// this->frameFreePool[i] = this->frameAll[i]; // all frames are free in the beginning
		this->frameFreePool.push_back(this->frameAll[i]); // all frames are free in the beginning
	}
};

Frame* FrameTable::allocate_frame_from_free_list()
{
	// Get a frame to use
	// if (!frameFreePool.empty())
	if(frameFreePool.size()>0)
	{
		Frame* frameToUse = this->frameFreePool.front();
		this->frameFreePool.pop_front();
		// cout << "------------ ALLOCATE FRAME ID " << frameToUse->frameID << endl;
		return frameToUse; // free frames available 
	}
	return nullptr; // no free frames available 
};

void FrameTable::free_one_frame(int frameIdx)
{
	this->frameFreePool.push_back(this->frameAll[frameIdx]); // free a frame to use later
};



