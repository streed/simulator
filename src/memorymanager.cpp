#include "memorymanager.h"
#include <iostream>
using std::cout;

void print( Link<MemoryBlock *> *a )
{
	cout << a->getData()->getBase() << " " << a->getData()->getSize() << "\n";
}

bool compare_blocks_size( MemoryBlock *a, MemoryBlock *b )
{
	if( a->getSize() <= b->getSize() )
		return true;
	else
		return false;
}

bool compare_blocks_base( MemoryBlock *a, MemoryBlock *b )
{
	if( a->getBase() == b->getBase() )
		return true;
	else
		return false;
}

int order_blocks_by_base( MemoryBlock *a, MemoryBlock *b )
{
	cout << ( a->getBase() < b->getBase() ) << "\n";
	if( a->getBase() <= b->getBase() )
		return -1;
	else if( a->getBase() > b->getBase() )
		return 1;

	return 0;
}

Link<MemoryBlock *> *merge_blocks( Link<MemoryBlock *> *a, Link<MemoryBlock *> *b )
{
	MemoryBlock *ab = a->getData();
	MemoryBlock *bb = b->getData();

	cout << ab->getBase() << " " << ab->getSize() << " " << bb->getBase() << "\n";

	if( ab->getBase() + ab->getSize() == bb->getBase() )
	{
		ab->setSize( ab->getSize() + bb->getSize() );
		a->setNext( b->getNext() );
		delete bb;
	}

	return a;
}

MemoryBlock::MemoryBlock( int32_t *b, int s ): base( b ), size( s )
{}

MemoryManager::MemoryManager( int s )
{
	blocks = new List<MemoryBlock *>();
	used = new List<MemoryBlock *>();

	size = s;
	memory = new int32_t[size];

	MemoryBlock *mb = new MemoryBlock( memory, size );

	blocks->add( mb );
	blocks->setCompare( order_blocks_by_base );
}

int32_t *MemoryManager::allocate( int size )
{
	if( blocks->length() > 0 )
	{
		Link<MemoryBlock *> *found = blocks->get( 0 );
		int diff = found->getData()->getSize() - size;

		for( int i = 1; i < blocks->length(); i++ )
		{
			Link<MemoryBlock *> *temp = blocks->get( i );
			int tempDiff = temp->getData()->getSize() - size;
			if( tempDiff >= 0 && tempDiff < diff )
			{
				diff = tempDiff;
				found = temp;
			}
		}

		int32_t *base = 0;
	
		if( found && diff >= 0 )
		{
			MemoryBlock *block = found->getData();
			base = block->getBase();
			if( size == block->getSize() )
			{
				MemoryBlock *temp = new MemoryBlock( base, block->getSize() );
				used->add( temp );
				blocks->deleteOne( block, compare_blocks_base );
				return base;
			}
			else
			{
				MemoryBlock *temp = new MemoryBlock( base, size );
				used->add( temp );
				temp = new MemoryBlock( base + size / 4, block->getSize() - size );
				blocks->deleteOne( block, compare_blocks_base );
				blocks->add( temp );
				return base;
			}
		}
		else
			return base;
	}
	else
		return NULL;
}

void MemoryManager::merge( int32_t *base, int size )
{
	size = size / 4;
	MemoryBlock temp( base, size );
	Link<MemoryBlock *> *block = blocks->find( &temp, compare_blocks_base );
	Link<MemoryBlock *> *found = NULL;
	do
	{
		MemoryBlock fake( block->getData()->getBase() + size, 0 );

		found = blocks->find( &fake, compare_blocks_base );

		if( found )
		{
			MemoryBlock *temp = new MemoryBlock( block->getData()->getBase(), block->getData()->getSize() + 
											  found->getData()->getSize() );
			blocks->deleteOne( block->getData(), compare_blocks_base );
			blocks->deleteOne( found->getData(), compare_blocks_base );
			blocks->add( temp );
			block = blocks->find( temp, compare_blocks_base );
		}
		else
		{
			fake.setBase( block->getData()->getBase() - size );
			found = blocks->find( &fake, compare_blocks_base );

			if( found )
			{
				MemoryBlock *temp = new MemoryBlock( found->getData()->getBase(), block->getData()->getSize() + 
												  found->getData()->getSize() );
				blocks->deleteOne( block->getData(), compare_blocks_base );
				blocks->deleteOne( found->getData(), compare_blocks_base );
				blocks->add( temp );
				block = blocks->find( temp, compare_blocks_base );
			}
		}
	}while( found != 0 );
}

void MemoryManager::printLists()
{
	cout << "Free blocks\n";
	blocks->map( print );

	cout << "Used blocks\n";
	used->map( print );
}

void MemoryManager::free( int32_t *mem )
{
	MemoryBlock needle( mem, 0 );
	Link<MemoryBlock *> *found = used->find( &needle, compare_blocks_base );

	if( found )
	{	
		MemoryBlock *temp = new MemoryBlock( found->getData()->getBase(), 
							found->getData()->getSize() );
		blocks->add( temp );
		used->deleteOne( temp, compare_blocks_base );
		merge( temp->getBase(), temp->getSize() );
	}
}

