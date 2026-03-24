
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   mat44.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	4 by 4 element matrix container and functions.
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

#include "aabb.h"
#include "vec2.h"
#include "plane.h"
#include "mat33.h"
#include "mat43.h"
#include "mat44.h"
#include "box_verts.h"
#include "box_faces.h"
#include "consts.h"
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
	namespace MAT44
	{
		const mat44		IDENTITY = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

//  open-uniform cubic b-spline (cubic Bezier}
//  A, B, C, D : produces 4 weights for 4 control points, passes through points A and D
		const mat44		BEZIER = {
							{ -1.0f,  3.0f, -3.0f,  1.0f },
							{  3.0f, -6.0f,  3.0f,  0.0f },
							{ -3.0f,  3.0f,  0.0f,  0.0f },
							{  1.0f,  0.0f,  0.0f,  0.0f } };

//  uniform cubic b-spline (cubic b-spline}
//  A, B, C, D : produces 4 weights for 4 control points, does not pass through any control points
		const mat44		B_SPLINE = {
							{ -( 1.0f / 6.0f ),  ( 3.0f / 6.0f ), -( 3.0f / 6.0f ),  ( 1.0f / 6.0f ) },
							{  ( 3.0f / 6.0f ), -( 6.0f / 6.0f ),  ( 3.0f / 6.0f ),  ( 0.0f / 6.0f ) },
							{ -( 3.0f / 6.0f ),  ( 0.0f / 6.0f ),  ( 3.0f / 6.0f ),  ( 0.0f / 6.0f ) },
							{  ( 1.0f / 6.0f ),  ( 4.0f / 6.0f ),  ( 1.0f / 6.0f ),  ( 0.0f / 6.0f ) } };

//	Catmull-Rom spline with tension = 0.5f
//  A, B, C, D : produces 4 weights for 4 control points, passes through points B and C
		const mat44		CATMULL_ROM = {
							{ -0.5f,  1.5f, -1.5f,  0.5f },
							{  1.0f, -2.5f,  2.0f, -0.5f },
							{ -0.5f,  0.0f,  0.5f,  0.0f },
							{  0.0f,  1.0f,  0.0f,  0.0f } };

//  Hermite spline
//  U, A, B, V : produces 4 weights for 2 control points and 2 control tangents (both pointing into the span), passes through points A and B
		const mat44		HERMITE = {
							{   1.0f,   2.0f,  -2.0f,  -1.0f },
							{  -2.0f,  -3.0f,   3.0f,   1.0f },
							{   1.0f,   0.0f,   0.0f,   0.0f },
							{   0.0f,   1.0f,   0.0f,   0.0f } };

//  Hermite Bezier spline (Hermite but tangents scaled to match Bezier)
//  U, A, B, V : produces 4 weights for 2 control points and 2 control tangents (both pointing into the span), passes through points A and B
		const mat44		HBEZIER =  {
							{   3.0f,   2.0f,  -2.0f,  -3.0f },
							{  -6.0f,  -3.0f,   3.0f,   3.0f },
							{   3.0f,   0.0f,   0.0f,   0.0f },
							{   0.0f,   1.0f,   0.0f,   0.0f } };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    mat44
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool mat44::IsNan( void ) const
{
	return( x.IsNan() || y.IsNan() || z.IsNan() || w.IsNan() );
}

bool mat44::IsReal( void ) const
{
	return( x.IsReal() && y.IsReal() && z.IsReal() && w.IsReal() );
}

float mat44::Det( void ) const
{
	return( 
		( ( ( x.x * y.y ) - ( x.y * y.x ) ) * ( ( z.z * w.w ) - ( z.w * w.z ) ) ) +
		( ( ( x.x * y.z ) - ( x.z * y.x ) ) * ( ( z.w * w.y ) - ( z.y * w.w ) ) ) +
		( ( ( x.x * y.w ) - ( x.w * y.x ) ) * ( ( z.y * w.z ) - ( z.z * w.y ) ) ) +
		( ( ( x.y * y.z ) - ( x.z * y.y ) ) * ( ( z.x * w.w ) - ( z.w * w.x ) ) ) +
		( ( ( x.y * y.w ) - ( x.w * y.y ) ) * ( ( z.z * w.x ) - ( z.x * w.z ) ) ) +
		( ( ( x.z * y.w ) - ( x.w * y.z ) ) * ( ( z.x * w.y ) - ( z.y * w.x ) ) ) );
}

void mat44::Mul( const mat44& mul )
{
	mat44 tmp;
	tmp.SetMul( *this, mul );
	Set( tmp );
}

void mat44::Mul( const mat43& mul )
{
	mat44 tmp;
	tmp.SetMul( *this, mul );
	Set( tmp );
}

void mat44::Mul( const mat33& mul )
{
	mat44 tmp;
	tmp.SetMul( *this, mul );
	Set( tmp );
}

bool mat44::Invert( void )
{
	vec4 a, b, c;
	c.x = ( ( x.x * y.y ) - ( x.y * y.x ) );
	a.x = ( ( x.x * y.z ) - ( x.z * y.x ) );
	a.y = ( ( x.x * y.w ) - ( x.w * y.x ) );
	a.z = ( ( x.y * y.z ) - ( x.z * y.y ) );
	a.w = ( ( x.y * y.w ) - ( x.w * y.y ) );
	c.y = ( ( x.z * y.w ) - ( x.w * y.z ) );
	c.z = ( ( z.x * w.y ) - ( z.y * w.x ) );
	b.x = ( ( z.x * w.z ) - ( z.z * w.x ) );
	b.y = ( ( z.x * w.w ) - ( z.w * w.x ) );
	b.z = ( ( z.y * w.z ) - ( z.z * w.y ) );
	b.w = ( ( z.y * w.w ) - ( z.w * w.y ) );
	c.w = ( ( z.z * w.w ) - ( z.w * w.z ) );
	float d = ( ( c.x * c.w ) - ( a.x * b.w ) + ( a.y * b.z ) + ( a.z * b.y ) - ( a.w * b.x ) + ( c.y * c.z ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		d = ( 1.0f / d );
		a.x *= d;
		a.y *= d;
		a.z *= d;
		a.w *= d;
		b.x *= d;
		b.y *= d;
		b.z *= d;
		b.w *= d;
		c.x *= d;
		c.y *= d;
		c.z *= d;
		c.w *= d;
		mat44 tmp;
		tmp.x.x = ( ( y.w * b.z ) - ( y.z * b.w ) + ( y.y * c.w ) );
		tmp.y.x = ( ( y.z * b.y ) - ( y.w * b.x ) - ( y.x * c.w ) );
		tmp.z.x = ( ( y.x * b.w ) - ( y.y * b.y ) + ( y.w * c.z ) );
		tmp.w.x = ( ( y.y * b.x ) - ( y.x * b.z ) - ( y.z * c.z ) );
		tmp.x.y = ( ( x.z * b.w ) - ( x.w * b.z ) - ( x.y * c.w ) );
		tmp.y.y = ( ( x.w * b.x ) - ( x.z * b.y ) + ( x.x * c.w ) );
		tmp.z.y = ( ( x.y * b.y ) - ( x.x * b.w ) - ( x.w * c.z ) );
		tmp.w.y = ( ( x.x * b.z ) - ( x.y * b.x ) + ( x.z * c.z ) );
		tmp.x.z = ( ( w.w * a.z ) - ( w.z * a.w ) + ( w.y * c.y ) );
		tmp.y.z = ( ( w.z * a.y ) - ( w.w * a.x ) - ( w.x * c.y ) );
		tmp.z.z = ( ( w.x * a.w ) - ( w.y * a.y ) + ( w.w * c.x ) );
		tmp.w.z = ( ( w.y * a.x ) - ( w.x * a.z ) - ( w.z * c.x ) );
		tmp.x.w = ( ( z.z * a.w ) - ( z.w * a.z ) - ( z.y * c.y ) );
		tmp.y.w = ( ( z.w * a.x ) - ( z.z * a.y ) + ( z.x * c.y ) );
		tmp.z.w = ( ( z.y * a.y ) - ( z.x * a.w ) - ( z.w * c.x ) );
		tmp.w.w = ( ( z.x * a.z ) - ( z.y * a.x ) + ( z.z * c.x ) );
		Set( tmp );
		return( true );
	}
	else
	{
		SetIdentity();
		return( false );
	}
}

void mat44::Transpose( void )
{
	float t;
	t = x.y;
	x.y = y.x;
	y.x = t;
	t = x.z;
	x.z = z.x;
	z.x = t;
	t = x.w;
	x.w = w.x;
	w.x = t;
	t = y.z;
	y.z = z.y;
	z.y = t;
	t = y.w;
	y.w = w.y;
	w.y = t;
	t = z.w;
	z.w = w.z;
	w.z = t;
}

bool mat44::InvertTranspose( void )
{
	vec4 a, b, c;
	c.x = ( ( x.x * y.y ) - ( x.y * y.x ) );
	a.x = ( ( x.x * y.z ) - ( x.z * y.x ) );
	a.y = ( ( x.x * y.w ) - ( x.w * y.x ) );
	a.z = ( ( x.y * y.z ) - ( x.z * y.y ) );
	a.w = ( ( x.y * y.w ) - ( x.w * y.y ) );
	c.y = ( ( x.z * y.w ) - ( x.w * y.z ) );
	c.z = ( ( z.x * w.y ) - ( z.y * w.x ) );
	b.x = ( ( z.x * w.z ) - ( z.z * w.x ) );
	b.y = ( ( z.x * w.w ) - ( z.w * w.x ) );
	b.z = ( ( z.y * w.z ) - ( z.z * w.y ) );
	b.w = ( ( z.y * w.w ) - ( z.w * w.y ) );
	c.w = ( ( z.z * w.w ) - ( z.w * w.z ) );
	float d = ( ( c.x * c.w ) - ( a.x * b.w ) + ( a.y * b.z ) + ( a.z * b.y ) - ( a.w * b.x ) + ( c.y * c.z ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		d = ( 1.0f / d );
		a.x *= d;
		a.y *= d;
		a.z *= d;
		a.w *= d;
		b.x *= d;
		b.y *= d;
		b.z *= d;
		b.w *= d;
		c.x *= d;
		c.y *= d;
		c.z *= d;
		c.w *= d;
		mat44 tmp;
		tmp.x.x = ( ( y.w * b.z ) - ( y.z * b.w ) + ( y.y * c.w ) );
		tmp.x.y = ( ( y.z * b.y ) - ( y.w * b.x ) - ( y.x * c.w ) );
		tmp.x.z = ( ( y.x * b.w ) - ( y.y * b.y ) + ( y.w * c.z ) );
		tmp.x.w = ( ( y.y * b.x ) - ( y.x * b.z ) - ( y.z * c.z ) );
		tmp.y.x = ( ( x.z * b.w ) - ( x.w * b.z ) - ( x.y * c.w ) );
		tmp.y.y = ( ( x.w * b.x ) - ( x.z * b.y ) + ( x.x * c.w ) );
		tmp.y.z = ( ( x.y * b.y ) - ( x.x * b.w ) - ( x.w * c.z ) );
		tmp.y.w = ( ( x.x * b.z ) - ( x.y * b.x ) + ( x.z * c.z ) );
		tmp.z.x = ( ( w.w * a.z ) - ( w.z * a.w ) + ( w.y * c.y ) );
		tmp.z.y = ( ( w.z * a.y ) - ( w.w * a.x ) - ( w.x * c.y ) );
		tmp.z.z = ( ( w.x * a.w ) - ( w.y * a.y ) + ( w.w * c.x ) );
		tmp.z.w = ( ( w.y * a.x ) - ( w.x * a.z ) - ( w.z * c.x ) );
		tmp.w.x = ( ( z.z * a.w ) - ( z.w * a.z ) - ( z.y * c.y ) );
		tmp.w.y = ( ( z.w * a.x ) - ( z.z * a.y ) + ( z.x * c.y ) );
		tmp.w.z = ( ( z.y * a.y ) - ( z.x * a.w ) - ( z.w * c.x ) );
		tmp.w.w = ( ( z.x * a.z ) - ( z.y * a.x ) + ( z.z * c.x ) );
		Set( tmp );
		return( true );
	}
	else
	{
		SetIdentity();
		return( false );
	}
}

void mat44::PartialInvert( void )
{
	mat44 tmp;
	tmp.SetPartialInverse( *this );
	Set( tmp );
}

void mat44::PartialInvertTranspose( void )
{
	mat44 tmp;
	tmp.SetPartialInverseTranspose( *this );
	Set( tmp );
}

void mat44::Reflect( const plane& about )
{
	float s = -( 2.0f / ( ( about.x * about.x ) + ( about.y * about.y ) + ( about.z * about.z ) ) );
	{
		float r = ( ( ( x.x * about.x ) + ( y.x * about.y ) + ( z.x * about.z ) + ( w.x * about.w ) ) * s );
		x.x += ( about.x * r );
		y.x += ( about.y * r );
		z.x += ( about.z * r );
	}
	{
		float r = ( ( ( x.y * about.x ) + ( y.y * about.y ) + ( z.y * about.z ) + ( w.y * about.w ) ) * s );
		x.y += ( about.x * r );
		y.y += ( about.y * r );
		z.y += ( about.z * r );
	}
	{
		float r = ( ( ( x.z * about.x ) + ( y.z * about.y ) + ( z.z * about.z ) + ( w.z * about.w ) ) * s );
		x.z += ( about.x * r );
		y.z += ( about.y * r );
		z.z += ( about.z * r );
	}
	{
		float r = ( ( ( x.w * about.x ) + ( y.w * about.y ) + ( z.w * about.z ) + ( w.w * about.w ) ) * s );
		x.w += ( about.x * r );
		y.w += ( about.y * r );
		z.w += ( about.z * r );
	}
}

void mat44::SetMul( const mat44& src, const mat44& mul )
{
	mat44 tmp;
	tmp.x.x = ( ( src.x.x * mul.x.x ) + ( src.y.x * mul.x.y ) + ( src.z.x * mul.x.z ) + ( src.w.x * mul.x.w ) );
	tmp.x.y = ( ( src.x.y * mul.x.x ) + ( src.y.y * mul.x.y ) + ( src.z.y * mul.x.z ) + ( src.w.y * mul.x.w ) );
	tmp.x.z = ( ( src.x.z * mul.x.x ) + ( src.y.z * mul.x.y ) + ( src.z.z * mul.x.z ) + ( src.w.z * mul.x.w ) );
	tmp.x.w = ( ( src.x.w * mul.x.x ) + ( src.y.w * mul.x.y ) + ( src.z.w * mul.x.z ) + ( src.w.w * mul.x.w ) );
	tmp.y.x = ( ( src.x.x * mul.y.x ) + ( src.y.x * mul.y.y ) + ( src.z.x * mul.y.z ) + ( src.w.x * mul.y.w ) );
	tmp.y.y = ( ( src.x.y * mul.y.x ) + ( src.y.y * mul.y.y ) + ( src.z.y * mul.y.z ) + ( src.w.y * mul.y.w ) );
	tmp.y.z = ( ( src.x.z * mul.y.x ) + ( src.y.z * mul.y.y ) + ( src.z.z * mul.y.z ) + ( src.w.z * mul.y.w ) );
	tmp.y.w = ( ( src.x.w * mul.y.x ) + ( src.y.w * mul.y.y ) + ( src.z.w * mul.y.z ) + ( src.w.w * mul.y.w ) );
	tmp.z.x = ( ( src.x.x * mul.z.x ) + ( src.y.x * mul.z.y ) + ( src.z.x * mul.z.z ) + ( src.w.x * mul.z.w ) );
	tmp.z.y = ( ( src.x.y * mul.z.x ) + ( src.y.y * mul.z.y ) + ( src.z.y * mul.z.z ) + ( src.w.y * mul.z.w ) );
	tmp.z.z = ( ( src.x.z * mul.z.x ) + ( src.y.z * mul.z.y ) + ( src.z.z * mul.z.z ) + ( src.w.z * mul.z.w ) );
	tmp.z.w = ( ( src.x.w * mul.z.x ) + ( src.y.w * mul.z.y ) + ( src.z.w * mul.z.z ) + ( src.w.w * mul.z.w ) );
	tmp.w.x = ( ( src.x.x * mul.w.x ) + ( src.y.x * mul.w.y ) + ( src.z.x * mul.w.z ) + ( src.w.x * mul.w.w ) );
	tmp.w.y = ( ( src.x.y * mul.w.x ) + ( src.y.y * mul.w.y ) + ( src.z.y * mul.w.z ) + ( src.w.y * mul.w.w ) );
	tmp.w.z = ( ( src.x.z * mul.w.x ) + ( src.y.z * mul.w.y ) + ( src.z.z * mul.w.z ) + ( src.w.z * mul.w.w ) );
	tmp.w.w = ( ( src.x.w * mul.w.x ) + ( src.y.w * mul.w.y ) + ( src.z.w * mul.w.z ) + ( src.w.w * mul.w.w ) );
	Set( tmp );
}

void mat44::SetMul( const mat44& src, const mat43& mul )
{
	mat44 tmp;
	tmp.x.x = ( ( src.x.x * mul.x.x ) + ( src.y.x * mul.x.y ) + ( src.z.x * mul.x.z ) + ( src.w.x * mul.x.w ) );
	tmp.x.y = ( ( src.x.y * mul.x.x ) + ( src.y.y * mul.x.y ) + ( src.z.y * mul.x.z ) + ( src.w.y * mul.x.w ) );
	tmp.x.z = ( ( src.x.z * mul.x.x ) + ( src.y.z * mul.x.y ) + ( src.z.z * mul.x.z ) + ( src.w.z * mul.x.w ) );
	tmp.x.w = ( ( src.x.w * mul.x.x ) + ( src.y.w * mul.x.y ) + ( src.z.w * mul.x.z ) + ( src.w.w * mul.x.w ) );
	tmp.y.x = ( ( src.x.x * mul.y.x ) + ( src.y.x * mul.y.y ) + ( src.z.x * mul.y.z ) + ( src.w.x * mul.y.w ) );
	tmp.y.y = ( ( src.x.y * mul.y.x ) + ( src.y.y * mul.y.y ) + ( src.z.y * mul.y.z ) + ( src.w.y * mul.y.w ) );
	tmp.y.z = ( ( src.x.z * mul.y.x ) + ( src.y.z * mul.y.y ) + ( src.z.z * mul.y.z ) + ( src.w.z * mul.y.w ) );
	tmp.y.w = ( ( src.x.w * mul.y.x ) + ( src.y.w * mul.y.y ) + ( src.z.w * mul.y.z ) + ( src.w.w * mul.y.w ) );
	tmp.z.x = ( ( src.x.x * mul.z.x ) + ( src.y.x * mul.z.y ) + ( src.z.x * mul.z.z ) + ( src.w.x * mul.z.w ) );
	tmp.z.y = ( ( src.x.y * mul.z.x ) + ( src.y.y * mul.z.y ) + ( src.z.y * mul.z.z ) + ( src.w.y * mul.z.w ) );
	tmp.z.z = ( ( src.x.z * mul.z.x ) + ( src.y.z * mul.z.y ) + ( src.z.z * mul.z.z ) + ( src.w.z * mul.z.w ) );
	tmp.z.w = ( ( src.x.w * mul.z.x ) + ( src.y.w * mul.z.y ) + ( src.z.w * mul.z.z ) + ( src.w.w * mul.z.w ) );
	tmp.w.x = src.w.x;
	tmp.w.y = src.w.y;
	tmp.w.z = src.w.z;
	tmp.w.w = src.w.w;
	Set( tmp );
}

void mat44::SetMul( const mat44& src, const mat33& mul )
{
	mat44 tmp;
	tmp.x.x = ( ( src.x.x * mul.x.x ) + ( src.y.x * mul.x.y ) + ( src.z.x * mul.x.z ) );
	tmp.x.y = ( ( src.x.y * mul.x.x ) + ( src.y.y * mul.x.y ) + ( src.z.y * mul.x.z ) );
	tmp.x.z = ( ( src.x.z * mul.x.x ) + ( src.y.z * mul.x.y ) + ( src.z.z * mul.x.z ) );
	tmp.x.w = ( ( src.x.w * mul.x.x ) + ( src.y.w * mul.x.y ) + ( src.z.w * mul.x.z ) );
	tmp.y.x = ( ( src.x.x * mul.y.x ) + ( src.y.x * mul.y.y ) + ( src.z.x * mul.y.z ) );
	tmp.y.y = ( ( src.x.y * mul.y.x ) + ( src.y.y * mul.y.y ) + ( src.z.y * mul.y.z ) );
	tmp.y.z = ( ( src.x.z * mul.y.x ) + ( src.y.z * mul.y.y ) + ( src.z.z * mul.y.z ) );
	tmp.y.w = ( ( src.x.w * mul.y.x ) + ( src.y.w * mul.y.y ) + ( src.z.w * mul.y.z ) );
	tmp.z.x = ( ( src.x.x * mul.z.x ) + ( src.y.x * mul.z.y ) + ( src.z.x * mul.z.z ) );
	tmp.z.y = ( ( src.x.y * mul.z.x ) + ( src.y.y * mul.z.y ) + ( src.z.y * mul.z.z ) );
	tmp.z.z = ( ( src.x.z * mul.z.x ) + ( src.y.z * mul.z.y ) + ( src.z.z * mul.z.z ) );
	tmp.z.w = ( ( src.x.w * mul.z.x ) + ( src.y.w * mul.z.y ) + ( src.z.w * mul.z.z ) );
	tmp.w.x = src.w.x;
	tmp.w.y = src.w.y;
	tmp.w.z = src.w.z;
	tmp.w.w = src.w.w;
	Set( tmp );
}

void mat44::SetMul( const mat43& src, const mat44& mul )
{
	mat44 tmp;
	tmp.x.x = ( ( src.x.x * mul.x.x ) + ( src.y.x * mul.x.y ) + ( src.z.x * mul.x.z ) );
	tmp.x.y = ( ( src.x.y * mul.x.x ) + ( src.y.y * mul.x.y ) + ( src.z.y * mul.x.z ) );
	tmp.x.z = ( ( src.x.z * mul.x.x ) + ( src.y.z * mul.x.y ) + ( src.z.z * mul.x.z ) );
	tmp.x.w = ( ( src.x.w * mul.x.x ) + ( src.y.w * mul.x.y ) + ( src.z.w * mul.x.z ) + mul.x.w );
	tmp.y.x = ( ( src.x.x * mul.y.x ) + ( src.y.x * mul.y.y ) + ( src.z.x * mul.y.z ) );
	tmp.y.y = ( ( src.x.y * mul.y.x ) + ( src.y.y * mul.y.y ) + ( src.z.y * mul.y.z ) );
	tmp.y.z = ( ( src.x.z * mul.y.x ) + ( src.y.z * mul.y.y ) + ( src.z.z * mul.y.z ) );
	tmp.y.w = ( ( src.x.w * mul.y.x ) + ( src.y.w * mul.y.y ) + ( src.z.w * mul.y.z ) + mul.y.w );
	tmp.z.x = ( ( src.x.x * mul.z.x ) + ( src.y.x * mul.z.y ) + ( src.z.x * mul.z.z ) );
	tmp.z.y = ( ( src.x.y * mul.z.x ) + ( src.y.y * mul.z.y ) + ( src.z.y * mul.z.z ) );
	tmp.z.z = ( ( src.x.z * mul.z.x ) + ( src.y.z * mul.z.y ) + ( src.z.z * mul.z.z ) );
	tmp.z.w = ( ( src.x.w * mul.z.x ) + ( src.y.w * mul.z.y ) + ( src.z.w * mul.z.z ) + mul.z.w );
	tmp.w.x = ( ( src.x.x * mul.w.x ) + ( src.y.x * mul.w.y ) + ( src.z.x * mul.w.z ) );
	tmp.w.y = ( ( src.x.y * mul.w.x ) + ( src.y.y * mul.w.y ) + ( src.z.y * mul.w.z ) );
	tmp.w.z = ( ( src.x.z * mul.w.x ) + ( src.y.z * mul.w.y ) + ( src.z.z * mul.w.z ) );
	tmp.w.w = ( ( src.x.w * mul.w.x ) + ( src.y.w * mul.w.y ) + ( src.z.w * mul.w.z ) + mul.w.w );
	Set( tmp );
}

void mat44::SetMul( const mat33& src, const mat44& mul )
{
	mat44 tmp;
	tmp.x.x = ( ( src.x.x * mul.x.x ) + ( src.y.x * mul.x.y ) + ( src.z.x * mul.x.z ) );
	tmp.x.y = ( ( src.x.y * mul.x.x ) + ( src.y.y * mul.x.y ) + ( src.z.y * mul.x.z ) );
	tmp.x.z = ( ( src.x.z * mul.x.x ) + ( src.y.z * mul.x.y ) + ( src.z.z * mul.x.z ) );
	tmp.x.w = mul.x.w;
	tmp.y.x = ( ( src.x.x * mul.y.x ) + ( src.y.x * mul.y.y ) + ( src.z.x * mul.y.z ) );
	tmp.y.y = ( ( src.x.y * mul.y.x ) + ( src.y.y * mul.y.y ) + ( src.z.y * mul.y.z ) );
	tmp.y.z = ( ( src.x.z * mul.y.x ) + ( src.y.z * mul.y.y ) + ( src.z.z * mul.y.z ) );
	tmp.y.w = mul.y.w;
	tmp.z.x = ( ( src.x.x * mul.z.x ) + ( src.y.x * mul.z.y ) + ( src.z.x * mul.z.z ) );
	tmp.z.y = ( ( src.x.y * mul.z.x ) + ( src.y.y * mul.z.y ) + ( src.z.y * mul.z.z ) );
	tmp.z.z = ( ( src.x.z * mul.z.x ) + ( src.y.z * mul.z.y ) + ( src.z.z * mul.z.z ) );
	tmp.z.w = mul.z.w;
	tmp.w.x = ( ( src.x.x * mul.w.x ) + ( src.y.x * mul.w.y ) + ( src.z.x * mul.w.z ) );
	tmp.w.y = ( ( src.x.y * mul.w.x ) + ( src.y.y * mul.w.y ) + ( src.z.y * mul.w.z ) );
	tmp.w.z = ( ( src.x.z * mul.w.x ) + ( src.y.z * mul.w.y ) + ( src.z.z * mul.w.z ) );
	tmp.w.w = mul.w.w;
	Set( tmp );
}

void mat44::SetRemap( const aabb& src, const aabb& trg )
{
	t_mat43().SetRemap( src, trg );
	w.x = w.y = w.z = 0.0f;
	w.w = 1.0f;
}

void mat44::SetDelta( const mat44& src, const mat44& trg )
{
	SetInverse( src );
	Mul( trg );
}

bool mat44::SetInverse( const mat44& src )
{
	vec4 a, b, c;
	c.x = ( ( src.x.x * src.y.y ) - ( src.x.y * src.y.x ) );
	a.x = ( ( src.x.x * src.y.z ) - ( src.x.z * src.y.x ) );
	a.y = ( ( src.x.x * src.y.w ) - ( src.x.w * src.y.x ) );
	a.z = ( ( src.x.y * src.y.z ) - ( src.x.z * src.y.y ) );
	a.w = ( ( src.x.y * src.y.w ) - ( src.x.w * src.y.y ) );
	c.y = ( ( src.x.z * src.y.w ) - ( src.x.w * src.y.z ) );
	c.z = ( ( src.z.x * src.w.y ) - ( src.z.y * src.w.x ) );
	b.x = ( ( src.z.x * src.w.z ) - ( src.z.z * src.w.x ) );
	b.y = ( ( src.z.x * src.w.w ) - ( src.z.w * src.w.x ) );
	b.z = ( ( src.z.y * src.w.z ) - ( src.z.z * src.w.y ) );
	b.w = ( ( src.z.y * src.w.w ) - ( src.z.w * src.w.y ) );
	c.w = ( ( src.z.z * src.w.w ) - ( src.z.w * src.w.z ) );
	float d = ( ( c.x * c.w ) - ( a.x * b.w ) + ( a.y * b.z ) + ( a.z * b.y ) - ( a.w * b.x ) + ( c.y * c.z ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		mat44 tmp;
		d = ( 1.0f / d );
		a.x *= d;
		a.y *= d;
		a.z *= d;
		a.w *= d;
		b.x *= d;
		b.y *= d;
		b.z *= d;
		b.w *= d;
		c.x *= d;
		c.y *= d;
		c.z *= d;
		c.w *= d;
		tmp.x.x = ( ( src.y.w * b.z ) - ( src.y.z * b.w ) + ( src.y.y * c.w ) );
		tmp.y.x = ( ( src.y.z * b.y ) - ( src.y.w * b.x ) - ( src.y.x * c.w ) );
		tmp.z.x = ( ( src.y.x * b.w ) - ( src.y.y * b.y ) + ( src.y.w * c.z ) );
		tmp.w.x = ( ( src.y.y * b.x ) - ( src.y.x * b.z ) - ( src.y.z * c.z ) );
		tmp.x.y = ( ( src.x.z * b.w ) - ( src.x.w * b.z ) - ( src.x.y * c.w ) );
		tmp.y.y = ( ( src.x.w * b.x ) - ( src.x.z * b.y ) + ( src.x.x * c.w ) );
		tmp.z.y = ( ( src.x.y * b.y ) - ( src.x.x * b.w ) - ( src.x.w * c.z ) );
		tmp.w.y = ( ( src.x.x * b.z ) - ( src.x.y * b.x ) + ( src.x.z * c.z ) );
		tmp.x.z = ( ( src.w.w * a.z ) - ( src.w.z * a.w ) + ( src.w.y * c.y ) );
		tmp.y.z = ( ( src.w.z * a.y ) - ( src.w.w * a.x ) - ( src.w.x * c.y ) );
		tmp.z.z = ( ( src.w.x * a.w ) - ( src.w.y * a.y ) + ( src.w.w * c.x ) );
		tmp.w.z = ( ( src.w.y * a.x ) - ( src.w.x * a.z ) - ( src.w.z * c.x ) );
		tmp.x.w = ( ( src.z.z * a.w ) - ( src.z.w * a.z ) - ( src.z.y * c.y ) );
		tmp.y.w = ( ( src.z.w * a.x ) - ( src.z.z * a.y ) + ( src.z.x * c.y ) );
		tmp.z.w = ( ( src.z.y * a.y ) - ( src.z.x * a.w ) - ( src.z.w * c.x ) );
		tmp.w.w = ( ( src.z.x * a.z ) - ( src.z.y * a.x ) + ( src.z.z * c.x ) );
		Set( tmp );
		return( true );
	}
	else
	{
		SetIdentity();
		return( false );
	}
}

void mat44::SetTranspose( const mat43& src )
{
	float t;
	t = src.x.y;
	x.y = src.y.x;
	y.x = t;
	t = src.x.z;
	x.z = src.z.x;
	z.x = t;
	t = src.y.z;
	y.z = src.z.y;
	z.y = t;
	x.x = src.x.x;
	y.y = src.y.y;
	z.z = src.z.z;
	w.x = src.x.w;
	w.y = src.y.w;
	w.z = src.z.w;
	x.w = y.w = z.w = 0.0f;
	w.w = 1.0f;
}

void mat44::SetTranspose( const mat44& src )
{
	float t;
	t = src.x.y;
	x.y = src.y.x;
	y.x = t;
	t = src.x.z;
	x.z = src.z.x;
	z.x = t;
	t = src.y.z;
	y.z = src.z.y;
	z.y = t;
	t = src.x.w;
	x.w = src.w.x;
	w.x = t;
	t = src.y.w;
	y.w = src.w.y;
	w.y = t;
	t = src.z.w;
	z.w = src.w.z;
	w.z = t;
	x.x = src.x.x;
	y.y = src.y.y;
	z.z = src.z.z;
	w.w = src.w.w;
}

bool mat44::SetInverseTranspose( const mat43& src )
{
	w.w = 1.0f;
	if( t_mat33().SetInverseTranspose( src.t_mat33() ) )
	{
		w.x = ( ( src.x.w * x.x ) + ( src.y.w * y.x ) + ( src.z.w * z.x ) );
		w.y = ( ( src.x.w * x.y ) + ( src.y.w * y.y ) + ( src.z.w * z.y ) );
		w.z = ( ( src.x.w * x.z ) + ( src.y.w * y.z ) + ( src.z.w * z.z ) );
		x.w = y.w = z.w = 0.0f;
		return( true );
	}
	else
	{
		w.x = w.y = w.z = 0.0f;
		x.w = y.w = z.w = 0.0f;
		return( false );
	}
}

bool mat44::SetInverseTranspose( const mat44& src )
{
	vec4 a, b, c;
	c.x = ( ( src.x.x * src.y.y ) - ( src.x.y * src.y.x ) );
	a.x = ( ( src.x.x * src.y.z ) - ( src.x.z * src.y.x ) );
	a.y = ( ( src.x.x * src.y.w ) - ( src.x.w * src.y.x ) );
	a.z = ( ( src.x.y * src.y.z ) - ( src.x.z * src.y.y ) );
	a.w = ( ( src.x.y * src.y.w ) - ( src.x.w * src.y.y ) );
	c.y = ( ( src.x.z * src.y.w ) - ( src.x.w * src.y.z ) );
	c.z = ( ( src.z.x * src.w.y ) - ( src.z.y * src.w.x ) );
	b.x = ( ( src.z.x * src.w.z ) - ( src.z.z * src.w.x ) );
	b.y = ( ( src.z.x * src.w.w ) - ( src.z.w * src.w.x ) );
	b.z = ( ( src.z.y * src.w.z ) - ( src.z.z * src.w.y ) );
	b.w = ( ( src.z.y * src.w.w ) - ( src.z.w * src.w.y ) );
	c.w = ( ( src.z.z * src.w.w ) - ( src.z.w * src.w.z ) );
	float d = ( ( c.x * c.w ) - ( a.x * b.w ) + ( a.y * b.z ) + ( a.z * b.y ) - ( a.w * b.x ) + ( c.y * c.z ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		mat44 tmp;
		d = ( 1.0f / d );
		a.x *= d;
		a.y *= d;
		a.z *= d;
		a.w *= d;
		b.x *= d;
		b.y *= d;
		b.z *= d;
		b.w *= d;
		c.x *= d;
		c.y *= d;
		c.z *= d;
		c.w *= d;
		tmp.x.x = ( ( src.y.w * b.z ) - ( src.y.z * b.w ) + ( src.y.y * c.w ) );
		tmp.x.y = ( ( src.y.z * b.y ) - ( src.y.w * b.x ) - ( src.y.x * c.w ) );
		tmp.x.z = ( ( src.y.x * b.w ) - ( src.y.y * b.y ) + ( src.y.w * c.z ) );
		tmp.x.w = ( ( src.y.y * b.x ) - ( src.y.x * b.z ) - ( src.y.z * c.z ) );
		tmp.y.x = ( ( src.x.z * b.w ) - ( src.x.w * b.z ) - ( src.x.y * c.w ) );
		tmp.y.y = ( ( src.x.w * b.x ) - ( src.x.z * b.y ) + ( src.x.x * c.w ) );
		tmp.y.z = ( ( src.x.y * b.y ) - ( src.x.x * b.w ) - ( src.x.w * c.z ) );
		tmp.y.w = ( ( src.x.x * b.z ) - ( src.x.y * b.x ) + ( src.x.z * c.z ) );
		tmp.z.x = ( ( src.w.w * a.z ) - ( src.w.z * a.w ) + ( src.w.y * c.y ) );
		tmp.z.y = ( ( src.w.z * a.y ) - ( src.w.w * a.x ) - ( src.w.x * c.y ) );
		tmp.z.z = ( ( src.w.x * a.w ) - ( src.w.y * a.y ) + ( src.w.w * c.x ) );
		tmp.z.w = ( ( src.w.y * a.x ) - ( src.w.x * a.z ) - ( src.w.z * c.x ) );
		tmp.w.x = ( ( src.z.z * a.w ) - ( src.z.w * a.z ) - ( src.z.y * c.y ) );
		tmp.w.y = ( ( src.z.w * a.x ) - ( src.z.z * a.y ) + ( src.z.x * c.y ) );
		tmp.w.z = ( ( src.z.y * a.y ) - ( src.z.x * a.w ) - ( src.z.w * c.x ) );
		tmp.w.w = ( ( src.z.x * a.z ) - ( src.z.y * a.x ) + ( src.z.z * c.x ) );
		Set( tmp );
		return( true );
	}
	else
	{
		SetIdentity();
		return( false );
	}
}

void mat44::SetPartialInverse( const mat44& src )
{
	mat44 tmp;
	vec4 a, b, c;
	c.x = ( ( src.x.x * src.y.y ) - ( src.x.y * src.y.x ) );
	a.x = ( ( src.x.x * src.y.z ) - ( src.x.z * src.y.x ) );
	a.y = ( ( src.x.x * src.y.w ) - ( src.x.w * src.y.x ) );
	a.z = ( ( src.x.y * src.y.z ) - ( src.x.z * src.y.y ) );
	a.w = ( ( src.x.y * src.y.w ) - ( src.x.w * src.y.y ) );
	c.y = ( ( src.x.z * src.y.w ) - ( src.x.w * src.y.z ) );
	c.z = ( ( src.z.x * src.w.y ) - ( src.z.y * src.w.x ) );
	b.x = ( ( src.z.x * src.w.z ) - ( src.z.z * src.w.x ) );
	b.y = ( ( src.z.x * src.w.w ) - ( src.z.w * src.w.x ) );
	b.z = ( ( src.z.y * src.w.z ) - ( src.z.z * src.w.y ) );
	b.w = ( ( src.z.y * src.w.w ) - ( src.z.w * src.w.y ) );
	c.w = ( ( src.z.z * src.w.w ) - ( src.z.w * src.w.z ) );
	tmp.x.x = ( ( src.y.w * b.z ) - ( src.y.z * b.w ) + ( src.y.y * c.w ) );
	tmp.y.x = ( ( src.y.z * b.y ) - ( src.y.w * b.x ) - ( src.y.x * c.w ) );
	tmp.z.x = ( ( src.y.x * b.w ) - ( src.y.y * b.y ) + ( src.y.w * c.z ) );
	tmp.w.x = ( ( src.y.y * b.x ) - ( src.y.x * b.z ) - ( src.y.z * c.z ) );
	tmp.x.y = ( ( src.x.z * b.w ) - ( src.x.w * b.z ) - ( src.x.y * c.w ) );
	tmp.y.y = ( ( src.x.w * b.x ) - ( src.x.z * b.y ) + ( src.x.x * c.w ) );
	tmp.z.y = ( ( src.x.y * b.y ) - ( src.x.x * b.w ) - ( src.x.w * c.z ) );
	tmp.w.y = ( ( src.x.x * b.z ) - ( src.x.y * b.x ) + ( src.x.z * c.z ) );
	tmp.x.z = ( ( src.w.w * a.z ) - ( src.w.z * a.w ) + ( src.w.y * c.y ) );
	tmp.y.z = ( ( src.w.z * a.y ) - ( src.w.w * a.x ) - ( src.w.x * c.y ) );
	tmp.z.z = ( ( src.w.x * a.w ) - ( src.w.y * a.y ) + ( src.w.w * c.x ) );
	tmp.w.z = ( ( src.w.y * a.x ) - ( src.w.x * a.z ) - ( src.w.z * c.x ) );
	tmp.x.w = ( ( src.z.z * a.w ) - ( src.z.w * a.z ) - ( src.z.y * c.y ) );
	tmp.y.w = ( ( src.z.w * a.x ) - ( src.z.z * a.y ) + ( src.z.x * c.y ) );
	tmp.z.w = ( ( src.z.y * a.y ) - ( src.z.x * a.w ) - ( src.z.w * c.x ) );
	tmp.w.w = ( ( src.z.x * a.z ) - ( src.z.y * a.x ) + ( src.z.z * c.x ) );
	Set( tmp );
}

void mat44::SetPartialInverseTranspose( const mat44& src )
{
	mat44 tmp;
	vec4 a, b, c;
	c.x = ( ( src.x.x * src.y.y ) - ( src.x.y * src.y.x ) );
	a.x = ( ( src.x.x * src.y.z ) - ( src.x.z * src.y.x ) );
	a.y = ( ( src.x.x * src.y.w ) - ( src.x.w * src.y.x ) );
	a.z = ( ( src.x.y * src.y.z ) - ( src.x.z * src.y.y ) );
	a.w = ( ( src.x.y * src.y.w ) - ( src.x.w * src.y.y ) );
	c.y = ( ( src.x.z * src.y.w ) - ( src.x.w * src.y.z ) );
	c.z = ( ( src.z.x * src.w.y ) - ( src.z.y * src.w.x ) );
	b.x = ( ( src.z.x * src.w.z ) - ( src.z.z * src.w.x ) );
	b.y = ( ( src.z.x * src.w.w ) - ( src.z.w * src.w.x ) );
	b.z = ( ( src.z.y * src.w.z ) - ( src.z.z * src.w.y ) );
	b.w = ( ( src.z.y * src.w.w ) - ( src.z.w * src.w.y ) );
	c.w = ( ( src.z.z * src.w.w ) - ( src.z.w * src.w.z ) );
	tmp.x.x = ( ( src.y.w * b.z ) - ( src.y.z * b.w ) + ( src.y.y * c.w ) );
	tmp.x.y = ( ( src.y.z * b.y ) - ( src.y.w * b.x ) - ( src.y.x * c.w ) );
	tmp.x.z = ( ( src.y.x * b.w ) - ( src.y.y * b.y ) + ( src.y.w * c.z ) );
	tmp.x.w = ( ( src.y.y * b.x ) - ( src.y.x * b.z ) - ( src.y.z * c.z ) );
	tmp.y.x = ( ( src.x.z * b.w ) - ( src.x.w * b.z ) - ( src.x.y * c.w ) );
	tmp.y.y = ( ( src.x.w * b.x ) - ( src.x.z * b.y ) + ( src.x.x * c.w ) );
	tmp.y.z = ( ( src.x.y * b.y ) - ( src.x.x * b.w ) - ( src.x.w * c.z ) );
	tmp.y.w = ( ( src.x.x * b.z ) - ( src.x.y * b.x ) + ( src.x.z * c.z ) );
	tmp.z.x = ( ( src.w.w * a.z ) - ( src.w.z * a.w ) + ( src.w.y * c.y ) );
	tmp.z.y = ( ( src.w.z * a.y ) - ( src.w.w * a.x ) - ( src.w.x * c.y ) );
	tmp.z.z = ( ( src.w.x * a.w ) - ( src.w.y * a.y ) + ( src.w.w * c.x ) );
	tmp.z.w = ( ( src.w.y * a.x ) - ( src.w.x * a.z ) - ( src.w.z * c.x ) );
	tmp.w.x = ( ( src.z.z * a.w ) - ( src.z.w * a.z ) - ( src.z.y * c.y ) );
	tmp.w.y = ( ( src.z.w * a.x ) - ( src.z.z * a.y ) + ( src.z.x * c.y ) );
	tmp.w.z = ( ( src.z.y * a.y ) - ( src.z.x * a.w ) - ( src.z.w * c.x ) );
	tmp.w.w = ( ( src.z.x * a.z ) - ( src.z.y * a.x ) + ( src.z.z * c.x ) );
	Set( tmp );
}

void mat44::SetReflect( const plane& about )
{
	float s = -( 2.0f / ( ( about.x * about.x ) + ( about.y * about.y ) + ( about.z * about.z ) ) );
	x.x = ( ( about.x * about.x * s ) + 1.0f );
	x.y = ( about.x * about.y * s );
	x.z = ( about.x * about.z * s );
	x.w = ( about.x * about.w * s );
	y.x = ( about.y * about.x * s );
	y.y = ( ( about.y * about.y * s ) + 1.0f );
	y.z = ( about.y * about.z * s );
	y.w = ( about.y * about.w * s );
	z.x = ( about.z * about.x * s );
	z.y = ( about.z * about.y * s );
	z.z = ( ( about.z * about.z * s ) + 1.0f );
	z.w = ( about.z * about.w * s );
	w.x = w.y = w.z = 0.0f;
	w.w = 1.0f;
}

void mat44::SetReflected( const mat44& src, const plane& about )
{
	float s = -( 2.0f / ( ( about.x * about.x ) + ( about.y * about.y ) + ( about.z * about.z ) ) );
	{
		float r = ( ( ( src.x.x * about.x ) + ( src.y.x * about.y ) + ( src.z.x * about.z ) + ( src.w.x * about.w ) ) * s );
		x.x = ( src.x.x + ( about.x * r ) );
		y.x = ( src.y.x + ( about.y * r ) );
		z.x = ( src.z.x + ( about.z * r ) );
		w.x = src.w.x;
	}
	{
		float r = ( ( ( src.x.y * about.x ) + ( src.y.y * about.y ) + ( src.z.y * about.z ) + ( src.w.y * about.w ) ) * s );
		x.y = ( src.x.y + ( about.x * r ) );
		y.y = ( src.y.y + ( about.y * r ) );
		z.y = ( src.z.y + ( about.z * r ) );
		w.y = src.w.y;
	}
	{
		float r = ( ( ( src.x.z * about.x ) + ( src.y.z * about.y ) + ( src.z.z * about.z ) + ( src.w.z * about.w ) ) * s );
		x.z = ( src.x.z + ( about.x * r ) );
		y.z = ( src.y.z + ( about.y * r ) );
		z.z = ( src.z.z + ( about.z * r ) );
		w.z = src.w.z;
	}
	{
		float r = ( ( ( src.x.w * about.x ) + ( src.y.w * about.y ) + ( src.z.w * about.z ) + ( src.w.w * about.w ) ) * s );
		x.w = ( src.x.w + ( about.x * r ) );
		y.w = ( src.y.w + ( about.y * r ) );
		z.w = ( src.z.w + ( about.z * r ) );
		w.w = src.w.w;
	}
}

void mat44::SetTensor( const vec4& u, const vec4& v )
{
	x.x = ( u.x * v.x );
	x.y = ( u.y * v.x );
	x.z = ( u.z * v.x );
	x.w = ( u.w * v.x );
	y.x = ( u.x * v.y );
	y.y = ( u.y * v.y );
	y.z = ( u.z * v.y );
	y.w = ( u.w * v.y );
	z.x = ( u.x * v.z );
	z.y = ( u.y * v.z );
	z.z = ( u.z * v.z );
	z.w = ( u.w * v.z );
	w.x = ( u.x * v.w );
	w.y = ( u.y * v.w );
	w.z = ( u.z * v.w );
	w.w = ( u.w * v.w );
}

void mat44::SetBarycentricTetrahedron( const vec3& v1, const vec3& v2, const vec3& v3, const vec3& v4 )
{
	t_mat43().SetBarycentricTetrahedron( v1, v2, v3, v4 );
	w.x = ( 0.0f - x.x - y.x - z.x );
	w.y = ( 0.0f - x.y - y.y - z.y );
	w.z = ( 0.0f - x.z - y.z - z.z );
	w.w = ( 1.0f - x.w - y.w - z.w );
}

void mat44::Set( const mat44& m )
{
	x.x = m.x.x;
	x.y = m.x.y;
	x.z = m.x.z;
	x.w = m.x.w;
	y.x = m.y.x;
	y.y = m.y.y;
	y.z = m.y.z;
	y.w = m.y.w;
	z.x = m.z.x;
	z.y = m.z.y;
	z.z = m.z.z;
	z.w = m.z.w;
	w.x = m.w.x;
	w.y = m.w.y;
	w.z = m.w.z;
	w.w = m.w.w;
}

void mat44::Set( const mat43& m )
{
	t_mat43().Set( m );
	w.x = w.y = w.z = 0.0f;
	w.w = 1.0f;
}

void mat44::Set( const joint& j )
{
	t_mat43().Set( j );
	w.x = w.y = w.z = 0.0f;
	w.w = 1.0f;
}

void mat44::Set( const quat& q )
{
	t_mat33().Set( q );
	x.w = y.w = z.w = 0.0f;
	w.x = w.y = w.z = 0.0f;
	w.w = 1.0f;
}

void mat44::Get( mat44& m ) const
{
	m.Set( *this );
}

bool mat44::GetEye( vec3& eye ) const
{
	vec3 a, b, c;
	c.x = ( ( x.y * y.z ) - ( x.z * y.y ) );
	c.y = ( ( x.z * y.x ) - ( x.x * y.z ) );
	c.z = ( ( x.x * y.y ) - ( x.y * y.x ) );
	float d = ( ( w.x * c.x ) + ( w.y * c.y ) + ( w.z * c.z ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		d = ( 1.0f / d );
		b.x = ( ( w.y * x.z ) - ( w.z * x.y ) );
		b.y = ( ( w.z * x.x ) - ( w.x * x.z ) );
		b.z = ( ( w.x * x.y ) - ( w.y * x.x ) );
		a.x = ( ( y.y * w.z ) - ( y.z * w.y ) );
		a.y = ( ( y.z * w.x ) - ( y.x * w.z ) );
		a.z = ( ( y.x * w.y ) - ( y.y * w.x ) );
		eye.x = ( ( ( x.w * a.x ) + ( y.w * b.x ) + ( w.w * c.x ) ) * d );
		eye.y = ( ( ( x.w * a.y ) + ( y.w * b.y ) + ( w.w * c.y ) ) * d );
		eye.z = ( ( ( x.w * a.z ) + ( y.w * b.z ) + ( w.w * c.z ) ) * d );
		return( true );
	}
	return( false );
}

bool mat44::GetInverseEye( vec3& eye ) const
{
	ASSERT( fabsf( w.z ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( w.z ) >= consts::FLOAT_MIN_RCP )
	{
		float rcpClip = ( 1.0f / w.z );
		eye.x = ( x.z * rcpClip );
		eye.y = ( y.z * rcpClip );
		eye.z = ( z.z * rcpClip );
		return( true );
	}
	return( false );
}

void mat44::GetDescOBB( box_verts& verts, box_faces& faces, const bool dx ) const
{
	GetDescOBB( faces, dx );
	GetDescOBB( verts, dx );
}

void mat44::GetDescOBB( box_verts& verts, box_faces& faces, const aabb& bounds ) const
{
	GetDescOBB( faces, bounds );
	GetDescOBB( verts, bounds );
}

void mat44::GetDescOBB( box_verts& verts, const bool dx ) const
{
	float t;
    if( dx )
    {
	    t = ( w.w - w.y - w.x );
	    ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	    t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	    verts.verts[ BOX_VERT_NBL ].x = ( ( x.w - x.y - x.x ) * t );
	    verts.verts[ BOX_VERT_NBL ].y = ( ( y.w - y.y - y.x ) * t );
	    verts.verts[ BOX_VERT_NBL ].z = ( ( z.w - z.y - z.x ) * t );
	    t = ( w.w - w.y + w.x );
	    ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	    t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	    verts.verts[ BOX_VERT_NBR ].x = ( ( x.w - x.y + x.x ) * t );
	    verts.verts[ BOX_VERT_NBR ].y = ( ( y.w - y.y + y.x ) * t );
	    verts.verts[ BOX_VERT_NBR ].z = ( ( z.w - z.y + z.x ) * t );
	    t = ( w.w + w.y - w.x );
	    ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	    t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	    verts.verts[ BOX_VERT_NTL ].x = ( ( x.w + x.y - x.x ) * t );
	    verts.verts[ BOX_VERT_NTL ].y = ( ( y.w + y.y - y.x ) * t );
	    verts.verts[ BOX_VERT_NTL ].z = ( ( z.w + z.y - z.x ) * t );
	    t = ( w.w + w.y + w.x );
	    ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	    t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	    verts.verts[ BOX_VERT_NTR ].x = ( ( x.w + x.y + x.x ) * t );
	    verts.verts[ BOX_VERT_NTR ].y = ( ( y.w + y.y + y.x ) * t );
	    verts.verts[ BOX_VERT_NTR ].z = ( ( z.w + z.y + z.x ) * t );
    }
    else
    {
	    t = ( w.w - w.z - w.y - w.x );
	    ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	    t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	    verts.verts[ BOX_VERT_NBL ].x = ( ( x.w - x.z - x.y - x.x ) * t );
	    verts.verts[ BOX_VERT_NBL ].y = ( ( y.w - y.z - y.y - y.x ) * t );
	    verts.verts[ BOX_VERT_NBL ].z = ( ( z.w - z.z - z.y - z.x ) * t );
	    t = ( w.w - w.z - w.y + w.x );
	    ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	    t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	    verts.verts[ BOX_VERT_NBR ].x = ( ( x.w - x.z - x.y + x.x ) * t );
	    verts.verts[ BOX_VERT_NBR ].y = ( ( y.w - y.z - y.y + y.x ) * t );
	    verts.verts[ BOX_VERT_NBR ].z = ( ( z.w - z.z - z.y + z.x ) * t );
	    t = ( w.w - w.z + w.y - w.x );
	    ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	    t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	    verts.verts[ BOX_VERT_NTL ].x = ( ( x.w - x.z + x.y - x.x ) * t );
	    verts.verts[ BOX_VERT_NTL ].y = ( ( y.w - y.z + y.y - y.x ) * t );
	    verts.verts[ BOX_VERT_NTL ].z = ( ( z.w - z.z + z.y - z.x ) * t );
	    t = ( w.w - w.z + w.y + w.x );
	    ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	    t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	    verts.verts[ BOX_VERT_NTR ].x = ( ( x.w - x.z + x.y + x.x ) * t );
	    verts.verts[ BOX_VERT_NTR ].y = ( ( y.w - y.z + y.y + y.x ) * t );
	    verts.verts[ BOX_VERT_NTR ].z = ( ( z.w - z.z + z.y + z.x ) * t );
    }
	t = ( w.w + w.z - w.y - w.x );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_FBL ].x = ( ( x.w + x.z - x.y - x.x ) * t );
	verts.verts[ BOX_VERT_FBL ].y = ( ( y.w + y.z - y.y - y.x ) * t );
	verts.verts[ BOX_VERT_FBL ].z = ( ( z.w + z.z - z.y - z.x ) * t );
	t = ( w.w + w.z - w.y + w.x );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_FBR ].x = ( ( x.w + x.z - x.y + x.x ) * t );
	verts.verts[ BOX_VERT_FBR ].y = ( ( y.w + y.z - y.y + y.x ) * t );
	verts.verts[ BOX_VERT_FBR ].z = ( ( z.w + z.z - z.y + z.x ) * t );
	t = ( w.w + w.z + w.y - w.x );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_FTL ].x = ( ( x.w + x.z + x.y - x.x ) * t );
	verts.verts[ BOX_VERT_FTL ].y = ( ( y.w + y.z + y.y - y.x ) * t );
	verts.verts[ BOX_VERT_FTL ].z = ( ( z.w + z.z + z.y - z.x ) * t );
	t = ( w.w + w.z + w.y + w.x );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_FTR ].x = ( ( x.w + x.z + x.y + x.x ) * t );
	verts.verts[ BOX_VERT_FTR ].y = ( ( y.w + y.z + y.y + y.x ) * t );
	verts.verts[ BOX_VERT_FTR ].z = ( ( z.w + z.z + z.y + z.x ) * t );
}

void mat44::GetDescOBB( box_verts& verts, const aabb& bounds ) const
{
	mat43 min, max;
	float t;
	min.x.x = ( x.x * bounds.min.x );
	min.x.y = ( y.x * bounds.min.x );
	min.x.z = ( z.x * bounds.min.x );
	min.x.w = ( w.x * bounds.min.x );
	min.y.x = ( x.y * bounds.min.y );
	min.y.y = ( y.y * bounds.min.y );
	min.y.z = ( z.y * bounds.min.y );
	min.y.w = ( w.y * bounds.min.y );
	min.z.x = ( x.z * bounds.min.z );
	min.z.y = ( y.z * bounds.min.z );
	min.z.z = ( z.z * bounds.min.z );
	min.z.w = ( w.z * bounds.min.z );
	max.x.x = ( x.x * bounds.max.x );
	max.x.y = ( y.x * bounds.max.x );
	max.x.z = ( z.x * bounds.max.x );
	max.x.w = ( w.x * bounds.max.x );
	max.y.x = ( x.y * bounds.max.y );
	max.y.y = ( y.y * bounds.max.y );
	max.y.z = ( z.y * bounds.max.y );
	max.y.w = ( w.y * bounds.max.y );
	max.z.x = ( x.z * bounds.max.z );
	max.z.y = ( y.z * bounds.max.z );
	max.z.z = ( z.z * bounds.max.z );
	max.z.w = ( w.z * bounds.max.z );
	t = ( w.w + min.z.w + min.y.w + min.x.w );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_NBL ].x = ( ( x.w + min.z.x + min.y.x + min.x.x ) * t );
	verts.verts[ BOX_VERT_NBL ].y = ( ( y.w + min.z.y + min.y.y + min.x.y ) * t );
	verts.verts[ BOX_VERT_NBL ].z = ( ( z.w + min.z.z + min.y.z + min.x.z ) * t );
	t = ( w.w + min.z.w + min.y.w + max.x.w );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_NBR ].x = ( ( x.w + min.z.x + min.y.x + max.x.x ) * t );
	verts.verts[ BOX_VERT_NBR ].y = ( ( y.w + min.z.y + min.y.y + max.x.y ) * t );
	verts.verts[ BOX_VERT_NBR ].z = ( ( z.w + min.z.z + min.y.z + max.x.z ) * t );
	t = ( w.w + min.z.w + max.y.w + min.x.w );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_NTL ].x = ( ( x.w + min.z.x + max.y.x + min.x.x ) * t );
	verts.verts[ BOX_VERT_NTL ].y = ( ( y.w + min.z.y + max.y.y + min.x.y ) * t );
	verts.verts[ BOX_VERT_NTL ].z = ( ( z.w + min.z.z + max.y.z + min.x.z ) * t );
	t = ( w.w + min.z.w + max.y.w + max.x.w );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_NTR ].x = ( ( x.w + min.z.x + max.y.x + max.x.x ) * t );
	verts.verts[ BOX_VERT_NTR ].y = ( ( y.w + min.z.y + max.y.y + max.x.y ) * t );
	verts.verts[ BOX_VERT_NTR ].z = ( ( z.w + min.z.z + max.y.z + max.x.z ) * t );
	t = ( w.w + max.z.w + min.y.w + min.x.w );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_FBL ].x = ( ( x.w + max.z.x + min.y.x + min.x.x ) * t );
	verts.verts[ BOX_VERT_FBL ].y = ( ( y.w + max.z.y + min.y.y + min.x.y ) * t );
	verts.verts[ BOX_VERT_FBL ].z = ( ( z.w + max.z.z + min.y.z + min.x.z ) * t );
	t = ( w.w + max.z.w + min.y.w + max.x.w );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_FBR ].x = ( ( x.w + max.z.x + min.y.x + max.x.x ) * t );
	verts.verts[ BOX_VERT_FBR ].y = ( ( y.w + max.z.y + min.y.y + max.x.y ) * t );
	verts.verts[ BOX_VERT_FBR ].z = ( ( z.w + max.z.z + min.y.z + max.x.z ) * t );
	t = ( w.w + max.z.w + max.y.w + min.x.w );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_FTL ].x = ( ( x.w + max.z.x + max.y.x + min.x.x ) * t );
	verts.verts[ BOX_VERT_FTL ].y = ( ( y.w + max.z.y + max.y.y + min.x.y ) * t );
	verts.verts[ BOX_VERT_FTL ].z = ( ( z.w + max.z.z + max.y.z + min.x.z ) * t );
	t = ( w.w + max.z.w + max.y.w + max.x.w );
	ASSERT( fabsf( t ) >= consts::FLOAT_MIN_RCP );
	t = ( ( fabsf( t ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / t ) : 0.0f );
	verts.verts[ BOX_VERT_FTR ].x = ( ( x.w + max.z.x + max.y.x + max.x.x ) * t );
	verts.verts[ BOX_VERT_FTR ].y = ( ( y.w + max.z.y + max.y.y + max.x.y ) * t );
	verts.verts[ BOX_VERT_FTR ].z = ( ( z.w + max.z.z + max.y.z + max.x.z ) * t );
}

