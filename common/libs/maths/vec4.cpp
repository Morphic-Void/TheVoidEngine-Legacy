
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vec4.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	4 element vector container and functions.
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

#include "vec3.h"
#include "plane.h"
#include "dquat.h"
#include "joint.h"
#include "mat43.h"
#include "mat44.h"
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
	namespace VEC4
	{
		const vec4		ORIGIN = { 0, 0, 0, 1 };
		const vec4		UNIT_X = { 1, 0, 0, 0 };
		const vec4		UNIT_Y = { 0, 1, 0, 0 };
		const vec4		UNIT_Z = { 0, 0, 1, 0 };
		const vec4		UNIT_W = { 0, 0, 0, 1 };
		const vec4		ZERO   = { 0, 0, 0, 0 };
		const vec4		ONE    = { 1, 1, 1, 1 };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    vec4
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool vec4::IsNan( void ) const
{
	return( ::IsNan( x ) || ::IsNan( y ) || ::IsNan( z ) || ::IsNan( w ) );
}

bool vec4::IsReal( void ) const
{
	return( ::IsReal( x ) && ::IsReal( y ) && ::IsReal( z ) && ::IsReal( w ) );
}

float vec4::Manhattan( void ) const
{
	return( fabsf( x ) + fabsf( y ) + fabsf( z ) + fabsf( w ) );
}

float vec4::Chebyshev( void ) const
{
	float c = fabsf( x );
	float v = fabsf( y );
	if( c < v ) c = v;
	v = fabsf( z );
	if( c < v ) c = v; 
	v = fabsf( w );
	if( c < v ) c = v;
	return( c );
}

float vec4::Length( void ) const
{
	return( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ) );
}

float vec4::SqrLen( void ) const
{
	return( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
}

void vec4::Abs( void )
{
	x = fabsf( x );
	y = fabsf( y );
	z = fabsf( z );
	w = fabsf( w );
}

void vec4::Sat( void )
{
	x = fsatf( x );
	y = fsatf( y );
	z = fsatf( z );
	w = fsatf( w );
}

void vec4::Frac( void )
{
	x -= floorf( x );
	y -= floorf( y );
	z -= floorf( z );
	w -= floorf( w );
}

void vec4::Floor( void )
{
	x = floorf( x );
	y = floorf( y );
	z = floorf( z );
	w = floorf( w );
}

void vec4::Ceil( void )
{
	x = ceilf( x );
	y = ceilf( y );
	z = ceilf( z );
	w = ceilf( w );
}

float vec4::Dot( const vec3& dot ) const
{
	return( ( x * dot.x ) + ( y * dot.y ) + ( z * dot.z ) + w );
}

float vec4::Dot( const vec4& dot ) const
{
	return( ( x * dot.x ) + ( y * dot.y ) + ( z * dot.z ) + ( w * dot.w ) );
}

float vec4::Dot( const plane& dot ) const
{
	return( ( x * dot.x ) + ( y * dot.y ) + ( z * dot.z ) + ( w * dot.w ) );
}

void vec4::Add( const vec4& add )
{
	x += add.x;
	y += add.y;
	z += add.z;
	w += add.w;
}

void vec4::Sub( const vec4& sub )
{
	x -= sub.x;
	y -= sub.y;
	z -= sub.z;
	w -= sub.w;
}

void vec4::Mul( const vec4& mul )
{
	x *= mul.x;
	y *= mul.y;
	z *= mul.z;
	w *= mul.w;
}

void vec4::Mul( const float mul )
{
	x *= mul;
	y *= mul;
	z *= mul;
	w *= mul;
}

void vec4::Mul( const quat& mul )
{
	t_vec3().Mul( mul );
}

void vec4::Mul( const dquat& mul )
{
	SetMul( *this, mul );
}

void vec4::Mul( const joint& mul )
{
	SetMul( *this, mul );
}

void vec4::Mul( const mat33& mul )
{
	t_vec3().Mul( mul );
}

void vec4::Mul( const mat43& mul )
{
	SetMul( *this, mul );
}

void vec4::Mul( const mat44& mul )
{
	SetMul( *this, mul );
}

void vec4::Div( const vec4& div )
{
	ASSERT( ( fabsf( div.x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.y ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.z ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.w ) >= consts::FLOAT_MIN_RCP ) );
	x /= div.x;
	y /= div.y;
	z /= div.z;
	w /= div.w;
}

void vec4::Div( const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	Mul( 1.0f / div );
}

void vec4::Lerp( const vec4& trg, const float t )
{
	SetLerp( *this, trg, t );
}

void vec4::Negate( void )
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
}

