#include<iostream>
#include<fstream>
#include<string>
#include<cstring> 
#include <vector>
#pragma warning(disable : 4996)

using namespace std;

#define TOKENMAX 100    //2^30 would be 10 char. if any symbol or number is longer then this TOKENMAX, it will cause error
#define LINEMAX 4000    //cannot handle if there are too many spaces in a line
#define SYMBOLMAX 16
#define SYMBOLSMAX 256
#define WORDSMAX 512   //number of instruction 
#define DEFIUSEMAX 16
#include <map>

class symbol
{
public:
	symbol();
	~symbol();
	char name[SYMBOLMAX];
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
	~tokeninfo();
	char token[TOKENMAX];
	int tokenlength; //should <=16?
	int linenum;   //irl
	int lineoffset; //irl
	char linebuffer[LINEMAX];  //one line max how much????
private:

};

tokeninfo::tokeninfo()
{
	memset(token, 0, sizeof(token));
	memset(linebuffer, 0, sizeof(linebuffer));
	tokenlength = 0;
	linenum = 0;
	lineoffset = 1;
}

tokeninfo::~tokeninfo()
{
}

void __parseerror(int errcode,int linenum, int lineoffset) {
	static char* errstr[] = {
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

tokeninfo* getToken(ifstream& fileobj, tokeninfo* buffer) {
	//static char temp[20];
	string temp;
	memset(buffer->token, 0, sizeof(buffer->token));
	buffer->tokenlength = 0;
	while (true) {
		if (buffer->linebuffer[buffer->lineoffset - 1] == '\0')  //The file begin with double \n????   \n becomes \0 since we use getline
		{
			buffer->linenum++;
			buffer->lineoffset = 1;
			if (fileobj.eof())
			{
				return NULL;
			}
			getline(fileobj, temp);
			strcpy(buffer->linebuffer, temp.c_str());
		}
		else if (buffer->linebuffer[buffer->lineoffset - 1] == ' ' || buffer->linebuffer[buffer->lineoffset - 1] == '\t')
		{
			buffer->lineoffset++;
		}
		else  //encounter token
		{
			break;
		}
	}
	while (!(buffer->linebuffer[buffer->lineoffset - 1] == '\0' || buffer->linebuffer[buffer->lineoffset - 1] == ' ' || buffer->linebuffer[buffer->lineoffset - 1] == '\t'))
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
	if (tok == NULL)throw"NUMBEREXPECTED";
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
	if (tok == NULL)throw"SYMBOLEXPECTED";
	char* tokencopy = new char[SYMBOLMAX];
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
	if (tok == NULL)throw"MARIEEXPECTED";
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

bool SymbolExistanceInTable(vector<symbol*> symbol_table, int left, int right, char* name)  //closed range
{
	for (int i = left; i <= right; i++) {
		if (!strcmp(symbol_table[i]->name, name))
		{
			return true;
		}
	}
	return false;
}

void Pass1(ifstream& file,int* module_table, vector<symbol*> symbol_table) {
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
			if (tok == NULL)break;
			linenum = tok->linenum;
			lineoff = tok->lineoffset-tok->tokenlength;
			int defcount = readInt(tok);  // This is not a token. Storing a single attribute is allowed.
			if (defcount >= DEFIUSEMAX)
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
			if (usecount >= DEFIUSEMAX)
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
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			if (usecount >= WORDSMAX)
			{
				throw "WORDS";
			}
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
				if (symbol_table[j]->absadd >= module_table[modulecount]&&(!SymbolExistanceInTable(symbol_table,0,j-1,symbol_table[j]->name)))  //rule 5 exceed size
				{
					printf("Warning: Module %d: %s=%d valid=[0..%d] assume zero relative\n", modulecount - 1, symbol_table[j]->name, symbol_table[j]->absadd- module_table[modulecount - 1], module_table[modulecount] - module_table[modulecount - 1] - 1);//第三项改相对地址
				}
				if (SymbolExistanceInTable(symbol_table, 0, j - 1, symbol_table[j]->name))  //rule 5 redefinition
				{
					printf("Warning: Module %d: %s redefinition ignored\n", modulecount - 1, symbol_table[j]->name);
				}
			}
		}
		catch (const char* e)
		{
			if (tok != NULL)
			{
				linenum = tok->linenum;
				lineoff = tok->lineoffset - tok->tokenlength;
			}
			if (!strcmp(e, "NUMBEREXPECTED"))
			{
				__parseerror(0, linenum,lineoff);
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
		if (SymbolExistanceInTable(symbol_table, 0, i - 1, symbol_table[i]->name))//there is a former definition of symbol[i], whose duplicate is symbol[j]
		{
			rule2f = true;
		}
		if (!rule2f)
		{
			cout << symbol_table[i]->name << "=" << symbol_table[i]->absadd;
			if (SymbolExistanceInTable(symbol_table, i + 1, symbol_table.size()-1, symbol_table[i]->name))//there is a latter definition of symbol[i], whose duplicate is symbol[j]
			{
				rule2l = true;
			}
			if (rule2l)
			{
				cout << " Error: This variable is multiple times defined; first value used" << endl;
			}
		}
	}
}

void Pass2(ifstream& file, int* module_table, vector<symbol*> symbol_table) {
	tokeninfo* tok = new tokeninfo();
	int linenum = 1;
	int lineoff = 1;
	int modulecount = 0;
	while (true)  //one loop for one module.  //exit using "if (tok == NULL)break;"
	{
		try
		{
			//Definition list
			//single token process begin
			tok = getToken(file, tok);    //This is token
			if (tok == NULL)break;
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			int defcount = readInt(tok);  // This is not a token. Storing a single attribute is allowed.
			if (defcount >= DEFIUSEMAX)
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
				bool rule2 = false;
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
			if (usecount >= DEFIUSEMAX)
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
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			if (usecount >= WORDSMAX)
			{
				throw "WORDS";
			}
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
				if (symbol_table[j]->absadd >= module_table[modulecount] && (!SymbolExistanceInTable(symbol_table, 0, j - 1, symbol_table[j]->name)))  //rule 5 exceed size
				{
					printf("Warning: Module %d: %s=%d valid=[0..%d] assume zero relative\n", modulecount - 1, symbol_table[j]->name, symbol_table[j]->absadd - module_table[modulecount - 1], module_table[modulecount] - module_table[modulecount - 1] - 1);//第三项改相对地址
				}
				if (SymbolExistanceInTable(symbol_table, 0, j - 1, symbol_table[j]->name))  //rule 5 redefinition
				{
					printf("Warning: Module %d: %s redefinition ignored\n", modulecount - 1, symbol_table[j]->name);
				}
			}
		}
		catch (const char* e)
		{
			if (tok != NULL)
			{
				linenum = tok->linenum;
				lineoff = tok->lineoffset - tok->tokenlength;
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
		if (SymbolExistanceInTable(symbol_table, 0, i - 1, symbol_table[i]->name))//there is a former definition of symbol[i], whose duplicate is symbol[j]
		{
			rule2f = true;
		}
		/*for (int j = i - 1; j > -1; j++)
		{
			if (!(strcmp(symbol_table[i]->name, symbol_table[j]->name)))
			{
				rule2f = true;
				break;
			}
		}*/
		if (!rule2f)
		{
			cout << symbol_table[i]->name << "=" << symbol_table[i]->absadd;
			if (SymbolExistanceInTable(symbol_table, i + 1, symbol_table.size() - 1, symbol_table[i]->name))//there is a latter definition of symbol[i], whose duplicate is symbol[j]
			{
				rule2l = true;
			}
			/*
			for (int j = i + 1; j < symbol_table.size(); j++)
			{
				if (!(strcmp(symbol_table[i]->name, symbol_table[j]->name)))
				{
					rule2l = true;
					break;
				}
			}*/
			if (rule2l)
			{
				cout << " Error: This variable is multiple times defined; first value used";
			}
			cout << endl;
		}
	}
}
int main(int argc, char* argv[]) {
	ifstream file;
	/*if (argc <= 1)
	{
		cout << "A input file is needed";
		return 0;
	}
	file.open(argv[1]);
	*/
	string f = "F:/美国学习资料/OS/lab1/input-11";
	file.open(f);
	if (!file.is_open())
	{
		cout << "no file";
		return 0;
	}
	vector<symbol*> symbol_table;
	int module_table[WORDSMAX]; 
	module_table[0] = 0;
	Pass1(file, module_table, symbol_table);
	file.close();
	file.open(f);
	Pass2(file,module_table, symbol_table);
	file.close();
	return 0;
}