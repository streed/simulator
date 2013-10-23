#include "process.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

/*
 * I need to redirect cout to a internalized buffer temporarily so that I 
 * can then send the output from the program to a string buffer
 * and then send this string buffer back over the pipe to the shell process
 * otherwise this would require a lot of rewriting of code that I
 * frankly do not wish to do.
 */
#include <streambuf>
#include <sstream>

using std::cout;
using std::streambuf;//used to store the current cout buffer
using std::ostringstream;//Temporarily used to capture the output.

Process::Process(): Program()
{}

Process::~Process()
{}

void Process::wait_cmds( int c[], int p[] )
{
	child = c;
	parent = p;
	
	bool run = true;

	char input[MAX_LENGTH];
	while( run )
	{
		readMessage( input );

		if( strfind( input, EXIT ) != 0 && strfind( input, PRINT_MEMORY ) != 0
		    && strfind( input, STEP ) != 0 && strfind( input, MBR ) != 0
		    && strfind( input, RESTART ) != 0 && strfind( input, PRINT_CPU ) != 0 
		    && input[0] != '\0' )
		{
			reload( input );
		}
		else
		{
			streambuf *old = cout.rdbuf();
			ostringstream tempOut;
			cout.rdbuf( tempOut.rdbuf() );

			if( strfind( input, PRINT_CPU ) == 0 )
			{
				printCPU();
				writeMessage( tempOut.str().c_str() );
				tempOut.flush();
			}
			else if( strfind( input, PRINT_MEMORY ) == 0 )
			{
				//get first param
				char line[MAX_LENGTH];
				readMessage( line );
				unsigned int a = getInt( line );
				readMessage( line );
				unsigned int b = getInt( line );
				printMemory( a, b );
				writeMessage( tempOut.str().c_str() );
			}
			else if( strfind( input, MBR ) == 0 )
			{
				char line[MAX_LENGTH];
				sprintf( line, "mbr = 0x%08X\n", getMBR() );
				writeMessage( line );
			}
			else if( strfind( input, STEP ) == 0 )
			{
				running = step();

				if( running )
					writeMessage( RUNNING );
				else
					writeMessage( DONE );
			}
			else if( strfind( input, EXIT ) == 0 )
			{
				run = false;
			}

			tempOut.flush();
			cout.rdbuf( old );
			cout.flush();
		}
	}
}

void Process::reload( char *filename )
{
	restart();
	_read( filename );
}

void Process::in()
{
	writeMessage( INPUT );
	char input[MAX_LENGTH];
	readMessage( input );
	int value = getInt( input );
	regs[mbr.x] = value;
}

void Process::out()
{
	writeMessage( OUTPUT );
	char output[MAX_LENGTH];

	sprintf( output, "%d\n", regs[mbr.x] );
	writeMessage( output );
}

void Process::writeMessage( const char *msg )
{
	char r[1];
	for( int i = 0; msg[i] != '\0' && i < MAX_LENGTH; i++ )
	{
		r[0] = msg[i];
		write( parent[1], r, 1 );
	}
	r[0] = '\0';
	write( parent[1], r, 1 );
}

void Process::readMessage( char *msg )
{
	int i = 0;
	char ch;

	do
	{
		read( child[0], &ch, 1 );
		msg[i] = ch;
		i++;
	}while( ch != '\0' );

	msg[i] = '\0';
}
