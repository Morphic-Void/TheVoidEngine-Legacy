
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   list_sort.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	In-place list sorting functions.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard begin
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef	__TLIST_SORT_INCLUDED__
#define	__TLIST_SORT_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "list_sort.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin sorting namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sorting
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Templated in-place list sorting classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///	strings
template< typename TItem >
class SortableByString
{
public:
	inline			SortableByString() {};
	virtual			~SortableByString() = 0;
	const char*		GetString( const TItem& item ) const = 0;
	void			RadixSort( TItem* const items, const unsigned int count ) const;
	inline void		BinaryInsertSort( TItem* const items, const unsigned int count ) const { BinaryInsertAppend( items, 0, count ); };
	void			BinaryInsertAppend( TItem* const items, const unsigned int count, const unsigned int append ) const;
	int				BinarySearch( const char* const string, const TItem* const items, const unsigned int count ) const;
protected:
	void			RecurseRadixSort( TItem* items, unsigned int count, unsigned int* blocks, unsigned int character = 0 ) const;
};

///	values (TValue must be a shiftable type)
template< typename TItem, typename TValue >
class SortableByValue
{
public:
	inline			SortableByValue() {};
	virtual			~SortableByValue() = 0;
	TValue			GetValue( const TItem& item ) const = 0;
	void			RadixSort( TItem* const items, const unsigned int count ) const;
	void			RadixSort( TItem* const items, const unsigned int count, TItem* const buffer ) const;
	inline void		BinaryInsertSort( TItem* const items, const unsigned int count ) const { BinaryInsertAppend( items, 0, count ); };
	void			BinaryInsertAppend( TItem* const items, const unsigned int count, const unsigned int append ) const;
	int				BinarySearch( const TValue value, const TItem* const items, const unsigned int count ) const;
protected:
	void			RecurseRadixSort( TItem* items, unsigned int count, unsigned int* blocks, unsigned int shift = static_cast< unsigned int >( sizeof( TValue ) << 3 ) ) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    SortableByString template class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///	strings
template< typename TItem >
SortableByString<TItem>::~SortableByString() {};

///	MSL radix sorting by string (private recursive worker function)
template< typename TItem >
void SortableByString<TItem>::RecurseRadixSort( TItem* items, unsigned int count, unsigned int* blocks, unsigned int character ) const
{
	unsigned int index;
	const char* string;
	unsigned int* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index )
	{
		string = GetString( items[ index ] );
		++counts[ string ? static_cast< uint8_t >( string[ character ] ) : 0 ];
	}
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	unsigned int limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( unsigned int block = blocks[ index ]; block != limit; ++block )
		{
			unsigned int radix;
			TItem item = items[ block ];
			while( ( radix = ( ( string = GetString( item ) ) ? static_cast< uint8_t >( string[ character ] ) : 0 ) ) != index )
			{
				TItem swap = items[ blocks[ radix ] ];
				items[ blocks[ radix ] ] = item;
				item = swap;
				++blocks[ radix ];
			}
			items[ block ] = item;
		}
	}
	++character;
	items += counts[ 0 ];
	if( character < 4 )
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 )
			{
				RecurseRadixSort( items, count, blocks, character );
			}
			else if( count > 1 )
			{
				BinaryInsertSort( items, count );
			}
			items += count;
		}
	}
	else
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 1 ) 
			{
				BinaryInsertSort( items, count );
			}
			items += count;
		}
	}
}