void vec4::Normalize( void )
{
	float n = ( 1.0f / ( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ) + consts::FLOAT_MIN_NORM ) );
	x *= n;
	y *= n;
	z *= n;
	w *= n;
}

void vec4::SetAbs( const vec4& src )
{
	x = fabsf( src.x );
	y = fabsf( src.y );
	z = fabsf( src.z );
	w = fabsf( src.w );
}

void vec4::SetSat( const vec4& src )
{
	x = fsatf( src.x );
	y = fsatf( src.y );
	z = fsatf( src.z );
	w = fsatf( src.w );
}

void vec4::SetFrac( const vec4& src )
{
	x -= floorf( src.x );
	y -= floorf( src.y );
	z -= floorf( src.z );
	w -= floorf( src.w );
}

void vec4::SetFloor( const vec4& src )
{
	x = floorf( src.x );
	y = floorf( src.y );
	z = floorf( src.z );
	w = floorf( src.w );
}

void vec4::SetCeil( const vec4& src )
{
	x = ceilf( src.x );
	y = ceilf( src.y );
	z = ceilf( src.z );
	w = ceilf( src.w );
}

void vec4::SetAdd( const vec4& src, const vec4& add )
{
	x = ( src.x + add.x );
	y = ( src.y + add.y );
	z = ( src.z + add.z );
	w = ( src.w + add.w );
}

void vec4::SetSub( const vec4& src, const vec4& sub )
{
	x = ( src.x - sub.x );
	y = ( src.y - sub.y );
	z = ( src.z - sub.z );
	w = ( src.w - sub.w );
}

void vec4::SetMul( const vec4& src, const vec4& mul )
{
	x = ( src.x * mul.x );
	y = ( src.y * mul.y );
	z = ( src.z * mul.z );
	w = ( src.w * mul.w );
}

void vec4::SetMul( const vec4& src, const float mul )
{
	x = ( src.x * mul );
	y = ( src.y * mul );
	z = ( src.z * mul );
	w = ( src.w * mul );
}

void vec4::SetMul( const vec4& src, const quat& mul )
{
	mul.Transform( src.t_vec3(), t_vec3() );
	w = src.w;
}

void vec4::SetMul( const vec4& src, const dquat& mul )
{
	mul.Transform( src, *this );
}

void vec4::SetMul( const vec4& src, const joint& mul )
{
	mul.Transform( src, *this );
}

void vec4::SetMul( const vec4& src, const mat33& mul )
{
	t_vec3().SetMul( src.t_vec3(), mul );
	w = src.w;
}

void vec4::SetMul( const vec4& src, const mat43& mul )
{
	float _x = ( ( src.x * mul.x.x ) + ( src.y * mul.x.y ) + ( src.z * mul.x.z ) + ( src.w * mul.x.w ) );
	float _y = ( ( src.x * mul.y.x ) + ( src.y * mul.y.y ) + ( src.z * mul.y.z ) + ( src.w * mul.y.w ) );
	float _z = ( ( src.x * mul.z.x ) + ( src.y * mul.z.y ) + ( src.z * mul.z.z ) + ( src.w * mul.z.w ) );
	x = _x;
	y = _y;
	z = _z;
	w = src.w;
}

