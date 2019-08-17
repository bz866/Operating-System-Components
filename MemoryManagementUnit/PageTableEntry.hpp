//
// HEader File
// // PageTableEntry.hpp
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
(easier). It cannot be a structure of multiple int values that collectively are larger than 
32-bits. (see http://www.cs.cf.ac.uk/Dave/C/node13.html (BitFields) or 
http://www.catonmat.net/blog/bit-hacks-header-file/ as an example, I highly encourage you to use 
this technique, let the compiler do the hard work for you).

*/
//

#ifndef PageTableEntry_HPP
#define PageTableEntry_HPP

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


// Structure Tag VS. Structure Variables 
// Ref: http://ee.hawaii.edu/~tep/EE160/Book/chap12/subsection2.1.1.2.html
// The main advantage of splitting the template definition from variable declaration
// is that the template need be defined only once and may then be used to declare variables in many places. We will see the utility of this below when we pass structures to function.
typedef struct PageTableEntry
{
	// unsigned: unsigned int will hold values from 0 to 65535. You can use this specifier when you know that your variable will never need to be negative. 
	// Ref: https://en.wikibooks.org/wiki/C%2B%2B_Programming/Programming_Languages/C%2B%2B/Code/Keywords/unsigned
	unsigned int PRESENT:1;
	unsigned int WRITE_PROTECT:1;
	unsigned int MODIFIED:1;
	unsigned int REFERENCED:1;
	unsigned int PAGEDOUT:1;
	unsigned int pageTableBits:20;
	unsigned int frameNum:7;

	//Note (again): you MUST NOT set any bits in the PTE before instruction simulation start, 
	// i.e. the pte (i.e. all bits) should be initialized to “0” before the instruction simulation starts.
	PageTableEntry(); //overloaded constructor with all zeros intialize values
	// ~PageTableEntry();

}pte_t;

#endif



