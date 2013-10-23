#include "forkshell.h"
#include "processgroup.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using std::cout;
using std::cin;

#include <fcntl.h>

int strlen( const char *m )
{
	int ret = 0;

	while( m[ret] != '\0' )
		ret++;

	return ret;
}

int strfind( const char *stack, const char *needle )
{
	int start = -1;

	for( int i = 0; stack[i] != '\0' && start == -1; i++ )
	{
		if( stack[i] == needle[0] )
		{
			int temp = 0;
			for( int j = 1; needle[j] != '\0' && temp != -1; j++ )
			{
				if( needle[j] == stack[i + j] )
					temp = i;
				else
					temp = -1;
			}

			if( temp >= 0 )
				start = temp;
		}
	}

	return start;
}
int strtonum( const char *str )
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

ForkShell::ForkShell( int ts, int ms ): Shell()
{
	timeSlice = ts;
	memorySize = ms;
	foreground = false;
	if( pipe( parent ) == -1 || pipe2( child, O_NONBLOCK ) == -1 )
	{
		cout << "Could not create readPipe.\n";
	}
}

void ForkShell::run()
{
	cout.flush();
	int retPid = fork();

	if( retPid == 0 )
	{
		ProcessGroup *pg = new ProcessGroup( timeSlice, memorySize );
		pg->wait_commands( child, parent );
	}
	else
	{	
		running = true;
		while( running )
		{
			if( !foreground && !stepping )
			{
				cout << "> ";
				cin.getline( currentLine, LINE_LENGTH );	
				executeCurrentLine();
			}
			else
			{
				char output[MAX_LENGTH];
				readMessage( output );

				if( strcmp( output, INPUT ) == 0 )
				{
					cout << "% ";
					char line[MAX_LENGTH];
					cin.get( line, MAX_LENGTH );
					writeMessage( line );
					readMessage( output );
					if( strcmp( output, DONE ) == 0 )
						foreground = false;
				}
				else if( strcmp( output, OUTPUT ) == 0 )
				{
					char line[MAX_LENGTH];
					readMessage( line );
					cout << line;
					readMessage( output );
					if( strcmp( output, DONE ) == 0 )
						foreground = false;
				}
				else if( strcmp( output, DONE ) == 0 )
					foreground = false;
				else if( strcmp( output, "nostep" ) == 0 )
					stepping = false;

				if( stepping )
				{
					steppingSteps--;

					if( steppingSteps <= 0 )
						stepping = false;
				}
			}
		}
	}
}

