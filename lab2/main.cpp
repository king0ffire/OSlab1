#include <iostream>
#include <fstream>
//#include <unistd.h>
#include <string>
#include <queue>
#include <deque>
#include <list>
#include <vector>
#pragma warning(disable : 4996)
#define RANDMAX 10000
using namespace std;


int vflag = 0;
int tflag = 0;
int eflag = 1;
int pflag = 0;
int sflag = 0;
typedef enum { STATE_RUNNING, STATE_BLOCKED,STATE_CREATED,STATE_READY, STATE_PREEMPTED,STATE_TERMINATED} process_state_t;
struct SimRes
{
	int time_cpubusy = 0;
	int time_iobusy = 0;
	int finishtime = 0;
};

class Process
{
public:
	Process();
	~Process();
	int AT;
	int TC;
	int CB;
	int IO;

	int FT;
	//int TT=FT-AT
	int IT;
	int CW;

	int static_priority;
	int dynamic_priority;
	int state_ts;  //time state start
	int remainingtime;
	int state;
	int pid;
private:

};

Process::Process()
{
	AT = 0;
	TC = 0;
	CB = 0;
	IO = 0;
	FT = 0;
	IT = 0;
	CW = 0;
	static_priority = 0;
	dynamic_priority = 0;
	state_ts = 0;
	remainingtime = 0;
	state = STATE_CREATED;
}

Process::~Process()
{
}
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
	timestamp = 0;
	process = nullptr;
	oldstate = 0;
	newstate = 0;
}

Event::~Event()
{
}

void put_event(list<Event*> &eventQ, Event* event) {
	if (eventQ.empty() || eventQ.back()->timestamp <= event->timestamp) {
		eventQ.push_back(event);
		return;
	}
	for (std::list<Event*>::iterator it = eventQ.begin(); it != eventQ.end(); it++) {
		if (event->timestamp < (*it)->timestamp)
		{
			eventQ.insert(it, event);
			return;
		}
	}
}

Event* get_event(list<Event*>& eventQ) {
	if (eventQ.empty()) return nullptr;
	return eventQ.front();
}

void rm_event(list<Event*>& eventQ) {
	if (eventQ.empty()) return;
	eventQ.pop_front();
	return;
}
class Scheduler
{
public:
	Scheduler();
	~Scheduler();
	virtual void add_process(Process* p) = 0;
	virtual Process* get_next_process() = 0;
	virtual bool test_preempt(Process* p, int curtime)=0;
	virtual string getname() = 0;
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
	void add_process(Process* p);
	Process* get_next_process();
	bool test_preempt(Process* p, int curtime);
	string getname();
	queue<Process*> runqueue;
private:

};
FIFO::FIFO()
{
}
FIFO::~FIFO()
{
}
void FIFO::add_process(Process* p)
{
	runqueue.push(p);
}
Process* FIFO::get_next_process()
{
	if (runqueue.empty()) return nullptr;
	Process* proc = runqueue.front();
	runqueue.pop();
	return proc;
}
bool FIFO::test_preempt(Process* p, int curtime)
{
	return false;
}

string FIFO::getname()
{
	return "FCFS";
}

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

int myrandom(vector<int> &randvals,int &currentind, int randnumbers, int burst) { //return [1...burst] randnumber
	if (currentind >= randnumbers) {
		currentind = 0;
	}
	int res = 1 + (randvals[currentind] % burst);
	currentind++;
	return res;
}

