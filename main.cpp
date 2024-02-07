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
#define DEFUSEMAX 16
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

void Pass1(ifstream& file, int* module_table, vector<symbol*> symbol_table) {
	tokeninfo* tok = new tokeninfo();
	int linenum = 1;
	int lineoff = 1;
	int modulecount = 0;
	
	while (true)  //one loop for one module.
	{
		try
		{
			//single token process begin
			tok = getToken(file, tok);    //This is token
			linenum = tok->linenum;
			lineoff = tok->lineoffset-tok->tokenlength;
			if (tok == NULL)break;
			int defcount = readInt(tok);  // This is not a token. Storing a single attribute is allowed.
			if (defcount >= DEFUSEMAX)
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
				for (int i = 0; i < symbol_table.size(); i++)//rule 2
				{
					if (!strcmp(symbol_table[i]->name, sym))
					{
						rule2 = true;
						break;
					}
				}
				if (rule2 == true)
				{

				}
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
			//single token process begin
			tok = getToken(file, tok);
			int usecount = readInt(tok);
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;
			if (usecount >= DEFUSEMAX)
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

			modulecount++;
			linenum = tok->linenum;
			lineoff = tok->lineoffset - tok->tokenlength;

		}
		catch (const char* e)
		{
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

}

void Pass2() {

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
	string f = "F:/Documents/lab1/syntaxerror";
	file.open(f);
	if (!file.is_open())
	{
		cout << "no file";
		return 0;
	}
	int module_table[WORDSMAX];
	//map<string, int> symbol_table;
	vector<symbol*> symbol_table;
	module_table[0] = 0;
	Pass1(file, module_table, symbol_table);
	cout << "Symbol Table" << endl;
	for (int i = 0; i < symbol_table.size(); i++)
	{
		cout << symbol_table[i]->name << "=" << symbol_table[i]->absadd << endl;
	}
	file.close();
	file.open(f);
	Pass2();
	return 0;
}