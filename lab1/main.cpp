#include<iostream>
#include<fstream>
#include<string>
#include<cstring> 
#include <vector>

#define TOKENMAX 100    //2^30 would be 10 char. if any symbol or number is longer then this TOKENMAX, it will cause error
#define LINEMAX 4096    //cannot handle if there are too many spaces in a line
#define SYMBOLMAX 16
#define SYMBOLSMAX 256
#define WORDSMAX 512   //number of instruction 
#define DEFIUSEMAX 16

#include <map>

using namespace std;

class symbol
{
public:
	symbol();
	~symbol();
	char name[SYMBOLMAX + 1];
	int absadd;
private:

};

symbol::symbol()
{
	memset(name, 0, sizeof(name));
	absadd = 0;
}

symbol::~symbol()
{
}

class tokeninfo
{
public:
	tokeninfo();
	tokeninfo(const tokeninfo& tok);
	~tokeninfo();
	char token[TOKENMAX];
	int tokenlength; //should <=16?
	int linenum;   //irl
	int lineoffset; //irl
	char linebuffer[LINEMAX + 1];  //one line max how much????
	bool eof;
private:

};

tokeninfo::tokeninfo()
{
	memset(this->token, 0, sizeof(token));
	memset(this->linebuffer, 0, sizeof(linebuffer));
	this->tokenlength = 0;
	this->linenum = 0;
	this->lineoffset = 1;
	this->eof = false;
}

tokeninfo::tokeninfo(const tokeninfo& tok) //deep copy
{
	strcpy(this->token, tok.token);
	this->tokenlength = tok.tokenlength;
	this->linenum = tok.linenum;
	this->lineoffset = tok.lineoffset;
	strcpy(this->linebuffer, tok.linebuffer);
	this->eof = tok.eof;
}
tokeninfo::~tokeninfo()
{
}


void __parseerror(int errcode, int linenum, int lineoffset) {
	const char* errstr[] = {
	"NUM_EXPECTED", // Number expect, anything >= 2^30 is not a number either
	"SYM_EXPECTED", // Symbol Expected
	"MARIE_EXPECTED", // Addressing Expected which is M/A/R/I/E
	"SYM_TOO_LONG", // Symbol Name is too long
	"TOO_MANY_DEF_IN_MODULE", // > 16
	"TOO_MANY_USE_IN_MODULE", // > 16
	"TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
	};
	printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
	exit(1);
}

tokeninfo* getToken(ifstream& fileobj, tokeninfo* buffer) { //the offset is always point to the end of a token  
	//static char temp[20];
	string temp;
	memset(buffer->token, 0, sizeof(buffer->token));
	buffer->tokenlength = 0;
	while (true) {

		if (buffer->linebuffer[buffer->lineoffset - 1] == '\0')  //The file begin with double \n???? 
		{
			getline(fileobj, temp);
			if (fileobj.fail())
			{
				buffer->lineoffset--;
				buffer->eof = true;
				return buffer;
			}
			if (temp == "" || !fileobj.eof()) temp += '\n';
			buffer->linenum++;
			buffer->lineoffset = 1;
			strcpy(buffer->linebuffer, temp.c_str());
		}
		else if (buffer->linebuffer[buffer->lineoffset - 1] == '\n')
		{
			buffer->lineoffset++;
		}
		else if (buffer->linebuffer[buffer->lineoffset - 1] == ' ' || buffer->linebuffer[buffer->lineoffset - 1] == '\t')
		{
			buffer->lineoffset++; //指向数组地址下一个开始的地方
		}
		else  //encounter token
		{
			break;
		}
	}
	while (!(buffer->linebuffer[buffer->lineoffset - 1] == '\n' || buffer->linebuffer[buffer->lineoffset - 1] == ' ' || buffer->linebuffer[buffer->lineoffset - 1] == '\t' || buffer->linebuffer[buffer->lineoffset - 1] == '\0'))
	{
		if (buffer->tokenlength > 16)
		{
			//errormessage 
		}
		buffer->token[buffer->tokenlength] = buffer->linebuffer[buffer->lineoffset - 1];
		buffer->tokenlength++;
		buffer->lineoffset++;
	}
	return buffer;
}
/*
int tokenizemain(int argc, char* argv[]) {
	ifstream file;
	if (argc <= 1)
	{
		cout << "A input file is needed";
		return 0;
	}
	file.open(argv[1]);

	file.open("./input-1");
	if (!file.is_open())
	{
		cout << "no file";
		return 0;
	}
	tokeninfo* tok = new tokeninfo();

	int linenum = 1;
	int lineoff = 1;
	while ((tok = getToken(file, tok)) != NULL)
	{
		linenum = tok->linenum;
		lineoff = tok->lineoffset;
		printf("Token: %d:%d : %s\n", tok->linenum, tok->lineoffset - tok->tokenlength, tok->token);
	}
	printf("EOF position % d: % d\n", linenum, lineoff);
	return 0;
}
*/
int readInt(tokeninfo* tok)
{
	int count = 0;
	int sum = 0;
	if (tok->eof == true)throw"NUMBEREXPECTED";
	while (tok->token[count] != '\0')
	{
		if (!(tok->token[count] <= '9' && tok->token[count] >= '0'))
		{
			throw "NUMBEREXPECTED";
			//return -1;
		}
		if (sum > 214748364 || (sum == 214748364 && int(tok->token[count] - '0') > 7)) {
			throw "NUMBEREXPECTED";
		}
		sum = sum * 10 + int(tok->token[count] - '0');
		count++;
	}
	return sum;
}

