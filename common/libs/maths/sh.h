
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   sh.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Spherical harmonics container and functions.
////
////    Notes:
////
////    	Radiance is illumination relative to angular spread about the surface normal (specular and reflection).
////    	Irradiance is illumination per of unit of surface area (diffuse).
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

#ifndef	__MATHS_SH_INCLUDED__
#define	__MATHS_SH_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "vec3.h"
#include "vec4.h"

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
////    forward declarations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sh2;
struct sh2_rgb;
struct sh2_rgb_packed;
struct sh3;
struct sh3_rgb;
struct sh3_rgb_packed;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    consts
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace consts
{
	namespace SH
	{
		extern const vec3		Radiance;
		extern const vec3		Irradiance;
		extern const vec3		RadianceToIrradiance;
		extern const vec3		IrradianceToRadiance;
		extern const vec3		RootRadiance;
		extern const vec3		RootIrradiance;
		extern const vec3		RootRadianceAsIrradiance;
		extern const vec3		RootIrradianceAsRadiance;
		extern const sh3_rgb	BREEZEWAY;
		extern const sh3_rgb	GALILEOS_TOMB;
		extern const sh3_rgb	CAMPUS_SUNSET;
		extern const sh3_rgb	UFFUZI_GALLERY;
		extern const sh3_rgb	OLD_TOWN_SQUARE;
		extern const sh3_rgb	GRACE_CATHEDRAL;
		extern const sh3_rgb	VINE_ST_KITCHEN;
		extern const sh3_rgb	EUCALYPTUS_GROVE;
		extern const sh3_rgb	ST_PETERS_BASILICA;
		extern const sh3_rgb	FUNSTON_BEACH_SUNSET;
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh2
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sh2
{
	float					c[ 4 ];
	bool					IsNan( void ) const;
	bool					IsReal( void ) const;
	void					Madd( const sh2& src, const float mul );
	void					Add( const sh2& add );
	void					Sub( const sh2& sub );
	void					Mul( const sh2& mul );
	void					Mul( const float mul );
	void					Div( const float div );
	void					Lerp( const sh2& trg, const float t );
	void					Negate( void );
	void					MulConsts( const vec3& cv );
	void					SetMadd( const sh2& src, const float mul, const sh2& add );
	void					SetAdd( const sh2& src, const sh2& add );
	void					SetSub( const sh2& src, const sh2& sub );
	void					SetMul( const sh2& src, const sh2& mul );
	void					SetMul( const sh2& src, const float mul );
	void					SetDiv( const sh2& src, const float div );
	void					SetLerp( const sh2& src, const sh2& trg, const float t );
	void					SetNegate( const sh2& src );
	void					SetConsts( const vec3& cv );
	void					SetBasis( const vec3& v );
	void					Set( const sh3& src );
	void					Set( const sh2& src );
	void					Get( sh2& trg ) const;
	float					GetCoefficient( const int index ) const;
	float					GetOrder1( void ) const;
	float					GetOrder2( void ) const;
	inline sh2&				operator=( const sh3& h ) { Set( h ); return( *this ); };
	inline sh2&				operator=( const sh2& h ) { Set( h ); return( *this ); };
	inline sh2&				operator+=( const sh2& h ) { Add( h ); return( *this ); };
	inline sh2&				operator-=( const sh2& h ) { Sub( h ); return( *this ); };
	inline sh2&				operator*=( const sh2& h ) { Mul( h ); return( *this ); };
	inline sh2&				operator*=( const float f ) { Mul( f ); return( *this ); };
	inline sh2&				operator/=( const float f ) { Div( f ); return( *this ); };
};

inline sh2 operator+( const sh2& src, const sh2& add ) { sh2 h; h.SetAdd( src, add ); return( h ); };
inline sh2 operator-( const sh2& src, const sh2& sub ) { sh2 h; h.SetSub( src, sub ); return( h ); };
inline sh2 operator*( const sh2& src, const sh2& mul ) { sh2 h; h.SetMul( src, mul ); return( h ); };
inline sh2 operator*( const sh2& src, const float mul ) { sh2 h; h.SetMul( src, mul ); return( h ); };
inline sh2 operator/( const sh2& src, const float div ) { sh2 h; h.SetDiv( src, div ); return( h ); };
inline sh2 operator-( const sh2& src ) { sh2 h; h.SetNegate( src ); return( h ); };

inline sh2 lerp( const sh2& a, const sh2& b, const float t ) { sh2 h; h.SetLerp( a, b, t ); return( h ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh2_rgb
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sh2_rgb
{
	sh2						r, g, b;
	bool					IsNan( void ) const;
	bool					IsReal( void ) const;
	void					Madd( const sh2_rgb& src, const float mul );
	void					Add( const sh2_rgb& add );
	void					Sub( const sh2_rgb& sub );
	void					Mul( const sh2_rgb& mul );
	void					Mul( const sh2& mul );
	void					Mul( const float mul );
	void					Div( const float div );
	void					Lerp( const sh2_rgb& trg, const float t );
	void					Negate( void );
	void					MulConsts( const vec3& cv );
	void					SetMadd( const sh2_rgb& src, const float mul, const sh2_rgb& add );
	void					SetAdd( const sh2_rgb& src, const sh2_rgb& add );
	void					SetSub( const sh2_rgb& src, const sh2_rgb& sub );
	void					SetMul( const sh2_rgb& src, const sh2_rgb& mul );
	void					SetMul( const sh2_rgb& src, const sh2& mul );
	void					SetMul( const sh2_rgb& src, const float mul );
	void					SetDiv( const sh2_rgb& src, const float div );
	void					SetLerp( const sh2_rgb& src, const sh2_rgb& trg, const float t );
	void					SetNegate( const sh2_rgb& src );
	void					SetConsts( const vec3& cv );
	void					SetBasis( const vec3& v );
	void					Set( const sh3& src );
	void					Set( const sh2& src );
	void					Set( const sh3_rgb& src );
	void					Set( const sh2_rgb& src );
	void					Get( sh2_rgb& trg ) const;
	inline sh2_rgb&			operator=( const sh3& h ) { Set( h ); return( *this ); };
	inline sh2_rgb&			operator=( const sh2& h ) { Set( h ); return( *this ); };
	inline sh2_rgb&			operator=( const sh3_rgb& h ) { Set( h ); return( *this ); };
	inline sh2_rgb&			operator=( const sh2_rgb& h ) { Set( h ); return( *this ); };
	inline sh2_rgb&			operator+=( const sh2_rgb& h ) { Add( h ); return( *this ); };
	inline sh2_rgb&			operator-=( const sh2_rgb& h ) { Sub( h ); return( *this ); };
	inline sh2_rgb&			operator*=( const sh2_rgb& h ) { Mul( h ); return( *this ); };
	inline sh2_rgb&			operator*=( const sh2& h ) { Mul( h ); return( *this ); };
	inline sh2_rgb&			operator*=( const float f ) { Mul( f ); return( *this ); };
	inline sh2_rgb&			operator/=( const float f ) { Div( f ); return( *this ); };
};

inline sh2_rgb operator+( const sh2_rgb& src, const sh2_rgb& add ) { sh2_rgb h; h.SetAdd( src, add ); return( h ); };
inline sh2_rgb operator-( const sh2_rgb& src, const sh2_rgb& sub ) { sh2_rgb h; h.SetSub( src, sub ); return( h ); };
inline sh2_rgb operator*( const sh2_rgb& src, const sh2_rgb& mul ) { sh2_rgb h; h.SetMul( src, mul ); return( h ); };
inline sh2_rgb operator*( const sh2_rgb& src, const sh2& mul ) { sh2_rgb h; h.SetMul( src, mul ); return( h ); };
inline sh2_rgb operator*( const sh2_rgb& src, const float mul ) { sh2_rgb h; h.SetMul( src, mul ); return( h ); };
inline sh2_rgb operator/( const sh2_rgb& src, const float div ) { sh2_rgb h; h.SetDiv( src, div ); return( h ); };
inline sh2_rgb operator-( const sh2_rgb& src ) { sh2_rgb h; h.SetNegate( src ); return( h ); };

inline sh2_rgb lerp( const sh2_rgb& a, const sh2_rgb& b, const float t ) { sh2_rgb h; h.SetLerp( a, b, t ); return( h ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh2_rgb_packed
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sh2_rgb_packed
{
	vec4					r1;
	vec4					g1;
	vec4					b1;
	void					Set( const sh2_rgb& src );
	void					Eval( const vec3& v, vec3& colour ) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh3
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sh3
{
	float					c[ 9 ];
	bool					IsNan( void ) const;
	bool					IsReal( void ) const;
	void					Madd( const sh3& src, const float mul );
	void					Add( const sh3& add );
	void					Sub( const sh3& sub );
	void					Mul( const sh3& mul );
	void					Mul( const float mul );
	void					Div( const float div );
	void					Lerp( const sh3& trg, const float t );
	void					Negate( void );
	void					MulConsts( const vec3& cv );
	void					MulNormalize( void );
	void					MulSqrNormalize( void );
	void					SetMadd( const sh3& src, const float mul, const sh3& add );
	void					SetAdd( const sh3& src, const sh3& add );
	void					SetSub( const sh3& src, const sh3& sub );
	void					SetMul( const sh3& src, const sh3& mul );
	void					SetMul( const sh3& src, const float mul );
	void					SetDiv( const sh3& src, const float div );
	void					SetLerp( const sh3& src, const sh3& trg, const float t );
	void					SetNegate( const sh3& src );
	void					SetConsts( const vec3& cv );
	void					SetNormalize( void );
	void					SetSqrNormalize( void );
	void					SetNormalizedBasis( const vec3& v );
	void					SetUnitBasis( const vec3& v );
	void					SetBasis( const vec3& v );
	void					Set( const sh3& src );
	void					Get( sh3& trg ) const;
	float					GetCoefficient( const int index ) const;
	float					GetOrder1( void ) const;
	float					GetOrder2( void ) const;
	float					GetOrder3( void ) const;
	inline sh3&				operator=( const sh3& h ) { Set( h ); return( *this ); };
	inline sh3&				operator+=( const sh3& h ) { Add( h ); return( *this ); };
	inline sh3&				operator-=( const sh3& h ) { Sub( h ); return( *this ); };
	inline sh3&				operator*=( const sh3& h ) { Mul( h ); return( *this ); };
	inline sh3&				operator*=( const float f ) { Mul( f ); return( *this ); };
	inline sh3&				operator/=( const float f ) { Div( f ); return( *this ); };
};

inline sh3 operator+( const sh3& src, const sh3& add ) { sh3 h; h.SetAdd( src, add ); return( h ); };
inline sh3 operator-( const sh3& src, const sh3& sub ) { sh3 h; h.SetSub( src, sub ); return( h ); };
inline sh3 operator*( const sh3& src, const sh3& mul ) { sh3 h; h.SetMul( src, mul ); return( h ); };
inline sh3 operator*( const sh3& src, const float mul ) { sh3 h; h.SetMul( src, mul ); return( h ); };
inline sh3 operator/( const sh3& src, const float div ) { sh3 h; h.SetDiv( src, div ); return( h ); };
inline sh3 operator-( const sh3& src ) { sh3 h; h.SetNegate( src ); return( h ); };

inline sh3 lerp( const sh3& a, const sh3& b, const float t ) { sh3 h; h.SetLerp( a, b, t ); return( h ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh3_rgb
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sh3_rgb
{
	sh3						r, g, b;
	bool					IsNan( void ) const;
	bool					IsReal( void ) const;
	void					Madd( const sh3_rgb& src, const float mul );
	void					Add( const sh3_rgb& add );
	void					Sub( const sh3_rgb& sub );
	void					Mul( const sh3_rgb& mul );
	void					Mul( const sh3& mul );
	void					Mul( const float mul );
	void					Div( const float div );
	void					Lerp( const sh3_rgb& trg, const float t );
	void					Negate( void );
	void					MulConsts( const vec3& cv );
	void					MulNormalize( void );
	void					MulSqrNormalize( void );
	void					SetMadd( const sh3_rgb& src, const float mul, const sh3_rgb& add );
	void					SetAdd( const sh3_rgb& src, const sh3_rgb& add );
	void					SetSub( const sh3_rgb& src, const sh3_rgb& sub );
	void					SetMul( const sh3_rgb& src, const sh3_rgb& mul );
	void					SetMul( const sh3_rgb& src, const sh3& mul );
	void					SetMul( const sh3_rgb& src, const float mul );
	void					SetDiv( const sh3_rgb& src, const float div );
	void					SetLerp( const sh3_rgb& src, const sh3_rgb& trg, const float t );
	void					SetNegate( const sh3_rgb& src );
	void					SetConsts( const vec3& cv );
	void					SetNormalize( void );
	void					SetSqrNormalize( void );
	void					SetNormalizedBasis( const vec3& v );
	void					SetUnitBasis( const vec3& v );
	void					SetBasis( const vec3& v );
	void					Set( const sh3& src );
	void					Set( const sh3_rgb& src );
	void					Get( sh3_rgb& trg ) const;
	inline sh3_rgb&			operator=( const sh3& h ) { Set( h ); return( *this ); };
	inline sh3_rgb&			operator=( const sh3_rgb& h ) { Set( h ); return( *this ); };
	inline sh3_rgb&			operator+=( const sh3_rgb& h ) { Add( h ); return( *this ); };
	inline sh3_rgb&			operator-=( const sh3_rgb& h ) { Sub( h ); return( *this ); };
	inline sh3_rgb&			operator*=( const sh3_rgb& h ) { Mul( h ); return( *this ); };
	inline sh3_rgb&			operator*=( const sh3& h ) { Mul( h ); return( *this ); };
	inline sh3_rgb&			operator*=( const float f ) { Mul( f ); return( *this ); };
	inline sh3_rgb&			operator/=( const float f ) { Div( f ); return( *this ); };
};

inline sh3_rgb operator+( const sh3_rgb& src, const sh3_rgb& add ) { sh3_rgb h; h.SetAdd( src, add ); return( h ); };
inline sh3_rgb operator-( const sh3_rgb& src, const sh3_rgb& sub ) { sh3_rgb h; h.SetSub( src, sub ); return( h ); };
inline sh3_rgb operator*( const sh3_rgb& src, const sh3_rgb& mul ) { sh3_rgb h; h.SetMul( src, mul ); return( h ); };
inline sh3_rgb operator*( const sh3_rgb& src, const sh3& mul ) { sh3_rgb h; h.SetMul( src, mul ); return( h ); };
inline sh3_rgb operator*( const sh3_rgb& src, const float mul ) { sh3_rgb h; h.SetMul( src, mul ); return( h ); };
inline sh3_rgb operator/( const sh3_rgb& src, const float div ) { sh3_rgb h; h.SetDiv( src, div ); return( h ); };
inline sh3_rgb operator-( const sh3_rgb& src ) { sh3_rgb h; h.SetNegate( src ); return( h ); };

inline sh3_rgb lerp( const sh3_rgb& a, const sh3_rgb& b, const float t ) { sh3_rgb h; h.SetLerp( a, b, t ); return( h ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh3_rgb_packed
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sh3_rgb_packed
{
	vec4					ac;
	vec4					r1;
	vec4					g1;
	vec4					b1;
	vec4					r2;
	vec4					g2;
	vec4					b2;
	void					Set( const sh3_rgb& src );
	void					Eval( const vec3& v, vec3& colour ) const;
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

#endif	//	#ifndef	__MATHS_SH_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

