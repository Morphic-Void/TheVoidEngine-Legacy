
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   ascii.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Ascii text parsing support functions.
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

#include "ascii.h"
#include <math.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin ascii namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ascii
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    general text parsing helper functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!	determine if an ascii character is a control character
bool IsCC( const char ascii )
{
	if( ( ( ascii & 0x7f ) < ' ' ) || ( ascii == 0x7f ) )
	{
		return( true );
	}
	return( false );
}

//!	determine if an ascii character is white space
bool IsWhite( const char ascii )
{
	if( ( ascii == ' ' ) || ( ascii == '\t' ) || ( ascii == '\r' ) || ( ascii == '\n' ) )
	{
		return( true );
	}
	return( false );
}

//!	determine if an ascii character is a simple text character (excluding white space)
bool IsBlack( const char ascii )
{
	if( ascii > ' ' )
	{
		if( ascii < 0x7f )
		{
			return( true );
		}
	}
	return( false );
}

//!	determine if an ascii character is a simple text character (including white space)
bool IsText( const char ascii )
{
	if( ascii >= ' ' )
	{
		if( ascii < 0x7f )
		{
			return( true );
		}
	}
	else if( ( ascii == '\t' ) || ( ascii == '\r' ) || ( ascii == '\n' ) )
	{
		return( true );
	}
	return( false );
}

//!	determine if an ascii character is an XML name first character
bool IsNameFirstXML( const char ascii )
{
	if( ascii < 0 )
	{
		const unsigned char byte( ascii );
		if( ( byte >= 0xc0 ) && ( byte != 0xd7 ) && ( byte != 0xf7 ) )
		{	//	[0xc0-0xd6] [0xd8-0xf6] [0xf8-0xff]
			return( true );
		}
	}
	else if( ascii <= 'Z' )
	{
		if( ( ascii >= 'A' ) || ( ascii == ':' ) )
		{	//	':' [A-Z]
			return( true );
		}
	}
	else if( ascii <= 'z' )
	{
		if( ( ascii >= 'a' ) || ( ascii == '_' ) )
		{	//	'_' [a-z]
			return( true );
		}
	}
	return( false );
}

//!	determine if an ascii character is an XML name character
bool IsNameXML( const char ascii )
{
	if( ascii < 0 )
	{
		const unsigned char byte( ascii );
		if( byte >= 0xc0 )
		{
			if( ( byte != 0xd7 ) && ( byte != 0xf7 ) )
			{	//	[0xc0-0xd6] [0xd8-0xf6] [0xf8-0xff]
				return( true );
			}
		}
		else if( byte == 0xb7 )
		{
			return( true );
		}
	}
	else if( ascii <= ':' )
	{
		if( ( ascii >= '-' ) && ( ascii != '/' ) )
		{	//	'-' '.' [0-9] ':'
			return( true );
		}
	}
	else if( ascii <= 'Z' )
	{
		if( ( ascii >= 'A' ) || ( ascii == ':' ) )
		{	//	':' [A-Z]
			return( true );
		}
	}
	else if( ascii <= 'z' )
	{
		if( ( ascii >= 'a' ) || ( ascii == '_' ) )
		{	//	'_' [a-z]
			return( true );
		}
	}
	return( false );
}

//!	determine if an ascii character is XML post-name character
bool IsNameEndXML( const char ascii )
{
	switch( ascii )
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
	case( 0 ):
		{
			return( true );
		}
	default:
		{
			return( false );
		}
	}
}

//!	convert an ascii character to hexadecimal returning -1 if the character is not valid hexadecimal
char AsciiToHex( const char ascii )
{
	char n = ( ascii - '0' );
	char a = ( ( ascii | 0x20 ) - 'a' );
	return( ( n | ( ( n | ( 9 - n ) ) >> 7 ) ) & ( ( a + 10 ) | ( ( a | ( 5 - a ) ) >> 7 ) ) );
}

