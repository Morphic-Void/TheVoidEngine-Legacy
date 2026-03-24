
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   QueryTree.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	parsed data query class.
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

#include "QueryTree.h"
#include "QueryMisc.h"
#include "ParseJSON.h"
#include "ParseXML.h"
#include "ParseBVH.h"
#include "WriteJSON.h"
#include "WriteXML.h"
#include "libs/system/debug/asserts.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    QueryTree class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QueryTree::~QueryTree()
{
	Release();
}

void QueryTree::Release( void )
{
	m_items = 0;
	m_bytes = 0;
	m_count = 0;
	if( m_head )
	{
		delete[] m_head;
		m_head = 0;
	}
	m_tree = 0;
	m_text = 0;
	m_sort = 0;
}

bool QueryTree::WriteJSON( const void*& buffer, unsigned int& length ) const
{
	parsers::WriteJSON json;
	return( json.Parse( *this, buffer, length ) );
}

bool QueryTree::WriteXML( const void*& buffer, unsigned int& length ) const
{
	parsers::WriteXML xml;
	return( xml.Parse( *this, buffer, length ) );
}

bool QueryTree::ParseJSON( const void* const buffer, const unsigned int length )
{
	Release();
	parsers::ParseJSON json;
	if( json.Parse( buffer, length ) )
	{
		Transfer( json );
		return( true );
	}
	return( false );
}

bool QueryTree::ParseXML( const void* const buffer, const unsigned int length )
{
	Release();
	parsers::ParseXML xml;
	if( xml.Parse( buffer, length ) )
	{
		Transfer( xml );
		return( true );
	}
	return( false );
}

bool QueryTree::ParseBVH( const void* const buffer, const unsigned int length )
{
	Release();
	parsers::ParseBVH bvh;
	if( bvh.Parse( buffer, length ) )
	{
		Transfer( bvh );
		return( true );
	}
	return( false );
}

void QueryTree::GetBIN( const void*& buffer, unsigned int& length ) const
{
	buffer = reinterpret_cast< void* >( m_head );
	length = ( sizeof( QueryHead ) + ( m_items * sizeof( QueryItem ) ) + ( m_bytes * sizeof( char ) ) + ( m_count * sizeof( uint32_t ) ) );
}

bool QueryTree::SetBIN( const void* const buffer, const unsigned int length )
{
	Release();
    if( length >= sizeof( QueryHead ) )
    {
        QueryHead* head = reinterpret_cast< QueryHead* >( const_cast< void* >( buffer ) );
	    uint32_t items = head->m_items;
	    uint32_t bytes = head->m_bytes;
	    uint32_t count = head->m_count;
	    if( length >= ( sizeof( QueryHead ) + ( items * sizeof( QueryItem ) ) + ( bytes * sizeof( char ) ) + ( count * sizeof( uint32_t ) ) ) )
        {
            m_head = head;
        	m_tree = reinterpret_cast< QueryItem* >( &m_head[ 1 ] );
	        m_text = reinterpret_cast< char* >( &m_tree[ m_head->m_items ] );
	        m_sort = reinterpret_cast< uint32_t* >( &m_text[ m_head->m_bytes ] );
            m_items = items;
	        m_bytes = bytes;
	        m_count = count;
            return( true );
        }
    }
    return( false );
}

QUERY_VALUE_TYPE QueryTree::GetType( const uint32_t item ) const
{
	ASSERT( item < m_items );
	return( ( item < m_items ) ? static_cast< QUERY_VALUE_TYPE >( m_tree[ item ].type ) : QUERY_VALUE_INVALID );
}

const char* QueryTree::GetKey( const uint32_t item ) const
{
	ASSERT( item < m_items );
	return( ( item < m_items ) ? ( m_tree[ item ].key ? &m_text[ m_tree[ item ].key ] : 0 ) : 0 );
}

const char* QueryTree::GetText( const uint32_t item ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( ( m_tree[ item ].type == QUERY_VALUE_TEXT ) || ( m_tree[ item ].type == QUERY_VALUE_DATA ) )
		{
			return( &m_text[ m_tree[ item ].text ] );
		}
	}
	return( 0 );
}

bool QueryTree::GetBool( const uint32_t item, bool& boolean ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		switch( m_tree[ item ].type )
		{
		case( QUERY_VALUE_TRUE ):
			{
				boolean = true;
				return( true );
			}
		case( QUERY_VALUE_FALSE ):
			{
				boolean = false;
				return( true );
			}
		default:
			{
				break;
			}
		}
	}
	boolean = false;
	return( false );
}

bool QueryTree::GetNumber( const uint32_t item, double& number ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( m_tree[ item ].type == QUERY_VALUE_NUMBER )
		{
			number = m_tree[ item ].data.number;
			return( true );
		}
	}
	number = 0;
	return( false );
}

