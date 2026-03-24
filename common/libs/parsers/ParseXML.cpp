
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   ParseXML.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	xml parser class.
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

#include "ParseXML.h"
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
////    ParseXML class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParseXML::~ParseXML()
{
}

bool ParseXML::Parse( void )
{
	if( Items() && Next() )
	{
		if( m_parse.buffer[ m_parse.offset ] != '/' )
		{
			uint32_t prev = m_items;
			if( Tag() )
			{
				for( uint32_t next = m_items; ( Next() && Tag() ); next = m_items )
				{
					m_tree[ prev ].next = next;
					m_tree[ next ].prev = prev;
					prev = next;
				}
				m_tree[ 0 ].type = QUERY_VALUE_LIST;
				m_tree[ 0 ].data.list.members = 1;
				return( true );
			}
		}
	}
	return( false );
}

bool ParseXML::Next( void )
{
	while( m_parse.buffer[ m_parse.offset ] )
	{
		if( m_parse.buffer[ m_parse.offset++ ] == '<' )
		{
			if( ( m_parse.buffer[ m_parse.offset ] != '!' ) && ( m_parse.buffer[ m_parse.offset ] != '?' ) )
			{	//	start tag, end tag or empty tag
				return( true );
			}
			if( !Skip() ) return( false );
		}
	}
	return( false );
}

bool ParseXML::Skip( void )
{
	switch( m_parse.buffer[ m_parse.offset ] )
	{
	case( '!' ):
		{
			++m_parse.offset;
			if( ( m_parse.buffer[ m_parse.offset ] == '-' ) && ( m_parse.buffer[ m_parse.offset + 1 ] == '-' ) )
			{	//	skip comment
				m_parse.offset += 2;
				while( m_parse.buffer[ m_parse.offset ] )
				{
					if( m_parse.buffer[ m_parse.offset++ ] == '>' )
					{
						if( ( m_parse.buffer[ m_parse.offset - 2 ] == '-' ) && ( m_parse.buffer[ m_parse.offset - 3 ] == '-' ) && ( m_parse.buffer[ m_parse.offset - 4 ] != '-' ) )
						{
							return( true );
						}
					}
				}
				return( false );
			}
			if( m_parse.buffer[ m_parse.offset ] == '[' )
			{	//	skip [CDATA[ or similar
				++m_parse.offset;
				while( m_parse.buffer[ m_parse.offset ] )
				{
					if( m_parse.buffer[ m_parse.offset++ ] == '>' )
					{
						if( ( m_parse.buffer[ m_parse.offset - 2 ] == ']' ) && ( m_parse.buffer[ m_parse.offset - 3 ] == ']' ) )
						{
							return( true );
						}
					}
				}
				return( false );
			}
			while( m_parse.buffer[ m_parse.offset ] )
			{	//	skip DOCTYPE, ATTLIST, ELEMENT or similar
				if( m_parse.buffer[ m_parse.offset++ ] == '>' ) return( true );
			}
			break;
		}
	case( '?' ):
		{
			++m_parse.offset;
			while( m_parse.buffer[ m_parse.offset ] )
			{	//	skip processing instructions
				if( m_parse.buffer[ m_parse.offset++ ] == '>' )
				{
					if( m_parse.buffer[ m_parse.offset - 2 ] == '?' )
					{
						return( true );
					}
				}
			}
			break;
		}
	default:
		{
			while( m_parse.buffer[ m_parse.offset ] )
			{	//	skip simple tag
				if( m_parse.buffer[ m_parse.offset++ ] == '>' ) return( true );
			}
			break;
		}
	}
	return( false );
}

bool ParseXML::Tag( void )
{
	uint32_t item = m_items;
	if( Items() )
	{
		if( Name( item ) )
		{
			if( ( m_parse.buffer[ m_parse.offset ] != '>' ) && ( m_parse.buffer[ m_parse.offset ] != '/' ) )
			{
				uint32_t next = m_items;
				if( !Attribute() ) return( false );
				m_tree[ item ].data.list.attribs = next;
				m_tree[ next ].parent = item;
				while( ( m_parse.buffer[ m_parse.offset ] != '>' ) && ( m_parse.buffer[ m_parse.offset ] != '/' ) )
				{	//	parse attributes
					uint32_t prev = next;
					m_tree[ prev ].next = next = m_items;
					if( !Attribute() ) return( false );
					m_tree[ next ].prev = prev;
					m_tree[ next ].parent = item;
				}
			}
			if( m_parse.buffer[ m_parse.offset++ ] == '/' )
			{	//	empty tag closure possible
				m_tree[ item ].type = QUERY_VALUE_ITEM;
				return( ( m_parse.buffer[ m_parse.offset++ ] == '>' ) ? true : false );
			}
			uint32_t cdata = m_parse.offset;
			if( Next() && ( m_parse.buffer[ m_parse.offset ] != '/' ) )
			{
				uint32_t next = m_items;
				if( !Tag() ) return( false );
				m_tree[ item ].data.list.members = next;
				m_tree[ next ].parent = item;
				while( Next() && ( m_parse.buffer[ m_parse.offset ] != '/' ) )
				{
					uint32_t prev = next;
					m_tree[ prev ].next = next = m_items;
					if( !Tag() ) return( false );
					m_tree[ next ].prev = prev;
					m_tree[ next ].parent = item;
				}
			}
			while( m_parse.buffer[ m_parse.offset ] )
			{
				if( m_parse.buffer[ m_parse.offset++ ] == '>' )
				{
					if( m_tree[ item ].data.list.members )
					{
						m_tree[ item ].type = QUERY_VALUE_LIST;
						return( true );
					}
					return( CData( item, cdata ) );
				}
			}
		}
	}
	return( false );
}