//! convert a 4 bit hexadecimal number into it's ascii character representation
char HexToAscii( const char hex )
{
	char a = ( hex - 10 );
	return( a + 'A' - ( ( a >> 7 ) & 7 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    zero terminated text parsing helper functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParseBlack( const char* const string, unsigned int& read )
{
	unsigned int scan = 0;
	while( IsBlack( string[ scan ] ) ) ++scan;
	read = scan;
}

void ParseWhite( const char* const string, unsigned int& read )
{
	unsigned int scan = 0;
	while( IsWhite( string[ scan ] ) ) ++scan;
	read = scan;
}

bool ParseText( const char* const string, const char* const text, unsigned int& read )
{
	unsigned int white = 0;
	unsigned int black = static_cast< unsigned int >( strlen( text ) );
	ParseWhite( string, white );
	if( strncmp( &string[ white ], text, black ) == 0 )
	{
		read = ( white + black );
		return( true );
	}
	read = 0;
	return( false );
}

bool ParseWord( const char* const string, unsigned int& index, unsigned int& read )
{
	unsigned int scan = 0;
	while( IsWhite( string[ scan ] ) ) ++scan;
	if( IsBlack( string[ scan ] ) )
	{
		index = scan;
		for( ++scan; IsBlack( string[ scan ] ); ++scan );
		read = scan;
		return( true );
	}
	index = 0;
	read = 0;
	return( false );
}

bool ParseDecimal( const char* const string, double& value, unsigned int& read )
{	//	optional identifier: "&#"
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, scan );
	if( char digit = string[ scan ] )
	{
		bool zero = true;
		bool mneg = false;
		bool eneg = false;
		bool valid = false;
		double mantissa = 0;
		int exponent = 0;
		int decimals = 0;
		if( digit == '&' )
		{
			if( string[ ++scan ] != '#' ) return( false );
			digit = string[ ++scan ];
		}
		if( ( digit == '-' ) || ( digit == '+' ) )
		{	//	found sign
			if( digit == '-' ) mneg = true;
			digit = string[ ++scan ];
		}
		if( ( digit >= '0' ) && ( digit <= '9' ) )
		{
			valid = true;
			if( digit == '0' )
			{
				digit = string[ ++scan ];
			}
			else
			{
				zero = false;
				mantissa = static_cast< double >( digit - '0' );
				for( digit = string[ ++scan ]; ( digit >= '0' ) && ( digit <= '9' ); digit = string[ ++scan ] )
				{	//	parse integer digits
					mantissa *= 10;
					if( digit != '0' ) mantissa += static_cast< double >( digit - '0' );
				}
			}
		}
		if( digit == '.' )
		{	//	parse decimal places
			digit = string[ ++scan ];
			if( ( digit >= '0' ) && ( digit <= '9' ) )
			{
				valid = true;
				do
				{	//	parse fractional digits
					decimals--;
					mantissa *= 10;
					if( digit != '0' )
					{
						zero = false;
						mantissa += static_cast< double >( digit - '0' );
					}
					digit = string[ ++scan ];
				}
				while( ( digit >= '0' ) && ( digit <= '9' ) );
			}
		}
		if( valid )
		{
			if( ( digit == 'e' ) || ( digit == 'E' ) )
			{	//	parse exponent
				if( zero ) return( false );
				digit = string[ ++scan ];
				if( ( digit == '-' ) || ( digit == '+' ) )
				{	//	found sign
					if( digit == '-' ) eneg = true;
					digit = string[ ++scan ];
				}
				if( ( digit < '0' ) || ( digit > '9' ) ) return( false );
				do
				{	//	parse exponent digits
					exponent *= 10;
					exponent += ( digit - '0' );
					digit = string[ ++scan ];
				}
				while( ( digit >= '0' ) && ( digit <= '9' ) );
			}
			value = ( zero ? 0 : ( ( mneg ? -mantissa : mantissa ) * pow( 10.0, ( eneg ? ( decimals - exponent ) : ( decimals + exponent ) ) ) ) );
			read = scan;
			return( true );
		}
	}
	return( false );
}

bool ParseDecimal( const char* const string, long long& value, unsigned int& read )
{	//	optional identifier: "&#"
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, scan );
	if( char digit = string[ scan++ ] )
	{
		if( digit == '&' )
		{
			if( string[ scan++ ] != '#' ) return( false );
			digit = string[ scan++ ];
		}
		if( digit == '-' )
		{
			digit = string[ scan++ ];
			if( ( digit >= '0' ) && ( digit <= '9' ) )
			{
				if( digit == '0' )
				{
					value = 0;
				}
				else
				{
					unsigned long long ulo = ( digit - '0' );
					unsigned long long uhi = 0;
					for( digit = string[ scan ]; ( digit >= '0' ) && ( digit <= '9' ); digit = string[ ++scan ] )
					{
						ulo = ( ( ( ( ulo << 2 ) + ulo ) << 1 ) + digit - '0' );
						uhi = ( ( ( ( uhi << 2 ) + uhi ) << 1 ) + ( ulo >> 32 ) );
						if( uhi > 0x000000007fffffffll )
						{
							if( ulo || ( uhi > 0x0000000080000000ll ) ) return( false );
						}
						ulo &= 0x00000000ffffffffll;
					}
					value = ( 0 - ( uhi << 32 ) - ulo );
				}
				read = scan;
				return( true );
			}
		}
		else
		{
			if( digit == '+' ) digit = string[ scan++ ];
			if( ( digit >= '0' ) && ( digit <= '9' ) )
			{
				if( digit == '0' )
				{
					value = 0;
				}
				else
				{
					unsigned long long ulo = ( digit - '0' );
					unsigned long long uhi = 0;
					for( digit = string[ scan ]; ( digit >= '0' ) && ( digit <= '9' ); digit = string[ ++scan ] )
					{
						ulo = ( ( ( ( ulo << 2 ) + ulo ) << 1 ) + digit - '0' );
						uhi = ( ( ( ( uhi << 2 ) + uhi ) << 1 ) + ( ulo >> 32 ) );
						if( uhi > 0x000000007fffffffll ) return( false );
						ulo &= 0x00000000ffffffffll;
					}
					value = ( ( uhi << 32 ) + ulo );
				}
				read = scan;
				return( true );
			}
		}
	}
	return( false );
}

bool ParseDecimal( const char* const string, unsigned long long& value, unsigned int& read )
{	//	optional identifier: "&#"
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, scan );
	if( char digit = string[ scan++ ] )
	{
		if( digit == '&' )
		{
			if( string[ scan++ ] != '#' ) return( false );
			digit = string[ scan++ ];
		}
		if( ( digit >= '0' ) && ( digit <= '9' ) )
		{
			if( digit == '0' )
			{
				value = 0;
			}
			else
			{
				unsigned long long ulo = ( digit - '0' );
				unsigned long long uhi = 0;
				for( digit = string[ scan ]; ( digit >= '0' ) && ( digit <= '9' ); digit = string[ ++scan ] )
				{
					ulo = ( ( ( ( ulo << 2 ) + ulo ) << 1 ) + digit - '0' );
					uhi = ( ( ( ( uhi << 2 ) + uhi ) << 1 ) + ( ulo >> 32 ) );
					if( uhi > 0x00000000ffffffffll ) return( false );
					ulo &= 0x00000000ffffffffll;
				}
				value = ( ( uhi << 32 ) + ulo );
			}
			read = scan;
			return( true );
		}
	}
	return( false );
}

