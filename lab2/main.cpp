﻿#include <iostream>
#include <fstream>
//#include <unistd.h>
#include "getopt.h"
#include <string>

using namespace std;

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


	vector<symbol*> symbol_table;
	int module_table[WORDSMAX]; //seems to be 128? But just do it
	for (int i = 0; i < WORDSMAX; i++)
	{
		module_table[i] = -1;
	}
	module_table[0] = 0;
	Pass1(file, module_table, symbol_table);
	file.close();
	file.open(f);
	Pass2(file, module_table, symbol_table);
	file.close();
	return 0;
}