///	MSL radix sorting by string
template< typename TItem >
void SortableByString<TItem>::RadixSort( TItem* const items, const unsigned int count ) const
{
	ASSERT( items );
	unsigned int workspace[ 256 * 5 ];
	if( count > 32 ) 
	{
		RecurseRadixSort( items, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSort<TItem>( items, count );
	}
}

///	binary insert appending by string
template< typename TItem >
void SortableByString<TItem>::BinaryInsertAppend( TItem* const items, const unsigned int count, const unsigned int append ) const
{
	ASSERT( items );
	unsigned int index = ( count ? count : 1 );
	unsigned int limit = ( count + append );
	while( index < limit )
	{
		const char* string = GetString( items[ index ] )
		unsigned int upper = ( index - 1 );
		if( StringCompare( string, GetString( items[ upper ] ) ) < 0 )
		{
			unsigned int lower = 0;
			while( lower < upper )
			{
				unsigned int check = ( ( lower + upper ) >> 1 );
				if( StringCompare( string, GetString( tokens[ check ] ) ) >= 0 )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			TItem swap = tokens[ index ];
			for( upper = index; upper > lower; --upper ) tokens[ upper ] = tokens[ upper - 1 ];
			tokens[ upper ] = swap;
		}
		++index;
	}
}

///	binary search by string (returns index of item or -1)
template< typename TItem >
int SortableByString<TItem>::BinarySearch( const char* const string, const TItem* const items, const unsigned int count ) const
{
	ASSERT( string && items && count );
	unsigned int lower = 0;
	unsigned int upper = ( count - 1 );
	while( lower < upper )
	{
		unsigned int check = ( ( lower + upper ) >> 1 );
		if( StringCompare( string, GetString( items[ check ] ) ) > 0 )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( StringCompare( string, GetString( items[ lower ] ) ) ? -1 : static_cast< int >( lower ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    SortableByValue template class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///	values
template< typename TItem, typename TValue >
SortableByValue<TItem,TValue>::~SortableByValue() {};

///	MSL radix sorting of indirect values (private recursive worker function)
template< typename TItem, typename TValue >
void SortableByValue<TItem,TValue>::RecurseRadixSort( TItem* items, unsigned int count, unsigned int* blocks, unsigned int shift ) const
{
	shift -= 8;
	unsigned int index;
	unsigned int* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index ) ++counts[ ( GetValue( items[ index ] ) >> shift ) & 255 ];
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	unsigned int limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( unsigned int block = blocks[ index ]; block != limit; ++block )
		{
			unsigned int radix;
			TItem item = items[ block ];
			while( ( radix = static_cast< unsigned int >( ( GetValue( item ) >> shift ) & 255 ) ) != index )
			{
				TItem swap = items[ blocks[ radix ] ];
				items[ blocks[ radix ] ] = item;
				item = swap;
				++blocks[ radix ];
			}
			items[ block ] = item;
		}
	}
	if( shift )
	{
		for( index = 0; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 ) 
			{
				RecurseRadixSort( items, count, blocks, shift );
			}
			else if( count > 1 )
			{
				BinaryInsertSort( items, count );
			}
			items += count;
		}
	}
}

///	MSL radix sorting of indirect values
template< typename TItem, typename TValue >
void SortableByValue<TItem,TValue>::RadixSort( TItem* const items, const unsigned int count ) const
{
	ASSERT( values );
	unsigned int workspace[ 256 * ( sizeof( TValue ) + 1 ) ];
	if( count > 32 ) 
	{
		RecurseRadixSort( items, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSort( items, count );
	}
}

///	LSD (stable) radix sorting of indirect values (buffer must be at least same size as items array)
template< typename TItem, typename TValue >
void SortableByValue<TItem,TValue>::RadixSort( TItem* const items, const unsigned int count, TItem* const buffer ) const
{
	ASSERT( items && buffer );
	static const unsigned int bytes = static_cast< unsigned int >( sizeof( TValue ) );
	static const unsigned int bits = ( bytes << 3 );
	static const unsigned int work = ( bytes * 257 );
	unsigned int workspace[ sizeof( TValue ) * 257 ];
	unsigned int* blocks = &workspace[ 0 ];
	for( unsigned int clear = 0; clear < work; ++clear )
	{
		blocks[ clear ] = 0;
	}
	++blocks;
	for( unsigned int index = 0; index < count; ++index )
	{
		TValue value = GetValue( items[ index ] );
		for( unsigned int byte = 0; byte < bytes; ++byte )
		{
			++blocks[ static_cast< unsigned int >( value & 255 ) ];
			blocks += 257;
			value >>= 8;
		}
		blocks -= work;
	}
	--blocks;
	for( unsigned int byte = 0; byte < bytes; ++byte )
	{
		for( unsigned int index = 2; index < 256; ++index )
		{
			blocks[ index ] += blocks[ index - 1 ];
		}
		blocks += 257;
	}
	blocks -= work;
	const TItem* src = items;
	const TItem* dst = buffer;
	for( unsigned int shift = 0; shift < bits; shift += 8 )
	{
		for( unsigned int index = 0; index < count; ++index )
		{
			TItem item = src[ index ];
			unsigned int radix = static_cast< unsigned int >( ( GetValue( item ) >> shift ) & 255 );
			dst[ blocks[ radix ] ] = item;
			++blocks[ radix ];
		}
		const TItem* tmp = src;
		src = dst;
		dst = tmp;
		blocks += 257;
	}
	if( bytes & 1 )
	{
		for( unsigned int index = 0; index < count; ++index )
		{
			dst[ index ] = src[ index ];
		}
	}
}

///	binary insert appending of indirect values
template< typename TItem, typename TValue >
void SortableByValue<TItem,TValue>::BinaryInsertAppend( TItem* const items, const unsigned int count, const unsigned int append ) const
{
	ASSERT( items );
	unsigned int index = ( count ? count : 1 );
	unsigned int limit = ( count + append );
	while( index < limit )
	{
		TValue value = GetValue( items[ index ] );
		unsigned int upper = ( index - 1 );
		if( value < GetValue( items[ upper ] ) )
		{
			unsigned int lower = 0;
			while( lower < upper )
			{
				unsigned int check = ( ( lower + upper ) >> 1 );
				if( value >= GetValue( items[ check ] ) )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			TItem item = items[ index ];
			for( upper = index; upper > lower; --upper ) items[ upper ] = items[ upper - 1 ];
			items[ upper ] = item;
		}
		++index;
	}
}

///	binary search for indirect value (returns index of value or -1)
template< typename TItem, typename TValue >
int SortableByValue<TItem,TValue>::BinarySearch( const TValue value, const TItem* const items, const unsigned int count ) const
{
	ASSERT( items && count );
	unsigned int lower = 0;
	unsigned int upper = ( count - 1 );
	while( lower < upper )
	{
		unsigned int check = ( ( lower + upper ) >> 1 );
		if( value > GetValue( items[ check ] ) )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( ( value == GetValue( items[ lower ] ) ) ? static_cast< int >( lower ) : -1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end sorting namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace sorting

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__TLIST_SORT_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
