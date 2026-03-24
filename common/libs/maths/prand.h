
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   prand.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Assorted pseudo-random number generator classes.
////
////    Comments:
////
////    	The majority of pseudo-random number generators implemented in this
////    	file are taken from George Marsaglia's paper "Xorshift RNGs".
////
////        Any of these should produce reasonable pseudo-random numbers, but for hard core randomisation
////        it is recommended that you apply the rotation permute functions to the results or intermediates.
////
////        Your use case will determine how you choose to generate random numbers, the most important
////        considerations should be your required periodicity and dimensionality.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard begin
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef	__MATHS_PRAND_INCLUDED__
#define	__MATHS_PRAND_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin maths namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace maths
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Random number to floating point conversion functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline float HelperFRand( const uint32_t rand )
{   //  will never return 0.0f, range is FltEpsilon : 1.0f
	uint32_t f = ( ( 0x3f800000 | ( rand & 0x007fffff ) ) + 1 );
	return( *reinterpret_cast< float* >( &f ) - 1.0f );
}

inline float HelperFRand2( const uint32_t rand )
{   //  will never return 0.0f, range is +/- ( FltEpsilon : 1.0f )
	uint32_t u = ( 0x3f800000 | ( ( rand & 0x00800000 ) << 8 ) );
    uint32_t f = ( ( u | ( rand & 0x007fffff ) ) + 1 );
	return( *reinterpret_cast< float* >( &f ) - *reinterpret_cast< float* >( &u ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Bit rotation functions - useful for PCG permute randomisation
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline uint8_t  RotateBitsLeft( const uint8_t value, const uint rotation ) { return( ( value << rotation ) | ( value >> ( 8 - rotation ) ) ); };
inline uint16_t RotateBitsLeft( const uint16_t value, const uint rotation ) { return( ( value << rotation ) | ( value >> ( 16 - rotation ) ) ); };
inline uint32_t RotateBitsLeft( const uint32_t value, const uint rotation ) { return( ( value << rotation ) | ( value >> ( 32 - rotation ) ) ); };
inline uint64_t RotateBitsLeft( const uint64_t value, const uint rotation ) { return( ( value << rotation ) | ( value >> ( 64 - rotation ) ) ); };
inline uint8_t  RotateBitsRight( const uint8_t value, const uint rotation ) { return( ( value >> rotation ) | ( value << ( 8 - rotation ) ) ); };
inline uint16_t RotateBitsRight( const uint16_t value, const uint rotation ) { return( ( value >> rotation ) | ( value << ( 16 - rotation ) ) ); };
inline uint32_t RotateBitsRight( const uint32_t value, const uint rotation ) { return( ( value >> rotation ) | ( value << ( 32 - rotation ) ) ); };
inline uint64_t RotateBitsRight( const uint64_t value, const uint rotation ) { return( ( value >> rotation ) | ( value << ( 64 - rotation ) ) ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Permuted rotation calculation functions - useful for PCG permute randomisation
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline uint PermutedRotationLo( const uint8_t value )
{
    uint rotation;
    uint16_t temp;
    rotation = ( static_cast< uint >( value >> 1 ) & 1 );
    temp = ( value & 0x0c );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 2 ) ) >> rotation ) & 3 );
    temp = ( value & 0xf0 );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 4 ) ) >> rotation ) & 7 );
    return( rotation );
}

inline uint PermutedRotationHi( const uint8_t value )
{
    uint rotation;
    uint16_t temp;
    rotation = static_cast< uint >( value >> 7 );
    temp = ( ( value >> 5 ) & 0x03 );
    rotation = ( static_cast< uint >( ( temp | ( temp << 2 ) ) >> rotation ) & 3 );
    temp = ( ( value >> 1 ) & 0x0f );
    rotation = ( static_cast< uint >( ( temp | ( temp << 4 ) ) >> rotation ) & 7 );
    return( rotation );
}

inline uint PermutedRotationLo( const uint16_t value )
{
    uint rotation;
    uint16_t temp;
    rotation = ( static_cast< uint >( value >> 1 ) & 1 );
    temp = ( value & 0x000c );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 2 ) ) >> rotation ) & 3 );
    temp = ( value & 0x00f0 );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 4 ) ) >> rotation ) & 7 );
    temp = ( value & 0xff00 );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 8 ) ) >> rotation ) & 15 );
    return( rotation );
}

