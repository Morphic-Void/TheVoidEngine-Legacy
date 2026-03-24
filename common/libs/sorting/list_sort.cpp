
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   list_sort.cpp
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
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "list_sort.h"
#include "libs/system/debug/asserts.h"

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
////    String compare helper function
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int StringCompare( const char* const string1, const char* const string2, const uint32_t first_char_index )
{
	int char1 = 0;
	int char2 = 0;
	if( string1 && string2 )
	{
		uint32_t index = first_char_index;
		do
		{
			char1 = static_cast< unsigned char >( string1[ index ] );
			char2 = static_cast< unsigned char >( string2[ index ] );
			char1 -= char2;
			++index;
		}
		while( ( char1 == 0 ) && ( char2 != 0 ) );
	}
	else if( string1 )
	{
		char1 = 1;
	}
	else if( string2 )
	{
		char1 = -1;
	}
	return( char1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    In-place list sorting functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///	strings

///	MSL radix sorting of strings (private recursive worker function)
void RecurseRadixSortStrings( const char** strings, uint32_t count, uint32_t* blocks, uint32_t character = 0 )
{
	uint32_t index;
	uint32_t* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index )
	{
		const char* token = strings[ index ];
		++counts[ token ? static_cast< uint8_t >( token[ character ] ) : 0 ];
	}
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	uint32_t limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( uint32_t block = blocks[ index ]; block != limit; ++block )
		{
			uint32_t radix;
			const char* token = strings[ block ];
			while( ( radix = ( token ? static_cast< uint8_t >( token[ character ] ) : 0 ) ) != index )
			{
				const char* cycle = strings[ blocks[ radix ] ];
				strings[ blocks[ radix ] ] = token;
				token = cycle;
				++blocks[ radix ];
			}
			strings[ block ] = token;
		}
	}
	++character;
	strings += counts[ 0 ];
	if( character < 4 )
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 )
			{
				RecurseRadixSortStrings( strings, count, blocks, character );
			}
			else if( count > 1 )
			{
				BinaryInsertSortStrings( strings, count );
			}
			strings += count;
		}
	}
	else
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 1 ) 
			{
				BinaryInsertSortStrings( strings, count );
			}
			strings += count;
		}
	}
}

///	MSL radix sorting of strings
void RadixSortStrings( const char** const strings, const uint32_t count )
{
	ASSERT( strings );
	uint32_t workspace[ 256 * 5 ];
	if( count > 32 ) 
	{
		RecurseRadixSortStrings( strings, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSortStrings( strings, count );
	}
}

///	binary insert sorting of strings
void BinaryInsertSortStrings( const char** const strings, const uint32_t count )
{
	BinaryInsertAppendStrings( strings, 0, count );
}

///	binary insert appending of strings
void BinaryInsertAppendStrings( const char** const strings, const uint32_t count, const uint32_t append )
{
	ASSERT( strings );
	uint32_t index = ( count ? count : 1 );
	uint32_t limit = ( count + append );
	while( index < limit )
	{
		const char* string = strings[ index ];
		uint32_t upper = ( index - 1 );
		if( StringCompare( string, strings[ upper ] ) < 0 )
		{
			uint32_t lower = 0;
			while( lower < upper )
			{
				uint32_t check = ( ( lower + upper ) >> 1 );
				if( StringCompare( string, strings[ check ] ) >= 0 )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			for( upper = index; upper > lower; --upper ) strings[ upper ] = strings[ upper - 1 ];
			strings[ upper ] = string;
		}
		++index;
	}
}

///	binary search for string (returns index of string or -1)
int BinarySearchForString( const char* const string, const char** const strings, const uint32_t count )
{
	ASSERT( string && strings && count );
	uint32_t lower = 0;
	uint32_t upper = ( count - 1 );
	while( lower < upper )
	{
		uint32_t check = ( ( lower + upper ) >> 1 );
		if( StringCompare( string, strings[ check ] ) > 0 )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( StringCompare( string, strings[ lower ] ) ? -1 : static_cast< int >( lower ) );
}

///	indexed strings

///	MSL radix sorting of strings (private recursive worker function)
void RecurseRadixSortIndexedStrings( uint32_t* indices, const char* const* const strings, uint32_t count, uint32_t* blocks, uint32_t character = 0 )
{
	uint32_t index;
	uint32_t* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index )
	{
		uint32_t token = indices[ index ];
		++counts[ strings[ token ] ? static_cast< uint8_t >( strings[ token ][ character ] ) : 0 ];
	}
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	uint32_t limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( uint32_t block = blocks[ index ]; block != limit; ++block )
		{
			uint32_t radix;
			uint32_t token = indices[ block ];
			while( ( radix = ( strings[ token ] ? static_cast< uint8_t >( strings[ token ][ character ] ) : 0 ) ) != index )
			{
				uint32_t cycle = indices[ blocks[ radix ] ];
				indices[ blocks[ radix ] ] = token;
				token = cycle;
				++blocks[ radix ];
			}
			indices[ block ] = token;
		}
	}
	++character;
	indices += counts[ 0 ];
	if( character < 4 )
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 )
			{
				RecurseRadixSortIndexedStrings( indices, strings, count, blocks, character );
			}
			else if( count > 1 )
			{
				BinaryInsertSortIndexedStrings( indices, strings, count );
			}
			indices += count;
		}
	}
	else
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 1 ) 
			{
				BinaryInsertSortIndexedStrings( indices, strings, count );
			}
			indices += count;
		}
	}
}

