
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vec3.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	3 element vector container and functions.
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

#include "plane.h"
#include "dquat.h"
#include "joint.h"
#include "mat33.h"
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
	namespace VEC3
	{
		const vec3		ORIGIN = { 0, 0, 0, 0 };
		const vec3		UNIT_X = { 1, 0, 0, 0 };
		const vec3		UNIT_Y = { 0, 1, 0, 0 };
		const vec3		UNIT_Z = { 0, 0, 1, 0 };
		const vec3		ZERO   = { 0, 0, 0, 0 };
		const vec3		ONE    = { 1, 1, 1, 0 };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    vec3
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool vec3::IsNan( void ) const
{
	return( ::IsNan( x ) || ::IsNan( y ) || ::IsNan( z ) );
}

bool vec3::IsReal( void ) const
{
	return( ::IsReal( x ) && ::IsReal( y ) && ::IsReal( z ) );
}

float vec3::Manhattan( void ) const
{
	return( fabsf( x ) + fabsf( y ) + fabsf( z ) );
}

float vec3::Chebyshev( void ) const
{
	float c = fabsf( x );
	float v = fabsf( y );
	if( c < v ) c = v;
	v = fabsf( z );
	if( c < v ) c = v; 
	return( c );
}

float vec3::Length( void ) const
{
	return( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) ) );
}

float vec3::SqrLen( void ) const
{
	return( ( x * x ) + ( y * y ) + ( z * z ) );
}

void vec3::Abs( void )
{
	x = fabsf( x );
	y = fabsf( y );
	z = fabsf( z );
}

void vec3::Sat( void )
{
	x = fsatf( x );
	y = fsatf( y );
	z = fsatf( z );
}

void vec3::Frac( void )
{
	x -= floorf( x );
	y -= floorf( y );
	z -= floorf( z );
}

void vec3::Floor( void )
{
	x = floorf( x );
	y = floorf( y );
	z = floorf( z );
}

void vec3::Ceil( void )
{
	x = ceilf( x );
	y = ceilf( y );
	z = ceilf( z );
}

float vec3::Dot( const vec3& dot ) const
{
	return( ( x * dot.x ) + ( y * dot.y ) + ( z * dot.z ) );
}

float vec3::Dot( const plane& dot ) const
{
	return( ( x * dot.x ) + ( y * dot.y ) + ( z * dot.z ) + dot.w );
}

float vec3::Triple( const vec3& u, const vec3& v ) const
{
	return(
		( x * ( ( u.y * v.z ) - ( u.z * v.y ) ) ) +
		( y * ( ( u.z * v.x ) - ( u.x * v.z ) ) ) +
		( z * ( ( u.x * v.y ) - ( u.y * v.x ) ) ) );
}

void vec3::Add( const vec3& add )
{
	x += add.x;
	y += add.y;
	z += add.z;
}

void vec3::Sub( const vec3& sub )
{
	x -= sub.x;
	y -= sub.y;
	z -= sub.z;
}

void vec3::Mul( const vec3& mul )
{
	x *= mul.x;
	y *= mul.y;
	z *= mul.z;
}

void vec3::Mul( const float mul )
{
	x *= mul;
	y *= mul;
	z *= mul;
}

void vec3::Mul( const quat& mul )
{
	SetMul( *this, mul );
}

void vec3::Mul( const dquat& mul )
{
	SetMul( *this, mul );
}

void vec3::Mul( const joint& mul )
{
	SetMul( *this, mul );
}

void vec3::Mul( const mat33& mul )
{
	SetMul( *this, mul );
}

void vec3::Mul( const mat43& mul )
{
	SetMul( *this, mul );
}

bool vec3::Mul( const mat44& mul )
{
	return( SetMul( *this, mul ) );
}

void vec3::Div( const vec3& div )
{
	ASSERT( ( fabsf( div.x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.y ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.z ) >= consts::FLOAT_MIN_RCP ) );
	x /= div.x;
	y /= div.y;
	z /= div.z;
}

void vec3::Div( const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	Mul( 1.0f / div );
}

void vec3::Lerp( const vec3& trg, const float t )
{
	SetLerp( *this, trg, t );
}

void vec3::Cross( const vec3& cross, vec3& trg ) const
{
	trg.SetCross( *this, cross );
}

void vec3::Negate( void )
{
	x = -x;
	y = -y;
	z = -z;
}

void vec3::Normalize( void )
{
	float n = ( 1.0f / ( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) ) + consts::FLOAT_MIN_NORM ) );
	x *= n;
	y *= n;
	z *= n;
}

void vec3::SetAbs( const vec3& src )
{
	x = fabsf( src.x );
	y = fabsf( src.y );
	z = fabsf( src.z );
}

void vec3::SetSat( const vec3& src )
{
	x = fsatf( src.x );
	y = fsatf( src.y );
	z = fsatf( src.z );
}

