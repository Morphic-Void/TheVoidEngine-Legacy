
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   sh3.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Spherical harmonics container and functions.
////
////    Notes:
////
////    	Radiance is illumination relative to the point surface normal (specular and reflection)
////    	Irradiance is diffuse illumination (relative to a hemisphere with cosine falloff about the surface normal).
////
////    	Normalized basis functions:
////
////    		Order 1 :	c0 :	 0 :	1
////    		Order 2 :	c1 :	-1 :	y
////    		Order 2 :	c2 :	 0 :	z
////    		Order 2 :	c3 :	+1 :	x
////    		Order 3 :	c4 :	-2 :	xy*root(3)
////    		Order 3 :	c5 :	-1 :	yz*root(3)
////    		Order 3 :	c6 :	 0 :	((zz*3)-1)/2
////    		Order 3 :	c7 :	+1 :	xz*root(3)
////    		Order 3 :	c8 :	+2 :	((xx-yy)*root(3)/2)
////
////    	Radiance to irradiance:
////
////    		Order 1 :	( 2pi / 2 )
////    		Order 2 :	( 2pi / 3 )
////    		Order 3 :	( 2pi / 8 )
////
////    	Irradiance to radiance:
////
////    		Order 1 :	( 2 / 2pi )
////    		Order 2 :	( 3 / 2pi )
////    		Order 3 :	( 8 / 2pi )
////
////    	Radiance:
////
////    		Order 1 :	( 1 / 4pi )
////    		Order 2 :	( 3 / 4pi )
////    		Order 3 :	( 5 / 4pi )
////
////    	Irradiance:
////
////    		Order 1 :	( 4 / 16 )
////    		Order 2 :	( 8 / 16 )
////    		Order 3 :	( 5 / 16 )
////
////    	Root radiance as irradiance:
////
////    		Order 1 :	root( 1pi / 4 )
////    		Order 2 :	root( 3pi / 9 )
////    		Order 3 :	root( 5pi / 64 )
////
////    	Root irradiance as radiance:
////
////    		Order1 :	( 1 / 2pi )
////    		Order2 :	( root( 4.5 ) / 2pi )
////    		Order3 :	( root( 20 ) / 2pi )
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