///	MSL radix sorting of indexed strings
void RadixSortIndexedStrings( uint32_t* const indices, const char* const* const strings, const uint32_t count )
{
	ASSERT( indices && strings );
	uint32_t workspace[ 256 * 5 ];
	if( count > 32 ) 
	{
		RecurseRadixSortIndexedStrings( indices, strings, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSortIndexedStrings( indices, strings, count );
	}
}

///	binary insert sorting of indexed strings
void BinaryInsertSortIndexedStrings( uint32_t* const indices, const char* const* const strings, const uint32_t count )
{
	BinaryInsertAppendIndexedStrings( indices, strings, 0, count );
}

///	binary insert appending of indexed strings
void BinaryInsertAppendIndexedStrings( uint32_t* const indices, const char* const* const strings, const uint32_t count, const uint32_t append )
{
	ASSERT( indices && strings );
	uint32_t index = count;
	uint32_t limit = ( count + append );
	if( index == 0 )
	{
		indices[ 0 ] = 0;
		++index;
	}
	while( index < limit )
	{
		indices[ index ] = index;
		const char* const string = strings[ index ];
		uint32_t upper = ( index - 1 );
		if( StringCompare( string, strings[ indices[ upper ] ] ) < 0 )
		{
			uint32_t lower = 0;
			while( lower < upper )
			{
				uint32_t check = ( ( lower + upper ) >> 1 );
				if( StringCompare( string, strings[ indices[ check ] ] ) >= 0 )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			for( upper = index; upper > lower; --upper ) indices[ upper ] = indices[ upper - 1 ];
			indices[ upper ] = index;
		}
		++index;
	}
}

///	binary search for indexed string (returns index of index or -1)
int BinarySearchForIndexedString( const char* const string, const uint32_t* const indices, const char* const* const strings, const uint32_t count )
{
	ASSERT( string && indices && strings && count );
	uint32_t lower = 0;
	uint32_t upper = ( count - 1 );
	while( lower < upper )
	{
		uint32_t check = ( ( lower + upper ) >> 1 );
		if( StringCompare( string, strings[ indices[ check ] ] ) > 0 )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( StringCompare( string, strings[ indices[ lower ] ] ) ? -1 : static_cast< int >( lower ) );
}

///	string offsets

///	MSL radix sorting of string offsets (private recursive worker function)
void RecurseRadixSortStringOffsets( uint32_t* offsets, const char* const strings, uint32_t count, uint32_t* blocks, uint32_t character = 0 )
{
	uint32_t index;
	uint32_t* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index ) ++counts[ static_cast< uint8_t >( strings[ offsets[ index ] + character ] ) ];
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	uint32_t limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( uint32_t block = blocks[ index ]; block != limit; ++block )
		{
			uint32_t radix;
			uint32_t token = offsets[ block ];
			while( ( radix = static_cast< uint8_t >( strings[ token + character ] ) ) != index )
			{
				uint32_t cycle = offsets[ blocks[ radix ] ];
				offsets[ blocks[ radix ] ] = token;
				token = cycle;
				++blocks[ radix ];
			}
			offsets[ block ] = token;
		}
	}
	++character;
	offsets += counts[ 0 ];
	if( character < 4 )
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 )
			{
				RecurseRadixSortStringOffsets( offsets, strings, count, blocks, character );
			}
			else if( count > 1 )
			{
				BinaryInsertSortStringOffsets( offsets, strings, count );
			}
			offsets += count;
		}
	}
	else
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 1 ) 
			{
				BinaryInsertSortStringOffsets( offsets, strings, count );
			}
			offsets += count;
		}
	}
}