bool QueryTree::GetNumberAsInt( const uint32_t item, int& number ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( m_tree[ item ].type == QUERY_VALUE_NUMBER )
		{
			double check = m_tree[ item ].data.number;
			number = static_cast< int >( check );
			if( check == static_cast< double >( number ) )
			{
				return( true );
			}
		}
	}
	number = 0;
	return( false );
}

bool QueryTree::GetNumberAsUint( const uint32_t item, unsigned int& number ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( m_tree[ item ].type == QUERY_VALUE_NUMBER )
		{
			double check = m_tree[ item ].data.number;
			number = static_cast< unsigned int >( check );
			if( check == static_cast< double >( number ) )
			{
				return( true );
			}
		}
	}
	number = 0;
	return( false );
}

uint32_t QueryTree::GetMembers( const uint32_t item ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( m_tree[ item ].type != QUERY_VALUE_NUMBER )
		{
			uint32_t count = 0;
			for( uint32_t member = m_tree[ item ].data.list.members; member; member = m_tree[ member ].next )
			{
				++count;
			}
			return( count );
		}
	}
	return( 0 );
}

uint32_t QueryTree::GetMember( const uint32_t item, const uint32_t index ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( m_tree[ item ].type != QUERY_VALUE_NUMBER )
		{
			uint32_t count = index;
			for( uint32_t member = m_tree[ item ].data.list.members; member; member = m_tree[ member ].next )
			{
				if( !count ) return( member );
				--count;
			}
		}
	}
	return( 0 );
}

uint32_t QueryTree::GetMember( const uint32_t root, const char* const path ) const
{
	ASSERT( root < m_items );
	if( root < m_items )
	{
		return( GetItem( root, path ) );
	}
	return( 0 );
}

uint32_t QueryTree::GetAttribs( const uint32_t item ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( m_tree[ item ].type != QUERY_VALUE_NUMBER )
		{
			uint32_t count = 0;
			for( uint32_t attrib = m_tree[ item ].data.list.attribs; attrib; attrib = m_tree[ attrib ].next )
			{
				++count;
			}
			return( count );
		}
	}
	return( 0 );
}

uint32_t QueryTree::GetAttrib( const uint32_t item, const uint32_t index ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( m_tree[ item ].type != QUERY_VALUE_NUMBER )
		{
			uint32_t count = index;
			for( uint32_t attrib = m_tree[ item ].data.list.attribs; attrib; attrib = m_tree[ attrib ].next )
			{
				if( !count ) return( attrib );
				--count;
			}
		}
	}
	return( 0 );
}

uint32_t QueryTree::GetAttrib( const uint32_t item, const char* const name ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		if( m_tree[ item ].type != QUERY_VALUE_NUMBER )
		{
			for( uint32_t attrib = m_tree[ item ].data.list.attribs; attrib; attrib = m_tree[ attrib ].next )
			{
				if( m_tree[ attrib ].key )
				{
					if( strcmp( name, &m_text[ m_tree[ attrib ].key ] ) == 0 )
					{
						return( attrib );
					}
				}
			}
		}
	}
	return( 0 );
}

uint32_t QueryTree::GetParent( const uint32_t item ) const
{
	ASSERT( item < m_items );
	return( ( item < m_items ) ? m_tree[ item ].parent : 0 );
}

uint32_t QueryTree::GetPrev( const uint32_t item ) const
{
	ASSERT( item < m_items );
	return( ( item < m_items ) ? m_tree[ item ].prev : 0 );
}

uint32_t QueryTree::GetNext( const uint32_t item ) const
{
	ASSERT( item < m_items );
	return( ( item < m_items ) ? m_tree[ item ].next : 0 );
}

uint32_t QueryTree::FindRef( const uint32_t item ) const
{
	ASSERT( item < m_items );
	if( item < m_items )
	{
		ASSERT( m_text[ m_tree[ item ].text ] == '#' );
		if( m_text[ m_tree[ item ].text ] == '#' )
		{
			uint32_t text_hash = ( m_tree[ item ].text + 1 );
			for( uint32_t scan = 0; scan < m_items; ++scan )
			{
				if( m_tree[ scan ].text == text_hash ) return( scan );
			}
		}
	}
	return( 0 );
}

uint32_t QueryTree::FindRef( const char* const key, const uint32_t item ) const
{
	ASSERT( key );
	ASSERT( item < m_items );
	if( key && ( item < m_items ) )
	{
		ASSERT( m_text[ m_tree[ item ].text ] == '#' );
		if( m_text[ m_tree[ item ].text ] == '#' )
		{
			uint32_t text_hash = ( m_tree[ item ].text + 1 );
			uint32_t key_hash = GetHash( key );
			if( key_hash )
			{
				for( uint32_t scan = 0; scan < m_items; ++scan )
				{
					if( m_tree[ scan ].key == key_hash )
					{
						if( m_tree[ scan ].text == text_hash )
						{
							return( scan );
						}
					}
				}
			}
		}
	}
	return( 0 );
}

