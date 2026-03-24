
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   sparse.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Sparse indices compression and decompression functions.
////
////    Notes:
////
////    	Because these functions are not intended for packing data with repeated values,
////    	caution must be exercised when packing arrays as the code for a zero delta is
////    	interpreted as an end of stream marker. 
////
////    	The Append, Insert and array packing functions ensure that the packed data
////    	has a zero delta terminator.
////
////    	All functions operate on the section of the spd_buffer ending at buffer[ length - 1 ].
////
////    	The Append and Insert functions operate on data starting at buffer[ 0 ]. All other
////    	functions operate only on the data starting at buffer[ offset ].
////
////    	A false return value from any function indicates a buffer underflow or overflow.
////
////    The encoding:
////
////    	The encoding is as a stream of variable byte length index deltas.
////
////    	byte[ 0 ] value 0:
////
////    		1 byte
////    		Indicates end of stream or a delta of 0
////
////    	byte[ 0 ] value 1-142:
////
////    		1 byte, approx 7-bit delta
////    		delta = byte[ 0 ]
////
////    	byte[ 0 ] value 143-206:
////
////    		2 bytes, approx 14-bit delta
////    		delta = byte[ 0 ] + ( byte[ 1 ] << 6 )
////
////    	byte[ 0 ] value 207-238:
////
////    		3 bytes, approx 21-bit delta
////    		delta = byte[ 0 ] + ( byte[ 1 ] << 5 ) + ( byte[ 2 ] << 13 )
////    			+ 0x00004000 + 143 - 207
////
////    	byte[ 0 ] value 239-254:
////
////    		4 bytes, approx 28-bit delta
////    		delta = byte[ 0 ] + ( byte[ 1 ] << 4 ) + ( byte[ 2 ] << 12 ) + ( byte[ 3 ] << 20 )
////    			+ 0x00204000 + 143 - 239
////
////    	byte[ 0 ] value 255:
////
////    		5 bytes, approx 32-bit delta
////    		delta = byte[ 1 ] + ( byte[ 2 ] << 8 ) + ( byte[ 3 ] << 16 ) + ( byte[ 4 ] << 24 )
////    			+ 0x10204000 + 143
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

#include "sparse.h"
#include "libs/system/debug/asserts.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin compression namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace compression
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin sparse namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sparse
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Sparse indices compression and decompression functions.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////    packed index array index count query

bool GetIndexCount( const spd_buffer& spd, unsigned int& count )
{
	spd_buffer tmp = spd;
	uint32_t delta = 0;
	for( unsigned int index = 0; Unpack( tmp, delta ); ++index )
	{
		if( !delta )
		{
			count = index;
			return( true );
		}
	}
	count = 0;
	return( false );
}

////    calculate packed index array size

unsigned int GetPackedSize( const uint32_t* const values, const unsigned int count )
{
	ASSERT( values );
	unsigned int size = 1;
	uint32_t delta = 0;
	for( unsigned int index = 0; index < count; ++index )
	{
		uint32_t value = values[ index ];
		size += GetPackedSize( value - delta );
		delta = value;
	}
	return( size );
}

unsigned int GetPackedSize( const uint16_t* const values, const unsigned int count )
{
	ASSERT( values );
	unsigned int size = 1;
	uint32_t delta = 0;
	for( unsigned int index = 0; index < count; ++index )
	{
		uint32_t value = values[ index ];
		size += GetPackedSize( value - delta );
		delta = value;
	}
	return( size );
}

unsigned int GetPackedSize( const uint8_t* const values, const unsigned int count )
{
	ASSERT( values );
	unsigned int size = 1;
	uint32_t delta = 0;
	for( unsigned int index = 0; index < count; ++index )
	{
		uint32_t value = values[ index ];
		size += GetPackedSize( value - delta );
		delta = value;
	}
	return( size );
}

unsigned int GetPackedSize( const uint32_t delta )
{
	if( delta < 0x0000008f )
	{	//	1 byte encoding
		return( 1 );
	}
	else if( delta < 0x0000408f )
	{	//	2 byte encoding
		return( 2 );
	}
	else if( delta < 0x0020408f )
	{	//	3 byte encoding
		return( 3 );
	}
	else if( delta < 0x1020408f )
	{	//	4 byte encoding
		return( 4 );
	}
	else
	{	//	5 byte encoding
		return( 5 );
	}
}

////    index array packing functions

