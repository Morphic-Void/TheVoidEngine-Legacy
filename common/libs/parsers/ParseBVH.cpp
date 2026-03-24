
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   ParseBVH.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	bvh (hierarchy and animation) parser class.
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

#include "ParseBVH.h"
#include "QueryMisc.h"
#include "text/ascii.h"
#include "libs/system/debug/xstdio.h"
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
////    ParseBVH class constants
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const uint8_t ParseBVH::mk_Standard_Keys[] =
{
	'a', 'n', 'i', 'm', 'a', 't', 'i', 'o', 'n', 0,		//	10 bytes
	'c', 'h', 'a', 'n', 'n', 'e', 'l', 's', 0,			//	9 bytes
	'c', 'o', 'u', 'n', 't', 0,							//	6 bytes
	'e', 'n', 'd', 0,									//	4 bytes
	'f', 'r', 'a', 'm', 'e', 0,							//	6 bytes
	'f', 'r', 'a', 'm', 'e', 's', 0,					//	7 bytes
	'i', 'n', 'd', 'e', 'x', 0,							//	6 bytes
	'o', 'r', 'd', 'e', 'r', 0,							//	6 bytes
	's', 'k', 'e', 'l', 'e', 't', 'o', 'n', 0,			//	9 bytes
	's', 'p', 'e', 'e', 'd', 0,							//	6 bytes
	'x', 0,												//	2 bytes
	'y', 0,												//	2 bytes
	'z', 0												//	2 bytes
};

const uint32_t ParseBVH::mk_Key_Animation = 2;
const uint32_t ParseBVH::mk_Key_Channels = mk_Key_Animation + 10;
const uint32_t ParseBVH::mk_Key_Count = mk_Key_Channels + 9;
const uint32_t ParseBVH::mk_Key_End = mk_Key_Count + 6;
const uint32_t ParseBVH::mk_Key_Frame = mk_Key_End + 4;
const uint32_t ParseBVH::mk_Key_Frames = mk_Key_Frame + 6;
const uint32_t ParseBVH::mk_Key_Index = mk_Key_Frames + 7;
const uint32_t ParseBVH::mk_Key_Order = mk_Key_Index + 6;
const uint32_t ParseBVH::mk_Key_Skeleton = mk_Key_Order + 6;
const uint32_t ParseBVH::mk_Key_Speed = mk_Key_Skeleton + 9;
const uint32_t ParseBVH::mk_Key_X = mk_Key_Speed + 6;
const uint32_t ParseBVH::mk_Key_Y = mk_Key_X + 2;
const uint32_t ParseBVH::mk_Key_Z = mk_Key_Y + 2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ParseBVH class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParseBVH::~ParseBVH()
{
}

bool ParseBVH::Parse( void )
{
	if( Items() )
	{
		if( Clone( mk_Standard_Keys, ( sizeof( mk_Standard_Keys ) - 1 ) ) )
		{
			m_channels = 0;
			uint32_t next = m_items;
			if( Hierarchy() )
			{
				uint32_t prev = next;
				next = m_items;
				if( Motion() )
				{
					m_tree[ prev ].next = next;
					m_tree[ next ].prev = prev;
					m_tree[ 0 ].type = QUERY_VALUE_LIST;
					m_tree[ 0 ].data.list.members = 1;
					return( true );
				}
			}
		}
	}
	return( false );
}

bool ParseBVH::Hierarchy( void )
{
	unsigned int read = 0;
	if( Verify( "HIERARCHY", read ) )
	{
		m_parse.offset += read;
		uint32_t item = m_items;
		if( Items() )
		{
			if( Verify( "ROOT", read ) )
			{
				m_parse.offset += read;
				uint32_t next = m_items;
				if( Joint() )
				{
					m_tree[ item ].data.list.members = next;
					m_tree[ next ].parent = item;
					while( Verify( "ROOT", read ) )
					{
						m_parse.offset += read;
						uint32_t prev = next;
						next = m_items;
						if( !Joint() ) return( false );
						m_tree[ next ].parent = item;
						m_tree[ next ].prev = prev;
						m_tree[ prev ].next = next;
					}
					m_tree[ item ].type = QUERY_VALUE_LIST;
					m_tree[ item ].key = mk_Key_Skeleton;
					return( true );
				}
			}
		}
	}
	return( false );
}