#include "sh.h"
#include "scalar.h"
#include "consts.h"
#include "libs/system/base/types.h"
#include "libs/system/debug/asserts.h"
#include <math.h>

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
////    consts
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace consts
{
	namespace SH
	{
		const float		rcp3 = ( 1.0f / 3.0f );
		const float		root3 = sqrtf ( 3.0f );
		const float		root3d2 = ( root3 * 0.5f );
		const vec3		Radiance = { ( 1.0f / ( consts::PI * 4.0f ) ), ( 3.0f / ( consts::PI * 4.0f ) ), ( 5.0f / ( consts::PI * 4.0f ) ) };
		const vec3		Irradiance = { ( 4.0f / 16.0f ), ( 8.0f / 16.0f ), ( 5.0f / 16.0f ) };
		const vec3		RadianceToIrradiance = { ( ( consts::PI * 2.0f ) / 2.0f ), ( ( consts::PI * 2.0f ) / 3.0f ), ( ( consts::PI * 2.0f ) / 8.0f ) };
		const vec3		IrradianceToRadiance = { ( 2.0f / ( consts::PI * 2.0f ) ), ( 3.0f / ( consts::PI * 2.0f ) ), ( 8.0f / ( consts::PI * 2.0f ) ) };
		const vec3		RootRadiance = { sqrtf( Radiance.x ), sqrtf( Radiance.y ), sqrtf( Radiance.z ) };
		const vec3		RootIrradiance = { sqrtf( Irradiance.x ), sqrtf( Irradiance.y ), sqrtf( Irradiance.z ) };
		const vec3		RootRadianceAsIrradiance = { sqrtf( ( consts::PI * 1.0f ) / 4.0f ), sqrtf( ( consts::PI * 3.0f ) / 9.0f ), sqrtf( ( consts::PI * 5.0f ) / 64.0f ) };
		const vec3		RootIrradianceAsRadiance = { ( 1.0f / ( consts::PI * 2.0f ) ), ( sqrtf( 4.5f ) / ( consts::PI * 2.0f ) ), ( sqrtf( 20.0f ) / ( consts::PI * 2.0f ) ) };
		const sh3_rgb	BREEZEWAY = {
			{ 0.32f,  0.37f, -0.01f, -0.10f, -0.13f, -0.01f, -0.07f,  0.02f, -0.29f },
			{ 0.36f,  0.41f, -0.01f, -0.12f, -0.15f, -0.02f, -0.08f,  0.03f, -0.32f },
			{ 0.38f,  0.45f, -0.01f, -0.12f, -0.17f,  0.02f, -0.09f,  0.03f, -0.36f } };
		const sh3_rgb	GALILEOS_TOMB = {
			{ 1.04f,  0.44f, -0.22f,  0.71f,  0.64f, -0.12f, -0.37f, -0.17f,  0.55f },
			{ 0.76f,  0.34f, -0.18f,  0.54f,  0.50f, -0.09f, -0.28f, -0.13f,  0.42f },
			{ 0.71f,  0.34f, -0.17f,  0.56f,  0.52f, -0.08f, -0.29f, -0.13f,  0.42f } };
		const sh3_rgb	CAMPUS_SUNSET = {
			{ 0.79f,  0.44f, -0.10f,  0.45f,  0.18f, -0.14f, -0.39f,  0.09f,  0.67f },
			{ 0.94f,  0.56f, -0.18f,  0.38f,  0.14f, -0.22f, -0.40f,  0.07f,  0.67f },
			{ 0.98f,  0.70f, -0.27f,  0.20f,  0.05f, -0.31f, -0.36f,  0.04f,  0.52f } };
		const sh3_rgb	UFFUZI_GALLERY = {
			{ 0.32f,  0.37f,  0.00f, -0.01f, -0.02f, -0.01f, -0.28f,  0.00f, -0.24f },
			{ 0.31f,  0.37f,  0.00f, -0.01f, -0.02f, -0.01f, -0.28f,  0.00f, -0.24f },
			{ 0.35f,  0.43f,  0.00f, -0.01f, -0.03f, -0.01f, -0.32f,  0.00f, -0.28f } };
		const sh3_rgb	OLD_TOWN_SQUARE = {
			{ 0.87f,  0.18f,  0.03f,  0.00f, -0.12f,  0.00f, -0.03f, -0.08f, -0.16f },
			{ 0.88f,  0.25f,  0.04f, -0.03f, -0.12f,  0.00f, -0.02f, -0.09f, -0.19f },
			{ 0.86f,  0.31f,  0.04f, -0.05f, -0.12f,  0.01f, -0.02f, -0.09f, -0.22f } };
		const sh3_rgb	GRACE_CATHEDRAL = {
			{ 0.79f,  0.39f, -0.34f, -0.29f, -0.11f, -0.26f, -0.16f,  0.56f,  0.21f },
			{ 0.44f,  0.35f, -0.18f, -0.06f, -0.05f, -0.22f, -0.09f,  0.21f, -0.05f },
			{ 0.54f,  0.60f, -0.27f,  0.01f, -0.12f, -0.47f, -0.15f,  0.14f, -0.30f } };
		const sh3_rgb	VINE_ST_KITCHEN = {
			{ 0.64f,  0.28f,  0.42f, -0.05f, -0.10f,  0.25f,  0.38f,  0.06f, -0.03f },
			{ 0.67f,  0.32f,  0.60f, -0.04f, -0.08f,  0.39f,  0.54f,  0.01f, -0.02f },
			{ 0.73f,  0.33f,  0.77f, -0.02f, -0.05f,  0.53f,  0.71f, -0.02f, -0.03f } };
		const sh3_rgb	EUCALYPTUS_GROVE = {
			{ 0.38f,  0.29f,  0.04f, -0.10f, -0.06f,  0.01f, -0.09f, -0.06f,  0.02f },
			{ 0.43f,  0.36f,  0.03f, -0.10f, -0.06f,  0.01f, -0.13f, -0.05f,  0.00f },
			{ 0.45f,  0.41f,  0.01f, -0.09f, -0.04f, -0.05f, -0.15f, -0.04f,  0.05f } };
		const sh3_rgb	ST_PETERS_BASILICA = {
			{ 0.36f,  0.18f, -0.02f,  0.03f,  0.02f, -0.05f, -0.09f,  0.01f, -0.08f },
			{ 0.26f,  0.14f, -0.01f,  0.02f,  0.01f, -0.03f, -0.08f,  0.00f, -0.03f },
			{ 0.23f,  0.13f,  0.00f,  0.00f,  0.00f, -0.01f, -0.07f,  0.00f,  0.00f } };
		const sh3_rgb	FUNSTON_BEACH_SUNSET = {
			{ 0.68f,  0.32f, -0.17f, -0.45f, -0.17f, -0.08f, -0.03f,  0.16f,  0.37f },
			{ 0.69f,  0.37f, -0.17f, -0.42f, -0.17f, -0.09f, -0.02f,  0.14f,  0.31f },
			{ 0.70f,  0.44f, -0.17f, -0.34f, -0.15f, -0.10f, -0.01f,  0.10f,  0.20f } };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh2
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool sh2::IsNan( void ) const
{
	return( ::IsNan( c[ 0 ] ) || ::IsNan( c[ 1 ] ) || ::IsNan( c[ 2 ] ) || ::IsNan( c[ 3 ] ) );
}

bool sh2::IsReal( void ) const
{
	return( ::IsReal( c[ 0 ] ) && ::IsReal( c[ 1 ] ) && ::IsReal( c[ 2 ] ) && ::IsReal( c[ 3 ] ) );
}

void sh2::Madd( const sh2& src, const float mul )
{
	c[ 0 ] += ( src.c[ 0 ] * mul );
	c[ 1 ] += ( src.c[ 1 ] * mul );
	c[ 2 ] += ( src.c[ 2 ] * mul );
	c[ 3 ] += ( src.c[ 3 ] * mul );
}

void sh2::Add( const sh2& add )
{
	c[ 0 ] += add.c[ 0 ];
	c[ 1 ] += add.c[ 1 ];
	c[ 2 ] += add.c[ 2 ];
	c[ 3 ] += add.c[ 3 ];
}

void sh2::Sub( const sh2& sub )
{
	c[ 0 ] -= sub.c[ 0 ];
	c[ 1 ] -= sub.c[ 1 ];
	c[ 2 ] -= sub.c[ 2 ];
	c[ 3 ] -= sub.c[ 3 ];
}

void sh2::Mul( const sh2& mul )
{
	c[ 0 ] *= mul.c[ 0 ];
	c[ 1 ] *= mul.c[ 1 ];
	c[ 2 ] *= mul.c[ 2 ];
	c[ 3 ] *= mul.c[ 3 ];
}

void sh2::Mul( const float mul )
{
	c[ 0 ] *= mul;
	c[ 1 ] *= mul;
	c[ 2 ] *= mul;
	c[ 3 ] *= mul;
}

void sh2::Div( const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	Mul( 1.0f / div );
}

void sh2::Lerp( const sh2& trg, const float t )
{
	float s = ( 1.0f - t );
	c[ 0 ] = ( ( c[ 0 ] * s ) + ( trg.c[ 0 ] * t ) );
	c[ 1 ] = ( ( c[ 1 ] * s ) + ( trg.c[ 1 ] * t ) );
	c[ 2 ] = ( ( c[ 2 ] * s ) + ( trg.c[ 2 ] * t ) );
	c[ 3 ] = ( ( c[ 3 ] * s ) + ( trg.c[ 3 ] * t ) );
}

void sh2::Negate( void )
{
	c[ 0 ] = -c[ 0 ];
	c[ 1 ] = -c[ 1 ];
	c[ 2 ] = -c[ 2 ];
	c[ 3 ] = -c[ 3 ];
}

void sh2::MulConsts( const vec3& cv )
{
	c[ 0 ] *= cv.x;
	c[ 1 ] *= cv.y;
	c[ 2 ] *= cv.y;
	c[ 3 ] *= cv.y;
}

void sh2::SetMadd( const sh2& src, const float mul, const sh2& add )
{
	c[ 0 ] = ( ( src.c[ 0 ] * mul ) + add.c[ 0 ] );
	c[ 1 ] = ( ( src.c[ 1 ] * mul ) + add.c[ 1 ] );
	c[ 2 ] = ( ( src.c[ 2 ] * mul ) + add.c[ 2 ] );
	c[ 3 ] = ( ( src.c[ 3 ] * mul ) + add.c[ 3 ] );
}

void sh2::SetAdd( const sh2& src, const sh2& add )
{
	c[ 0 ] = ( src.c[ 0 ] + add.c[ 0 ] );
	c[ 1 ] = ( src.c[ 1 ] + add.c[ 1 ] );
	c[ 2 ] = ( src.c[ 2 ] + add.c[ 2 ] );
	c[ 3 ] = ( src.c[ 3 ] + add.c[ 3 ] );
}

void sh2::SetSub( const sh2& src, const sh2& sub )
{
	c[ 0 ] = ( src.c[ 0 ] - sub.c[ 0 ] );
	c[ 1 ] = ( src.c[ 1 ] - sub.c[ 1 ] );
	c[ 2 ] = ( src.c[ 2 ] - sub.c[ 2 ] );
	c[ 3 ] = ( src.c[ 3 ] - sub.c[ 3 ] );
}

void sh2::SetMul( const sh2& src, const sh2& mul )
{
	c[ 0 ] = ( src.c[ 0 ] * mul.c[ 0 ] );
	c[ 1 ] = ( src.c[ 1 ] * mul.c[ 1 ] );
	c[ 2 ] = ( src.c[ 2 ] * mul.c[ 2 ] );
	c[ 3 ] = ( src.c[ 3 ] * mul.c[ 3 ] );
}

void sh2::SetMul( const sh2& src, const float mul )
{
	c[ 0 ] = ( src.c[ 0 ] * mul );
	c[ 1 ] = ( src.c[ 1 ] * mul );
	c[ 2 ] = ( src.c[ 2 ] * mul );
	c[ 3 ] = ( src.c[ 3 ] * mul );
}

void sh2::SetDiv( const sh2& src, const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	SetMul( src, ( 1.0f / div ) );
}

void sh2::SetLerp( const sh2& src, const sh2& trg, const float t )
{
	float s = ( 1.0f - t );
	c[ 0 ] = ( ( src.c[ 0 ] * s ) + ( trg.c[ 0 ] * t ) );
	c[ 1 ] = ( ( src.c[ 1 ] * s ) + ( trg.c[ 1 ] * t ) );
	c[ 2 ] = ( ( src.c[ 2 ] * s ) + ( trg.c[ 2 ] * t ) );
	c[ 3 ] = ( ( src.c[ 3 ] * s ) + ( trg.c[ 3 ] * t ) );
}

void sh2::SetNegate( const sh2& src )
{
	c[ 0 ] = -src.c[ 0 ];
	c[ 1 ] = -src.c[ 1 ];
	c[ 2 ] = -src.c[ 2 ];
	c[ 3 ] = -src.c[ 3 ];
}

void sh2::SetConsts( const vec3& cv )
{
	c[ 0 ] = cv.x;
	c[ 1 ] = cv.y;
	c[ 2 ] = cv.y;
	c[ 3 ] = cv.y;
}

void sh2::SetBasis( const vec3& v )
{
	c[ 0 ] = 1;
	c[ 1 ] = v.y;
	c[ 2 ] = v.z;
	c[ 3 ] = v.x;
}

void sh2::Set( const sh3& src )
{
	c[ 0 ] = src.c[ 0 ];
	c[ 1 ] = src.c[ 1 ];
	c[ 2 ] = src.c[ 2 ];
	c[ 3 ] = src.c[ 3 ];
}

void sh2::Set( const sh2& src )
{
	c[ 0 ] = src.c[ 0 ];
	c[ 1 ] = src.c[ 1 ];
	c[ 2 ] = src.c[ 2 ];
	c[ 3 ] = src.c[ 3 ];
}

void sh2::Get( sh2& trg ) const
{
	trg.c[ 0 ] = c[ 0 ];
	trg.c[ 1 ] = c[ 1 ];
	trg.c[ 2 ] = c[ 2 ];
	trg.c[ 3 ] = c[ 3 ];
}

float sh2::GetCoefficient( const int index ) const
{
	return( ( static_cast< unsigned int >( index ) < 4 ) ? c[ index ] : 0 );
}

float sh2::GetOrder1( void ) const
{
	return( c[ 0 ] );
}

float sh2::GetOrder2( void ) const
{
	return( c[ 0 ] + c[ 1 ] + c[ 2 ] + c[ 3 ] );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh2_rgb
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool sh2_rgb::IsNan( void ) const
{
	return( r.IsNan() || g.IsNan() || b.IsNan() );
}

bool sh2_rgb::IsReal( void ) const
{
	return( r.IsReal() && g.IsReal() && b.IsReal() );
}

void sh2_rgb::Madd( const sh2_rgb& src, const float mul )
{
	r.Madd( src.r, mul );
	g.Madd( src.g, mul );
	b.Madd( src.b, mul );
}

void sh2_rgb::Add( const sh2_rgb& add )
{
	r.Add( add.r );
	g.Add( add.g );
	b.Add( add.b );
}

void sh2_rgb::Sub( const sh2_rgb& sub )
{
	r.Sub( sub.r );
	g.Sub( sub.g );
	b.Sub( sub.b );
}

void sh2_rgb::Mul( const sh2_rgb& mul )
{
	r.Mul( mul.r );
	g.Mul( mul.g );
	b.Mul( mul.b );
}

void sh2_rgb::Mul( const sh2& mul )
{
	r.Mul( mul );
	g.Mul( mul );
	b.Mul( mul );
}

void sh2_rgb::Mul( const float mul )
{
	r.Mul( mul );
	g.Mul( mul );
	b.Mul( mul );
}

void sh2_rgb::Div( const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	float mul = ( 1.0f / div );
	r.Mul( mul );
	g.Mul( mul );
	b.Mul( mul );
}

void sh2_rgb::Lerp( const sh2_rgb& trg, const float t )
{
	float s = ( 1.0f - t );
	r.c[ 0 ] = ( ( r.c[ 0 ] * s ) + ( trg.r.c[ 0 ] * t ) );
	r.c[ 1 ] = ( ( r.c[ 1 ] * s ) + ( trg.r.c[ 1 ] * t ) );
	r.c[ 2 ] = ( ( r.c[ 2 ] * s ) + ( trg.r.c[ 2 ] * t ) );
	r.c[ 3 ] = ( ( r.c[ 3 ] * s ) + ( trg.r.c[ 3 ] * t ) );
	g.c[ 0 ] = ( ( g.c[ 0 ] * s ) + ( trg.g.c[ 0 ] * t ) );
	g.c[ 1 ] = ( ( g.c[ 1 ] * s ) + ( trg.g.c[ 1 ] * t ) );
	g.c[ 2 ] = ( ( g.c[ 2 ] * s ) + ( trg.g.c[ 2 ] * t ) );
	g.c[ 3 ] = ( ( g.c[ 3 ] * s ) + ( trg.g.c[ 3 ] * t ) );
	b.c[ 0 ] = ( ( b.c[ 0 ] * s ) + ( trg.b.c[ 0 ] * t ) );
	b.c[ 1 ] = ( ( b.c[ 1 ] * s ) + ( trg.b.c[ 1 ] * t ) );
	b.c[ 2 ] = ( ( b.c[ 2 ] * s ) + ( trg.b.c[ 2 ] * t ) );
	b.c[ 3 ] = ( ( b.c[ 3 ] * s ) + ( trg.b.c[ 3 ] * t ) );
}

void sh2_rgb::Negate( void )
{
	r.Negate();
	g.Negate();
	b.Negate();
}

void sh2_rgb::MulConsts( const vec3& cv )
{
	r.MulConsts( cv );
	g.MulConsts( cv );
	b.MulConsts( cv );
}

void sh2_rgb::SetMadd( const sh2_rgb& src, const float mul, const sh2_rgb& add )
{
	r.SetMadd( src.r, mul, add.r );
	g.SetMadd( src.g, mul, add.g );
	b.SetMadd( src.b, mul, add.b );
}

void sh2_rgb::SetAdd( const sh2_rgb& src, const sh2_rgb& add )
{
	r.SetAdd( src.r, add.r );
	g.SetAdd( src.g, add.g );
	b.SetAdd( src.b, add.b );
}

void sh2_rgb::SetSub( const sh2_rgb& src, const sh2_rgb& sub )
{
	r.SetSub( src.r, sub.r );
	g.SetSub( src.g, sub.g );
	b.SetSub( src.b, sub.b );
}

void sh2_rgb::SetMul( const sh2_rgb& src, const sh2_rgb& mul )
{
	r.SetMul( src.r, mul.r );
	g.SetMul( src.g, mul.g );
	b.SetMul( src.b, mul.b );
}

void sh2_rgb::SetMul( const sh2_rgb& src, const sh2& mul )
{
	r.SetMul( src.r, mul );
	g.SetMul( src.g, mul );
	b.SetMul( src.b, mul );
}

void sh2_rgb::SetMul( const sh2_rgb& src, const float mul )
{
	r.SetMul( src.r, mul );
	g.SetMul( src.g, mul );
	b.SetMul( src.b, mul );
}

void sh2_rgb::SetDiv( const sh2_rgb& src, const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	float mul = ( 1.0f / div );
	r.SetMul( src.r, mul );
	g.SetMul( src.g, mul );
	b.SetMul( src.b, mul );
}

void sh2_rgb::SetLerp( const sh2_rgb& src, const sh2_rgb& trg, const float t )
{
	float s = ( 1.0f - t );
	r.c[ 0 ] = ( ( src.r.c[ 0 ] * s ) + ( trg.r.c[ 0 ] * t ) );
	r.c[ 1 ] = ( ( src.r.c[ 1 ] * s ) + ( trg.r.c[ 1 ] * t ) );
	r.c[ 2 ] = ( ( src.r.c[ 2 ] * s ) + ( trg.r.c[ 2 ] * t ) );
	r.c[ 3 ] = ( ( src.r.c[ 3 ] * s ) + ( trg.r.c[ 3 ] * t ) );
	g.c[ 0 ] = ( ( src.g.c[ 0 ] * s ) + ( trg.g.c[ 0 ] * t ) );
	g.c[ 1 ] = ( ( src.g.c[ 1 ] * s ) + ( trg.g.c[ 1 ] * t ) );
	g.c[ 2 ] = ( ( src.g.c[ 2 ] * s ) + ( trg.g.c[ 2 ] * t ) );
	g.c[ 3 ] = ( ( src.g.c[ 3 ] * s ) + ( trg.g.c[ 3 ] * t ) );
	b.c[ 0 ] = ( ( src.b.c[ 0 ] * s ) + ( trg.b.c[ 0 ] * t ) );
	b.c[ 1 ] = ( ( src.b.c[ 1 ] * s ) + ( trg.b.c[ 1 ] * t ) );
	b.c[ 2 ] = ( ( src.b.c[ 2 ] * s ) + ( trg.b.c[ 2 ] * t ) );
	b.c[ 3 ] = ( ( src.b.c[ 3 ] * s ) + ( trg.b.c[ 3 ] * t ) );
}

void sh2_rgb::SetNegate( const sh2_rgb& src )
{
	r.SetNegate( src.r );
	g.SetNegate( src.g );
	b.SetNegate( src.b );
}

void sh2_rgb::SetConsts( const vec3& cv )
{
	r.c[ 0 ] = g.c[ 0 ] = b.c[ 0 ] = cv.x;
	r.c[ 1 ] = g.c[ 1 ] = b.c[ 1 ] = cv.y;
	r.c[ 2 ] = g.c[ 2 ] = b.c[ 2 ] = cv.y;
	r.c[ 3 ] = g.c[ 3 ] = b.c[ 3 ] = cv.y;
}

void sh2_rgb::SetBasis( const vec3& v )
{
	r.c[ 0 ] = g.c[ 0 ] = b.c[ 0 ] = 1;
	r.c[ 1 ] = g.c[ 1 ] = b.c[ 1 ] = v.y;
	r.c[ 2 ] = g.c[ 2 ] = b.c[ 2 ] = v.z;
	r.c[ 3 ] = g.c[ 3 ] = b.c[ 3 ] = v.x;
}

void sh2_rgb::Set( const sh3& src )
{
	r.Set( src );
	g.Set( src );
	b.Set( src );
}

void sh2_rgb::Set( const sh2& src )
{
	r.Set( src );
	g.Set( src );
	b.Set( src );
}

void sh2_rgb::Set( const sh3_rgb& src )
{
	r.Set( src.r );
	g.Set( src.g );
	b.Set( src.b );
}

void sh2_rgb::Set( const sh2_rgb& src )
{
	r.Set( src.r );
	g.Set( src.g );
	b.Set( src.b );
}

void sh2_rgb::Get( sh2_rgb& trg ) const
{
	r.Get( trg.r );
	g.Get( trg.g );
	b.Get( trg.b );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh2_rgb_packed
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sh2_rgb_packed::Set( const sh2_rgb& src )
{
	r1.x = src.r.c[ 2 ];
	r1.y = src.r.c[ 3 ];
	r1.z = src.r.c[ 1 ];
	r1.w = src.r.c[ 0 ];
	g1.x = src.g.c[ 2 ];
	g1.y = src.g.c[ 3 ];
	g1.z = src.g.c[ 1 ];
	g1.w = src.g.c[ 0 ];
	b1.x = src.b.c[ 2 ];
	b1.y = src.b.c[ 3 ];
	b1.z = src.b.c[ 1 ];
	b1.w = src.b.c[ 0 ];
}

void sh2_rgb_packed::Eval( const vec3& v, vec3& colour ) const
{
	vec4 v1;
	v1.x = v.x;
	v1.y = v.y;
	v1.z = v.z;
	v1.w = 1.0f;
	colour.x = v1.Dot( r1 );
	colour.y = v1.Dot( g1 );
	colour.z = v1.Dot( b1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh3
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool sh3::IsNan( void ) const
{
	return(
		::IsNan( c[ 0 ] ) || ::IsNan( c[ 1 ] ) || ::IsNan( c[ 2 ] ) ||
		::IsNan( c[ 3 ] ) || ::IsNan( c[ 4 ] ) || ::IsNan( c[ 5 ] ) ||
		::IsNan( c[ 6 ] ) || ::IsNan( c[ 7 ] ) || ::IsNan( c[ 8 ] ) );
}

bool sh3::IsReal( void ) const
{
	return(
		::IsReal( c[ 0 ] ) && ::IsReal( c[ 1 ] ) && ::IsReal( c[ 2 ] ) &&
		::IsReal( c[ 3 ] ) && ::IsReal( c[ 4 ] ) && ::IsReal( c[ 5 ] ) &&
		::IsReal( c[ 6 ] ) && ::IsReal( c[ 7 ] ) && ::IsReal( c[ 8 ] ) );
}

void sh3::Madd( const sh3& src, const float mul )
{
	c[ 0 ] += ( src.c[ 0 ] * mul );
	c[ 1 ] += ( src.c[ 1 ] * mul );
	c[ 2 ] += ( src.c[ 2 ] * mul );
	c[ 3 ] += ( src.c[ 3 ] * mul );
	c[ 4 ] += ( src.c[ 4 ] * mul );
	c[ 5 ] += ( src.c[ 5 ] * mul );
	c[ 6 ] += ( src.c[ 6 ] * mul );
	c[ 7 ] += ( src.c[ 7 ] * mul );
	c[ 8 ] += ( src.c[ 8 ] * mul );
}

void sh3::Add( const sh3& add )
{
	c[ 0 ] += add.c[ 0 ];
	c[ 1 ] += add.c[ 1 ];
	c[ 2 ] += add.c[ 2 ];
	c[ 3 ] += add.c[ 3 ];
	c[ 4 ] += add.c[ 4 ];
	c[ 5 ] += add.c[ 5 ];
	c[ 6 ] += add.c[ 6 ];
	c[ 7 ] += add.c[ 7 ];
	c[ 8 ] += add.c[ 8 ];
}

void sh3::Sub( const sh3& sub )
{
	c[ 0 ] -= sub.c[ 0 ];
	c[ 1 ] -= sub.c[ 1 ];
	c[ 2 ] -= sub.c[ 2 ];
	c[ 3 ] -= sub.c[ 3 ];
	c[ 4 ] -= sub.c[ 4 ];
	c[ 5 ] -= sub.c[ 5 ];
	c[ 6 ] -= sub.c[ 6 ];
	c[ 7 ] -= sub.c[ 7 ];
	c[ 8 ] -= sub.c[ 8 ];
}

void sh3::Mul( const sh3& mul )
{
	c[ 0 ] *= mul.c[ 0 ];
	c[ 1 ] *= mul.c[ 1 ];
	c[ 2 ] *= mul.c[ 2 ];
	c[ 3 ] *= mul.c[ 3 ];
	c[ 4 ] *= mul.c[ 4 ];
	c[ 5 ] *= mul.c[ 5 ];
	c[ 6 ] *= mul.c[ 6 ];
	c[ 7 ] *= mul.c[ 7 ];
	c[ 8 ] *= mul.c[ 8 ];
}

void sh3::Mul( const float mul )
{
	c[ 0 ] *= mul;
	c[ 1 ] *= mul;
	c[ 2 ] *= mul;
	c[ 3 ] *= mul;
	c[ 4 ] *= mul;
	c[ 5 ] *= mul;
	c[ 6 ] *= mul;
	c[ 7 ] *= mul;
	c[ 8 ] *= mul;
}

void sh3::Div( const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	Mul( 1.0f / div );
}

void sh3::Lerp( const sh3& trg, const float t )
{
	float s = ( 1.0f - t );
	c[ 0 ] = ( ( c[ 0 ] * s ) + ( trg.c[ 0 ] * t ) );
	c[ 1 ] = ( ( c[ 1 ] * s ) + ( trg.c[ 1 ] * t ) );
	c[ 2 ] = ( ( c[ 2 ] * s ) + ( trg.c[ 2 ] * t ) );
	c[ 3 ] = ( ( c[ 3 ] * s ) + ( trg.c[ 3 ] * t ) );
	c[ 4 ] = ( ( c[ 4 ] * s ) + ( trg.c[ 4 ] * t ) );
	c[ 5 ] = ( ( c[ 5 ] * s ) + ( trg.c[ 5 ] * t ) );
	c[ 6 ] = ( ( c[ 6 ] * s ) + ( trg.c[ 6 ] * t ) );
	c[ 7 ] = ( ( c[ 7 ] * s ) + ( trg.c[ 7 ] * t ) );
	c[ 8 ] = ( ( c[ 8 ] * s ) + ( trg.c[ 8 ] * t ) );
}

void sh3::Negate( void )
{
	c[ 0 ] = -c[ 0 ];
	c[ 1 ] = -c[ 1 ];
	c[ 2 ] = -c[ 2 ];
	c[ 3 ] = -c[ 3 ];
	c[ 4 ] = -c[ 4 ];
	c[ 5 ] = -c[ 5 ];
	c[ 6 ] = -c[ 6 ];
	c[ 7 ] = -c[ 7 ];
	c[ 8 ] = -c[ 8 ];
}

void sh3::MulConsts( const vec3& cv )
{
	c[ 0 ] *= cv.x;
	c[ 1 ] *= cv.y;
	c[ 2 ] *= cv.y;
	c[ 3 ] *= cv.y;
	c[ 4 ] *= cv.z;
	c[ 5 ] *= cv.z;
	c[ 6 ] *= cv.z;
	c[ 7 ] *= cv.z;
	c[ 8 ] *= cv.z;
}

void sh3::MulNormalize( void )
{
	c[ 4 ] *= consts::SH::root3;
	c[ 5 ] *= consts::SH::root3;
	c[ 6 ] *= 1.5f;
	c[ 7 ] *= consts::SH::root3;
	c[ 8 ] *= consts::SH::root3d2;
}

void sh3::MulSqrNormalize( void )
{
	c[ 4 ] *= 3.0f;
	c[ 5 ] *= 3.0f;
	c[ 6 ] *= 2.25f;
	c[ 7 ] *= 3.0f;
	c[ 8 ] *= 0.75f;
}

void sh3::SetMadd( const sh3& src, const float mul, const sh3& add )
{
	c[ 0 ] = ( ( src.c[ 0 ] * mul ) + add.c[ 0 ] );
	c[ 1 ] = ( ( src.c[ 1 ] * mul ) + add.c[ 1 ] );
	c[ 2 ] = ( ( src.c[ 2 ] * mul ) + add.c[ 2 ] );
	c[ 3 ] = ( ( src.c[ 3 ] * mul ) + add.c[ 3 ] );
	c[ 4 ] = ( ( src.c[ 4 ] * mul ) + add.c[ 4 ] );
	c[ 5 ] = ( ( src.c[ 5 ] * mul ) + add.c[ 5 ] );
	c[ 6 ] = ( ( src.c[ 6 ] * mul ) + add.c[ 6 ] );
	c[ 7 ] = ( ( src.c[ 7 ] * mul ) + add.c[ 7 ] );
	c[ 8 ] = ( ( src.c[ 8 ] * mul ) + add.c[ 8 ] );
}

void sh3::SetAdd( const sh3& src, const sh3& add )
{
	c[ 0 ] = ( src.c[ 0 ] + add.c[ 0 ] );
	c[ 1 ] = ( src.c[ 1 ] + add.c[ 1 ] );
	c[ 2 ] = ( src.c[ 2 ] + add.c[ 2 ] );
	c[ 3 ] = ( src.c[ 3 ] + add.c[ 3 ] );
	c[ 4 ] = ( src.c[ 4 ] + add.c[ 4 ] );
	c[ 5 ] = ( src.c[ 5 ] + add.c[ 5 ] );
	c[ 6 ] = ( src.c[ 6 ] + add.c[ 6 ] );
	c[ 7 ] = ( src.c[ 7 ] + add.c[ 7 ] );
	c[ 8 ] = ( src.c[ 8 ] + add.c[ 8 ] );
}

void sh3::SetSub( const sh3& src, const sh3& sub )
{
	c[ 0 ] = ( src.c[ 0 ] - sub.c[ 0 ] );
	c[ 1 ] = ( src.c[ 1 ] - sub.c[ 1 ] );
	c[ 2 ] = ( src.c[ 2 ] - sub.c[ 2 ] );
	c[ 3 ] = ( src.c[ 3 ] - sub.c[ 3 ] );
	c[ 4 ] = ( src.c[ 4 ] - sub.c[ 4 ] );
	c[ 5 ] = ( src.c[ 5 ] - sub.c[ 5 ] );
	c[ 6 ] = ( src.c[ 6 ] - sub.c[ 6 ] );
	c[ 7 ] = ( src.c[ 7 ] - sub.c[ 7 ] );
	c[ 8 ] = ( src.c[ 8 ] - sub.c[ 8 ] );
}

void sh3::SetMul( const sh3& src, const sh3& mul )
{
	c[ 0 ] = ( src.c[ 0 ] * mul.c[ 0 ] );
	c[ 1 ] = ( src.c[ 1 ] * mul.c[ 1 ] );
	c[ 2 ] = ( src.c[ 2 ] * mul.c[ 2 ] );
	c[ 3 ] = ( src.c[ 3 ] * mul.c[ 3 ] );
	c[ 4 ] = ( src.c[ 4 ] * mul.c[ 4 ] );
	c[ 5 ] = ( src.c[ 5 ] * mul.c[ 5 ] );
	c[ 6 ] = ( src.c[ 6 ] * mul.c[ 6 ] );
	c[ 7 ] = ( src.c[ 7 ] * mul.c[ 7 ] );
	c[ 8 ] = ( src.c[ 8 ] * mul.c[ 8 ] );
}

void sh3::SetMul( const sh3& src, const float mul )
{
	c[ 0 ] = ( src.c[ 0 ] * mul );
	c[ 1 ] = ( src.c[ 1 ] * mul );
	c[ 2 ] = ( src.c[ 2 ] * mul );
	c[ 3 ] = ( src.c[ 3 ] * mul );
	c[ 4 ] = ( src.c[ 4 ] * mul );
	c[ 5 ] = ( src.c[ 5 ] * mul );
	c[ 6 ] = ( src.c[ 6 ] * mul );
	c[ 7 ] = ( src.c[ 7 ] * mul );
	c[ 8 ] = ( src.c[ 8 ] * mul );
}

void sh3::SetDiv( const sh3& src, const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	SetMul( src, ( 1.0f / div ) );
}

void sh3::SetLerp( const sh3& src, const sh3& trg, const float t )
{
	float s = ( 1.0f - t );
	c[ 0 ] = ( ( src.c[ 0 ] * s ) + ( trg.c[ 0 ] * t ) );
	c[ 1 ] = ( ( src.c[ 1 ] * s ) + ( trg.c[ 1 ] * t ) );
	c[ 2 ] = ( ( src.c[ 2 ] * s ) + ( trg.c[ 2 ] * t ) );
	c[ 3 ] = ( ( src.c[ 3 ] * s ) + ( trg.c[ 3 ] * t ) );
	c[ 4 ] = ( ( src.c[ 4 ] * s ) + ( trg.c[ 4 ] * t ) );
	c[ 5 ] = ( ( src.c[ 5 ] * s ) + ( trg.c[ 5 ] * t ) );
	c[ 6 ] = ( ( src.c[ 6 ] * s ) + ( trg.c[ 6 ] * t ) );
	c[ 7 ] = ( ( src.c[ 7 ] * s ) + ( trg.c[ 7 ] * t ) );
	c[ 8 ] = ( ( src.c[ 8 ] * s ) + ( trg.c[ 8 ] * t ) );
}

void sh3::SetNegate( const sh3& src )
{
	c[ 0 ] = -src.c[ 0 ];
	c[ 1 ] = -src.c[ 1 ];
	c[ 2 ] = -src.c[ 2 ];
	c[ 3 ] = -src.c[ 3 ];
	c[ 4 ] = -src.c[ 4 ];
	c[ 5 ] = -src.c[ 5 ];
	c[ 6 ] = -src.c[ 6 ];
	c[ 7 ] = -src.c[ 7 ];
	c[ 8 ] = -src.c[ 8 ];
}

void sh3::SetConsts( const vec3& cv )
{
	c[ 0 ] = cv.x;
	c[ 1 ] = cv.y;
	c[ 2 ] = cv.y;
	c[ 3 ] = cv.y;
	c[ 4 ] = cv.z;
	c[ 5 ] = cv.z;
	c[ 6 ] = cv.z;
	c[ 7 ] = cv.z;
	c[ 8 ] = cv.z;
}

void sh3::SetNormalize( void )
{
	c[ 0 ] = c[ 1 ] = c[ 2 ] = c[ 3 ] = 1.0f;
	c[ 4 ] = c[ 5 ] = c[ 7 ] = consts::SH::root3;
	c[ 6 ] = 1.5f;
	c[ 8 ] = consts::SH::root3d2;
}

void sh3::SetSqrNormalize( void )
{
	c[ 0 ] = c[ 1 ] = c[ 2 ] = c[ 3 ] = 1.0f;
	c[ 4 ] = c[ 5 ] = c[ 7 ] = 3.0f;
	c[ 6 ] = 2.25f;
	c[ 8 ] = 0.75f;
}

void sh3::SetNormalizedBasis( const vec3& v )
{
	c[ 0 ] = 1;
	c[ 1 ] = v.y;
	c[ 2 ] = v.z;
	c[ 3 ] = v.x;
	c[ 4 ] = ( v.x * v.y * consts::SH::root3 );
	c[ 5 ] = ( v.y * v.z * consts::SH::root3 );
	c[ 6 ] = ( ( v.z * v.z * 1.5f ) - 0.5f );
	c[ 7 ] = ( v.z * v.x * consts::SH::root3 );
	c[ 8 ] = ( ( ( v.x * v.x ) - ( v.y * v.y ) ) * consts::SH::root3d2 );
}

void sh3::SetUnitBasis( const vec3& v )
{
	c[ 0 ] = 1;
	c[ 1 ] = v.y;
	c[ 2 ] = v.z;
	c[ 3 ] = v.x;
	c[ 4 ] = ( v.x * v.y * 2 );
	c[ 5 ] = ( v.y * v.z * 2 );
	c[ 6 ] = ( ( v.z * v.z * 1.5f ) - 0.5f );
	c[ 7 ] = ( v.z * v.x * 2 );
	c[ 8 ] = ( ( v.x * v.x ) - ( v.y * v.y ) );
}

void sh3::SetBasis( const vec3& v )
{
	c[ 0 ] = 1;
	c[ 1 ] = v.y;
	c[ 2 ] = v.z;
	c[ 3 ] = v.x;
	c[ 4 ] = ( v.x * v.y );
	c[ 5 ] = ( v.y * v.z );
	c[ 6 ] = ( ( v.z * v.z ) - consts::SH::rcp3 );
	c[ 7 ] = ( v.z * v.x );
	c[ 8 ] = ( ( v.x * v.x ) - ( v.y * v.y ) );
}

void sh3::Set( const sh3& src )
{
	c[ 0 ] = src.c[ 0 ];
	c[ 1 ] = src.c[ 1 ];
	c[ 2 ] = src.c[ 2 ];
	c[ 3 ] = src.c[ 3 ];
	c[ 4 ] = src.c[ 4 ];
	c[ 5 ] = src.c[ 5 ];
	c[ 6 ] = src.c[ 6 ];
	c[ 7 ] = src.c[ 7 ];
	c[ 8 ] = src.c[ 8 ];
}

void sh3::Get( sh3& trg ) const
{
	trg.c[ 0 ] = c[ 0 ];
	trg.c[ 1 ] = c[ 1 ];
	trg.c[ 2 ] = c[ 2 ];
	trg.c[ 3 ] = c[ 3 ];
	trg.c[ 4 ] = c[ 4 ];
	trg.c[ 5 ] = c[ 5 ];
	trg.c[ 6 ] = c[ 6 ];
	trg.c[ 7 ] = c[ 7 ];
	trg.c[ 8 ] = c[ 8 ];
}

float sh3::GetCoefficient( const int index ) const
{
	return( ( static_cast< unsigned int >( index ) < 9 ) ? c[ index ] : 0 );
}

float sh3::GetOrder1( void ) const
{
	return( c[ 0 ] );
}

float sh3::GetOrder2( void ) const
{
	return( c[ 0 ] + c[ 1 ] + c[ 2 ] + c[ 3 ] );
}

float sh3::GetOrder3( void ) const
{
	return( c[ 0 ] + c[ 1 ] + c[ 2 ] + c[ 3 ] + c[ 4 ] + c[ 5 ] + c[ 6 ] + c[ 7 ] + c[ 8 ] );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh3_rgb
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool sh3_rgb::IsNan( void ) const
{
	return( r.IsNan() || g.IsNan() || b.IsNan() );
}

bool sh3_rgb::IsReal( void ) const
{
	return( r.IsReal() && g.IsReal() && b.IsReal() );
}

void sh3_rgb::Madd( const sh3_rgb& src, const float mul )
{
	r.Madd( src.r, mul );
	g.Madd( src.g, mul );
	b.Madd( src.b, mul );
}

void sh3_rgb::Add( const sh3_rgb& add )
{
	r.Add( add.r );
	g.Add( add.g );
	b.Add( add.b );
}

void sh3_rgb::Sub( const sh3_rgb& sub )
{
	r.Sub( sub.r );
	g.Sub( sub.g );
	b.Sub( sub.b );
}

void sh3_rgb::Mul( const sh3_rgb& mul )
{
	r.Mul( mul.r );
	g.Mul( mul.g );
	b.Mul( mul.b );
}

void sh3_rgb::Mul( const sh3& mul )
{
	r.Mul( mul );
	g.Mul( mul );
	b.Mul( mul );
}

void sh3_rgb::Mul( const float mul )
{
	r.Mul( mul );
	g.Mul( mul );
	b.Mul( mul );
}

void sh3_rgb::Div( const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	float mul = ( 1.0f / div );
	r.Mul( mul );
	g.Mul( mul );
	b.Mul( mul );
}

void sh3_rgb::Lerp( const sh3_rgb& trg, const float t )
{
	float s = ( 1.0f - t );
	r.c[ 0 ] = ( ( r.c[ 0 ] * s ) + ( trg.r.c[ 0 ] * t ) );
	r.c[ 1 ] = ( ( r.c[ 1 ] * s ) + ( trg.r.c[ 1 ] * t ) );
	r.c[ 2 ] = ( ( r.c[ 2 ] * s ) + ( trg.r.c[ 2 ] * t ) );
	r.c[ 3 ] = ( ( r.c[ 3 ] * s ) + ( trg.r.c[ 3 ] * t ) );
	r.c[ 4 ] = ( ( r.c[ 4 ] * s ) + ( trg.r.c[ 4 ] * t ) );
	r.c[ 5 ] = ( ( r.c[ 5 ] * s ) + ( trg.r.c[ 5 ] * t ) );
	r.c[ 6 ] = ( ( r.c[ 6 ] * s ) + ( trg.r.c[ 6 ] * t ) );
	r.c[ 7 ] = ( ( r.c[ 7 ] * s ) + ( trg.r.c[ 7 ] * t ) );
	r.c[ 8 ] = ( ( r.c[ 8 ] * s ) + ( trg.r.c[ 8 ] * t ) );
	g.c[ 0 ] = ( ( g.c[ 0 ] * s ) + ( trg.g.c[ 0 ] * t ) );
	g.c[ 1 ] = ( ( g.c[ 1 ] * s ) + ( trg.g.c[ 1 ] * t ) );
	g.c[ 2 ] = ( ( g.c[ 2 ] * s ) + ( trg.g.c[ 2 ] * t ) );
	g.c[ 3 ] = ( ( g.c[ 3 ] * s ) + ( trg.g.c[ 3 ] * t ) );
	g.c[ 4 ] = ( ( g.c[ 4 ] * s ) + ( trg.g.c[ 4 ] * t ) );
	g.c[ 5 ] = ( ( g.c[ 5 ] * s ) + ( trg.g.c[ 5 ] * t ) );
	g.c[ 6 ] = ( ( g.c[ 6 ] * s ) + ( trg.g.c[ 6 ] * t ) );
	g.c[ 7 ] = ( ( g.c[ 7 ] * s ) + ( trg.g.c[ 7 ] * t ) );
	g.c[ 8 ] = ( ( g.c[ 8 ] * s ) + ( trg.g.c[ 8 ] * t ) );
	b.c[ 0 ] = ( ( b.c[ 0 ] * s ) + ( trg.b.c[ 0 ] * t ) );
	b.c[ 1 ] = ( ( b.c[ 1 ] * s ) + ( trg.b.c[ 1 ] * t ) );
	b.c[ 2 ] = ( ( b.c[ 2 ] * s ) + ( trg.b.c[ 2 ] * t ) );
	b.c[ 3 ] = ( ( b.c[ 3 ] * s ) + ( trg.b.c[ 3 ] * t ) );
	b.c[ 4 ] = ( ( b.c[ 4 ] * s ) + ( trg.b.c[ 4 ] * t ) );
	b.c[ 5 ] = ( ( b.c[ 5 ] * s ) + ( trg.b.c[ 5 ] * t ) );
	b.c[ 6 ] = ( ( b.c[ 6 ] * s ) + ( trg.b.c[ 6 ] * t ) );
	b.c[ 7 ] = ( ( b.c[ 7 ] * s ) + ( trg.b.c[ 7 ] * t ) );
	b.c[ 8 ] = ( ( b.c[ 8 ] * s ) + ( trg.b.c[ 8 ] * t ) );
}

void sh3_rgb::Negate( void )
{
	r.Negate();
	g.Negate();
	b.Negate();
}

void sh3_rgb::MulConsts( const vec3& cv )
{
	r.MulConsts( cv );
	g.MulConsts( cv );
	b.MulConsts( cv );
}

void sh3_rgb::MulNormalize( void )
{
	r.MulNormalize();
	g.MulNormalize();
	b.MulNormalize();
}

void sh3_rgb::MulSqrNormalize( void )
{
	r.MulSqrNormalize();
	g.MulSqrNormalize();
	b.MulSqrNormalize();
}

void sh3_rgb::SetMadd( const sh3_rgb& src, const float mul, const sh3_rgb& add )
{
	r.SetMadd( src.r, mul, add.r );
	g.SetMadd( src.g, mul, add.g );
	b.SetMadd( src.b, mul, add.b );
}

void sh3_rgb::SetAdd( const sh3_rgb& src, const sh3_rgb& add )
{
	r.SetAdd( src.r, add.r );
	g.SetAdd( src.g, add.g );
	b.SetAdd( src.b, add.b );
}

void sh3_rgb::SetSub( const sh3_rgb& src, const sh3_rgb& sub )
{
	r.SetSub( src.r, sub.r );
	g.SetSub( src.g, sub.g );
	b.SetSub( src.b, sub.b );
}

void sh3_rgb::SetMul( const sh3_rgb& src, const sh3_rgb& mul )
{
	r.SetMul( src.r, mul.r );
	g.SetMul( src.g, mul.g );
	b.SetMul( src.b, mul.b );
}

void sh3_rgb::SetMul( const sh3_rgb& src, const sh3& mul )
{
	r.SetMul( src.r, mul );
	g.SetMul( src.g, mul );
	b.SetMul( src.b, mul );
}

void sh3_rgb::SetMul( const sh3_rgb& src, const float mul )
{
	r.SetMul( src.r, mul );
	g.SetMul( src.g, mul );
	b.SetMul( src.b, mul );
}

void sh3_rgb::SetDiv( const sh3_rgb& src, const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	float mul = ( 1.0f / div );
	r.SetMul( src.r, mul );
	g.SetMul( src.g, mul );
	b.SetMul( src.b, mul );
}

void sh3_rgb::SetLerp( const sh3_rgb& src, const sh3_rgb& trg, const float t )
{
	float s = ( 1.0f - t );
	r.c[ 0 ] = ( ( src.r.c[ 0 ] * s ) + ( trg.r.c[ 0 ] * t ) );
	r.c[ 1 ] = ( ( src.r.c[ 1 ] * s ) + ( trg.r.c[ 1 ] * t ) );
	r.c[ 2 ] = ( ( src.r.c[ 2 ] * s ) + ( trg.r.c[ 2 ] * t ) );
	r.c[ 3 ] = ( ( src.r.c[ 3 ] * s ) + ( trg.r.c[ 3 ] * t ) );
	r.c[ 4 ] = ( ( src.r.c[ 4 ] * s ) + ( trg.r.c[ 4 ] * t ) );
	r.c[ 5 ] = ( ( src.r.c[ 5 ] * s ) + ( trg.r.c[ 5 ] * t ) );
	r.c[ 6 ] = ( ( src.r.c[ 6 ] * s ) + ( trg.r.c[ 6 ] * t ) );
	r.c[ 7 ] = ( ( src.r.c[ 7 ] * s ) + ( trg.r.c[ 7 ] * t ) );
	r.c[ 8 ] = ( ( src.r.c[ 8 ] * s ) + ( trg.r.c[ 8 ] * t ) );
	g.c[ 0 ] = ( ( src.g.c[ 0 ] * s ) + ( trg.g.c[ 0 ] * t ) );
	g.c[ 1 ] = ( ( src.g.c[ 1 ] * s ) + ( trg.g.c[ 1 ] * t ) );
	g.c[ 2 ] = ( ( src.g.c[ 2 ] * s ) + ( trg.g.c[ 2 ] * t ) );
	g.c[ 3 ] = ( ( src.g.c[ 3 ] * s ) + ( trg.g.c[ 3 ] * t ) );
	g.c[ 4 ] = ( ( src.g.c[ 4 ] * s ) + ( trg.g.c[ 4 ] * t ) );
	g.c[ 5 ] = ( ( src.g.c[ 5 ] * s ) + ( trg.g.c[ 5 ] * t ) );
	g.c[ 6 ] = ( ( src.g.c[ 6 ] * s ) + ( trg.g.c[ 6 ] * t ) );
	g.c[ 7 ] = ( ( src.g.c[ 7 ] * s ) + ( trg.g.c[ 7 ] * t ) );
	g.c[ 8 ] = ( ( src.g.c[ 8 ] * s ) + ( trg.g.c[ 8 ] * t ) );
	b.c[ 0 ] = ( ( src.b.c[ 0 ] * s ) + ( trg.b.c[ 0 ] * t ) );
	b.c[ 1 ] = ( ( src.b.c[ 1 ] * s ) + ( trg.b.c[ 1 ] * t ) );
	b.c[ 2 ] = ( ( src.b.c[ 2 ] * s ) + ( trg.b.c[ 2 ] * t ) );
	b.c[ 3 ] = ( ( src.b.c[ 3 ] * s ) + ( trg.b.c[ 3 ] * t ) );
	b.c[ 4 ] = ( ( src.b.c[ 4 ] * s ) + ( trg.b.c[ 4 ] * t ) );
	b.c[ 5 ] = ( ( src.b.c[ 5 ] * s ) + ( trg.b.c[ 5 ] * t ) );
	b.c[ 6 ] = ( ( src.b.c[ 6 ] * s ) + ( trg.b.c[ 6 ] * t ) );
	b.c[ 7 ] = ( ( src.b.c[ 7 ] * s ) + ( trg.b.c[ 7 ] * t ) );
	b.c[ 8 ] = ( ( src.b.c[ 8 ] * s ) + ( trg.b.c[ 8 ] * t ) );
}

void sh3_rgb::SetNegate( const sh3_rgb& src )
{
	r.SetNegate( src.r );
	g.SetNegate( src.g );
	b.SetNegate( src.b );
}

void sh3_rgb::SetConsts( const vec3& cv )
{
	r.c[ 0 ] = g.c[ 0 ] = b.c[ 0 ] = cv.x;
	r.c[ 1 ] = g.c[ 1 ] = b.c[ 1 ] = cv.y;
	r.c[ 2 ] = g.c[ 2 ] = b.c[ 2 ] = cv.y;
	r.c[ 3 ] = g.c[ 3 ] = b.c[ 3 ] = cv.y;
	r.c[ 4 ] = g.c[ 4 ] = b.c[ 4 ] = cv.z;
	r.c[ 5 ] = g.c[ 5 ] = b.c[ 5 ] = cv.z;
	r.c[ 6 ] = g.c[ 6 ] = b.c[ 6 ] = cv.z;
	r.c[ 7 ] = g.c[ 7 ] = b.c[ 7 ] = cv.z;
	r.c[ 8 ] = g.c[ 8 ] = b.c[ 8 ] = cv.z;
}

void sh3_rgb::SetNormalize( void )
{
	r.SetNormalize();
	g.SetNormalize();
	b.SetNormalize();
}

void sh3_rgb::SetSqrNormalize( void )
{
	r.SetSqrNormalize();
	g.SetSqrNormalize();
	b.SetSqrNormalize();
}

void sh3_rgb::SetNormalizedBasis( const vec3& v )
{
	r.c[ 0 ] = g.c[ 0 ] = b.c[ 0 ] = 1;
	r.c[ 1 ] = g.c[ 1 ] = b.c[ 1 ] = v.y;
	r.c[ 2 ] = g.c[ 2 ] = b.c[ 2 ] = v.z;
	r.c[ 3 ] = g.c[ 3 ] = b.c[ 3 ] = v.x;
	r.c[ 4 ] = g.c[ 4 ] = b.c[ 4 ] = ( v.x * v.y * consts::SH::root3 );
	r.c[ 5 ] = g.c[ 5 ] = b.c[ 5 ] = ( v.y * v.z * consts::SH::root3 );
	r.c[ 6 ] = g.c[ 6 ] = b.c[ 6 ] = ( ( v.z * v.z * 1.5f ) - 0.5f );
	r.c[ 7 ] = g.c[ 7 ] = b.c[ 7 ] = ( v.z * v.x * consts::SH::root3 );
	r.c[ 8 ] = g.c[ 8 ] = b.c[ 8 ] = ( ( ( v.x * v.x ) - ( v.y * v.y ) ) * consts::SH::root3d2 );
}

void sh3_rgb::SetUnitBasis( const vec3& v )
{
	r.c[ 0 ] = g.c[ 0 ] = b.c[ 0 ] = 1;
	r.c[ 1 ] = g.c[ 1 ] = b.c[ 1 ] = v.y;
	r.c[ 2 ] = g.c[ 2 ] = b.c[ 2 ] = v.z;
	r.c[ 3 ] = g.c[ 3 ] = b.c[ 3 ] = v.x;
	r.c[ 4 ] = g.c[ 4 ] = b.c[ 4 ] = ( v.x * v.y * 2 );
	r.c[ 5 ] = g.c[ 5 ] = b.c[ 5 ] = ( v.y * v.z * 2 );
	r.c[ 6 ] = g.c[ 6 ] = b.c[ 6 ] = ( ( v.z * v.z * 1.5f ) - 0.5f );
	r.c[ 7 ] = g.c[ 7 ] = b.c[ 7 ] = ( v.z * v.x * 2 );
	r.c[ 8 ] = g.c[ 8 ] = b.c[ 8 ] = ( ( v.x * v.x ) - ( v.y * v.y ) );
}

void sh3_rgb::SetBasis( const vec3& v )
{
	r.c[ 0 ] = g.c[ 0 ] = b.c[ 0 ] = 1;
	r.c[ 1 ] = g.c[ 1 ] = b.c[ 1 ] = v.y;
	r.c[ 2 ] = g.c[ 2 ] = b.c[ 2 ] = v.z;
	r.c[ 3 ] = g.c[ 3 ] = b.c[ 3 ] = v.x;
	r.c[ 4 ] = g.c[ 4 ] = b.c[ 4 ] = ( v.x * v.y );
	r.c[ 5 ] = g.c[ 5 ] = b.c[ 5 ] = ( v.y * v.z );
	r.c[ 6 ] = g.c[ 6 ] = b.c[ 6 ] = ( ( v.z * v.z ) - consts::SH::rcp3 );
	r.c[ 7 ] = g.c[ 7 ] = b.c[ 7 ] = ( v.z * v.x );
	r.c[ 8 ] = g.c[ 8 ] = b.c[ 8 ] = ( ( v.x * v.x ) - ( v.y * v.y ) );
}

void sh3_rgb::Set( const sh3& src )
{
	r.Set( src );
	g.Set( src );
	b.Set( src );
}

void sh3_rgb::Set( const sh3_rgb& src )
{
	r.Set( src.r );
	g.Set( src.g );
	b.Set( src.b );
}

void sh3_rgb::Get( sh3_rgb& trg ) const
{
	r.Get( trg.r );
	g.Get( trg.g );
	b.Get( trg.b );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sh3_rgb_packed
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sh3_rgb_packed::Set( const sh3_rgb& src )
{
	ac.x = src.r.c[ 0 ];
	ac.y = src.g.c[ 0 ];
	ac.z = src.b.c[ 0 ];
	ac.w = consts::SH::rcp3;
	r1.x = src.r.c[ 2 ];
	r1.y = src.r.c[ 3 ];
	r1.z = src.r.c[ 1 ];
	r1.w = src.r.c[ 8 ];
	r2.x = src.r.c[ 4 ];
	r2.y = src.r.c[ 5 ];
	r2.z = src.r.c[ 7 ];
	r2.w = src.r.c[ 6 ];
	g1.x = src.g.c[ 2 ];
	g1.y = src.g.c[ 3 ];
	g1.z = src.g.c[ 1 ];
	g1.w = src.g.c[ 8 ];
	g2.x = src.g.c[ 4 ];
	g2.y = src.g.c[ 5 ];
	g2.z = src.g.c[ 7 ];
	g2.w = src.g.c[ 6 ];
	b1.x = src.b.c[ 2 ];
	b1.y = src.b.c[ 3 ];
	b1.z = src.b.c[ 1 ];
	b1.w = src.b.c[ 8 ];
	b2.x = src.b.c[ 4 ];
	b2.y = src.b.c[ 5 ];
	b2.z = src.b.c[ 7 ];
	b2.w = src.b.c[ 6 ];
}

void sh3_rgb_packed::Eval( const vec3& v, vec3& colour ) const
{
	vec4 v1, v2;
	v1.x = v.x;
	v1.y = v.y;
	v1.z = v.z;
	v1.w = ( ( v.x * v.x ) - ( v.y * v.y ) );
	v2.x = ( v.x * v.y );
	v2.x = ( v.y * v.z );
	v2.x = ( v.z * v.x );
	v2.x = ( ( v.z * v.z ) - ac.w );
	colour.x = ( ac.x + v1.Dot( r1 ) + v2.Dot( r2 ) );
	colour.y = ( ac.y + v1.Dot( g1 ) + v2.Dot( g2 ) );
	colour.z = ( ac.z + v1.Dot( b1 ) + v2.Dot( b2 ) );
}

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
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

