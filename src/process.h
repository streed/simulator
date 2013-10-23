#ifndef __PROCESS__
#define __PROCESS__

#include "forkshell.h"
#include "program.h"

class Process: public Program
{
	public: 
		Process();

		virtual ~Process();

		/*
		 * PRE: This object is initialized
		 * POST: After this function is done it will have
		 * 	loaded up the new filename specified at `filename`.
		 * 	This will also call the `init` method to initialize the 
		 * 	process to the proper start state.
		 */
		void reload( char *filename );

		/*
		 * PRE: This object is intialized and hte pipe is opened.
		 * POST: The pipe will be read to execute the proper command.
		 * 	It will also determine if the program is still running
		 * 	or not and if it is then it will continue to send the 
		 * 	`running` message back to the pipe else it will send
		 * 	`done` message to the pipe.
		 */
		void processPipe();

		/*
		 * PRE: This object is intialized and the pipes are open.
		 * POST: This method will wait for the command across the pipe and then 
		 * 	call to processPipe in order to execute the proper method.
		 */
		void wait_cmds( int child[], int parent[] );

	protected:

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

		/*
		 * PRE: This oject is intialized and the write pipe is opened.
		 * POST: The message will have been written to the pipe.
		 */
		void writeMessage( const char *msg );

		/*
		 * PRE: This object is intialized and the read pipe is opened.
		 * POST: If there is a message on the pipe then it will wait
		 * 	to read it else it will just continue you.
		 */
		void readMessage( char *msg );

		bool running;
		int *child;
		int *parent;
};

#endif
