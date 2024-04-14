#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <deque>
#include <list>
#include <vector>
#include <stack>
#pragma warning(disable : 4996)

#define MAX_VPAGES 64
#define MAX_FRAMES 128
using namespace std;
struct PTE_t
{
	unsigned int VALID : 1; //for first fault?
	unsigned int PRESENT : 1;
	unsigned int REFERENCED : 1;
	unsigned int MODIFIED : 1;
	unsigned int WRITE_PROTECT : 1;
	unsigned int PAGEDOUT : 1;
	unsigned int FILEMAPPED : 1;
	unsigned int FRAME_NUMBER : 7;
	PTE_t() : VALID(0), PRESENT(0), REFERENCED(0), MODIFIED(0), WRITE_PROTECT(0), PAGEDOUT(0), FRAME_NUMBER(0) {
	}
};

struct frame_t
{
	int process;
	int vpage;
	int ref_count;
	int locked;
	frame_t() :process(-1), vpage(-1), ref_count(0), locked(0) {

	}
};

class VMA
{
public:
	int start_vpage;
	int end_vpage;
	bool write_protected;
	bool file_mapped;

	VMA() : start_vpage(0), end_vpage(0), write_protected(0), file_mapped(0)
	{
	}
};

class Process
{
public:
	int pid;
	vector<VMA*> vmas;
	PTE_t page_table[MAX_VPAGES];
	unsigned long maps;
	unsigned long  unmaps;
	unsigned long  ins;
	unsigned long  outs;
	unsigned long  fins;
	unsigned long  fouts;
	unsigned long  zeros;
	unsigned long  segv;
	unsigned long  segprot;
	Process() : pid(0), vmas(), maps(0), unmaps(0), ins(0), outs(0), fins(0), fouts(0), zeros(0), segv(0), segprot(0)
	{

	}
};

class Pager {
public:
	int index;
	virtual int select_victim_frame(frame_t* frame_table, int frame_numbers,vector<Process*> &process_table) = 0; // virtual base class
	Pager() :index(0)
	{

	}
};

class FIFO :public Pager {
	// 通过 Pager 继承
	int select_victim_frame(frame_t* frame_table, int frame_numbers, vector<Process*> &process_table) override
	{
		int j = index;
		for (int i = 0; i < frame_numbers; i++)
		{
			if (true) //always true, and the for loop only run once
			{
				index = (index + i + 1) % frame_numbers;
				return (j + i) % frame_numbers;
			}
		}
	}
};

class CLOCK :public Pager {
	// 通过 Pager 继承
	int select_victim_frame(frame_t* frame_table, int frame_numbers, vector<Process*>& process_table) override
	{
		int j = index;
		while (true)
		{
			if (process_table[frame_table[j].process]->page_table[frame_table[j].vpage].REFERENCED)
			{
				process_table[frame_table[j].process]->page_table[frame_table[j].vpage].REFERENCED = 0;
				j = (j + 1) % frame_numbers;
			}
			else
			{
				index = (j + 1) % frame_numbers;
				return j;
			}
		}
	}
};

