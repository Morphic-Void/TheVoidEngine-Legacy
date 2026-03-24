
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   mat33.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	3 by 3 element matrix container and functions.
////
////    	Notes:
////
////    		Twist, turn and tilt are post-multipliers (rotate relative to the world axes).
////    		Roll, pitch and yaw are pre-multipliers (rotate relative to the local axes).
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

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "qtangent.h"
#include "mat33.h"
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
	namespace MAT33
	{
		const mat33		IDENTITY = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 } };

//  open-uniform quadratic b-spline (quadratic Bezier)
//  A, B, C : produces 3 weights for 3 control points, passes through points A and C
		const mat33		BEZIER = {
							{  1.0f, -2.0f,  1.0f,  0.0f },
							{ -2.0f,  2.0f,  0.0f,  0.0f },
							{  1.0f,  0.0f,  0.0f,  0.0f } };

//  uniform quadratic b-spline (quadratic b-spline}
//  A, B, C : produces 3 weights for 3 control points, does not pass through any control points
		const mat33		B_SPLINE = {
							{  0.5f, -1.0f,  0.5f,  0.0f },
							{ -1.0f,  1.0f,  0.0f,  0.0f },
							{  0.5f,  0.5f,  0.0f,  0.0f } };	//	A,B,C
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    mat33
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool mat33::IsNan( void ) const
{
	return( x.IsNan() || y.IsNan() || z.IsNan() );
}

bool mat33::IsReal( void ) const
{
	return( x.IsReal() && y.IsReal() && z.IsReal() );
}

float mat33::Det( void ) const
{
	return(
		( x.x * ( ( y.y * z.z ) - ( y.z * z.y ) ) ) +
		( x.y * ( ( y.z * z.x ) - ( y.x * z.z ) ) ) +
		( x.z * ( ( y.x * z.y ) - ( y.y * z.x ) ) ) );
}

void mat33::PointTo( const vec3& to, const vec3& up )
{
	x.z = ( ( to.y * up.z ) - ( to.z * up.y ) );
	y.z = ( ( to.z * up.x ) - ( to.x * up.z ) );
	z.z = ( ( to.x * up.y ) - ( to.y * up.x ) );
	float n = ( 1.0f / ( sqrtf( ( x.z * x.z ) + ( y.z * y.z ) + ( z.z * z.z ) ) + consts::FLOAT_MIN_NORM ) );
	x.z *= n;
	y.z *= n;
	z.z *= n;
	x.x = to.x;
	y.x = to.y;
	z.x = to.z;
	n = ( 1.0f / sqrtf( ( x.x * x.x ) + ( y.x * y.x ) + ( z.x * z.x ) ) );
	x.x *= n;
	y.x *= n;
	z.x *= n;
	x.y = ( ( y.z * z.x ) - ( z.z * y.x ) );
	y.y = ( ( z.z * x.x ) - ( x.z * z.x ) );
	z.y = ( ( x.z * y.x ) - ( y.z * x.x ) );
}

void mat33::LookTo( const vec3& to, const vec3& up )
{
	x.x = ( ( up.y * to.z ) - ( up.z * to.y ) );
	y.x = ( ( up.z * to.x ) - ( up.x * to.z ) );
	z.x = ( ( up.x * to.y ) - ( up.y * to.x ) );
	float n = ( 1.0f / ( sqrtf( ( x.x * x.x ) + ( y.x * y.x ) + ( z.x * z.x ) ) + consts::FLOAT_MIN_NORM ) );
	x.x *= n;
	y.x *= n;
	z.x *= n;
	x.z = to.x;
	y.z = to.y;
	z.z = to.z;
	n = ( 1.0f / sqrtf( ( x.z * x.z ) + ( y.z * y.z ) + ( z.z * z.z ) ) );
	x.z *= n;
	y.z *= n;
	z.z *= n;
	x.y = ( ( y.z * z.x ) - ( z.z * y.x ) );
	y.y = ( ( z.z * x.x ) - ( x.z * z.x ) );
	z.y = ( ( x.z * y.x ) - ( y.z * x.x ) );
}

void mat33::Scale( const float scale )
{
	x.x *= scale;
	y.x *= scale;
	z.x *= scale;
	x.y *= scale;
	y.y *= scale;
	z.y *= scale;
	x.z *= scale;
	y.z *= scale;
	z.z *= scale;
}

