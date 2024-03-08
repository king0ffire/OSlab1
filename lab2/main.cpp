﻿#include <iostream>
#include <fstream>
//#include <unistd.h>
#include "getopt.h"
#include <string>
#include <queue>
#include <deque>
#define RANDMAX 10000
using namespace std;

typedef enum { STATE_RUNNING, STATE_BLOCKED } process_state_t;

queue<Process*> readyqueue;  //queue or deque
queue<Process*> runqueue;

int randvals[RANDMAX];
int randvalofs = 0;

class Event
{
public:
	Event();
	~Event();
	int timestamp;
	Process* process;
	int oldstate;
	int newstate;
private:

};

Event::Event()
{
}

Event::~Event()
{
}

class Process
{
public:
	Process();
	~Process();
	int AT;
	int TC;
	int CB;
	int IO;
	int PRIO;
	int state_ts
private:

};

Process::Process()
{
}

Process::~Process()
{
}

class Scheduler
{
public:
	Scheduler();
	~Scheduler();
	virtual void add_process(Process* p) = 0;
	virtual Process* get_next_process() = 0;
	virtual bool test_preempt(Process* p, int curtime)=0;
	//Container<*Process> RUN_QUEUE;
private:

};

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
}

class FIFO :public Scheduler
{
public:
	FIFO();
	~FIFO();

private:

};

FIFO::FIFO()
{
}

FIFO::~FIFO()
{
}
class DESlayer
{
public:
	DESlayer();
	~DESlayer();

private:

};

class PriorityScheduler
{
public:
	PriorityScheduler();
	~PriorityScheduler();
	queue<Process*>* activeQ;
	queue<Process*>* expiredQ;
private:

};

PriorityScheduler::PriorityScheduler()
{
}

PriorityScheduler::~PriorityScheduler()
{
}


DESlayer::DESlayer()
{
}

DESlayer::~DESlayer()
{
}

int myrandom(int burst) { //怎么++和怎么回滚
	return 1 + (randvals[randvalofs] % burst);
}

void Simulation() {
	Event* evt;
	while ((evt = get_event())) {
		Process* proc = evt->evtProcess; // this is the process the event works on
		CURRENT_TIME = evt->evtTimeStamp;
		int transition = evt->transition;
		int timeInPrevState = CURRENT_TIME – proc->state_ts; // for accounting
		delete evt; evt = nullptr; // remove cur event obj and don’t touch anymore
		switch (transition) { // encodes where we come from and where we go
		case TRANS_TO_READY:
			// must come from BLOCKED or CREATED
			// add to run queue, no event created
			CALL_SCHEDULER = true;
			break;
		case TRANS_TO_PREEMPT: // similar to TRANS_TO_READY // must come from RUNNING (preemption)
			// add to runqueue (no event is generated)
			CALL_SCHEDULER = true;
			break;
		case TRANS_TO_RUN:
			// create event for either preemption or blocking
			break;
		case TRANS_TO_BLOCK:
			//create an event for when process becomes READY again
			CALL_SCHEDULER = true;
			break;
		}
		if (CALL_SCHEDULER) {
			if (get_next_event_time() == CURRENT_TIME)
				continue; //process next event from Event queue
			CALL_SCHEDULER = false; // reset global flag
			if (CURRENT_RUNNING_PROCESS == nullptr) {
				CURRENT_RUNNING_PROCESS = THE_SCHEDULER->get_next_process();
				if (CURRENT_RUNNING_PROCESS == nullptr)
					continue;
				// create event to make this process runnable for same time.
			}
		}
	}
}

int main(int argc, char* argv[]) {


	int vflag = 0;
	int tflag = 0;
	int eflag = 0;
	int pflag = 0;
	int sflag = 0;
	int quantum = 0;
	int maxprios = 4;
	string ss;
	int c;

	while ((c = getopt(argc, argv, "vteps:")) != -1)
	{
		switch (c)
		{
		case 'v':
			vflag = 1;
			break;
		case't':
			tflag = 1;
			break;
		case'e':
			eflag = 1;
			break;
		case'p':
			pflag = 1;
			break;
		case's':
			sflag = 1;
			ss = optarg;
			break;
		default:
			break;
		}
	}
	ifstream inputfile;
	ifstream randfile;
	if (argc- optind <= 1)
	{
		cout << "A input file is needed";
		return 0;
	}
	switch (ss[0])
	{
	case'F':
		//do FCFS
		break;
	case'L':
		//do LCFS
		break;
	case'S':
		//do SRTF
		break;	
	case'R':
		//do RR
		break;
	case'P':
		//do PRIO
		sscanf(ss.substr(1).c_str(), "%d:%d", &quantum, &maxprios);
		break;
	case'E':
		//do PREE PRIO
		sscanf(ss.substr(1).c_str(), "%d:%d", &quantum, &maxprios);
		break;
	default:
		break;
	}


	string f1 = argv[1+optind];
	string f2 = argv[2+optind];

	inputfile.open(f1);
	if (!inputfile.is_open())
	{
		cout << "no file";
		return 0;
	}
	randfile.open(f2);
	if (!randfile.is_open())
	{
		cout << "no file";
		return 0;
	}


	int time_cpubusy, time_iobusy, num_processes, finishtime; 
	double cpu_util = 100.0 * (time_cpubusy / (double)finishtime);
	double io_util = 100.0 * (time_iobusy / (double)finishtime);
	double throughput = 100.0 * (num_processes / (double)finishtime);
	return 0;
}