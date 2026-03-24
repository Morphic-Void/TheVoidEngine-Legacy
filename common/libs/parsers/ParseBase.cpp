
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   ParseBase.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	parsers base class.
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

#include "ParseBase.h"
#include "QueryMisc.h"
#include "text/ascii.h"
#include "libs/system/debug/asserts.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin parsers namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace parsers
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ParseBase class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParseBase::~ParseBase()
{
}

bool ParseBase::Parse( const void* const buffer, const unsigned int length )
{
	if( Setup( buffer, length ) && Parse() && Close( buffer ) ) return( true );
	return( false );
}

//! allocate and initialise query items
bool ParseBase::Items( const uint32_t items )
{
	if( ( m_limit - m_items ) <= items )
	{
		uint32_t space = ( m_items + items );
		uint32_t limit = ( m_limit << 1 );
		while( limit <= space ) limit <<= 1;
		QueryItem* tree = new QueryItem[ limit ];
		ASSERT( tree );
		if( !tree ) return( false );
		memcpy( reinterpret_cast< void* const >( tree ), reinterpret_cast< const void* const >( m_tree ), ( m_items * sizeof( QueryItem ) ) );
		delete[] m_tree;
		m_tree = tree;
		m_limit = limit;
	}
	for( uint32_t index = 0; index < items; ++index )
	{
		m_tree[ m_items ].type = QUERY_VALUE_NULL;
		m_tree[ m_items ].parent = 0;
		m_tree[ m_items ].prev = 0;
		m_tree[ m_items ].next = 0;
		m_tree[ m_items ].key = 0;
		m_tree[ m_items ].text = 0;
		m_tree[ m_items ].data.reserved[ 0 ] = 0;
		m_tree[ m_items ].data.reserved[ 1 ] = 0;
		++m_items;
	}
	return( true );
}

//! write a single unicode character
bool ParseBase::Write( const int32_t unicode )
{
	unicode::UNICODE_ERROR error = unicode::SetUTF8( m_write, unicode );
	if( error == unicode::UNICODE_ERROR_BUFFER_OVERFLOW )
	{
		unsigned int length = ( m_write.length + 4096 );
		uint8_t* buffer = new uint8_t[ length ];
		ASSERT( buffer );
		if( !buffer ) return( false );
		memcpy( reinterpret_cast< void* const>( buffer ), reinterpret_cast< const void* const >( m_write.buffer ), m_write.offset );
		delete[] m_write.buffer;
		m_write.buffer = buffer;
		m_write.length = length;
		error = unicode::SetUTF8( m_write, unicode );
	}
	return( ( error == unicode::UNICODE_SUCCESS ) ? true : false );
}

//! write an nul terminated ascii string including the nul terminator
bool ParseBase::Write( const char* const text )
{
	ASSERT( text );
	if( text )
	{
		for( uint32_t index = 0; text[ index ]; ++index )
		{
			if( !Write( text[ index ] ) ) return( false );
		}
	}
	return( Write() );
}

//! clone a specified length utf-8 string and add a nul terminator
bool ParseBase::Clone( const uint8_t* const text, const uint32_t bytes )
{
	ASSERT( text );
	if( text )
	{
		if( ( m_write.length - m_write.offset ) <= bytes )
		{
			unsigned int length = ( ( m_write.offset + bytes + 4096 ) & -4096 );
			uint8_t* buffer = new uint8_t[ length ];
			ASSERT( buffer );
			if( !buffer ) return( false );
			memcpy( reinterpret_cast< void* const>( buffer ), reinterpret_cast< const void* const >( m_write.buffer ), m_write.offset );
			delete[] m_write.buffer;
			m_write.buffer = buffer;
			m_write.length = length;
		}
		memcpy( reinterpret_cast< void* const >( &m_write.buffer[ m_write.offset ] ), reinterpret_cast< const void* const >( text ), bytes );
		m_write.offset += bytes;
		m_write.buffer[ m_write.offset ] = 0;
		++m_write.offset;
		return( true );
	}
	return( Write() );
}

//! skip white space
void ParseBase::White( void )
{
	while( m_parse.offset < m_parse.length )
	{
		if( !ascii::IsWhite( m_parse.buffer[ m_parse.offset ] ) ) break;
		++m_parse.offset;
	}
}

//!	clear all members
void ParseBase::Clear( void )
{
	m_parse.length = 0;
	m_parse.offset = 0;
	m_parse.buffer = 0;
	m_write.length = 0;
	m_write.offset = 0;
	m_write.buffer = 0;
	m_limit = 0;
}

