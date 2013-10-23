#pragma once
#ifndef __LIST__
#define __LIST__
#include <cstdarg>

template<class T> class Link
{
	public:
		Link();
		Link( T obj );
		~Link();
		// PRE: This object is defined.
		// POST: The RV is the mData member.
		T getData() { return mData; }
		// PRE: This object is defined as is nData.
		// POST: The mData will be set to nData.
		void setData( T nData ) { mData = nData; }
		// PRE: This object is defined.
		// POST: The RV is mPrev.
		Link<T> *getPrev() { return mPrev; }
		// PRE: This object, nPrev are defined.
		// POST: mPrev is set to nPrev.
		void setPrev( Link *nPrev ) { mPrev = nPrev; }
		// PRE: This object is defined.
		// POST: The RV is mNext.
		Link<T> *getNext() { return mNext; }
		// PRE: This object, nNext are defined.
		// POST: mNext is set to nNext.
		void setNext( Link *nNext ) { mNext = nNext; }

	private:
		T mData;
		Link<T> *mNext, *mPrev;
};
// PRE: This object is not defined. 
// POST: This object is defined.
template <class T> Link<T>::Link(): mNext( 0 ), mPrev( 0 )
{}

// PRE: This object is not defined.
// POST: This object is defined.
template <class T> Link<T>::Link( T obj ): mPrev( 0 ), mNext( 0 )
{
	mData = obj;
}

template <class T> Link<T>::~Link()
{
	delete mNext;
	mNext = 0;
	mPrev = 0;
}

template <class T> class List
{
	public:
		List();
		List( List<T> *nList );
		List( int (*compare)(T a, T b) );
		~List();
		void add( T obj );
		T pop();
		T peek();
		Link<T> *addUnique( T obj );
		Link<T> *find( T needle, bool (*match)( T, T ) );
		void addSorted( T obj );
		void replace( Link<T> *link, int numInsert, ... );
		Link<T> *operator[]( int index );
		int length() { return mLength; }
		void setCompare( int (*compare) ( T , T ) ) { mCompare = compare; }
		Link<T> *get( int i );
		void deleteOne( T needle, bool (*match)( T a, T b ) );	
		Link<T> *merge( Link<T> * (*func)( Link<T> *a, Link<T> *b ) );
		void map( void (*func)( Link<T> *a ) );
	protected:
		int mLength;
		Link<T> *mHead, *mTail;
		int (*mCompare)( T a, T b );
};
// PRE: This object is not defined.
// POST: This object is defined.
template <class T> List<T>::List(): mHead( 0 ),mTail( 0 ), mLength( 0 )
{}

// PRE: This object is not defined.
// POST: This object is defined.
template <class T> List<T>::List( int (*compare)( T a, T b ) ) : mHead( 0 ), mTail( 0 ), mLength( 0 ), mCompare( compare )
{}

template <class T> List<T>::List( List<T> *nList ): mHead( 0 ),mTail( 0 ), mLength( 0 )
{
	Link<T> *walker = nList->mHead;

	for( int i = 0; i < nList->length(); i++ )
	{
		add( walker->getData() );
	}
}

// PRE: This object is defined.
// POST: This object is destroyed and memory free'd
template <class T> List<T>::~List()
{
	delete mHead;
	mHead = 0;
	mTail = 0;
	mCompare = 0;
}

// PRE: This object is defined and as is obj.
// POST: The list will contain obj encapsulated in a Link object
//		added to the end of this list.
template <class T> void List<T>::add( T obj )
{
	if( mHead == 0 )
	{
		mHead = new Link<T>( obj );
		mTail = mHead;
	}
	else if( mHead == mTail )
	{
		Link<T>* temp = new Link<T>( obj );
		mTail = temp;
		mHead->setNext( mTail );
		mTail->setPrev( mHead );
		mTail->setNext( 0 );
	}
	else
	{
		Link<T>* temp = new Link<T>( obj );
		mTail->setNext( temp );
		temp->setPrev( mTail );
		mTail = temp;
	}

	mLength++;
}

// PRE: This object and obj are defined.
// POST: If the obj is not in the current list then it is
//		encapsulated in a Link object and appened to the end
//		of the list, else it is not added and the RV is
//		the link that is of the same value as obj.
#include <iostream>
template <class T> Link<T> *List<T>::addUnique( T obj )
{
	Link<T> *ret = 0;
	if( length() == 0 )
	{
		add( obj );
	}
	else
	{
		Link<T> *walker = mHead;
		bool addObj = true;
		for( int i = 0; i < length() && addObj; i++ )
		{
			//std::cout << walker << "\n";	
			if( walker != 0 )
			{
				if( mCompare(  obj, walker->getData() ) == 0 )
				{
					addObj = false;
				}
				else
					walker = walker->getNext();
			}
			else
				addObj = 0;
		}

		if( addObj )
			add( obj );
		else
			ret = walker;
	}

	return ret;
}

#include <iostream>
using std::cout;

