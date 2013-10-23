#include "processgroup.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>

#include <streambuf>
#include <sstream>

using std::cout;
using std::streambuf;//used to store the current cout buffer
using std::ostringstream;//Temporarily used to capture the output.

int strcmp( const char *a, const char *b )
{
	int ret = 0;
	if( *a == '\0' && *b != '\0' )
		ret = 1;
	else
		while( *a != '\0' && *b != '\0' )
		{
			ret += *a - *b;
			a++;
			b++;
		}

	return ret;
}

bool compare_by_name( MultiProgram *a, MultiProgram *b )
{
	if( strcmp( a->getFilename(), b->getFilename() ) == 0 )
		return true;
	else
		return false;
}

bool compare_by_pid( MultiProgram *a, MultiProgram *b )
{
	if( a->getPid() == b->getPid() )
		return true;
	else 
		return false;
}

void print_queue( Link<MultiProgram *> *b )
{
	MultiProgram *a = b->getData();
	cout << a->getPid() << "\t" << a->getFilename() << "\t" << a->getStartCount() << "\n";
}

ProcessGroup::ProcessGroup( int ts, int ms )
{
	ready = new List<MultiProgram *>();
	io = new List<MultiProgram *>();
	memory = new MemoryManager( ms );
	numTicks = ts;
	running = true;
	pid = 0;
	waitingInput = false;
	startCount = 0;
	suspended = false;
}

void ProcessGroup::load( char *filename )
{
	MultiProgram *p = new MultiProgram( pid++, filename, memory, this, startCount );
	p->init();
	if( !p->getError() )
	{
		scheduleReady( p );
		writeMessage( LOAD_SUCCESS );
	}
	else
		writeMessage( LOAD_ERROR );
}

void ProcessGroup::reload( char *filename )
{
	//This will find the program or return Null if not found. 
	//The compare_by_name function will be used to compared the various programs
	//looking for the specific program.
	MultiProgram fakeMultiProgram;
	fakeMultiProgram.setFilename( filename );
	Link<MultiProgram *> *found = ready->find( &fakeMultiProgram, compare_by_name );	 

	if( found == 0 )
	{
		found = io->find( &fakeMultiProgram, compare_by_name );

		if( found )
		{
			found->getData()->restart();
		}
	}
}

void ProcessGroup::processPipe()
{
	char buffer[MAX_READ];
	buffer[0] = '\0';
	readMessage( buffer );
	if( buffer[0] != '\0' )
	{
		processBuffer( buffer );
	}
}