SimRes* Simulation(list<Event*> &eventQ,vector<int> &randvals,int &currentind,int randnumbers, Scheduler* scheduler,int quantum) {
	Event* evt;
	int CURRENT_TIME=0;
	bool CALL_SCHEDULER = false;
	Process* currentproc = nullptr;
	int time_cpubusy = 0;
	int time_iobusy = 0;
	
	while ((evt = get_event(eventQ))!=nullptr) {
		rm_event(eventQ);
		Process* proc = evt->process; // this is the process the event works on
		CURRENT_TIME = evt->timestamp;
		int transition = evt->newstate;
		int fromstate = evt->oldstate;
		int timeInPrevState = CURRENT_TIME - proc->state_ts; // for accounting
		delete evt; evt = nullptr; // remove cur event obj and don’t touch anymore
		int cpuburst = 0;
		int ioburst = 0;
		Event* newevent=nullptr;
		switch (transition) { // encodes where we come from and where we go
		case STATE_READY:
			// transition 1,4
			// must come from BLOCKED or CREATED
			// add to run queue, no event created
			if (fromstate == STATE_BLOCKED) { // ALL return from IO reset dynamic 
				proc->dynamic_priority = proc->static_priority-1;
				//proc->IO += timeInPrevState;
				proc->state = STATE_READY;
			}
			proc->state = STATE_READY;
			proc->state_ts = CURRENT_TIME;
			scheduler->add_process(proc);
			CALL_SCHEDULER = true;

			if (eflag == 1&& fromstate == STATE_BLOCKED)
			{
				printf("%d %d %d: BLOCK -> READY\n", CURRENT_TIME, proc->pid, timeInPrevState);
			}
			else if (eflag == 1 && fromstate == STATE_CREATED)
			{
				printf("%d %d %d: CREATED -> READY\n", CURRENT_TIME, proc->pid, timeInPrevState);
			}
			break;
		case STATE_PREEMPTED: 
			// transition 5
			// similar to TRANS_TO_READY // must come from RUNNING (preemption)
			// add to runqueue (no event is generated)
			scheduler->add_process(proc);
			
			CALL_SCHEDULER = true;
			break;
		case STATE_RUNNING:
			// transition 2
			// create event for either preemption or blocking
			proc->CW += timeInPrevState;

			cpuburst = myrandom(randvals,currentind,randnumbers,proc->CB);
			if (cpuburst > proc->remainingtime) cpuburst = proc->remainingtime; 
			if (cpuburst > quantum) cpuburst = quantum;

			newevent = new Event();
			if (!(scheduler->test_preempt(proc,CURRENT_TIME))) {
				newevent->newstate = STATE_BLOCKED;
				newevent->timestamp = CURRENT_TIME + cpuburst;
			}
			else
			{
				newevent->newstate = STATE_PREEMPTED;
				newevent->timestamp = CURRENT_TIME + cpuburst;
			}
			newevent->oldstate = STATE_RUNNING;
			newevent->process = proc;
			proc->state = STATE_RUNNING;
			proc->state_ts = CURRENT_TIME;
			put_event(eventQ, newevent);

			if (eflag == 1)
			{
				printf("%d %d %d: READY -> RUNNG cb=%d rem=%d prio=%d\n", CURRENT_TIME, proc->pid, timeInPrevState, cpuburst,proc->remainingtime,proc->dynamic_priority);
			}
			break;
		case STATE_BLOCKED:
			// transition 3
			// create an event for when process becomes READY again
			proc->remainingtime = proc->remainingtime - timeInPrevState;
			if (proc->remainingtime == 0)
			{
				proc->FT=CURRENT_TIME;
				proc->state = STATE_TERMINATED;

				if (eflag == 1)
				{
				printf("%d %d %d: Done\n", CURRENT_TIME, proc->pid, timeInPrevState);
				}
				continue;
			}
			time_cpubusy += timeInPrevState;
			currentproc = nullptr;

			ioburst = myrandom(randvals,currentind,randnumbers,proc->IO);

			newevent=new Event();
			newevent->timestamp = CURRENT_TIME + ioburst;
			newevent->newstate = STATE_READY;
			newevent->oldstate = STATE_BLOCKED;
			newevent->process = proc;
			proc->state = STATE_BLOCKED;
			proc->state_ts = CURRENT_TIME;
			put_event(eventQ, newevent);
			CALL_SCHEDULER = true;

			if (eflag == 1)
			{
				printf("%d %d %d: RUNNG -> BLOCK  ib=%d rem=%d\n", CURRENT_TIME, proc->pid, timeInPrevState, ioburst, proc->remainingtime);
			}
			break;
		}
		if (CALL_SCHEDULER) {
			Event* nextevent = get_event(eventQ);
			if (nextevent!=nullptr && nextevent->timestamp == CURRENT_TIME) // process all event at one arrival time
				continue; //process next event from Event queue
			CALL_SCHEDULER = false; // reset global flag
			if (currentproc == nullptr) {
				currentproc = scheduler->get_next_process();
				if (currentproc == nullptr)
					continue;
				// create event to make this process runnable for same time.
				newevent = new Event();
				newevent->newstate = STATE_RUNNING;
				newevent->oldstate = STATE_READY;
				newevent->process = currentproc;
				newevent->timestamp = CURRENT_TIME;// since the 2th-last continue, the current_time is smaller than any other event.
				put_event(eventQ, newevent);
			}
		}
	}
	SimRes* res = new SimRes();
	res->time_cpubusy = time_cpubusy;
	res->time_iobusy = time_iobusy;
	res->finishtime = CURRENT_TIME;
	return res;
}