///	MSL radix sorting of string offsets
void RadixSortStringOffsets( uint32_t* const offsets, const char* const strings, const uint32_t count )
{
	ASSERT( offsets && strings );
	uint32_t workspace[ 256 * 5 ];
	if( count > 32 ) 
	{
		RecurseRadixSortStringOffsets( offsets, strings, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSortStringOffsets( offsets, strings, count );
	}
}

///	binary insert sorting of string offsets
void BinaryInsertSortStringOffsets( uint32_t* const offsets, const char* const strings, const uint32_t count )
{
	BinaryInsertAppendStringOffsets( offsets, strings, 0, count );
}

///	binary insert appending of string offsets
void BinaryInsertAppendStringOffsets( uint32_t* const offsets, const char* const strings, const uint32_t count, const uint32_t append )
{
	ASSERT( offsets && strings );
	uint32_t index = ( count ? count : 1 );
	uint32_t limit = ( count + append );
	while( index < limit )
	{
		uint32_t offset = offsets[ index ];
		const char* string = &strings[ offset ];
		uint32_t upper = ( index - 1 );
		if( StringCompare( string, &strings[ offsets[ upper ] ] ) < 0 )
		{
			uint32_t lower = 0;
			while( lower < upper )
			{
				uint32_t check = ( ( lower + upper ) >> 1 );
				if( StringCompare( string, &strings[ offsets[ check ] ] ) >= 0 )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			for( upper = index; upper > lower; --upper ) offsets[ upper ] = offsets[ upper - 1 ];
			offsets[ upper ] = offset;
		}
		++index;
	}
}

///	binary search for string offset (returns index of offset or -1)
int BinarySearchForStringOffset( const char* const string, const uint32_t* const offsets, const char* const strings, const uint32_t count )
{
	ASSERT( string && offsets && strings && count );
	uint32_t lower = 0;
	uint32_t upper = ( count - 1 );
	while( lower < upper )
	{
		uint32_t check = ( ( lower + upper ) >> 1 );
		if( StringCompare( string, &strings[ offsets[ check ] ] ) > 0 )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( StringCompare( string, &strings[ offsets[ lower ] ] ) ? -1 : static_cast< int >( lower ) );
}

///	indexed string offsets

///	MSL radix sorting of indexed string offsets (private recursive worker function)
void RecurseRadixSortIndexedStringOffsets( uint32_t* indices, const uint32_t* const offsets, const char* const strings, uint32_t count, uint32_t* blocks, uint32_t character = 0 )
{
	uint32_t index;
	uint32_t* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index ) ++counts[ static_cast< uint8_t >( strings[ offsets[ indices[ index ] ] + character ] ) ];
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	uint32_t limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( uint32_t block = blocks[ index ]; block != limit; ++block )
		{
			uint32_t radix;
			uint32_t token = indices[ block ];
			while( ( radix = static_cast< uint8_t >( strings[ offsets[ token ] + character ] ) ) != index )
			{
				uint32_t cycle = indices[ blocks[ radix ] ];
				indices[ blocks[ radix ] ] = token;
				token = cycle;
				++blocks[ radix ];
			}
			indices[ block ] = token;
		}
	}
	++character;
	indices += counts[ 0 ];
	if( character < 4 )
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 )
			{
				RecurseRadixSortIndexedStringOffsets( indices, offsets, strings, count, blocks, character );
			}
			else if( count > 1 )
			{
				BinaryInsertSortIndexedStringOffsets( indices, offsets, strings, count );
			}
			indices += count;
		}
	}
	else
	{
		for( index = 1; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 1 ) 
			{
				BinaryInsertSortIndexedStringOffsets( indices, offsets, strings, count );
			}
			indices += count;
		}
	}
}