bool ParseOctal( const char* const string, unsigned long long& value, unsigned int& read )
{	//	leading character must be '0'
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, scan );
	if( string[ scan++ ] == '0' )
	{
		unsigned long long ull = 0;
		for( char digit = string[ scan ]; ( digit >= '0' ) && ( digit <= '7' ); digit = string[ ++scan ] )
		{
			if( ull > 0x1fffffffffffffffll ) return( false );
			ull = ( ( ull << 3 ) + digit - '0' );
		}
		value = ull;
		read = scan;
		return( true );
	}
	return( false );
}

bool ParseHex( const char* const string, unsigned long long& value, unsigned int& read )
{	//	optional identifiers are: '#', '$', "#$", 'x', 'X', "#x", "#X", "0x", "0X", "&#x", "&#X" or "\\u"
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, scan );
	if( char digit = string[ scan ] )
	{
		switch( digit )
		{
		case( '$' ):
		case( 'x' ):
		case( 'X' ):
			{
				digit = string[ ++scan ];
				break;
			}
		case( '\\' ):
			{
				if( string[ ++scan ] != 'u' ) return( false );
				digit = string[ ++scan ];
				break;
			}
		case( '#' ):
			{
				digit = string[ ++scan ];
				if( ( digit == '$' ) || ( digit == 'x' ) || ( digit == 'X' ) ) digit = string[ ++scan ];
				break;
			}
		case( '0' ):
			{
				char check = string[ scan + 1 ];
				if( ( check == 'x' ) || ( check == 'X' ) )
				{
					scan += 2;
					digit = string[ scan ];
				}
				break;
			}
		case( '&' ):
			{
				if( string[ ++scan ] != '#' ) return( false );
				digit = string[ ++scan ];
				if( ( digit != 'x' ) && ( digit != 'X' ) ) return( false );
				digit = string[ ++scan ];
				break;
			}
		default:
			{
				break;
			}
		}
		if( ( digit >= '0' ) && ( digit <= '9' ) )
		{
			digit -= '0';
		}
		else if( ( digit >= 'a' ) && ( digit <= 'f' ) )
		{
			digit -= ( 'a' - 10 );
		}
		else if( ( digit >= 'A' ) && ( digit <= 'F' ) )
		{
			digit -= ( 'A' - 10 );
		}
		else
		{
			return( false );
		}
		unsigned long long ull = digit;
		for(;;)
		{
			digit = string[ ++scan ];
			if( ( digit >= '0' ) && ( digit <= '9' ) )
			{
				digit -= '0';
			}
			else if( ( digit >= 'a' ) && ( digit <= 'f' ) )
			{
				digit -= ( 'a' - 10 );
			}
			else if( ( digit >= 'A' ) && ( digit <= 'F' ) )
			{
				digit -= ( 'A' - 10 );
			}
			else
			{
				value = ull;
				read = scan;
				return( true );
			}
			if( ull > 0x0fffffffffffffffll ) return( false );
			ull = ( ( ull << 4 ) + digit );
		}
	}
	return( false );
}