char* readSymbol(tokeninfo* tok)
{
	int count = 0;
	if (tok->eof == true)throw"SYMBOLEXPECTED";
	char* tokencopy = new char[SYMBOLMAX + 1];
	while (tok->token[count] != '\0') {
		if (count == 0)
		{
			if (!((tok->token[count] >= 'a' && tok->token[count] <= 'z') || (tok->token[count] >= 'A' && tok->token[count] <= 'Z')))
			{
				throw "SYMBOLEXPECTED";
				//return NULL;
			}
		}
		else
		{
			if (!((tok->token[count] >= 'a' && tok->token[count] <= 'z') || (tok->token[count] >= 'A' && tok->token[count] <= 'Z') || (tok->token[count] >= '0' && tok->token[count] <= '9')))
			{
				throw "SYMBOLEXPECTED";
				//return NULL;
			}
		}
		if (count >= SYMBOLMAX)
		{
			throw "SYMBOLTOOLONG";
			//return NULL;
		}
		count++;
	}
	strcpy(tokencopy, tok->token);
	return tokencopy;
}

char* readMARIE(tokeninfo* tok)
{
	char* tokencopy = new char[SYMBOLMAX];   //used for indicating mode, but with 17 size.
	if (tok->eof == true)throw"MARIEEXPECTED";
	strcpy(tokencopy, tok->token); //strcpy stop at \0
	if (tok->token[1] == '\0')
	{
		switch (tok->token[0])
		{
		case 'M':
			return tokencopy;
			break;
		case 'A':
			return tokencopy;
			break;
		case 'R':
			return tokencopy;
			break;
		case 'I':
			return tokencopy;
			break;
		case 'E':
			return tokencopy;
			break;
		}
	}
	throw "MARIEEXPECTED";
	//return NULL;
}

int SymbolExistanceInTable(vector<symbol*>& symbol_table, int left, int right, char* name)  //closed range
{
	if (left > right)
	{
		return -1;
	}
	for (int i = left; i <= right; i++) {
		if (!strcmp(symbol_table[i]->name, name))
		{
			return i;
		}
	}
	return -1;
}


int NumberofModules(int* module_table)
{
	int sizeofmoduletable = 0;
	for (int i = 0; i < WORDSMAX; i++)
	{
		if (module_table[i] == -1)
		{
			break;
		}
		sizeofmoduletable++;
	}
	return sizeofmoduletable - 1;//since module table always store the start of next module
}