bool ForkShell::executeCurrentLine()
{
	int i = 0;
	int end = i;
	cout.flush();
	if( strfind( currentLine, "mem" ) == 0 )
	{
		i += 3;
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

			writeMessage( PRINT_MEMORY );
			char line[MAX_LENGTH];
			sprintf( line, "%u", a[0] );
			writeMessage( line );
			sprintf( line, "%u", a[1] );
			writeMessage( line );
			char output[MAX_LENGTH];
			readMessage( output );
			cout << output << "\n";
		}
		else
		{
			cout << "mem start end\n";
		}
	}
	else if( strfind( currentLine, "step" ) == 0 )
	{
		writeMessage( STEP );
		i += 4;
		unsigned numSteps = 0;
		char steps[MAX_LENGTH];
		if( currentLine[++i] == ' ' )
		{
			int j = 0;
			for( ; currentLine[i] != '\0'; i++ )
			{
				if( currentLine[i] == '\n' || currentLine[i] == '\0' || currentLine[i] == ' ' )
					break;
				else
				{
					 steps[j++] = currentLine[i];
				}
			}		

			steps[j] = '\0';
			writeMessage( steps );

			char pid[MAX_LENGTH];
			if( currentLine[++i] == ' ' )
			{
				int j = 0;
				for( ; currentLine[i] != '\0'; i++ )
				{
					if( currentLine[i] == '\n' || currentLine[i] == '\0' || currentLine[i] == ' ' )
						break;
					else
					{
						pid[j++] = currentLine[i];
					}
				}		

				steps[j] = '\0';
				steppingSteps = strtonum( steps );
				writeMessage( steps );
			}
		}
		else
		{
			steppingSteps = 1;
			writeMessage( "N" );
			writeMessage( "N" );
		}

		char line[MAX_LENGTH];
		readMessage( line );

		if( strfind( line, STEP ) == 0 )
		{
			stepping = true;
		}
		else if( strfind( line, "notstep" ) == 0 )
		{
			stepping = false;
		}
	}
	else if( strfind( currentLine, "cpu" ) == 0 )
	{
		writeMessage( PRINT_CPU );
		char output[MAX_LENGTH];
		readMessage( output );
		cout << output;
	}
	else if( strfind( currentLine, "instr" ) == 0 )
	{
		writeMessage( MBR );
		char output[MAX_LENGTH];
		readMessage( output );
		cout << output;
	}
	else if( strfind( currentLine, "run" ) == 0 )
	{
		bool run = true;
		while( run )
		{
			writeMessage( STEP );
			char output[MAX_LENGTH];
			readMessage( output );

			if( strcmp( output, INPUT ) == 0 )
			{
				cout << "% ";
				char line[MAX_LENGTH];
				cin.get( line, MAX_LENGTH );
				writeMessage( line );
				readMessage( output );
				if( strcmp( output, DONE ) == 0 )
					run = false;
			}
			else if( strcmp( output, OUTPUT ) == 0 )
			{
				char line[MAX_LENGTH];
				readMessage( line );
				cout << line;
				readMessage( output );
				if( strcmp( output, DONE ) == 0 )
					run = false;
			}
			else if( strcmp( output, DONE ) == 0 )
				run = false;
		}
	}
	else if( strfind( currentLine, "restart" ) == 0 )
	{
		writeMessage( RESTART_SIM );
	}
	else if( strfind( currentLine, "exit" ) == 0 )
	{
		writeMessage( EXIT );
		wait();
		running = false;
	}
	else if( strfind( currentLine, "jobs" ) == 0 )
	{
		writeMessage( JOBS );
		char line[MAX_LENGTH];
		readMessage( line );
		cout << line;
	}
	else if( strfind( currentLine, "fg" ) == 0 )
	{
		i += 2;
		if( currentLine[++i] == ' ' )
		{
			int pid = strtonum( ((char *)(currentLine + i + 1 ) ) );
			if( pid >= 0 )
			{
				writeMessage( BRING_CURRENT_PROCESS_TO_FOREGROUND );
				writeMessage( ((char *)(currentLine + i + 1 ) ) );
			}
		}
		else
		{
			writeMessage( BRING_LAST_PROCESS_TO_FOREGROUND );
			foreground = true;
		}
	}
	else if( strfind( currentLine, "queue" ) == 0 )
	{
		writeMessage( QUEUE );
		char line[MAX_LENGTH];
		readMessage( line );
		cout << line;
	}
	else if( strfind( currentLine, "kill" ) == 0 )
	{
		writeMessage( KILL );
		i += 4;
		if( currentLine[i] == ' ' )
		{
			writeMessage( ((char *)(currentLine + i + 1) ) );
		}
		else
			writeMessage( "N" );
	}
	else if( strfind( currentLine, "suspend" ) == 0 )
	{
		writeMessage( SUSPEND_SIM );
	}
	else if( strfind( currentLine, FREEMEM ) == 0 )
	{
		writeMessage( FREEMEM );
		char line[MAX_LENGTH];
		readMessage( line );
		cout << line;
	}
	else
	{
		if( currentLine[0] != '\0' && currentLine[0] != '\r' && currentLine[0] != '\n' )
		{
			writeMessage( currentLine );
			char status[MAX_LENGTH];
			readMessage( status );

			if( strfind( status, LOAD_SUCCESS ) == 0 )
			{
				int len = strlen( currentLine );
				if( currentLine[len - 1] != '&' )
					foreground = true;
				else
					foreground = false;
			}
			else if( strfind( status, LOAD_ERROR ) == 0 )
			{
				cout << "Could not load: " << currentLine << "\n";
			}
		}
	}
}

void ForkShell::writeMessage( const char *msg )
{
	for( int i = 0; msg[i] != '\0'; i++ )
		write( child[1], &msg[i], 1 );
	char c = '\0';
	write( child[1], &c, 1 );
}

void ForkShell::readMessage( char *msg )
{
	int i = 0;
	char ch;

	do
	{
		read( parent[0], &ch, 1 );
		msg[i] = ch;
		i++;
	}while( ch != '\0' && i < MAX_LENGTH );

	i--;
	msg[i] = '\0';
}