bool ParseXML::Attribute( void )
{
	uint32_t item = m_items;
	if( Items() )
	{
		if( Name( item ) )
		{
			if( m_parse.buffer[ m_parse.offset ] == '=' )
			{
				++m_parse.offset;
				White();
				if( ( m_parse.buffer[ m_parse.offset ] == '"' ) || ( m_parse.buffer[ m_parse.offset ] == '\'' ) )
				{
					char quote = m_parse.buffer[ m_parse.offset++ ];
					White();
					uint32_t text = m_write.offset;
					int32_t unicode = 0;
					bool white = false;
					while( m_parse.buffer[ m_parse.offset ] )
					{
						if( m_parse.buffer[ m_parse.offset ] == quote )
						{	//	found end of string
							if( text == m_write.offset )
							{	//	string was empty, use default empty string
								text = 1;
							}
							else if( !Write() )
							{	//	failed to add zero terminator to string
								return( false );
							}
							m_tree[ item ].type = QUERY_VALUE_TEXT;
							m_tree[ item ].text = text;
							++m_parse.offset;
							White();
							return( true );
						}
						else
						{	//	found character data or white space
							unicode::GetUTF8( m_parse, unicode );
							if( unicode::IsWhiteXML( unicode ) )
							{	//	found white space
								White();
								white = true;
							}
							else
							{	//	found character data
								if( white )
								{	//	write seperating white space
									if( !Write( ' ' ) ) break;
									white = false;
								}
								if( !Write( unicode ) ) break;
							}
						}
					}
				}
			}
		}
	}
	return( false );
}

bool ParseXML::Name( const uint32_t item )
{
	White();
	if( !ascii::IsNameEndXML( m_parse.buffer[ m_parse.offset ] ) )
	{
		m_tree[ item ].key = m_write.offset;
		unsigned int bytes = 1;
		while( !ascii::IsNameEndXML( m_parse.buffer[ ++m_parse.offset ] ) ) ++bytes;
		return( Clone( &m_parse.buffer[ m_parse.offset - bytes ], bytes ) );
	}
	return( false );
}

bool ParseXML::CData( const uint32_t item, const uint32_t cdata )
{
	uint32_t offset = m_parse.offset;
	m_parse.offset = cdata;
	for(;;)
	{
		if( m_parse.buffer[ m_parse.offset++ ] == '<' )
		{
			if( m_parse.buffer[ m_parse.offset ] == '/' )
			{	//	simple text
				m_parse.offset = cdata;
				for(;;)
				{
					White();
					if( m_parse.buffer[ m_parse.offset ] == '<' )
					{	//	skip all leading non-character data
						if( m_parse.buffer[ ++m_parse.offset ] == '/' )
						{	//	no character data found
							m_tree[ item ].type = QUERY_VALUE_ITEM;
							break;
						}
						Skip();
					}
					else
					{	//	found character data
						m_tree[ item ].text = m_write.offset;
						int32_t unicode = 0;
						bool white = false;
						for(;;)
						{
							if( m_parse.buffer[ m_parse.offset ] == '<' )
							{	//	found non-character data
								if( m_parse.buffer[ ++m_parse.offset ] == '/' )
								{	//	end of character data
									if( !Write() ) return( false );
									m_tree[ item ].type = QUERY_VALUE_TEXT;
									break;
								}
								Skip();
								White();
								white = true;
							}
							else
							{	//	found character data or white space
								unicode::GetUTF8( m_parse, unicode );
								if( unicode::IsWhiteXML( unicode ) )
								{	//	found white space
									White();
									white = true;
								}
								else
								{	//	found character data
									if( white )
									{
										if( !Write( ' ' ) ) return( false );
										white = false;
									}
									if( !Write( unicode ) ) return( false );
								}
							}
						}
						break;
					}
				}
				break;
			}
			if( strncmp( reinterpret_cast< char* >( &m_parse.buffer[ m_parse.offset ] ), "![CDATA[", 8 ) == 0 )
			{	//	CDATA
				m_parse.offset += 8;
                uint32_t bytes = 0;;
				for(;;)
				{
					if( m_parse.buffer[ m_parse.offset++ ] == '>' )
					{
						if( ( m_parse.buffer[ m_parse.offset - 2 ] == ']' ) && ( m_parse.buffer[ m_parse.offset - 3 ] == ']' ) )
						{
							if( bytes )
							{
								m_tree[ item ].text = m_write.offset;
								if( !Clone( &m_parse.buffer[ m_parse.offset - 3 - bytes ], bytes ) ) return( false );
							}
							else
							{
								m_tree[ item ].text = 1;
							}
							m_tree[ item ].type = QUERY_VALUE_DATA;
							break;
						}
					}
                    ++bytes;
				}
				break;
			}
			Skip();
		}
	}
	m_parse.offset = offset;
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