bool Pack( spd_buffer& spd, const uint32_t* const values, const unsigned int count )
{
	ASSERT( values );
	uint32_t delta = 0;
	for( unsigned int index = 0; index < count; ++index )
	{
		uint32_t value = values[ index ];
		if( !Pack( spd, ( value - delta ) ) )
		{
			return( false );
		}
		delta = value;
	}
	return( Pack( spd, 0 ) );
}

bool Pack( spd_buffer& spd, const uint16_t* const values, const unsigned int count )
{
	ASSERT( values );
	uint32_t delta = 0;
	for( unsigned int index = 0; index < count; ++index )
	{
		uint32_t value = values[ index ];
		if( !Pack( spd, ( value - delta ) ) )
		{
			return( false );
		}
		delta = value;
	}
	return( Pack( spd, 0 ) );
}

bool Pack( spd_buffer& spd, const uint8_t* const values, const unsigned int count )
{
	ASSERT( values );
	uint32_t delta = 0;
	for( unsigned int index = 0; index < count; ++index )
	{
		uint32_t value = values[ index ];
		if( !Pack( spd, ( value - delta ) ) )
		{
			return( false );
		}
		delta = value;
	}
	return( Pack( spd, 0 ) );
}

bool Pack( spd_buffer& spd, const uint32_t delta )
{
	if( spd.offset >= spd.length )
	{	//	buffer overflow
		return( false );
	}
	if( delta < 0x0000008f )
	{	//	1 byte encoding
		spd.buffer[ spd.offset ] = static_cast< uint8_t >( delta );
		++spd.offset;
	}
	else if( delta < 0x0000408f )
	{	//	2 byte encoding
		if( ( spd.length - spd.offset ) < 2 )
		{	//	buffer overflow
			return( false );
		}
		uint32_t code = ( delta - 0x0000008f );
		spd.buffer[ spd.offset + 0 ] = static_cast< uint8_t >( ( code & 63 ) + 143 );
		spd.buffer[ spd.offset + 1 ] = static_cast< uint8_t >( code >> 6 );
		spd.offset += 2;
	}
	else if( delta < 0x0020408f )
	{	//	3 byte encoding
		if( ( spd.length - spd.offset ) < 3 )
		{	//	buffer overflow
			return( false );
		}
		uint32_t code = ( delta - 0x0000408f );
		spd.buffer[ spd.offset + 0 ] = static_cast< uint8_t >( ( code & 31 ) + 207 );
		spd.buffer[ spd.offset + 1 ] = static_cast< uint8_t >( code >> 5 );
		spd.buffer[ spd.offset + 2 ] = static_cast< uint8_t >( code >> 13 );
		spd.offset += 3;
	}
	else if( delta < 0x1020408f )
	{	//	4 byte encoding
		if( ( spd.length - spd.offset ) < 4 )
		{	//	buffer overflow
			return( false );
		}
		uint32_t code = ( delta - 0x0020408f );
		spd.buffer[ spd.offset + 0 ] = static_cast< uint8_t >( ( code & 15 ) + 239 );
		spd.buffer[ spd.offset + 1 ] = static_cast< uint8_t >( code >> 4 );
		spd.buffer[ spd.offset + 2 ] = static_cast< uint8_t >( code >> 12 );
		spd.buffer[ spd.offset + 4 ] = static_cast< uint8_t >( code >> 20 );
		spd.offset += 4;
	}
	else
	{	//	5 byte encoding
		if( ( spd.length - spd.offset ) < 5 )
		{	//	buffer overflow
			return( false );
		}
		uint32_t code = ( delta - 0x1020408f );
		spd.buffer[ spd.offset + 0 ] = 255;
		spd.buffer[ spd.offset + 1 ] = static_cast< uint8_t >( code );
		spd.buffer[ spd.offset + 2 ] = static_cast< uint8_t >( code >> 8 );
		spd.buffer[ spd.offset + 3 ] = static_cast< uint8_t >( code >> 16 );
		spd.buffer[ spd.offset + 4 ] = static_cast< uint8_t >( code >> 24 );
		spd.offset += 5;
	}
	return( true );
}

////    index array unpacking functions

bool Unpack( spd_buffer& spd, uint32_t* const values, const unsigned int limit, unsigned int& count )
{
	ASSERT( values );
	uint32_t delta = 0;
	uint32_t value = 0;
	for( unsigned int index = 0; Unpack( spd, delta ); ++index )
	{
		if( delta )
		{
			if( index >= limit ) break;
			value += delta;
			values[ index ] = value;
		}
		else
		{
			count = index;
			return( true );
		}
	}
	count = 0;
	return( false );
}

