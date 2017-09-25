#ifndef AECOMPILER_ARCHIVER_H
#define AECOMPILER_ARCHIVER_H

#include <string>

class Archiver
{
public:
	Archiver() :
		fileName( "Resource" ) {}

	std::string fileName;
};

#endif // AECOMPILER_ARCHIVER_H