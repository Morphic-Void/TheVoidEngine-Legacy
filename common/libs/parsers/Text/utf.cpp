
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   utf.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	UTF support functions.
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

#include "utf.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin unicode namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace unicode
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    unicode byte-order-marking value and encoding lengths
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int32_t UnicodeBOM = 0x0000feff;
const int32_t LenUTF8_BOM = 3;
const int32_t LenUTF16_BOM = 2;
const int32_t LenUTF32_BOM = 4;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    general unicode functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!	determine if a unicode character is a control character
bool IsCC( const int32_t unicode )
{
	if( ( unicode <= 0x0000001f ) || ( ( unicode >= 0x0000007f ) && ( unicode <= 0x0000009f ) ) )
	{	//	c0 or c1 control code characters
		return( true );
	}
	return( false );
}

//!	determine if a unicode character is white space
bool IsWhite( const int32_t unicode )
{
	if( unicode >= 0x2000 )
	{
		if( unicode <= 0x200a )
		{
			return( true );
		}
		switch( unicode )
		{
		case( 0x2028 ):
		case( 0x2029 ):
		case( 0x202f ):
		case( 0x205f ):
		case( 0x3000 ):
			{
				return( true );
			}
		default:
			{
				return( false );
			}
		}
	}
	else
	{
		if( unicode <= 0x0008 )
		{
			return( false );
		}
		if( unicode <= 0x000d )
		{
			return( true );
		}
		switch( unicode )
		{
		case( 0x0020 ):
		case( 0x0085 ):
		case( 0x00a0 ):
		case( 0x1680 ):
		case( 0x180e ):
		case( 0xfeff ):
			{
				return( true );
			}
		default:
			{
				return( false );
			}
		}
	}
}

//!	determine if a unicode character is ascii text
bool IsAscii( const int32_t unicode )
{
	if( unicode <= 0x0000007e )
	{
		if( ( unicode >= ' ' ) || ( unicode == '\t' ) || ( unicode == '\r' ) || ( unicode == '\n' ) )
		{
			return( true );
		}
	}
	return( false );
}

//!	convert a unicode character to hexadecimal returning -1 if the character is not valid hexadecimal
int32_t UnicodeToHex( const int32_t unicode )
{
	int32_t n = ( unicode - '0' );
	int32_t a = ( ( unicode | 0x20 ) - 'a' );
	return( ( n | ( ( n | ( 9 - n ) ) >> 31 ) ) & ( ( a + 10 ) | ( ( a | ( 5 - a ) ) >> 31 ) ) );
}

