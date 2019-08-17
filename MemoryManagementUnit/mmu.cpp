// mmu.hpp
//
//
// main File
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
The main file of the Memory Mangement Unit

Implementation of different page replacement algorithms {FIFO, Random, ESC/NRU, Clock, Aging, Working_Set}

Ref: 
https://www.cs.princeton.edu/courses/archive/fall16/cos318/lectures/20.VirtualMachines.pdf
https://github.com/teyuhuang/2018_OS_Lab3_MMU
https://github.com/alizhiyu46/OS-LAB3

To Run:

./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputFile randomFile (optional arguments in any order).
e.g. ./mmu –ac –o[OPFS] infile rfile selects the Clock Algorithm and creates output for operations, 
final page table content and final frame table content and summary line (see above). 
The outputs should be generated in that order if specified in the option string regardless how the order 
appears in the option string. 

Note:

The test input files and the sample file with random numbers are supplied. The random file is required for the Random algorithm. Please reuse the code you have written for lab2, but note the difference in the modulo function which now indexes into [ 0, size ) vs previously ( 0, size ]. In the Random replacement algorithm you compute the frame selected as with (size==num_frames). As in the lab2 case, you increase the rofs and wrap around from the input file.
• The ‘O’ (ohhh) option shall generate the required output as shown in output-1/3.
• The ‘P’ (pagetable option) should print after the execution of all instructions the state of the pagetable:
As a single line for each process, you print the content of the pagetable pte entries as follows:
PT[0]: 0:RMS 1:RMS 2:RMS 3:R-S 4:R-S 5:RMS 6:R-S 7:R-S 8:RMS 9:R-S 10:RMS 11:R-S 12:R-- 13:RM- # # 16:R-- 17:R-S # # 20:R-- # 22:R-S 23:RM- 24:RMS # # 27:R-S 28:RMS # # # # # 34:R-S 35:R-S # 37:RM- 38:R-S * # 41:R-- # 43:RMS 44:RMS # 46:R-S * * # * * * # 54:R-S # * * 58:RM- * * # * *
R (referenced), M (modified), S (swapped out) (note we don’t show the write protection bit as it is implied/inherited from the specified VMA.
Pages that are not valid are represented by a ‘#’ if they have been swapped out (note you don’t have to swap out a page if it was only referenced but not modified), or a ‘*’ if it does not have a swap area associated with. Otherwise (valid) indicates the virtual page index and RMS bits with ‘-‘ indicated that that bit is not set.
Note a virtual page, that was once referenced, but was not modified and then is selected by the replacement algorithm, does not have to be paged out (by definition all content must still be ZERO) and can transition to ‘*’.
• The ‘F’ (frame table option) should print after the execution and should show which frame is mapped at the end to which <pid:virtual page> or ‘*’ if not currently mapped by any virtual page.
FT: 0:32 0:42 0:4 1:8 * 0:39 0:3 0:44 1:19 0:29 1:61 * 1:58 0:6 0:27 1:34
• The ‘S’ option prints the summary line (“SUM ...”) described above.
• The ‘x’ options prints the current page table after each instructions (see example outputs) and this should help you
significantly to track down bugs and transitions
• The ‘y’ option is like ‘x’ but prints the page table of all processes instead.
• The ‘f’ option prints the frame table after each instruction.
• The ‘a’ option prints additional “aging” information during victim_select and after each instruction for complex
algorithms (not all algorithms have the details described in more detail below)
We will not test or use the ‘-f’ ,’-a’ or the ‘-x,-y’ option during the grading. It is purely for your benefit to add these and compare with the reference program under ~frankeh/Public/mmu on any assigned cims machines.
(Note only a max of 10 processes and 8 VMAs per process are supported in the reference program which means that is the max we test with).

*/
//
//

#include "PageTableEntry.hpp"
#include "VirtualMemoryArea.hpp"
#include "FrameTable.hpp"
#include "Process.hpp"
#include "PageReplacementAlgos.hpp"
#include "Utils.hpp"

// #include <iomanip>
// #include <iostream>
// #include <string.h>
// #include <sstream>      // std::istringstream
// #include <istream>
// #include <ctype.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <utility>
// #include <list>
// #include <vector>
// #include <getopt.h>
// #include <unistd.h>

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

/* your program needs to compute and print the summary statistics related to the VMM if r
equested by an option. This means it needs to track the number of instructions, segv, segprot, unmap, 
map, pageins (IN, FIN), pageouts (OUT, FOUT), and zero operations for each process. In addition you should 
compute the overall execution time in cycles, where maps and unmaps each cost 400 cycles, page-in/outs each 
cost 3000 cycles, file in/outs cost 2500 cycles, zeroing a page costs 150 cycles, a segv costs 240 cycles, a 
segprot costs 300 cycles and each access (read or write) costs 1 cycles and a context switch costs 121 cycles
 and process exit costs 175 cycles.
 */

unsigned int MAP_COST = 400;
unsigned int UNMAP_COST = 400;
unsigned int PAGE_IN_OUT_COST = 3000;
unsigned int FILE_IN_OUT_COST = 2500;
unsigned int ZEROING_PAGE = 150;
unsigned int SEGV_COST = 240;
unsigned int SEGPROT_COST = 300;
unsigned int ACCESS_COST = 1;
unsigned int CONTEXT_SWITCH_COST = 121;
unsigned int PROCESS_EXIT_COST = 175;

int FRAME_TABLE_SIZE = 16;
int PAGE_TABLE_SIZE = 64;
string VALID_OPERATIONS = "crwe";


int main(int argc, char *argv[]) // dont use char const *argv[] in with getopt
{
	string inputFilePath; // file of instructions specified by argument in commend line
	string randomFilePath; // file of pre-defined random numbers in commend line [optional]
	bool randomFlg = false;
	PageReplacementAlgos* pager; // page replacement algorithms specified by argument -a
	bool O_optflg, P_optflg, F_optflg, S_optflg; // Option flags specified by argument -o
	int num_frames = 16; //default 16 or 32 In the Random replacement algorithm you compute the frame selected as with (size==num_frames)
	int c; // argument parser 
	int index; // the index for parsing file names from commend line 
	string algoNameArg, OPFSArg, numFramesArg;

	// ==================================== Parsing Arguments =============================================
	while ((c = getopt(argc, argv, "a:o:f:")) != -1)
	{
		switch (c)
		{
			// ================================= Specify the Page Replacement Algorithm =============================
			case 'a':
			{
				string algoNameArg = optarg;
				switch (algoNameArg[0])
				{
				case 'f':
					pager = new class FIFOPager();
					break;

				case 'r':
				{
					pager = new class RandomPager();
					// pager->getRandomNumber(randomFilePath); // Note: read file in switch case failed
					randomFlg = true;
					break;
				}

				case 'c':
					pager = new class ClockPager();
					break;

				case 'e':
					pager = new class ESCNRUPager();
					break;

				case 'a':
					pager = new class AgingPager();
					break;

				case 'w':
					pager = new class WorkingSetPager();
					break;

				default:
					std::cerr << "Invalid page replacement algorithm; Only support 'f, r, c, e, a, w'. " << endl;
					abort();
				}
				break;
			}

			// ================================= Parse options arguments OPFS ======================================
			case 'o':
			{
				string OPFSArg = optarg;
				for (int i = 0; i < OPFSArg.size(); i++)
				{
					switch (OPFSArg[i])
					{
					case 'O':
						O_optflg = true;
						break;

					case 'P':
						P_optflg = true;
						break;
					
					case 'F':
						F_optflg = true;
						break;
					
					case 'S':
						S_optflg = true;
						break;
					
					default:
						std::cerr << "Invalid option name; Only support 'OPFS'. " << endl;
						abort(); 
						break;
					}
				}
				break;
			}

			case 'f':
			{
				string numFramesArg = optarg;
				if (!numFramesArg.empty())
				{
					FRAME_TABLE_SIZE = atoi(numFramesArg.c_str());
					// string::size_type sz; //alias of size_t
					// FRAME_TABLE_SIZE = stoi(numFramesArg.substr(1, numFramesArg.length()-1), &sz, 10);
				}
				break;	
			}

			default:
			{
				std::cerr << "Invalid argument, only allow '-a -o -f'. "<< endl;
				abort();
				break;
			}
		}
	}

	// ================================== Get File Names ===================================================
	index = optind;
	inputFilePath = argv[index]; // Get file of instruction
	index++;
	randomFilePath = argv[index]; // get file of random number

	// =============================== parse the file of instructions ======================================
	int numProcess = 0; // number of processes in the file of instructions
	int numVMAs = 0; // number of VMAs in one process
	std::vector<Process*> processVec; // all processes from the input file
	Process* curProcess = nullptr; 
	FrameTable* frameTable = new FrameTable(FRAME_TABLE_SIZE); 
	unsigned long instrCnt = 0; 
	unsigned long ctx_switches = 0;
	unsigned long process_exits = 0;
	unsigned long total_cycles = 0;
	unsigned long long total_cost = 0;

	char linehead;
	string line;
	int starting_virtual_page;
	int ending_virtual_page;
	int write_protected; // [0/1] 
	int filemapped; // [0/1]

	ifstream inputFile;
	inputFile.open(inputFilePath);
	if (!inputFile.good())
	{
		std::cerr << "File of instructions cannot be opend. " << endl;
		abort();
	}
	if (randomFlg)
	{
		pager->getRandomNumber(randomFilePath);
	}

	// The input to your program will be a comprised of:
	// 1. the number of processes (processes are numbered starting from 0)
	// 2. a specification for each process’ address space is comprised of
	// i. the number of virtual memory areas / segments (aka VMAs)
	// ii. specification for each said VMA comprises of 4 numbers:
	// “starting_virtual_page ending_virtual_page write_protected[0/1] filemapped[0/1]”

	while (getline(inputFile, line)) // read file line by line
	{
		istringstream linestream(line);
		char linehead;
		linestream >> linehead;
		if (linehead == '#') {continue;}

		// Note: in ASCII code, the numbers (digits) start from 48. 
		numProcess = (int)linehead - 48; // 1. the number of processes (processes are numbered starting from 0)
		processVec.reserve(numProcess);
		// for (int i = 0; i < processVec.size(); i++)
		for (int i = 0; i < numProcess; i++)
		{
			Process* proc = new Process(i);
			// get process content
			getline(inputFile, line);
			istringstream linestream(line);
			linestream >> linehead;
			if (linehead == '#') // title of a process
			{
				i--; // line starting with # is not a VMA
				continue;
			}
			// Note: in ASCII code, the numbers (digits) start from 48. 
			numVMAs = (int)linehead - 48;
			proc->vmas.reserve(numVMAs); 
			// for (int cnt = 0; cnt < proc->vmas.size(); cnt++
			for(int cnt = 0; cnt < numVMAs; cnt++)
			{
				getline(inputFile, line);
				istringstream linestream(line);
				linestream >> starting_virtual_page >> ending_virtual_page >> write_protected >> filemapped; // parse 4 numbers in one VMA
				VirtualMemoryArea* vma = new VirtualMemoryArea(starting_virtual_page, ending_virtual_page, write_protected, filemapped); // new vma
				// proc->vmas[cnt] = vma; // store new vma
				proc->vmas.push_back(vma);
			}
			// processVec[i] = proc; // store new process
			processVec.push_back(proc);
		}

		//================================== parsing operations/instructions =================================
		string operation;
		string vpage;
		while (get_next_instruction(inputFile, operation, vpage))
		{
			// cout << "FIRST instructions GET" << endl;
			int vpageIdx; // The index to access the corresponding virtual page
			vpageIdx = std::stoi(vpage);
			if (O_optflg)
			{
				cout << instrCnt << ": ==> " << operation << " " << vpage << endl;
			}
			// cout << "VPAGE INDEX " << vpageIdx << endl;
			instrCnt++; 

			if (operation == "c") // context switch
			{
				curProcess = processVec[vpageIdx];
				ctx_switches += 1;
				total_cycles += 1;
				total_cost += CONTEXT_SWITCH_COST;
			} // end of context switch situation
			else if (operation == "r") // load/read operation
			{
				total_cycles += 1;
				total_cost += 1;
				if (curProcess->not_a_hole(vpageIdx))
				{
					PageTableEntry* curPTE = curProcess->pageTable[vpageIdx];
					Frame* selectedFrame = nullptr;
					if (curPTE->PRESENT) // current PTE is already presented 
					{
						// cout << "PRESENT TRIGGERED" << endl;
						selectedFrame = frameTable->frameAll[curPTE->frameNum];
					}
					else// !curPTE->PRESENT // current PTE not presented
					{
						selectedFrame = frameTable->allocate_frame_from_free_list(); // get a free frame for the current PTE
						// if (selectedFrame != nullptr)
						// {
						// 	cout << "ALLOCATE FRAME ID" << selectedFrame->frameID << endl;
						// 	cout << "ALLOCATE FRAME TABLE SIZE" <<  frameTable->frameAll.size() << ":" << frameTable->frameFreePool.size() << endl;
						// }
						// else 
						// {
    		// 				cout << "ALLOCATE NULLPTR" << endl;
						// }
						// cout << "FRAME TABLE SLOTS" << frameTable->frameAll.size() << ":" << frameTable->frameFreePool.size() << endl; 
						if (selectedFrame == nullptr) // no free frames available
						{	
							selectedFrame = pager->select_victim_frame(frameTable, processVec, instrCnt);
							// if (selectedFrame != nullptr)
							// {
							// 	cout << "SELECTED FRAME ID" << selectedFrame->frameID << endl;
							// 	cout << "SELECTED FRAME TABLE SIZE" <<  frameTable->frameAll.size() << ":" << frameTable->frameFreePool.size() << endl;
							// }
							// else 
							// {
							// 	cout << "SELECTED NULLPTR" << endl;
							// }
							// cout << "SELECT OK" << endl;
							// cout << selectedFrame->reverse_mapping.first << endl;
							// solve Unmap issue 
							Process* oriProcess = processVec[selectedFrame->reverse_mapping.first];
							PageTableEntry* oriPTE = oriProcess->pageTable[selectedFrame->reverse_mapping.second];
							// int original_pid = selectedFrame->reverse_mapping.first;
							// int original_pteIdx = selectedFrame->reverse_mapping.second;
							// Process* oriProcess = processVec[original_pid];
							// PageTableEntry* oriPTE = oriProcess->pageTable[original_pteIdx];
							// cout << "SELECT OK" << endl;

							// cout << "ORIGINAL " << selectedFrame->reverse_mapping.first << ":" << selectedFrame->reverse_mapping.second << endl;
							
							oriProcess->processStats->unmaps += 1;
							oriPTE->PRESENT = 0;
							oriPTE->REFERENCED = 0;
							total_cycles += 1;
							total_cost += UNMAP_COST; 
							if (O_optflg)
							{
								cout << " UNMAP " << selectedFrame->reverse_mapping.first << ":" 
									 << selectedFrame->reverse_mapping.second << endl;
							}

							// modified/dirty, OUT & FOUT 
							if (oriPTE->MODIFIED) // dirty
							{
								oriPTE->MODIFIED = 0; // reset M
								if (oriProcess->is_file_mapped(selectedFrame->reverse_mapping.second)) //FOUT
								{
									oriProcess->processStats->fouts += 1;
									total_cycles += 1;
									total_cost += FILE_IN_OUT_COST;
									if (O_optflg)
									{
										cout << " FOUT" << endl;
									}
								}
								else
								{
									oriProcess->processStats->outs += 1;
									oriPTE->PAGEDOUT = 1;
									total_cycles += 1;
									total_cost += PAGE_IN_OUT_COST;
									if (O_optflg)
									{
										cout << " OUT" << endl;
									}
								}
							}
						} // end of no free frames available handling

						if (curProcess->is_file_mapped(vpageIdx)) // FIN
						{
							curProcess->processStats->fins += 1;
							total_cycles += 1;
							total_cost += FILE_IN_OUT_COST;
							if (O_optflg)
							{
								cout << " FIN" << endl;
							}
						}
						else if (curPTE->PAGEDOUT) // IN
						{
							curProcess->processStats->ins += 1;
							total_cycles += 1;
							total_cost += PAGE_IN_OUT_COST;
							if (O_optflg)
							{
								cout << " IN" << endl;
							}
						}
						else // ZEROS
						{
							curProcess->processStats->zeros += 1;
							total_cycles += 1;
							total_cost += ZEROING_PAGE;
							if (O_optflg)
							{
								cout << " ZERO" << endl;
							}
						}

						// reset reverse mapping
						selectedFrame->reverse_mapping.first = curProcess->processID;
						selectedFrame->reverse_mapping.second = vpageIdx;
						curPTE->frameNum = selectedFrame->frameID;

						curProcess->processStats->maps += 1;
						selectedFrame->age = 0;
						curPTE->PRESENT = 1;
						total_cycles += 1; 
						total_cost += MAP_COST;

						if (!selectedFrame->exited_flg)
						{
							pager->framesPhysical.push_back(selectedFrame);
						}
						else
						{
							selectedFrame->exited_flg = false;
						}
						if (O_optflg)
						{
							cout << " MAP " << selectedFrame->frameID << endl;
						}
					}
					
					curPTE->REFERENCED = 1;
					selectedFrame->tau = instrCnt - 1; // Note: assigning long long to long causes segmentation fault: 11					
					// cout << "CHECK" << endl;
				}
				else // requested virtual page is in a hole
				{
					curProcess->processStats->segv += 1;
					if (O_optflg)
					{
						cout << " SEGV" << endl;
					}
					total_cost += SEGV_COST;
					continue;
				} 
			}// end of load/read situation
			else if (operation == "w") // store/write opertion
			{
				total_cycles += 1;
				total_cost += 1;
				if (curProcess->not_a_hole(vpageIdx))
				{
					PageTableEntry* curPTE = curProcess->pageTable[vpageIdx];
					Frame* selectedFrame = nullptr;
					if (curPTE->PRESENT) // current PTE is already presented 
					{
						// cout << "PRESENT TRIGGERED" << endl;
						selectedFrame = frameTable->frameAll[curPTE->frameNum];
					}
					else// !curPTE->PRESENT // current PTE not presented
					{
						selectedFrame = frameTable->allocate_frame_from_free_list(); // get a free frame for the current PTE
						// if (selectedFrame != nullptr)
						// {
						// 	cout << "ALLOCATE FRAME ID" << selectedFrame->frameID << endl;
						// 	cout << "ALLOCATE FRAME TABLE SIZE" <<  frameTable->frameAll.size() << ":" << frameTable->frameFreePool.size() << endl;
						// }
						// else 
						// {
						//	cout << "ALLOCATE NULLPTR" << endl;
						// }
						// cout << "FRAME TABLE SLOTS" << frameTable->frameAll.size() << ":" << frameTable->frameFreePool.size() << endl; 
						if (selectedFrame == nullptr) // no free frames available
						{	
							selectedFrame = pager->select_victim_frame(frameTable, processVec, instrCnt);
							// if (selectedFrame != nullptr)
							// {
							// 	cout << "SELECTED FRAME ID" << selectedFrame->frameID << endl;
							// 	cout << "SELECTED FRAME TABLE SIZE" <<  frameTable->frameAll.size() << ":" << frameTable->frameFreePool.size() << endl;
							// }
							// else 
							// {
							// 	cout << "SELECTED NULLPTR" << endl;
							// }
							// cout << "SELECT OK" << endl;
							// cout << selectedFrame->reverse_mapping.first << endl;
							// solve Unmap issue 
							Process* oriProcess = processVec[selectedFrame->reverse_mapping.first];
							PageTableEntry* oriPTE = oriProcess->pageTable[selectedFrame->reverse_mapping.second];
							// int original_pid = selectedFrame->reverse_mapping.first;
							// int original_pteIdx = selectedFrame->reverse_mapping.second;
							// Process* oriProcess = processVec[original_pid];
							// PageTableEntry* oriPTE = oriProcess->pageTable[original_pteIdx];
							// cout << "SELECT OK" << endl;

							// cout << "ORIGINAL " << selectedFrame->reverse_mapping.first << ":" << selectedFrame->reverse_mapping.second << endl;
							
							oriProcess->processStats->unmaps += 1;
							oriPTE->PRESENT = 0;
							oriPTE->REFERENCED = 0;
							total_cycles += 1;
							total_cost += UNMAP_COST; 
							if (O_optflg)
							{
								cout << " UNMAP " << selectedFrame->reverse_mapping.first << ":" 
									 << selectedFrame->reverse_mapping.second << endl;
							}

							// modified/dirty, OUT & FOUT 
							if (oriPTE->MODIFIED) // dirty
							{
								oriPTE->MODIFIED = 0; // reset M
								if (oriProcess->is_file_mapped(selectedFrame->reverse_mapping.second)) //FOUT
								{
									oriProcess->processStats->fouts += 1;
									total_cycles += 1;
									total_cost += FILE_IN_OUT_COST;
									if (O_optflg)
									{
										cout << " FOUT" << endl;
									}
								}
								else
								{
									oriProcess->processStats->outs += 1;
									oriPTE->PAGEDOUT = 1;
									total_cycles += 1;
									total_cost += PAGE_IN_OUT_COST;
									if (O_optflg)
									{
										cout << " OUT" << endl;
									}
								}
							}
						} // end of no free frames available handling

						if (curProcess->is_file_mapped(vpageIdx)) // FIN
						{
							curProcess->processStats->fins += 1;
							total_cycles += 1;
							total_cost += FILE_IN_OUT_COST;
							if (O_optflg)
							{
								cout << " FIN" << endl;
							}
						}
						else if (curPTE->PAGEDOUT) // IN
						{
							curProcess->processStats->ins += 1;
							total_cycles += 1;
							total_cost += PAGE_IN_OUT_COST;
							if (O_optflg)
							{
								cout << " IN" << endl;
							}
						}
						else // ZEROS
						{
							curProcess->processStats->zeros += 1;
							total_cycles += 1;
							total_cost += ZEROING_PAGE;
							if (O_optflg)
							{
								cout << " ZERO" << endl;
							}
						}

						// reset reverse mapping
						selectedFrame->reverse_mapping.first = curProcess->processID;
						selectedFrame->reverse_mapping.second = vpageIdx;
						curPTE->frameNum = selectedFrame->frameID;

						curProcess->processStats->maps += 1;
						selectedFrame->age = 0;
						curPTE->PRESENT = 1;
						total_cycles += 1; 
						total_cost += MAP_COST;

						if (!selectedFrame->exited_flg)
						{
							pager->framesPhysical.push_back(selectedFrame);
						}
						else
						{
							selectedFrame->exited_flg = false;
						}
						if (O_optflg)
						{
							cout << " MAP " << selectedFrame->frameID << endl;
						}
					}

					curPTE->REFERENCED = 1;
					selectedFrame->tau = instrCnt - 1; // Note: assigning long long to long causes segmentation fault: 11					
					// cout << "CHECK" << endl;
					if (operation == "w") // extra part different with 'r'
					{
						if (curProcess->is_write_protected(vpageIdx))
						{
							curProcess->processStats->segprot += 1;
							total_cycles += 1;
							total_cost += SEGPROT_COST;
							if (O_optflg)
							{
								cout << " SEGPROT" << endl;
							}
						}
						else
						{
							curPTE->MODIFIED = 1; // dirty
						}
					}
				}
				else // requested virtual page is in a hole
				{
					curProcess->processStats->segv += 1;
					if (O_optflg)
					{
						cout << " SEGV" << endl;
					}
					total_cost += SEGV_COST;
					continue;
				} 
			}// end of store/write situation
			else if (operation == "e") // current process exits
			{
				process_exits += 1;
				total_cycles += 1;
				total_cost += PROCESS_EXIT_COST;
				if (O_optflg)
				{
					cout << "EXIT current process " << vpageIdx << endl;
				}
				for (int i = 0; i < PAGE_TABLE_SIZE; i++)
				{
					PageTableEntry* curPTE = curProcess->pageTable[i];
					if (curPTE->PRESENT)
					{
						frameTable->free_one_frame(curPTE->frameNum); // frame become free again since process exits
						Frame* frameOnHand = frameTable->frameAll[curPTE->frameNum];
						if (O_optflg)
						{
							cout << " UNMAP " << frameOnHand->reverse_mapping.first << ":" 
								 << frameOnHand->reverse_mapping.second << endl;
						}
						frameOnHand->reverse_mapping = pair<int, int>(-1, -1); // remove mapping to initilization
						frameOnHand->exited_flg = true;

						curProcess->processStats->unmaps += 1;
						total_cycles += 1;
						total_cost += UNMAP_COST;

						if (curProcess->is_file_mapped(i) && curPTE->MODIFIED)
						{
							curProcess->processStats->fouts += 1;
							total_cycles += 1;
							total_cost += FILE_IN_OUT_COST;
							if (O_optflg)
							{
								cout << " FOUT" << endl;
							}
						}
						curPTE->MODIFIED = 0; 
						curPTE->PRESENT = 0;
						curPTE->REFERENCED = 0;
					}

					curPTE->PAGEDOUT = 0;
				}
			}// end of current process exits
		}// end of parsing instructions // while get_next_instruction
	} // end of pasing inputFile // while getline

	// print page table
	if (P_optflg)
	{
		vector<Process*>::iterator procIter = processVec.begin();
		for (; procIter != processVec.end(); procIter++)
		{
			cout << "PT[" << (*procIter)->processID << "]:";
			vector<PageTableEntry*>::iterator PTEIter = (*procIter)->pageTable.begin();
			int cnt = 0;
			for (; PTEIter != (*procIter)->pageTable.end(); PTEIter++)
			{
				if (!(*PTEIter)->PRESENT)
				{	
					if((*PTEIter)->PAGEDOUT)
					{
						cout << " #";
					}
					else
					{
						cout << " *";
					}
				}
				else 
				{
					cout << " " << cnt << ":";
					if ((*PTEIter)->REFERENCED)
					{
						cout << "R";
					}
					else
					{
						cout << "-";
					}
					if ((*PTEIter)->MODIFIED)
					{
						cout << "M";
					}
					else 
					{
						cout << "-";
					}
					if ((*PTEIter)->PAGEDOUT)
					{
						cout << "S";
					}
					else 
					{
						cout << "-"; 
					}
				}
				cnt++; 
			}
			cout << endl;
		}
	}
	
	// print frame table
	if (F_optflg)
	{
		cout << "FT:";
		vector<Frame*>::iterator frameIter = frameTable->frameAll.begin();
		for (; frameIter != frameTable->frameAll.end(); frameIter++)
		{
			if ((*frameIter)->reverse_mapping.first != -1)  // valid mapping
			{
				cout << " " << (*frameIter)->reverse_mapping.first << ":" << (*frameIter)->reverse_mapping.second;
			}
			else
			{
				cout << " *";
			}
		}
		cout << endl;
	}
	
	// print all single processes
	if (S_optflg)
	{
		vector<Process*>::iterator procIter = processVec.begin();
		for (; procIter != processVec.end(); procIter++)
		{
			(*procIter)->printProcessStats();
		}
	}

	// print total summary
	printf("TOTALCOST %lu %lu %lu %llu\n", instrCnt, ctx_switches, process_exits, total_cost);	
	return 0;
}













