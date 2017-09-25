#include "Archiver.h"
#include "Compiler.h"
#include <iostream>
#include <fstream>
#include <string>

void displayHelp(const char* _argv0)
{
	std::cout << "usage: " << _argv0 << " [hviof] FILE..."
		<< std::endl
		<< "  -h         "
		<< "print this help and exit"
		<< std::endl
		<< "  -v         "
		<< "print version and exit"
		<< std::endl
		<< "  -i FILE    "
		<< "read input files from this file"
		<< std::endl
		<< "  -o FILE    "
		<< "use this filename for output (.cpp/.h)"
		<< ", default is \"Resources\""
		<< std::endl;
}

int main( int argc, char* argv[] )
{
	Archiver archive;
	Compiler c;
	
	try
	{
		for( const char *binary = *argv; --argc; )
			if( **(++argv) != '-' )
				c.Add( *argv );
			else
				switch( *(*argv+1) )
				{
					default:
						std::cerr << "unknown option '"
							<< *(*argv+1)
							<< "'"
							<< std::endl;
					//	CompilerGUI();
						break;
					case 'h':
					case '?':
						std::cout << "usage: " << binary << " [hviof] FILE..."
							<< std::endl
							<< "  -h         "
							<< "print this help and exit"
							<< std::endl
							<< "  -v         "
							<< "print version and exit"
							<< std::endl
							<< "  -i FILE    "
							<< "read input files from this file"
							<< std::endl
							<< "  -o FILE    "
							<< "use this filename for output (.cpp/.h)"
								<< ", default is \"Resources\""
							<< std::endl
							<< "  -f FORMAT  "
							<< "set output format: STL (default) or wxWidgets"
							<< std::endl;
						return 0;
					case 'v':
						std::cout << "C++ Resource Compiler 0.4.3 "
							<< "(c) 2006 Markus Fisch <mf@markusfisch.de>"
							<< std::endl;
						return 0;
					case 'i':
						if( !--argc )
							throw "missing argument for -i option !";
						else
						{
							std::ifstream in( *++argv );

							if( !in ||
								!in.good() )
								throw "can't open input file !";

							for(std::string s; std::getline( in, s ); )
							{
								// don't use lines that start by #
								std::string::size_type p =
									s.find_first_not_of( " \t" );

								if( p == std::string::npos ||
									s[p] == '#' )
									continue;

								c.Add( s.c_str() );
							}

							in.close();
						}
						break;
					case 'o':
						if( !--argc )
							throw "missing argument for -o option !";
						c.SetFileName( *++argv );
						break;
				}
	}
	catch( const char *e )
	{
		std::cout << "C++ Resource Compiler 0.4.3 "
			<< "(c) 2006 Markus Fisch <mf@markusfisch.de>"
			<< std::endl;
		std::cerr << "error: " << e << std::endl;
	}
	displayHelp(argv[0]);
	return 0;
}