///	MSL radix sorting of indexed string offsets
void RadixSortIndexedStringOffsets( uint32_t* const indices, const uint32_t* const offsets, const char* const strings, const uint32_t count )
{
	ASSERT( indices && offsets && strings );
	uint32_t workspace[ 256 * 5 ];
	if( count > 32 ) 
	{
		RecurseRadixSortIndexedStringOffsets( indices, offsets, strings, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSortIndexedStringOffsets( indices, offsets, strings, count );
	}
}

///	binary insert sorting of indexed string offsets
void BinaryInsertSortIndexedStringOffsets( uint32_t* const indices, const uint32_t* const offsets, const char* const strings, const uint32_t count )
{
	BinaryInsertAppendIndexedStringOffsets( indices, offsets, strings, 0, count );
}

///	binary insert appending of indexed string offsets
void BinaryInsertAppendIndexedStringOffsets( uint32_t* const indices, const uint32_t* const offsets, const char* const strings, const uint32_t count, const uint32_t append )
{
	ASSERT( indices && strings );
	uint32_t index = count;
	uint32_t limit = ( count + append );
	if( index == 0 )
	{
		indices[ 0 ] = 0;
		++index;
	}
	while( index < limit )
	{
		indices[ index ] = index;
		const char* const string = &strings[ offsets[ index ] ];
		uint32_t upper = ( index - 1 );
		if( StringCompare( string, &strings[ offsets[ indices[ upper ] ] ] ) < 0 )
		{
			uint32_t lower = 0;
			while( lower < upper )
			{
				uint32_t check = ( ( lower + upper ) >> 1 );
				if( StringCompare( string, &strings[ offsets[ indices[ check ] ] ] ) >= 0 )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			for( upper = index; upper > lower; --upper ) indices[ upper ] = indices[ upper - 1 ];
			indices[ upper ] = index;
		}
		++index;
	}
}

///	binary search for indexed string offset (returns index of index or -1)
int BinarySearchForIndexedStringOffset( const char* const string, const uint32_t* const indices, const uint32_t* const offsets, const char* const strings, const uint32_t count )
{
	ASSERT( string && indices && offsets && strings && count );
	uint32_t lower = 0;
	uint32_t upper = ( count - 1 );
	while( lower < upper )
	{
		uint32_t check = ( ( lower + upper ) >> 1 );
		if( StringCompare( string, &strings[ offsets[ indices[ check ] ] ] ) > 0 )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( StringCompare( string, &strings[ offsets[ indices[ lower ] ] ] ) ? -1 : static_cast< int >( lower ) );
}

///	values

///	MSL radix sorting of values (private recursive worker function)
void RecurseRadixSortValues( uint32_t* values, uint32_t count, uint32_t* blocks, uint32_t shift = 24 )
{
	uint32_t index;
	uint32_t* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index ) ++counts[ ( values[ index ] >> shift ) & 255 ];
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	uint32_t limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( uint32_t block = blocks[ index ]; block != limit; ++block )
		{
			uint32_t radix;
			uint32_t token = values[ block ];
			while( ( radix = ( ( token >> shift ) & 255 ) ) != index )
			{
				uint32_t cycle = values[ blocks[ radix ] ];
				values[ blocks[ radix ] ] = token;
				token = cycle;
				++blocks[ radix ];
			}
			values[ block ] = token;
		}
	}
	if( shift )
	{
		shift -= 8;
		for( index = 0; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 ) 
			{
				RecurseRadixSortValues( values, count, blocks, shift );
			}
			else if( count > 1 )
			{
				BinaryInsertSortValues( values, count );
			}
			values += count;
		}
	}
}

