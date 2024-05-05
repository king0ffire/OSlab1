#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <deque>
#include <list>
#include <vector>
#include <stack>
#include <cstdint>
#include <unistd.h>
#include <cmath>
#pragma warning(disable : 4996)

#define MAX_VPAGES 64
#define MAX_FRAMES 128
using namespace std;

class IORequest
{
public:
	int id;
	int timearrive;
	int track;
	int starttime;
	int endtime;

	IORequest()
		: id(0), timearrive(0), track(0), starttime(0), endtime(0)
	{
	}
};

class IOscheduler
{
public:
	virtual IORequest* select_next_request(int currenttrack, bool direction) = 0;
	virtual void addqueue(IORequest* i) = 0;
};


class FIFO :public IOscheduler {
	queue<IORequest*> requestQ;

	// 通过 IOscheduler 继承
	IORequest* select_next_request(int currenttrack, bool direction) override
	{
		if (requestQ.empty())return nullptr;
		IORequest* temp = requestQ.front();
		requestQ.pop();
		return temp;
	}
	void addqueue(IORequest* i) override
	{
		requestQ.push(i);
	}
};

class SSTF :public IOscheduler
{
public:
	list<IORequest*> requestQ;
	// 通过 IOscheduler 继承
	IORequest* select_next_request(int currenttrack, bool direction) override
	{
		if (requestQ.empty())return nullptr;
		list<IORequest*>::iterator mini = requestQ.begin();
		for (list<IORequest*>::iterator it = requestQ.begin(); it != requestQ.end(); ++it)
		{
			if (abs((*it)->track - currenttrack) < abs((*mini)->track - currenttrack))
			{
				mini = it;
			}
		}
		IORequest* temp = *mini;
		requestQ.erase(mini);
		return temp;
	}
	void addqueue(IORequest* i) override
	{
		requestQ.push_back(i);
	}

};

class LOOK :public IOscheduler
{
public:
	list<IORequest*> requestQ;
	// 通过 IOscheduler 继承
	IORequest* select_next_request(int currenttrack, bool direction) override
	{
		if (requestQ.empty())return nullptr;
		list<IORequest*>::iterator closestleft = requestQ.end();
		list<IORequest*>::iterator closestright = requestQ.end();
		IORequest* temp1 = nullptr;
		for (list<IORequest*>::iterator it = requestQ.begin(); it != requestQ.end(); ++it)
		{
			if ((*it)->track == currenttrack)
			{
				temp1 = *it;
				requestQ.erase(it);
				return temp1;
			}
			if ((*it)->track < currenttrack)
			{
				if (closestleft == requestQ.end() || currenttrack - (*it)->track < currenttrack - (*closestleft)->track)
				{
					closestleft = it;
				}
			}
			else
			{
				if (closestright == requestQ.end() || (*it)->track - currenttrack < (*closestright)->track - currenttrack)
				{
					closestright = it;
				}
			}
		}
		list<IORequest*>::iterator temp = requestQ.end();
		if (direction)
		{
			if (closestright != requestQ.end())
			{
				temp = closestright;
			}
			else
			{
				temp = closestleft;
			}
		}
		else
		{
			if (closestleft != requestQ.end())
			{
				temp = closestleft;
			}
			else
			{
				temp = closestright;
			}
		}
		// temp cannot be end()
		temp1 = *temp;
		requestQ.erase(temp);
		return temp1;
	}

	void addqueue(IORequest* i) override
	{
		requestQ.push_back(i);
	}

};


class CLOOK :public IOscheduler
{
public:
	list<IORequest*> requestQ;
	// 通过 IOscheduler 继承
	IORequest* select_next_request(int currenttrack, bool direction) override
	{
		if (requestQ.empty())return nullptr;
		//list<IORequest*>::iterator closestleft = requestQ.end();
		list<IORequest*>::iterator closestright = requestQ.end();
		list<IORequest*>::iterator mini = requestQ.end();
		//list<IORequest*>::iterator maxi = requestQ.end();

		IORequest* temp1 = nullptr;
		for (list<IORequest*>::iterator it = requestQ.begin(); it != requestQ.end(); ++it)
		{
			if ((*it)->track == currenttrack)
			{
				temp1 = *it;
				requestQ.erase(it);
				return temp1;
			}
			if (mini==requestQ.end()||(*it)->track < (*mini)->track)
			{
				mini = it;
			}
			if ((*it)->track > currenttrack)
			{
				if (closestright == requestQ.end() || (*it)->track - currenttrack < (*closestright)->track - currenttrack)
				{
					closestright = it;
				}
			}
		}
		list<IORequest*>::iterator temp = requestQ.end();
		if (closestright != requestQ.end())
		{
			temp = closestright;
		}
		else
		{
			temp = mini;
		}
		// temp cannot be end()
		temp1 = *temp;
		requestQ.erase(temp);
		return temp1;
	}

