
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////	File:	fp16.cpp
////	Author:	Ritchie Brannan
////	Date:	11 Nov 10
////
////	Description:
////
////		Half precision (16-bit) floating point support class.
////
////	Notes:
////
////		Where conversions require bit truncation, values are rounded to the nearest bit.
////		The rounding used for bit truncation is : floor( signed value + one half bit )
////
////		For example:
////
////			fp16(  0.50 ) => int( 1 )
////			fp16(  0.49 ) => int( 0 )
////			fp16( -0.50 ) => int( 0 )
////			fp16( -0.51 ) => int( -1 )
////			int(  65520 ) => fp16(  INF )
////			int(  65519 ) => fp16(  65504.0 )
////			int( -65520 ) => fp16( -65504.0 )
////			int( -65521 ) => fp16( -INF )
////			float(  65520.000 ) => fp16(  INF )
////			float(  65519.996 ) => fp16(  65504.0 )
////			float( -65520.000 ) => fp16( -65504.0 )
////			float( -65520.004 ) => fp16( -INF )
////
////			fp16() => float() : no rounding
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////	includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "fp16.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////	fp16 half precision (16-bit) floating point class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fp16::Set( float f )
{
	unsigned int ui = *reinterpret_cast< unsigned int* >( &f );
	unsigned int sign = ( ui & 0x80000000 );
	ui &= 0x7fffffff;
	if( sign )
	{	//	negative
		if( ui > 0x477ff000 )
		{	//	INF or other special
			if( ui > 0x7f7fffff )
			{	//	special
				m_bits = ( static_cast< unsigned short >( ui >> 13 ) | 0xfc00 );
			}
			else
			{	//	INF
				m_bits = 0xfc00;
			}
		}
		else if( ui > 0x33000000 )
		{	//	normalised or denormalised
			if( ui > 0x387fe000 )	//	normalised limit == 0x387ff000
			{	//	normalised ( some overlap into denormalised )
				m_bits = ( static_cast< unsigned short >( ( ui + 0xc8000fff ) >> 13 ) | 0x8000 );
			}
			else
			{	//	denormalised
				unsigned int exp = ( ( ui & 0x7f800000 ) >> 23 );
				m_bits = ( static_cast< unsigned short >( ( ( ui & 0x007fffff ) + ( 1 << ( 125 - exp ) ) + 0x007fffff ) >> ( 126 - exp ) ) | 0x8000 );
			}
		}
		else
		{	//	zero
			m_bits = 0x8000;
		}
	}
	else
	{	//	positive
		if( ui > 0x477fefff )
		{	//	INF or other special
			if( ui > 0x7f7fffff )
			{	//	special
				m_bits = ( ( static_cast< unsigned short >( ui >> 13 ) | 0x7c00 ) & 0x7fff );
			}
			else
			{	//	INF
				m_bits = 0x7c00;
			}
		}
		else if( ui > 0x32ffffff )
		{	//	normalised or denormalised
			if( ui > 0x387fdfff )	//	normalised limit == 0x387fefff
			{	//	normalised ( some overlap into denormalised )
				m_bits = ( static_cast< unsigned short >( ( ui + 0xc8001000 ) >> 13 ) & 0x7fff );
			}
			else
			{	//	denormalised
				unsigned int exp = ( ( ui & 0x7f800000 ) >> 23 );
				m_bits = static_cast< unsigned short >( ( ( ui & 0x007fffff ) + ( 1 << ( 125 - exp ) ) + 0x00800000 ) >> ( 126 - exp ) );
			}
		}
		else
		{	//	zero
			m_bits = 0x0000;
		}
	}
}

void fp16::Set( int i )
{
	if( i == 0 )
	{	//	zero
		m_bits = 0x0000;
	}
	else if( i < 0 )
	{	//	negative
		if( i < -65520 )
		{	//	INF
			m_bits = 0xfc00;
		}
		else
		{
			i = -i;
			unsigned int exp = 15;
			if( i & 0x0000ff00 )
			{
				exp += 8;
			}
			else
			{
				i <<= 8;
			}
			if( i & 0x0000f000 )
			{
				exp += 4;
			}
			else
			{
				i <<= 4;
			}
			if( i & 0x0000c000 )
			{
				exp += 2;
			}
			else
			{
				i <<= 2;
			}
			if( i & 0x00008000 )
			{
				exp += 1;
			}
			else
			{
				i <<= 1;
			}
			i += 0x0000000f;
			if( i > 0x0000ffff )
			{
				exp++;
				i >>= 1;
			}
			m_bits = static_cast< unsigned short >( 0x00008000 + ( exp << 10 ) + ( ( i >> 5 ) & 0x000003ff ) );
		}
	}
	else
	{	//	positive
		if( i > 65519 )
		{	//	INF
			m_bits = 0x7c00;
		}
		else
		{
			unsigned int exp = 15;
			if( i & 0x0000ff00 )
			{
				exp += 8;
			}
			else
			{
				i <<= 8;
			}
			if( i & 0x0000f000 )
			{
				exp += 4;
			}
			else
			{
				i <<= 4;
			}
			if( i & 0x0000c000 )
			{
				exp += 2;
			}
			else
			{
				i <<= 2;
			}
			if( i & 0x00008000 )
			{
				exp += 1;
			}
			else
			{
				i <<= 1;
			}
			i += 0x00000010;
			if( i > 0x0000ffff )
			{
				exp++;
				i >>= 1;
			}
			m_bits = static_cast< unsigned short >( ( exp << 10 ) + ( ( i >> 5 ) & 0x000003ff ) );
		}
	}
}

