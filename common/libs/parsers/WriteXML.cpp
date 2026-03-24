
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   WriteXML.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	xml writer class.
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

#include "WriteXML.h"
#include "QueryTree.h"
#include "libs/system/debug/xstdio.h"

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
////    WriteXML class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WriteXML::~WriteXML()
{
}

bool WriteXML::Parse( void )
{
	if( Write( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" ) )
	{
		if( m_tree->GetMembers( 0 ) > 1 )
		{
			if( !Write( "<body>" ) ) return( false );
			++m_depth;
			for( uint32_t member = m_tree->GetMember( 0 ); member; member = m_tree->GetNext( member ) )
			{
			    if( !Write( "\r\n" ) ) return( false );
				if( !Item( member ) ) return( false );
			}
			--m_depth;
			if( !Write( "\r\n</body>" ) ) return( false );
		}
		else
		{
			if( !Item( 1 ) ) return( false );
		}
		return( Write( "\r\n" ) );
	}
	return( false );
}

//! decode and write an item
bool WriteXML::Item( const uint32_t item )
{
	const uint8_t* const key = Key( item );
	if( !key || !Write( '<' ) || !Clone( key ) ) return( false );
	switch( m_tree->GetType( item ) )
	{
	case( QUERY_VALUE_LIST ):
		{
			++m_depth;
			if( !Attributes( item ) || !Write( ">" ) ) return( false );
			for( uint32_t member = m_tree->GetMember( item ); member; member = m_tree->GetNext( member ) )
			{
				if( !Write( "\r\n" ) ) return( false );
				if( !Item( member ) ) return( false );
			}
			--m_depth;
			if( !Write( "\r\n</" ) || !Clone( key ) ) return( false );
			break;
		}
	case( QUERY_VALUE_ITEM ):
		{
			if( !Attributes( item ) || !Write( '/' ) ) return( false );
			break;
		}
	case( QUERY_VALUE_TEXT ):
		{
			if( !Attributes( item ) || !Write( ">" ) ) return( false );
			if( !Clone( reinterpret_cast< const uint8_t* const >( m_tree->GetText( item ) ) ) ) return( false );
			if( !Write( "</" ) || !Clone( key ) ) return( false );
			break;
		}
	case( QUERY_VALUE_DATA ):
		{
			if( !Attributes( item ) || !Write( "><![CDATA[" ) ) return( false );
			if( !Clone( reinterpret_cast< const uint8_t* const >( m_tree->GetText( item ) ) ) ) return( false );
			if( !Write( "]]></" ) || !Clone( key ) ) return( false );
			break;
		}
	case( QUERY_VALUE_NULL ):
		{
			if( !Write( '/' ) ) return( false );
			break;
		}
	case( QUERY_VALUE_TRUE ):
		{
			if( !Write( ">true</" ) || !Clone( key ) ) return( false );
			break;
		}
	case( QUERY_VALUE_FALSE ):
		{
			if( !Write( ">false</" ) || !Clone( key ) ) return( false );
			break;
		}
	case( QUERY_VALUE_NUMBER ):
		{
			if( !Write( '>' ) || !Number( item ) || !Write( "</" ) || !Clone( key ) ) return( false );
			break;
		}
	default:
		{
			return( false );
		}
	}
	return( Write( '>' ) );
}

const uint8_t* WriteXML::Key( const uint32_t item )
{
	const uint8_t* key = reinterpret_cast< const uint8_t* const >( m_tree->GetKey( item ) );
	if( !key )
	{
		switch( m_tree->GetType( item ) )
		{
		case( QUERY_VALUE_LIST ):
			{
				static const uint8_t text[] = "list";
				key = text;
				break;
			}
		case( QUERY_VALUE_ITEM ):
			{
				static const uint8_t text[] = "item";
				key = text;
				break;
			}
		case( QUERY_VALUE_TEXT ):
			{
				static const uint8_t text[] = "text";
				key = text;
				break;
			}
		case( QUERY_VALUE_DATA ):
			{
				static const uint8_t text[] = "data";
				key = text;
				break;
			}
		case( QUERY_VALUE_NULL ):
			{
				static const uint8_t text[] = "null";
				key = text;
				break;
			}
		case( QUERY_VALUE_TRUE ):
		case( QUERY_VALUE_FALSE ):
			{
				static const uint8_t text[] = "bool";
				key = text;
				break;
			}
		case( QUERY_VALUE_NUMBER ):
			{
				static const uint8_t text[] = "number";
				key = text;
				break;
			}
		default:
			{
				break;
			}
		}
	}
	return( key );
}

//! write a number
bool WriteXML::Number( const uint32_t item )
{
	double number = 0;
	if( m_tree->GetNumber( item, number ) )
	{
		char text[ 32 ];
		sprintf_s( text, 32, "%g", number );
		return( Write( text ) );
	}
	return( false );
}

//! write attributes
bool WriteXML::Attributes( const uint32_t item )
{
	for( uint32_t attrib = m_tree->GetAttrib( item ); attrib; attrib = m_tree->GetNext( attrib ) )
	{
		if( !Write( ' ' ) ) return( false );
		const uint8_t* const key = Key( attrib );
		if( !key ) return( false );
		if( !Clone( key ) ) return( false );
		if( !Write( '=' ) ) return( false );
		switch( m_tree->GetType( attrib ) )
		{
		case( QUERY_VALUE_TEXT ):
			{
				const uint8_t* const text = reinterpret_cast< const uint8_t* const >( m_tree->GetText( attrib ) );
				if( text )
				{
					char quote = '"';
					for( uint32_t scan = 0; text[ scan ]; ++scan )
					{
						if( text[ scan ] == quote )
						{
							quote = '\'';
							break;
						}
					}
					if( !Write( quote ) ) return( false );
					if( !Clone( text ) ) return( false );
					if( !Write( quote ) ) return( false );
				}
				else
				{
					if( !Write( "\"\"" ) ) return( false );
				}
				break;
			}
		case( QUERY_VALUE_NULL ):
			{
				if( !Write( "null") ) return( false );
				break;
			}
		case( QUERY_VALUE_TRUE ):
			{
				if( !Write( "true") ) return( false );
				break;
			}
		case( QUERY_VALUE_FALSE ):
			{
				if( !Write( "false") ) return( false );
				break;
			}
		case( QUERY_VALUE_NUMBER ):
			{
				double number = 0;
				if( !m_tree->GetNumber( attrib, number ) ) return( false );
				char text[ 32 ];
				sprintf_s( text, 32, "\"%g\"", number );
				if( !Write( text ) ) return( false );
				break;
			}
		default:
			{
				if( !Write( "\"\"" ) ) return( false );
				break;
			}
		}
	}
	return( true );
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
