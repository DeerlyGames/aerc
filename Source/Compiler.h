#ifndef AECOMPILER_COMPILER_H
#define AECOMPILER_COMPILER_H

#include <string>
#include <sstream>

class Compiler
{
public:
	Compiler() :
		offset( 0 ),
		fileName( "Resource" ) {}
	
	virtual ~Compiler();

	virtual void Add( const char* _file );
	
	inline void SetFileName( const char *f ) { fileName = f; Flush(); }

	inline const std::string &GetFileName() const { return fileName; }
	
protected:
	std::ostringstream data;
	std::ostringstream index;
	int offset;

	virtual int Hash( const char * _s) const;
	virtual void Flush();

private:
	enum
	{
		HashMax = 600011
	};

	std::string fileName;
};

#endif // AECOMPILER_COMPILER_H