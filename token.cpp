#include<iostream>

using namespace std;

class tokeninfo
{
public:
	tokeninfo();
	~tokeninfo();
	char* token[20];
	int linecount;
	int offset;
private:

};

tokeninfo::tokeninfo()
{
	linecount = 0;
	offset = 0;
}

tokeninfo::~tokeninfo()
{
}

tokeninfo& getToken(fileobj,tokeninfo &buffer) {

}

int main(int argc, char* argv[]) {
	cout >> "hello";

}