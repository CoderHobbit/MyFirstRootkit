#include <iostream>
#include <sys/stat.h>
#include <windows.h>

using namespace std;

int main()
{
  struct stat buff;
  cout << "henlo worl" << endl;
  while(stat("C:\\Users\\Public\\Documents\\done.txt", &buff) != 0) // Secret backdoor killswitch >_>
  {
	Sleep(100);
    cout << "\a";
  }
  cout << "bai worl" << endl;
  return 0;
}