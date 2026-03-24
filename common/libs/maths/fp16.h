
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////	File:	fp16.h
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
////	include guard begin
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef	__MATHS_FP16_INCLUDED__
#define	__MATHS_FP16_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////	fp16 half precision (16-bit) floating point class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class fp16
{
public:
	inline				fp16() : m_bits( 0 ) {};
	inline				fp16( const short bits, const bool endianSwap ) { Init( bits, endianSwap ); };
	inline				fp16( const unsigned short bits, const bool endianSwap ) { Init( bits, endianSwap ); };
	inline explicit		fp16( const fp16& h ) { Set( h ); };
	inline explicit		fp16( const double d ) { Set( d ); };
	inline explicit		fp16( const float f ) { Set( f ); };
	inline explicit		fp16( const int i ) { Set( i ); };
	inline explicit		fp16( const unsigned int ui ) { Set( ui ); };
	inline explicit		fp16( const long l ) { Set( l ); };
	inline explicit		fp16( const unsigned long ul ) { Set( ul ); };
	inline explicit		fp16( const short s ) { Set( s ); };
	inline explicit		fp16( const unsigned short us ) { Set( us ); };
	inline explicit		fp16( const char c ) { Set( c ); };
	inline explicit		fp16( const unsigned char uc ) { Set( uc ); };
	inline				~fp16() {};
	inline void			Init( const short bits, const bool endianSwap ) { Init( static_cast< unsigned short >( bits ), endianSwap ); };
	inline void			Init( const unsigned short bits, const bool endianSwap ) { m_bits = ( endianSwap ? ( ( bits << 8 ) | ( bits >> 8 ) ) : bits ); };
	inline void			EndianSwap( void ) { m_bits = ( ( m_bits << 8 ) | ( m_bits >> 8 ) ); };
	inline bool			Sign( void ) const { return( ( m_bits & 0x8000 ) ? true : false ); };
	inline bool			IsZero( void ) const { return( ( ( m_bits & 0x7fff ) == 0x0000 ) ? true : false ); };
	inline bool			IsNAN( void ) const { return( ( ( m_bits & 0x7fff ) > 0x7c00 ) ? true : false ); };
	inline bool			IsINF( void ) const { return( ( ( m_bits & 0x7fff ) == 0x7c00 ) ? true : false ); };
	inline bool			IsReal( void ) const { return( ( ( m_bits & 0x7fff ) < 0x7c00 ) ? true : false ); };
	inline bool			IsPositive( void ) const { return( ( m_bits & 0x8000 ) ? false : true ); };
	inline bool			IsNegative( void ) const { return( ( m_bits & 0x8000 ) ? true : false ); };
	inline void			Set( const fp16& h ) { m_bits = h.m_bits; };
	inline void			Set( const double d ) { Set( static_cast< float >( d ) ); };
	void				Set( float f );
	void				Set( int i );
	void				Set( unsigned int ui );
	inline void			Set( const long l ) { Set( static_cast< int >( l ) ); };
	inline void			Set( const unsigned long ul ) { Set( static_cast< unsigned int >( ul ) ); };
	inline void			Set( const short s ) { Set( static_cast< int >( s ) ); };
	inline void			Set( const unsigned short us ) { Set( static_cast< unsigned int >( us ) ); };
	inline void			Set( const char c ) { Set( static_cast< int >( c ) ); };
	inline void			Set( const unsigned char uc ) { Set( static_cast< unsigned int >( uc ) ); };
	inline fp16&		operator=( const fp16& h ) { Set( h ); return( *this ); };
	inline fp16&		operator=( const double d ) { Set( d ); return( *this ); };
	inline fp16&		operator=( const float f ) { Set( f ); return( *this ); };
	inline fp16&		operator=( const int i ) { Set( i ); return( *this ); };
	inline fp16&		operator=( const unsigned int ui ) { Set( ui ); return( *this ); };
	inline fp16&		operator=( const long l ) { Set( l ); return( *this ); };
	inline fp16&		operator=( const unsigned long ul ) { Set( ul ); return( *this ); };
	inline fp16&		operator=( const short s ) { Set( s ); return( *this ); };
	inline fp16&		operator=( const unsigned short us ) { Set( us ); return( *this ); };
	inline fp16&		operator=( const char c ) { Set( c ); return( *this ); };
	inline fp16&		operator=( const unsigned char uc ) { Set( uc ); return( *this ); };
	inline bool			operator==( const fp16& h ) const { return( ( m_bits == h.m_bits ) ? true : false ); };
	inline bool			operator!=( const fp16& h ) const { return( ( m_bits == h.m_bits ) ? false : true ); };
	inline bool			operator<( const fp16& h ) const { return( ( static_cast< short >( m_bits ) < static_cast< short >( h.m_bits ) ) ? false : true ); };
	inline bool			operator<=( const fp16& h ) const { return( ( static_cast< short >( m_bits ) <= static_cast< short >( h.m_bits ) ) ? false : true ); };
	inline bool			operator>( const fp16& h ) const { return( ( static_cast< short >( m_bits ) > static_cast< short >( h.m_bits ) ) ? false : true ); };
	inline bool			operator>=( const fp16& h ) const { return( ( static_cast< short >( m_bits ) >= static_cast< short >( h.m_bits ) ) ? false : true ); };
	inline 				operator double( void ) const { float f = *this; return( static_cast< double >( f ) ); };
	 					operator float( void ) const;
						operator int( void ) const;
						operator unsigned int( void ) const;
	inline 				operator long( void ) const { int i = *this; return( static_cast< long >( i ) ); };
	inline 				operator unsigned long( void ) const { unsigned int ui = *this; return( static_cast< unsigned long >( ui ) ); };
	inline				operator short( void ) const { int i = *this; return( static_cast< short >( ( i > 32767 ) ? 32767 : ( ( i < -32768 ) ? -32768 : i ) ) ); };
	inline				operator unsigned short( void ) const { unsigned int ui = *this; return( static_cast< unsigned short >( ( ui > 65535 ) ? 65535 : ui ) ); };
	inline				operator char( void ) const { int i = *this; return( static_cast< char >( ( i > 127 ) ? 127 : ( ( i < -128 ) ? -128 : i ) ) ); };
	inline				operator unsigned char( void ) const { unsigned int ui = *this; return( static_cast< unsigned char >( ( ui > 255 ) ? 255 : ui ) ); };
private:
	unsigned short		m_bits;
};

//	storage size specific type:

typedef fp16            float16_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////	include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__MATHS_FP16_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////	End of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