uint32_t QueryTree::FindRef( const char* const key, const char* const text ) const
{
	ASSERT( key && text );
	if( key && text )
	{
		uint32_t key_hash = GetHash( key );
		if( key_hash )
		{
			uint32_t text_hash = GetHash( text );
			if( text_hash )
			{
				for( uint32_t scan = 0; scan < m_items; ++scan )
				{
					if( m_tree[ scan ].key == key_hash )
					{
						if( m_tree[ scan ].text == text_hash )
						{
							return( scan );
						}
					}
				}
			}
		}
	}
	return( 0 );
}

uint32_t QueryTree::FindItem( const uint32_t root, const char* const path ) const
{
	ASSERT( path );
	ASSERT( root < m_items );
	if( root < m_items )
	{
		if( m_tree[ root ].type == QUERY_VALUE_LIST )
		{
			uint32_t item = 0;
			uint32_t find = root;
			while( ( item = GetItem( find, path ) ) == 0 )
			{
				find = m_tree[ find ].data.list.members;
				if( !find )
				{
					return( 0 );
				}
				while( !( m_tree[ root ].type == QUERY_VALUE_LIST ) )
				{
					while( !m_tree[ find ].next )
					{
						find = m_tree[ find ].parent;
						if( find == root )
						{
							return( 0 );
						}
					}
					find = m_tree[ find ].next;
				}
			}
			return( item );
		}
	}
	return( 0 );
}

uint32_t QueryTree::GetItem( const uint32_t root, const char* const path ) const
{
	uint32_t item = root;
	const char* text = path;
	while( text[ 0 ] )
	{
		if( m_tree[ root ].type != QUERY_VALUE_LIST ) return( 0 );
		if( text[ 0 ] == '[' )
		{	//	array
			++text;
			while( text[ 0 ] == ' ' ) ++text;
			if( ( text[ 0 ] < '0' ) || ( text[ 0 ] > '9' ) ) return( 0 );
			uint32_t next = ( text[ 0 ] - '0' );
			for( ++text; ( text[ 0 ] >= '0' ) && ( text[ 0 ] <= '9' ); ++text )
			{
				next *= 10;
				next += ( text[ 0 ] - '0' );
			}
			while( text[ 0 ] == ' ' ) ++text;
			if( text[ 0 ] != ']' ) return( 0 );
			++text;
			item = m_tree[ item ].data.list.members;
			while( next && item )
			{
				item = m_tree[ item ].next;
				--next;
			}
			if( !item ) return( 0 );
		}
		else
		{	//	named
			if( text[ 0 ] == '.' ) ++text;
			int best = -1;
			for( uint32_t find = m_tree[ item ].data.list.members; find; find = m_tree[ find ].next )
			{
				if( m_tree[ find ].key )
				{
					const char* const key = &m_text[ m_tree[ find ].key ];
					int scan = 0;
					while( key[ scan ] )
					{
						if( key[ scan ] != text[ scan ] )
						{
							scan = -1;
							break;
						}
						++scan;
					}
					if( best < scan )
					{
						if( ( text[ scan ] == 0 ) || ( text[ scan ] == '.' ) || ( text[ scan ] == '[' ) )
						{
							item = find;
							best = scan;
						}
					}
				}
			}
			if( best < 0 ) return( 0 );
			text += best;
		}
	}
	return( item );
}

uint32_t QueryTree::GetHash( const char* const text ) const
{
	ASSERT( text );
	if( !text ) return( 0 );
	if( !text[ 0 ] ) return( 1 );	//	string 1 is the empty string
	uint32_t found = 0;
	uint32_t limit = m_count;
	const unsigned char* const text0 = reinterpret_cast< const unsigned char* const >( text );
	while( found < limit )
	{
		uint32_t check = ( ( found + limit ) >> 1 );
		const unsigned char* text1 = text0;
		const unsigned char* text2 = reinterpret_cast< const unsigned char* >( &m_text[ m_sort[ check ] ] );
		while( *text1 == *text2 )
		{
			if( *text1 == 0 ) break;
			++text1;
			++text2;
		}
		if( *text1 > *text2 )
		{
			found = ( check + 1 );
		}
		else
		{
			limit = check;
		}
	}
	if( found < m_count )
	{
		const unsigned char* text1 = text0;
		const unsigned char* text2 = reinterpret_cast< const unsigned char* >( &m_text[ m_sort[ found ] ] );
		while( *text1 == *text2 )
		{
			if( *text2 == 0 ) return( m_sort[ found ] );
			++text1;
			++text2;
		}
	}
	return( 0 );
}

void QueryTree::Transfer( QueryTree& other )
{
	m_items = other.m_items;
	m_bytes = other.m_bytes;
	m_count = other.m_count;
	m_head = other.m_head;
	m_tree = other.m_tree;
	m_text = other.m_text;
	m_sort = other.m_sort;
	other.m_items = 0;
	other.m_bytes = 0;
	other.m_count = 0;
	other.m_head = 0;
	other.m_tree = 0;
	other.m_text = 0;
	other.m_sort = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
