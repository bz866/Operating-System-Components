Operating System Lab 2 Submission

Author: Binqian Zeng
NetID: bz866
Email: bz866@nyu.edu

*** MakeFile:

Makefile

*** Source Code:

- main:

Schedule.cpp

- others:

Utils.hpp
Utils.cpp
Scheduler.hpp
Scheduler.cpp
Process.hpp
Process.cpp
EVENT.hpp
EVENT.cpp
DiscreteEventSimulator.hpp
DiscreteEventSimulator.cpp

*** Compile as executable file:

$make 

The executable file will be named as "sched".

*** Simulate processes given input file

E.g.
$./sched -v –s[FLS] inputFilePath RandFilePath
OR
$./sched -v –s[R<num> | P<num>[:<maxprio>] | E<num>[:<maxprios>] ] inputFilePath RandFilePath

-v: if -v, print verbose information to help debugging

-s: Specify Scheduling algorithm, quantum and optional max priority


