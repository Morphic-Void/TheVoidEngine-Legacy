
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   ParseJSON.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	json parser class.
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

#include "ParseJSON.h"
#include "QueryMisc.h"
#include "text/ascii.h"
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
////    ParseJSON class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParseJSON::~ParseJSON()
{
}

bool ParseJSON::Parse( void )
{
	return( Value() );
}

bool ParseJSON::Value( void )
{
	if( Items() )
	{
		uint32_t item = ( m_items - 1 );
		White();
		char ascii = m_parse.buffer[ m_parse.offset ];
		switch( ascii )
		{
		case( '[' ):
			{	//	start of array
				return( Array() );
			}
		case( '{' ):
			{	//	start of object
				return( Object() );
			}
		case( '"' ):
			{	//	start of string
				if( ( m_tree[ item ].text = String() ) != 0 )
				{
					m_tree[ item ].type = QUERY_VALUE_TEXT;
					return( true );
				}
				break;
			}
		case( 'n' ):
			{	//	null
				if( strncmp( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset + 1 ] ), "ull", 3 ) == 0 )
				{
					m_parse.offset += 4;
					m_tree[ item ].type = QUERY_VALUE_NULL;
					return( true );
				}
				break;
			}
		case( 't' ):
			{	//	true
				if( strncmp( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset + 1 ] ), "rue", 3 ) == 0 )
				{
					m_parse.offset += 4;
					m_tree[ item ].type = QUERY_VALUE_TRUE;
					return( true );
				}
				break;
			}
		case( 'f' ):
			{	//	false
				if( strncmp( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset + 1 ] ), "alse", 4 ) == 0 )
				{
					m_parse.offset += 5;
					m_tree[ item ].type = QUERY_VALUE_FALSE;
					return( true );
				}
				break;
			}
		default:
			{
				if( ( ( ascii >= '0' ) && ( ascii <= '9' ) ) || ( ascii == '+' ) || ( ascii == '-' ) )
				{	//	number
					unsigned int read = 0;
					if( ascii::ParseDecimal( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset ] ), m_tree[ item ].data.number, read ) )
					{
						m_parse.offset += read;
						m_tree[ item ].type = QUERY_VALUE_NUMBER;
						return( true );
					}
				}
				break;
			}
		}
	}
	return( false );
}

bool ParseJSON::Array( void )
{
	++m_parse.offset;
	uint32_t item = ( m_items - 1 );
	White();
	if( m_parse.buffer[ m_parse.offset ] == ']' )
	{
		++m_parse.offset;
		m_tree[ item ].type = QUERY_VALUE_ITEM;
		return( true );
	}
	uint32_t next = m_items;
	if( !Value() ) return( false );
	m_tree[ item ].type = QUERY_VALUE_LIST;
	m_tree[ item ].data.list.members = next;
	m_tree[ next ].parent = item;
	White();
	if( m_parse.buffer[ m_parse.offset ] == ']' )
	{
		++m_parse.offset;
		return( true );
	}
	while( m_parse.buffer[ m_parse.offset ] == ',' )
	{
		++m_parse.offset;
		uint32_t prev = next;
		m_tree[ prev ].next = next = m_items;
		if( !Value() ) return( false );
		m_tree[ next ].prev = prev;
		m_tree[ next ].parent = item;
		White();
		if( m_parse.buffer[ m_parse.offset ] == ']' )
		{
			++m_parse.offset;
			return( true );
		}
	}
	return( false );
}