inline uint PermutedRotationHi( const uint16_t value )
{
    uint rotation;
    uint16_t temp;
    rotation = static_cast< uint >( value >> 15 );
    temp = ( ( value >> 13 ) & 0x0003 );
    rotation = ( static_cast< uint >( ( temp | ( temp << 2 ) ) >> rotation ) & 3 );
    temp = ( ( value >> 9 ) & 0x000f );
    rotation = ( static_cast< uint >( ( temp | ( temp << 4 ) ) >> rotation ) & 7 );
    temp = ( ( value >> 1 ) & 0x00ff );
    rotation = ( static_cast< uint >( ( temp | ( temp << 8 ) ) >> rotation ) & 15 );
    return( rotation );
}

inline uint PermutedRotationLo( const uint32_t value )
{
    uint rotation;
    uint32_t temp;
    rotation = ( static_cast< uint >( value >> 1 ) & 1 );
    temp = ( value & 0x0000000c );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 2 ) ) >> rotation ) & 3 );
    temp = ( value & 0x000000f0 );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 4 ) ) >> rotation ) & 7 );
    temp = ( value & 0x0000ff00 );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 8 ) ) >> rotation ) & 15 );
    temp = ( value & 0xffff0000 );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 16 ) ) >> rotation ) & 31 );
    return( rotation );
}

inline uint PermutedRotationHi( const uint32_t value )
{
    uint rotation;
    uint32_t temp;
    rotation = static_cast< uint >( value >> 31 );
    temp = ( ( value >> 29 ) & 0x00000003 );
    rotation = ( static_cast< uint >( ( temp | ( temp << 2 ) ) >> rotation ) & 3 );
    temp = ( ( value >> 25 ) & 0x0000000f );
    rotation = ( static_cast< uint >( ( temp | ( temp << 4 ) ) >> rotation ) & 7 );
    temp = ( ( value >> 17 ) & 0x000000ff );
    rotation = ( static_cast< uint >( ( temp | ( temp << 8 ) ) >> rotation ) & 15 );
    temp = ( ( value >> 1 ) & 0x0000ffff );
    rotation = ( static_cast< uint >( ( temp | ( temp << 16 ) ) >> rotation ) & 31 );
    return( rotation );
}

inline uint PermutedRotationLo( const uint64_t value )
{
    uint rotation;
    uint64_t temp;
    rotation = ( static_cast< uint >( value >> 1 ) & 1 );
    temp = ( value & 0x000000000000000cULL );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 2 ) ) >> rotation ) & 3 );
    temp = ( value & 0x00000000000000f0ULL );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 4 ) ) >> rotation ) & 7 );
    temp = ( value & 0x000000000000ff00ULL );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 8 ) ) >> rotation ) & 15 );
    temp = ( value & 0x00000000ffff0000ULL );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 16 ) ) >> rotation ) & 31 );
    temp = ( value & 0xffffffff00000000ULL );
    rotation = ( static_cast< uint >( ( temp | ( temp >> 32 ) ) >> rotation ) & 63 );
    return( rotation );
}

