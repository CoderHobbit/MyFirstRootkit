// play sound

#include <iostream>
#include <windows.h>
#include <mmsystem.h>

using namespace std;

int main()
{
	cout << "starting" << endl;
	while(true)
		PlaySound("C:\\Users\\Sergey\\Documents\\MyFirstRootkit\\NyanCat.wav", NULL, SND_FILENAME);
	return 0;
}
