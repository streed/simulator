#include "UnitTest++.h"
#include "program.h"
#include "shell.h"
#include "memorymanager.h"
#include "processgroup.h"

#include <stdio.h>

static char *junk_file = "./tests/junk_file.bin";
static char *simple_program = "./tests/simple_add.bin";

#include <iostream>
using std::cout;

/*
SUITE( Program_tests )
{
	TEST( InstructionUnion_extracts_parts_correctly )
	{
		InstructionUnion a;
	
		a.binary = 0x43200001;
		CHECK_EQUAL( 4, a.op );
		CHECK_EQUAL( 3, a.x );
		CHECK_EQUAL( 2, a.y );
		CHECK_EQUAL( 1, a.z );
		CHECK_EQUAL( 1, a.value );
	}

	TEST( makeWord_works_properly )
	{
		unsigned char a[] = { 'A', 'B', 'C', 'D' };
	
		CHECK_EQUAL( 0x41424344, makeWord( a ) );
	}

	TEST( Program_loads_the_file_correctly )
	{	
		Program p( junk_file, 100 );
	
		int32_t memory = p.getWord( 0 );

		CHECK_EQUAL( 0x41424344, memory );

		memory = p.getWord( 1 );

		CHECK_EQUAL( 0x45464748, memory );
	}

	TEST( Program_after_fetch_has_the_value_of_0x44434241_in_mbr )
	{
		Program p( junk_file, 100 );

		p.fetch();
		CHECK_EQUAL( 0x41424344, p.getMBR() );
	}

	TEST( Program_fetch_increments_PC )
	{
		Program p( junk_file, 100 );
	
		CHECK_EQUAL( 0, p.getPC() );
		p.fetch();
		CHECK_EQUAL( 4, p.getPC() );
	}

	TEST( Program_does_the_addi_correctly_and_first_sets_t0_to_0_then_sets_it_to_1 )
	{
		Program p( simple_program, 100 );

		p.step();

		int32_t pc = p.getPC();

		CHECK_EQUAL( 4, pc );

		int32_t t0 = p.getRegister( 6 );

		CHECK_EQUAL( 0, t0 );

		p.step();

		pc = p.getPC();
		CHECK_EQUAL( 8, pc );

		t0 = p.getRegister( 6 );
		CHECK_EQUAL( 1, t0 );
	}

	TEST( Program_does_addi_correctly_with_negative_number )
	{
		Program p( "./tests/add_neg.bin", 100 );

		p.step();

		int32_t t0 = p.getRegister( 6 );

		CHECK_EQUAL( -1, (signed int)t0 );
	}

	TEST( Program_does_R_type_instruction_correctly )
	{
		Program p( "./tests/nand.bin", 100 );

		p.step();
		p.step();

		int32_t t0 = p.getRegister( 6 );

		CHECK_EQUAL( (int32_t)(~( 1 & 1 )), t0 );
	}

	TEST( Program_does_I_sw_correctly )
	{
		Program p( "./tests/sw.bin", 100 );

		p.step();
		p.step();

		int32_t mem = p.getWord( 2 );

		CHECK_EQUAL( 1, mem );
	}

	TEST( Program_does_I_lw_correctly )
	{
		Program p( "./tests/lw.bin", 100 );

		p.step();
		p.step();

		int32_t t0 = p.getRegister( 6 );

		CHECK_EQUAL( 1, t0 );
	}

	TEST( Program_does_jarl_correctly )
	{
		Program p( "./tests/jalr.bin", 100 );

		p.step();
		p.step();

		int32_t t0 = p.getRegister( 6 );
		int32_t t1 = p.getRegister( 7 );
		int32_t pc = p.getPC();

		CHECK_EQUAL( 8, t0 );
		CHECK_EQUAL( 8, t1 );
		CHECK_EQUAL( 8, pc );
	}

	TEST( Program_does_unconditional_zero_equals_zero_correctly )
	{
		Program p( "./tests/beq_un.bin", 100 );

		p.step();
	
		int32_t pc = p.getPC();

		CHECK_EQUAL( 44, pc );
	}

	TEST( Program_does_unconditional_t0_not_equal_t1 )
	{
		Program p( "./tests/beq_ne.bin", 100 );
	
		p.step();
		p.step();
	
		int32_t pc = p.getPC();

		CHECK_EQUAL( 8, pc );
	}

	TEST( Program_does_proper_unconditional_beq_with_negative_offset )
	{
		Program p( "./tests/beq_neg.bin", 100 );

		p.step();

		CHECK_EQUAL( 0, p.getPC() );
	}

	TEST( Program_collects_input_correctly )
	{
		Program p( "./tests/in.bin", 100 );

		printf( "Please enter in 100\n" );
		p.step();

		int32_t t0 = p.getRegister( 6 );

		CHECK_EQUAL( 100, t0 );
	}

	TEST( Program_outputs_correctly )
	{
		Program p( "./tests/out.bin", 100 );

		p.step();
		p.step();
	}

	TEST( Program_runs_a_complete_program )
	{
		printf( "Running prog.bin\nPlease enter in 4\n" );
		Program p( "./tests/prog.bin", 100 );

		bool run = p.step();
		while( run )
		{
			run = p.step();
		}

		CHECK_EQUAL( 16, p.getRegister( 7 ) );
	}
}
*/
/*
SUITE( Shell_tests )
{
	TEST( Shell_parses_and_executes_step_correctly )
	{
		Program *p = new Program( "./tests/beq_ne.bin", 100 );

		Shell s( p );

		s.setCurrentLine( "step" );
		s.executeCurrentLine();
		s.setCurrentLine( "step" );
		s.executeCurrentLine();

		CHECK_EQUAL( 8, p->getPC() );	

		delete p;
	}

	TEST( Shell_parses_and_exeuctes_step_with_num_steps_parameter )
	{
		Program *p = new Program( "./tests/beq_ne.bin", 100 );
		Shell s( p );

		s.setCurrentLine( "step 2" );
		s.executeCurrentLine();

		CHECK_EQUAL( 8, p->getPC() );

		delete p;
	}

	TEST( Shell_parses_and_executes_mem_correctly )
	{
		Program *p = new Program( "./tests/beq_ne.bin", 100 );
		Shell s( p );
		s.setCurrentLine( "mem 0 10" );
		s.executeCurrentLine();
	}

	TEST( Shell_parses_and_executes_cpu_correctly )
	{
		Program *p = new Program( "./tests/beq_ne.bin", 100 );
		Shell s( p );

		s.setCurrentLine( "cpu" );
		s.executeCurrentLine();
	}

	TEST( Shell_parses_and_executes_instr_correctly )
	{
		Program *p = new Program( "./tests/beq_ne.bin", 100 );
		Shell s( p );
		s.setCurrentLine( "instr" );
		s.executeCurrentLine();
	}

	TEST( Shell_parses_and_executes_run_correctly )
	{
		Program *p = new Program( "./tests/prog.bin", 100 );
		Shell s( p );

		printf( "Please enter 4 at the prompt\n" );
		s.setCurrentLine( "run" );
		s.executeCurrentLine();

		CHECK_EQUAL( 16, p->getRegister( 7 ) );
	}
}
*/
SUITE( MemoryManager_ )
{
	TEST( MemoryManager_allocates_memory_correctly )
	{
		MemoryManager m( 100 );

		CHECK( 0 != m.getMemory() );
	}

	TEST( MemoryManager_allocates_block_correctly )
	{
		MemoryManager m( 100 );

		CHECK( 0 != m.allocate( 50 ) );
	}

	TEST( MemoryManager_cannot_find_free_block_of_suitable_size )
	{
		MemoryManager m( 50 );

		int32_t *v = m.allocate( 60 );

		CHECK_EQUAL( (int32_t *)0, v );
	}

	TEST( MemoryManager_finds_most_suitable_memory_location )
	{
		MemoryManager m( 50 );

		int32_t *a = m.allocate( 10 );
		int32_t *b = m.allocate( 10 );
		int32_t *c = m.allocate( 5 );

		int32_t *save_b = b;

		m.free( b );
		
		int32_t *d = m.allocate( 10 );

		CHECK_EQUAL( save_b, d );
	}

	TEST( MemoryManager_combines_blocks_forward_on_free )
	{
		MemoryManager m( 20 );


		int32_t *a = m.allocate( 10 );
		int32_t *b = m.allocate( 10 );

		int32_t *save_a = a;

		m.free( b );

		CHECK_EQUAL( 1, m.getNumFreeBlocks() );

		m.free( a );

		CHECK_EQUAL( 1, m.getNumFreeBlocks() );

		int32_t *c = m.allocate( 20 );

		CHECK_EQUAL( save_a, c );
	}

	TEST( MemoryManager_combines_blocks_backwards_on_free )
	{
		MemoryManager m( 20 );

		int32_t *a = m.allocate( 10 );
		int32_t *b = m.allocate( 10 );

		int32_t *save_a = a;

		m.free( a );

		CHECK_EQUAL( 1, m.getNumFreeBlocks() );

		m.free( b );

		CHECK_EQUAL( 1, m.getNumFreeBlocks() );

		int32_t *c = m.allocate( 20 );

		CHECK_EQUAL( save_a, c );
	}
}
SUITE( ProcessGroup )
{
	TEST( ProcessGroup_loads_a_program_and_adds_to_ready_queue )
	{
		ProcessGroup pg;

		pg.load( "./square" );

		CHECK_EQUAL( "./square", pg.getReadyQueue( 0 )->getData()->getFilename() );
	}

	TEST( ProcessGroup_runs_a_program_for_the_specified_time_slice )
	{
		ProcessGroup pg;

		pg.load( "./square" );

		pg.getNextProcess();
		pg.performTicks();

		CHECK_EQUAL( 8, pg.getReadyQueue( 0 )->getData()->getPC() );
	}

	TEST( ProcessGroup_loads_more_than_one_program_and_places_on_ready_queue )
	{
		ProcessGroup pg;

		pg.load( "./square" );
		pg.load( "./sum" );

		CHECK_EQUAL( "./square", pg.getReadyQueue( 0 )->getData()->getFilename() );
		CHECK_EQUAL( "./sum", pg.getReadyQueue( 1 )->getData()->getFilename() );
	}

	TEST( ProcessGroup_correctly_goes_through_ready_queue )
	{
		ProcessGroup pg;

		pg.load( "./square" );
		pg.load( "./sum" );

		pg.getNextProcess();
		pg.performTicks();
		pg.getNextProcess();
		pg.performTicks();

		CHECK_EQUAL( 8, pg.getReadyQueue( 0 )->getData()->getPC() );
		CHECK_EQUAL( 8, pg.getReadyQueue( 1 )->getData()->getPC() );
	}
}

int main()
{
	return UnitTest::RunAllTests();
}
