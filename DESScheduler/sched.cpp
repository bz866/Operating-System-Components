// sched.hpp
//
//
// main File
//
//
//
//
// Lab 2 exercise Operating System 2019 Summer
//
// Author: Binqian Zeng
//
// start: 07/06/2019
//
//
/*
The main file of the scheduler/ dispatcher 

Implementation of different scheduling policies {FCFS, LCFS, SRTF, RR(Round Robin), PRIO(Priority Scheduling), PREemptive PRIO(PREPRIO)}

Ref: 
https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/6_CPU_Scheduling.html
https://en.wikipedia.org/wiki/Scheduling_(computing)

To Run:

$./sched [-v] [-s<schedalgo>] {INPUTFILE_PATH} {RANDFILE_PATH}

Note:

The scheduler specification is “–s [ FLS | R<num> | P<num>[:<MAXPRIOS>] | E<num>[:<MAXPRIOS>] ]”, where F=FCFS, L=LCFS, S=SRTF and R10 and P10 are RR and PRIO with QUANTUM 10. 
(e.g. “./sched –sR10”) and E10 is the preemptive prio scheduler. Supporting this parameter is required and the QUANTUM is a positive number. In addition the number of priority 
levels is specified in PRIO and PREPRIO with an optional “:num” addition. E.g. “-sE10:5” implies QUANTUM=10 and numprios=5. If the addition is omitted then MAXPRIOS=4 by default 
(lookup sscanf(optarg, “%d:%d”,&QUANTUM,&MAXPRIOS))

The –v option stands for verbose and should print out some tracing information that allows one to follow the state transition.


Output:
At the end of the program you should print the following information and the example outputs provide the proper expected formatting (including precision);
 this is necessary to automate the results checking; all required output should go to the console ( stdout / cout ).
a) Scheduler information (which scheduler algorithm and in case of RR/PRIO/PREPRIO also the QUANTUM)
b) Per process information (see below):
for each process (assume processes start with pid=0), the correct desired format is shown below:
pid: AT TC CB IO PRIO | FT TT IT CW FT: Finishing time
TT: Turnaround time ( finishing time - AT )
IT: I/O Time ( time in blocked state)
PRIO: static priority assigned to the process ( note this only has meaning in PRIO/PREPRIO case ) c) CW: CPU Waiting time ( time in Ready state )
d) Summary Information - Finally print a summary for the simulation:
Finishing time of the last event (i.e. the last process finished execution)
CPU utilization (i.e. percentage (0.0 – 100.0) of time at least one process is running
IO utilization (i.e. percentage (0.0 – 100.0) of time at least one process is performing IO Average turnaround time among processes
Average cpu waiting time among processes
Throughput of number processes per 100 time units
CPU / IO utilizations and throughput are computed from time=0 till the finishing time.

Example:
FCFS
0000: 0 100 10 100| 223 223 123 0 
0001: 500 100 20 100| 638 138 38 0 
SUM: 638 31.35 25.24 180.50 0.00 0.313

*/
//
//

#include "Utils.hpp"
#include "Scheduler.hpp"
#include "Process.hpp"
#include "EVENT.hpp"
#include "DiscreteEventSimulator.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <list>
#include <istream>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <cstring>
#include <deque>
#include <list>

/*
To use "getopt" without bugs, we need include standard libraries IN ORDER. 
*/



using namespace std;
// namespace poargs = boost::program_options;


//Global Variables
int isVerbose; // if print support trace information
int QUANTUM; // For RR, PRIO, PREPRIO
int MAXPRIOS; // optional for PRIO PREPRIO
int DEFAULT_MAXPRIOS = 4; //if the addition is omitted then MAXPRIOS=4 by default (page 4)
int DEFAULT_QUANTUM = 100000; //deal with schedulers is to treat non- preemptive scheduler as preemptive with very large quantum 10K
int randnums_num; // total number of random numbers in randFile
int ofs; // the index of the ith random number 
std::vector<int> randvals; // INitialize an array to store all random numbers from the file

