//
// // VirtualMemoryArea.cpp
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


#include "VirtualMemoryArea.hpp"

using namespace std;

VirtualMemoryArea::VirtualMemoryArea(int start_vpg, int end_vpg, int write_prtctd, int file_mppd)// Overload constructor with given initial values 
{
	this->start_vpage = start_vpg;
	this->end_vpage = end_vpg;
	this->write_protected = write_prtctd;
	this->file_mapped = file_mppd;
}










