#include "program.h"

#ifndef __SHELL__
#define __SHELL__

#define LINE_LENGTH 80

class Shell
{
	public:
		/*
		 * PRE: This object is not instantiated.
		 * POST: This object is instantiated, but it cannot
		 * 	be used until it's program is set.
		 */
		Shell();
		/*
		 * PRE: This object is not instantiated.
		 * POST: The object is instantiated and is able to control
		 * 	the passed in program.
		 */
		Shell( Program *program );

		/*
		 * PRE: This object is instantiated.
		 * POST: The shell will be waiting for commands and will exit once it
		 * 	receives the `exit` command.
		 */
		void run();

		/*
		 * PRE: The object is instantiated.
		 * POST: The currentLine will be set to the value held in s
		 */
		void setCurrentLine( const char *s );


		/*
		 * PRE: This object is instantiated and the current line has been parsed.
		 * POST: The proper execution for the current command will be done.
		 */
		bool executeCurrentLine();

	protected:

		/*
		 * PRE: This object is instantiated and there is a current line.
		 * POST: The parsed parts will be stored in the command, parameter, parameter
		 * 	member variables.
		 */
		void parseLine();


		/*
		 * PRE: The string is a valid string with a null terminator.
		 * POST: The integer represented by the string.
		 */
		int getInt( const char *s );

		
		Program *program;
		char currentLine[LINE_LENGTH];
		bool running;
};

#endif
