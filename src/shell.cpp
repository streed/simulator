#include <iostream>
using std::cin;
using std::cout;

#include <stdio.h>

#include "shell.h"

Shell::Shell()
{}

Shell::Shell( Program *program )
{
	this->program = program;	
}

void Shell::run()
{
	running = true;
	while( running )
	{
		cout << "> ";
		cin.getline( currentLine, LINE_LENGTH );

		executeCurrentLine();
	}
}

void Shell::setCurrentLine( const char *s )
{
	for( int i = 0; i < LINE_LENGTH && s[i] != '\0'; i++ )
		currentLine[i] = s[i];
}

void Shell::parseLine()
{
	int i = 0;
	int end = i;
	switch( currentLine[i] )
	{
		case 'm':
			if( currentLine[++i] == 'e' && currentLine[++i] == 'm' )
			{
				unsigned int a[2];
				if( currentLine[++i] == ' ' )
				{
					for( int j = 0; j < 2; j++ )
					{
						a[j] = 0;
						for( ++i; currentLine[i] != '\0'; i++ )
						{	
							if( currentLine[i] == '\n' || currentLine[i] == '\0' || currentLine[i] == ' ' )
								break;
							else
							{
								a[j] *= 10;
								a[j] += currentLine[i] - '0';
							}	
						}
					}
					program->printMemory( a[0], a[1] );
				}
				else
				{
					cout << "mem start end\n";
				}
			}
			break;
		case 's':
			if( currentLine[++i] == 't' && currentLine[++i] == 'e' &&
					currentLine[++i] == 'p' )
			{
				unsigned numSteps = 0;
				if( currentLine[++i] == ' ' )
				{
					for( int j = ++i; currentLine[j] != '\0'; j++ )
					{
						if( currentLine[j] == '\n' || currentLine[j] == '\0' || currentLine[j] == ' ' )
							break;
						else
						{
							numSteps *= 10;
							numSteps += currentLine[j] - '0';
						}
					}		
				}
				if( numSteps == 0 )
					numSteps = 1;
				for( unsigned int i = 0; i < numSteps; i++ )
					program->step();
			}
			break;
		case 'c':
			if( currentLine[++i] == 'p' && currentLine[++i] == 'u' )
				program->printCPU();
			break;
		case 'i':
			if( currentLine[++i] == 'n' && currentLine[++i] == 's' &&
					currentLine[++i] == 't' && currentLine[++i] == 'r' )
			{
				unsigned int mbr = program->getMBR();
				printf( "mbr = %08x\n", mbr );
			}
			break;
		case 'r':
			if( currentLine[i + 1] == 'u' && currentLine[i + 2] == 'n' )
			{
				cout << "Running the program\n";
				bool run = program->step();
				while( run )
					run = program->step();
				cout << "Done running the program\n";
			}
			else if( currentLine[i + 1] == 'e' && currentLine[i + 2] == 's' )
			{
				cout << "Restarting the program.\n";
				program->restart();
			}
			break;
		case 'e':
			if( currentLine[++i] == 'x' && currentLine[++i] == 'i' &&
					currentLine[++i] == 't' )
				running = false;
			break;
		default:
			cout << "Unknown command `" << currentLine << "`\n";
	}
}

bool Shell::executeCurrentLine()
{
	parseLine();
}

int Shell::getInt( const char *s )
{
	bool neg = false;
	int ret = 0;

	if( s[0] == '-' )
		neg = true;

	int i = 0;
	if( neg )
		i = 1;

	for( ; s[i] != '\0'; i++ )
	{
		ret *= 10;
		ret += s[i] - '0';
	}

	if( neg )
		ret *= -1;

	return ret;
}


