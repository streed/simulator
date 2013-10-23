#ifndef __MEMORY_MANAGER__
#define __MEMORY_MANAGER__

#include "list.h"
#include <stdint.h>

class MemoryBlock
{
	public:
		MemoryBlock( int32_t *base, int size );

		int32_t *getBase() const { return base; }

		int getSize() const { return size; }

		void setBase( int32_t *b ) { base = b; }
		void setSize( int s ) { size = s; }
	
	private:
		int32_t *base;
		int size;
};

class MemoryManager
{
	public:
		MemoryManager( int size );

		int32_t *allocate( int size );

		int getFreeBlocks();

		void free( int32_t *base );

		int getNumFreeBlocks() { return blocks->length(); }

		int32_t *getMemory() { return memory; }

		void printLists();

	private:
		void merge( int32_t *base, int size );

		List<MemoryBlock *> *blocks;
		List<MemoryBlock *> *used;
		int32_t *memory;
		int size;
};

#endif
