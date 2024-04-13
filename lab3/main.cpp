#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <deque>
#include <list>
#include <vector>
#include <stack>

#define MAX_VPAGES 64
#define MAX_FRAMES 128
using namespace std;
struct PTE_t
{
	unsigned int PRESENT : 1;
	unsigned int REFERENCED : 1;
	unsigned int MODIFIED : 1;
	unsigned int WRITE_PROTECT : 1;
	unsigned int PAGEDOUT : 1;
	unsigned int FRAME_NUMBER : 7;
	PTE_t() : PRESENT(0), REFERENCED(0), MODIFIED(0), WRITE_PROTECT(0), PAGEDOUT(0), FRAME_NUMBER(0) {
	}
};

struct frame_t
{
	int process;
	int vpage;

	int ref_count;
	int locked;
};

class VMA
{
public:
	VMA();
	~VMA();
	int start_vpage;
	int end_vpage;
	bool write_protected;
	bool file_mapped;

	VMA(): start_vpage(0), end_vpage(0), write_protected(0), file_mapped(0)
	{
	}
};

class Process
{
public:
	Process();
	~Process();
	int pid;
	vector<VMA*> vmas;
	PTE_t page_table[MAX_VPAGES];

	unsigned long long maps;
	unsigned long long unmaps;
	unsigned long long ins;
	unsigned long long outs;
	unsigned long long fins;
	unsigned long long fouts;
	unsigned long long zeros;
	unsigned long long segv;
	unsigned long long segprot;
	Process() : pid(0), vmas(), maps(0), unmaps(0), ins(0), outs(0), fins(0), fouts(0), zeros(0), segv(0), segprot(0)
	{

	}
};

class Pager {
	virtual frame_t* select_victim_frame() = 0; // virtual base class
};

frame_t* get_frame() {
	frame_t* frame = allocate_frame_from_free_list();
	if (frame == NULL) frame = THE_PAGER->select_victim_frame();
	return frame;
}

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