	void addqueue(IORequest* i) override
	{
		requestQ.push_back(i);
	}

};

class FLOOK :public IOscheduler
{
public:
	list<IORequest*> requestQ_active;
	list<IORequest*> requestQ_add;
	// 通过 IOscheduler 继承
	IORequest* select_next_request(int currenttrack, bool direction) override
	{
		if (requestQ_active.empty()&&requestQ_add.empty())return nullptr;
		if (requestQ_active.empty())
		{
			requestQ_active.swap(requestQ_add);
		}
		list<IORequest*>::iterator closestleft = requestQ_active.end();
		list<IORequest*>::iterator closestright = requestQ_active.end();
		IORequest* temp1 = nullptr;
		for (list<IORequest*>::iterator it = requestQ_active.begin(); it != requestQ_active.end(); ++it)
		{
			if ((*it)->track == currenttrack)
			{
				temp1 = *it;
				requestQ_active.erase(it);
				return temp1;
			}
			if ((*it)->track < currenttrack)
			{
				if (closestleft == requestQ_active.end() || currenttrack - (*it)->track < currenttrack - (*closestleft)->track)
				{
					closestleft = it;
				}
			}
			else
			{
				if (closestright == requestQ_active.end() || (*it)->track - currenttrack < (*closestright)->track - currenttrack)
				{
					closestright = it;
				}
			}
		}
		list<IORequest*>::iterator temp = requestQ_active.end();
		if (direction)
		{
			if (closestright != requestQ_active.end())
			{
				temp = closestright;
			}
			else
			{
				temp = closestleft;
			}
		}
		else
		{
			if (closestleft != requestQ_active.end())
			{
				temp = closestleft;
			}
			else
			{
				temp = closestright;
			}
		}
		// temp cannot be end()
		temp1 = *temp;
		requestQ_active.erase(temp);
		return temp1;
	}

	void addqueue(IORequest* i) override
	{
		requestQ_add.push_back(i);
	}

};


int get_valid_line(ifstream& file, string& oneline)
{
	while (true)
	{
		getline(file, oneline);
		if (file.eof()) {
			return 0;
		}
		if (oneline[0] != '#')
		{
			return 1;
		}
	}
}