inline uint PermutedRotationHi( const uint64_t value )
{
    uint rotation;
    uint64_t temp;
    rotation = static_cast< uint >( value >> 63 );
    temp = ( ( value >> 61 ) & 0x0000000000000003ULL );
    rotation = ( static_cast< uint >( ( temp | ( temp << 2 ) ) >> rotation ) & 3 );
    temp = ( ( value >> 57 ) & 0x000000000000000fULL );
    rotation = ( static_cast< uint >( ( temp | ( temp << 4 ) ) >> rotation ) & 7 );
    temp = ( ( value >> 49 ) & 0x00000000000000ffULL );
    rotation = ( static_cast< uint >( ( temp | ( temp << 8 ) ) >> rotation ) & 15 );
    temp = ( ( value >> 33 ) & 0x000000000000ffffULL );
    rotation = ( static_cast< uint >( ( temp | ( temp << 16 ) ) >> rotation ) & 31 );
    temp = ( ( value >> 1 ) & 0x00000000ffffffffULL );
    rotation = ( static_cast< uint >( ( temp | ( temp << 32 ) ) >> rotation ) & 63 );
    return( rotation );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    XprngV96:
////
////    	Xor based pseudo-random number (PRN) generator with optional Weyl sequence.
////
////    	Period:
////
////    		( 2^128 - 2^32 ) with Weyl sequence enabled ( default ).
////    		( 2^96 - 1 ) with Weyl sequence disabled ( SetWayl( 0, 0 ); ).
////
////    	This generator is suitable for the generation of vectors
////    	with up to 3 32-bit elements.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XprngV96
{
public:
	inline				XprngV96() { SetSeed(); SetWeyl(); };
	inline				~XprngV96() {};
	inline void			SetSeed( const uint32_t seed1 = 123456789u, const uint32_t seed2 = 362436069u, const uint32_t seed3 = 521288629u ) { m_Data[ 0 ] = seed1; m_Data[ 1 ] = seed2; m_Data[ 2 ] = seed3; };
	inline void			SetWeyl( const uint32_t bias = 6615241u, const uint32_t step = 362437u ) { m_Bias = bias; m_Step = ( step ? ( step | 1 ) : 0 ); };
	uint32_t			Rand( void );
	inline float		FRand( void ) { return( HelperFRand( Rand() >> 8 ) ); };
	inline float		FRand2( void ) { return( HelperFRand2( Rand() >> 8 ) ); };
protected:
	uint32_t			m_Bias;
	uint32_t			m_Step;
	uint32_t			m_Data[ 3 ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    XprngV128:
////
////    	Xor based pseudo-random number (PRN) generator with optional Weyl sequence.
////
////    	Period:
////
////    		( 2^160 - 2^32 ) with Weyl sequence enabled ( default ).
////    		( 2^128 - 1 ) with Weyl sequence disabled ( SetWayl( 0, 0 ); ).
////
////    	This generator is suitable for the generation of vectors
////    	with up to 4 32-bit elements.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XprngV128
{
public:
	inline				XprngV128() { SetSeed(); SetWeyl(); };
	inline				~XprngV128() {};
	inline void			SetSeed( const uint32_t seed1 = 123456789u, const uint32_t seed2 = 362436069u, const uint32_t seed3 = 521288629u, const uint32_t seed4 = 88675123u ) { m_Data[ 0 ] = seed1; m_Data[ 1 ] = seed2; m_Data[ 2 ] = seed3; m_Data[ 3 ] = seed4; };
	inline void			SetWeyl( const uint32_t bias = 6615241u, const uint32_t step = 362437u ) { m_Bias = bias; m_Step = ( step ? ( step | 1 ) : 0 ); };
	uint32_t			Rand( void );
	inline float		FRand( void ) { return( HelperFRand( Rand() >> 8 ) ); };
	inline float		FRand2( void ) { return( HelperFRand2( Rand() >> 8 ) ); };
protected:
	uint32_t			m_Bias;
	uint32_t			m_Step;
	uint32_t			m_Data[ 4 ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    XprngP32:
////
////    	Xor based pseudo-random number (PRN) generator, with multiple shift
////    	patterns and optional Weyl sequence.
////
////    	Period:
////
////    		( 2^64 - 2^32 ) with Weyl sequence enabled ( default ).
////    		( 2^32 - 1 ) with Weyl sequence disabled ( SetWayl( 0, 0 ); ).
////
////    	This generator is suitable for the generation of vectors
////    	with up to 2 32-bit elements.
////
////    	By default the shift pattern used will cycle with each instantiation
////    	of the class.
////
////    	The shift pattern can be manually cycled by calling SetForm() or a
////    	specific pattern can be selected by calling SetForm( [ 0: 2199 ] ).
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XprngP32
{
public:
	inline				XprngP32() { SetSeed(); SetWeyl(); SetForm(); };
	inline				~XprngP32() {};
	inline void			SetSeed( const uint32_t seed = 2463534242u ) { m_Bits = ( seed ? seed : 2463534242u ); };
	inline void			SetWeyl( const uint32_t bias = 6615241u, const uint32_t step = 362437u ) { m_Bias = bias; m_Step = ( step ? ( step | 1 ) : 0 ); };
	inline void			SetForm( void ) { SetForm( ms_Cycle ); ms_Cycle = ( ( ms_Cycle + 1 ) % ( 81 * 8 ) ); };
	void				SetForm( const uint32_t form );
	uint32_t			Rand( void );
	inline float		FRand( void ) { return( HelperFRand( Rand() >> 8 ) ); };
	inline float		FRand2( void ) { return( HelperFRand2( Rand() >> 8 ) ); };
protected:
	uint32_t			m_Bias;
	uint32_t			m_Step;
	uint32_t			m_Bits;
	char				m_RotA;
	char				m_RotB;
	char				m_RotC;
private:
	static int			ms_Cycle;
	static const char	mk_Shifts[ 81 * 3 ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    XprngP64:
////
////    	Xor based pseudo-random number (PRN) generator, with multiple shift
////    	patterns and optional Weyl sequence.
////
////    	Period:
////
////    		( 2^96 - 2^32 ) with Weyl sequence enabled ( default ).
////    		( 2^64 - 1 ) with Weyl sequence disabled ( SetWayl( 0, 0 ); ).
////
////    	This generator is suitable for the generation of vectors
////    	with up to 2 32-bit elements.
////
////    	By default the shift pattern used will cycle with each instantiation
////    	of the class.
////
////    	The shift pattern can be manually cycled by calling SetForm() or a
////    	specific pattern can be selected by calling SetForm( [ 0: 2199 ] ).
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XprngP64
{
public:
	inline				XprngP64() { SetSeed(); SetWeyl(); SetForm(); };
	inline				~XprngP64() {};
	inline void			SetSeed( const uint64_t seed = 88172645463325252ll ) { m_Bits = ( seed ? seed : 88172645463325252ll ); };
	inline void			SetWeyl( const uint32_t bias = 6615241u, const uint32_t step = 362437u ) { m_Bias = bias; m_Step = ( step ? ( step | 1 ) : 0 ); };
	inline void			SetForm( void ) { SetForm( ms_Cycle ); ms_Cycle = ( ( ms_Cycle + 1 ) % ( 275 * 8 ) ); };
	void				SetForm( const uint32_t form );
	uint32_t			Rand( void );
	inline float		FRand( void ) { return( HelperFRand( Rand() >> 8 ) ); };
	inline float		FRand2( void ) { return( HelperFRand2( Rand() >> 8 ) ); };
protected:
	uint32_t			m_Bias;
	uint32_t			m_Step;
	uint64_t			m_Bits;
	char				m_RotA;
	char				m_RotB;
	char				m_RotC;
private:
	static int			ms_Cycle;
	static const char	mk_Shifts[ 275 * 3 ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Xprng128:
////
////    	Xor based pseudo-random number (PRN) generator with optional Weyl sequence.
////
////    	Period:
////
////    		( 2^160 - 2^32 ) with Weyl sequence enabled ( default ).
////    		( 2^128 - 1 ) with Weyl sequence disabled ( SetWayl( 0, 0 ); ).
////
////    	This generator is suitable for the generation of vectors
////    	with up to 4 32-bit elements.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Xprng128
{
public:
	inline				Xprng128() { SetSeed(); SetWeyl( 0, 0 ); };
	inline				~Xprng128() {};
	inline void			SetSeed( const uint32_t seed1 = 123456789u, const uint32_t seed2 = 362436069u, const uint32_t seed3 = 521288629u, const uint32_t seed4 = 88675123u ) { m_Data[ 0 ] = seed1; m_Data[ 1 ] = seed2; m_Data[ 2 ] = seed3; m_Data[ 3 ] = seed4; };
	inline void			SetWeyl( const uint32_t bias = 6615241u, const uint32_t step = 362437u ) { m_Bias = bias; m_Step = ( step ? ( step | 1 ) : 0 ); };
	uint32_t			Rand( void );
	inline float		FRand( void ) { return( HelperFRand( Rand() >> 8 ) ); };
	inline float		FRand2( void ) { return( HelperFRand2( Rand() >> 8 ) ); };
protected:
	uint32_t			m_Bias;
	uint32_t			m_Step;
	uint32_t			m_Data[ 4 ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Xprng160:
////
////    	Xor based pseudo-random number (PRN) generator with optional Weyl sequence.
////
////    	Period:
////
////    		( 2^192 - 2^32 ) with Weyl sequence enabled ( default ).
////    		( 2^160 - 1 ) with Weyl sequence disabled ( SetWayl( 0, 0 ); ).
////
////    	This generator is suitable for the generation of vectors
////    	with up to 5 32-bit elements.
////
////    	For most purposes, this should be considered the standard PRN generator.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Xprng160
{
public:
	inline				Xprng160() { SetSeed(); SetWeyl(); };
	inline				~Xprng160() {};
	inline void			SetSeed( const uint32_t seed1 = 123456789u, const uint32_t seed2 = 362436069u, const uint32_t seed3 = 521288629u, const unsigned seed4 = 88675123u, const unsigned seed5 = 5783321u ) { m_Data[ 0 ] = seed1; m_Data[ 1 ] = seed2; m_Data[ 2 ] = seed3; m_Data[ 3 ] = seed4; m_Data[ 4 ] = seed5; };
	inline void			SetWeyl( const uint32_t bias = 6615241u, const uint32_t step = 362437u ) { m_Bias = bias; m_Step = ( step ? ( step | 1 ) : 0 ); };
	uint32_t			Rand( void );
	inline float		FRand( void ) { return( HelperFRand( Rand() >> 8 ) ); };
	inline float		FRand2( void ) { return( HelperFRand2( Rand() >> 8 ) ); };
protected:
	uint32_t			m_Bias;
	uint32_t			m_Step;
	uint32_t			m_Data[ 5 ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    LCprng:
////
////    	Linear congruent pseudo-random number generator.
////
////    	Period: ( 2^32 )
////
////    	This generator emulates several common Rand() function implementations.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum LC_PRNG_FORM
{
	LC_PRNG_DEFAULT = 0,
	LC_PRNG_NUMERICAL_RECIPES = LC_PRNG_DEFAULT,
	LC_PRNG_ANSI,
	LC_PRNG_MS,
	LC_PRNG_BORLAND,
	LC_PRNG_DELPHI,
	LC_PRNG_FORCE32 = 0x7fffffff
};

class LCprng
{
public:
	inline				LCprng() { Init(); };
	inline				LCprng( const uint32_t seed ) { Init( seed ); };
	inline				~LCprng() {};
	void				SetForm( const LC_PRNG_FORM form );
	inline void			Reset( const uint32_t seed = 123456789u ) { m_Data[ 0 ] = seed; };
	inline uint16_t		Rand( void ) { return( ( ( m_Data[ 0 ] = ( ( m_Data[ 0 ] *  m_Data[ 1 ] ) + m_Data[ 2 ] ) ) >> 16 ) & 0x00007fff ); };
	inline uint32_t		LRand( void ) { return( ( m_Data[ 0 ] = ( ( m_Data[ 0 ] *  m_Data[ 1 ] ) + m_Data[ 2 ] ) ) & 0x7ffffff ); };
	inline float		FRand( void ) { return( HelperFRand( LRand() >> 7 ) ); };
	inline float		FRand2( void ) { return( HelperFRand2( LRand() >> 7 ) ); };
protected:
	inline void			Init( const uint32_t seed = 123456789u ) { m_Data[ 0 ] = seed; m_Data[ 1 ] = 1664525u; m_Data[ 2 ] = 1013904223u; };
	uint32_t			m_Data[ 3 ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    PLCprng:
////
////    	Permuted linear congruent pseudo-random number generator.
////
////    	Period: ( 2^64 )
////
////    	This is a good general purpose generator
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PLCprng
{
public:
	inline				PLCprng() { Init(); };
	inline				PLCprng( const uint64_t seed, const uint64_t step = 1 ) { Init( seed, step ); };
	inline				~PLCprng() {};
	inline void		    SetSeed( const uint64_t seed ) { m_Data = seed; };
	inline void		    SetStep( const uint64_t step ) { m_Step = ( step | 1 ); };
	inline uint32_t		Rand( void ) { uint rotation = PermutedRotationLo( m_Data ); Update(); return( RotateBitsRight( static_cast< uint32_t >( ( ( m_Data >> 18 ) ^ m_Data ) >> 27 ), rotation ) ); };
	inline float		FRand( void ) { return( HelperFRand( Rand() >> 8 ) ); };
	inline float		FRand2( void ) { return( HelperFRand2( Rand() >> 8 ) ); };
protected:
	inline void			Init( const uint64_t seed = 123456789u, const uint64_t step = 17u ) { m_Data = seed; m_Step = ( step | 1 ); };
    inline void         Update() { m_Data = ( ( m_Data * 6364136223846793005ULL ) + m_Step ); };
    uint64_t            m_Data;
    uint64_t            m_Step;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end maths namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace maths

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__MATHS_PRAND_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

