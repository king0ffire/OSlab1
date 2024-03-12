#include <iostream>
#include <fstream>
//#include <unistd.h>
#include <string>
#include <queue>
#include <deque>
#include <list>
#include <vector>
#include <stack>
#pragma warning(disable : 4996)
#define DEFAULTQUANTUM 10000
#define DEFAULTMAXPRIO 4
using namespace std;



typedef enum { STATE_RUNNING, STATE_BLOCKED,STATE_CREATED,STATE_READY, STATE_PREEMPTED,STATE_TERMINATED} process_state_t;
struct SimRes
{
	int time_cpubusy = 0;
	int time_iobusy = 0;
	int finishtime = 0;
};
struct Debugparas {
	bool vflag = false;
	bool tflag = false;
	bool eflag = false;
	bool pflag = false;
	bool sflag = false;
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
	int remainingcpuburst;
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
	pid = 0;
	remainingcpuburst = 0;
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
list<Event*>::iterator findeventiteratorbyprocess(list<Event*>& eventQ,Process* proc)
{
	for (list<Event*>::iterator it = eventQ.begin(); it != eventQ.end(); it++)
	{
		if ((*it)->process == proc)
		{
			return it;
		}
	}
	return eventQ.end();
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

void rm_event_preempt(list<Event*>& eventQ, Process* runningproc)  //assuming the runningproc is always exist only once in the Q
{
			eventQ.erase(findeventiteratorbyprocess(eventQ,runningproc));
			return;
}

class Scheduler
{
public:
	int quantum;
	int maxprio;
	Scheduler();
	~Scheduler();
	virtual void add_process(Process* p) = 0;
	virtual Process* get_next_process() = 0;
	virtual int test_preempt(list<Event*>&eventQ,Process* p, int curtime)=0;
	virtual string getname() = 0;
	//Container<*Process> RUN_QUEUE;
private:

};
Scheduler::Scheduler()
{
	quantum = 0;
	maxprio = 0;
}
Scheduler::~Scheduler()
{
}

class FIFO :public Scheduler
{
public:
	FIFO();
	FIFO(int,int);
	~FIFO();
	void add_process(Process* p);
	Process* get_next_process();
	int test_preempt(list<Event*>& eventQ, Process* p, int curtime);
	string getname();
	queue<Process*> runqueue;
private:

};
FIFO::FIFO()
{
}
FIFO::FIFO(int a, int b)
{
	quantum = a;
	maxprio = b;
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
int FIFO::test_preempt(list<Event*>& eventQ, Process* p, int curtime)
{
	return -1;
}
string FIFO::getname()
{
	return "FCFS";
}

class LCFS:public Scheduler
{
public:
	LCFS();
	LCFS(int, int);
	~LCFS();
	void add_process(Process* p);
	Process* get_next_process();
	int test_preempt(list<Event*>& eventQ,Process* p, int curtime);
	string getname();
	stack<Process*> runqueue;
private:

};
LCFS::LCFS()
{
}
LCFS::LCFS(int a,int b)
{
	quantum = a;
	maxprio = b;
}
LCFS::~LCFS()
{
}
void LCFS::add_process(Process* p)
{
	runqueue.push(p);
}
Process* LCFS::get_next_process()
{
	if (runqueue.empty()) return nullptr;
	Process* temp = runqueue.top();
	runqueue.pop();
	return temp;
}
int LCFS::test_preempt(list<Event*>& eventQ, Process* p, int curtime)
{
	return -1;
}
string LCFS::getname()
{
	return "LCFS";
}

class SRTF:public Scheduler
{
public:
	SRTF();
		SRTF(int,int);
	~SRTF();
	void add_process(Process* p);
	Process* get_next_process();
	int test_preempt(list<Event*>& eventQ, Process* p, int curtime);
	string getname();
	list<Process*> runqueue;
private:

};
SRTF::SRTF()
{
}
SRTF::SRTF(int a,int b)
{
	quantum = a;
	maxprio = b;
}
SRTF::~SRTF()
{
}
void SRTF::add_process(Process* p)
{
	if (runqueue.empty() || runqueue.back()->remainingtime <= p->remainingtime) {
		runqueue.push_back(p);
		return;
	}
	for (std::list<Process*>::iterator it = runqueue.begin(); it != runqueue.end(); it++) {
		if (p->remainingtime < (*it)->remainingtime)
		{
			runqueue.insert(it, p);
			return;
		}
	}
}
Process* SRTF::get_next_process()
{
	if (runqueue.empty()) return nullptr;
	Process* temp = runqueue.front();
	runqueue.pop_front();
	return temp;
}
int SRTF::test_preempt(list<Event*>& eventQ, Process* p, int curtime)
{
	return -1;
}
string SRTF::getname()
{
	return "SRTF";
}

class RR:public Scheduler
{
public:
	RR();
	RR(int,int);
	~RR();
	void add_process(Process* p);
	Process* get_next_process();
	int test_preempt(list<Event*>& eventQ, Process* p, int curtime);
	string getname();
	queue<Process*> runqueue;
private:

};
RR::RR()
{
}
RR::RR(int a,int b)
{
	quantum = a;
	maxprio = b;
}
RR::~RR()
{
}
void RR::add_process(Process* p)
{
	p->dynamic_priority = p->static_priority - 1;
	runqueue.push(p);
}
Process* RR::get_next_process()
{
	if (runqueue.empty()) return nullptr;
	Process* proc = runqueue.front();
	runqueue.pop();
	return proc;
}
int RR::test_preempt(list<Event*>& eventQ, Process* p, int curtime)
{
	return -1;
}

string RR::getname()
{
	return "RR "+to_string(quantum);
}

class PriorityScheduler:public Scheduler
{
public:
	PriorityScheduler();
	PriorityScheduler(int, int);
	~PriorityScheduler();
	void add_process(Process* p);
	Process* get_next_process();
	int test_preempt(list<Event*>& eventQ, Process* p, int curtime);
	string getname();
	queue<Process*>* activeQ;
	queue<Process*>* expiredQ;
private:

};
PriorityScheduler::PriorityScheduler()
{
	activeQ = nullptr;
	expiredQ = nullptr;
}
PriorityScheduler::PriorityScheduler(int a,int b)
{
	quantum = a;
	maxprio = b;
	activeQ = new queue<Process*>[maxprio];
	expiredQ = new queue<Process*>[maxprio];
}
PriorityScheduler::~PriorityScheduler()
{
	delete[] activeQ;
	delete[] expiredQ;
}
void PriorityScheduler::add_process(Process* p)
{
	if (p->dynamic_priority <= -1)
	{
		p->dynamic_priority = p->static_priority - 1;
		expiredQ[p->dynamic_priority].push(p);
	}
	else
	{
		activeQ[p->dynamic_priority].push(p);
	}
}
Process* PriorityScheduler::get_next_process()
{
	for (int i = maxprio-1; i >=0; i--)
	{
		if (!activeQ[i].empty())
		{
			Process* temp = activeQ[i].front();
			activeQ[i].pop();
			return temp;
		}
	}
	queue<Process*>* temp = activeQ;
	activeQ = expiredQ;
	expiredQ = temp;
	for (int i = maxprio - 1; i >= 0; i--)
	{
		if (!activeQ[i].empty())
		{
			Process* temp = activeQ[i].front();
			activeQ[i].pop();
			return temp;
		}
	}
	return nullptr;
}
int PriorityScheduler::test_preempt(list<Event*>& eventQ, Process* p, int curtime)
{
	return -1;
}
string PriorityScheduler::getname()
{
	return "PRIO " + to_string(quantum);
}

class PREEPRIO:public Scheduler
{
public:
	PREEPRIO();
	PREEPRIO(int, int);
	~PREEPRIO();
	void add_process(Process* p);
	Process* get_next_process();
	int test_preempt(list<Event*>& eventQ, Process* p, int curtime);
	string getname();
	queue<Process*>* activeQ;
	queue<Process*>* expiredQ;
private:

};
PREEPRIO::PREEPRIO()
{
	activeQ = nullptr;
	expiredQ = nullptr;
}
PREEPRIO::PREEPRIO(int a, int b)
{
	quantum = a;
	maxprio = b;
	activeQ = new queue<Process*>[maxprio];
	expiredQ = new queue<Process*>[maxprio];
}
PREEPRIO::~PREEPRIO()
{
	delete[] activeQ;
	delete[] expiredQ;
}
void PREEPRIO::add_process(Process* p)
{
	if (p->dynamic_priority <= -1)
	{
		p->dynamic_priority = p->static_priority - 1;
		expiredQ[p->dynamic_priority].push(p);
	}
	else
	{
		activeQ[p->dynamic_priority].push(p);
	}
}
Process* PREEPRIO::get_next_process()
{
	for (int i = maxprio - 1; i >= 0; i--)
	{
		if (!activeQ[i].empty())
		{
			Process* temp = activeQ[i].front();
			activeQ[i].pop();
			return temp;
		}
	}
	queue<Process*>* temp = activeQ;
	activeQ = expiredQ;
	expiredQ = temp;
	for (int i = maxprio - 1; i >= 0; i--)
	{
		if (!activeQ[i].empty())
		{
			Process* temp = activeQ[i].front();
			activeQ[i].pop();
			return temp;
		}
	}
	return nullptr;
}
int PREEPRIO::test_preempt(list<Event*> &eventQ, Process* p, int curtime)
{
	list<Event*>::iterator it = findeventiteratorbyprocess(eventQ, p);
	int eventtimeofp = (*it)->timestamp;
	return eventtimeofp - curtime;
}
string PREEPRIO::getname()
{
	return "PREPRIO " + to_string(quantum);
}



int myrandom(vector<int> &randvals,int &currentind, int randnumbers, int burst) { //return [1...burst] randnumber
	if (currentind >= randnumbers) {
		currentind = 0;
	}
	int res = 1 + (randvals[currentind] % burst);
	currentind++;
	return res;
}

SimRes* Simulation(list<Event*> &eventQ,vector<int> &randvals,int &currentind,int randnumbers, Scheduler* scheduler, Debugparas debugparas) {
	Event* evt;
	int CURRENT_TIME=0;
	bool CALL_SCHEDULER = false;
	Process* currentproc = nullptr;
	int time_cpubusy = 0;
	int time_iobusy = 0;
	int time_iononbusy = 0;
	int ionumbers = 0;
	bool preemptdebug1 = false;
	int preemptdebug2 = -1;
	while ((evt = get_event(eventQ))!=nullptr) {
		rm_event(eventQ);
		if (ionumbers > 0)
		{
			time_iobusy += evt->timestamp - CURRENT_TIME;  //the event time stamp is non decreasing. Every time increase, check if the increase duration is iobusy.
		}
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
				proc->IT += timeInPrevState;
				ionumbers--;
			}
			
			if (currentproc != nullptr)
			{
				preemptdebug1 = proc->dynamic_priority > currentproc->dynamic_priority;
				preemptdebug2 = scheduler->test_preempt(eventQ, currentproc, CURRENT_TIME);
				if (preemptdebug1 && preemptdebug2 > 0)
				{
					newevent = new Event();
					newevent->timestamp = CURRENT_TIME;
					newevent->oldstate = STATE_RUNNING;
					newevent->newstate = STATE_PREEMPTED;
					newevent->process = currentproc;
					rm_event_preempt(eventQ, currentproc);
					put_event(eventQ, newevent);
				}
			}
			proc->state = STATE_READY;
			proc->state_ts = CURRENT_TIME;
			scheduler->add_process(proc);
			CALL_SCHEDULER = true;

			if (debugparas.eflag == true)
			{
				if (fromstate == STATE_BLOCKED)
				{
					printf("%d %d %d: BLOCK -> READY\n", CURRENT_TIME, proc->pid, timeInPrevState);
				}
				else if (fromstate == STATE_CREATED)
				{
					printf("%d %d %d: CREATED -> READY\n", CURRENT_TIME, proc->pid, timeInPrevState);
				}
				if (currentproc != nullptr&&preemptdebug2>-1) {
				printf("    --> PrioPreempt Cond1 = %d Cond2 = %d (%d) -- > ", preemptdebug1, preemptdebug2>0, preemptdebug2);
				if (preemptdebug1 && preemptdebug2>0)
				{
					printf("YES\n");
				}
				else
				{
					printf("NO\n");
				}
			}
			}
			preemptdebug1 = false;
			preemptdebug2 = -1;
			break;
		case STATE_PREEMPTED: 
			// transition 5
			// similar to TRANS_TO_READY // must come from RUNNING (preemption)  //quantum expiration or PREPRIO
			// add to runqueue (no event is generated
			proc->state = STATE_PREEMPTED;
			proc->state_ts = CURRENT_TIME;
			proc->remainingtime = proc->remainingtime - timeInPrevState;
			proc->remainingcpuburst = proc->remainingcpuburst - timeInPrevState;
			if (debugparas.eflag == true)
			{
				printf("%d %d %d: RUNNG -> READY  cb=%d rem=%d prio=%d\n", CURRENT_TIME, proc->pid, timeInPrevState, proc->remainingcpuburst, proc->remainingtime, proc->dynamic_priority);
			}
			time_cpubusy += timeInPrevState;
			currentproc = nullptr;
			
			proc->dynamic_priority--;

			scheduler->add_process(proc);
			CALL_SCHEDULER = true;

			
			break;
		case STATE_RUNNING:
			// transition 2
			// create event for either preemption or blocking
			
			proc->CW += timeInPrevState;

			if (proc->remainingcpuburst == 0) {
				cpuburst = myrandom(randvals, currentind, randnumbers, proc->CB);
				if (cpuburst > proc->remainingtime) cpuburst = proc->remainingtime;
				//if (cpuburst > scheduler->quantum) cpuburst = scheduler->quantum;
			}
			else
			{
				cpuburst = proc->remainingcpuburst;
			}

			newevent = new Event();
			if (cpuburst > scheduler->quantum)
			{
				newevent->newstate = STATE_PREEMPTED;
				newevent->timestamp = CURRENT_TIME + scheduler->quantum;
			}
			/*else if (scheduler->test_preempt(proc, CURRENT_TIME))
			{

				newevent->newstate = STATE_PREEMPTED;
				newevent->timestamp = CURRENT_TIME + cpuburst;
			}*/
			else
			{
				newevent->newstate = STATE_BLOCKED;
				newevent->timestamp = CURRENT_TIME + cpuburst;
			}
			newevent->oldstate = STATE_RUNNING;
			newevent->process = proc;
			proc->remainingcpuburst = cpuburst;
			proc->state = STATE_RUNNING;
			proc->state_ts = CURRENT_TIME;
			put_event(eventQ, newevent);

			if (debugparas.eflag == true)
			{
				printf("%d %d %d: READY -> RUNNG cb=%d rem=%d prio=%d\n", CURRENT_TIME, proc->pid, timeInPrevState, cpuburst,proc->remainingtime,proc->dynamic_priority);
			}
			break;
		case STATE_BLOCKED:
			// transition 3
			// create an event for when process becomes READY again
			proc->remainingtime = proc->remainingtime - timeInPrevState;
			proc->remainingcpuburst = proc->remainingcpuburst - timeInPrevState;
			time_cpubusy += timeInPrevState;
			currentproc = nullptr;
			if (proc->remainingtime == 0)
			{
				proc->FT=CURRENT_TIME;
				proc->state = STATE_TERMINATED;
				CALL_SCHEDULER = true;
				if (debugparas.eflag == true)
				{
				printf("%d %d %d: Done\n", CURRENT_TIME, proc->pid, timeInPrevState);
				}
				break;
			}


			ioburst = myrandom(randvals,currentind,randnumbers,proc->IO);
			ionumbers++;
			newevent=new Event();
			newevent->timestamp = CURRENT_TIME + ioburst;
			newevent->newstate = STATE_READY;
			newevent->oldstate = STATE_BLOCKED;
			newevent->process = proc;
			proc->state = STATE_BLOCKED;
			proc->state_ts = CURRENT_TIME;
			put_event(eventQ, newevent);
			CALL_SCHEDULER = true;

			if (debugparas.eflag == true)
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
	Scheduler* scheduler = nullptr;
	string ss = argv[1];  //scheduled select
	string f1 = argv[2];
	string f2 = argv[3];
	Debugparas debugparas;
	//debugparas.eflag = true;

	ifstream inputfile;
	ifstream randfile;
	int quantum = DEFAULTQUANTUM;
	int maxprio = DEFAULTMAXPRIO;
	switch (ss[1])
	{
	case'F':
		//do FCFS
		scheduler = new FIFO(quantum, maxprio);
		break;
	case'L':
		//do LCFS
		scheduler = new LCFS(quantum, maxprio);
		break;
	case'S':
		//do SRTF
		scheduler = new SRTF(quantum, maxprio);
		break;	
	case'R':
		//do RR
		sscanf(ss.substr(2).c_str(), "%d", &quantum);
		scheduler = new RR(quantum,maxprio);
		break;
	case'P':
		//do PRIO
		sscanf(ss.substr(2).c_str(), "%d:%d", &quantum, &maxprio);
		scheduler = new PriorityScheduler(quantum, maxprio);
		break;
	case'E':
		//do PREE PRIO
		sscanf(ss.substr(2).c_str(), "%d:%d", &quantum, &maxprio);
		scheduler = new PREEPRIO(quantum, maxprio);
		break;
	default:
		break;
	}

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
	randfile.close();

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
		temp->static_priority = myrandom(randvals, randvalofs, randmax, scheduler->maxprio);
		temp->dynamic_priority = temp->static_priority-1;
		temp->remainingtime = temp->TC;
		temp->pid = num_processes;
		temp->state_ts = temp->AT;
		allprocess.push_back(temp);
		num_processes++;
		Event* newevent = new Event();
		newevent->oldstate = STATE_CREATED;
		newevent->newstate = STATE_READY;
		newevent->timestamp = temp->AT;
		newevent->process = temp;
		put_event(eventQ, newevent);
	}
	inputfile.close();
	SimRes* res=Simulation(eventQ, randvals,randvalofs, randmax, scheduler,debugparas);

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