void mat33::Scale( const float scale_x, const float scale_y, const float scale_z )
{
	x.x *= scale_x;
	y.x *= scale_x;
	z.x *= scale_x;
	x.y *= scale_y;
	y.y *= scale_y;
	z.y *= scale_y;
	x.z *= scale_z;
	y.z *= scale_z;
	z.z *= scale_z;
}

void mat33::Twist( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec3 t = { x.x, x.y, x.z };
	x.x = ( ( x.x * c ) - ( y.x * s ) );
	x.y = ( ( x.y * c ) - ( y.y * s ) );
	x.z = ( ( x.z * c ) - ( y.z * s ) );
	y.x = ( ( y.x * c ) + ( t.x * s ) );
	y.y = ( ( y.y * c ) + ( t.y * s ) );
	y.z = ( ( y.z * c ) + ( t.z * s ) );
}

void mat33::Tilt( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec3 t = { y.x, y.y, y.z };
	y.x = ( ( y.x * c ) - ( z.x * s ) );
	y.y = ( ( y.y * c ) - ( z.y * s ) );
	y.z = ( ( y.z * c ) - ( z.z * s ) );
	z.x = ( ( z.x * c ) + ( t.x * s ) );
	z.y = ( ( z.y * c ) + ( t.y * s ) );
	z.z = ( ( z.z * c ) + ( t.z * s ) );
}

void mat33::Turn( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec3 t = { z.x, z.y, z.z };
	z.x = ( ( z.x * c ) - ( x.x * s ) );
	z.y = ( ( z.y * c ) - ( x.y * s ) );
	z.z = ( ( z.z * c ) - ( x.z * s ) );
	x.x = ( ( x.x * c ) + ( t.x * s ) );
	x.y = ( ( x.y * c ) + ( t.y * s ) );
	x.z = ( ( x.z * c ) + ( t.z * s ) );
}

void mat33::Roll( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec3 t = { x.x, y.x, z.x };
	x.x = ( ( x.x * c ) + ( x.y * s ) );
	y.x = ( ( y.x * c ) + ( y.y * s ) );
	z.x = ( ( z.x * c ) + ( z.y * s ) );
	x.y = ( ( x.y * c ) - ( t.x * s ) );
	y.y = ( ( y.y * c ) - ( t.y * s ) );
	z.y = ( ( z.y * c ) - ( t.z * s ) );
}

void mat33::Pitch( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec3 t = { x.y, y.y, z.y };
	x.y = ( ( x.y * c ) + ( x.z * s ) );
	y.y = ( ( y.y * c ) + ( y.z * s ) );
	z.y = ( ( z.y * c ) + ( z.z * s ) );
	x.z = ( ( x.z * c ) - ( t.x * s ) );
	y.z = ( ( y.z * c ) - ( t.y * s ) );
	z.z = ( ( z.z * c ) - ( t.z * s ) );
}

void mat33::Yaw( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec3 t = { x.z, y.z, z.z };
	x.z = ( ( x.z * c ) + ( x.x * s ) );
	y.z = ( ( y.z * c ) + ( y.x * s ) );
	z.z = ( ( z.z * c ) + ( z.x * s ) );
	x.x = ( ( x.x * c ) - ( t.x * s ) );
	y.x = ( ( y.x * c ) - ( t.y * s ) );
	z.x = ( ( z.x * c ) - ( t.z * s ) );
}

void mat33::Mul( const mat33& mul )
{
	SetMul( *this, mul );
}

