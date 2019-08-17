//
// // Process.cpp
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

*/
//


#include "Process.hpp"

ProcessStats::ProcessStats() // Overloaded default constructor with all zeros initial values
{
	this->unmaps = 0;
	this->maps = 0;
	this->ins = 0;
	this->outs = 0;
	this->fins = 0;
	this->fouts = 0;
	this->zeros = 0;
	this->segv = 0;
	this->segprot = 0;
}

Process::Process(int PID)
{
	this->processID = PID;
	this->processStats = new ProcessStats();
	for (int i = 0; i < PAGE_TABLE_SIZE; i++)
	{
		this->pageTable.push_back(new PageTableEntry());
	}
}

bool Process::is_write_protected(int vpageIdx) // correspond to @write_protected in VMA: binary whether the VMA is write protected or not
{
	for (int vmaIdx = 0; vmaIdx < this->vmas.size(); vmaIdx++)
	{
		if (vpageIdx >= this->vmas[vmaIdx]->start_vpage && vpageIdx <= this->vmas[vmaIdx]->end_vpage)
		{
			if (this->vmas[vmaIdx]->write_protected == 0) // 1 as protected 0 as not protected
			{
				return false; // not write protected
			}
			else
			{
				return true; // not write protected
			}
		}
	}

	return false; // vpageIdx not within vmas range
}

bool Process::is_file_mapped(int vpageIdx) // correspond to @file_mapped in VMA: binary to indicated whether the VMA is mapped to a file or not
{
	for (int vmaIdx = 0; vmaIdx < this->vmas.size(); vmaIdx++)
	{
		if (vpageIdx >= this->vmas[vmaIdx]->start_vpage && vpageIdx <= this->vmas[vmaIdx]->end_vpage)
		{
			if (this->vmas[vmaIdx]->file_mapped == 0) // // 1 as mapped 0 as not mapped
			{
				return false; // file not mapped
			}
			else 
			{
				return true; // file mapped
			}
		}
	}
	return false; // vpageIdx not within vmas range
}

bool Process::not_a_hole(int vpageIdx)
{
	// check if the requested virtual page is a hole 
	for (int i = 0; i < this->vmas.size(); i++) // check if vpageIdx in VMAs table 
	{
		if(this->vmas[i]->start_vpage <= vpageIdx && vpageIdx <= this->vmas[i]->end_vpage)
		{
			return true;
		}
	}
	return false;
}

void Process::printProcessStats()
{
	printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
            this->processID,
			this->processStats->unmaps, this->processStats->maps, this->processStats->ins, this->processStats->outs,
			this->processStats->fins, this->processStats->fouts, this->processStats->zeros,
			this->processStats->segv, this->processStats->segprot);
}