void Pass1(ifstream& file, int* module_table, vector<symbol*>& symbol_table) {
	tokeninfo* tok = new tokeninfo();
	int linenum = 1;
	int lineoff = 1;
	int modulecount = 0;

	while (true)  //one loop for one module.
	{
		try
		{
			//Definition list
			//single token process begin
			tok = getToken(file, tok);    //This is token
			if (tok->eof == true)break;
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			int defcount = readInt(tok);  // This is not a token. Storing a single attribute is allowed.
			if (defcount > DEFIUSEMAX)
			{
				throw "DEF";
			}
			//single token process end
			for (int i = 0; i < defcount; i++) {

				//single token process begin
				tok = getToken(file, tok);
				char* sym = readSymbol(tok);
				linenum = tok->linenum;
				lineoff = tok->lineoffset - tok->tokenlength;
				//single token process end

				//single token process begin
				tok = getToken(file, tok);
				int val = readInt(tok);
				linenum = tok->linenum;
				lineoff = tok->lineoffset - tok->tokenlength;
				//run some check
				if (true) {//condition to be added
					symbol* temp = new symbol();
					strcpy(temp->name, sym);
					temp->absadd = val + module_table[modulecount];
					symbol_table.push_back(temp);                             //this would change in pass2
				}
				//single token process end
			}

			//Use list
			//single token process begin
			tok = getToken(file, tok);
			int usecount = readInt(tok);
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			if (usecount > DEFIUSEMAX)
			{
				throw "USE";
			}
			//single token process end
			for (int i = 0; i < usecount; i++) {
				//single token process begin
				tok = getToken(file, tok);
				char* sym = readSymbol(tok);
				linenum = tok->linenum;
				lineoff = tok->lineoffset - tok->tokenlength;
				//we don’t do anything here this would change in pass2 
				//single token process end
			}

			//Instruction list
			//single token process begin
			tok = getToken(file, tok);
			int instcount = readInt(tok);
			if (module_table[modulecount] + instcount > WORDSMAX)
			{
				throw "WORDS";
			}
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			module_table[modulecount + 1] = module_table[modulecount] + instcount;  //will always store the base of "next" module.
			//single token process end

			for (int i = 0; i < instcount; i++) {
				//single token process begin
				tok = getToken(file, tok);
				char* addressmode = readMARIE(tok);
				linenum = tok->linenum;
				lineoff = tok->lineoffset - tok->tokenlength;
				//single token process end

				//single token process begin
				tok = getToken(file, tok);
				int operand = readInt(tok);
				linenum = tok->linenum;
				lineoff = tok->lineoffset - tok->tokenlength;
				//single token process end
			}

			modulecount++;   //now point to next module
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			for (int i = 0; i < defcount; i++)
			{
				int j = symbol_table.size() - defcount + i; //absolute location of symbol in symboltable
				if (symbol_table[j]->absadd >= module_table[modulecount] && (SymbolExistanceInTable(symbol_table, 0, j - 1, symbol_table[j]->name) == -1))  //rule 5 exceed size
				{
					printf("Warning: Module %d: %s=%d valid=[0..%d] assume zero relative\n", modulecount - 1, symbol_table[j]->name, symbol_table[j]->absadd - module_table[modulecount - 1], module_table[modulecount] - module_table[modulecount - 1] - 1);//第三项改相对地址
					symbol_table[j]->absadd = module_table[modulecount - 1]; //Now the absolute location of them point to the modulebase (relative 0).   This modifies the symbol table in pass1 during error checking process.
				}
				if (SymbolExistanceInTable(symbol_table, 0, j - 1, symbol_table[j]->name) != -1)  //rule 5 redefinition
				{
					printf("Warning: Module %d: %s redefinition ignored\n", modulecount - 1, symbol_table[j]->name);
				}
			}
		}
		catch (const char* e)
		{
			if (tok->eof == false)
			{
				linenum = tok->linenum;
				lineoff = tok->lineoffset - tok->tokenlength;
			}
			else
			{
				linenum = tok->linenum;
				lineoff = tok->lineoffset;
			}
			if (!strcmp(e, "NUMBEREXPECTED"))
			{
				__parseerror(0, linenum, lineoff);
			}
			if (!strcmp(e, "SYMBOLEXPECTED"))
			{
				__parseerror(1, linenum, lineoff);
			}
			if (!strcmp(e, "SYMBOLTOOLONG"))
			{
				__parseerror(3, linenum, lineoff);
			}
			if (!strcmp(e, "MARIEEXPECTED"))
			{
				__parseerror(2, linenum, lineoff);
			}
			if (!strcmp(e, "DEF"))
			{
				__parseerror(4, linenum, lineoff);
			}
			if (!strcmp(e, "USE"))
			{
				__parseerror(5, linenum, lineoff);
			}
			if (!strcmp(e, "WORDS"))
			{
				__parseerror(6, linenum, lineoff);
			}
		}

	}

	cout << "Symbol Table" << endl;
	for (int i = 0; i < symbol_table.size(); i++)
	{
		bool rule2f = false;   //not print when true, since only the first definition of one should be print
		bool rule2l = false;   //print rule 2 error messsage
		if (SymbolExistanceInTable(symbol_table, 0, i - 1, symbol_table[i]->name) != -1)//there is a former definition of symbol[i], whose duplicate is symbol[j]
		{
			rule2f = true;
		}
		if (!rule2f)
		{
			cout << symbol_table[i]->name << "=" << symbol_table[i]->absadd;
			if (SymbolExistanceInTable(symbol_table, i + 1, symbol_table.size() - 1, symbol_table[i]->name) != -1)//there is a latter definition of symbol[i], whose duplicate is symbol[j]
			{
				rule2l = true;
			}
			if (rule2l)
			{
				cout << " Error: This variable is multiple times defined; first value used";
			}
			cout << endl; //在有输出时才换行
		}
		//
	}
	cout << endl;
}

