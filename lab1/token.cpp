#include<iostream>
#include<fstream>
#include<cstring>
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

char* getToken(fstream& fileobj, char* buffer) {
  static char temp[20];
  static int linenum = 1;
  static int lineoffset
  memset(temp, 0, sizeof(temp));


  return temp;
}

int main(int argc, char* argv[]) {
  fstream file;
  if (argc <= 1)
  {
    cout << "A input file is needed";
    return 0;
  }
  file.open(argv[1]);
  char tok[20];
  int linenum = 1;
  int lineoffset = 1;
  while ((tok = getToken(file)) != NULL)
  {
    printf("token = < % s>  position = % d: % d\n", tok, linenum, lineoffset);
  }
  printf(“EOF position % d: % d\n”, linenum, lineoff);
  cout << "hello";

}