//! initialise parsing
bool ParseBase::Setup( const void* const buffer, const unsigned int length )
{
	Release();
	ASSERT( buffer );
	if( buffer )
	{
		QueryItem* tree = new QueryItem[ 32 ];
		ASSERT( tree );
		if( tree )
		{
			m_tree = tree;
			m_items = 0;
			m_limit = 32;
			uint8_t* text = new uint8_t[ 4096 ];
			ASSERT( text );
			if( text )
			{
				m_write.buffer = text;
				m_write.length = 4096;
				m_write.offset = 0;
				m_parse.buffer = reinterpret_cast< uint8_t* >( const_cast< void* >( buffer ) );
				m_parse.length = length;
				m_parse.offset = 0;
				unicode::UTF_TYPE type = unicode::IdentifyUTF( m_parse );
				if( ( type == unicode::UTF_TYPE_UTF8 ) && ( m_parse.buffer[ m_parse.length - 1 ] == 0 ) )
				{	//	already zero terminated utf-8, validate only
					int32_t parse = 0;
					while( unicode::GetUTF8( m_parse, parse ) == unicode::UNICODE_SUCCESS )
					{
						if( m_parse.offset == m_parse.length )
						{
                            m_parse.offset = 0;
							m_write.offset = 2;
							m_write.buffer[ 0 ] = m_write.buffer[ 1 ] = 0;
							return( true );
						}
					}
				}
				else
				{	//	transform to utf-8 and zero terminate
					const unicode::IUTF& handler = *unicode::GetHandlerUTF( type );
					int32_t parse = 0;
					while( handler.Get( m_parse, parse ) == unicode::UNICODE_SUCCESS )
					{
						if( !Write( parse ) ) break;
						if( m_parse.offset == m_parse.length )
						{	//	parsing to utf-8 is complete
							if( Write() )
							{
								text = new uint8_t[ 4096 ];
								ASSERT( text );
								if( text )
								{
									m_parse.buffer = m_write.buffer;
									m_parse.length = m_write.offset;
									m_parse.offset = 0;
									m_write.buffer = text;
									m_write.length = 4096;
									m_write.offset = 2;
									m_write.buffer[ 0 ] = m_write.buffer[ 1 ] = 0;
									return( true );
								}
							}
							break;
						}
					}
				}
			}
			delete[] m_write.buffer;
			m_write.buffer = 0;
			m_write.length = 0;
			m_write.offset = 0;
		}
		delete[] m_tree;
		m_tree = 0;
		m_items = 0;
		m_limit = 0;
	}
	return( false );
}