void mat44::GetDescOBB( box_faces& faces, const bool dx ) const
{
	mat44 tmp;
	tmp.SetInverse( *this );
	tmp.GetDescFrustum( faces, dx );
	faces.Invert();
}

void mat44::GetDescOBB( box_faces& faces, const aabb& bounds ) const
{
	mat44 tmp;
	tmp.SetInverse( *this );
	tmp.GetDescFrustum( faces, bounds );
	faces.Invert();
}

void mat44::GetDescFrustum( box_verts& verts, box_faces& faces, const bool dx ) const
{
	GetRawDescFrustum( faces, dx );
	faces.GetVerts( verts );
	faces.Normalize();
}

void mat44::GetDescFrustum( box_verts& verts, box_faces& faces, const aabb& bounds ) const
{
	GetRawDescFrustum( faces, bounds );
	faces.GetVerts( verts );
	faces.Normalize();
}

void mat44::GetDescFrustum( box_verts& verts, const bool dx ) const
{
	mat44 tmp;
	tmp.SetInverse( *this );
	tmp.GetDescOBB( verts, dx );
}

void mat44::GetDescFrustum( box_verts& verts, const aabb& bounds ) const
{
	mat44 tmp;
	tmp.SetInverse( *this );
	tmp.GetDescOBB( verts, bounds );
}