bool mat33::Invert( void )
{
	vec3 a, b, c;
	a.x = ( ( y.y * z.z ) - ( y.z * z.y ) );
	a.y = ( ( y.z * z.x ) - ( y.x * z.z ) );
	a.z = ( ( y.x * z.y ) - ( y.y * z.x ) );
	float d = ( ( x.x * a.x ) + ( x.y * a.y ) + ( x.z * a.z ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		d = ( 1.0f / d );
		b.x = ( ( z.y * x.z ) - ( z.z * x.y ) );
		b.y = ( ( z.z * x.x ) - ( z.x * x.z ) );
		b.z = ( ( z.x * x.y ) - ( z.y * x.x ) );
		c.x = ( ( x.y * y.z ) - ( x.z * y.y ) );
		c.y = ( ( x.z * y.x ) - ( x.x * y.z ) );
		c.z = ( ( x.x * y.y ) - ( x.y * y.x ) );
		x.x = ( a.x * d );
		y.x = ( a.y * d );
		z.x = ( a.z * d );
		x.y = ( b.x * d );
		y.y = ( b.y * d );
		z.y = ( b.z * d );
		x.z = ( c.x * d );
		y.z = ( c.y * d );
		z.z = ( c.z * d );
		return( true );
	}
	else
	{
		x.x = y.y = z.z = 1.0f;
		x.y = y.z = z.x = 0.0f;
		x.z = y.x = z.y = 0.0f;
		return( false );
	}
}

void mat33::Transpose( void )
{
	float t;
	t = x.y;
	x.y = y.x;
	y.x = t;
	t = x.z;
	x.z = z.x;
	z.x = t;
	t = y.z;
	y.z = z.y;
	z.y = t;
}

bool mat33::InvertTranspose( void )
{
	vec3 a, b, c;
	a.x = ( ( y.y * z.z ) - ( y.z * z.y ) );
	a.y = ( ( y.z * z.x ) - ( y.x * z.z ) );
	a.z = ( ( y.x * z.y ) - ( y.y * z.x ) );
	float d = ( ( x.x * a.x ) + ( x.y * a.y ) + ( x.z * a.z ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		d = ( 1.0f / d );
		b.x = ( ( z.y * x.z ) - ( z.z * x.y ) );
		b.y = ( ( z.z * x.x ) - ( z.x * x.z ) );
		b.z = ( ( z.x * x.y ) - ( z.y * x.x ) );
		c.x = ( ( x.y * y.z ) - ( x.z * y.y ) );
		c.y = ( ( x.z * y.x ) - ( x.x * y.z ) );
		c.z = ( ( x.x * y.y ) - ( x.y * y.x ) );
		x.x = ( a.x * d );
		x.y = ( a.y * d );
		x.z = ( a.z * d );
		y.x = ( b.x * d );
		y.y = ( b.y * d );
		y.z = ( b.z * d );
		z.x = ( c.x * d );
		z.y = ( c.y * d );
		z.z = ( c.z * d );
		return( true );
	}
	else
	{
		x.x = y.y = z.z = 1.0f;
		x.y = y.z = z.x = 0.0f;
		x.z = y.x = z.y = 0.0f;
		return( false );
	}
}

void mat33::PartialInvert( void )
{
	vec3 a, b, c;
	a.x = ( ( y.y * z.z ) - ( y.z * z.y ) );
	a.y = ( ( y.z * z.x ) - ( y.x * z.z ) );
	a.z = ( ( y.x * z.y ) - ( y.y * z.x ) );
	b.x = ( ( z.y * x.z ) - ( z.z * x.y ) );
	b.y = ( ( z.z * x.x ) - ( z.x * x.z ) );
	b.z = ( ( z.x * x.y ) - ( z.y * x.x ) );
	c.x = ( ( x.y * y.z ) - ( x.z * y.y ) );
	c.y = ( ( x.z * y.x ) - ( x.x * y.z ) );
	c.z = ( ( x.x * y.y ) - ( x.y * y.x ) );
	x.x = a.x;
	y.x = a.y;
	z.x = a.z;
	x.y = b.x;
	y.y = b.y;
	z.y = b.z;
	x.z = c.x;
	y.z = c.y;
	z.z = c.z;
}

void mat33::PartialInvertTranspose( void )
{
	vec3 a, b, c;
	a.x = ( ( y.y * z.z ) - ( y.z * z.y ) );
	a.y = ( ( y.z * z.x ) - ( y.x * z.z ) );
	a.z = ( ( y.x * z.y ) - ( y.y * z.x ) );
	b.x = ( ( z.y * x.z ) - ( z.z * x.y ) );
	b.y = ( ( z.z * x.x ) - ( z.x * x.z ) );
	b.z = ( ( z.x * x.y ) - ( z.y * x.x ) );
	c.x = ( ( x.y * y.z ) - ( x.z * y.y ) );
	c.y = ( ( x.z * y.x ) - ( x.x * y.z ) );
	c.z = ( ( x.x * y.y ) - ( x.y * y.x ) );
	x.x = a.x;
	x.y = a.y;
	x.z = a.z;
	y.x = b.x;
	y.y = b.y;
	y.z = b.z;
	z.x = c.x;
	z.y = c.y;
	z.z = c.z;
}

void mat33::SetMul( const mat33& src, const mat33& mul )
{
	mat33 tmp;
	tmp.x.x = ( ( src.x.x * mul.x.x ) + ( src.y.x * mul.x.y ) + ( src.z.x * mul.x.z ) );
	tmp.x.y = ( ( src.x.y * mul.x.x ) + ( src.y.y * mul.x.y ) + ( src.z.y * mul.x.z ) );
	tmp.x.z = ( ( src.x.z * mul.x.x ) + ( src.y.z * mul.x.y ) + ( src.z.z * mul.x.z ) );
	tmp.y.x = ( ( src.x.x * mul.y.x ) + ( src.y.x * mul.y.y ) + ( src.z.x * mul.y.z ) );
	tmp.y.y = ( ( src.x.y * mul.y.x ) + ( src.y.y * mul.y.y ) + ( src.z.y * mul.y.z ) );
	tmp.y.z = ( ( src.x.z * mul.y.x ) + ( src.y.z * mul.y.y ) + ( src.z.z * mul.y.z ) );
	tmp.z.x = ( ( src.x.x * mul.z.x ) + ( src.y.x * mul.z.y ) + ( src.z.x * mul.z.z ) );
	tmp.z.y = ( ( src.x.y * mul.z.x ) + ( src.y.y * mul.z.y ) + ( src.z.y * mul.z.z ) );
	tmp.z.z = ( ( src.x.z * mul.z.x ) + ( src.y.z * mul.z.y ) + ( src.z.z * mul.z.z ) );
	Set( tmp );
}

void mat33::SetDelta( const mat33& src, const mat33& trg )
{
	SetInverse( src );
	Mul( trg );
}

bool mat33::SetInverse( const mat33& src )
{
	mat33 tmp;
	tmp.x.x = ( ( src.y.y * src.z.z ) - ( src.y.z * src.z.y ) );
	tmp.y.x = ( ( src.y.z * src.z.x ) - ( src.y.x * src.z.z ) );
	tmp.z.x = ( ( src.y.x * src.z.y ) - ( src.y.y * src.z.x ) );
	float d = ( ( src.x.x * tmp.x.x ) + ( src.x.y * tmp.y.x ) + ( src.x.z * tmp.z.x ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		d = ( 1.0f / d );
		tmp.x.x *= d;
		tmp.y.x *= d;
		tmp.z.x *= d;
		tmp.x.y = ( ( ( src.z.y * src.x.z ) - ( src.z.z * src.x.y ) ) * d );
		tmp.y.y = ( ( ( src.z.z * src.x.x ) - ( src.z.x * src.x.z ) ) * d );
		tmp.z.y = ( ( ( src.z.x * src.x.y ) - ( src.z.y * src.x.x ) ) * d );
		tmp.x.z = ( ( ( src.x.y * src.y.z ) - ( src.x.z * src.y.y ) ) * d );
		tmp.y.z = ( ( ( src.x.z * src.y.x ) - ( src.x.x * src.y.z ) ) * d );
		tmp.z.z = ( ( ( src.x.x * src.y.y ) - ( src.x.y * src.y.x ) ) * d );
		Set( tmp );
		return( true );
	}
	else
	{
		x.x = y.y = z.z = 1.0f;
		x.y = y.z = z.x = 0.0f;
		x.z = y.x = z.y = 0.0f;
		return( false );
	}
}

void mat33::SetTranspose( const mat33& src )
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
}

bool mat33::SetInverseTranspose( const mat33& src )
{
	mat33 tmp;
	tmp.x.x = ( ( src.y.y * src.z.z ) - ( src.y.z * src.z.y ) );
	tmp.x.y = ( ( src.y.z * src.z.x ) - ( src.y.x * src.z.z ) );
	tmp.x.z = ( ( src.y.x * src.z.y ) - ( src.y.y * src.z.x ) );
	float d = ( ( src.x.x * tmp.x.x ) + ( src.x.y * tmp.x.y ) + ( src.x.z * tmp.x.z ) );
	ASSERT( fabsf( d ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{
		d = ( 1.0f / d );
		tmp.x.x *= d;
		tmp.x.y *= d;
		tmp.x.z *= d;
		tmp.y.x = ( ( ( src.z.y * src.x.z ) - ( src.z.z * src.x.y ) ) * d );
		tmp.y.y = ( ( ( src.z.z * src.x.x ) - ( src.z.x * src.x.z ) ) * d );
		tmp.y.z = ( ( ( src.z.x * src.x.y ) - ( src.z.y * src.x.x ) ) * d );
		tmp.z.x = ( ( ( src.x.y * src.y.z ) - ( src.x.z * src.y.y ) ) * d );
		tmp.z.y = ( ( ( src.x.z * src.y.x ) - ( src.x.x * src.y.z ) ) * d );
		tmp.z.z = ( ( ( src.x.x * src.y.y ) - ( src.x.y * src.y.x ) ) * d );
		Set( tmp );
		return( true );
	}
	else
	{
		x.x = y.y = z.z = 1.0f;
		x.y = y.z = z.x = 0.0f;
		x.z = y.x = z.y = 0.0f;
		return( false );
	}
}

void mat33::SetPartialInverse( const mat33& src )
{
	mat33 tmp;
	tmp.x.x = ( ( src.y.y * src.z.z ) - ( src.y.z * src.z.y ) );
	tmp.y.x = ( ( src.y.z * src.z.x ) - ( src.y.x * src.z.z ) );
	tmp.z.x = ( ( src.y.x * src.z.y ) - ( src.y.y * src.z.x ) );
	tmp.x.y = ( ( src.z.y * src.x.z ) - ( src.z.z * src.x.y ) );
	tmp.y.y = ( ( src.z.z * src.x.x ) - ( src.z.x * src.x.z ) );
	tmp.z.y = ( ( src.z.x * src.x.y ) - ( src.z.y * src.x.x ) );
	tmp.x.z = ( ( src.x.y * src.y.z ) - ( src.x.z * src.y.y ) );
	tmp.y.z = ( ( src.x.z * src.y.x ) - ( src.x.x * src.y.z ) );
	tmp.z.z = ( ( src.x.x * src.y.y ) - ( src.x.y * src.y.x ) );
	Set( tmp );
}

void mat33::SetPartialInverseTranspose( const mat33& src )
{
	mat33 tmp;
	tmp.x.x = ( ( src.y.y * src.z.z ) - ( src.y.z * src.z.y ) );
	tmp.x.y = ( ( src.y.z * src.z.x ) - ( src.y.x * src.z.z ) );
	tmp.x.z = ( ( src.y.x * src.z.y ) - ( src.y.y * src.z.x ) );
	tmp.y.x = ( ( src.z.y * src.x.z ) - ( src.z.z * src.x.y ) );
	tmp.y.y = ( ( src.z.z * src.x.x ) - ( src.z.x * src.x.z ) );
	tmp.y.z = ( ( src.z.x * src.x.y ) - ( src.z.y * src.x.x ) );
	tmp.z.x = ( ( src.x.y * src.y.z ) - ( src.x.z * src.y.y ) );
	tmp.z.y = ( ( src.x.z * src.y.x ) - ( src.x.x * src.y.z ) );
	tmp.z.z = ( ( src.x.x * src.y.y ) - ( src.x.y * src.y.x ) );
	Set( tmp );
}

void mat33::SetRollPitchYaw( const float roll, const float pitch, const float yaw )
{
	float sr = sinf( roll );
	float cr = cosf( roll );
	float sp = sinf( pitch );
	float cp = cosf( pitch );
	float sy = sinf( yaw );
	float cy = cosf( yaw );
	float srsy = ( sr * sy );
	float srcy = ( sr * cy );
	float crsy = ( cr * sy );
	float crcy = ( cr * cy );
	x.x = ( crcy - ( srsy * sp ) );
	x.y = -( sr * cp );
	x.z = ( crsy + ( srcy * sp ) );
	y.x = ( srcy + ( crsy * sp ) );
	y.y = ( cr * cp );
	y.z = ( srsy - ( crcy * sp ) );
	z.x = -( sy * cp );
	z.y = sp;
	z.z = ( cy * cp );
}

void mat33::SetYawPitchRoll( const float yaw, const float pitch, const float roll )
{
	float sr = sinf( roll );
	float cr = cosf( roll );
	float sp = sinf( pitch );
	float cp = cosf( pitch );
	float sy = sinf( yaw );
	float cy = cosf( yaw );
	float srsy = ( sr * sy );
	float srcy = ( sr * cy );
	float crsy = ( cr * sy );
	float crcy = ( cr * cy );
	x.x = ( ( srsy * sp ) + crcy );
	y.x = ( sr * cp );
	z.x = ( ( srcy * sp ) - crsy );
	x.y = ( ( crsy * sp ) - srcy );
	y.y = ( cr * cp );
	z.y = ( ( crcy * sp ) + srsy );
	x.z = ( sy * cp );
	y.z = -sp;
	z.z = ( cy * cp );
}

void mat33::SetPitchYaw( const float pitch, const float yaw )
{
	float sp = sinf( pitch );
	float cp = cosf( pitch );
	float sy = sinf( yaw );
	float cy = cosf( yaw );
	x.x = cy;
	x.y = 0.0f;
	x.z = sy;
	y.x = ( sy * sp );
	y.y = cp;
	y.z = -( cy * sp );
	z.x = -( sy * cp );
	z.y = sp;
	z.z = ( cy * cp );
}

void mat33::SetYawPitch( const float yaw, const float pitch )
{
	float sp = sinf( pitch );
	float cp = cosf( pitch );
	float sy = sinf( yaw );
	float cy = cosf( yaw );
	x.x = cy;
	y.x = 0.0f;
	z.x = -sy;
	x.y = ( sy * sp );
	y.y = cp;
	z.y = ( cy * sp );
	x.z = ( sy * cp );
	y.z = -sp;
	z.z = ( cy * cp );
}

void mat33::SetAxisAngle( const float axis_x, const float axis_y, const float axis_z, const float radians )
{
	vec3 axis = { axis_x, axis_y, axis_z };
	SetAxisAngle( axis, radians );
}

void mat33::SetAxisAngle( const vec3& axis, const float radians )
{
	float n = ( 1.0f / ( sqrtf( ( axis.x * axis.x ) + ( axis.y * axis.y ) + ( axis.z * axis.z ) ) + consts::FLOAT_MIN_NORM ) );
	float s = sinf( radians );
	float c = cosf( radians );
	float i = ( 1.0f - c );
	float wx = ( axis.x * n );
	float wy = ( axis.y * n );
	float wz = ( axis.z * n );
	float xy = ( wx * i );
	float yz = ( wy * i );
	float zx = ( wz * i );
	x.x = ( c + ( xy * wx ) );
	y.y = ( c + ( yz * wy ) );
	z.z = ( c + ( zx * wz ) );
	xy *= wy;
	yz *= wz;
	zx *= wx;
	wx *= s;
	wy *= s;
	wz *= s;
	x.y = ( xy - wz );
	y.x = ( xy + wz );
	y.z = ( yz - wx );
	z.y = ( yz + wx );
	z.x = ( zx - wy );
	x.z = ( zx + wy );
}

void mat33::SetScaling( const float scale )
{
	x.x = y.y = z.z = scale;
	x.y = y.x = y.z = z.y = x.z = z.x = 0.0f;
}

void mat33::SetScaling( const float scale_x, const float scale_y, const float scale_z )
{
	x.x = scale_x;
	y.y = scale_y;
	z.z = scale_z;
	x.y = y.x = y.z = z.y = x.z = z.x = 0.0f;
}

bool mat33::SetTanSpace( const vec3& dc1, const vec2& dt1, const vec3& dc2, const vec2& dt2, float& weight )
{
	float dt = dt1.Cross( dt2 );
	if( fabsf( dt ) < consts::UINT_FLOAT_MIN_NORM )
	{	//	texture deltas are too similar
		weight = 0;
		SetIdentity();
		return( false );
	}
	z.SetCross( dc1, dc2 );
	float dc = z.SqrLen();
	if( dc < consts::UINT_FLOAT_MIN_NORM )
	{	//	position deltas are too similar
		weight = 0;
		SetIdentity();
		return( false );
	}
	dt /= fabsf( dt );	//	determines sign
	x.x = ( ( dc1.x * dt2.y ) - ( dc2.x * dt1.y ) );
	x.y = ( ( dc1.y * dt2.y ) - ( dc2.y * dt1.y ) );
	x.z = ( ( dc1.z * dt2.y ) - ( dc2.z * dt1.y ) );
	y.x = ( ( dc2.x * dt1.x ) - ( dc1.x * dt2.x ) );
	y.y = ( ( dc2.y * dt1.x ) - ( dc1.y * dt2.x ) );
	y.z = ( ( dc2.z * dt1.x ) - ( dc1.z * dt2.x ) );
	x.Mul( dt / x.Length() );
	y.Mul( dt / y.Length() );
	z.Mul( 1.0f / sqrtf( dc ) );
	weight = sqrtf( dc / ( dc1.SqrLen() * dc2.SqrLen() ) );
	return( true );
}

bool mat33::SetTanSpace( const vec3& n, const vec3& dc1, const vec2& dt1, const vec3& dc2, const vec2& dt2, float& weight )
{
	if( !SetTanSpace( dc1, dt1, dc2, dt2, weight ) )
	{
		return( false );
	}
	float t = n.Length();
	if( t < consts::UINT_FLOAT_MIN_RCP )
	{
		weight = 0;
		SetIdentity();
		return( false );
	}
	t = ( 1.0f / t );
	z.x = ( n.x * t );
	z.y = ( n.y * t );
	z.z = ( n.z * t );
	t = z.Dot( x );
	x.x -= ( z.x * t );
	x.y -= ( z.y * t );
	x.z -= ( z.z * t );
	t = x.Length();
	if( t < consts::UINT_FLOAT_MIN_RCP )
	{
		weight = 0;
		SetIdentity();
		return( false );
	}
	t = ( 1.0f / t );
	x.x *= t;
	x.y *= t;
	x.z *= t;
	t = z.Dot( y );
	y.x -= ( z.x * t );
	y.y -= ( z.y * t );
	y.z -= ( z.z * t );
	t = y.Length();
	if( t < consts::UINT_FLOAT_MIN_RCP )
	{
		weight = 0;
		SetIdentity();
		return( false );
	}
	t = ( 1.0f / t );
	y.x *= t;
	y.y *= t;
	y.z *= t;
	return( true );
}

void mat33::SetTensor( const vec3& u, const vec3& v )
{
	x.x = ( u.x * v.x );
	x.y = ( u.y * v.x );
	x.z = ( u.z * v.x );
	y.x = ( u.x * v.y );
	y.y = ( u.y * v.y );
	y.z = ( u.z * v.y );
	z.x = ( u.x * v.z );
	z.y = ( u.y * v.z );
	z.z = ( u.z * v.z );
}

void mat33::Set( const mat33& m )
{
	x.x = m.x.x;
	x.y = m.x.y;
	x.z = m.x.z;
	y.x = m.y.x;
	y.y = m.y.y;
	y.z = m.y.z;
	z.x = m.z.x;
	z.y = m.z.y;
	z.z = m.z.z;
}

void mat33::Set( const quat& q )
{
	q.Get( *this );
}

void mat33::Set( const qtangent& t )
{
	t.Get( *this );
}

void mat33::GetRollPitchYaw( float& roll, float& pitch, float& yaw ) const
{
	yaw = atan2f( -z.x, z.z );
	float sy = sinf( -yaw );
	float cy = cosf( -yaw );
	pitch = atan2f( z.y, sqrtf( ( z.x * z.x ) + ( z.z * z.z ) ) );
	float sp = sinf( -pitch );
	float cp = cosf( -pitch );
	roll = atan2f( ( ( y.x * cy ) - ( y.z * sy ) ), ( ( y.y * cp ) + ( ( ( y.z * cy ) + ( y.x * sy ) ) * sp ) ) );
}

void mat33::GetYawPitchRoll( float& yaw, float& pitch, float& roll ) const
{
	roll = atan2f( y.x, y.y );
	float sr = sinf( -roll );
	float cr = cosf( -roll );
	pitch = atan2f( -y.z, sqrtf( ( y.x * y.x ) + ( y.y * y.y ) ) );
	float sp = sinf( -pitch );
	float cp = cosf( -pitch );
	yaw = atan2f( -( ( z.x * cr ) + ( z.y * sr ) ), ( ( z.z * cp ) - ( ( ( z.y * cr ) - ( z.x * sr ) ) * sp ) ) );
}

void mat33::Get( mat33& m ) const
{
	m.Set( *this );
}

void mat33::Get( quat& q ) const
{
	q.Set( *this );
}

void mat33::Get( qtangent& t ) const
{
	t.Set( *this );
}

bool mat33::SetBSpline( const float ( &knots )[ 4 ] )
{
	const float n = ( knots[ 2 ] - knots[ 1 ] );
	if( n < consts::FLOAT_MIN_NORM )
	{
		x.x = y.y = z.z = 1;
		x.y = x.z = y.x = y.z = z.x = z.y = 0;
		return( false );
	}
	else
	{
		const float r = ( knots[ 1 ] - knots[ 0 ] );
		const float s = ( knots[ 2 ] - knots[ 0 ] );
		const float t = ( knots[ 1 ] - knots[ 3 ] );
		const float a = ( n / s );
		const float b = ( r / s );
		const float c = ( n / t );
		x.x = a;
		x.y = ( c - a );
		x.z = -c;
		y.x = -( a + a );
		y.y = ( 1 + a - b );
		y.z = 0;
		z.x = a;
		z.y = b;
		z.z = 0;
		return( true );
	}
}

void mat33::SetControl( const float a, const float b, const float c )
{
	x.x = a;
	y.x = b;
	z.x = c;
	x.y = y.y = z.y = 1;
	x.z = y.z = z.z = 1;
}

void mat33::SetControl( const vec2& a, const vec2& b, const vec2& c )
{
	x.t_vec2().Set( a );
	y.t_vec2().Set( b );
	z.t_vec2().Set( c );
	x.z = y.z = z.z = 1;
}

void mat33::SetControl( const vec3& a, const vec3& b, const vec3& c )
{
	x.Set( a );
	y.Set( b );
	z.Set( c );
}

void mat33::EvaluatePolynomial( const float t, vec3& value ) const
{
	value.x = ( ( ( ( x.x * t ) + y.x ) * t ) + z.x );
	value.y = ( ( ( ( x.y * t ) + y.y ) * t ) + z.y );
	value.z = ( ( ( ( x.z * t ) + y.z ) * t ) + z.z );
}

void mat33::EvaluatePolynomial( const float t, vec3& value, vec3& first ) const
{
	value.x = ( ( ( ( x.x * t ) + y.x ) * t ) + z.x );
	value.y = ( ( ( ( x.y * t ) + y.y ) * t ) + z.y );
	value.z = ( ( ( ( x.z * t ) + y.z ) * t ) + z.z );
	first.x = ( ( ( x.x + x.x ) * t ) + y.x );
	first.y = ( ( ( x.y + x.y ) * t ) + y.y );
	first.z = ( ( ( x.z + x.z ) * t ) + y.z );
}

void mat33::EvaluateDerivative( const float t, vec3& first ) const
{
	first.x = ( ( ( x.x + x.x ) * t ) + y.x );
	first.y = ( ( ( x.y + x.y ) * t ) + y.y );
	first.z = ( ( ( x.z + x.z ) * t ) + y.z );
}

float mat33::IntegratePolynomialLength( const float lower_t, const float upper_t ) const
{
	vec3 a, b, c, d;
    float delta = ( upper_t - lower_t );
	EvaluateRationalDerivative( ( ( delta * 0.125f ) + lower_t ), a );
	EvaluateRationalDerivative( ( ( delta * 0.375f ) + lower_t ), b );
	EvaluateRationalDerivative( ( ( delta * 0.625f ) + lower_t ), c );
	EvaluateRationalDerivative( ( ( delta * 0.875f ) + lower_t ), d );
	return( ( a.Length() + b.Length() + c.Length() + d.Length() ) * delta * 0.25f );
}

float mat33::IntegratePolynomialLength( const unsigned int steps, const float lower_t, const float upper_t ) const
{
    unsigned int n = ( ( steps < 4 ) ? 4 : steps );
    float dt = ( ( upper_t - lower_t ) / n );
    float t = ( ( dt * 0.5f ) + lower_t );
    float sum = 0;
    while( n )
    {
		vec3 first;
		EvaluateDerivative( t, first );
		sum += first.Length();
        t += dt;
		--n;
    }
    return( sum * dt );
}

void mat33::EvaluateRationalPolynomial( const float t, vec3& value ) const
{
	EvaluatePolynomial( t, value );
}

void mat33::EvaluateRationalPolynomial( const float t, vec3& value, vec3& first ) const
{
	EvaluatePolynomial( t, value, first );
	float a = value.z;
	float b = first.z;
	float c = ( a * a );
	first.x = ( ( first.x * a ) - ( value.x * b ) );
	first.y = ( ( first.y * a ) - ( value.y * b ) );
	first.z = c;
}

void mat33::EvaluateRationalDerivative( const float t, vec3& first ) const
{
	vec3 value;
	EvaluateRationalPolynomial( t, value, first );
}

float mat33::IntegrateRationalPolynomialLength( const float lower_t, const float upper_t ) const
{
	vec3 a, b, c, d;
    float delta = ( upper_t - lower_t );
	EvaluateRationalDerivative( ( ( delta * 0.125f ) + lower_t ), a );
	EvaluateRationalDerivative( ( ( delta * 0.375f ) + lower_t ), b );
	EvaluateRationalDerivative( ( ( delta * 0.625f ) + lower_t ), c );
	EvaluateRationalDerivative( ( ( delta * 0.875f ) + lower_t ), d );
	return( ( ( a.t_vec2().Length() / a.z ) + ( b.t_vec2().Length() / b.z ) + ( c.t_vec2().Length() / c.z ) + ( d.t_vec2().Length() / d.z ) ) * delta * 0.25f );
}

float mat33::IntegrateRationalPolynomialLength( const unsigned int steps, const float lower_t, const float upper_t ) const
{
    unsigned int n = ( ( steps < 4 ) ? 4 : steps );
    float dt = ( ( upper_t - lower_t ) / n );
    float t = ( ( dt * 0.5f ) + lower_t );
    float sum = 0;
    while( n )
    {
		vec3 first;
		EvaluateRationalDerivative( t, first );
		sum += ( first.t_vec2().Length() / first.z );
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