void ProcessGroup::processBuffer( char *input )
{
	cout.flush();
	if( waitingInput )
	{
		int i = 0;
		for( ; input[i] != '\0'; i++ )
		{
			shellInput[i] = input[i];
		}
		
		shellInput[i] = '\0';
		waitingInput = false;
		return;
	}

	if( strfind( input, EXIT ) != 0 && strfind( input, PRINT_MEMORY ) != 0
	    && strfind( input, STEP ) != 0 && strfind( input, MBR ) != 0
	    && strfind( input, RESTART ) != 0 && strfind( input, PRINT_CPU ) != 0 
	    && strfind( input, BRING_CURRENT_PROCESS_TO_FOREGROUND ) != 0
	    && strfind( input, BRING_LAST_PROCESS_TO_FOREGROUND ) != 0
	    && strfind( input, QUEUE ) != 0
	    && strfind( input, JOBS ) != 0
	    && strfind( input, SUSPEND_SIM ) != 0
	    && strfind( input, KILL ) != 0
	    && strfind( input, FREEMEM ) != 0
	    && input[0] != '\0' )
	{
		load( input );
		return;
	}
	else
	{
		if( strfind( input, QUEUE ) == 0 )
		{
			streambuf *old = cout.rdbuf();
			ostringstream tempOut;
			cout.rdbuf( tempOut.rdbuf() );

			cout << "Ready Queue\n";
			ready->map( print_queue );
			cout << "I/O Queue\n";
			io->map( print_queue );
			writeMessage( tempOut.str().c_str() );

			tempOut.flush();
			cout.rdbuf( old );
			cout.flush();
		}
		else if( strfind( input, BRING_CURRENT_PROCESS_TO_FOREGROUND ) == 0 )
		{
			char line[MAX_LENGTH];
			do
			{
				readMessage( line );
			}while( line[0] == '\0' );

			int pid = getInt( line );
			bool gaveShell = false;

			for( int i = 0; i < ready->length() && !gaveShell; i++ )
			{
				MultiProgram *c = ready->get( i )->getData();

				if( c->getPid() == pid )
				{
					c->giveShell();
					gaveShell = true;
				}
			}

			if( !gaveShell )
			{
				for( int i = 0; i < io->length() && !gaveShell; i++ )
				{
					MultiProgram *c = io->get( i )->getData();

					if( c->getPid() == pid )
					{
						c->giveShell();
						gaveShell = true;
					}
				}
			}
		}
		else if( strfind( input, BRING_LAST_PROCESS_TO_FOREGROUND ) == 0 )
		{
			MultiProgram *last = NULL;
			if( ready->length() > 0 )
			{
				last = ready->get( 0 )->getData();

				for( int i = 1; i < ready->length(); i++ )
				{
					MultiProgram *c = ready->get( i )->getData();

					if( c->getStartCount() > last->getStartCount() )
						last = c;
				}
			}

			if( io->length() > 0 )
			{

				last = io->get( 0 )->getData();

				for( int i = 1; i < io->length(); i++ )
				{
					MultiProgram *c = io->get( i )->getData();

					if( c->getStartCount() > last->getStartCount() )
						last = c;
				}
			}

			if( last )
				last->giveShell();
		}
		else if( strfind( input, JOBS ) == 0 )
		{

			streambuf *old = cout.rdbuf();
			ostringstream tempOut;
			cout.rdbuf( tempOut.rdbuf() );
			
			cout << "Jobs: \nPID\tFilename\twaiting\n";

			for( int i = 0; i < ready->length(); i++ )
			{
				MultiProgram *c = ready->get( i )->getData();

				cout << c->getPid() << "\t" << c->getFilename() << "\t" << c->waitingIO() << "\n";
			}

			for( int i = 0; i < io->length(); i++ )
			{
				MultiProgram *c = io->get( i )->getData();

				cout << c->getPid() << "\t" << c->getFilename() << "\t" << c->waitingIO() << "\n";
			}

			cout << "\n";
			writeMessage( tempOut.str().c_str() );

			tempOut.flush();
			cout.rdbuf( old );
			cout.flush();
		}
		else if( strfind( input, SUSPEND_SIM ) == 0 )
		{
			suspended = true;
		}
		else if( strfind( input, RESTART ) == 0 )
		{
			suspended = false;
		}
		else if( strfind( input, KILL ) == 0 )
		{
			
			char line[MAX_LENGTH];
			do
			{
				readMessage( line );
			}while( line[0] == '\0' );

			
			if( line[0] == 'N' )
			{
				if( current )
				{

					ready->deleteOne( current, compare_by_pid );
					io->deleteOne( current, compare_by_pid );
					current->free();
					current = NULL;
				}
			}
			else
			{
				int pid = getInt( line );
				bool found = false;
				MultiProgram *c;

				for( int i = 0; i < ready->length() && !found; i++ )
				{
					c = ready->get( i )->getData();

					if( c->getPid() == pid )
					{
						found = true;
						c->free();
						ready->deleteOne( c, compare_by_pid );
					}
				}

				if( !found )
				{
					for( int i = 0; i < io->length() && !found; i++ )
					{
						c = io->get( i )->getData();

						if( c->getPid() == pid )
						{
							found = true;
							c->free();
							io->deleteOne( c, compare_by_pid );
						}
					}
				}
			}
		}
		else if( strfind( input, STEP ) == 0 )
		{
			if( suspended )
			{
				stepping = true;
				char steps[MAX_LENGTH];
				do
				{
					readMessage( steps );
				}while( steps[0] == '\0' );

				char pid[MAX_LENGTH];
				do
				{
					readMessage(pid );
				}while( pid[0] == '\0' );

				if( steps[0] != 'N' )
				{
					steppingSteps = getInt( steps );
					if( pid[0] != 'N' )
					{
						steppingPid = getInt( pid );
					}
					else
					{
						if( current )
							steppingPid = current->getPid();
						else
						{
							current = io->pop();

							if( current )
								steppingPid = current->getPid();
							else
								stepping = false;
						}
					}
				}
				else
					steppingSteps = 1;

				writeMessage( STEP );
			}
			else
				writeMessage( "notstep" );
		}
		else if( strfind( input, FREEMEM ) == 0 )
		{
			streambuf *old = cout.rdbuf();
			ostringstream tempOut;
			cout.rdbuf( tempOut.rdbuf() );

			memory->printLists();
			writeMessage( tempOut.str().c_str() );

			tempOut.flush();
			cout.rdbuf( old );
			cout.flush();
		}

		if( current )
		{
			streambuf *old = cout.rdbuf();
			ostringstream tempOut;
			cout.rdbuf( tempOut.rdbuf() );

			if( strfind( input, PRINT_CPU ) == 0 )
			{
				current->printCPU();
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
				current->printMemory( a, b );
				writeMessage( tempOut.str().c_str() );
			}
			else if( strfind( input, MBR ) == 0 )
			{
				char line[MAX_LENGTH];
				sprintf( line, "mbr = 0x%08X\n", current->getMBR() );
				writeMessage( line );
			}
			else if( strfind( input, STEP ) == 0 )
			{
				running = current->step();
					if( running )
					writeMessage( RUNNING );
				else
					writeMessage( DONE );
			}
			else if( strfind( input, EXIT ) == 0 )
			{
				running = false;
			}

			tempOut.flush();
			cout.rdbuf( old );
			cout.flush();
		}
	}
}

