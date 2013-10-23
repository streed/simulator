#ifndef __FORKSHELL__
#define __FORKSHELL__

#include "shell.h"

#define PRINT_MEMORY "printmem"
#define STEP         "step"
#define MBR          "mbr"
#define RESTART      "restart"
#define PRINT_CPU    "cpu"
#define RUNNING      "run"
#define DONE         "done"
#define INPUT        "input"
#define OUTPUT       "output"
#define EXIT         "exit"
#define RESTART_SIM  "restart_sim"
#define JOBS         "jobs"
#define BRING_CURRENT_PROCESS_TO_FOREGROUND "bcptf"
#define BRING_LAST_PROCESS_TO_FOREGROUND    "blptf"
#define QUEUE        "queue"
#define SUSPEND_SIM  "suspend_sim"
#define LOAD_ERROR   "load_error"
#define LOAD_SUCCESS "load_success"
#define KILL         "kill"
#define FREEMEM      "freemem"

int strcmp( const char *a, const char *b );
int strfind( const char *stack, const char *needle );

class ForkShell: public Shell
{
	public:
		ForkShell( int timeslice, int memorySize );

		/*
		 * PRE: This object is initialized
		 * POST: The running of the code consists of the following behavior
		 * 	the current Shell will start a new Process that will not load
		 * 	any file. It will then sit and wait for a command. Once it
		 * 	receives the "run" command, or rather a string that is not
		 * 	a command. Once this is done then the pipe will be used to 
		 * 	communicate with thte other process.
		 */
		void run();

		/*
		 * PRE: This object is initialized and the currentLine is not
		 * 	empty and contains input.
		 * POST: The proper commands will be sent to the remote
		 * 	process for processing by way of the pipe.
		 */
		bool executeCurrentLine();

	protected:

		/*
		 * PRE: This object is instantiated and the pipe is open.
		 * POST: This will read a message from the pipe into the buffer
		 * 	`msg`.
		 */
		void readMessage( char *msg );

		/*
		 * PRE: This object is instantiated and the pipe is open.
		 * POST: The message in `msg` will be sent to the pipe.
		 */
		void writeMessage( const char *msg );

		int parent[2];
		int child[2];
		bool foreground;

		int steppingSteps;
		bool stepping;

		int timeSlice;
		int memorySize;
};

#endif