void mat44::GetDescFrustum( box_faces& faces, const bool dx ) const
{
	GetRawDescFrustum( faces, dx );
	faces.Normalize();
}

void mat44::GetDescFrustum( box_faces& faces, const aabb& bounds ) const
{
	GetRawDescFrustum( faces, bounds );
	faces.Normalize();
}

void mat44::GetRawDescFrustum( box_faces& faces, const bool dx ) const
{
	faces.faces[ BOX_FACE_L ].x = ( w.x + x.x );
	faces.faces[ BOX_FACE_L ].y = ( w.y + x.y );
	faces.faces[ BOX_FACE_L ].z = ( w.z + x.z );
	faces.faces[ BOX_FACE_L ].w = ( w.w + x.w );
	faces.faces[ BOX_FACE_R ].x = ( w.x - x.x );
	faces.faces[ BOX_FACE_R ].y = ( w.y - x.y );
	faces.faces[ BOX_FACE_R ].z = ( w.z - x.z );
	faces.faces[ BOX_FACE_R ].w = ( w.w - x.w );
	faces.faces[ BOX_FACE_B ].x = ( w.x + y.x );
	faces.faces[ BOX_FACE_B ].y = ( w.y + y.y );
	faces.faces[ BOX_FACE_B ].z = ( w.z + y.z );
	faces.faces[ BOX_FACE_B ].w = ( w.w + y.w );
	faces.faces[ BOX_FACE_T ].x = ( w.x - y.x );
	faces.faces[ BOX_FACE_T ].y = ( w.y - y.y );
	faces.faces[ BOX_FACE_T ].z = ( w.z - y.z );
	faces.faces[ BOX_FACE_T ].w = ( w.w - y.w );
    if( dx )
    {
	    faces.faces[ BOX_FACE_N ].x = z.x;
	    faces.faces[ BOX_FACE_N ].y = z.y;
	    faces.faces[ BOX_FACE_N ].z = z.z;
	    faces.faces[ BOX_FACE_N ].w = z.w;
    }
    else
    {
	    faces.faces[ BOX_FACE_N ].x = ( w.x + z.x );
	    faces.faces[ BOX_FACE_N ].y = ( w.y + z.y );
	    faces.faces[ BOX_FACE_N ].z = ( w.z + z.z );
	    faces.faces[ BOX_FACE_N ].w = ( w.w + z.w );
    }
	faces.faces[ BOX_FACE_F ].x = ( w.x - z.x );
	faces.faces[ BOX_FACE_F ].y = ( w.y - z.y );
	faces.faces[ BOX_FACE_F ].z = ( w.z - z.z );
	faces.faces[ BOX_FACE_F ].w = ( w.w - z.w );
}