//!	eliminate string duplication, sort strings and coalesce memory buffer
bool ParseBase::Close( const void* const buffer )
{
	m_bytes = m_write.offset;
	m_text = reinterpret_cast< char* >( m_write.buffer );
	if( m_parse.buffer != buffer ) delete[] m_parse.buffer;
	m_parse.buffer = 0;
	m_parse.length = 0;
	m_parse.offset = 0;
	m_write.buffer = 0;
	m_write.length = 0;
	m_write.offset = 0;
	m_limit = 0;
	uint32_t count = 0;
	uint32_t bytes = 1;
	while( bytes < m_bytes )
	{	//	count strings
		if( m_text[ bytes ] == 0 ) ++count;
		++bytes;
	}
	uint32_t* sorted = new uint32_t[ count << 2 ];
	ASSERT( sorted );
	if( sorted )
	{
		uint32_t index, found, limit, check, other, basis;
		const unsigned char* text0;
		const unsigned char* text1;
		const unsigned char* text2;
		uint32_t* source = &sorted[ count ];
		uint32_t* target = &source[ count ];
		uint32_t* length = &target[ count ];
		index = 0;
		basis = 0;
		for( bytes = 1; bytes < m_bytes; ++bytes )
		{	//	prepare and sort strings (sorting by reversed strings to accellerate sub-string packing)
			if( m_text[ bytes ] == 0 )
			{
				length[ index ] = ( bytes - basis );
				source[ index ] = ( basis + 1 );
				target[ index ] = index;
				basis = bytes;
				found = 0;
				limit = index;
				text0 = reinterpret_cast< const unsigned char* >( &m_text[ source[ index ] + length[ index ] - 2 ] );
				while( found < limit )
				{
					check = ( ( found + limit ) >> 1 );
					other = sorted[ check ];
					text1 = text0;
					text2 = reinterpret_cast< const unsigned char* >( &m_text[ source[ other ] + length[ other ] - 2 ] );
					while( *text1 == *text2 )
					{
						if( *text1 == 0 ) break;
						--text1;
						--text2;
					}
					if( *text1 < *text2 )
					{
						limit = check;
					}
					else
					{
						found = ( check + 1 );
					}
				}
				for( limit = index; limit > found; --limit )
				{
					sorted[ limit ] = sorted[ limit - 1 ];
				}
				sorted[ found ] = index;
				++index;
			}
		}
		uint32_t pair_indices[ 2 ];
		uint32_t pair_sources[ 2 ];
		uint32_t pair_lengths[ 2 ];
		pair_indices[ 1 ] = sorted[ 0 ];
		pair_sources[ 1 ] = source[ pair_indices[ 1 ] ];
		pair_lengths[ 1 ] = length[ pair_indices[ 1 ] ];
		for( index = 1; index < count; ++index )
		{	//	find duplicates
			pair_indices[ 0 ] = pair_indices[ 1 ];
			pair_sources[ 0 ] = pair_sources[ 1 ];
			pair_lengths[ 0 ] = pair_lengths[ 1 ];
			pair_indices[ 1 ] = sorted[ index ];
			pair_sources[ 1 ] = source[ pair_indices[ 1 ] ];
			pair_lengths[ 1 ] = length[ pair_indices[ 1 ] ];
			if( pair_lengths[ 0 ] <= pair_lengths[ 1 ] )
			{
				text1 = reinterpret_cast< const unsigned char* >( &m_text[ pair_sources[ 0 ] ] );
				text2 = reinterpret_cast< const unsigned char* >( &m_text[ pair_sources[ 1 ] + pair_lengths[ 1 ] - pair_lengths[ 0 ] ] );
				while( *text1 == *text2 )
				{
					if( *text1 == 0 )
					{
						target[ pair_indices[ 0 ] ] = pair_indices[ 1 ];
						break;
					}
					++text1;
					++text2;
				}
			}
		}
		for( index = ( count - 1 ); index < count; --index )
		{	//	propagate duplicates
			uint32_t& remap = target[ sorted[ index ] ];
			remap = target[ remap ];
		}
		for( index = 0; index < count; ++index )
		{	//	remap empty string indices
			if( length[ index ] == 1 ) target[ index ] = 1;
		}
		bytes = 1;
		for( index = 0; index < count; ++index )
		{	//	create target string indices (uses the sorted array as storage as sorting no longer needed)
			sorted[ index ] = 0;
			if( target[ index ] == index )
			{
				memcpy( reinterpret_cast< void* >( &m_text[ bytes ] ), reinterpret_cast< const void* const >( &m_text[ source[ index ] ] ), length[ index ] );
				sorted[ index ] = bytes;
				bytes += length[ index ];
			}
		}
		for( index = 0; index < count; ++index )
		{	//	remap all targets
			uint32_t& remap = target[ index ];
			remap = ( sorted[ remap ] + length[ remap ] - length[ index ] );
		}
		for( index = 0; index < m_items; ++index )
		{	//	remap string references
			for( basis = 0; basis < 2; ++basis )
			{
				uint32_t& string = ( basis ? m_tree[ index ].text : m_tree[ index ].key );
				if( string )
				{
					found = 0;
					limit = count;
					while( found < limit )
					{
						check = ( ( found + limit ) >> 1 );
						if( string > source[ check ] )
						{
							found = ( check + 1 );
						}
						else
						{
							limit = check;
						}
					}
					string = target[ found ];
				}
			}
		}
		index = 0;
		for( basis = 0; basis < count; ++basis )
		{	//	pack string offsets and lengths and sort strings
			if( sorted[ basis ] )
			{
				length[ index ] = length[ basis ];
				source[ index ] = target[ basis ];
				found = 0;
				limit = index;
				text0 = reinterpret_cast< const unsigned char* >( &m_text[ source[ index ] ] );
				while( found < limit )
				{
					check = ( ( found + limit ) >> 1 );
					other = sorted[ check ];
					text1 = text0;
					text2 = reinterpret_cast< const unsigned char* >( &m_text[ source[ other ] ] );
					while( *text1 == *text2 )
					{
						if( *text1 == 0 ) break;
						++text1;
						++text2;
					}
					if( *text1 < *text2 )
					{
						limit = check;
					}
					else
					{
						found = ( check + 1 );
					}
				}
				for( limit = index; limit > found; --limit )
				{
					sorted[ limit ] = sorted[ limit - 1 ];
				}
				sorted[ found ] = index;
				++index;
			}
		}
		count = index;
		m_bytes = bytes;
		m_count = count;
		QueryHead* head = reinterpret_cast< QueryHead* >( new uint8_t[ sizeof( QueryHead ) + ( m_items * sizeof( QueryItem ) ) + ( m_bytes * sizeof( char ) ) + ( m_count * sizeof( uint32_t ) ) ] );
		ASSERT( head );
		if( head )
		{
			m_head = head;
			QueryItem* tree = reinterpret_cast< QueryItem* >( &head[ 1 ] );
			char* text = reinterpret_cast< char* >( &tree[ m_items ] );
			uint32_t* sort = reinterpret_cast< uint32_t* >( &text[ m_bytes ] );
			head->m_items = m_items;
			head->m_bytes = m_bytes;
			head->m_count = m_count;
			memcpy( reinterpret_cast< void* >( tree ), reinterpret_cast< const void* const >( m_tree ), ( m_items * sizeof( QueryItem ) ) );
			memcpy( reinterpret_cast< void* >( text ), reinterpret_cast< const void* const >( m_text ), ( m_bytes * sizeof( char ) ) );
			memcpy( reinterpret_cast< void* >( sort ), reinterpret_cast< const void* const >( sorted ), ( m_count * sizeof( uint32_t ) ) );
			delete[] m_tree;
			delete[] m_text;
			delete[] sorted;
			m_tree = tree;
			m_text = text;
			m_sort = sort;
			return( true );
		}
		delete[] sorted;
	}
	m_items = 0;
	m_bytes = 0;
	m_count = 0;
	m_head = 0;
	delete[] m_tree;
	m_tree = 0;
	delete[] m_text;
	m_text = 0;
	m_sort = 0;
	return( false );
}

//! default parsing function
bool ParseBase::Parse( void )
{
	return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end parsers namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace parsers

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