EOL_TYPE ParseLine( const char* const string, unsigned int& bytes, unsigned int& read )
{
	unsigned int scan = 0;
	while( string[ scan ] )
	{
		if( string[ scan ] == 0x0d )
		{	//	carriage-return
			bytes = scan;
			++scan;
			if( string[ scan ] == 0x0a )
			{	//	carriage-return, new-line pair
				++scan;
				read = scan;
				return( EOL_CRNL );
			}
			read = scan;
			return( EOL_CR );
		}
		if( string[ scan ] == 0x0a )
		{	//	new-line
			bytes = scan;
			++scan;
			if( string[ scan ] == 0x0d )
			{	//	new-line, carriage-return pair
				++scan;
				read = scan;
				return( EOL_NLCR );
			}
			read = scan;
			return( EOL_NL );
		}
		++scan;
	}
	bytes = scan;
	read = ( scan + 1 );
	return( EOL_NUL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    string length text parsing helper functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParseBlack( const char* const string, const unsigned int length, unsigned int& read )
{
	unsigned int scan = 0;
	while( scan < length )
	{
		if( !IsBlack( string[ scan ] ) ) break;
		++scan;
	}
	read = scan;
}

void ParseWhite( const char* const string, const unsigned int length, unsigned int& read )
{
	unsigned int scan = 0;
	while( scan < length )
	{
		if( !IsWhite( string[ scan ] ) ) break;
		++scan;
	}
	read = scan;
}

bool ParseText( const char* const string, const unsigned int length, const char* const text, unsigned int& read )
{
	unsigned int white = 0;
	unsigned int black = static_cast< unsigned int >( strlen( text ) );
	ParseWhite( string, length, white );
	if( ( length - white ) >= black )
	{
		if( strncmp( &string[ white ], text, black ) == 0 )
		{
			read = ( white + black );
			return( true );
		}
	}
	read = 0;
	return( false );
}

bool ParseWord( const char* const string, const unsigned int length, unsigned int& index, unsigned int& read )
{
	for( unsigned int scan = 0; ( scan < length ); ++scan )
	{
		if( !IsWhite( string[ scan ] ) )
		{
			if( IsBlack( string[ scan ] ) )
			{
				index = scan;
				for( ++scan; ( scan < length ); ++scan )
				{
					if( !IsBlack( string[ scan ] ) ) break;
				}
				read = scan;
				return( true );
			}
			break;
		}
	}
	index = 0;
	read = 0;
	return( false );
}

bool ParseDecimal( const char* const string, const unsigned int length, double& value, unsigned int& read )
{	//	optional identifier: "&#"
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, length, scan );
	if( scan < length )
	{
		bool zero = true;
		bool mneg = false;
		bool eneg = false;
		bool valid = false;
		double mantissa = 0;
		int exponent = 0;
		int decimals = 0;
		char digit = string[ scan++ ];
		if( digit == '&' )
		{
			if( ( scan >= length ) || ( string[ scan++ ] != '#' ) ) return( false );
			digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
		}
		if( ( digit == '-' ) || ( digit == '+' ) )
		{	//	found sign
			if( digit == '-' ) mneg = true;
			digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
		}
		if( ( digit >= '0' ) && ( digit <= '9' ) )
		{
			valid = true;
			if( digit == '0' )
			{
				digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
			}
			else
			{
				zero = false;
				mantissa = static_cast< double >( digit - '0' );
				digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
				while( ( digit >= '0' ) && ( digit <= '9' ) )
				{	//	parse integer digits
					mantissa *= 10;
					if( digit != '0' ) mantissa += static_cast< double >( digit - '0' );
					digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
				}
			}
		}
		if( digit == '.' )
		{	//	parse decimal places
			digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
			if( ( digit >= '0' ) && ( digit <= '9' ) )
			{
				valid = true;
				do
				{	//	parse fractional digits
					decimals--;
					mantissa *= 10;
					if( digit != '0' )
					{
						zero = false;
						mantissa += static_cast< double >( digit - '0' );
					}
					digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
				}
				while( ( digit >= '0' ) && ( digit <= '9' ) );
			}
		}
		if( valid )
		{
			if( ( digit == 'e' ) || ( digit == 'E' ) )
			{	//	parse exponent
				if( zero ) return( false );
				digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
				if( ( digit == '-' ) || ( digit == '+' ) )
				{	//	found sign
					if( digit == '-' ) eneg = true;
					digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
				}
				if( ( digit < '0' ) || ( digit > '9' ) ) return( false );
				do
				{	//	parse exponent digits
					exponent *= 10;
					exponent += ( digit - '0' );
					digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
				}
				while( ( digit >= '0' ) && ( digit <= '9' ) );
			}
			if( digit || ( string[ scan - 1 ] == 0 ) ) --scan;
			value = ( zero ? 0 : ( ( mneg ? -mantissa : mantissa ) * pow( 10.0, ( eneg ? ( decimals - exponent ) : ( decimals + exponent ) ) ) ) );
			read = scan;
			return( true );
		}
	}
	return( false );
}

bool ParseDecimal( const char* const string, const unsigned int length, long long& value, unsigned int& read )
{	//	optional identifier: "&#"
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, length, scan );
	if( scan < length )
	{
		char digit = string[ scan++ ];
		if( digit == '&' )
		{
			if( ( scan == length ) || ( string[ scan++ ] != '#' ) ) return( false );
			digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
		}
		if( digit == '-' )
		{
			digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
			if( ( digit >= '0' ) && ( digit <= '9' ) )
			{
				if( digit == '0' )
				{
					value = 0;
				}
				else
				{
					unsigned long long ulo = ( digit - '0' );
					unsigned long long uhi = 0;
					if( scan < length )
					{
						for( digit = string[ scan++ ]; ( digit >= '0' ) && ( digit <= '9' ); digit = string[ scan++ ] )
						{
							ulo = ( ( ( ( ulo << 2 ) + ulo ) << 1 ) + digit - '0' );
							uhi = ( ( ( ( uhi << 2 ) + uhi ) << 1 ) + ( ulo >> 32 ) );
							if( uhi > 0x000000007fffffffll )
							{
								if( ulo || ( uhi > 0x0000000080000000ll ) ) return( false );
							}
							ulo &= 0x00000000ffffffffll;
							if( scan >= length ) break;
						}
					}
					value = ( 0 - ( uhi << 32 ) - ulo );
				}
				read = scan;
				return( true );
			}
		}
		else
		{
			if( digit == '+' ) digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
			if( ( digit >= '0' ) && ( digit <= '9' ) )
			{
				if( digit == '0' )
				{
					value = 0;
				}
				else
				{
					unsigned long long ulo = ( digit - '0' );
					unsigned long long uhi = 0;
					if( scan < length )
					{
						for( digit = string[ scan++ ]; ( digit >= '0' ) && ( digit <= '9' ); digit = string[ scan++ ] )
						{
							ulo = ( ( ( ( ulo << 2 ) + ulo ) << 1 ) + digit - '0' );
							uhi = ( ( ( ( uhi << 2 ) + uhi ) << 1 ) + ( ulo >> 32 ) );
							if( uhi > 0x000000007fffffffll ) return( false );
							ulo &= 0x00000000ffffffffll;
							if( scan >= length ) break;
						}
					}
					value = ( ( uhi << 32 ) + ulo );
				}
				read = scan;
				return( true );
			}
		}
	}
	return( false );
}