int allocate_frame_from_free_list(frame_t* frame_table, deque<int>& free_pool)
{
	if (free_pool.empty())
	{
		return -1;
	}
	else
	{
		int temp = free_pool.front();
		free_pool.pop_front();
		return temp;
	}
}
int get_frame(frame_t* frame_table, int frame_numbers, deque<int>& free_pool, Pager* the_pager, vector<Process*>& process_table) {
	int frame = allocate_frame_from_free_list(frame_table, free_pool);
	if (frame == -1) frame = the_pager->select_victim_frame(frame_table, frame_numbers, process_table);
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

int find_valid_VMA_of_vpage(Process* current_process, int vpage)
{
	for (int i = 0; i < current_process->vmas.size(); i++)//If the vpage is unknow for the OS, then ask process's VMA if the vpage is a needed page in any VMA area.
	{
		if (vpage >= current_process->vmas[i]->start_vpage && vpage <= current_process->vmas[i]->end_vpage)
		{

			return i;
		}
	}
	return -1;
}

bool get_next_instruction(ifstream& file, string* command, int* id)
{
	string oneline;
	if (!get_valid_line(file, oneline)) return false;
	char ch[3];
	sscanf(oneline.c_str(), "%s %d", &ch, id);
	*command = ch;
	return true;
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
	int frame_numbers = 0;
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

	f = argv[1];
	a = argv[2];
	o = argv[3];
	file1 = argv[4];
	file2 = argv[5];*/
	f = "-f32";
	a = "-aC";
	o = "-oOSPF";
	file1 = "F:/美国学习资料/OS/lab3/lab3_assign/in11";
	file2 = "F:/美国学习资料/OS/lab3/lab3_assign/rfile";

	//以上进linux改

	frame_numbers = stoi(f.substr(2));

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

	switch (a[2])
	{
	case'F':
		pager = new FIFO();
		break;
	case'C':
		pager = new CLOCK();
		break;
		/*

		case'S':
			pager = new Clock();
			break;
		case'R':
			pager = new NRU();
			break;
		case'P':
			pager = new Aging();
			break;
		case'E':
			pager = new Working_Set();
			break;
			*/
	default:
		return 0; //very brute
	}

	while (getline(randfile, oneline)) //read all into the vector
	{
		sscanf(oneline.c_str(), "%d", &temp0);
		randvals.push_back(temp0);
	}
	randfile.close();

	int number_processes = 0;
	int number_pages = 0;
	frame_t frame_table[MAX_FRAMES];
	deque<int> free_pool;
	for (int i = 0; i < frame_numbers; i++)
	{
		free_pool.push_back(i);
	}

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
			sscanf(oneline.c_str(), "%d %d %d %d", &newvma->start_vpage, &newvma->end_vpage, &newvma->write_protected, &newvma->file_mapped);
			newproc->vmas.push_back(newvma);
		}
	}

	string operation;
	int vpage;
	Process* current_process = nullptr;
	unsigned long instruction_count = 0;
	unsigned long ctx_switches = 0;
	unsigned long process_exits = 0;
	unsigned long long total_costs = 0;
	//before this, all pte bits should be zero
	while (get_next_instruction(inputfile, &operation, &vpage)) {
		if (outputO)printf("%d: ==> %c %d\n", instruction_count, operation[0], vpage);
		instruction_count++;
		if (operation[0] == 'c')
		{
			total_costs += 130;
			ctx_switches++;
			current_process = process_table[vpage];
			continue;
		}
		if (operation[0] == 'e')
		{
			for (int i = 0; i < MAX_VPAGES; i++)
			{

				if (current_process->page_table[i].PRESENT)
				{
					if (outputO)printf(" UNMAP %d:%d\n", current_process->pid,i); //frame_table[newframe].process is the victim process
					total_costs += 410;
					current_process->unmaps++;

					//-> figure out if/what to do with old frame if it was mapped
					if (current_process->page_table[i].MODIFIED == 1)
					{
						if (current_process->page_table[i].FILEMAPPED)
						{
							if (outputO)printf(" FOUT\n");
							total_costs += 2800;
							current_process->fouts++;
						}
					}
					free_pool.push_back(current_process->page_table[i].FRAME_NUMBER);
					frame_table[current_process->page_table[i].FRAME_NUMBER].process = -1;
					frame_table[current_process->page_table[i].FRAME_NUMBER].vpage = -1;
					frame_table[current_process->page_table[i].FRAME_NUMBER].locked = 0;
					frame_table[current_process->page_table[i].FRAME_NUMBER].ref_count = 0;
				}
				current_process->page_table[i].MODIFIED = 0;
				current_process->page_table[i].REFERENCED = 0;
				current_process->page_table[i].PAGEDOUT = 0;
				current_process->page_table[i].PRESENT = 0;//its entry in the owning process’s page_table must be removed(“UNMAP”)
			}
			total_costs += 1230;
			process_exits++;
			continue;
		}
		// handle special case of “c” and “e” instruction
		
		total_costs += 1;//w and r instruction cost+1

		// now the real instructions for read and write
		PTE_t* pte = &current_process->page_table[vpage];
		if (!pte->PRESENT) {
			// Page fault handler: this in reality generates the page fault exception and now you 
			// verify this is actually a valid page in a vma if not raise error and next inst
			if (pte->VALID == 0)
			{
				int VMAindex = find_valid_VMA_of_vpage(current_process, vpage);
				if (VMAindex == -1) {
					if (outputO)printf(" SEGV\n");
					total_costs += 440;
					current_process->segv++;
					continue;
				}
				else
				{
					//init the vpage and its pte. Init when fault, instead of Init when ctx switch.
					pte->VALID = 1;
					pte->WRITE_PROTECT = current_process->vmas[VMAindex]->write_protected;
					pte->FILEMAPPED = current_process->vmas[VMAindex]->file_mapped;
				}
			}
			int newframe = get_frame(frame_table, frame_numbers, free_pool, pager, process_table); //How about return frame number
			if (frame_table[newframe].process != -1)
			{
				if (outputO)printf(" UNMAP %d:%d\n", frame_table[newframe].process, frame_table[newframe].vpage); //frame_table[newframe].process is the victim process
				total_costs += 410;
				process_table[frame_table[newframe].process]->unmaps++;

				//-> figure out if/what to do with old frame if it was mapped
				if (process_table[frame_table[newframe].process]->page_table[frame_table[newframe].vpage].MODIFIED == 1)
				{
					process_table[frame_table[newframe].process]->page_table[frame_table[newframe].vpage].PAGEDOUT = 1;
					if (process_table[frame_table[newframe].process]->page_table[frame_table[newframe].vpage].FILEMAPPED)
					{
						if (outputO)printf(" FOUT\n");
						total_costs += 2800;
						process_table[frame_table[newframe].process]->fouts++;
					}
					else
					{
						if (outputO)printf(" OUT\n");
						total_costs += 2750;
						process_table[frame_table[newframe].process]->outs++;
					}
				}
				process_table[frame_table[newframe].process]->page_table[frame_table[newframe].vpage].PRESENT = 0;//its entry in the owning process’s page_table must be removed(“UNMAP”)
				process_table[frame_table[newframe].process]->page_table[frame_table[newframe].vpage].MODIFIED = 0;
				process_table[frame_table[newframe].process]->page_table[frame_table[newframe].vpage].REFERENCED = 0;

			}
			// see general outline in MM-slides under Lab3 header and writeup below
			// see whether and how to bring in the content of the access page.

			if (pte->FILEMAPPED)
			{
				if (outputO)printf(" FIN\n");
				total_costs += 2350;
				current_process->fins++;
			}
			else if (pte->PAGEDOUT == 0)
			{
				if (outputO)printf(" ZERO\n");
				total_costs += 150;
				current_process->zeros++;
			}
			else
			{
				if (outputO)printf(" IN\n");
				total_costs += 3200;
				current_process->ins++;
			}

			pte->FRAME_NUMBER = newframe;  //pte map to frame
			frame_table[newframe].process = current_process->pid; //frame reverse map to pte
			frame_table[newframe].vpage = vpage;
			//frame_table[newframe].ref_count = 0;
			pte->PRESENT = 1;
			if (outputO)printf(" MAP %d\n", newframe);
			total_costs += 350;
			current_process->maps++;
		}
		// now the page is definitely present // check write protection
		if (current_process->page_table[vpage].WRITE_PROTECT == 1 && operation[0] == 'w')
		{
			printf(" SEGPROT\n");
			total_costs += 410;
			current_process->segprot++;
			pte->REFERENCED = 1;
		}else if (operation[0] == 'r')
		{
			pte->REFERENCED = 1;
		}
		else if(operation[0] == 'w')
		{
			pte->REFERENCED = 1;
			pte->MODIFIED = 1;
		}
	}

	inputfile.close();


	if (outputP)
	{
		for (int i = 0; i < number_processes; i++)
		{
			printf("PT[%d]:",process_table[i]->pid); //page table of a process
			for (int j = 0; j < MAX_VPAGES; j++)
			{
				if (!process_table[i]->page_table[j].PRESENT && process_table[i]->page_table[j].PAGEDOUT) //referenced but not modified
				{
					printf(" #");
				}
				else if (!process_table[i]->page_table[j].PRESENT && !process_table[i]->page_table[j].PAGEDOUT)
				{
					printf(" *");
				}
				else
				{
					printf(" %d:", j);
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
		printf("FT:");
		for (int i = 0; i < frame_numbers; i++)
		{
			if (frame_table[i].process != -1)
			{
				printf(" %d:%d", frame_table[i].process, frame_table[i].vpage);
			}
			else
			{
				printf(" *");
			}
		}
		printf("\n");
	}
	if (outputS) {
		for (int i = 0; i < number_processes; i++)
		{
			Process* proc = process_table[i];
			printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
				proc->pid,
				proc->unmaps, proc->maps, proc->ins, proc->outs,
				proc->fins, proc->fouts, proc->zeros,
				proc->segv, proc->segprot);
		}
		printf("TOTALCOST %lu %lu %lu %llu %lu\n", instruction_count, ctx_switches, process_exits, total_costs, sizeof(PTE_t));
	}
	return 0;
}