bool ParseJSON::Object( void )
{
	++m_parse.offset;
	uint32_t item = ( m_items - 1 );
	White();
	if( m_parse.buffer[ m_parse.offset ] == '}' )
	{
		++m_parse.offset;
		m_tree[ item ].type = QUERY_VALUE_ITEM;
		return( true );
	}
	if( m_parse.buffer[ m_parse.offset ] != '"' ) return( false );
	uint32_t key = String();
	if( !key ) return( false );
	White();
	if( m_parse.buffer[ m_parse.offset ] != ':' ) return( false );
	++m_parse.offset;
	uint32_t next = m_items;
	if( !Value() ) return( false );
	m_tree[ item ].type = QUERY_VALUE_LIST;
	m_tree[ item ].data.list.members = next;
	m_tree[ next ].parent = item;
	m_tree[ next ].key = key;
	White();
	if( m_parse.buffer[ m_parse.offset ] == '}' )
	{
		++m_parse.offset;
		return( true );
	}
	while( m_parse.buffer[ m_parse.offset ] == ',' )
	{
		++m_parse.offset;
		White();
		if( m_parse.buffer[ m_parse.offset ] != '"' ) return( false );
		key = String();
		if( !key ) return( false );
		White();
		if( m_parse.buffer[ m_parse.offset ] != ':' ) return( false );
		++m_parse.offset;
		uint32_t prev = next;
		m_tree[ prev ].next = next = m_items;
		if( !Value() ) return( false );
		m_tree[ next ].prev = prev;
		m_tree[ next ].parent = item;
		m_tree[ next ].key = key;
		White();
		if( m_parse.buffer[ m_parse.offset ] == '}' )
		{
			++m_parse.offset;
			return( true );
		}
	}
	return( false );
}

uint32_t ParseJSON::String( void )
{
	++m_parse.offset;
	int32_t unicode = 0;
	uint32_t string = m_write.offset;
	while( m_parse.buffer[ m_parse.offset ] )
	{
		if( m_parse.buffer[ m_parse.offset ] == '"' )
		{	//	end of simple string found
			++m_parse.offset;
			if( string == m_write.offset )
			{	//	string was empty, use default empty string
				return( 1 );
			}
			else if( !Write() )
			{	//	failed to add zero terminator to string
				return( 0 );
			}
			return( string );
		}
		else
		{
			unicode::GetUTF8( m_parse, unicode );
			if( unicode::IsCC( unicode ) ) return( 0 );
			if( unicode == '\\' )
			{	//	escape code
				unicode::GetUTF8( m_parse, unicode );
				switch( unicode )
				{	//	process escape descriptor
				case( '"' ):	//	quotation mark
				case( '/' ):	//	solidus
				case( '\\' ):	//	reverse solidus
					{	//	pass-through
						break;
					}
				case( 'b' ):
					{	//	backspace
						unicode = '\b';
						break;
					}
				case( 'f' ):
					{	//	form-feed
						unicode = '\f';
						break;
					}
				case( 'n' ):
					{	//	new line
						unicode = '\n';
						break;
					}
				case( 'r' ):
					{	//	carriage return
						unicode = '\r';
						break;
					}
				case( 't' ):
					{	//	tab
						unicode = '\t';
						break;
					}
				case( 'u' ):
					{	//	ascii hex unicode escape
						unsigned long long ull = 0;
						unsigned int read = 0;
						m_parse.offset -= 2;
						if( !ascii::ParseHex( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset ] ), 6, ull, read ) ) return( 0 );
						if( read != 6 ) return( 0 );
						m_parse.offset += 6;
						unicode = static_cast< int32_t >( ull );
						if( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) )
						{	//	unicode is part of surrogate pair
							if( unicode >= 0x0000dc00 ) return( 0 );
							if( ( m_parse.buffer[ m_parse.offset ] != '\\' ) || ( m_parse.buffer[ m_parse.offset + 1 ] != 'u' ) ) return( 0 );
							if( !ascii::ParseHex( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset ] ), 6, ull, read ) ) return( 0 );
							if( read != 6 ) return( 0 );
							m_parse.offset += 6;
							int32_t unipair = static_cast< int32_t >( ull );
							if( ( unipair < 0x0000dc00 ) || ( unipair >= 0x0000e000 ) ) return( 0 );
							unicode = ( ( ( unicode & 0x000003ff ) << 10 ) + ( unipair & 0x000003ff ) + 0x00010000 );
						}
						else if( unicode == 0 )
						{
							return( 0 );
						}
						break;
					}
				default:
					{	//	not a valid escape code descriptor
						return( 0 );
					}
				}
			}
			if( !Write( unicode ) ) return( 0 );
		}
	}
	return( 0 );
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
