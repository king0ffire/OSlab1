#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <deque>
#include <list>
#include <vector>
#include <stack>
#include <cstdint>
//#include <unistd.h>
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
	IOscheduler();
	~IOscheduler();
	virtual IORequest* select_next_request() = 0;
	virtual void addqueue(IORequest* i) = 0;
};

IOscheduler::IOscheduler()
{
}

IOscheduler::~IOscheduler()
{
}

class FIFO :public IOscheduler {
	queue<IORequest*> requestQ;

	// 通过 IOscheduler 继承
	IORequest* select_next_request() override
	{
		if(requestQ.empty())return nullptr;
		IORequest* temp = requestQ.front();
		requestQ.pop();
		return temp;
	}
	void addqueue(IORequest* i) override
	{
		requestQ.push(i);
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

	/*
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
	*/

	s = "-sN";
	outputV = false;
	outputQ = false;
	outputF = false;
	file1 = "F:/美国学习资料/OS/lab4/lab4_assign/input0";

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
	bool currentdirectiontohigh=true;
	int currenttrack = 0;

	int timeiobusy = 0;
	int totaltrunaround=0;
	int totalwaittime = 0;
	int maxwaittime = 0;
	while (true)
	{
		if (requestpointer<numberofIORequest&&request_table[requestpointer]->timearrive == currenttime)
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
			pendingIOtask = scheduler->select_next_request();
			if (pendingIOtask != nullptr)
			{
				currentIOtask = pendingIOtask;
				currentIOtask->starttime = currenttime;
				if (currenttrack < currentIOtask->track)
				{
					currentdirectiontohigh = true;
					currentIOtask->endtime = currenttime + (pendingIOtask->track - currenttrack);
				}
				else
				{
					currentdirectiontohigh = false;
					currentIOtask->endtime = currenttime - (pendingIOtask->track - currenttrack);
				}
			}
			else if (requestpointer>=numberofIORequest)
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
		printf("%5d: %5d %5d %5d\n", request_table[i]->id, request_table[i]->timearrive,request_table[i]->starttime,request_table[i]->endtime);
	}
	printf("SUM: %d %d %.4lf %.2lf %.2lf %d\n",	currenttime, timeiobusy, (double)timeiobusy / currenttime, (double)totaltrunaround / numberofIORequest, (double) totalwaittime/ numberofIORequest, maxwaittime);
	return 0;
}