void fp16::Set( unsigned int ui )
{
	if( ui == 0 )
	{	//	zero
		m_bits = 0x0000;
	}
	else
	{	//	non-zero
		if( ui > 65519 )
		{	//	INF
			m_bits = 0x7bff;
		}
		else
		{
			unsigned int exp = 15;
			if( ui & 0x0000ff00 )
			{
				exp += 8;
			}
			else
			{
				ui <<= 8;
			}
			if( ui & 0x0000f000 )
			{
				exp += 4;
			}
			else
			{
				ui <<= 4;
			}
			if( ui & 0x0000c000 )
			{
				exp += 2;
			}
			else
			{
				ui <<= 2;
			}
			if( ui & 0x00008000 )
			{
				exp += 1;
			}
			else
			{
				ui <<= 1;
			}
			ui += 0x00000010;
			if( ui > 0x0000ffff )
			{
				exp++;
				ui >>= 1;
			}
			m_bits = static_cast< unsigned short >( ( exp << 10 ) + ( ( ui >> 5 ) & 0x000003ff ) );
		}
	}
}

fp16::operator float( void ) const
{
	unsigned int uif = ( static_cast< unsigned int >( m_bits & 0x8000 ) << 16 );
	unsigned int exp = ( m_bits & 0x7c00 );
	if( exp )
	{	//	normalised or special
		if( exp == 0x00007c00 )
		{	//	special
			uif += ( 0x7f800000 + ( static_cast< unsigned int >( m_bits & 0x03ff ) << 13 ) );
		}
		else
		{	//	normalised
			uif += ( 0x38000000 + ( static_cast< unsigned int >( m_bits & 0x7fff ) << 13 ) );
		}
	}
	else
	{	//	denormalised or zero
		unsigned int ui = ( m_bits & 0x03ff );
		if( ui )
		{	//	fp16 is denormalised and not +/- 0
			exp = 103;
			if( ui & 0x0000ff00 )
			{
				exp += 8;
			}
			else
			{
				ui <<= 8;
			}
			if( ui & 0x0000f000 )
			{
				exp += 4;
			}
			else
			{
				ui <<= 4;
			}
			if( ui & 0x0000c000 )
			{
				exp += 2;
			}
			else
			{
				ui <<= 2;
			}
			if( ui & 0x00008000 )
			{
				exp += 1;
			}
			else
			{
				ui <<= 1;
			}
			uif += ( ( exp << 23 ) + ( ( ui << 8 ) & 0x007fffff ) );
		}
	}
	return( *reinterpret_cast< float* >( &uif ) );
}

fp16::operator int( void ) const
{
	if( m_bits & 0x8000 )
	{
		if( ( m_bits <= 0xb800 ) || ( m_bits > 0xfc00 ) )
		{	//	>= -0.5f or NAN
			return( 0 );
		}
		else if( m_bits < 0xfc00 )
		{	//	finite
			return( -( ( ( static_cast< int >( ( m_bits & 0x03ff ) | 0x0400 ) << ( ( ( m_bits & 0x7c00 ) >> 10 ) - 14 ) ) + 0x000003ff ) >> 11 ) );
		}
		else
		{	//	INF
			return( 0x80000000 );
		}
	}
	else
	{
		if( ( m_bits < 0x3800 ) || ( m_bits > 0x7c00 ) )
		{	//	< 0.5f or NAN
			return( 0 );
		}
		else if( m_bits < 0x7c00 )
		{	//	finite
			return( ( ( static_cast< int >( ( m_bits & 0x03ff ) | 0x0400 ) << ( ( ( m_bits & 0x7c00 ) >> 10 ) - 14 ) ) + 0x00000400 ) >> 11 );
		}
		else
		{	//	INF
			return( 0x7fffffff );
		}
	}
}

fp16::operator unsigned int( void ) const
{
	if( ( m_bits < 0x3800 ) || ( m_bits > 0x7c00 ) )
	{	//	< 0.5f or NAN
		return( 0 );
	}
	else if( m_bits < 0x7c00 )
	{	//	finite
		return( ( ( static_cast< unsigned int >( ( m_bits & 0x03ff ) | 0x0400 ) << ( ( ( m_bits & 0x7c00 ) >> 10 ) - 14 ) ) + 0x00000400 ) >> 11 );
	}
	else
	{	//	INF
		return( 0xffffffff );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////	end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
