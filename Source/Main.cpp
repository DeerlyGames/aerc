#include "Aerc.h"
#include <iostream>

void displayHelp(const char* _argv0)
{
	static bool showOnce = true;
	if(showOnce){
		std::cout << "Usage: " << _argv0 << " [input]"
			<< std::endl
			<< "  [input]  "
			<< "input file, usually something.aerc."
			<< std::endl
			<< "  -watch  "
			<< "specify if you want the resource compiler to run in loop. (optional)"
			<< std::endl;
		showOnce = false;
		}
}

int main( int argc, char* argv[] )
{
	int i = 0;
	if(argc==1) displayHelp(argv[0]);
	if (++i != argc)
		return !FromFile(argv[i]);
	return 0;
}