int main(int argc, char* argv[]) {

	int quantum = 10000;
	int maxprio = 4;
	string ss="F";  //scheduled select
	//int c;
	Scheduler* scheduler = nullptr;
	/*
	while ((c = getopt(argc, argv, "vtep")) != -1)
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
		default:
			if
			sflag = 1;
			ss = optarg;
			break;
			break;
		}
	}
	*/
	ifstream inputfile;
	ifstream randfile;
	/*
	if (argc- optind <= 1)
	{
		cout << "A input file is needed";
		return 0;
	}*/
	switch (ss[0])
	{
	case'F':
		//do FCFS
		scheduler = new FIFO();
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
		sscanf(ss.substr(2).c_str(), "%d:%d", &quantum, &maxprio);
		break;
	case'E':
		//do PREE PRIO
		sscanf(ss.substr(2).c_str(), "%d:%d", &quantum, &maxprio);
		break;
	default:
		break;
	}

	string f1 = "F:/美国学习资料/OS/lab2/lab2_assign/input0";
	string f2 = "F:/美国学习资料/OS/lab2/lab2_assign/rfile";
	//string f1 = argv[1+0]; //optind
	//string f2 = argv[2+0];

	inputfile.open(f1);
	if (!inputfile.is_open())
	{
		cout << "no file 1";
		return 0;
	}
	randfile.open(f2);
	if (!randfile.is_open())
	{
		cout << "no file 2";
		return 0;
	}

	vector<int> randvals;
	int randvalofs = 0;
	int randmax;
	string oneline;
	getline(randfile, oneline);
	sscanf(oneline.c_str(), "%d", &randmax);
	int temp0;
	getline(randfile, oneline);
	while (!randfile.eof()) //read all into the vector
	{
		sscanf(oneline.c_str(), "%d", &temp0);
		randvals.push_back(temp0);
		getline(randfile, oneline);
	}

	list<Event*> eventQ;
	vector<Process*> allprocess;
	string onelineprocess;
	int temp1;
	int temp2;
	int temp3;
	int temp4;
	int num_processes=0;
	while (true)
	{
		getline(inputfile, onelineprocess);
		if (inputfile.eof())
		{
			break;
		}
		sscanf(onelineprocess.c_str(), "%d %d %d %d", &temp1, &temp2, &temp3, &temp4);
		Process* temp = new Process();
		temp->AT = temp1;
		temp->TC = temp2;
		temp->CB = temp3;
		temp->IO = temp4;
		temp->static_priority = myrandom(randvals, randvalofs, randmax, maxprio);
		temp->dynamic_priority = temp->static_priority-1;
		temp->remainingtime = temp->TC;
		temp->pid = num_processes;
		allprocess.push_back(temp);
		num_processes++;
		Event* newevent = new Event();
		newevent->oldstate = STATE_CREATED;
		newevent->newstate = STATE_READY;
		newevent->timestamp = temp->AT;
		newevent->process = temp;
		put_event(eventQ, newevent);
	}
	SimRes* res=Simulation(eventQ, randvals,randvalofs, randmax, scheduler,quantum);

	int totalturnaround = 0;
	int totalcpuwaiting = 0;
	double cpu_util = 100.0 * (res->time_cpubusy / (double)res->finishtime);
	double io_util = 100.0 * (res->time_iobusy / (double)res->finishtime);
	double throughput = 100.0 * (num_processes / (double)res->finishtime);
	cout << scheduler->getname() << endl;
	for (int i=0; i<allprocess.size(); i++)
	{
		totalturnaround += allprocess[i]->FT - allprocess[i]->AT;
		totalcpuwaiting += allprocess[i]->CW;
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",i, allprocess[i]->AT, allprocess[i]->TC, 
			allprocess[i]->CB, allprocess[i]->IO, allprocess[i]->static_priority, 
			allprocess[i]->FT, allprocess[i]->FT - allprocess[i]->AT, allprocess[i]->IT, allprocess[i]->CW );
	}
	double averturnaround = totalturnaround / (double)num_processes;
	double avercpuwaiting = totalcpuwaiting / (double) num_processes;
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", res->finishtime, cpu_util, io_util,averturnaround, avercpuwaiting, throughput);
	return 0;
}