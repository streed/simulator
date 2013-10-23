#ifndef __PROGRAM__
#define __PROGRAM__

#include <stdint.h>

#define NUM_REGISTERS 16
#define BYTES_IN_WORD 4

#define BYTE( a ) ( a & 0xFF )
#define makeWord( w ) ( \
			( BYTE( w[3] ) ) | \
			( BYTE( w[2] ) << 8 ) | \
			( BYTE( w[1] ) << 16 ) | \
			( BYTE( w[0] ) << 24 ) )

#define MAX_LINE_LENGTH 80
#define MAX_LENGTH      80
#define STACK_SIZE      20
#define SP 13 //The stack point to reference it in the init()
#define ZERO 0 //The zero register to reference it in the init()

/*
 *     Opcode representation in memory.
 *         The structure is inverted because of the bit endiness.
 *
 */
typedef union _instructionunion
{
	struct
	{
		unsigned z:4;//4 bit value for Z register.
		unsigned :28;//Padding of 28bits.
	};
	struct
	{
		signed value:20;//signed value. 
		unsigned :12;//Padding of 12bits.
	};
	struct
	{
		unsigned :20;//Padding of 20bits.
		unsigned y:4;//4 bit value for Y register.
		unsigned x:4;//4 bit value for X register.
		unsigned op:4;//4 bit value for OPCODE.
	};
	unsigned int binary;
}InstructionUnion;

/*
 *     Opcode values - prevents coder error and enforces that these are the only
 *         valid opcode values. ADD = 0 ... HALT = 7
 */
typedef enum
{   
	ADD = 0,
	NAND,
	ADDI,
	LW,
	SW,
	BEQ,
	JALR,
	HALT,
	PLACEHOLDER,//to get the opcode into the right position.
	IN,
	OUT
}Opcode;

static char *registerNames[] = { 
	"zero",
	"at",
	"v0",
	"a0",
	"a1",
	"a2",
	"t0",
	"t1",
	"t2",
	"s0",
	"s1",
	"s2",
	"k0",
	"sp",
	"fp",
	"ra"
};

class Program
{
	public:
		/*
		 * PRE: This program object is not intialized.
		 * POST: This object will be in a inoperable state.
		 * 	To use this object after calling this the `read` method must
		 * 	be called.
		 */
		Program();

		/*
		 * PRE: This program object is not intialized.
		 * POST: The program will be ready to execute any instructions
		 * 	contained in the bytecode of the file.
		 */
		Program( char *filename, unsigned int memorySize );

		/*
		 *PRE: This object is instantiated.
		 *POST: This program's memory will be free'd and set to 0
		 */
		~Program();

		/*
		 * PRE: This object is intialized.
		 * POST: The next instruction will be placed into this programs
		 * 	MBR and the PC will incremented by 4.
		 */
		bool fetch();


		/*
		 * PRE: This object is intialized and there is an instruction
		 * 	in the MBR to be executed.
		 * */
		bool execute();

		/*
		 * PRE: This object is intialized.
		 * POST: After this call the following will have happened.
		 * 	fetch will be called.
		 * 	And the program's state will advance as it should have according 
		 * 	to the instruction in the current word.
		 */
		bool step();

		/*
		 * PRE: This object is intialized.
		 * POST: After this method is run the initial state of the
		 * 	program will be setup so that it can be run again.
		 */
		void restart();

		/*
		 * PRE: This object is intialized.
		 * POST: The following format will be followed.
		 * 	PC = <unsigned int> MBR = 0x.....
		 * 	Each of the registers will be printed in the following format:
		 * 	<register name> = <regsiter value in hexidecimal with 8 places>
		 *
		 * 	The above format will contain four of them per row with a total
		 * 	of 4 rours.
		 */
		void printCPU();

		/*
		 * PRE: This object is intialized.
		 * POST: The following format will be used to print the memory:
		 * 	N-rows of the following format
		 * 	<position of first word> <word in hexidecimal> <word in hexidecimal> <word in hexidecimal> <word in hexidecimal>
		 **/
		void printMemory( unsigned int start, unsigned int end );

		/*
		 * PRE: This object is intialized.
		 * POST: The word at the specified location is returned.
		 */
		unsigned int getWord( unsigned int index );

		/*
		 * PRE: This objecy is intialized.
		 * POST: The register value is returned.
		 */
		unsigned int getRegister( unsigned int reg ) const { return regs[reg]; }

		/*
		 * PRE: This object is intialized.
		 * POST: The PC is returned.
		 */
		unsigned int getPC() const { return pc; }

		/*
		 * PRE: This object is initialized.
		 * POST: The MBR is returned.
		 */
		unsigned int getMBR() const { return mbr.binary; }

		/*
		 * PRE: This object is intialized.
		 * POST: The memorySize member will be set to size.
		 */
		void setMemorySize( int size ) { memorySize = size; }

		char *getFilename() { return filename; }

		void setFilename( const char *f );

		void setMemory( int32_t *mem ) { memory = mem; }

		bool getError() { return error; }

	protected:
		/*
		 * PRE: The program's "CPU" and "Memory" is not initialized.
		 * POST: The program's "CPU" and "Memory" are fully initialized.
		 * 	With the PC set to 0 and the memory has the fully loaded
		 * 	program in it.
		 *
		 */
		virtual void init();

		/*
		 * PRE: The filename is not null and is a valid path to a file.
		 * POST: The byte code represented in the file will be loaded into
		 * 	this program's memory.
		 */
		void _read( char *filename );	

		/*
		 * PRE: The object is instantiated.
		 * POST: The contents of reg x will equal the addition of
		 * 	reg y and reg z
		 */
		void add();

		/*
		 * PRE: The object instantiated.
		 * POST: The contents of reg x will equal the bitwise
		 * 	not of the bitwise and of reg y and z.
		 */
		void nand();

		/*
		 * PRE: The object is instantiated.
		 * POST: The contents of reg x will be equal to
		 * 	the addition of reg y wil the value.
		 */
		void addi();

		/*
		 * PRE: The object is instantiated.
		 * POST: reg x will contain the word at memory location 
		 * 	whose location is computed as the addition of
		 * 	reg y and the value (offset)
		 */
		void lw();

		/*
		 * PRE: The object is instantiated.
		 * POST: The memory location whose location is computed as
		 * 	the addition of reg y and the value (offset) will
		 * 	contain the value in reg x.
		 */
		void sw();

		/*
		 * PRE: The obhect is instantiated.
		 * POST: The PC will be set to the value(offset) if the
		 * 	reg x and reg y are equal to each other.
		 */
		void beq();

		/*
		 * PRE: This object is instantiated.
		 * POST: The reg y will hold the next instruction from PC and
		 * 	then set PC to the value stored in reg x.
		 */
		void jalr();

		/*
		 * PRE: This object is instantiated.
		 * POST: The program will have it's `running` flag 
		 * 	changed to false.
		 */
		void halt();

		/*
		 * PRE: This object is instantiated.
		 * POST: One word of input will be read and stored into 
		 * 	reg x.
		 */
		virtual void in();

		/*
		 * PRE: The object is instantiated.
		 * POST: The word in reg x will be printed to std::out.
		 */
		virtual void out();

		/*
		 * PRE: The string is a non-null string.
		 * POST: The integer that was represented by the string.
		 */
		int getInt( const char *input );

		InstructionUnion mbr;

		unsigned int pc;
		unsigned int memorySize;
		unsigned int baseAddress;

		int regs[NUM_REGISTERS];
		int *memory;

		bool running;

		char filename[MAX_LENGTH];

		bool error;
};

#endif