bool ParseBVH::Joint( void )
{
	uint32_t item = m_items;
	if( Items() )
	{
		unsigned int read = 0;
		unsigned int index = 0;
		if( String( index, read ) )
		{
			m_tree[ item ].key = m_write.offset;
			if( Clone( &m_parse.buffer[ m_parse.offset + index ], ( read - index ) ) )
			{
				m_parse.offset += read;
				if( Verify( "{", read ) )
				{
					m_parse.offset += read;
					if( Offset( item ) )
					{
						uint32_t next = m_items;
						if( Channels() )
						{
							m_tree[ item ].data.list.members = next;
							m_tree[ next ].parent = item;
							uint32_t prev = next;
							next = m_items;
							while( Verify( "JOINT", read ) )
							{
								m_parse.offset += read;
								if( !Joint() ) return( false );
								m_tree[ prev ].next = next;
								m_tree[ next ].prev = prev;
								m_tree[ next ].parent = item;
								prev = next;
								next = m_items;
							}
							if( Verify( "End Site", read ) )
							{
								m_parse.offset += read;
								if( !EndSite() ) return( false );
								m_tree[ prev ].next = next;
								m_tree[ next ].prev = prev;
								m_tree[ next ].parent = item;
							}
							if( Verify( "}", read ) )
							{
								m_parse.offset += read;
								m_tree[ item ].type = QUERY_VALUE_LIST;
								return( true );
							}
						}
					}
				}
			}
		}
	}
	return( false );
}

bool ParseBVH::EndSite( void )
{
	uint32_t item = m_items;
	if( Items() )
	{
		unsigned int read = 0;
		if( Verify( "{", read ) )
		{
			m_parse.offset += read;
			if( Offset( item ) )
			{
				if( Verify( "}", read ) )
				{
					m_parse.offset += read;
					m_tree[ item ].type = QUERY_VALUE_ITEM;
					m_tree[ item ].key = mk_Key_End;
					return( true );
				}
			}
		}
	}
	return( false );
}

bool ParseBVH::Offset( const uint32_t parent )
{
	unsigned int read = 0;
	if( Verify( "OFFSET", read ) )
	{
		m_parse.offset += read;
		uint32_t item = m_items;
		if( Items( 3 ) )
		{
			m_tree[ parent ].data.list.attribs = item;
			m_tree[ item + 1 ].prev = item;
			m_tree[ item + 2 ].prev = m_tree[ item ].next = ( item + 1 );
			m_tree[ item + 1 ].next = ( item + 2 );
			double number = 0;
			if( Number( number, read ) )
			{
				m_parse.offset += read;
				m_tree[ item ].type = QUERY_VALUE_NUMBER;
				m_tree[ item ].parent = parent;
				m_tree[ item ].key = mk_Key_X;
				m_tree[ item ].data.number = number;
				m_tree[ item ].next = ( item + 1 );
				++item;
				if( Number( number, read ) )
				{
					m_parse.offset += read;
					m_tree[ item ].type = QUERY_VALUE_NUMBER;
					m_tree[ item ].parent = parent;
					m_tree[ item ].key = mk_Key_Y;
					m_tree[ item ].data.number = number;
					m_tree[ item ].prev = ( item - 1 );
					m_tree[ item ].next = ( item + 1 );
					++item;
					if( Number( number, read ) )
					{
						m_parse.offset += read;
						m_tree[ item ].type = QUERY_VALUE_NUMBER;
						m_tree[ item ].parent = parent;
						m_tree[ item ].key = mk_Key_Z;
						m_tree[ item ].data.number = number;
						m_tree[ item ].prev = ( item - 1 );
						return( true );
					}
				}
			}
		}
	}
	return( false );
}

bool ParseBVH::Channels( void )
{
	unsigned int read = 0;
	if( Verify( "CHANNELS", read ) )
	{
		m_parse.offset += read;
		uint32_t channels = 0;
		if( Number( channels, read ) )
		{
			m_parse.offset += read;
			if( ( channels > 0 ) && ( channels < 7 ) )
			{
				uint32_t order = m_write.offset;
				unsigned int index = 0;
				for( uint32_t channel = 0; channel < channels; ++channel )
				{
					if( !String( index, read ) ) return( false );
					if( ( read - index ) != 9 ) return( false );
					index += m_parse.offset;
					if( ( m_parse.buffer[ index ] < 'X' ) || ( m_parse.buffer[ index ] > 'Z' ) ) return( false );
					if( strncmp( reinterpret_cast< const char* const >( &m_parse.buffer[ index + 1 ] ), "position", 8 ) &&
						strncmp( reinterpret_cast< const char* const >( &m_parse.buffer[ index + 1 ] ), "rotation", 8 ) ) return( false );
					if( channel ) if( !Write( ' ' ) ) return( false );
					if( !Write( ( m_parse.buffer[ index + 1 ] == 'p' ) ? 't' : 'r' ) ) return( false );
					if( !Write( m_parse.buffer[ index ] - 'X' + 'x' ) ) return( false );
					m_parse.offset += read;
				}
				if( !Write() ) return( false );
				uint32_t item = m_items;
				if( Items( 4 ) )
				{
					m_tree[ item ].type = QUERY_VALUE_ITEM;
					m_tree[ item ].key = mk_Key_Channels;
					m_tree[ item ].data.list.attribs = ( item + 1 );
					m_tree[ item + 1 ].parent = m_tree[ item + 2 ].parent = m_tree[ item + 3 ].parent = item;
					++item;
					m_tree[ item ].type = QUERY_VALUE_NUMBER;
					m_tree[ item ].key = mk_Key_Index;
					m_tree[ item ].data.number = static_cast< double >( m_channels );
					m_tree[ item ].next = ( item + 1 );
					++item;
					m_tree[ item ].type = QUERY_VALUE_NUMBER;
					m_tree[ item ].key = mk_Key_Count;
					m_tree[ item ].data.number = static_cast< double >( channels );
					m_tree[ item ].prev = ( item - 1 );
					m_tree[ item ].next = ( item + 1 );
					++item;
					m_tree[ item ].type = QUERY_VALUE_TEXT;
					m_tree[ item ].key = mk_Key_Order;
					m_tree[ item ].text = order;
					m_tree[ item ].prev = ( item - 1 );
					m_channels += channels;
					return( true );
				}
			}
		}
	}
	return( false );
}