bool Unpack( spd_buffer& spd, uint16_t* const values, const unsigned int limit, unsigned int& count )
{
	ASSERT( values );
	uint32_t delta = 0;
	uint32_t value = 0;
	for( unsigned int index = 0; Unpack( spd, delta ); ++index )
	{
		if( delta )
		{
			if( index >= limit ) break;
			value += delta;
			values[ index ] = static_cast< uint16_t >( value );
		}
		else
		{
			count = index;
			return( true );
		}
	}
	count = 0;
	return( false );
}

bool Unpack( spd_buffer& spd, uint8_t* const values, const unsigned int limit, unsigned int& count )
{
	ASSERT( values );
	uint32_t delta = 0;
	uint32_t value = 0;
	for( unsigned int index = 0; Unpack( spd, delta ); ++index )
	{
		if( delta )
		{
			if( index >= limit ) break;
			value += delta;
			values[ index ] = static_cast< uint8_t >( value );
		}
		else
		{
			count = index;
			return( true );
		}
	}
	count = 0;
	return( false );
}

bool Unpack( spd_buffer& spd, uint32_t& delta )
{
	delta = 0;
	if( spd.offset >= spd.length )
	{	//	buffer underflow
		return( false );
	}
	uint32_t code = spd.buffer[ spd.offset ];
	if( code < 143 )
	{	//	1 byte encoding, deltas 0x00000001-0x0000008e
		++spd.offset;
	}
	else if( code < 207 )
	{	//	2 byte encoding, deltas 0x0000008f-0x0000408e
		if( ( spd.length - spd.offset ) < 2 )
		{	//	buffer underflow
			return( false );
		}
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 1 ] ) << 6 );
		spd.offset += 2;
	}
	else if( code < 239 )
	{	//	3 byte encoding, deltas 0x0000408f-0x0020408e
		if( ( spd.length - spd.offset ) < 3 )
		{	//	buffer underflow
			return( false );
		}
		code += ( 0x0000408f - 207 );
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 1 ] ) << 5 );
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 2 ] ) << 13 );
		spd.offset += 3;
	}
	else if( code < 255 )
	{	//	4 byte encoding, deltas 0x0020408f-0x1020408e
		if( ( spd.length - spd.offset ) < 4 )
		{	//	buffer underflow
			return( false );
		}
		code += ( 0x0020408f - 239 );
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 1 ] ) << 4 );
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 2 ] ) << 12 );
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 3 ] ) << 20 );
		spd.offset += 4;
	}
	else
	{	//	5 byte encoding, deltas 0x10203d83 and greater
		if( ( spd.length - spd.offset ) < 5 )
		{	//	buffer underflow
			return( false );
		}
		code = 0x10203d83;
		code += static_cast< uint32_t >( spd.buffer[ spd.offset + 1 ] );
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 2 ] ) << 8 );
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 3 ] ) << 16 );
		code += ( static_cast< uint32_t >( spd.buffer[ spd.offset + 4 ] ) << 24 );
		spd.offset += 5;
	}
	delta = code;
	return( true );
}

////    index array sorting functions

void Sort( uint32_t* const values, const unsigned int count )
{
	ASSERT( values );
	if( count > 1 )
	{
		if( values[ 0 ] > values[ 1 ] )
		{
			uint32_t swap = values[ 0 ];
			values[ 0 ] = values[ 1 ];
			values[ 1 ] = swap;
		}
		for( unsigned int index = 2; index < count; ++index )
		{
			uint32_t value = values[ index ];
			unsigned int place = 0;
			unsigned int limit = index;
			while( place < limit )
			{
				unsigned int check = ( ( limit + place ) >> 1 );
				if( value < values[ check ] )
				{
					limit = check;
				}
				else
				{
					place = ( check + 1 );
				}
			}
			if( place < index )
			{
				for( unsigned int shift = index; shift > place; --shift )
				{
					values[ shift ] = values[ shift - 1 ];
				}
				values[ place ] = value;
			}
		}
	}
}

void Sort( uint16_t* const values, const unsigned int count )
{
	ASSERT( values );
	if( count > 1 )
	{
		if( values[ 0 ] > values[ 1 ] )
		{
			uint16_t swap = values[ 0 ];
			values[ 0 ] = values[ 1 ];
			values[ 1 ] = swap;
		}
		for( unsigned int index = 2; index < count; ++index )
		{
			uint16_t value = values[ index ];
			unsigned int place = 0;
			unsigned int limit = index;
			while( place < limit )
			{
				unsigned int check = ( ( limit + place ) >> 1 );
				if( value < values[ check ] )
				{
					limit = check;
				}
				else
				{
					place = ( check + 1 );
				}
			}
			if( place < index )
			{
				for( unsigned int shift = index; shift > place; --shift )
				{
					values[ shift ] = values[ shift - 1 ];
				}
				values[ place ] = value;
			}
		}
	}
}