void ProcessGroup::wait_commands( int c[], int p[] )
{
	parent = p;
	child = c;
	while( running )
	{
		if( stepping )
		{
			printf( "Stepping\nlooking in ready\n" );
			MultiProgram fake;
			fake.setPid( steppingPid );
			Link<MultiProgram *> *f = ready->find( &fake, compare_by_pid );

			if( !f )
			{
				printf( "looking in io\n" );
				f = io->find( &fake, compare_by_pid );
			}

			if( f )
			{
				printf( "Stepping\n" );
				current = f->getData();
				current->giveShell();
				bool run = true;
				for( int i = 0; i < steppingSteps && run; i++ )
				{
					run = current->step();

					if( current->hasShell() )
					{
						if( run )
							sendOutput( RUNNING );
						else
						{
							current->free();
							sendOutput( DONE );
						}
					}
				}
			}

			stepping = false;
		}
		else
		{
			if( !suspended )
				getNextProcess();
			processPipe();
	
			if( !suspended )
				performTicks();
		}
	}
}

void ProcessGroup::performTicks()
{
	startCount++;
	if( current )
	{
		bool run = true;
		for( int i = 0; i < numTicks && run && !current->waitingIO(); i++ )
		{
			run = current->step();

			if( current->hasShell() )
			{
				if( run )
					sendOutput( RUNNING );
				else
				{
					current->free();
					sendOutput( DONE );
				}
			}
		}

		if( run && !current->waitingIO() )
			ready->add( current );

		if( !run )
			current->free();
	}

	current = io->pop();
	if( current )
	{
		current->step();
	}
}

void ProcessGroup::scheduleReady( MultiProgram *mp )
{
	ready->add( mp );
}

void ProcessGroup::scheduleIO( MultiProgram *mp )
{
	io->add( mp );
}

void ProcessGroup::readMessage( char *msg )
{
	int i = 0;
	char ch;

	for( int i = 0; i < MAX_LENGTH; i++ )
		msg[i] = '\0';

	do
	{
		int status = read( child[0], &ch, 1 );
		if( status == -1 )
		{
			msg[i] = '\0';
			ch = '\0';
		}
		else
		{
			msg[i] = ch;
			i++;
		}
	}while( ch != '\0' );

	msg[i] = '\0';
}

void ProcessGroup::writeMessage( const char *msg )
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

int ProcessGroup::getInt( const char *input )
{
	int temp = 0;
	for( int i = 0; input[i] != '\0'; i++ )
	{
		temp *= 10;
		temp += ( input[i] - '0' );
	}

	return temp;
}

bool ProcessGroup::hasInput()
{
	if( shellInput[0] != '\0' )
		return true;
	else
		return false;
}

int ProcessGroup::getInput()
{
	waitingInput = false;
	int ret =  getInt( shellInput );
	shellInput[0] = '\0';

	return ret;
}

void ProcessGroup::getNextProcess()
{
	current = ready->pop();
}