bool ParseBVH::Motion( void )
{
	if( m_channels )
	{
		unsigned int read = 0;
		if( Verify( "MOTION", read ) )
		{
			m_parse.offset += read;
			if( Verify( "Frames:", read ) )
			{
				m_parse.offset += read;
				uint32_t frames = 0;
				if( Number( frames, read ) )
				{
					m_parse.offset += read;
					if( ( frames > 0 ) && ( frames < ( 30 * 60 * 15 ) ) )	//	limit to 15 minutes of animation
					{
						if( Verify( "Frame Time:", read ) )
						{
							m_parse.offset += read;
							double speed = 0;
							if( Number( speed, read ) )
							{
								m_parse.offset += read;
								uint32_t item = m_items;
								if( Items( 4 ) )
								{
									m_tree[ item ].type = QUERY_VALUE_LIST;
									m_tree[ item ].key = mk_Key_Animation;
									m_tree[ item ].data.list.attribs = ( item + 1 );
									m_tree[ item ].data.list.members = ( item + 4 );
									m_tree[ item + 1 ].type = m_tree[ item + 2 ].type = m_tree[ item + 3 ].type = QUERY_VALUE_NUMBER;
									m_tree[ item + 1 ].parent = m_tree[ item + 2 ].parent = m_tree[ item + 3 ].parent = item;
									m_tree[ item + 1 ].key = mk_Key_Speed;
									m_tree[ item + 2 ].key = mk_Key_Frames;
									m_tree[ item + 3 ].key = mk_Key_Channels;
									m_tree[ item + 1 ].data.number = speed;
									m_tree[ item + 2 ].data.number = static_cast< double >( frames );
									m_tree[ item + 3 ].data.number = static_cast< double >( m_channels );
									m_tree[ item + 2 ].prev = ( item + 1 );
									m_tree[ item + 3 ].prev = m_tree[ item + 1 ].next = ( item + 2 );
									m_tree[ item + 2 ].next = ( item + 3 );
									uint32_t next = m_items;
									if( Frame() )
									{
										m_tree[ next ].parent = item;
										--frames;
										if( !frames ) return( true );
										uint32_t prev = next;
										next = m_items;
										while( Frame() )
										{
											m_tree[ next ].parent = item;
											m_tree[ next ].prev = prev;
											m_tree[ prev ].next = next;
											prev = next;
											next = m_items;
											--frames;
											if( !frames ) return( true );
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return( false );
}

bool ParseBVH::Frame( void )
{
	uint32_t item = m_items;
	if( Items() )
	{
		m_tree[ item ].type = QUERY_VALUE_TEXT;
		m_tree[ item ].key = mk_Key_Frame;
		m_tree[ item ].text = m_write.offset;
		unsigned int read = 0;
		double value = 0;
		char text[ 32 ];
		uint32_t channels = m_channels;
		if( Number( value, read ) )
		{
			m_parse.offset += read;
			sprintf_s( text, 32, "%g", value );
			if( Write( text ) )
			{
				--channels;
				if( !channels ) return( true );
				while( Number( value, read ) )
				{
					m_parse.offset += read;
					sprintf_s( text, 32, "%g", value );
					m_write.buffer[ m_write.offset - 1 ] = ' ';
					if( !Write( text ) ) break;
					--channels;
					if( !channels ) return( true );
				}
			}
		}
	}
	return( false );
}

bool ParseBVH::String( unsigned int& index, unsigned int& read )
{
	return( ascii::ParseWord( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset ] ), index, read ) );
}

bool ParseBVH::Verify( const char* const string, unsigned int& read )
{
	return( ascii::ParseText( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset ] ), string, read ) );
}

bool ParseBVH::Number( double& value, unsigned int& read )
{
	return( ascii::ParseDecimal( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset ] ), value, read ) );
}

bool ParseBVH::Number( uint32_t& value, unsigned int& read )
{
	unsigned long long ull = 0;
	if( ascii::ParseDecimal( reinterpret_cast< const char* const >( &m_parse.buffer[ m_parse.offset ] ), ull, read ) )
	{
		if( ull <= 0x00000000ffffffff )
		{
			value = static_cast< uint32_t >( ull );
			return( true );
		}
	}
	value = 0;
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