void vec3::SetFrac( const vec3& src )
{
	x -= floorf( src.x );
	y -= floorf( src.y );
	z -= floorf( src.z );
}

void vec3::SetFloor( const vec3& src )
{
	x = floorf( src.x );
	y = floorf( src.y );
	z = floorf( src.z );
}

void vec3::SetCeil( const vec3& src )
{
	x = ceilf( src.x );
	y = ceilf( src.y );
	z = ceilf( src.z );
}

void vec3::SetAdd( const vec3& src, const vec3& add )
{
	x = ( src.x + add.x );
	y = ( src.y + add.y );
	z = ( src.z + add.z );
}

void vec3::SetSub( const vec3& src, const vec3& sub )
{
	x = ( src.x - sub.x );
	y = ( src.y - sub.y );
	z = ( src.z - sub.z );
}

void vec3::SetMul( const vec3& src, const vec3& mul )
{
	x = ( src.x * mul.x );
	y = ( src.y * mul.y );
	z = ( src.z * mul.z );
}

void vec3::SetMul( const vec3& src, const float mul )
{
	x = ( src.x * mul );
	y = ( src.y * mul );
	z = ( src.z * mul );
}

void vec3::SetMul( const vec3& src, const quat& mul )
{
	mul.Transform( src, *this );
}

void vec3::SetMul( const vec3& src, const dquat& mul )
{
	mul.Transform( src, *this );
}

void vec3::SetMul( const vec3& src, const joint& mul )
{
	mul.Transform( src, *this );
}

void vec3::SetMul( const vec3& src, const mat33& mul )
{
	float _x = ( ( src.x * mul.x.x ) + ( src.y * mul.x.y ) + ( src.z * mul.x.z ) );
	float _y = ( ( src.x * mul.y.x ) + ( src.y * mul.y.y ) + ( src.z * mul.y.z ) );
	float _z = ( ( src.x * mul.z.x ) + ( src.y * mul.z.y ) + ( src.z * mul.z.z ) );
	x = _x;
	y = _y;
	z = _z;
}

void vec3::SetMul( const vec3& src, const mat43& mul )
{
	float _x = ( ( src.x * mul.x.x ) + ( src.y * mul.x.y ) + ( src.z * mul.x.z ) + mul.x.w );
	float _y = ( ( src.x * mul.y.x ) + ( src.y * mul.y.y ) + ( src.z * mul.y.z ) + mul.y.w );
	float _z = ( ( src.x * mul.z.x ) + ( src.y * mul.z.y ) + ( src.z * mul.z.z ) + mul.z.w );
	x = _x;
	y = _y;
	z = _z;
}

