#include<iostream>
#include<fstream>
#include<string>
#include<cstring> 
#include <vector>
#pragma warning(disable : 4996)

using namespace std;

#define TOKENMAX 30
#define LINEMAX 4000
#define SYMBOLMAX 16
#define SYMBOLSMAX 256
#define WORDSMAX 512   //number of instruction 
#include <map>

class symbol
{
public:
	symbol();
	~symbol();
	char name[SYMBOLMAX];
	int relativeadd;
private:

};

symbol::symbol()
{
	memset(name, 0, sizeof(name));
	relativeadd = 0;
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

void __parseerror(int errcode) {
	static char* errstr[] = {
	"NUM_EXPECTED", // Number expect, anything >= 2^30 is not a number either
	"SYM_EXPECTED", // Symbol Expected
	"MARIE_EXPECTED", // Addressing Expected which is M/A/R/I/E
	"SYM_TOO_LONG", // Symbol Name is too long
	"TOO_MANY_DEF_IN_MODULE", // > 16
	"TOO_MANY_USE_IN_MODULE", // > 16
	"TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
	};
	//printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
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
	while (tok->token[count] != '\0')
	{
		if (!(tok->token[count] <= '9' && tok->token[count] >= '0'))
		{
			//rule 1 syntax error
			throw "SYNTAXERROR";
			//return -1;
		}
		sum = sum * 10 + int(tok->token[count] - '0');
		count++;
	}
	return sum;
}

char* readSymbol(tokeninfo* tok)
{
	int count = 0;
	char* tokencopy = new char[SYMBOLMAX];
	while (tok->token[count] != '\0') {
		if (count == 0)
		{
			if (!((tok->token[count] >= 'a' && tok->token[count] <= 'z') || (tok->token[count] >= 'A' && tok->token[count] <= 'Z')))
			{
				//rule 1 syntax error
				throw "SYNTAXERROR";
				//return NULL;
			}
		}
		else
		{
			if (!((tok->token[count] >= 'a' && tok->token[count] <= 'z') || (tok->token[count] >= 'A' && tok->token[count] <= 'Z') || (tok->token[count] >= '0' && tok->token[count] <= '9')))
			{
				//rule 1 syntax error
				throw "SYNTAXERROR";
				//return NULL;
			}
		}
		if (count >= SYMBOLMAX)
		{
			//errormessage here
			return NULL;
		}
		count++;
	}
	strcpy(tokencopy, tok->token);
	return tokencopy;
}

char* readMARIE(tokeninfo* tok)
{
	char* tokencopy = new char[SYMBOLMAX];   //used for indicating mode, but with 17 size.
	if (tok == NULL)
	{
		//end of file encountered
		return NULL;
	}
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
	throw "SYNTAXERROR";
	//return NULL;
}

void Pass1(ifstream& file, int* module_table, vector<symbol*> symbol_table) {
	tokeninfo* tok = new tokeninfo();
	int linenum = 1;
	int lineoff = 1;
	int modulecount = 0;
	cout << "Symbol Table" << endl;
	while (true)  //one loop for one module.
	{
		try
		{


			//single token process begin
			tok = getToken(file, tok);    //This is token
			if (tok == NULL)break;
			int defcount = readInt(tok);  // This is not a token. Storing a single attribute is allowed.
			if (defcount < 0)
			{
				//syntax error
				exit(2);
			}
			//single token process end

			for (int i = 0; i < defcount; i++) {

				//single token process begin
				tok = getToken(file, tok);
				char* sym = readSymbol(tok);
				//run some check
				////single token process end

				//single token process begin
				tok = getToken(file, tok);
				int val = readInt(tok);
				//run some check
				if (true) {//condition to be added
					symbol* temp = new symbol();
					strcpy(temp->name, sym);
					temp->relativeadd = val + module_table[modulecount];
					symbol_table.push_back(temp);                             //this would change in pass2
					cout << temp->name << "=" << temp->relativeadd << endl;
				}
				//single token process end
			}
			//single token process begin
			tok = getToken(file, tok);
			int usecount = readInt(tok);

			for (int i = 0; i < usecount; i++) {
				//single token process begin
				tok = getToken(file, tok);
				char* sym = readSymbol(tok);
				//we don’t do anything here this would change in pass2 
				//single token process end
			}

			//single token process begin
			tok = getToken(file, tok);
			int instcount = readInt(tok);
			module_table[modulecount + 1] = module_table[modulecount] + instcount;  //will always store the base of "next" module.
			//single token process end

			for (int i = 0; i < instcount; i++) {
				//single token process begin
				tok = getToken(file, tok);
				char* addressmode = readMARIE(tok);
				//single token process end

				//single token process begin
				tok = getToken(file, tok);
				int operand = readInt(tok);
				//single token process end
			}

			modulecount++;
			linenum = tok->linenum;
			lineoff = tok->lineoffset;

		}
		catch (const char* e)
		{

		}

	}
	printf("EOF position % d: % d\n", linenum, lineoff);
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
	string f = "./input-1";
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
	file.close();
	file.open(f);
	Pass2();
	return 0;
}