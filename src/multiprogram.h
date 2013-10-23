#ifndef __MULTIPROGRAM__
#define __MULTIPROGRAM__

#include "program.h"
#include "memorymanager.h"
#include "processgroup.h"

class ProcessGroup;

class MultiProgram: public Program
{
	public:
		MultiProgram();

		/*
		 * PRE: This will call the normal Program constructor as the
		 * 	programs memory block must be passed to it.
		 * POST: The program will be in an inoperable state
		 * 	because it will have no memory space.
		 */
		MultiProgram( int id, char *filename, MemoryManager *manager, ProcessGroup *pg, int startCount );

		virtual ~MultiProgram();

		/*
		 * PRE: This object is instantiated.
		 * POST The program's memory will be set to the proper base location in the global
		 * 	memory pool. Then all the registers will be set to the
		 * 	proper starting values, specifically the stack pointer and starting
		 * 	address.
		 */
		void init();

		int getSize() const { return size; }

		bool hasShell() { return shellControl; }

		void giveShell() { shellControl = true; }
		void takeShell() { shellControl = false; }

		void redoInstruction() { pc -= 4; }

		int getNumExecutedInstructions() { return numExecutedInstructions; }

		int getPid() { return pid; }
		void setPid( int p ) { pid = p; }

		int getStartCount() { return startCount; }

		bool waitingIO() { return waiting; }

		void free() { memoryManager->free( memory ); }


	private:

		/*
		 * PRE: This object is intialized and this instruction
		 * 	is the current instruction.
		 * POST: The pipe will first have that the message to tell
		 * 	the shell that something needs to be read in. Then
		 * 	it will block until the shell sends back the typed
		 * 	in information. Then the program will read in the 
		 * 	typed in information and continue execution.
		 */
		virtual void in();

		virtual void out();

		MemoryManager *memoryManager;
		ProcessGroup *processGroup;

		int size;
		int pid;
		int numExecutedInstructions;
		int startCount;

		bool shellControl;
		bool waiting;
};
#endif