bool ParseDecimal( const char* const string, const unsigned int length, unsigned long long& value, unsigned int& read )
{	//	optional identifier: "&#"
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, length, scan );
	if( scan < length )
	{
		char digit = string[ scan++ ];
		if( digit == '&' )
		{
			if( ( scan == length ) || ( string[ scan++ ] != '#' ) ) return( false );
			digit = ( ( scan < length ) ? string[ scan++ ] : 0 );
		}
		if( ( digit >= '0' ) && ( digit <= '9' ) )
		{
			if( digit == '0' )
			{
				value = 0;
			}
			else
			{
				unsigned long long ulo = ( digit - '0' );
				unsigned long long uhi = 0;
				if( scan < length )
				{
					for( digit = string[ scan++ ]; ( digit >= '0' ) && ( digit <= '9' ); digit = string[ scan++ ] )
					{
						ulo = ( ( ( ( ulo << 2 ) + ulo ) << 1 ) + digit - '0' );
						uhi = ( ( ( ( uhi << 2 ) + uhi ) << 1 ) + ( ulo >> 32 ) );
						if( uhi > 0x00000000ffffffffll ) return( false );
						ulo &= 0x00000000ffffffffll;
						if( scan >= length ) break;
					}
				}
				value = ( ( uhi << 32 ) + ulo );
			}
			read = scan;
			return( true );
		}
	}
	return( false );
}