void vec4::SetMul( const vec4& src, const mat44& mul )
{
	float _x = ( ( src.x * mul.x.x ) + ( src.y * mul.x.y ) + ( src.z * mul.x.z ) + ( src.w * mul.x.w ) );
	float _y = ( ( src.x * mul.y.x ) + ( src.y * mul.y.y ) + ( src.z * mul.y.z ) + ( src.w * mul.y.w ) );
	float _z = ( ( src.x * mul.z.x ) + ( src.y * mul.z.y ) + ( src.z * mul.z.z ) + ( src.w * mul.z.w ) );
	float _w = ( ( src.x * mul.w.x ) + ( src.y * mul.w.y ) + ( src.z * mul.w.z ) + ( src.w * mul.w.w ) );
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

void vec4::SetMul( const vec3& src, const mat44& mul )
{
	float _x = ( ( src.x * mul.x.x ) + ( src.y * mul.x.y ) + ( src.z * mul.x.z ) + mul.x.w );
	float _y = ( ( src.x * mul.y.x ) + ( src.y * mul.y.y ) + ( src.z * mul.y.z ) + mul.y.w );
	float _z = ( ( src.x * mul.z.x ) + ( src.y * mul.z.y ) + ( src.z * mul.z.z ) + mul.z.w );
	float _w = ( ( src.x * mul.w.x ) + ( src.y * mul.w.y ) + ( src.z * mul.w.z ) + mul.w.w );
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

void vec4::SetDiv( const vec4& src, const vec4& div )
{
	ASSERT( ( fabsf( div.x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.y ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.z ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.w ) >= consts::FLOAT_MIN_RCP ) );
	x = ( src.x / div.x );
	y = ( src.y / div.y );
	z = ( src.z / div.z );
	w = ( src.w / div.w );
}

void vec4::SetDiv( const vec4& src, const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	SetMul( src, ( 1.0f / div ) );
}

void vec4::SetLerp( const vec4& src, const vec4& trg, const float t )
{
	float s = ( 1.0f - t );
	x = ( ( src.x * s ) + ( trg.x * t ) );
	y = ( ( src.y * s ) + ( trg.y * t ) );
	z = ( ( src.z * s ) + ( trg.z * t ) );
	w = ( ( src.w * s ) + ( trg.w * t ) );
}

void vec4::SetNegate( const vec4& src )
{
	x = -src.x;
	y = -src.y;
	z = -src.z;
	w = -src.w;
}

void vec4::SetNormalized( const vec4& src )
{
	Set( src );
	Normalize();
}

void vec4::Set( const float vx, const float vy, const float vz, const float vw )
{
	x = vx;
	y = vy;
	z = vz;
	w = vw;
}

void vec4::Set( const vec4& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

void vec4::Set( const vec3& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = 1.0f;
}

void vec4::Get( float& vx, float& vy, float& vz, float& vw ) const
{
	vx = x;
	vy = y;
	vz = z;
	vw = w;
}

void vec4::Get( vec4& v ) const
{
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
}

void vec4::Get( vec3& v ) const
{
	ASSERT( fabsf( w ) >= consts::FLOAT_MIN_RCP );
	float n = ( 1.0f / w );
	v.x = ( x * n );
	v.y = ( y * n );
	v.z = ( z * n );
}

float vec4::EvaluatePolynomial( const float t ) const
{
	return( ( ( ( ( ( x * t ) + y ) * t ) + z ) * t ) + w );
}

float vec4::EvaluatePolynomial( const float t, float& first ) const
{
	first = ( ( ( ( x * t * 3.0f ) + y + y ) * t ) + z );
	return( ( ( ( ( ( x * t ) + y ) * t ) + z ) * t ) + w );
}

float vec4::EvaluatePolynomial( const float t, float& first, float& second ) const
{
	first = ( ( ( ( x * t * 3.0f ) + y + y ) * t ) + z );
	second = ( ( x * t * 6.0f ) + y + y );
	return( ( ( ( ( ( x * t ) + y ) * t ) + z ) * t ) + w );
}

float vec4::EvaluateDerivative( const float t ) const
{
	return( ( ( ( x * t * 3.0f ) + y + y ) * t ) + z );
}

void vec4::EvaluateDerivatives( const float t, float& first, float& second ) const
{
	first = ( ( ( ( x * t * 3.0f ) + y + y ) * t ) + z );
	second = ( ( x * t * 6.0f ) + y + y );
}

float vec4::IntegratePolynomialLength( const float lower_t, const float upper_t ) const
{
    float delta = ( upper_t - lower_t );
	return( (
		fabsf( EvaluateDerivative( ( delta * 0.125f ) + lower_t ) ) +
		fabsf( EvaluateDerivative( ( delta * 0.375f ) + lower_t ) ) +
		fabsf( EvaluateDerivative( ( delta * 0.625f ) + lower_t ) ) +
		fabsf( EvaluateDerivative( ( delta * 0.875f ) + lower_t ) ) ) * delta * 0.25f );
}

float vec4::IntegratePolynomialLength( const unsigned int steps, const float lower_t, const float upper_t ) const
{
    unsigned int n = ( ( steps < 4 ) ? 4 : steps );
    float dt = ( ( upper_t - lower_t ) / n );
    float t = ( ( dt * 0.5f ) + lower_t );
    float sum = 0;
    while( n )
    {
		sum += fabsf( EvaluateDerivative( t ) );
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