///	MSL radix sorting of values
void RadixSortValues( uint32_t* const values, const uint32_t count )
{
	ASSERT( values );
	uint32_t workspace[ 256 * 5 ];
	if( count > 32 ) 
	{
		RecurseRadixSortValues( values, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSortValues( values, count );
	}
}

///	LSD (stable) radix sorting of values (buffer must be at least same size as values array)
void RadixSortValues( uint32_t* const values, const uint32_t count, uint32_t* const buffer )
{
	ASSERT( values && buffer );
	uint32_t workspace[ 257 * 4 ];
	uint32_t* blocks = &workspace[ 257 * 4 ];
	for( uint32_t clear = 257; clear; --clear )
	{
		blocks -= 4;
		blocks[ 0 ] = blocks[ 1 ] = blocks[ 2 ] = blocks[ 3 ] = 0;
	}
	++blocks;
	for( uint32_t index = 0; index < count; ++index )
	{
		uint32_t value = values[ index ];
		++blocks[ value & 255 ];
		++blocks[ ( ( value >> 8 ) & 255 ) + 257 ];
		++blocks[ ( ( value >> 16 ) & 255 ) + ( 257 * 2 ) ];
		++blocks[ ( value >> 24 ) + ( 257 * 3 ) ];
	}
	--blocks;
	for( uint32_t index = 2; index < 256; ++index )
	{
		blocks[ index ] += blocks[ index - 1 ];
		blocks[ index + 257 ] += blocks[ index + 257 - 1 ];
		blocks[ index + ( 257 * 2 ) ] += blocks[ index + ( 257 * 2 ) - 1 ];
		blocks[ index + ( 257 * 3 ) ] += blocks[ index + ( 257 * 3 ) - 1 ];
	}
	uint32_t* src = values;
	uint32_t* dst = buffer;
	for( uint32_t shift = 0; shift < 32; shift += 8 )
	{
		for( uint32_t index = 0; index < count; ++index )
		{
			uint32_t token = src[ index ];
			uint32_t radix = ( ( token >> shift ) & 255 );
			dst[ blocks[ radix ] ] = token;
			++blocks[ radix ];
		}
		uint32_t* tmp = src;
		src = dst;
		dst = tmp;
		blocks += 257;
	}
}

///	binary insert sorting of values
void BinaryInsertSortValues( uint32_t* const values, const uint32_t count )
{
	BinaryInsertAppendValues( values, 0, count );
}

///	binary insert appending of values
void BinaryInsertAppendValues( uint32_t* const values, const uint32_t count, const uint32_t append )
{
	ASSERT( values );
	uint32_t index = ( count ? count : 1 );
	uint32_t limit = ( count + append );
	while( index < limit )
	{
		uint32_t value = values[ index ];
		uint32_t upper = ( index - 1 );
		if( value < values[ upper ] )
		{
			uint32_t lower = 0;
			while( lower < upper )
			{
				uint32_t check = ( ( lower + upper ) >> 1 );
				if( value >= values[ check ] )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			for( upper = index; upper > lower; --upper ) values[ upper ] = values[ upper - 1 ];
			values[ upper ] = value;
		}
		++index;
	}
}

///	binary search for value (returns index of value or -1)
int BinarySearchForValue( const uint32_t value, const uint32_t* const values, const uint32_t count )
{
	ASSERT( values && count );
	uint32_t lower = 0;
	uint32_t upper = ( count - 1 );
	while( lower < upper )
	{
		uint32_t check = ( ( lower + upper ) >> 1 );
		if( value > values[ check ] )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( ( value == values[ lower ] ) ? static_cast< int >( lower ) : -1 );
}

///	indexed values

///	MSL radix sorting of indexed values (private recursive worker function)
void RecurseRadixSortIndexedValues( uint32_t* indices, const uint32_t* const values, uint32_t count, uint32_t* blocks, uint32_t shift = 24 )
{
	uint32_t index;
	uint32_t* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index ) ++counts[ ( values[ indices[ index ] ] >> shift ) & 255 ];
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	uint32_t limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( uint32_t block = blocks[ index ]; block != limit; ++block )
		{
			uint32_t radix;
			uint32_t token = indices[ block ];
			while( ( radix = ( ( values[ token ] >> shift ) & 255 ) ) != index )
			{
				uint32_t cycle = indices[ blocks[ radix ] ];
				indices[ blocks[ radix ] ] = token;
				token = cycle;
				++blocks[ radix ];
			}
			indices[ block ] = token;
		}
	}
	if( shift )
	{
		shift -= 8;
		for( index = 0; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 ) 
			{
				RecurseRadixSortIndexedValues( indices, values, count, blocks, shift );
			}
			else if( count > 1 )
			{
				BinaryInsertSortIndexedValues( indices, values, count );
			}
			indices += count;
		}
	}
}