int main(int argc, char* argv[]) {
	IOscheduler* scheduler = nullptr;
	string s;
	string file1;
	int opt;
	bool outputV = false;
	bool outputQ = false;
	bool outputF = false;

	
	while ((opt = getopt(argc, argv, "s:v:q:f:")) != -1) {
		switch (opt) {
		case 's':
			s = string("-") + (char)opt + string(optarg);
			break;
		case 'v':
			outputV = true;
			break;
		case 'q':
			outputQ = true;
			break;
		case 'f':
			outputF = true;
			break;
		default:
			std::cerr << "./iosched [ –s<schedalgo> | -v | -q | -f ] <inputfile>" << std::endl;
			return 1;
		}
	}
	file1 = argv[optind];
	//file2 = argv[optind + 1];
	
	/*
	s = "-sF";
	outputV = false;
	outputQ = false;
	outputF = false;
	file1 = "F:/美国学习资料/OS/lab4/lab4_assign/input9";
	*/
	//以上进linux改

	/*
	for (int i = 2; i < o.size(); i++)
	{
		switch (o[i])
		{
		case'O':
			outputO = true;
			break;
		case'P':
			outputP = true;
			break;
		case'F':
			outputF = true;
			break;
		case'S':
			outputS = true;
			break;
		}
	}
	*/
	ifstream inputfile;
	//ifstream randfile;
	inputfile.open(file1);
	if (!inputfile.is_open())
	{
		cout << "no file 1";
		return 0;
	}
	/*
	randfile.open(file2);
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
	*/
	if (s.length() > 0) {
		switch (s[2])
		{
		case'N':
			scheduler = new FIFO();
			break;
		case'S':
			scheduler = new SSTF();
			break;
		case'L':
			scheduler = new LOOK();
			break;
		case'C':
			scheduler = new CLOOK();
			break;
		case'F':
			scheduler = new FLOOK();
			break;
		default:
			scheduler = new FIFO();
		}
	}
	else
	{
		scheduler = new FIFO();
	}
	/*
	while (getline(randfile, oneline)) //read all into the vector
	{
		sscanf(oneline.c_str(), "%d", &temp0);
		randvals.push_back(temp0);
	}*/
	//randfile.close();

	int timearrive = 0;
	int track = 0;
	/*
	frame_t frame_table[MAX_FRAMES];
	deque<int> free_pool;
	for (int i = 0; i < frame_numbers; i++)
	{
		free_pool.push_back(i);
	}

	vector<Process*> process_table;
	*/
	vector<IORequest*> request_table;
	string oneline;
	int numberofIORequest = 0;
	while (get_valid_line(inputfile, oneline))
	{
		sscanf(oneline.c_str(), "%d %d", &timearrive, &track);
		IORequest* temp = new IORequest();
		temp->id = numberofIORequest;
		temp->timearrive = timearrive;
		temp->track = track;
		request_table.push_back(temp);
		numberofIORequest++;
	}

	int currenttime = 0;
	int requestpointer = 0;
	IORequest* currentIOtask = nullptr;
	IORequest* pendingIOtask = nullptr;
	bool currentdirectiontohigh = true;
	int currenttrack = 0;

	unsigned long long  timeiobusy = 0;
	unsigned long long totaltrunaround = 0;
	unsigned long long totalwaittime = 0;
	unsigned long long  maxwaittime = 0;
	while (true)
	{
		if (requestpointer < numberofIORequest && request_table[requestpointer]->timearrive == currenttime)
		{
			scheduler->addqueue(request_table[requestpointer]);
			requestpointer++;
			continue;
		}
		if (currentIOtask != nullptr && currentIOtask->endtime == currenttime)
		{
			totaltrunaround += currentIOtask->endtime - currentIOtask->timearrive;
			totalwaittime += currentIOtask->starttime - currentIOtask->timearrive;
			if (currentIOtask->starttime - currentIOtask->timearrive > maxwaittime)
			{
				maxwaittime = currentIOtask->starttime - currentIOtask->timearrive;
			}
			currentIOtask = nullptr;
			continue;
		}
		if (currentIOtask == nullptr)
		{
			pendingIOtask = scheduler->select_next_request(currenttrack, currentdirectiontohigh);
			if (pendingIOtask != nullptr)
			{
				currentIOtask = pendingIOtask;
				currentIOtask->starttime = currenttime;
				if (currenttrack < currentIOtask->track)
				{
					currentdirectiontohigh = true;
					currentIOtask->endtime = currenttime + (pendingIOtask->track - currenttrack);
				}
				else if (currenttrack > currentIOtask->track)
				{
					currentdirectiontohigh = false;
					currentIOtask->endtime = currenttime - (pendingIOtask->track - currenttrack);
				}
				else
				{
					currentIOtask->endtime = currenttime;
				}
				continue;
			}
			else if (requestpointer >= numberofIORequest)
			{
				break;
			}

		}
		if (currentIOtask != nullptr)
		{
			if (currentdirectiontohigh)
			{
				currenttrack++;
			}
			else
			{
				currenttrack--;
			}
			timeiobusy++;
		}
		currenttime++;
	}

	inputfile.close();
	for (int i = 0; i < numberofIORequest; i++)
	{
		printf("%5d: %5d %5d %5d\n", request_table[i]->id, request_table[i]->timearrive, request_table[i]->starttime, request_table[i]->endtime);
	}
	printf("SUM: %d %d %.4lf %.2lf %.2lf %d\n", currenttime, timeiobusy, (double)timeiobusy / currenttime, (double)totaltrunaround / numberofIORequest, (double)totalwaittime / numberofIORequest, maxwaittime);
	return 0;
}