//Queue 
//Use deque to access front and back easily to meet requirement of all Scheduling Algorithm
// std::deque<EVENT*> eventQueue; // queue of events, push all process features as first event, then insert remained process events features by evtTimeStamp
std::list<EVENT*> eventQueue;
//extern std::deque<Process*> readyQueue; // 
//extern std::deque<Process*> finishedQueue;
Process* CURRENT_RUNNING_PROCESS = nullptr;
int CURRENT_TIME;


//============================ Main ===========================================

int main(int argc, char **argv)
{
	//bool isVerbose;
	SCHEDULE_ALGO schedalgo; //{FCFS, LCFS, SRTF, RR, PRIO, PREPRIO};
	string schedspec; //the scheduler specification [ FLS | R<num> | P<num>[:<MAXPRIOS>] | E<num>[:<MAXPRIOS>] ]
	//int QUANTUM; //For RR, PRIO, PREPRIO
	string::size_type sz; //alias of size_t
	//int MAXPRIOS; //optional, for PRIO, PREPRIO
	string inputFilePath; // input txt file of the processes
	string randFilePath; // reference file of random numbers
	int index; // for optind to access a specific argument by index
	int c; 

	// std::ifstream randFile;
	// std::ifstream inputFile;

	// ================ Arguments Parsing ==============================
	// CPP parsing arguments
	// Ref: https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

	while ((c = getopt(argc, argv, "vs:")) != -1)
	{
		switch(c)
		{
			case 'v': //ifVerbose == true, print trace information
			{	
				isVerbose = true;
				break;
			}
			case 's': // given schedalgo to define scheduling algorithm
			{
				switch(optarg[0])
				{
					case 'F':
					{
						schedalgo = FCFS;
						QUANTUM = DEFAULT_QUANTUM; // 10K never fire
						MAXPRIOS = DEFAULT_MAXPRIOS; // = 4 by default
						cout << "FCFS" << endl;
						break;
					}

					case 'L':
					{
						schedalgo = LCFS;
						QUANTUM = DEFAULT_QUANTUM; // 10K never fire
						MAXPRIOS = DEFAULT_MAXPRIOS; // = 4 by default
						cout << "LCFS" << endl;
						break;
					}

					case 'S':
					{
						schedalgo = SRTF;
						QUANTUM = DEFAULT_QUANTUM;
						MAXPRIOS = DEFAULT_MAXPRIOS;
						cout << "SRTF" << endl;
						break;
					}

					case 'R':
					{
						schedalgo = RR;
						// schedspec.assign(optarg);
						schedspec = optarg;
						QUANTUM = stoi(schedspec.substr(1, schedspec.length()-1), &sz, 10); //QUANTUM 
						if (contains_number(schedspec.substr(1, schedspec.length()-1).substr(sz)))
						{
							MAXPRIOS = stoi(schedspec.substr(1, schedspec.length()-1).substr(sz+1)); // MAXPRIOS
							cout << "RR should not be specified MAXPRIOS" << endl;
						}
						else
						{
							MAXPRIOS = DEFAULT_MAXPRIOS;
							// cout << "DEFAULT " << "RR" << " " << QUANTUM << " " << MAXPRIOS << endl;
						}
						cout << "RR" << " " << QUANTUM << endl;
						// cout << schedspec << endl;
						// std::istringstream schedspecss(extract_ints(schedspec));
						// if (!schedspecss.eof()) schedspecss >> QUANTUM;
						// if (!schedspecss.eof()) 
						// {
						// 	schedspecss >> MAXPRIOS;
						// }
						// else
						// {
						// 	MAXPRIOS = DEFAULT_MAXPRIOS;
						// }
						// cout << "RR" << " " << QUANTUM << " " << MAXPRIOS << endl;
						break;
					}

					case 'P':
					{
						schedalgo = PRIO;
						// schedspec.assign(optarg);
						schedspec = optarg;
						QUANTUM = stoi(schedspec.substr(1, schedspec.length()-1), &sz, 10); //QUANTUM 
						if (contains_number(schedspec.substr(1, schedspec.length()-1).substr(sz)))
						{
							MAXPRIOS = stoi(schedspec.substr(1, schedspec.length()-1).substr(sz+1)); // MAXPRIOS
							// cout << "PRIO" << " " << QUANTUM << " " << MAXPRIOS << endl;
						}
						else
						{
							MAXPRIOS = DEFAULT_MAXPRIOS;
							// cout << "DEFAULT " << "PRIO" << " " << QUANTUM << " " << MAXPRIOS << endl;
						}
						cout << "PRIO" << " " << QUANTUM << endl;
						break;
					}

					case 'E':
					{
						schedalgo = PREPRIO;
						// schedspec.assign(optarg);
						schedspec = optarg;
						QUANTUM = stoi(schedspec.substr(1, schedspec.length()-1), &sz, 10);
						if (contains_number(schedspec.substr(1, schedspec.length()-1).substr(sz)))
						{
							MAXPRIOS = stoi(schedspec.substr(1, schedspec.length()-1).substr(sz+1)); // MAXPRIOS
							// cout << "PRIO" << " " << QUANTUM << " " << MAXPRIOS << endl;
						}
						else
						{
							MAXPRIOS = DEFAULT_MAXPRIOS;
						}
						cout << "PREPRIO" << " " << QUANTUM << endl;
						break;
					}

					default:
					{
						std::cerr << "Unknown Scheduler spec: -v {FLSRPE}" << endl;
						abort();
						return 1;
					}
				}
				break;
			}
			case '?': //Invalid option
			{
				std::cerr << "invalid option -- '%s'" << optarg << endl;
				abort();
				return 1;
			}
			default:
			{
				abort();
				return 1;
			}
		}
	}




	//========= Files Name ==============
	index = optind;
	inputFilePath = argv[index]; //inputFile name
	index++;
	randFilePath = argv[index]; //randFile name
	// inputFilePath = argv[argc - 2];
	// randFilePath = argc[argc - 1];

	//================= Load randFile for random numbers ===================
	//Total number of random numbers in the randFile
	// randFile.open(randFilePath);
	// if (!randFile.good())
	// {
	// 	std::cerr << "Random Number file cannot be opend. " << endl;
	// 	abort();
	// }

	//================= Load inputFile processes as a fstream ====================================
	// inputFile.open(inputFilePath); //inputFile as file stream
	// if (!inputFile.good())
	// {
	// 	std::cerr << "Input file cannot be opend. " << endl;
	// 	abort();
	// }

	//================= Use DiscreteEventSimulator to simulate all process ========================
	//Pass fstream object to function; Ref: http://www.cplusplus.com/forum/beginner/84043/
	DiscreteEventSimulator DES(inputFilePath, randFilePath, schedalgo); //initialization

	DES.Simulation(); //Simulation all processes

	DES.collectSummaryInformation(); 

	DES.printOutputInformation();

	return 0;
}


// //================== Argument parsing with boost =========================

// 	poargs::options_description desc("Allowed options");
// 	desc.add_options()
// 		("h", "produce help message")
//         ("v", "print verbose information to debug")
//         ("s", poargs::value<string>(), "set scheduling algorithm and/or QUANTUM and/or MAXPRIOS");
	
// 	po::variables_map vm;
//     po::store(po::parse_command_line(argc, argv, desc), vm);
//     po::notify(vm);

//     if (vm.count("h")) {
//         cout << desc << "\n";
//         return 0;
//     }

//     try (vm.count("s"))
//     {
//     	std::istringstream sPart(vm["s"].as<string>());
//     }
//     catch(exception& e)
//     {
//     	cerr << "error: " << e.what("Scheduling Algorithm must be specified.") << "\n";
//         return 1;
//     }

//     if (vm.count("v"))
//     {
//     	isVerbose = true;
//     }

//     while (sPart != eof())
//     {
//     	string schedspec
//     }




