//! convert a 4 bit hexadecimal number into it's unicode character representation
int32_t HexToUnicode( const int32_t hex )
{
	int32_t a = ( hex - 10 );
	return( a + 'A' - ( ( a >> 31 ) & 7 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    xml specific unicode functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!	determine if a unicode character is unrestricted XML
bool IsCleanXML( const int32_t unicode )
{
	if( unicode >= 0x0000fde0 )
	{
		if( ( unicode & 0x0000ffff ) <= 0x0000fffd )
		{
			return( true );
		}
	}
	else
	{
		if( unicode <= 0x0000007e )
		{
			if( unicode >= 0x00000020 )
			{
				return( true );
			}
			if( ( unicode == 0x00000009 ) || ( unicode == 0x0000000a ) || ( unicode == 0x0000000d ) )
			{
				return( true );
			}
		}
		else if( unicode >= 0x000000a0 )
		{
			if( unicode >= 0x0000e000 )
			{
				if( unicode <= 0x0000fdef )
				{
					return( true );
				}
			}
			else
			{
				if( unicode <= 0x0000d7ff )
				{
					return( true );
				}
			}
		}
		else if( unicode == 0x00000085 )
		{
			return( true );
		}
	}
	return( false );
}

//!	determine if a unicode character is an XML white space character
bool IsWhiteXML( const int32_t unicode )
{
	if( ( unicode == ' ' ) || ( unicode == '\t' ) || ( unicode == '\r' ) || ( unicode == '\n' ) )
	{
		return( true );
	}
	return( false );
}

//!	determine if a unicode character is an XML name first character
bool IsNameFirstXML( const int32_t unicode )
{
	if( unicode < 0x00002000 )
	{
		if( unicode >= 0x000000c0 )
		{
			if( unicode >= 0x00000300 )
			{
				if( ( unicode >= 0x00000370 ) && ( unicode != 0x0000037e ) )
				{	//	[0x0370-0x37d] [0x037f-0x1fff]
					return( true );
				}
			}
			else
			{
				if( ( unicode != 0x000000d7 ) && ( unicode != 0x000000f7 ) )
				{	//	[0xc0-0xd6] [0xd8-0xf6] [0xf8-0x02ff]
					return( true );
				}
			}
		}
		else if( unicode <= 'Z' )
		{
			if( ( unicode >= 'A' ) || ( unicode == ':' ) )
			{	//	':' [A-Z]
				return( true );
			}
		}
		else if( unicode <= 'z' )
		{
			if( ( unicode >= 'a' ) || ( unicode == '_' ) )
			{	//	'_' [a-z]
				return( true );
			}
		}
	}
	else if( unicode >= 0x0000f900 )
	{
		if( unicode >= 0x0000fffe )
		{
			if( ( unicode >= 0x00010000 ) && ( unicode <= 0x000effff ) )
			{	//	[0x00010000-0x000effff]
				return( true );
			}
		}
		else
		{
			if( ( unicode <= 0x0000fdcf ) || ( unicode >= 0x0000fdf0 ) )
			{	//	[0xf900-0xfdcf] [0xfdf0-0xfffd]
				return( true );
			}
		}
	}
	else if( unicode >= 0x00002070 )
	{
		if( unicode >= 0x00002ff0 )
		{
			if( ( unicode >= 0x00003001 ) && ( unicode <= 0x0000d7ff ) )
			{	//	[0x3001-0xd7ff]
				return( true );
			}
		}
		else
		{
			if( ( unicode <= 0x0000218f ) || ( unicode >= 0x00002c00 ) )
			{	//	[0x2070-0x218f] [0x2c00-0x2fef]
				return( true );
			}
		}
	}
	else if( ( unicode == 0x0000200c ) || ( unicode == 0x0000200d ) )
	{	//	[0x200c-0x200d]
		return( true );
	}
	return( false );
}

//!	determine if a unicode character is an XML name character
bool IsNameXML( const int32_t unicode )
{
	if( unicode < 0x00002000 )
	{
		if( unicode >= 0x000000c0 )
		{
			if( ( unicode != 0x000000d7 ) && ( unicode != 0x000000f7 ) && ( unicode != 0x0000037e ) )
			{	//	[0xc0-0xd6] [0xd8-0xf6] [0xf8-0x037d] [0x037f-0x1fff]
				return( true );
			}
		}
		else if( unicode >= 'a' )
		{
			if( ( unicode <= 'z' ) || ( unicode == 0x000000b7 ) )
			{	//	[a-z] [0xb7]
				return( true );
			}
		}
		else if( unicode >= 'A' )
		{
			if( ( unicode <= 'Z' ) || ( unicode == '_' ) )
			{	//	[A-Z] '_'
				return( true );
			}
		}
		else if( unicode >= '-' )
		{
			if( ( unicode <= ':' ) && ( unicode != '/' ) )
			{	//	'-' '.' [0-9] ':'
				return( true );
			}
		}
	}
	else if( unicode >= 0x0000f900 )
	{
		if( unicode >= 0x0000fffe )
		{
			if( ( unicode >= 0x00010000 ) && ( unicode <= 0x000effff ) )
			{	//	[0x00010000-0x000effff]
				return( true );
			}
		}
		else
		{
			if( ( unicode <= 0x0000fdcf ) || ( unicode >= 0x0000fdf0 ) )
			{	//	[0xf900-0xfdcf] [0xfdf0-0xfffd]
				return( true );
			}
		}
	}
	else if( unicode >= 0x00002070 )
	{
		if( unicode >= 0x00002ff0 )
		{
			if( ( unicode >= 0x00003001 ) && ( unicode <= 0x0000d7ff ) )
			{	//	[0x3001-0xd7ff]
				return( true );
			}
		}
		else
		{
			if( ( unicode <= 0x0000218f ) || ( unicode >= 0x00002c00 ) )
			{	//	[0x2070-0x218f] [0x2c00-0x2fef]
				return( true );
			}
		}
	}
	else if( ( unicode == 0x0000200c ) || ( unicode == 0x0000200d ) || ( unicode == 0x0000203f ) || ( unicode == 0x00002040 ) )
	{	//	[0x200c-0x200d] [0x203f-0x2040]
		return( true );
	}
	return( false );
}

//!	determine if a unicode character is XML post-name character
bool IsNameEndXML( const int32_t unicode )
{
	switch( unicode )
	{
	case( '=' ):
	case( ' ' ):
	case( '\t' ):
	case( '\r' ):
	case( '\n' ):
	case( '/' ):
	case( '>' ):
	case( '?' ):
	case( '[' ):
	case( ']' ):
	case( '&' ):
	case( '#' ):
		{
			return( true );
		}
	default:
		{
			return( false );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    utf type identification function
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!	attempt to identify a utf encoding type by byte-order-marker or 2 leading standard ascii characters (0x01-0x7f) 
UTF_TYPE IdentifyUTF( utf_buffer& utf )
{
	if( utf.offset < utf.length )
	{
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		const unsigned int length = ( utf.length - utf.offset );
		if( length >= 2 )
		{
			if( length >= 3 )
			{
				if( length >= 4 )
				{
					if( ( buffer[ 0 ] == 0xff ) && ( buffer[ 1 ] == 0xfe ) && ( buffer[ 2 ] == 0x00 ) && ( buffer[ 3 ] == 0x00 ) )
					{
						utf.offset += 4;
						return( UTF_TYPE_UTF32le );
					}
					if( ( buffer[ 0 ] == 0x00 ) && ( buffer[ 1 ] == 0x00 ) && ( buffer[ 2 ] == 0xfe ) && ( buffer[ 3 ] == 0xff ) )
					{
						utf.offset += 4;
						return( UTF_TYPE_UTF32be );
					}
				}
				if( ( buffer[ 0 ] == 0xef ) && ( buffer[ 1 ] == 0xbb ) && ( buffer[ 2 ] == 0xbf ) )
				{
					utf.offset += 3;
					return( UTF_TYPE_UTF8 );
				}
			}
			if( ( buffer[ 0 ] == 0xff ) && ( buffer[ 1 ] == 0xfe ) )
			{
				utf.offset += 2;
				return( UTF_TYPE_UTF16le );
			}
			if( ( buffer[ 0 ] == 0xfe ) && ( buffer[ 1 ] == 0xff ) )
			{
				utf.offset += 2;
				return( UTF_TYPE_UTF16be );
			}
		}
		char* const string = reinterpret_cast< char* const >( buffer );
		if( length >= 4 )
		{	//	possible 16-bit or 32-bit utf
			if( length >= 8 )
			{	//	possible 32-bit utf
				if( ( string[ 1 ] == 0 ) && ( string[ 2 ] == 0 ) && ( string[ 5 ] == 0 ) && ( string[ 6 ] == 0 ) )
				{
					if( ( string[ 3 ] == 0 ) && ( string[ 7 ] == 0 ) && ( string[ 0 ] > 0 ) && ( string[ 4 ] > 0 ) )
					{
						return( UTF_TYPE_UTF32le );
					}
					if( ( string[ 0 ] == 0 ) && ( string[ 4 ] == 0 ) && ( string[ 3 ] > 0 ) && ( string[ 7 ] > 0 ) )
					{
						return( UTF_TYPE_UTF32be );
					}
				}
			}
			if( ( string[ 1 ] == 0 ) && ( string[ 3 ] == 0 ) && ( string[ 0 ] > 0 ) && ( string[ 2 ] > 0 ) )
			{
				return( UTF_TYPE_UTF16le );
			}
			if( ( string[ 0 ] == 0 ) && ( string[ 2 ] == 0 ) && ( string[ 1 ] > 0 ) && ( string[ 3 ] > 0 ) )
			{
				return( UTF_TYPE_UTF16be );
			}
		}
	}
	return( UTF_TYPE_UTF8 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    utf-8 handling functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! find the length of a unicode character as utf-8
UNICODE_ERROR LenUTF8( const int32_t unicode, unsigned int& length )
{
	if( unicode < 0x00000000 )
	{	//	illegal unicode value
		length = 0;
		return( UNICODE_ERROR_ILLEGAL_UNICODE );
	}
	if( unicode <= 0x0000007f )
	{	//	1 byte (7 bits)
		length = 1;
	}
	else if( unicode <= 0x000007ff )
	{	//	2 bytes (11 bits)
		length = 2;
	}
	else if( unicode <= 0x0000ffff )
	{	//	3 bytes (16 bits)
		length = 3;
	}
	else if( unicode <= 0x001fffff )
	{	//	4 bytes (21 bits)
		length = 4;
	}
	else if( unicode <= 0x03ffffff )
	{	//	5 bytes (26 bits)
		length = 5;
	}
	else
	{	//	6 bytes (31 bits)
		length = 6;
	}
	if( ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_WARNING_ILLEGAL_UNICODE );
	}
	return( UNICODE_SUCCESS );
}

//! find the length in characters and bytes (excluding the 0 terminator) of a utf-8 string
UNICODE_ERROR LenUTF8( const uint8_t* const string, unsigned int& chars, unsigned int& bytes )
{
	chars = 0;
	bytes = 0;
	UNICODE_ERROR error = UNICODE_ERROR_BUFFER_UNDERFLOW;
	if( string )
	{
		unsigned int length = 0;
		while( string[ length ] )
		{
			++length;
		}
		utf_buffer utf;
		utf.length = ( length + 1 );
		utf.offset = 0;
		utf.buffer = const_cast< uint8_t* const >( string );
		int32_t unicode = 0;
		for( unsigned int count = 0; ( ( error = GetUTF8( utf, unicode ) ) == UNICODE_SUCCESS ); ++count )
		{
			if( unicode == 0 )
			{
				chars = count;
				bytes = length;
				break;
			}
		}
	}
	return( error );
}

//! read a unicode character as utf-8
UNICODE_ERROR GetUTF8( utf_buffer& utf, int32_t& unicode )
{
	unicode = 0;
	if( utf.offset >= utf.length )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	uint8_t* const buffer = &utf.buffer[ utf.offset ];
	uint8_t byte = buffer[ 0 ];
	if( byte <= 0x7f )
	{	//	1 byte (7 bits)
		unicode = static_cast< int32_t >( byte );
		++utf.offset;
		return( UNICODE_SUCCESS ); 
	}
	unsigned int bytes;
	if( byte <= 0xbf )
	{	//	unexpected continuation byte
		return( UNICODE_ERROR_UNEXPECTED_CONTINUATION );
	}
	else if( byte <= 0xdf )
	{	//	2 bytes (11 bits)
		bytes = 2;
	}
	else if( byte <= 0xef )
	{	//	3 bytes (16 bits)
		bytes = 3;
	}
	else if( byte <= 0xf7 )
	{	//	4 bytes (21 bits)
		bytes = 4;
	}
	else if( byte <= 0xfb )
	{	//	5 bytes (26 bits)
		bytes = 5;
	}
	else if( byte <= 0xfd )
	{	//	6 bytes (31 bits)
		bytes = 6;
	}
	else
	{	//	invalid leading byte
		return( UNICODE_ERROR_INVALID_ENCODING );
	}
	const unsigned int length = ( utf.length - utf.offset );
	if( length < bytes )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	int32_t value = ( static_cast< int32_t >( byte ) & ( ( int32_t( 1 ) << ( 7 - bytes ) ) - 1 ) );
	for( unsigned int continuation = 1; continuation < bytes; ++continuation )
	{
		byte = buffer[ continuation ];
		if( ( byte <= 0x7f ) || ( byte >= 0xc0 ) )
		{
			return( UNICODE_ERROR_MISSING_CONTINUATION );
		}
		value = ( ( value << 6 ) | ( byte & 0x3f ) );
	}
	unicode = value;
	utf.offset += bytes;
	if( ( value > 0x0010ffff ) || ( ( value >= 0x0000d800 ) && ( value < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_WARNING_ILLEGAL_UNICODE );
	}
	if( value < ( int32_t( 1 ) << ( ( ( int32_t( 26 ) << 20 ) | ( int32_t( 21 ) << 15 ) | ( int32_t( 16 ) << 10 ) | ( int32_t( 11 ) << 5 ) | int32_t( 7 ) ) >> ( ( bytes - 2 ) * 5 ) ) ) )
	{	//	unicode character used an overlong encoding
		if( ( value == 0 ) && ( bytes == 2 ) )
		{	//	modified utf-8 encoding of U+0000, this is a common exception to the overlong encoding rule
			return( UNICODE_WARNING_MODIFIED_ENCODING );
		}
		return( UNICODE_WARNING_OVERLONG_ENCODING );
	}
	return( UNICODE_SUCCESS );
}

//! write a unicode character as utf-8
UNICODE_ERROR SetUTF8( utf_buffer& utf, const int32_t unicode )
{
	if( unicode < 0x00000000 )
	{	//	illegal unicode value
		return( UNICODE_ERROR_ILLEGAL_UNICODE );
	}
	if( utf.offset >= utf.length )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	if( unicode <= 0x0000007f )
	{	//	1 byte (7 bits)
		utf.buffer[ utf.offset ] = static_cast< uint8_t >( unicode );
		++utf.offset;
	}
	else if( unicode <= 0x000007ff )
	{	//	2 bytes (11 bits)
		if( ( utf.length - utf.offset ) < 2 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = ( static_cast< uint8_t >( unicode >> 6 ) | 0xc0 );
		buffer[ 1 ] = ( ( static_cast< uint8_t >( unicode ) & 0x3f ) | 0x80 );
		utf.offset += 2;
	}
	else if( unicode <= 0x0000ffff )
	{	//	3 bytes (16 bits)
		if( ( utf.length - utf.offset ) < 3 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = ( static_cast< uint8_t >( unicode >> 12 ) | 0xe0 );
		buffer[ 1 ] = ( ( static_cast< uint8_t >( unicode >> 6 ) & 0x3f ) | 0x80 );
		buffer[ 2 ] = ( ( static_cast< uint8_t >( unicode ) & 0x3f ) | 0x80 );
		utf.offset += 3;
	}
	else if( unicode <= 0x001fffff )
	{	//	4 bytes (21 bits)
		if( ( utf.length - utf.offset ) < 4 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = ( static_cast< uint8_t >( unicode >> 18 ) | 0xf0 );
		buffer[ 1 ] = ( ( static_cast< uint8_t >( unicode >> 12 ) & 0x3f ) | 0x80 );
		buffer[ 2 ] = ( ( static_cast< uint8_t >( unicode >> 6 ) & 0x3f ) | 0x80 );
		buffer[ 3 ] = ( ( static_cast< uint8_t >( unicode ) & 0x3f ) | 0x80 );
		utf.offset += 4;
	}
	else if( unicode <= 0x03ffffff )
	{	//	5 bytes (26 bits)
		if( ( utf.length - utf.offset ) < 5 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = ( static_cast< uint8_t >( unicode >> 24 ) | 0xf8 );
		buffer[ 1 ] = ( ( static_cast< uint8_t >( unicode >> 18 ) & 0x3f ) | 0x80 );
		buffer[ 2 ] = ( ( static_cast< uint8_t >( unicode >> 12 ) & 0x3f ) | 0x80 );
		buffer[ 3 ] = ( ( static_cast< uint8_t >( unicode >> 6 ) & 0x3f ) | 0x80 );
		buffer[ 4 ] = ( ( static_cast< uint8_t >( unicode ) & 0x3f ) | 0x80 );
		utf.offset += 5;
	}
	else
	{	//	6 bytes (31 bits)
		if( ( utf.length - utf.offset ) < 6 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = ( ( static_cast< uint8_t >( unicode >> 30 ) & 0x01 ) | 0xfc );
		buffer[ 1 ] = ( ( static_cast< uint8_t >( unicode >> 24 ) & 0x3f ) | 0x80 );
		buffer[ 2 ] = ( ( static_cast< uint8_t >( unicode >> 18 ) & 0x3f ) | 0x80 );
		buffer[ 3 ] = ( ( static_cast< uint8_t >( unicode >> 12 ) & 0x3f ) | 0x80 );
		buffer[ 4 ] = ( ( static_cast< uint8_t >( unicode >> 6 ) & 0x3f ) | 0x80 );
		buffer[ 5 ] = ( ( static_cast< uint8_t >( unicode ) & 0x3f ) | 0x80 );
		utf.offset += 6;
	}
	if( ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_WARNING_ILLEGAL_UNICODE );
	}
	return( UNICODE_SUCCESS );
}

//! write a zero terminated ascii string as utf-8
UNICODE_ERROR SetUTF8( utf_buffer& utf, const char* const string )
{
	for( unsigned int index = 0; string[ index ]; ++index )
	{
		UNICODE_ERROR error = SetUTF8( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

//! write a specified length ascii string as utf-8
UNICODE_ERROR SetUTF8( utf_buffer& utf, const char* const string, const unsigned int length )
{
	for( unsigned int index = 0; index < length; ++index )
	{
		UNICODE_ERROR error = SetUTF8( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

//! write a modified utf-8 null value
UNICODE_ERROR SetUTF8_Null( utf_buffer& utf )
{
	if( utf.offset >= utf.length )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	if( ( utf.length - utf.offset ) < 2 )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	uint8_t* const buffer = &utf.buffer[ utf.offset ];
	buffer[ 0 ] = 0xc0;
	buffer[ 1 ] = 0x80;
	utf.offset += 2;
	return( UNICODE_SUCCESS );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    utf-16 handling functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! find the length of a unicode character as utf-16
UNICODE_ERROR LenUTF16( const int32_t unicode, unsigned int& length )
{
	if( ( unicode < 0x00000000 ) || ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		length = 0;
		return( UNICODE_ERROR_ILLEGAL_UNICODE );
	}
	length = ( ( unicode <= 0x0000ffff ) ? 2 : 4 );
	return( UNICODE_SUCCESS );
}

//! find the length in characters and bytes (excluding the 0 terminator) of a little-endian utf-16 string
UNICODE_ERROR LenUTF16le( const uint8_t* const string, unsigned int& chars, unsigned int& bytes )
{
	chars = 0;
	bytes = 0;
	UNICODE_ERROR error = UNICODE_ERROR_BUFFER_UNDERFLOW;
	if( string )
	{
		unsigned int length = 0;
		while( string[ length + 0 ] || string[ length + 1 ] )
		{
			length += 2;
		}
		utf_buffer utf;
		utf.length = ( length + 2 );
		utf.offset = 0;
		utf.buffer = const_cast< uint8_t* const >( string );
		int32_t unicode = 0;
		for( unsigned int count = 0; ( ( error = GetUTF16le( utf, unicode ) ) == UNICODE_SUCCESS ); ++count )
		{
			if( unicode == 0 )
			{
				chars = count;
				bytes = length;
				break;
			}
		}
	}
	return( error );
}

//! find the length in characters and bytes (excluding the 0 terminator) of a big-endian utf-16 string
UNICODE_ERROR LenUTF16be( const uint8_t* const string, unsigned int& chars, unsigned int& bytes )
{
	chars = 0;
	bytes = 0;
	UNICODE_ERROR error = UNICODE_ERROR_BUFFER_UNDERFLOW;
	if( string )
	{
		unsigned int length = 0;
		while( string[ length + 1 ] || string[ length + 0 ] )
		{
			length += 2;
		}
		utf_buffer utf;
		utf.length = ( length + 2 );
		utf.offset = 0;
		utf.buffer = const_cast< uint8_t* const >( string );
		int32_t unicode = 0;
		for( unsigned int count = 0; ( ( error = GetUTF16be( utf, unicode ) ) == UNICODE_SUCCESS ); ++count )
		{
			if( unicode == 0 )
			{
				chars = count;
				bytes = length;
				break;
			}
		}
	}
	return( error );
}

//! read a unicode character as little-endian utf-16
UNICODE_ERROR GetUTF16le( utf_buffer& utf, int32_t& unicode )
{
	unicode = 0;
	if( utf.offset >= utf.length )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	const unsigned int length = ( utf.length - utf.offset );
	if( length < 2 )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	uint8_t* const buffer = &utf.buffer[ utf.offset ];
	int32_t code1 = ( ( static_cast< int32_t >( buffer[ 1 ] ) << 8 ) + buffer[ 0 ] );
	if( ( code1 >= 0x0000d800 ) && ( code1 < 0x0000e000 ) )
	{	//	found surrogate
		if( code1 >= 0x0000dc00 )
		{	//	found unexpected low surrogate (second code unit)
			return( UNICODE_ERROR_UNEXPECTED_LOW_SURROGATE );
		}
		if( length < 4 )
		{	//	buffer underflow
			return( UNICODE_ERROR_BUFFER_UNDERFLOW );
		}
		int32_t code2 = ( ( static_cast< int32_t >( buffer[ 3 ] ) << 8 ) + buffer[ 2 ] );
		if( ( code2 < 0x0000dc00 ) || ( code2 >= 0x0000e000 ) )
		{	//	invalid surrogate
			return( UNICODE_ERROR_INVALID_SURROGATE );
		}
		unicode = ( ( ( code1 & 0x000003ff ) << 10 ) + ( code2 & 0x000003ff ) + 0x00010000 );
		utf.offset += 4;
	}
	else
	{
		unicode = code1;
		utf.offset += 2;
	}
	return( UNICODE_SUCCESS );
}

//! read a unicode character as big-endian utf-16
UNICODE_ERROR GetUTF16be( utf_buffer& utf, int32_t& unicode )
{
	unicode = 0;
	if( utf.offset >= utf.length )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	const unsigned int length = ( utf.length - utf.offset );
	if( length < 2 )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	uint8_t* const buffer = &utf.buffer[ utf.offset ];
	int32_t code1 = ( ( static_cast< int32_t >( buffer[ 0 ] ) << 8 ) + buffer[ 1 ] );
	if( ( code1 >= 0x0000d800 ) && ( code1 < 0x0000e000 ) )
	{	//	found surrogate
		if( code1 >= 0x0000dc00 )
		{	//	found unexpected low surrogate (second code unit)
			return( UNICODE_ERROR_UNEXPECTED_LOW_SURROGATE );
		}
		if( length < 4 )
		{	//	buffer underflow
			return( UNICODE_ERROR_BUFFER_UNDERFLOW );
		}
		int32_t code2 = ( ( static_cast< int32_t >( buffer[ 2 ] ) << 8 ) + buffer[ 3 ] );
		if( ( code2 < 0x0000dc00 ) || ( code2 >= 0x0000e000 ) )
		{	//	invalid surrogate
			return( UNICODE_ERROR_INVALID_SURROGATE );
		}
		unicode = ( ( ( code1 & 0x000003ff ) << 10 ) + ( code2 & 0x000003ff ) + 0x00010000 );
		utf.offset += 4;
	}
	else
	{
		unicode = code1;
		utf.offset += 2;
	}
	return( UNICODE_SUCCESS );
}

//! write a unicode character as little-endian utf-16
UNICODE_ERROR SetUTF16le( utf_buffer& utf, const int32_t unicode )
{
	if( ( unicode < 0x00000000 ) || ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_ERROR_ILLEGAL_UNICODE );
	}
	if( utf.offset >= utf.length )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	if( unicode <= 0x0000ffff )
	{
		if( ( utf.length - utf.offset ) < 2 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = static_cast< uint8_t >( unicode );
		buffer[ 1 ] = static_cast< uint8_t >( unicode >> 8 );
		utf.offset += 2;
	}
	else
	{
		if( ( utf.length - utf.offset ) < 4 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		int32_t surrogate = ( unicode - 0x00010000 );
		surrogate = ( ( ( ( surrogate >> 10 ) | ( surrogate << 16 ) ) & 0x03ff03ff ) | 0xdc00d800 );
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = static_cast< uint8_t >( surrogate );
		buffer[ 1 ] = static_cast< uint8_t >( surrogate >> 8 );
		buffer[ 2 ] = static_cast< uint8_t >( surrogate >> 16 );
		buffer[ 3 ] = static_cast< uint8_t >( surrogate >> 24 );
		utf.offset += 4;
	}
	return( UNICODE_SUCCESS );
}

//! write a unicode character as big-endian utf-16
UNICODE_ERROR SetUTF16be( utf_buffer& utf, const int32_t unicode )
{
	if( ( unicode < 0x00000000 ) || ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_ERROR_ILLEGAL_UNICODE );
	}
	if( utf.offset >= utf.length )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	if( unicode <= 0x0000ffff )
	{
		if( ( utf.length - utf.offset ) < 2 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = static_cast< uint8_t >( unicode >> 8 );
		buffer[ 1 ] = static_cast< uint8_t >( unicode );
		utf.offset += 2;
	}
	else
	{
		if( ( utf.length - utf.offset ) < 4 )
		{	//	buffer overflow
			return( UNICODE_ERROR_BUFFER_OVERFLOW );
		}
		int32_t surrogate = ( unicode - 0x00010000 );
		surrogate = ( ( ( ( surrogate >> 10 ) | ( surrogate << 16 ) ) & 0x03ff03ff ) | 0xdc00d800 );
		uint8_t* const buffer = &utf.buffer[ utf.offset ];
		buffer[ 0 ] = static_cast< uint8_t >( surrogate >> 8 );
		buffer[ 1 ] = static_cast< uint8_t >( surrogate );
		buffer[ 2 ] = static_cast< uint8_t >( surrogate >> 24 );
		buffer[ 3 ] = static_cast< uint8_t >( surrogate >> 16 );
		utf.offset += 4;
	}
	return( UNICODE_SUCCESS );
}

//! write a zero terminated ascii string as little endian utf-16
UNICODE_ERROR SetUTF16le( utf_buffer& utf, const char* const string )
{
	for( unsigned int index = 0; string[ index ]; ++index )
	{
		UNICODE_ERROR error = SetUTF16le( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

//! write a zero terminated ascii string as big endian utf-16
UNICODE_ERROR SetUTF16be( utf_buffer& utf, const char* const string )
{
	for( unsigned int index = 0; string[ index ]; ++index )
	{
		UNICODE_ERROR error = SetUTF16be( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

//! write a specified length ascii string as little endian utf-16
UNICODE_ERROR SetUTF16le( utf_buffer& utf, const char* const string, const unsigned int length )
{
	for( unsigned int index = 0; index < length; ++index )
	{
		UNICODE_ERROR error = SetUTF16le( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

//! write a specified length ascii string as big endian utf-16
UNICODE_ERROR SetUTF16be( utf_buffer& utf, const char* const string, const unsigned int length )
{
	for( unsigned int index = 0; index < length; ++index )
	{
		UNICODE_ERROR error = SetUTF16be( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    utf-32 handling functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! find the length of a unicode character as utf-32
UNICODE_ERROR LenUTF32( const int32_t unicode, unsigned int& length )
{
	length = 4;
	if( ( unicode < 0x00000000 ) || ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_WARNING_ILLEGAL_UNICODE );
	}
	return( UNICODE_SUCCESS );
}

//! find the length in characters and bytes (excluding the 0 terminator) of a little-endian utf-32 string
UNICODE_ERROR LenUTF32le( const uint8_t* const string, unsigned int& chars, unsigned int& bytes )
{
	chars = 0;
	bytes = 0;
	UNICODE_ERROR error = UNICODE_ERROR_BUFFER_UNDERFLOW;
	if( string )
	{
		unsigned int length = 0;
		while( string[ length + 0 ] || string[ length + 1 ] || string[ length + 2 ] || string[ length + 3 ] )
		{
			length += 4;
		}
		utf_buffer utf;
		utf.length = ( length + 4 );
		utf.offset = 0;
		utf.buffer = const_cast< uint8_t* const >( string );
		int32_t unicode = 0;
		for( unsigned int count = 0; ( ( error = GetUTF32le( utf, unicode ) ) == UNICODE_SUCCESS ); ++count )
		{
			if( unicode == 0 )
			{
				chars = count;
				bytes = length;
				break;
			}
		}
	}
	return( error );
}

//! find the length in characters and bytes (excluding the 0 terminator) of a big-endian utf-32 string
UNICODE_ERROR LenUTF32be( const uint8_t* const string, unsigned int& chars, unsigned int& bytes )
{
	chars = 0;
	bytes = 0;
	UNICODE_ERROR error = UNICODE_ERROR_BUFFER_UNDERFLOW;
	if( string )
	{
		unsigned int length = 0;
		while( string[ length + 3 ] || string[ length + 2 ] || string[ length + 1 ] || string[ length + 0 ] )
		{
			length += 4;
		}
		utf_buffer utf;
		utf.length = ( length + 4 );
		utf.offset = 0;
		utf.buffer = const_cast< uint8_t* const >( string );
		int32_t unicode = 0;
		for( unsigned int count = 0; ( ( error = GetUTF32be( utf, unicode ) ) == UNICODE_SUCCESS ); ++count )
		{
			if( unicode == 0 )
			{
				chars = count;
				bytes = length;
				break;
			}
		}
	}
	return( error );
}

//! read a unicode character as little-endian utf-32
UNICODE_ERROR GetUTF32le( utf_buffer& utf, int32_t& unicode )
{
	unicode = 0;
	if( utf.offset >= utf.length )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	if( ( utf.length - utf.offset ) < 4 )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	uint8_t* const buffer = &utf.buffer[ utf.offset ];
	unicode = ( ( ( ( ( ( static_cast< int32_t >( 
		buffer[ 3 ] ) << 8 ) +
		buffer[ 2 ] ) << 8 ) +
		buffer[ 1 ] ) << 8 ) +
		buffer[ 0 ] );
	utf.offset += 4;
	if( ( unicode < 0x00000000 ) || ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_WARNING_ILLEGAL_UNICODE );
	}
	return( UNICODE_SUCCESS );
}

//! read a unicode character as big-endian utf-32
UNICODE_ERROR GetUTF32be( utf_buffer& utf, int32_t& unicode )
{
	unicode = 0;
	if( utf.offset >= utf.length )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	if( ( utf.length - utf.offset ) < 4 )
	{	//	buffer underflow
		return( UNICODE_ERROR_BUFFER_UNDERFLOW );
	}
	uint8_t* const buffer = &utf.buffer[ utf.offset ];
	unicode = ( ( ( ( ( ( static_cast< int32_t >( 
		buffer[ 0 ] ) << 8 ) + 
		buffer[ 1 ] ) << 8 ) + 
		buffer[ 2 ] ) << 8 ) + 
		buffer[ 3 ] );
	utf.offset += 4;
	if( ( unicode < 0x00000000 ) || ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_WARNING_ILLEGAL_UNICODE );
	}
	return( UNICODE_SUCCESS );
}

//! write a unicode character as little-endian utf-32
UNICODE_ERROR SetUTF32le( utf_buffer& utf, const int32_t unicode )
{
	if( utf.offset >= utf.length )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	if( ( utf.length - utf.offset ) < 4 )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	uint8_t* const buffer = &utf.buffer[ utf.offset ];
	buffer[ 0 ] = static_cast< uint8_t >( unicode );
	buffer[ 1 ] = static_cast< uint8_t >( unicode >> 8 );
	buffer[ 2 ] = static_cast< uint8_t >( unicode >> 16 );
	buffer[ 3 ] = static_cast< uint8_t >( unicode >> 24 );
	utf.offset += 4;
	if( ( unicode < 0x00000000 ) || ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_WARNING_ILLEGAL_UNICODE );
	}
	return( UNICODE_SUCCESS );
}

//! write a unicode character as big-endian utf-32
UNICODE_ERROR SetUTF32be( utf_buffer& utf, const int32_t unicode )
{
	if( utf.offset >= utf.length )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	if( ( utf.length - utf.offset ) < 4 )
	{	//	buffer overflow
		return( UNICODE_ERROR_BUFFER_OVERFLOW );
	}
	uint8_t* const buffer = &utf.buffer[ utf.offset ];
	buffer[ 0 ] = static_cast< uint8_t >( unicode >> 24 );
	buffer[ 1 ] = static_cast< uint8_t >( unicode >> 16 );
	buffer[ 2 ] = static_cast< uint8_t >( unicode >> 8 );
	buffer[ 3 ] = static_cast< uint8_t >( unicode );
	utf.offset += 4;
	if( ( unicode < 0x00000000 ) || ( unicode > 0x0010ffff ) || ( ( unicode >= 0x0000d800 ) && ( unicode < 0x0000e000 ) ) )
	{	//	illegal unicode value
		return( UNICODE_WARNING_ILLEGAL_UNICODE );
	}
	return( UNICODE_SUCCESS );
}

//! write a zero terminated ascii string as little endian utf-32
UNICODE_ERROR SetUTF32le( utf_buffer& utf, const char* const string )
{
	for( unsigned int index = 0; string[ index ]; ++index )
	{
		UNICODE_ERROR error = SetUTF32le( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

//! write a zero terminated ascii string as big endian utf-32
UNICODE_ERROR SetUTF32be( utf_buffer& utf, const char* const string )
{
	for( unsigned int index = 0; string[ index ]; ++index )
	{
		UNICODE_ERROR error = SetUTF32be( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

//! write a specified length ascii string as little endian utf-32
UNICODE_ERROR SetUTF32le( utf_buffer& utf, const char* const string, const unsigned int length )
{
	for( unsigned int index = 0; index < length; ++index )
	{
		UNICODE_ERROR error = SetUTF32le( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

//! write a specified length ascii string as big endian utf-32
UNICODE_ERROR SetUTF32be( utf_buffer& utf, const char* const string, const unsigned int length )
{
	for( unsigned int index = 0; index < length; ++index )
	{
		UNICODE_ERROR error = SetUTF32be( utf, string[ index ] );
		if( error != UNICODE_SUCCESS )
		{
			return( error );
		}
	}
	return( UNICODE_SUCCESS );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    unicode type handling function abstraction classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UTF_TYPE CUTF8::Type( void ) const
{
	return( UTF_TYPE_UTF8 );
}

UNICODE_ERROR CUTF8::Len( const int32_t unicode, unsigned int& length ) const
{
	return( LenUTF8( unicode, length ) );
}

UNICODE_ERROR CUTF8::Len( const uint8_t* const string, unsigned int& chars, unsigned int& bytes ) const
{
	return( LenUTF8( string, chars, bytes ) );
}

UNICODE_ERROR CUTF8::Get( utf_buffer& utf, int32_t& unicode ) const
{
	return( GetUTF8( utf, unicode ) );
}

UNICODE_ERROR CUTF8::Set( utf_buffer& utf, const int32_t unicode ) const
{
	return( SetUTF8( utf, unicode ) );
}

UNICODE_ERROR CUTF8::Set( utf_buffer& utf, const char* const string ) const
{
	return( SetUTF8( utf, string ) );
}

UNICODE_ERROR CUTF8::Set( utf_buffer& utf, const char* const string, const unsigned int length ) const
{
	return( SetUTF8( utf, string, length ) );
}

UNICODE_ERROR CUTF8::Set_Null( utf_buffer& utf ) const
{
	return( SetUTF8_Null( utf ) );
}

UTF_TYPE CUTF16le::Type( void ) const
{
	return( UTF_TYPE_UTF16le );
}

UNICODE_ERROR CUTF16le::Len( const int32_t unicode, unsigned int& length ) const
{
	return( LenUTF16( unicode, length ) );
}

UNICODE_ERROR CUTF16le::Len( const uint8_t* const string, unsigned int& chars, unsigned int& bytes ) const
{
	return( LenUTF16le( string, chars, bytes ) );
}

UNICODE_ERROR CUTF16le::Get( utf_buffer& utf, int32_t& unicode ) const
{
	return( GetUTF16le( utf, unicode ) );
}

UNICODE_ERROR CUTF16le::Set( utf_buffer& utf, const int32_t unicode ) const
{
	return( SetUTF16le( utf, unicode ) );
}

UNICODE_ERROR CUTF16le::Set( utf_buffer& utf, const char* const string ) const
{
	return( SetUTF16le( utf, string ) );
}

UNICODE_ERROR CUTF16le::Set( utf_buffer& utf, const char* const string, const unsigned int length ) const
{
	return( SetUTF16le( utf, string, length ) );
}

UNICODE_ERROR CUTF16le::Set_Null( utf_buffer& utf ) const
{
	return( Set( utf ) );
}

UTF_TYPE CUTF16be::Type( void ) const
{
	return( UTF_TYPE_UTF16be );
}

UNICODE_ERROR CUTF16be::Len( const int32_t unicode, unsigned int& length ) const
{
	return( LenUTF16( unicode, length ) );
}

UNICODE_ERROR CUTF16be::Len( const uint8_t* const string, unsigned int& chars, unsigned int& bytes ) const
{
	return( LenUTF16be( string, chars, bytes ) );
}

UNICODE_ERROR CUTF16be::Get( utf_buffer& utf, int32_t& unicode ) const
{
	return( GetUTF16be( utf, unicode ) );
}

UNICODE_ERROR CUTF16be::Set( utf_buffer& utf, const int32_t unicode ) const
{
	return( SetUTF16be( utf, unicode ) );
}

UNICODE_ERROR CUTF16be::Set( utf_buffer& utf, const char* const string ) const
{
	return( SetUTF16be( utf, string ) );
}

UNICODE_ERROR CUTF16be::Set( utf_buffer& utf, const char* const string, const unsigned int length ) const
{
	return( SetUTF16be( utf, string, length ) );
}

UNICODE_ERROR CUTF16be::Set_Null( utf_buffer& utf ) const
{
	return( Set( utf ) );
}

UTF_TYPE CUTF32le::Type( void ) const
{
	return( UTF_TYPE_UTF32le );
}

UNICODE_ERROR CUTF32le::Len( const int32_t unicode, unsigned int& length ) const
{
	return( LenUTF32( unicode, length ) );
}

UNICODE_ERROR CUTF32le::Len( const uint8_t* const string, unsigned int& chars, unsigned int& bytes ) const
{
	return( LenUTF32le( string, chars, bytes ) );
}

UNICODE_ERROR CUTF32le::Get( utf_buffer& utf, int32_t& unicode ) const
{
	return( GetUTF32le( utf, unicode ) );
}

UNICODE_ERROR CUTF32le::Set( utf_buffer& utf, const int32_t unicode ) const
{
	return( SetUTF32le( utf, unicode ) );
}

UNICODE_ERROR CUTF32le::Set( utf_buffer& utf, const char* const string ) const
{
	return( SetUTF32le( utf, string ) );
}

UNICODE_ERROR CUTF32le::Set( utf_buffer& utf, const char* const string, const unsigned int length ) const
{
	return( SetUTF32le( utf, string, length ) );
}

UNICODE_ERROR CUTF32le::Set_Null( utf_buffer& utf ) const
{
	return( Set( utf ) );
}

UTF_TYPE CUTF32be::Type( void ) const
{
	return( UTF_TYPE_UTF32be );
}

UNICODE_ERROR CUTF32be::Len( const int32_t unicode, unsigned int& length ) const
{
	return( LenUTF32( unicode, length ) );
}

UNICODE_ERROR CUTF32be::Len( const uint8_t* const string, unsigned int& chars, unsigned int& bytes ) const
{
	return( LenUTF32be( string, chars, bytes ) );
}

UNICODE_ERROR CUTF32be::Get( utf_buffer& utf, int32_t& unicode ) const
{
	return( GetUTF32be( utf, unicode ) );
}

UNICODE_ERROR CUTF32be::Set( utf_buffer& utf, const int32_t unicode ) const
{
	return( SetUTF32be( utf, unicode ) );
}

UNICODE_ERROR CUTF32be::Set( utf_buffer& utf, const char* const string ) const
{
	return( SetUTF32be( utf, string ) );
}

UNICODE_ERROR CUTF32be::Set( utf_buffer& utf, const char* const string, const unsigned int length ) const
{
	return( SetUTF32be( utf, string, length ) );
}

UNICODE_ERROR CUTF32be::Set_Null( utf_buffer& utf ) const
{
	return( Set( utf ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    unicode type function abstraction function
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const IUTF* const GetHandlerUTF( const UTF_TYPE type )
{
	switch( type )
	{
	case( UTF_TYPE_UTF8 ):
		{
			static const CUTF8 handler;
			return( &handler );
		}
	case( UTF_TYPE_UTF16le ):
		{
			static const CUTF16le handler;
			return( &handler );
		}
	case( UTF_TYPE_UTF16be ):
		{
			static const CUTF16be handler;
			return( &handler );
		}
	case( UTF_TYPE_UTF32le ):
		{
			static const CUTF32le handler;
			return( &handler );
		}
	case( UTF_TYPE_UTF32be ):
		{
			static const CUTF32be handler;
			return( &handler );
		}
	default:
		{
			return( NULL );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end unicode namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace unicode

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