int get_next_instruction(ifstream& file, string* command, int* id)
{
	string oneline;
	get_valid_line(file, oneline);
	sscanf(oneline.c_str(), "%c %d", command, id);
}
int main(int argc, char* argv[]) {
	Pager* pager = nullptr;
	string f;
	string a;
	string o;
	string file1;
	string file2;
	int opt;
	bool outputO = false;
	bool outputP = false;
	bool outputF = false;
	bool outputS = false;
	/*
	* while ((opt = getopt(argc, argv, "f:a:o:")) != -1) {
		switch (opt) {
			case 'f':
				f = string("-") + (char)opt + string(optarg);
				break;
			case 'a':
				a = string("-") + (char)opt + string(optarg);
				break;
			case 'o':
				o = string("-") + (char)opt + string(optarg);
				break;
			default:
				std::cerr << "Usage: ./mmu -f<num_frames> -a<algo> [-o<options>] inputfile randomfile" << std::endl;
				return 1;
		}
	}
	*/
	f = argv[1];
	a = argv[2];
	o = argv[3];
	file1 = argv[4];
	file2 = argv[5];
	//以上进linux改
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

	ifstream inputfile;
	ifstream randfile;
	inputfile.open(file1);
	if (!inputfile.is_open())
	{
		cout << "no file 1";
		return 0;
	}
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

	switch (ss[2])
	{
	case'F':
		//do FCFS
		pager = new FIFO(quantum, maxprio);
		break;
	case'L':
		//do LCFS
		pager = new LCFS(quantum, maxprio);
		break;
	case'S':
		//do SRTF
		pager = new SRTF(quantum, maxprio);
		break;
	case'R':
		//do RR
		sscanf(ss.substr(3).c_str(), "%d", &quantum);
		pager = new RR(quantum, maxprio);
		break;
	case'P':
		//do PRIO
		sscanf(ss.substr(3).c_str(), "%d:%d", &quantum, &maxprio);
		pager = new PriorityScheduler(quantum, maxprio);
		break;
	case'E':
		//do PREE PRIO
		sscanf(ss.substr(3).c_str(), "%d:%d", &quantum, &maxprio);
		pager = new PREEPRIO(quantum, maxprio);
		break;
	default:
		break;
	}

	while (getline(randfile, oneline)) //read all into the vector
	{
		sscanf(oneline.c_str(), "%d", &temp0);
		randvals.push_back(temp0);
	}
	randfile.close();

	string oneline;
	int temp1;
	int temp2;
	int temp3;
	int temp4;
	int number_processes = 0;
	int number_pages = 0;
	frame_t frame_table[MAX_FRAMES];
	vector<Process*> process_table;
	if (get_valid_line(inputfile, oneline) == 0)
	{
		return 0;//Won't ever be triggered
	}
	sscanf(oneline.c_str(), "%d", &number_processes);

	for (int i = 0; i < number_processes; i++)
	{
		Process* newproc = new Process();
		newproc->pid = i;
		process_table.push_back(newproc);
		get_valid_line(inputfile, oneline);
		sscanf(oneline.c_str(), "%d", &number_pages);
		for (int j = 0; j < number_pages; j++)
		{
			get_valid_line(inputfile, oneline);
			VMA* newvma = new VMA();
			sscanf(oneline.c_str(), "%d %d %d %d", &newvma->start_vpage, &newvma->end_vpage,&newvma->write_protected,&newvma->file_mapped);
			newproc->vmas.push_back(newvma);
		}
	}

	string operation;
	int vpage;
	Process* current_process;
	int instruction_count = 0;
	//before this, all pte bits should be zero
	while (get_next_instruction(inputfile, &operation, &vpage)) {
		instruction_count++;
		if (operation[0] == 'c')
		{
			continue;
		}
		if (operation[0] == 'e')
		{
			continue;
		}
		// handle special case of “c” and “e” instruction
		// now the real instructions for read and write
		PTE_t* pte = &current_process->page_table[vpage];
		if (!pte->PRESENT) {
			// this in reality generates the page fault exception and now you 
			// // verify this is actually a valid page in a vma if not raise error and next inst
			if (!valid())
			{
				exception SEGV;
				continue;
			}
			frame_t* newframe = get_frame(); //How about return frame number
			UNMAP;
			//-> figure out if/what to do with old frame if it was mapped
			if (newframe->maps->MODIFIED == 1)
			{
				OUT / FOUT;
			}

			// see general outline in MM-slides under Lab3 header and writeup below
			// see whether and how to bring in the content of the access page.
			IN;
			current_process->page_table[vpage].frame = allocated_frame; PRINT("MAP", newframenumber);
			pte->PRESENT = 1;
		}
		// now the page is definitely present // check write protection
		if (current_process->page_table[vpage].WRITE_PROTECT == 1 && operation[0] == 'r')
		{
			SEGPROT;
			current_process->page_table[vpage].REFERENCED = 1;
		}
		// simulate instruction execution by hardware by updating the R/M PTE bits
		update_pte(pte->) bits based on operations.
	}


	while (get_valid_line(inputfile, oneline) == 1)
	{
		sscanf(oneline.c_str(), "%c %d", &number_pages);
	}



	inputfile.close();
	SimRes* res = Simulation(eventQ, randvals, randvalofs, randmax, scheduler, debugparas);


	unsigned long long total_inst = 0;
	unsigned long long total_ctx_switches = 0;
	unsigned long long total_proc_exits = 0;

	if (outputP)
	{
		for (int i = 0; i < number_processes; i++)
		{
			printf("PT[%d]:");
			for (int j = 0; j < MAX_VPAGES; j++)
			{
				if (!process_table[i]->page_table[j].PRESENT&&process_table[i]->page_table[j].PAGEDOUT) //referenced but not modified
				{
					printf(" #");
				}
				else if(!process_table[i]->page_table[j].PRESENT && !process_table[i]->page_table[j].MODIFIED)
				{
					printf(" *");
				}
				else
				{
					printf(" %d:");
					if (process_table[i]->page_table[j].REFERENCED)
					{
						printf("R");
					}
					else
					{
						printf("-");
					}
					if (process_table[i]->page_table[j].MODIFIED)
					{
						printf("M");
					}
					else
					{
						printf("-");
					}
					if (process_table[i]->page_table[j].PAGEDOUT)
					{
						printf("S");
					}
					else
					{
						printf("-");
					}
				}
			}
			printf("\n");
		}
	}
	if (outputF)
	{

	}

	int totalturnaround = 0;
	int totalcpuwaiting = 0;
	double cpu_util = 100.0 * (res->time_cpubusy / (double)res->finishtime);
	double io_util = 100.0 * (res->time_iobusy / (double)res->finishtime);
	double throughput = 100.0 * (num_processes / (double)res->finishtime);
	cout << scheduler->getname() << endl;
	for (int i = 0; i < allprocess.size(); i++)
	{
		totalturnaround += allprocess[i]->FT - allprocess[i]->AT;
		totalcpuwaiting += allprocess[i]->CW;
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", i, allprocess[i]->AT, allprocess[i]->TC,
			allprocess[i]->CB, allprocess[i]->IO, allprocess[i]->static_priority,
			allprocess[i]->FT, allprocess[i]->FT - allprocess[i]->AT, allprocess[i]->IT, allprocess[i]->CW);
	}
	double averturnaround = totalturnaround / (double)num_processes;
	double avercpuwaiting = totalcpuwaiting / (double)num_processes;
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", res->finishtime, cpu_util, io_util, averturnaround, avercpuwaiting, throughput);

	return 0;
}