#include <iostream>
using std::cin;
using std::cout;

#include <fstream>
using std::ifstream;

#include <stdio.h>

#include "program.h"

#define MAX_LENGTH 80

#define flip( v ) ( ( ( v & 0xFF ) << 12 ) | ( v & 0x0F00 ) | ( ( v & 0xFF000 ) >> 12 ) )

Program::Program(): memorySize( 1024 )
{}

Program::Program( char *filename, unsigned int memSize )
{
	memorySize = memSize;
	init();
	_read( filename );
}

Program::~Program()
{
	delete [] memory;
	memory = 0;
}

void Program::init()
{
	pc = 0;
	mbr.binary = 0;
	regs[ZERO] = 0;
	regs[SP] = memorySize - 1;
	baseAddress = 0;
}

void Program::_read( char *filename )
{
	ifstream is;
	is.open( filename );
	unsigned int currentMemory = baseAddress;


	if( is.is_open() )
	{
		while( is.good() )
		{
			unsigned char word[BYTES_IN_WORD];
	
			for( int i = 0 ; i < BYTES_IN_WORD; i++ )
				word[i] = is.get();

			memory[currentMemory] = makeWord( word );
			currentMemory++;
		}
	}
	else
	{
		cout << "Could not open '" << filename << "'" << "\n";
		error = true;
	}

	is.close();
}

void Program::restart()
{
	if( memory )
	{
		delete [] memory;
		memory = 0;
	}
	init();
}

bool Program::fetch()
{
	unsigned int loc = pc / 4;
	mbr.binary = memory[loc];
	pc += 4;

	return true;
}

bool Program::execute()
{
	bool ret = true;
	switch( mbr.op )
	{
		case ADD: add(); break;
		case NAND: nand(); break;
		case ADDI: addi(); break;
		case LW: lw(); break;
		case SW: sw(); break;
		case BEQ: beq(); break;
		case JALR: jalr(); break;
		case HALT: halt(); ret = false; break;
		case IN: in(); break;
		case OUT: out(); break;
		default: ret = false;
	}

	return ret;
}

bool Program::step()
{
	fetch();
	return execute();
}

void Program::printCPU()
{
	char temp[MAX_LENGTH];
	sprintf( temp, "mbr( 0x%08X ) pc( 0x%08X )", mbr, pc ); 
	cout << temp;

	for( int i = 0; i < NUM_REGISTERS; i++ )
	{
		if( i % 4 == 0 )
			cout << "\n";

		sprintf( temp,  "%s( 0x%08X ) ", registerNames[i], regs[i] );
		cout << temp;
	}

	cout << "\n";
}

void Program::printMemory( unsigned int start, unsigned int end )
{
	char line[MAX_LENGTH];
	for( int i = start; i < end; i++ )
	{
		if( i != 0 && i % 4 == 0 )
			cout << "\n";

		sprintf( line, "0x%08X ", memory[i] );
		cout << line;
	}

	cout << "\n\n";
}

unsigned int Program::getWord( unsigned int index )
{
	return memory[index];
}

void Program::add()
{
	if( mbr.y == SP && mbr.x == SP )
		regs[mbr.x] = regs[mbr.y] + ( regs[mbr.z] / 4 );
	else
		regs[mbr.x] = regs[mbr.y] + regs[mbr.z];
}

void Program::nand()
{
	regs[mbr.x] = ~( regs[mbr.y] & regs[mbr.z] );
}

void Program::addi()
{
	if( mbr.y == SP && mbr.x == SP )
		regs[mbr.x] = regs[mbr.y] + ( mbr.value / 4 );
	else
		regs[mbr.x] = regs[mbr.y] + mbr.value;
}

void Program::lw()
{
	regs[mbr.x] = memory[regs[mbr.y] + mbr.value];
}

void Program::sw()
{
	memory[regs[mbr.y] + mbr.value] = regs[mbr.x];
}

void Program::beq()
{
	if( regs[mbr.x] == regs[mbr.y] )
		pc += mbr.value * 4;
}

void Program::jalr()
{
	if( mbr.y == mbr.x )
	{
		pc = regs[mbr.x];
	}
	else
	{
		regs[mbr.y] = pc;
		pc = regs[mbr.x];
	}
}

void Program::halt()
{
	running = false;
}

void Program::in()
{
	char line[MAX_LINE_LENGTH];
	cout << "% ";
	cin >> line;

	int temp = getInt( line );

	regs[mbr.x] = temp;
}

void Program::out()
{
	cout << (regs[mbr.x]) << "\n";
}

int Program::getInt( const char *input )
{
	int temp = 0;
	for( int i = 0; input[i] != '\0'; i++ )
	{
		temp *= 10;
		temp += ( input[i] - '0' );
	}

	return temp;
}

void Program::setFilename( const char *f )
{
	int i = 0;
	for( ; f[i] != '\0'; i++ )
		filename[i] = f[i];
	filename[i] = '\0';
}