bool vec3::SetMul( const vec3& src, const mat44& mul )
{
	float _w = ( ( src.x * mul.w.x ) + ( src.y * mul.w.y ) + ( src.z * mul.w.z ) + mul.w.w );
	ASSERT( fabsf( _w ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( _w ) >= consts::FLOAT_MIN_RCP )
	{
		_w = ( 1.0f / _w );
		x = ( ( ( src.x * mul.x.x ) + ( src.y * mul.x.y ) + ( src.z * mul.x.z ) + mul.x.w ) * _w );
		y = ( ( ( src.x * mul.y.x ) + ( src.y * mul.y.y ) + ( src.z * mul.y.z ) + mul.y.w ) * _w );
		z = ( ( ( src.x * mul.z.x ) + ( src.y * mul.z.y ) + ( src.z * mul.z.z ) + mul.z.w ) * _w );
		return( true );
	}
	return( false );
}

bool vec3::SetMul( const vec4& src, const mat44& mul )
{
	float _w = ( ( src.x * mul.w.x ) + ( src.y * mul.w.y ) + ( src.z * mul.w.z ) + ( src.w * mul.w.w ) );
	ASSERT( fabsf( _w ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( _w ) >= consts::FLOAT_MIN_RCP )
	{
		_w = ( 1.0f / _w );
		x = ( ( ( src.x * mul.x.x ) + ( src.y * mul.x.y ) + ( src.z * mul.x.z ) + ( src.w * mul.x.w ) ) * _w );
		y = ( ( ( src.x * mul.y.x ) + ( src.y * mul.y.y ) + ( src.z * mul.y.z ) + ( src.w * mul.y.w ) ) * _w );
		z = ( ( ( src.x * mul.z.x ) + ( src.y * mul.z.y ) + ( src.z * mul.z.z ) + ( src.w * mul.z.w ) ) * _w );
		return( true );
	}
	return( false );
}

void vec3::SetDiv( const vec3& src, const vec3& div )
{
	ASSERT( ( fabsf( div.x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.y ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( div.z ) >= consts::FLOAT_MIN_RCP ) );
	x = ( src.x / div.x );
	y = ( src.y / div.y );
	z = ( src.z / div.z );
}

void vec3::SetDiv( const vec3& src, const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	SetMul( src, ( 1.0f / div ) );
}

void vec3::SetLerp( const vec3& src, const vec3& trg, const float t )
{
	float s = ( 1.0f - t );
	x = ( ( src.x * s ) + ( trg.x * t ) );
	y = ( ( src.y * s ) + ( trg.y * t ) );
	z = ( ( src.z * s ) + ( trg.z * t ) );
}

void vec3::SetCross( const vec3& src, const vec3& cross )
{
	float _x = ( ( src.y * cross.z ) - ( src.z * cross.y ) );
	float _y = ( ( src.z * cross.x ) - ( src.x * cross.z ) );
	float _z = ( ( src.x * cross.y ) - ( src.y * cross.x ) );
	x = _x;
	y = _y;
	z = _z;
}

//  sets cross(this,cross(b,c))
void vec3::SetTriple( const vec3& b, const vec3& c )
{
    float ab = Dot( b );
    float ac = Dot( c );
	float _x = ( ( b.x * ac ) - ( c.x * ab ) );
	float _y = ( ( b.y * ac ) - ( c.y * ab ) );
	float _z = ( ( b.z * ac ) - ( c.z * ab ) );
	x = _x;
	y = _y;
	z = _z;
}

//  sets cross(a,cross(b,c))
void vec3::SetTriple( const vec3& a, const vec3& b, const vec3& c )
{
    float ab = a.Dot( b );
    float ac = a.Dot( c );
	float _x = ( ( b.x * ac ) - ( c.x * ab ) );
	float _y = ( ( b.y * ac ) - ( c.y * ab ) );
	float _z = ( ( b.z * ac ) - ( c.z * ab ) );
	x = _x;
	y = _y;
	z = _z;
}

void vec3::SetNegate( const vec3& src )
{
	x = -src.x;
	y = -src.y;
	z = -src.z;
}

void vec3::SetNormalized( const vec3& src )
{
	Set( src );
	Normalize();
}

bool vec3::SetProjected( const vec4& src )
{
	ASSERT( fabsf( src.w ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( src.w ) >= consts::FLOAT_MIN_RCP )
    {
        float mul = ( 1.0f / src.w );
    	x = ( src.x * mul );
	    y = ( src.y * mul );
	    z = ( src.z * mul );
        return( true );
    }
    return( false );
}

void vec3::Set( const float vx, const float vy, const float vz )
{
	x = vx;
	y = vy;
	z = vz;
}

void vec3::Set( const vec3& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
}

void vec3::Get( float& vx, float& vy, float& vz ) const
{
	vx = x;
	vy = y;
	vz = z;
}

void vec3::Get( vec3& v ) const
{
	v.x = x;
	v.y = y;
	v.z = z;
}

bool vec3::PointIsInTriangle( const vec3& vtx1, const vec3& vtx2, const vec3& vtx3 ) const
{
	vec3 du, dv, dp;
	du.SetSub( vtx2, vtx1 );
	dv.SetSub( vtx3, vtx1 );
	dp.SetSub( *this, vtx1 );
	const float uv = du.Dot( dv );
	const float up = du.Dot( dp );
	const float vp = dv.Dot( dp );
	const float uu = du.SqrLen();
	const float cu = ( ( uu * vp ) - ( uv * up ) );
	if( cu >= 0 )
	{
		const float vv = dv.SqrLen();
		const float cv = ( ( vv * up ) - ( uv * vp ) );
		if( cv >= 0 )
		{
			if( ( cu + cv ) <= ( ( uu * vv ) - ( uv * uv ) ) )
			{
				return( true );
			}
		}
	}
	return( false );
}

float vec3::EvaluatePolynomial( const float t ) const
{
	return( ( ( ( x * t ) + y ) * t ) + z );
}

float vec3::EvaluatePolynomial( const float t, float& first ) const
{
	first = ( ( ( x + x ) * t ) + y );
	return( ( ( ( x * t ) + y ) * t ) + z );
}

float vec3::EvaluateDerivative( const float t ) const
{
	return( ( ( x + x ) * t ) + y );
}

float vec3::IntegratePolynomialLength( const float lower_t, const float upper_t ) const
{
    float delta = ( upper_t - lower_t );
	return( (
		fabsf( EvaluateDerivative( ( delta * 0.125f ) + lower_t ) ) +
		fabsf( EvaluateDerivative( ( delta * 0.375f ) + lower_t ) ) +
		fabsf( EvaluateDerivative( ( delta * 0.625f ) + lower_t ) ) +
		fabsf( EvaluateDerivative( ( delta * 0.875f ) + lower_t ) ) ) * delta * 0.25f );
}

float vec3::IntegratePolynomialLength( const unsigned int steps, const float lower_t, const float upper_t ) const
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
