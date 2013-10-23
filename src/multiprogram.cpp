#include "multiprogram.h"
#include <fstream>
#include <iostream>
using std::ifstream;
using std::ios;
using std::cout;
using std::cin;

#include <stdio.h>

#include "forkshell.h"


MultiProgram::MultiProgram(): Program()
{}

MultiProgram::MultiProgram( int id, char *fn, MemoryManager *manager, ProcessGroup *pg, int start ): 
	Program(), 
	memoryManager( manager ), 
	processGroup( pg )
{
	pid = id;
	startCount = start;
	shellControl = true;
	int i = 0;
	for( ; fn[i] != '\0'; i++ )
		if( fn[i] == ' ' )
		{
			shellControl = !(fn[i + 1] == '&');
			i += 2;
		}
		else
			filename[i] = fn[i];
	if( !shellControl )
		filename[i - 2] = '\0';
	else
		filename[i] = '\0';
}

MultiProgram::~MultiProgram()
{
	memoryManager->free( memory );
	memory = 0;
}

void MultiProgram::init()
{
	ifstream sizer( filename, ios::binary | ios::ate );
	size = sizer.tellg();
	size += 20;
	sizer.close();
	memory = memoryManager->allocate( size );
	
	_read( filename );

	pc = 0;
	regs[ZERO] = 0;
	regs[SP] = size - 1;
	waiting = false;
}

void MultiProgram::in()
{
	if( !hasShell() )
	{
		processGroup->scheduleIO( this );
		redoInstruction();
		waiting = true;
	}
	else
	{
		if( processGroup->hasInput() )
		{
			int value = processGroup->getInput();
			regs[mbr.x] = value;
			waiting = false;
			processGroup->scheduleReady( this );
		}
		else
		{
			if( !processGroup->waitingForShell() )
			{
				processGroup->toggleWaitingForShell();
				processGroup->sendOutput( INPUT );
			}
			processGroup->scheduleIO( this );
			redoInstruction();
		}
	}
}

void MultiProgram::out()
{
	if( !hasShell() )
	{
		processGroup->scheduleIO( this );
		redoInstruction();
		waiting = true;
	}
	else
	{
		processGroup->sendOutput( OUTPUT );
		char output[MAX_LENGTH];
		sprintf( output, "%d\n", regs[mbr.x] );
		processGroup->sendOutput( output );
		waiting = false;
	}
}