void Sort( uint8_t* const values, const unsigned int count )
{
	ASSERT( values );
	if( count > 1 )
	{
		if( values[ 0 ] > values[ 1 ] )
		{
			uint8_t swap = values[ 0 ];
			values[ 0 ] = values[ 1 ];
			values[ 1 ] = swap;
		}
		for( unsigned int index = 2; index < count; ++index )
		{
			uint8_t value = values[ index ];
			unsigned int place = 0;
			unsigned int limit = index;
			while( place < limit )
			{
				unsigned int check = ( ( limit + place ) >> 1 );
				if( value < values[ check ] )
				{
					limit = check;
				}
				else
				{
					place = ( check + 1 );
				}
			}
			if( place < index )
			{
				for( unsigned int shift = index; shift > place; --shift )
				{
					values[ shift ] = values[ shift - 1 ];
				}
				values[ place ] = value;
			}
		}
	}
}

////    find value in packed data

bool Find( spd_buffer& spd, const uint32_t value )
{
	spd_buffer tmp = spd;
	uint32_t delta = 0;
	uint32_t check = 0;
	while( Unpack( tmp, delta ) )
	{
		check += delta;
		if( check == value )
		{
			return( true );
		}
		if( !delta )
		{
			return( false );
		}
	}
	return( false );
}

////    index appending

bool Append( spd_buffer& spd, const uint32_t value )
{
	if( spd.offset >= spd.length )
	{
		return( false );
	}
	if( spd.offset )
	{
		if( spd.buffer[ spd.offset - 1 ] )
		{
			Pack( spd, 0 );
		}
	}
	else
	{
		if( GetPackedSize( value ) >= ( spd.length - spd.offset ) )
		{
			return( false );
		}
		Pack( spd, value );
		Pack( spd, 0 );
		return( true );
	}
	spd_buffer tmp;
	tmp.length = spd.offset;
	tmp.offset = 0;
	tmp.buffer = spd.buffer;
	uint32_t delta = 0;
	uint32_t check = 0;
	while( Unpack( tmp, delta ) )
	{
		if( delta )
		{
			check += delta;
		}
		else
		{
			delta = ( value - check );
			if( GetPackedSize( delta ) > ( spd.length - spd.offset ) )
			{
				return( false );
			}
			--spd.offset;
			Pack( spd, delta );
			Pack( spd, 0 );
			return( true );
		}
	}
	return( false );
}

////    packed index insertion (assumes packed data is sorted)

bool Insert( spd_buffer& spd, const uint32_t value )
{
	if( spd.offset >= spd.length )
	{
		return( false );
	}
	if( spd.offset )
	{
		if( spd.buffer[ spd.offset - 1 ] )
		{
			Pack( spd, 0 );
		}
	}
	else
	{
		if( GetPackedSize( value ) >= ( spd.length - spd.offset ) )
		{
			return( false );
		}
		Pack( spd, value );
		Pack( spd, 0 );
		return( true );
	}
	spd_buffer tmp;
	tmp.length = spd.offset;
	tmp.offset = 0;
	tmp.buffer = spd.buffer;
	uint32_t delta = 0;
	uint32_t code0 = value;
	uint32_t code1 = 0;
	unsigned int offset = tmp.offset;
	while( Unpack( tmp, delta ) )
	{
		if( delta )
		{
			code1 += delta;
			if( code1 >= code0 )
			{
				code1 = ( code1 - code0 );
				code0 = ( delta - code1 );
				unsigned int shift = ( GetPackedSize( code0 ) + GetPackedSize( code1 ) + offset - tmp.offset );
				if( shift )
				{
					if( shift > ( spd.length - spd.offset ) )
					{
						return( false );
					}
					uint8_t* source = &tmp.buffer[ tmp.offset ];
					uint8_t* target = &source[ shift ];
					shift = ( spd.offset - tmp.offset );
					while( shift )
					{
						--shift;
						target[ shift ] = source[ shift ];
					}
				}
				tmp.offset = offset;
				Pack( tmp, code0 );
				Pack( tmp, code1 );
				return( true );
			}
		}
		else
		{
			return( false );
		}
		offset = tmp.offset;
	}
	return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end sparse namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace sparse

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end compression namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace compression

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
