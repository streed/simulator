#include "program.h"
#include "shell.h"
#include "stdio.h"

int main( int argc, char **argv )
{
	if( argc != 2 )
	{
		printf( "Useage: sim <program to run>\n" );
	}
	else
	{
		printf( "Running %s\n", argv[1] );
		Program *p = new Program( argv[1], 1024 );
		Shell s( p );
		s.run();
	}

	return 0;
}
