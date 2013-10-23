#include <iostream>
using std::cout;
#include "forkshell.h"

int ston( const char *str )
{
	int num = 0;
	for( int j = 0; str[j] != '\0'; j++ )
	{
		if( str[j] == '\n' || str[j] == '\0' || str[j] == ' ' )
			break;
		else
		{
			num *= 10;
			num += str[j] - '0';
		}
	}

	return num;
}

int main( int argc, char **argv )
{
	if( argc < 3 )
	{
		cout << "Usage: " << argv[0] << " <timeslice> <memorysize in words>\n";
	}
	else
	{
		int ts = ston( argv[1] );
		int ms = ston( argv[2] );
		ForkShell fs( ts, ms );
		fs.run();
	}
	return 0;
}
