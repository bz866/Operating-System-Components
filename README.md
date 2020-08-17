# Operating-System-Components

## Table of Contents

* [About the Project](#about-the-project)
  
* [Two-pass Linker](#getting-started)
* [Process Scheduler](#usage)
  * Concept
* [Virtual Memory Management](#roadmap)
* [I/O Scheduler ](#contributing)

## Process Scheduler 

### Concept
  The implementation and effects of different scheduling policies discussed in class on a set of processes/threads executing on a system. The system is to be implemented using Discrete Event Simulation (DES) (http://en.wikipedia.org/wiki/Discrete_event_simulation). In discrete-event simulation, the operation of a system is represented as a chronological sequence of events. Each event occurs at an instant in time and marks a change of state in the system. This implies that the system progresses in time through defining and executing the events (state transitions) and by progressing time discretely between the events as opposed to incrementing time continuously (e.g. don’t do “sim_time++”). Events are removed from the event queue in chronological order, processed and new events might be created during the simulation. 