///	MSL radix sorting of indexed values
void RadixSortIndexedValues( uint32_t* const indices, const uint32_t* const values, const uint32_t count )
{
	ASSERT( indices && values );
	uint32_t workspace[ 256 * 5 ];
	if( count > 32 ) 
	{
		RecurseRadixSortIndexedValues( indices, values, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSortIndexedValues( indices, values, count );
	}
}

///	LSD (stable) radix sorting of indexed values (buffer must be at least same size as indices array)
void RadixSortIndexedValues( uint32_t* const indices, const uint32_t* const values, const uint32_t count, uint32_t* const buffer )
{
	ASSERT( values && buffer );
	uint32_t workspace[ 257 * 4 ];
	uint32_t* blocks = &workspace[ 257 * 4 ];
	for( uint32_t clear = 257; clear; --clear )
	{
		blocks -= 4;
		blocks[ 0 ] = blocks[ 1 ] = blocks[ 2 ] = blocks[ 3 ] = 0;
	}
	++blocks;
	for( uint32_t index = 0; index < count; ++index )
	{
		uint32_t value = values[ indices[ index ] ];
		++blocks[ value & 255 ];
		++blocks[ ( ( value >> 8 ) & 255 ) + 257 ];
		++blocks[ ( ( value >> 16 ) & 255 ) + ( 257 * 2 ) ];
		++blocks[ ( value >> 24 ) + ( 257 * 3 ) ];
	}
	--blocks;
	for( uint32_t index = 2; index < 256; ++index )
	{
		blocks[ index ] += blocks[ index - 1 ];
		blocks[ index + 257 ] += blocks[ index + 257 - 1 ];
		blocks[ index + ( 257 * 2 ) ] += blocks[ index + ( 257 * 2 ) - 1 ];
		blocks[ index + ( 257 * 3 ) ] += blocks[ index + ( 257 * 3 ) - 1 ];
	}
	uint32_t* src = indices;
	uint32_t* dst = buffer;
	for( uint32_t shift = 0; shift < 32; shift += 8 )
	{
		for( uint32_t index = 0; index < count; ++index )
		{
			uint32_t token = src[ index ];
			uint32_t radix = ( ( values[ token ] >> shift ) & 255 );
			dst[ blocks[ radix ] ] = token;
			++blocks[ radix ];
		}
		uint32_t* tmp = src;
		src = dst;
		dst = tmp;
		blocks += 257;
	}
}

///	binary insert sorting of indexed values
void BinaryInsertSortIndexedValues( uint32_t* const indices, const uint32_t* const values, const uint32_t count )
{
	BinaryInsertAppendIndexedValues( indices, values, 0, count );
}

///	binary insert appending of indexed values
void BinaryInsertAppendIndexedValues( uint32_t* const indices, const uint32_t* const values, const uint32_t count, const uint32_t append )
{
	ASSERT( indices && values );
	uint32_t index = count;
	uint32_t limit = ( count + append );
	if( index == 0 )
	{
		indices[ 0 ] = 0;
		++index;
	}
	while( index < limit )
	{
		indices[ index ] = index;
		uint32_t value = values[ index ];
		uint32_t upper = ( index - 1 );
		if( value < values[ indices[ upper ] ] )
		{
			uint32_t lower = 0;
			while( lower < upper )
			{
				uint32_t check = ( ( lower + upper ) >> 1 );
				if( value >= values[ indices[ check ] ] )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			for( upper = index; upper > lower; --upper ) indices[ upper ] = indices[ upper - 1 ];
			indices[ upper ] = index;
		}
		++index;
	}
}

///	binary search for indexed value (returns index of index or -1)
int BinarySearchForIndexedValue( const uint32_t value, const uint32_t* const indices, const uint32_t* const values, const uint32_t count )
{
	ASSERT( indices && values && count );
	uint32_t lower = 0;
	uint32_t upper = ( count - 1 );
	while( lower < upper )
	{
		uint32_t check = ( ( lower + upper ) >> 1 );
		if( value > values[ indices[ check ] ] )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( ( value == values[ indices[ lower ] ] ) ? static_cast< int >( lower ) : -1 );
}

///	indirect values

///	MSL radix sorting of indirect values (private recursive worker function)
void RecurseRadixSortIndirectValues( const uint32_t** values, uint32_t count, uint32_t* blocks, uint32_t shift = 24 )
{
	uint32_t index;
	uint32_t* counts = blocks;
	blocks += 256;
	for( index = 0; index < 257; ++index ) counts[ index ] = 0;
	for( index = 0; index < count; ++index ) ++counts[ ( *values[ index ] >> shift ) & 255 ];
	for( index = 0; index < 255; ++index ) blocks[ index + 1 ] = ( blocks[ index ] + counts[ index ] );
	uint32_t limit = 0;
	for( index = 0; index < 256; ++index )
	{
		limit += counts[ index ];
		for( uint32_t block = blocks[ index ]; block != limit; ++block )
		{
			uint32_t radix;
			const uint32_t* token = values[ block ];
			while( ( radix = ( ( *token >> shift ) & 255 ) ) != index )
			{
				const uint32_t* cycle = values[ blocks[ radix ] ];
				values[ blocks[ radix ] ] = token;
				token = cycle;
				++blocks[ radix ];
			}
			values[ block ] = token;
		}
	}
	if( shift )
	{
		shift -= 8;
		for( index = 0; index < 256; ++index )
		{
			count = counts[ index ];
			if( count > 32 ) 
			{
				RecurseRadixSortIndirectValues( values, count, blocks, shift );
			}
			else if( count > 1 )
			{
				BinaryInsertSortIndirectValues( values, count );
			}
			values += count;
		}
	}
}

///	MSL radix sorting of indirect values
void RadixSortIndirectValues( const uint32_t** const values, const uint32_t count )
{
	ASSERT( values );
	uint32_t workspace[ 256 * 5 ];
	if( count > 32 ) 
	{
		RecurseRadixSortIndirectValues( values, count, workspace );
	}
	else if( count > 1 )
	{
		BinaryInsertSortIndirectValues( values, count );
	}
}

///	LSD (stable) radix sorting of indirect values (buffer must be at least same size as values array)
void RadixSortIndirectValues( const uint32_t** const values, const uint32_t count, const uint32_t** const buffer )
{
	ASSERT( values && buffer );
	uint32_t workspace[ 257 * 4 ];
	uint32_t* blocks = &workspace[ 257 * 4 ];
	for( uint32_t clear = 257; clear; --clear )
	{
		blocks -= 4;
		blocks[ 0 ] = blocks[ 1 ] = blocks[ 2 ] = blocks[ 3 ] = 0;
	}
	++blocks;
	for( uint32_t index = 0; index < count; ++index )
	{
		uint32_t value = *values[ index ];
		++blocks[ value & 255 ];
		++blocks[ ( ( value >> 8 ) & 255 ) + 257 ];
		++blocks[ ( ( value >> 16 ) & 255 ) + ( 257 * 2 ) ];
		++blocks[ ( value >> 24 ) + ( 257 * 3 ) ];
	}
	--blocks;
	for( uint32_t index = 2; index < 256; ++index )
	{
		blocks[ index ] += blocks[ index - 1 ];
		blocks[ index + 257 ] += blocks[ index + 257 - 1 ];
		blocks[ index + ( 257 * 2 ) ] += blocks[ index + ( 257 * 2 ) - 1 ];
		blocks[ index + ( 257 * 3 ) ] += blocks[ index + ( 257 * 3 ) - 1 ];
	}
	const uint32_t** src = values;
	const uint32_t** dst = buffer;
	for( uint32_t shift = 0; shift < 32; shift += 8 )
	{
		for( uint32_t index = 0; index < count; ++index )
		{
			const uint32_t* token = src[ index ];
			uint32_t radix = ( ( *token >> shift ) & 255 );
			dst[ blocks[ radix ] ] = token;
			++blocks[ radix ];
		}
		const uint32_t** tmp = src;
		src = dst;
		dst = tmp;
		blocks += 257;
	}
}

///	binary insert sorting of indirect values
void BinaryInsertSortIndirectValues( const uint32_t** const values, const uint32_t count )
{
	BinaryInsertAppendIndirectValues( values, 0, count );
}

///	binary insert appending of indirect values
void BinaryInsertAppendIndirectValues( const uint32_t** const values, const uint32_t count, const uint32_t append )
{
	ASSERT( values );
	uint32_t index = ( count ? count : 1 );
	uint32_t limit = ( count + append );
	while( index < limit )
	{
		const uint32_t* ivalue = values[ index ];
		uint32_t value = *ivalue;
		uint32_t upper = ( index - 1 );
		if( value < *values[ upper ] )
		{
			uint32_t lower = 0;
			while( lower < upper )
			{
				uint32_t check = ( ( lower + upper ) >> 1 );
				if( value >= *values[ check ] )
				{
					lower = ( check + 1 );
				}
				else
				{
					upper = check;
				}
			}
			for( upper = index; upper > lower; --upper ) values[ upper ] = values[ upper - 1 ];
			values[ upper ] = ivalue;
		}
		++index;
	}
}

///	binary search for indirect value (returns index of value or -1)
int BinarySearchForIndirectValue( const uint32_t value, const uint32_t* const* const values, const uint32_t count )
{
	ASSERT( values && count );
	uint32_t lower = 0;
	uint32_t upper = ( count - 1 );
	while( lower < upper )
	{
		uint32_t check = ( ( lower + upper ) >> 1 );
		if( value > *values[ check ] )
		{
			lower = ( check + 1 );
		}
		else
		{
			upper = check;
		}
	}
	return( ( value == *values[ lower ] ) ? static_cast< int >( lower ) : -1 );
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
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