bool ParseOctal( const char* const string, const unsigned int length, unsigned long long& value, unsigned int& read )
{	//	leading character must be '0'
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, length, scan );
	if( scan < length )
	{
		if( string[ scan++ ] == '0' )
		{
			unsigned long long ull = 0;
			if( scan < length )
			{
				for( char digit = string[ scan++ ]; ( digit >= '0' ) && ( digit <= '7' ); digit = string[ scan++ ] )
				{
					if( ull > 0x1fffffffffffffffll ) return( false );
					ull = ( ( ull << 3 ) + digit - '0' );
					if( scan >= length ) break;
				}
			}
			value = ull;
			read = scan;
			return( true );
		}
	}
	return( false );
}

bool ParseHex( const char* const string, const unsigned int length, unsigned long long& value, unsigned int& read )
{	//	optional identifiers are: '#', '$', "#$", 'x', 'X', "#x", "#X", "0x", "0X", "&#x", "&#X" or "\\u"
	value = 0;
	read = 0;
	unsigned int scan = 0;
	ParseWhite( string, length, scan );
	if( scan < length )
	{
		char digit = string[ scan++ ];
		switch( digit )
		{
		case( '$' ):
		case( 'x' ):
		case( 'X' ):
			{
				if( scan >= length ) return( false );
				digit = string[ scan++ ];
				break;
			}
		case( '\\' ):
			{
				if( ( length - scan ) < 2 ) return( false );
				if( string[ scan++ ] != 'u' ) return( false );
				digit = string[ scan++ ];
				break;
			}
		case( '#' ):
			{
				if( scan >= length ) return( false );
				digit = string[ scan++ ];
				if( ( digit == '$' ) || ( digit == 'x' ) || ( digit == 'X' ) )
				{
					if( scan >= length ) return( false );
					digit = string[ scan++ ];
				}
				break;
			}
		case( '0' ):
			{
				if( scan < length )
				{
					char check = string[ scan ];
					if( ( check == 'x' ) || ( check == 'X' ) )
					{
						++scan;
						if( scan >= length ) return( false );
						digit = string[ scan++ ];
					}
				}
				break;
			}
		case( '&' ):
			{
				if( ( length - scan ) < 3 ) return( false );
				if( string[ scan++ ] != '#' ) return( false );
				digit = string[ scan++ ];
				if( ( digit != 'x' ) && ( digit != 'X' ) ) return( false );
				digit = string[ scan++ ];
				break;
			}
		default:
			{
				break;
			}
		}
		if( ( digit >= '0' ) && ( digit <= '9' ) )
		{
			digit -= '0';
		}
		else if( ( digit >= 'a' ) && ( digit <= 'f' ) )
		{
			digit -= ( 'a' - 10 );
		}
		else if( ( digit >= 'A' ) && ( digit <= 'F' ) )
		{
			digit -= ( 'A' - 10 );
		}
		else
		{
			return( false );
		}
		unsigned long long ull = digit;
		while( scan < length )
		{
			digit = string[ scan++ ];
			if( ( digit >= '0' ) && ( digit <= '9' ) )
			{
				digit -= '0';
			}
			else if( ( digit >= 'a' ) && ( digit <= 'f' ) )
			{
				digit -= ( 'a' - 10 );
			}
			else if( ( digit >= 'A' ) && ( digit <= 'F' ) )
			{
				digit -= ( 'A' - 10 );
			}
			else
			{
				break;
			}
			if( ull > 0x0fffffffffffffffll ) return( false );
			ull = ( ( ull << 4 ) + digit );
		}
		value = ull;
		read = scan;
		return( true );
	}
	return( false );
}

EOL_TYPE ParseLine( const char* const string, const unsigned int length, unsigned int& bytes, unsigned int& read )
{
	unsigned int scan = 0;
	while( scan < length )
	{
		if( string[ scan ] == 0x00 )
		{	//	nul
			bytes = scan;
			read = ( scan + 1 );
			return( EOL_NUL );
		}
		if( string[ scan ] == 0x0d )
		{	//	carriage-return
			bytes = scan;
			++scan;
			if( scan < length )
			{
				if( string[ scan ] == 0x0a )
				{	//	carriage-return, new-line pair
					++scan;
					read = scan;
					return( EOL_CRNL );
				}
			}
			read = scan;
			return( EOL_CR );
		}
		if( string[ scan ] == 0x0a )
		{	//	new-line
			bytes = scan;
			++scan;
			if( scan < length )
			{
				if( string[ scan ] == 0x0d )
				{	//	new-line, carriage-return pair
					++scan;
					read = scan;
					return( EOL_NLCR );
				}
			}
			read = scan;
			return( EOL_NL );
		}
		++scan;
	}
	bytes = scan;
	read = scan;
	return( EOL_EOF );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end ascii namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace ascii

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
