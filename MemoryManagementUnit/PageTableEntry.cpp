//
// // PageTableEntry.cpp
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

A page table naturally must contain exactly 64 page table entries (PTE). Please use constants 
rather than hardcoding “64”. A PTE is comprised of the PRESENT/VALID, WRITE_PROTECT, MODIFIED, 
REFERENCED and PAGEDOUT bits and an index to the physical frame (in case the pte is present). 
This information can and should be implemented as a single 32-bit value or as a bit structures 
(easier). It cannot be a structure of multiple integer values that collectively are larger than 
32-bits. (see http://www.cs.cf.ac.uk/Dave/C/node13.html (BitFields) or 
http://www.catonmat.net/blog/bit-hacks-header-file/ as an example, I highly encourage you to use 
this technique, let the compiler do the hard work for you).

*/
//

#include "PageTableEntry.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

PageTableEntry::PageTableEntry() // Overload the default constructor with initial values as all zeros
{
	// Note (again): you MUST NOT set any bits in the PTE before instruction simulation start, 
	// i.e. the pte (i.e. all bits) should be initialized to “0” before the instruction simulation starts. 
	this->PRESENT = 0;
	this->WRITE_PROTECT = 0;
	this->MODIFIED = 0;
	this->REFERENCED = 0;
	this->PAGEDOUT = 0;
	this->pageTableBits = 0;
	this->frameNum = 0;
}

// PageTableEntry::~PageTableEntry(){}