void Pass2(ifstream& file, int* module_table, vector<symbol*>& symbol_table) {
	tokeninfo* tok = new tokeninfo();
	int linenum = 1;
	int lineoff = 1;
	int modulecount = 0;
	vector<bool> symboltableused;
	vector<symbol*> uselisttable;  //abs store module number
	vector<bool> uselistused;  //第一次用，不知道会怎样
	int deflistbase_table[WORDSMAX];
	for (int i = 0; i < WORDSMAX; i++)
	{
		deflistbase_table[i] = -1;
	}
	deflistbase_table[0] = 0;
	for (int i = 0; i < symbol_table.size(); i++)
	{
		symboltableused.push_back(false);
	}
	cout << "Memory Map" << endl;
	while (true)  //one loop for one module.
	{
		//try block removed
		//Definition list
		//single token process begin
		tok = getToken(file, tok);    //This is token
		if (tok->eof == true)break;
		linenum = tok->linenum;
		lineoff = tok->lineoffset - tok->tokenlength;
		int defcount = readInt(tok);  // This is not a token. Storing a single attribute is allowed.
		deflistbase_table[modulecount + 1] = deflistbase_table[modulecount] + defcount; //same size as module_table
		//single token process end
		for (int i = 0; i < defcount; i++) {

			//single token process begin
			tok = getToken(file, tok);
			char* sym = readSymbol(tok);
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			//single token process end

			//single token process begin
			tok = getToken(file, tok);
			int val = readInt(tok);
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			//single token process end
		}

		//Use list
		//vector<char*> uselist;

		//single token process begin
		tok = getToken(file, tok);
		int usecount = readInt(tok);
		linenum = tok->linenum;
		lineoff = tok->lineoffset - tok->tokenlength;
		//modulebaseinusetable_table[modulecount + 1] = modulebaseinusetable_table[modulecount] + usecount;
		//single token process end
		for (int i = 0; i < usecount; i++) {
			//single token process begin
			tok = getToken(file, tok);
			char* sym = readSymbol(tok);
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			symbol* temp = new symbol();
			strcpy(temp->name, sym);
			temp->absadd = modulecount;  //store the that which module is the usesymbol from
			uselisttable.push_back(temp);
			uselistused.push_back(false);

			//single token process end
		}

		//Instruction list
		//single token process begin
		tok = getToken(file, tok);
		int instcount = readInt(tok);
		linenum = tok->linenum;
		lineoff = tok->lineoffset - tok->tokenlength;
		//module_table[modulecount + 1] = module_table[modulecount] + instcount;  //will always store the base of "next" module.
		//single token process end

		for (int i = 0; i < instcount; i++) {
			//single token process begin
			tok = getToken(file, tok);
			char* addressmode = readMARIE(tok);
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			//single token process end

			//single token process begin
			tok = getToken(file, tok);
			int op = readInt(tok);
			//bool op9999 = false;
			if (op >= 10000) //rule 11
			{
				printf("%03d: %04d Error: Illegal opcode; treated as 9999\n", module_table[modulecount] + i, 9999);
				//op = 9999;
				//op9999 = true;
			}
			else {
				int opcode = op / 1000;
				int operand = op % 1000;
				switch (addressmode[0])
				{
				case 'M':
					if (true)
					{
						int sizeofmoduletable = NumberofModules(module_table);
						if (operand >= sizeofmoduletable)//rule 12
						{
							printf("%03d: %04d Error: Illegal module operand ; treated as module=0\n", module_table[modulecount] + i, 1000 * opcode); //assume the 0-th module is starting at address 0
							break;
						}
						printf("%03d: %04d\n", module_table[modulecount] + i, 1000 * opcode + module_table[operand]);
					}
					break;
				case 'A':
					if (true)
					{//machine size 512????
						if (operand >= WORDSMAX) //rule 8
						{
							printf("%03d: %04d Error: Absolute address exceeds machine size; zero used\n", module_table[modulecount] + i, 1000 * opcode);
							break;
						}
						printf("%03d: %04d\n", module_table[modulecount] + i, 1000 * opcode + operand);
					}
					break;
				case 'R':
					if (true)
					{
						if (operand >= instcount) //rule 9
						{
							printf("%03d: %04d Error: Relative address exceeds module size; relative zero used\n", module_table[modulecount] + i, 1000 * opcode + module_table[modulecount]);
							break;
						}
						printf("%03d: %04d\n", module_table[modulecount] + i, 1000 * opcode + module_table[modulecount] + operand);
					}
					break;
				case 'I':
					if (true)
					{//less than 900????
						if (operand >= 900)
						{
							printf("%03d: %04d Error: Illegal immediate operand; treated as 999\n", module_table[modulecount] + i, 1000 * opcode + 999);
							break;
						}
						printf("%03d: %04d\n", module_table[modulecount] + i, 1000 * opcode + operand);
					}
					break;
				case 'E':
					if (true)
					{
						if (operand + 1 > usecount)//rule 6
						{
							printf("%03d: %04d Error: External operand exceeds length of uselist; treated as relative=0\n", module_table[modulecount] + i, 1000 * opcode + module_table[modulecount]);//措辞不是很懂建议再看看
							break;
						}
						//symbol = uselisttable[k] 
						int j = uselisttable.size() - usecount + operand; //absolute location of symbol in uselisttable
						uselistused[j] = true;
						int firstposition = SymbolExistanceInTable(symbol_table, 0, symbol_table.size() - 1, uselisttable[j]->name);//the position in symbol_table is found from left to right, so the return position would be the position of first one
						if (firstposition == -1)//rule 3
						{
							printf("%03d: %04d Error: %s is not defined; zero used\n", module_table[modulecount] + i, 1000 * opcode, uselisttable[j]->name);
							break;
						}
						symboltableused[firstposition] = true;
						printf("%03d: %04d\n", module_table[modulecount] + i, 1000 * opcode + symbol_table[firstposition]->absadd);

					}
					break;
				}
			}
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			//single token process end
		}

		for (int i = 0; i < usecount; i++)//rule 7
		{
			int j = uselisttable.size() - usecount + i; //absolute location of symbol in uselisttable
			if (uselistused[j] == false)
			{
				printf("Warning: Module %d: uselist[%d]=%s was not used\n", modulecount, i, uselisttable[j]);
			}
		}
		modulecount++;   //now point to next module
		linenum = tok->linenum;
		lineoff = tok->lineoffset - tok->tokenlength;
	}
	cout << endl;

	int whichmodule = 0;
	int numberofmodules = NumberofModules(module_table);
	for (int i = 0; i < symbol_table.size(); i++) //rule 4 怎么判断i这个数是来自哪个模块 i号symbol的abs落在modulebase里？
	{
		if (symboltableused[i] == false && SymbolExistanceInTable(symbol_table, 0, i - 1, symbol_table[i]->name) == -1)//symbol table 和其他table 的元素的唯一性？？
		{
			for (int j = 0; j < numberofmodules; j++)
			{
				if (i < deflistbase_table[j + 1])//symbol_table[i]->absadd < module_table[j + 1])
				{
					whichmodule = j;
					break;
				}
			}
			printf("Warning: Module %d: %s was defined but never used\n", whichmodule, symbol_table[i]->name);
		}
	}
}


int main(int argc, char* argv[]) {
	ifstream file;
	if (argc <= 1)
	{
		cout << "A input file is needed";
		return 0;
	}
	string f = argv[1];
	
	file.open(f);
	if (!file.is_open())
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