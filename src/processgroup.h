#ifndef __PROCESS_GROUP__
#define __PROCESS_GROUP__

#include "forkshell.h"
#include "multiprogram.h"
#include "list.h"
#include "memorymanager.h"

#define MAX_READ 80
#define NO_OUTPUT "no_output"

class MultiProgram;

class ProcessGroup
{
	public:
		ProcessGroup( int ts, int ms );

		/*
		 * PRE: This object is instantiated.
		 * POST: If the program with the corrosponding filename is found
		 * 	it will be reloaded and restarted.
		 */
		void reload( char *filename );

		/*
		 * PRE: This object is instantiated
		 * POST: The program will attempt to load into the 
		 * 	system's memory.
		 */
		void load( char *filename );

		/*
		 * PRE: This object is instantiated and the pipe is opened.
		 * POST: The pipe will be read to execute the proper commands
		 *	for the proper running program.
		 *	The message format looks as such:
		 *		<program id: unsigned int><command>[<parameters>]
		 */
		void processPipe();

		/*
		 * PRE: This object is instantiated and the pipes are open.
		 * POST: This method will wait and perform the actual execution of the programs.
		 */
		void wait_commands( int child[], int parent[] );

		/*
		 * PRE: This object is instantiated.
		 * POST: This will schedule and perform the proper number of
		 * 	cpu ticks on the particular process before switching.
		 * 	If the ready queue is empty this will return immediatly.
		 * 	If a process is done them `deallocateMem` will be called 
		 * 	and the process will be unloaded from the simulators memory,
		 * 	if it calls halt.
		 */
		void  performTicks();

		/*
		 * PRE: This object s instantiated.
		 * POST: The `Link` object associated with the index `i` will be
		 * 	returned else if the index `i` does not exist then `NULL` 
		 * 	will be returned.
		 */
		Link<MultiProgram *> *getReadyQueue( int i ) const { return ready->get( i ); }
		Link<MultiProgram *> *getIoQueue( int i ) const { return io->get( i ); }

		void getNextProcess();

		/*
		 * PRE: This object is instantiated.
		 * POST: This will schedule a process onto the
		 * 	ready queue.
		 */
		void scheduleReady( MultiProgram *mp );

		/*
		 * PRE: This obhect is instantiated.
		 * POST: This will schedule the process for I/O.
		 */
		void scheduleIO( MultiProgram *mp );

		/*
		 * PRE: This object is instantiated and it is waiting for shell
		 * 	input.
		 * POST: The return value is either true or false. Depending
		 * 	on if there is input received from the shell.
		 */
		bool hasInput();

		/*
		 * PRE: This object is instantiated and it has received input.
		 * POST: The return value of this method is the integer
		 * 	representation of the input.
		 */
		int getInput();

		/*
		 * Wraps the private `writeMessage`.
		 */
		void sendOutput( char *output ) { writeMessage( output ); }

		/*
		 * PRE: This object is instantiated.
		 * POST: The return value is weather we are
		 * 	waiting for the shell to send a response.
		 */
		bool waitingForShell() { return waitingInput; }

		void toggleWaitingForShell() { waitingInput = !waitingInput; }

	private:

		/*
		 * PRE: This object is instantiated.
		 * POST: The return value will either be a value >= 0 that
		 * 	represents the base address for the newly allocated memory block.
		 * 	If memory cannot be allocated then a -1 is returned.
		 */
		int allocatedMem();

		/*
		 * PRE: This object is instantiated and the base address is not < 0
		 * POST: The corrosponding memory block in the memoryBlocks list will
		 *	be deleted, thus freeing that portion of memory.
		 */
		void deallocateMem( int base );

		/*
		 * PRE: This object is instantiated and the pipe is opened.
		 * POST: This will write a message to the parent process.
		 */
		void writeMessage( const char *msg );

		/*
		 * PRE: Thios object is instantiated and the pipe is opened.
		 * POST: This will read a message from the parent process.
		 */
		void readMessage( char *msg );

		/*
		 * PRE: This object is instantiated and the buffer has been read.
		 * POST: The proper action will be completed based on the data
		 * 	in the read in buffer from the pipe.
		 */
		void processBuffer( char *buf );

		int getInt( const char *buf );

		List<MultiProgram *> *ready;//The queue of ready processes.
		List<MultiProgram *> *io;//The I/O queue.
		
		MemoryManager *memory;

		int numTicks;
		bool running;
		bool waitingInput;
		bool suspended;
		bool stepping;

		int *parent;
		int *child;

		int pid;

		MultiProgram *current;

		char shellInput[MAX_LENGTH];
		int startCount;

		int steppingPid;
		int steppingSteps;
};

#endif