template <class T> void List<T>::addSorted( T obj )
{
	Link<T> *walker = mHead;

	if( mLength == 0 )
	{
		mHead = new Link<T>( obj );
		mTail = mHead;
		mLength++;
	}
	else
	{
		bool added = false;
		for( int i = 0; i < mLength && !added; i++ )
		{
			cout << "addSorted " << mCompare( walker->getData(), obj ) << "\n";
			if( mCompare( walker->getData(), obj ) < 0 )
			{
				walker = walker->getNext();
			}
			else
			{
				Link<T> *temp = new Link<T>( obj );
				temp->setNext( walker );
				temp->setPrev( walker->getPrev() );
				walker->setPrev( temp );
			
				added = true;
			}
		}

		if( !added )
		{
			Link<T> *temp = new Link<T>( obj );
			mTail->setNext( temp );
			temp->setPrev( mTail );
		}

		mLength++;
	}
}

// PRE: This object, link are defined and numInsert describes the number of
//		parameters passed to the function.
// POST: The link is replaced by a new list of numInsert elements in its place
//		and link is deleted from the list.
template <class T> void List<T>::replace( Link<T> *link, int numInsert, ... )
{
	va_list arguments;
	va_start( arguments, numInsert );

	Link<T> *tPrev = link->getPrev();
	Link<T> *tNext = link->getNext();
	Link<T> *tListHead = 0;
	Link<T> *tListTail = 0;
	for( int i = 0; i < numInsert; i++ )
	{
		Link<T> *temp = new Link<T>( va_arg ( arguments, T ) );
		if( tListHead == 0 )
		{
			tListHead = temp;
			tListHead->setPrev( tPrev );	
			tListTail = tListHead;
		}
		else if( tListHead == tListTail )
		{
			tListTail = temp;
			tListHead->setNext( tListTail );
			tListTail->setPrev( tListHead );
			tListTail->setNext( 0 );
		}
		else
		{
			tListTail->setNext( temp );
			temp->setPrev( tListTail );
			tListTail = temp;
		}
	}

	tListTail->setNext( tNext );
	
	if( link->getPrev() == 0 )
		mHead = tListHead;
	else
		link->getPrev()->setNext( tListHead );

	if( link->getNext() == 0 )
		mTail = tListTail;

	delete link;
}

// PRE: This object is defined and index is a valid index into the list.
// POST: The RV is the link at that position or 0.
template <class T> Link<T> *List<T>::operator[]( int index )
{
	int position = 0;
	Link<T> *walker = mHead;
	
	while( walker != 0 && position < index )
	{
		walker = walker->getNext();
		position++;
	}

	return walker;	
}

template <class T> Link<T> *List<T>::get( int i )
{	
	int position = 0;
	Link<T> *walker = mHead;

	while( walker != 0 && position < i )
	{
		walker = walker->getNext();
		position++;
	}

	return walker;
}

template <class T> T List<T>::pop()
{
	T data;

	Link<T> *temp = mHead;
	
	if( temp )
	{
		mHead = mHead->getNext();

		if( mHead != 0 )
			mHead->setPrev( 0 );

		data = temp->getData();

		mLength--;
	}
	else
	{
		return NULL;
	}

	return data;
}

template <class T> T List<T>::peek()
{
	if( mHead != 0 )
		return mHead->getData();
	else
		return 0;
}

template <class T> Link<T> *List<T>::find( T needle, bool (*match)( T a, T b ) )
{
	Link<T> *temp = mHead;
	bool tFound = false;

	while( temp != 0 && !tFound )
	{
		if( match( needle, temp->getData() ) )
			tFound = true;
		else
			temp = temp->getNext();
	}

	return temp;
}

#include <iostream>

template <class T> void List<T>::deleteOne( T needle, bool (*match)( T a, T b ) )
{
	Link<T> *walker = mHead;

	while( walker != 0 )
	{
		if( match( needle, walker->getData() ) )
		{
			if( walker == mHead )
				mHead = walker->getNext();
			else if( walker == mTail )
			{
				mTail = walker->getPrev();
				mTail->setNext(walker->getNext() );
			}
			else
			{
				if( walker->getPrev() )
					walker->getPrev()->setNext( walker->getNext() );

				if( walker->getNext() )
					walker->getNext()->setPrev( walker->getPrev() );

			}

			walker = 0;

			mLength--;
		}
		else
			walker = walker->getNext();
	}
}

template <class T> Link<T> *List<T>::merge( Link<T> * (*func)( Link<T> *a, Link<T> *b ) )
{
	Link<T> *walker = mHead;

	while( walker != 0 )
	{
		if( walker->getNext() != 0 )
			walker = func( walker, walker->getNext() );
		walker = walker->getNext();
	}
}

template <class T> void List<T>::map( void (*func)( Link<T> *a ) )
{
	Link<T> *walker = mHead;

	while( walker != 0 )
	{
		func( walker );
		walker = walker->getNext();
	}
}

#endif
