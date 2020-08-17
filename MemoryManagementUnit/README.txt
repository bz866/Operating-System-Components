
Author: Eric Zeng
NetID: bz866
Email: bz866@nyu.edu

*** MakeFile:

Makefile

*** Source Code:

- main:

mmu.cpp

- others:

FrameTable.hpp
FrameTable.cpp
PageReplacementAlgos.hpp
PageReplacementAlgos.cpp
PageTableEntry.hpp
PageTableEntry.cpp
Process.hpp
Process.cpp
Utils.hpp
Utils.cpp
VirtualMemoryArea.hpp
VirtualMemoryArea.cpp

*** Compile as executable file:

$make 

The executable file will be named as "mmu".

*** Simulate the scheduling of pagging given input file

E.g.
$./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile

-s: Specify paging algorithm, {FIFO (f), Random (r), Clock (c), Enhanced Second Chance / NRU (e), Aging (a), and Working Set (w)}