void mat44::GetRawDescFrustum( box_faces& faces, const aabb& bounds ) const
{
	float r, s, t;
	r = ( bounds.max.x - bounds.min.x );
	ASSERT( fabsf( r ) >= consts::FLOAT_MIN_RCP );
	r = ( ( fabsf( r ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / r ) : 0.0f );
	s = ( ( bounds.max.x + bounds.min.x ) * r );
	r += r;
	t = ( ( x.x * r ) - ( w.x * s ) );
	faces.faces[ BOX_FACE_L ].x = ( w.x + t );
	faces.faces[ BOX_FACE_R ].x = ( w.x - t );
	t = ( ( x.y * r ) - ( w.y * s ) );
	faces.faces[ BOX_FACE_L ].y = ( w.y + t );
	faces.faces[ BOX_FACE_R ].y = ( w.y - t );
	t = ( ( x.z * r ) - ( w.z * s ) );
	faces.faces[ BOX_FACE_L ].z = ( w.z + t );
	faces.faces[ BOX_FACE_R ].z = ( w.z - t );
	t = ( ( x.w * r ) - ( w.w * s ) );
	faces.faces[ BOX_FACE_L ].w = ( w.w + t );
	faces.faces[ BOX_FACE_R ].w = ( w.w - t );
	r = ( bounds.max.y - bounds.min.y );
	ASSERT( fabsf( r ) >= consts::FLOAT_MIN_RCP );
	r = ( ( fabsf( r ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / r ) : 0.0f );
	s = ( ( bounds.max.y + bounds.min.y ) * r );
	r += r;
	t = ( ( y.x * r ) - ( w.x * s ) );
	faces.faces[ BOX_FACE_B ].x = ( w.x + t );
	faces.faces[ BOX_FACE_T ].x = ( w.x - t );
	t = ( ( y.y * r ) - ( w.y * s ) );
	faces.faces[ BOX_FACE_B ].y = ( w.y + t );
	faces.faces[ BOX_FACE_T ].y = ( w.y - t );
	t = ( ( y.z * r ) - ( w.z * s ) );
	faces.faces[ BOX_FACE_B ].z = ( w.z + t );
	faces.faces[ BOX_FACE_T ].z = ( w.z - t );
	t = ( ( y.w * r ) - ( w.w * s ) );
	faces.faces[ BOX_FACE_B ].w = ( w.w + t );
	faces.faces[ BOX_FACE_T ].w = ( w.w - t );
	r = ( bounds.max.z - bounds.min.z );
	ASSERT( fabsf( r ) >= consts::FLOAT_MIN_RCP );
	r = ( ( fabsf( r ) >= consts::FLOAT_MIN_RCP ) ? ( 1.0f / r ) : 0.0f );
	s = ( ( bounds.max.z + bounds.min.z ) * r );
	r += r;
	t = ( ( z.x * r ) - ( w.x * s ) );
	faces.faces[ BOX_FACE_N ].x = ( w.x + t );
	faces.faces[ BOX_FACE_F ].x = ( w.x - t );
	t = ( ( z.y * r ) - ( w.y * s ) );
	faces.faces[ BOX_FACE_N ].y = ( w.y + t );
	faces.faces[ BOX_FACE_F ].y = ( w.y - t );
	t = ( ( z.z * r ) - ( w.z * s ) );
	faces.faces[ BOX_FACE_N ].z = ( w.z + t );
	faces.faces[ BOX_FACE_F ].z = ( w.z - t );
	t = ( ( z.w * r ) - ( w.w * s ) );
	faces.faces[ BOX_FACE_N ].w = ( w.w + t );
	faces.faces[ BOX_FACE_F ].w = ( w.w - t );
}

void mat44::GetPositiveFrustumPlaneX( const float c, plane& p ) const
{
	p.x = ( x.x - ( w.x * c ) );
	p.y = ( x.y - ( w.y * c ) );
	p.z = ( x.z - ( w.z * c ) );
	p.w = ( x.w - ( w.w * c ) );
}

void mat44::GetPositiveFrustumPlaneY( const float c, plane& p ) const
{
	p.x = ( y.x - ( w.x * c ) );
	p.y = ( y.y - ( w.y * c ) );
	p.z = ( y.z - ( w.z * c ) );
	p.w = ( y.w - ( w.w * c ) );
}

void mat44::GetPositiveFrustumPlaneZ( const float c, plane& p ) const
{
	p.x = ( z.x - ( w.x * c ) );
	p.y = ( z.y - ( w.y * c ) );
	p.z = ( z.z - ( w.z * c ) );
	p.w = ( z.w - ( w.w * c ) );
}

void mat44::GetNegativeFrustumPlaneX( const float c, plane& p ) const
{
	p.x = ( ( w.x * c ) - x.x );
	p.y = ( ( w.y * c ) - x.y );
	p.z = ( ( w.z * c ) - x.z );
	p.w = ( ( w.w * c ) - x.w );
}

void mat44::GetNegativeFrustumPlaneY( const float c, plane& p ) const
{
	p.x = ( ( w.x * c ) - y.x );
	p.y = ( ( w.y * c ) - y.y );
	p.z = ( ( w.z * c ) - y.z );
	p.w = ( ( w.w * c ) - y.w );
}

void mat44::GetNegativeFrustumPlaneZ( const float c, plane& p ) const
{
	p.x = ( ( w.x * c ) - z.x );
	p.y = ( ( w.y * c ) - z.y );
	p.z = ( ( w.z * c ) - z.z );
	p.w = ( ( w.w * c ) - z.w );
}

void mat44::GetFrustumPoint( const vec3& c, vec3& p ) const
{
	plane i, j, k;
	GetPositiveFrustumPlaneX( c.x, i );
	GetPositiveFrustumPlaneY( c.y, j );
	GetPositiveFrustumPlaneZ( c.z, k );
	IntersectPlanes( i, j, k, p );
}

bool mat44::SetBSpline( const float ( &knots )[ 6 ] )
{
    const float n = ( knots[ 3 ] - knots[ 2 ] );
	if( n < consts::FLOAT_MIN_NORM )
	{
		x.x = y.y = z.z = w.w = 1;
		x.y = x.z = x.w = y.x = y.z = y.w = z.x = z.y = z.w = w.x = w.y = w.z = 0;
		return( false );
	}
	else
	{
		const float s = ( ( knots[ 0 ] - knots[ 2 ] ) / n );
		const float t = ( ( knots[ 1 ] - knots[ 2 ] ) / n );
		const float u = ( ( knots[ 4 ] - knots[ 2 ] ) / n );
		const float v = ( ( knots[ 5 ] - knots[ 2 ] ) / n );
		const float a = ( 1 / ( ( 1 - t ) * ( 1 - s ) ) );
		const float b = ( 1 / ( ( 1 - t ) * ( u - t ) ) );
		const float c = ( 1 / ( u * ( u - t ) ) );
		const float d = ( 1 / ( u * v ) );
		x.x = -a;
		x.y = ( a + b + c );
		x.z = -( b + c + d );
		x.w = d;
		y.x = ( a * 3 );
		y.y = -( ( ( s + 2 ) * a ) + ( ( t + u + 1 ) * b ) + ( ( u + u ) * c ) );
		y.z = ( ( ( t + t + 1 ) * b ) + ( ( t + u ) * c ) + ( v * d ) );
		y.w = 0;
		z.x = -( a * 3 );
		z.y = ( ( ( s + s + 1 ) * a ) + ( ( t + u + ( t * u ) ) * b ) + ( u * u * c ) );
		z.z = -( ( ( ( t + 2 ) * b ) + ( u * c ) ) * t );
		z.w = 0;
		w.x = a;
		w.y = -( ( s * a ) + ( t * u * b ) );
		w.z = ( t * t * b );
		w.w = 0;
		return( true );
	}
}

bool mat44::SetCatmull( const float d1, const float d2, const float d3 )
{
	if( d2 < consts::FLOAT_MIN_NORM )
	{
		x.x = y.y = z.z = w.w = 1;
		x.y = x.z = x.w = y.x = y.z = y.w = z.x = z.y = z.w = w.x = w.y = w.z = 0;
		return( false );
	}
	else
	{
		const float a = ( d2 / ( d2 + d1 ) );
		const float b = ( d2 / ( d2 + d3 ) );
		x.x = -a;
		x.y = ( 2 - b );
		x.z = ( a - 2 );
		x.w = b;
		y.x = ( a + a );
		y.y = ( b - 3 );
		y.z = ( 3 - a - a );
		y.w = -b;
		z.x = -a;
		z.y = 0;
		z.z = a;
		z.w = 0;
		w.x = 0;
		w.y = 1;
		w.z = 0;
		w.w = 0;
		return( true );
	}
}

void mat44::SetControl( const float a, const float b, const float c, const float d )
{
	x.x = a;
	y.x = b;
	z.x = c;
	w.x = d;
	x.y = y.y = z.y = w.y = 1;
	x.z = y.z = z.z = w.z = 1;
	x.w = y.w = z.w = w.w = 1;
}

void mat44::SetControl( const vec2& a, const vec2& b, const vec2& c, const vec2& d )
{
	x.t_vec2().Set( a );
	y.t_vec2().Set( b );
	z.t_vec2().Set( c );
	w.t_vec2().Set( d );
	x.z = y.z = z.z = w.z = 1;
	x.w = y.w = z.w = w.w = 1;
}

void mat44::SetControl( const vec3& a, const vec3& b, const vec3& c, const vec3& d )
{
	x.t_vec3().Set( a );
	y.t_vec3().Set( b );
	z.t_vec3().Set( c );
	w.t_vec3().Set( d );
	x.w = y.w = z.w = w.w = 1;
}

void mat44::SetControl( const vec4& a, const vec4& b, const vec4& c, const vec4& d )
{
	x.Set( a );
	y.Set( b );
	z.Set( c );
	w.Set( d );
}

void mat44::EvaluatePolynomial( const float t, vec4& value ) const
{
	value.x = ( ( ( ( ( ( x.x * t ) + y.x ) * t ) + z.x ) * t ) + w.x );
	value.y = ( ( ( ( ( ( x.y * t ) + y.y ) * t ) + z.y ) * t ) + w.y );
	value.z = ( ( ( ( ( ( x.z * t ) + y.z ) * t ) + z.z ) * t ) + w.z );
	value.w = ( ( ( ( ( ( x.w * t ) + y.w ) * t ) + z.w ) * t ) + w.w );
}

void mat44::EvaluatePolynomial( const float t, vec4& value, vec4& first ) const
{
	value.x = ( ( ( ( ( ( x.x * t ) + y.x ) * t ) + z.x ) * t ) + w.x );
	value.y = ( ( ( ( ( ( x.y * t ) + y.y ) * t ) + z.y ) * t ) + w.y );
	value.z = ( ( ( ( ( ( x.z * t ) + y.z ) * t ) + z.z ) * t ) + w.z );
	value.w = ( ( ( ( ( ( x.w * t ) + y.w ) * t ) + z.w ) * t ) + w.w );
	first.x = ( ( ( ( x.x * t * 3.0f ) + y.x + y.x ) * t ) + z.x );
	first.y = ( ( ( ( x.y * t * 3.0f ) + y.y + y.y ) * t ) + z.y );
	first.z = ( ( ( ( x.z * t * 3.0f ) + y.z + y.z ) * t ) + z.z );
	first.w = ( ( ( ( x.w * t * 3.0f ) + y.w + y.w ) * t ) + z.w );
}

void mat44::EvaluatePolynomial( const float t, vec4& value, vec4& first, vec4& second ) const
{
	value.x = ( ( ( ( ( ( x.x * t ) + y.x ) * t ) + z.x ) * t ) + w.x );
	value.y = ( ( ( ( ( ( x.y * t ) + y.y ) * t ) + z.y ) * t ) + w.y );
	value.z = ( ( ( ( ( ( x.z * t ) + y.z ) * t ) + z.z ) * t ) + w.z );
	value.w = ( ( ( ( ( ( x.w * t ) + y.w ) * t ) + z.w ) * t ) + w.w );
	first.x = ( ( ( ( x.x * t * 3.0f ) + y.x + y.x ) * t ) + z.x );
	first.y = ( ( ( ( x.y * t * 3.0f ) + y.y + y.y ) * t ) + z.y );
	first.z = ( ( ( ( x.z * t * 3.0f ) + y.z + y.z ) * t ) + z.z );
	first.w = ( ( ( ( x.w * t * 3.0f ) + y.w + y.w ) * t ) + z.w );
	second.x = ( ( x.x * t * 6.0f ) + y.x + y.x );
	second.y = ( ( x.y * t * 6.0f ) + y.y + y.y );
	second.z = ( ( x.z * t * 6.0f ) + y.z + y.z );
	second.w = ( ( x.w * t * 6.0f ) + y.w + y.w );
}

void mat44::EvaluateDerivative( const float t, vec4& first ) const
{
	first.x = ( ( ( ( x.x * t * 3.0f ) + y.x + y.x ) * t ) + z.x );
	first.y = ( ( ( ( x.y * t * 3.0f ) + y.y + y.y ) * t ) + z.y );
	first.z = ( ( ( ( x.z * t * 3.0f ) + y.z + y.z ) * t ) + z.z );
	first.w = ( ( ( ( x.w * t * 3.0f ) + y.w + y.w ) * t ) + z.w );
}

void mat44::EvaluateDerivatives( const float t, vec4& first, vec4& second ) const
{
	first.x = ( ( ( ( x.x * t * 3.0f ) + y.x + y.x ) * t ) + z.x );
	first.y = ( ( ( ( x.y * t * 3.0f ) + y.y + y.y ) * t ) + z.y );
	first.z = ( ( ( ( x.z * t * 3.0f ) + y.z + y.z ) * t ) + z.z );
	first.w = ( ( ( ( x.w * t * 3.0f ) + y.w + y.w ) * t ) + z.w );
	second.x = ( ( x.x * t * 6.0f ) + y.x + y.x );
	second.y = ( ( x.y * t * 6.0f ) + y.y + y.y );
	second.z = ( ( x.z * t * 6.0f ) + y.z + y.z );
	second.w = ( ( x.w * t * 6.0f ) + y.w + y.w );
}

float mat44::IntegratePolynomialLength( const float lower_t, const float upper_t ) const
{
	vec4 a, b, c, d;
    float delta = ( upper_t - lower_t );
	EvaluateRationalDerivative( ( ( delta * 0.125f ) + lower_t ), a );
	EvaluateRationalDerivative( ( ( delta * 0.375f ) + lower_t ), b );
	EvaluateRationalDerivative( ( ( delta * 0.625f ) + lower_t ), c );
	EvaluateRationalDerivative( ( ( delta * 0.875f ) + lower_t ), d );
	return( ( a.Length() + b.Length() + c.Length() + d.Length() ) * delta * 0.25f );
}

float mat44::IntegratePolynomialLength( const unsigned int steps, const float lower_t, const float upper_t ) const
{
    unsigned int n = ( ( steps < 4 ) ? 4 : steps );
    float dt = ( ( upper_t - lower_t ) / n );
    float t = ( ( dt * 0.5f ) + lower_t );
    float sum = 0;
    while( n )
    {
		vec4 first;
		EvaluateDerivative( t, first );
		sum += first.Length();
        t += dt;
		--n;
    }
    return( sum * dt );
}

void mat44::EvaluateRationalPolynomial( const float t, vec4& value ) const
{
	EvaluatePolynomial( t, value );
}

void mat44::EvaluateRationalPolynomial( const float t, vec4& value, vec4& first ) const
{
	EvaluatePolynomial( t, value, first );
	float a = value.w;
	float b = first.w;
	float c = ( a * a );
	first.x = ( ( first.x * a ) - ( value.x * b ) );
	first.y = ( ( first.y * a ) - ( value.y * b ) );
	first.z = ( ( first.z * a ) - ( value.z * b ) );
	first.w = c;
}

void mat44::EvaluateRationalPolynomial( const float t, vec4& value, vec4& first, vec4& second ) const
{
	EvaluatePolynomial( t, value, first, second );
	float a = value.w;
	float b = first.w;
	float c = ( a * a );
	first.x = ( ( first.x * a ) - ( value.x * b ) );
	first.y = ( ( first.y * a ) - ( value.y * b ) );
	first.z = ( ( first.z * a ) - ( value.z * b ) );
	first.w = c;
	float d = ( a * c );
	float e = ( a * b * 2 );
	float f = ( second.w * c );
	float g = ( c * c );
	second.x = ( ( second.x * d ) - ( first.x * e ) - ( value.x * f ) );
	second.y = ( ( second.y * d ) - ( first.y * e ) - ( value.y * f ) );
	second.z = ( ( second.z * d ) - ( first.z * e ) - ( value.z * f ) );
	second.w = g;
}

void mat44::EvaluateRationalDerivative( const float t, vec4& first ) const
{
	vec4 value;
	EvaluateRationalPolynomial( t, value, first );
}

void mat44::EvaluateRationalDerivatives( const float t, vec4& first, vec4& second ) const
{
	vec4 value;
	EvaluateRationalPolynomial( t, value, first, second );
}

float mat44::IntegrateRationalPolynomialLength( const float lower_t, const float upper_t ) const
{
	vec4 a, b, c, d;
    float delta = ( upper_t - lower_t );
	EvaluateRationalDerivative( ( ( delta * 0.125f ) + lower_t ), a );
	EvaluateRationalDerivative( ( ( delta * 0.375f ) + lower_t ), b );
	EvaluateRationalDerivative( ( ( delta * 0.625f ) + lower_t ), c );
	EvaluateRationalDerivative( ( ( delta * 0.875f ) + lower_t ), d );
	return( ( ( a.t_vec3().Length() / a.w ) + ( b.t_vec3().Length() / b.w ) + ( c.t_vec3().Length() / c.w ) + ( d.t_vec3().Length() / d.w ) ) * delta * 0.25f );
}

float mat44::IntegrateRationalPolynomialLength( const unsigned int steps, const float lower_t, const float upper_t ) const
{
    unsigned int n = ( ( steps < 4 ) ? 4 : steps );
    float dt = ( ( upper_t - lower_t ) / n );
    float t = ( ( dt * 0.5f ) + lower_t );
    float sum = 0;
    while( n )
    {
		vec4 first;
		EvaluateRationalDerivative( t, first );
		sum += ( first.t_vec3().Length() / first.w );
        t += dt;
		--n;
    }
    return( sum * dt );
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
