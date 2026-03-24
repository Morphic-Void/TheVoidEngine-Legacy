
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   mat43.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	4 by 3 element matrix container and functions.
////
////    	Notes:
////
////    		Scale functions scale relative to the local axes.
////    		End rotation and scaling functions do not modify the translation.
////    		Twist, turn and tilt rotate relative to the world axes.
////    		Roll, pitch and yaw rotate relative to the local axes.
////    		Move translates relative to the world axes.
////    		Pan translates relative to the local axes.
////    		UnscaledPan translates relative to the local axes without scaling the translation.
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
#include "plane.h"
#include "dquat.h"
#include "joint.h"
#include "mat33.h"
#include "mat43.h"
#include "sphere.h"
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
	namespace MAT43
	{
		const mat43		IDENTITY = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 } };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    mat43
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool mat43::IsNan( void ) const
{
	return( x.IsNan() || y.IsNan() || z.IsNan() );
}

bool mat43::IsReal( void ) const
{
	return( x.IsReal() && y.IsReal() && z.IsReal() );
}

void mat43::PointAt( const vec3& from, const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - from.x ), ( at.y - from.y ), ( at.z - from.z ) };
	PointTo( from, to, up );
}

void mat43::PointTo( const vec3& from, const vec3& to, const vec3& up )
{
	t_mat33().PointTo( to, up );
	x.w = from.x;
	y.w = from.y;
	z.w = from.z;
}

void mat43::PointAt( const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - x.w ), ( at.y - y.w ), ( at.z - z.w ) };
	t_mat33().PointTo( to, up );
}

void mat43::PointTo( const vec3& to, const vec3& up )
{
	t_mat33().PointTo( to, up );
}

void mat43::LookAt( const vec3& from, const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - from.x ), ( at.y - from.y ), ( at.z - from.z ) };
	LookTo( from, to, up );
}

void mat43::LookTo( const vec3& from, const vec3& to, const vec3& up )
{
	t_mat33().LookTo( to, up );
	x.w = from.x;
	y.w = from.y;
	z.w = from.z;
}

void mat43::LookAt( const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - x.w ), ( at.y - y.w ), ( at.z - z.w ) };
	t_mat33().LookTo( to, up );
}

void mat43::LookTo( const vec3& to, const vec3& up )
{
	t_mat33().LookTo( to, up );
}

void mat43::Scale( const float scale )
{
	x.x *= scale;
	x.y *= scale;
	x.z *= scale;
	x.w *= scale;
	y.x *= scale;
	y.y *= scale;
	y.z *= scale;
	y.w *= scale;
	z.x *= scale;
	z.y *= scale;
	z.z *= scale;
	z.w *= scale;
}

void mat43::Twist( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec4 t = { x.x, x.y, x.z, x.w };
	x.x = ( ( x.x * c ) - ( y.x * s ) );
	x.y = ( ( x.y * c ) - ( y.y * s ) );
	x.z = ( ( x.z * c ) - ( y.z * s ) );
	x.w = ( ( x.w * c ) - ( y.w * s ) );
	y.x = ( ( y.x * c ) + ( t.x * s ) );
	y.y = ( ( y.y * c ) + ( t.y * s ) );
	y.z = ( ( y.z * c ) + ( t.z * s ) );
	y.w = ( ( y.w * c ) + ( t.w * s ) );
}

void mat43::Tilt( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec4 t = { y.x, y.y, y.z, y.w };
	y.x = ( ( y.x * c ) - ( z.x * s ) );
	y.y = ( ( y.y * c ) - ( z.y * s ) );
	y.z = ( ( y.z * c ) - ( z.z * s ) );
	y.w = ( ( y.w * c ) - ( z.w * s ) );
	z.x = ( ( z.x * c ) + ( t.x * s ) );
	z.y = ( ( z.y * c ) + ( t.y * s ) );
	z.z = ( ( z.z * c ) + ( t.z * s ) );
	z.w = ( ( z.w * c ) + ( t.w * s ) );
}

void mat43::Turn( const float radians )
{
	float s = sinf( radians );
	float c = cosf( radians );
	vec4 t = { z.x, z.y, z.z, z.w };
	z.x = ( ( z.x * c ) - ( x.x * s ) );
	z.y = ( ( z.y * c ) - ( x.y * s ) );
	z.z = ( ( z.z * c ) - ( x.z * s ) );
	z.w = ( ( z.w * c ) - ( x.w * s ) );
	x.x = ( ( x.x * c ) + ( t.x * s ) );
	x.y = ( ( x.y * c ) + ( t.y * s ) );
	x.z = ( ( x.z * c ) + ( t.z * s ) );
	x.w = ( ( x.w * c ) + ( t.w * s ) );
}

void mat43::Roll( const float radians )
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
	float n = ( 1.0f / ( sqrtf( ( x.z * x.z ) + ( y.z * y.z ) + ( z.z * z.z ) ) + consts::FLOAT_MIN_NORM ) );
	float i = ( 1.0f - c );
	float wx = ( x.z * n );
	float wy = ( y.z * n );
	float wz = ( z.z * n );
	float xy = ( wx * i );
	float yz = ( wy * i );
	float zx = ( wz * i );
	mat33 m;
	m.x.x = ( c + ( xy * wx ) );
	m.y.y = ( c + ( yz * wy ) );
	m.z.z = ( c + ( zx * wz ) );
	xy *= wy;
	yz *= wz;
	zx *= wx;
	wx *= s;
	wy *= s;
	wz *= s;
	m.x.y = ( xy - wz );
	m.y.x = ( xy + wz );
	m.y.z = ( yz - wx );
	m.z.y = ( yz + wx );
	m.z.x = ( zx - wy );
	m.x.z = ( zx + wy );
	wx = x.w;
	wy = y.w;
	wz = z.w;
	x.w = ( ( wx * m.x.x ) + ( wy * m.x.y ) + ( wz * m.x.z ) );
	y.w = ( ( wx * m.y.x ) + ( wy * m.y.y ) + ( wz * m.y.z ) );
	z.w = ( ( wx * m.z.x ) + ( wy * m.z.y ) + ( wz * m.z.z ) );
}

void mat43::Pitch( const float radians )
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
	float n = ( 1.0f / ( sqrtf( ( x.x * x.x ) + ( y.x * y.x ) + ( z.x * z.x ) ) + consts::FLOAT_MIN_NORM ) );
	float i = ( 1.0f - c );
	float wx = ( x.x * n );
	float wy = ( y.x * n );
	float wz = ( z.x * n );
	float xy = ( wx * i );
	float yz = ( wy * i );
	float zx = ( wz * i );
	mat33 m;
	m.x.x = ( c + ( xy * wx ) );
	m.y.y = ( c + ( yz * wy ) );
	m.z.z = ( c + ( zx * wz ) );
	xy *= wy;
	yz *= wz;
	zx *= wx;
	wx *= s;
	wy *= s;
	wz *= s;
	m.x.y = ( xy - wz );
	m.y.x = ( xy + wz );
	m.y.z = ( yz - wx );
	m.z.y = ( yz + wx );
	m.z.x = ( zx - wy );
	m.x.z = ( zx + wy );
	wx = x.w;
	wy = y.w;
	wz = z.w;
	x.w = ( ( wx * m.x.x ) + ( wy * m.x.y ) + ( wz * m.x.z ) );
	y.w = ( ( wx * m.y.x ) + ( wy * m.y.y ) + ( wz * m.y.z ) );
	z.w = ( ( wx * m.z.x ) + ( wy * m.z.y ) + ( wz * m.z.z ) );
}

void mat43::Yaw( const float radians )
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
	float n = ( 1.0f / ( sqrtf( ( x.y * x.y ) + ( y.y * y.y ) + ( z.y * z.y ) ) + consts::FLOAT_MIN_NORM ) );
	float i = ( 1.0f - c );
	float wx = ( x.y * n );
	float wy = ( y.y * n );
	float wz = ( z.y * n );
	float xy = ( wx * i );
	float yz = ( wy * i );
	float zx = ( wz * i );
	mat33 m;
	m.x.x = ( c + ( xy * wx ) );
	m.y.y = ( c + ( yz * wy ) );
	m.z.z = ( c + ( zx * wz ) );
	xy *= wy;
	yz *= wz;
	zx *= wx;
	wx *= s;
	wy *= s;
	wz *= s;
	m.x.y = ( xy - wz );
	m.y.x = ( xy + wz );
	m.y.z = ( yz - wx );
	m.z.y = ( yz + wx );
	m.z.x = ( zx - wy );
	m.x.z = ( zx + wy );
	wx = x.w;
	wy = y.w;
	wz = z.w;
	x.w = ( ( wx * m.x.x ) + ( wy * m.x.y ) + ( wz * m.x.z ) );
	y.w = ( ( wx * m.y.x ) + ( wy * m.y.y ) + ( wz * m.y.z ) );
	z.w = ( ( wx * m.z.x ) + ( wy * m.z.y ) + ( wz * m.z.z ) );
}

void mat43::EndScale( const float scale )
{
	t_mat33().Scale( scale );
}

void mat43::EndScale( const float scale_x, const float scale_y, const float scale_z )
{
	t_mat33().Scale( scale_x, scale_y, scale_z );
}

void mat43::EndTwist( const float radians )
{
	t_mat33().Twist( radians );
}

void mat43::EndTilt( const float radians )
{
	t_mat33().Tilt( radians );
}

void mat43::EndTurn( const float radians )
{
	t_mat33().Turn( radians );
}

void mat43::EndRoll( const float radians )
{
	t_mat33().Roll( radians );
}

void mat43::EndPitch( const float radians )
{
	t_mat33().Pitch( radians );
}

void mat43::EndYaw( const float radians )
{
	t_mat33().Yaw( radians );
}

void mat43::Move( const float move_x, const float move_y, const float move_z )
{
	x.w += move_x;
	y.w += move_y;
	z.w += move_z;
}

void mat43::Move( const vec3& move )
{
	x.w += move.x;
	y.w += move.y;
	z.w += move.z;
}

void mat43::Pan( const float pan_x, const float pan_y, const float pan_z )
{
	x.w += ( ( pan_x * x.x ) + ( pan_y * x.y ) + ( pan_z * x.z ) );
	y.w += ( ( pan_x * y.x ) + ( pan_y * y.y ) + ( pan_z * y.z ) );
	z.w += ( ( pan_x * z.x ) + ( pan_y * z.y ) + ( pan_z * z.z ) );
}

void mat43::Pan( const vec3& pan )
{
	x.w += ( ( pan.x * x.x ) + ( pan.y * x.y ) + ( pan.z * x.z ) );
	y.w += ( ( pan.x * y.x ) + ( pan.y * y.y ) + ( pan.z * y.z ) );
	z.w += ( ( pan.x * z.x ) + ( pan.y * z.y ) + ( pan.z * z.z ) );
}

void mat43::UnscaledPan( const float pan_x, const float pan_y, const float pan_z )
{
	vec3 pan = { pan_x, pan_y, pan_z };
	UnscaledPan( pan );
}

void mat43::UnscaledPan( const vec3& pan )
{
	vec3 move;
	move.x = ( ( pan.x * x.x ) + ( pan.y * x.y ) + ( pan.z * x.z ) );
	move.y = ( ( pan.x * y.x ) + ( pan.y * y.y ) + ( pan.z * y.z ) );
	move.z = ( ( pan.x * z.x ) + ( pan.y * z.y ) + ( pan.z * z.z ) );
	move.Mul( sqrtf( pan.Length() / ( move.Length() + consts::FLOAT_MIN_NORM ) ) );
	Move( move );
	//	alternative implementation:
	//		x.w += ( pan.Dot( x.t_vec3() ) / ( x.t_vec3().Length() + consts::FLOAT_MIN_NORM ) );
	//		y.w += ( pan.Dot( y.t_vec3() ) / ( y.t_vec3().Length() + consts::FLOAT_MIN_NORM ) );
	//		z.w += ( pan.Dot( z.t_vec3() ) / ( z.t_vec3().Length() + consts::FLOAT_MIN_NORM ) );
}

void mat43::Mul( const mat43& mul )
{
	mat43 tmp;
	tmp.SetMul( *this, mul );
	Set( tmp );
}

void mat43::Mul( const mat33& mul )
{
	mat43 tmp;
	tmp.SetMul( *this, mul );
	Set( tmp );
}

bool mat43::Invert( void )
{
	if( t_mat33().Invert() )
	{
		vec3 w = { x.w, y.w, z.w };
		x.w = -( ( w.x * x.x ) + ( w.y * x.y ) + ( w.z * x.z ) );
		y.w = -( ( w.x * y.x ) + ( w.y * y.y ) + ( w.z * y.z ) );
		z.w = -( ( w.x * z.x ) + ( w.y * z.y ) + ( w.z * z.z ) );
		return( true );
	}
	else
	{
		x.w = y.w = z.w = 0.0f;
		return( false );
	}
}

void mat43::OrthoInvert( void )
{
	t_mat33().Transpose();
	vec3 w = { x.w, y.w, z.w };
	x.w = -( ( w.x * x.x ) + ( w.y * x.y ) + ( w.z * x.z ) );
	y.w = -( ( w.x * y.x ) + ( w.y * y.y ) + ( w.z * y.z ) );
	z.w = -( ( w.x * z.x ) + ( w.y * z.y ) + ( w.z * z.z ) );
}

void mat43::Reflect( const plane& about )
{
	float s = -( 2.0f / ( ( about.x * about.x ) + ( about.y * about.y ) + ( about.z * about.z ) ) );
	{
		float r = ( ( ( x.x * about.x ) + ( y.x * about.y ) + ( z.x * about.z ) ) * s );
		x.x += ( about.x * r );
		y.x += ( about.y * r );
		z.x += ( about.z * r );
	}
	{
		float r = ( ( ( x.y * about.x ) + ( y.y * about.y ) + ( z.y * about.z ) ) * s );
		x.y += ( about.x * r );
		y.y += ( about.y * r );
		z.y += ( about.z * r );
	}
	{
		float r = ( ( ( x.z * about.x ) + ( y.z * about.y ) + ( z.z * about.z ) ) * s );
		x.z += ( about.x * r );
		y.z += ( about.y * r );
		z.z += ( about.z * r );
	}
	{
		float r = ( ( ( x.w * about.x ) + ( y.w * about.y ) + ( z.w * about.z ) + about.w ) * s );
		x.w += ( about.x * r );
		y.w += ( about.y * r );
		z.w += ( about.z * r );
	}
}

void mat43::SetMul( const mat43& src, const mat43& mul )
{
	mat43 tmp;
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
	Set( tmp );
}

void mat43::SetMul( const mat43& src, const mat33& mul )
{
	mat43 tmp;
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
	Set( tmp );
}

void mat43::SetMul( const mat33& src, const mat43& mul )
{
	mat43 tmp;
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
	Set( tmp );
}

void mat43::SetRemap( const aabb& src, const aabb& trg )
{
	vec3 mul;
	mul.SetSub( src.max, src.min );
	ASSERT( ( fabsf( mul.x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( mul.y ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( mul.z ) >= consts::FLOAT_MIN_RCP ) );
	if( ( fabsf( mul.x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( mul.y ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( mul.z ) >= consts::FLOAT_MIN_RCP ) )
	{
		mul.x = ( 1.0f / mul.x );
		mul.y = ( 1.0f / mul.y );
		mul.z = ( 1.0f / mul.z );
		x.x = ( ( trg.max.x - trg.min.x ) * mul.x );
		x.w = ( ( ( src.max.x * trg.min.x ) - ( src.min.x * trg.max.x ) ) * mul.x );
		y.y = ( ( trg.max.y - trg.min.y ) * mul.y );
		y.w = ( ( ( src.max.y * trg.min.y ) - ( src.min.y * trg.max.y ) ) * mul.y );
		z.z = ( ( trg.max.z - trg.min.z ) * mul.z );
		z.w = ( ( ( src.max.z * trg.min.z ) - ( src.min.z * trg.max.z ) ) * mul.z );
		y.x = z.x = z.y = x.y = x.z = y.z = 0.0f;
	}
	else
	{
		SetIdentity();
	}
}

void mat43::SetDelta( const mat43& src, const mat43& trg )
{
	SetInverse( src );
	Mul( trg );
}

bool mat43::SetInverse( const mat43& src )
{
	if( t_mat33().SetInverse( src.t_mat33() ) )
	{
		vec3 tmp = { src.x.w, src.y.w, src.z.w };
		x.w = -( ( tmp.x * x.x ) + ( tmp.y * x.y ) + ( tmp.z * x.z ) );
		y.w = -( ( tmp.x * y.x ) + ( tmp.y * y.y ) + ( tmp.z * y.z ) );
		z.w = -( ( tmp.x * z.x ) + ( tmp.y * z.y ) + ( tmp.z * z.z ) );
		return( true );
	}
	else
	{
		x.w = y.w = z.w = 0.0f;
		return( false );
	}
}

void mat43::SetOrthoInverse( const mat43& src )
{
	t_mat33().SetTranspose( src.t_mat33() );
	vec3 tmp = { src.x.w, src.y.w, src.z.w };
	x.w = -( ( tmp.x * x.x ) + ( tmp.y * x.y ) + ( tmp.z * x.z ) );
	y.w = -( ( tmp.x * y.x ) + ( tmp.y * y.y ) + ( tmp.z * y.z ) );
	z.w = -( ( tmp.x * z.x ) + ( tmp.y * z.y ) + ( tmp.z * z.z ) );
}

void mat43::SetReflect( const plane& about )
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
}

void mat43::SetReflected( const mat43& src, const plane& about )
{
	float s = -( 2.0f / ( ( about.x * about.x ) + ( about.y * about.y ) + ( about.z * about.z ) ) );
	{
		float r = ( ( ( src.x.x * about.x ) + ( src.y.x * about.y ) + ( src.z.x * about.z ) ) * s );
		x.x = ( src.x.x + ( about.x * r ) );
		y.x = ( src.y.x + ( about.y * r ) );
		z.x = ( src.z.x + ( about.z * r ) );
	}
	{
		float r = ( ( ( src.x.y * about.x ) + ( src.y.y * about.y ) + ( src.z.y * about.z ) ) * s );
		x.y = ( src.x.y + ( about.x * r ) );
		y.y = ( src.y.y + ( about.y * r ) );
		z.y = ( src.z.y + ( about.z * r ) );
	}
	{
		float r = ( ( ( src.x.z * about.x ) + ( src.y.z * about.y ) + ( src.z.z * about.z ) ) * s );
		x.z = ( src.x.z + ( about.x * r ) );
		y.z = ( src.y.z + ( about.y * r ) );
		z.z = ( src.z.z + ( about.z * r ) );
	}
	{
		float r = ( ( ( src.x.w * about.x ) + ( src.y.w * about.y ) + ( src.z.w * about.z ) + about.w ) * s );
		x.w = ( src.x.w + ( about.x * r ) );
		y.w = ( src.y.w + ( about.y * r ) );
		z.w = ( src.z.w + ( about.z * r ) );
	}
}

void mat43::SetBarycentricTetrahedron( const vec3& v1, const vec3& v2, const vec3& v3, const vec3& v4 )
{
	x.t_vec3().SetSub( v1, v4 );
	y.t_vec3().SetSub( v2, v4 );
	z.t_vec3().SetSub( v3, v4 );
	x.w = v4.x;
	y.w = v4.y;
	z.w = v4.z;
	Invert();
}

void mat43::Set( const mat43& m )
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
}

void mat43::Set( const dquat& d )
{
	d.Get( *this );
}

void mat43::Set( const joint& j )
{
	j.Get( *this );
}

void mat43::Set( const quat& q, const vec3& v )
{
	q.Get( t_mat33() );
	x.w = v.x;
	y.w = v.y;
	z.w = v.z;
}

void mat43::Set( const sphere& asphere )
{
	x.y = x.z = y.z = y.x = z.x = z.y = 0;
	x.x = y.y = z.z = asphere.r;
	x.w = asphere.x;
	y.w = asphere.y;
	z.w = asphere.z;
}

void mat43::Set( const aabb& abox )
{
	x.y = x.z = y.z = y.x = z.x = z.y = 0;
	x.x = ( ( abox.max.x - abox.min.x ) * 0.5f );
	y.y = ( ( abox.max.y - abox.min.y ) * 0.5f );
	z.z = ( ( abox.max.z - abox.min.z ) * 0.5f );
	x.w = ( ( abox.max.x + abox.min.x ) * 0.5f );
	y.w = ( ( abox.max.y + abox.min.y ) * 0.5f );
	z.w = ( ( abox.max.z + abox.min.z ) * 0.5f );
}

void mat43::Get( mat43& m ) const
{
	m.Set( *this );
}

void mat43::Get( dquat& d ) const
{
	d.Set( *this );
}

void mat43::Get( joint& j ) const
{
	j.Set( *this );
}

void mat43::Get( quat& q, vec3& v ) const
{
	q.Set( t_mat33() );
	v.x = x.w;
	v.y = y.w;
	v.z = z.w;
}

void mat43::GetDescOBB( box_verts& verts, box_faces& faces ) const
{
	GetDescOBB( faces );
	GetDescOBB( verts );
}

void mat43::GetDescOBB( box_verts& verts, box_faces& faces, const aabb& bounds ) const
{
	GetDescOBB( faces, bounds );
	GetDescOBB( verts, bounds );
}

void mat43::GetDescOBB( box_verts& verts ) const
{
	verts.verts[ BOX_VERT_NBL ].x = ( x.w - x.z - x.y - x.x );
	verts.verts[ BOX_VERT_NBL ].y = ( y.w - y.z - y.y - y.x );
	verts.verts[ BOX_VERT_NBL ].z = ( z.w - z.z - z.y - z.x );
	verts.verts[ BOX_VERT_NBR ].x = ( x.w - x.z - x.y + x.x );
	verts.verts[ BOX_VERT_NBR ].y = ( y.w - y.z - y.y + y.x );
	verts.verts[ BOX_VERT_NBR ].z = ( z.w - z.z - z.y + z.x );
	verts.verts[ BOX_VERT_NTL ].x = ( x.w - x.z + x.y - x.x );
	verts.verts[ BOX_VERT_NTL ].y = ( y.w - y.z + y.y - y.x );
	verts.verts[ BOX_VERT_NTL ].z = ( z.w - z.z + z.y - z.x );
	verts.verts[ BOX_VERT_NTR ].x = ( x.w - x.z + x.y + x.x );
	verts.verts[ BOX_VERT_NTR ].y = ( y.w - y.z + y.y + y.x );
	verts.verts[ BOX_VERT_NTR ].z = ( z.w - z.z + z.y + z.x );
	verts.verts[ BOX_VERT_FBL ].x = ( x.w + x.z - x.y - x.x );
	verts.verts[ BOX_VERT_FBL ].y = ( y.w + y.z - y.y - y.x );
	verts.verts[ BOX_VERT_FBL ].z = ( z.w + z.z - z.y - z.x );
	verts.verts[ BOX_VERT_FBR ].x = ( x.w + x.z - x.y + x.x );
	verts.verts[ BOX_VERT_FBR ].y = ( y.w + y.z - y.y + y.x );
	verts.verts[ BOX_VERT_FBR ].z = ( z.w + z.z - z.y + z.x );
	verts.verts[ BOX_VERT_FTL ].x = ( x.w + x.z + x.y - x.x );
	verts.verts[ BOX_VERT_FTL ].y = ( y.w + y.z + y.y - y.x );
	verts.verts[ BOX_VERT_FTL ].z = ( z.w + z.z + z.y - z.x );
	verts.verts[ BOX_VERT_FTR ].x = ( x.w + x.z + x.y + x.x );
	verts.verts[ BOX_VERT_FTR ].y = ( y.w + y.z + y.y + y.x );
	verts.verts[ BOX_VERT_FTR ].z = ( z.w + z.z + z.y + z.x );
}

void mat43::GetDescOBB( box_verts& verts, const aabb& bounds ) const
{
	mat33 min, max;
	min.x.x = ( x.x * bounds.min.x );
	min.x.y = ( y.x * bounds.min.x );
	min.x.z = ( z.x * bounds.min.x );
	min.y.x = ( x.y * bounds.min.y );
	min.y.y = ( y.y * bounds.min.y );
	min.y.z = ( z.y * bounds.min.y );
	min.z.x = ( x.z * bounds.min.z );
	min.z.y = ( y.z * bounds.min.z );
	min.z.z = ( z.z * bounds.min.z );
	max.x.x = ( x.x * bounds.max.x );
	max.x.y = ( y.x * bounds.max.x );
	max.x.z = ( z.x * bounds.max.x );
	max.y.x = ( x.y * bounds.max.y );
	max.y.y = ( y.y * bounds.max.y );
	max.y.z = ( z.y * bounds.max.y );
	max.z.x = ( x.z * bounds.max.z );
	max.z.y = ( y.z * bounds.max.z );
	max.z.z = ( z.z * bounds.max.z );
	verts.verts[ BOX_VERT_NBL ].x = ( x.w + min.z.x + min.y.x + min.x.x );
	verts.verts[ BOX_VERT_NBL ].y = ( y.w + min.z.y + min.y.y + min.x.y );
	verts.verts[ BOX_VERT_NBL ].z = ( z.w + min.z.z + min.y.z + min.x.z );
	verts.verts[ BOX_VERT_NBR ].x = ( x.w + min.z.x + min.y.x + max.x.x );
	verts.verts[ BOX_VERT_NBR ].y = ( y.w + min.z.y + min.y.y + max.x.y );
	verts.verts[ BOX_VERT_NBR ].z = ( z.w + min.z.z + min.y.z + max.x.z );
	verts.verts[ BOX_VERT_NTL ].x = ( x.w + min.z.x + max.y.x + min.x.x );
	verts.verts[ BOX_VERT_NTL ].y = ( y.w + min.z.y + max.y.y + min.x.y );
	verts.verts[ BOX_VERT_NTL ].z = ( z.w + min.z.z + max.y.z + min.x.z );
	verts.verts[ BOX_VERT_NTR ].x = ( x.w + min.z.x + max.y.x + max.x.x );
	verts.verts[ BOX_VERT_NTR ].y = ( y.w + min.z.y + max.y.y + max.x.y );
	verts.verts[ BOX_VERT_NTR ].z = ( z.w + min.z.z + max.y.z + max.x.z );
	verts.verts[ BOX_VERT_FBL ].x = ( x.w + max.z.x + min.y.x + min.x.x );
	verts.verts[ BOX_VERT_FBL ].y = ( y.w + max.z.y + min.y.y + min.x.y );
	verts.verts[ BOX_VERT_FBL ].z = ( z.w + max.z.z + min.y.z + min.x.z );
	verts.verts[ BOX_VERT_FBR ].x = ( x.w + max.z.x + min.y.x + max.x.x );
	verts.verts[ BOX_VERT_FBR ].y = ( y.w + max.z.y + min.y.y + max.x.y );
	verts.verts[ BOX_VERT_FBR ].z = ( z.w + max.z.z + min.y.z + max.x.z );
	verts.verts[ BOX_VERT_FTL ].x = ( x.w + max.z.x + max.y.x + min.x.x );
	verts.verts[ BOX_VERT_FTL ].y = ( y.w + max.z.y + max.y.y + min.x.y );
	verts.verts[ BOX_VERT_FTL ].z = ( z.w + max.z.z + max.y.z + min.x.z );
	verts.verts[ BOX_VERT_FTR ].x = ( x.w + max.z.x + max.y.x + max.x.x );
	verts.verts[ BOX_VERT_FTR ].y = ( y.w + max.z.y + max.y.y + max.x.y );
	verts.verts[ BOX_VERT_FTR ].z = ( z.w + max.z.z + max.y.z + max.x.z );
}

void mat43::GetDescOBB( box_faces& faces ) const
{
	vec3 v;
	float n, dx, dy, dz, dw;
	v.x = ( ( y.z * z.y ) - ( y.y * z.z ) );
	v.y = ( ( z.z * x.y ) - ( z.y * x.z ) );
	v.z = ( ( x.z * y.y ) - ( x.y * y.z ) );
	n = ( 1.0f / ( sqrtf( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) ) + consts::FLOAT_MIN_NORM ) );
	v.x *= n;
	v.y *= n;
	v.z *= n;
	dw = ( ( x.w * v.x ) + ( y.w * v.y ) + ( z.w * v.z ) );
	dx = ( ( x.x * v.x ) + ( y.x * v.y ) + ( z.x * v.z ) );
	faces.faces[ BOX_FACE_R ].x = -( faces.faces[ BOX_FACE_L ].x = v.x );
	faces.faces[ BOX_FACE_R ].y = -( faces.faces[ BOX_FACE_L ].y = v.y );
	faces.faces[ BOX_FACE_R ].z = -( faces.faces[ BOX_FACE_L ].z = v.z );
	faces.faces[ BOX_FACE_L ].w = ( dx - dw );
	faces.faces[ BOX_FACE_R ].w = ( dx + dw );
	v.x = ( ( y.x * z.z ) - ( y.z * z.x ) );
	v.y = ( ( z.x * x.z ) - ( z.z * x.x ) );
	v.z = ( ( x.x * y.z ) - ( x.z * y.x ) );
	n = ( 1.0f / ( sqrtf( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) ) + consts::FLOAT_MIN_NORM ) );
	v.x *= n;
	v.y *= n;
	v.z *= n;
	dw = ( ( x.w * v.x ) + ( y.w * v.y ) + ( z.w * v.z ) );
	dy = ( ( x.y * v.x ) + ( y.y * v.y ) + ( z.y * v.z ) );
	faces.faces[ BOX_FACE_T ].x = -( faces.faces[ BOX_FACE_B ].x = v.x );
	faces.faces[ BOX_FACE_T ].y = -( faces.faces[ BOX_FACE_B ].y = v.y );
	faces.faces[ BOX_FACE_T ].z = -( faces.faces[ BOX_FACE_B ].z = v.z );
	faces.faces[ BOX_FACE_B ].w = ( dy - dw );
	faces.faces[ BOX_FACE_T ].w = ( dy + dw );
	v.x = ( ( y.y * z.x ) - ( y.x * z.y ) );
	v.y = ( ( z.y * x.x ) - ( z.x * x.y ) );
	v.z = ( ( x.y * y.x ) - ( x.x * y.y ) );
	n = ( 1.0f / ( sqrtf( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) ) + consts::FLOAT_MIN_NORM ) );
	v.x *= n;
	v.y *= n;
	v.z *= n;
	dw = ( ( x.w * v.x ) + ( y.w * v.y ) + ( z.w * v.z ) );
	dz = ( ( x.z * v.x ) + ( y.z * v.y ) + ( z.z * v.z ) );
	faces.faces[ BOX_FACE_F ].x = -( faces.faces[ BOX_FACE_N ].x = v.x );
	faces.faces[ BOX_FACE_F ].y = -( faces.faces[ BOX_FACE_N ].y = v.y );
	faces.faces[ BOX_FACE_F ].z = -( faces.faces[ BOX_FACE_N ].z = v.z );
	faces.faces[ BOX_FACE_N ].w = ( dz - dw );
	faces.faces[ BOX_FACE_F ].w = ( dz + dw );
}

void mat43::GetDescOBB( box_faces& faces, const aabb& bounds ) const
{
	vec3 v;
	float n, dx, dy, dz, dw;
	v.x = ( ( y.z * z.y ) - ( y.y * z.z ) );
	v.y = ( ( z.z * x.y ) - ( z.y * x.z ) );
	v.z = ( ( x.z * y.y ) - ( x.y * y.z ) );
	n = ( 1.0f / ( sqrtf( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) ) + consts::FLOAT_MIN_NORM ) );
	v.x *= n;
	v.y *= n;
	v.z *= n;
	dw = ( ( x.w * v.x ) + ( y.w * v.y ) + ( z.w * v.z ) );
	dx = ( ( x.x * v.x ) + ( y.x * v.y ) + ( z.x * v.z ) );
	faces.faces[ BOX_FACE_R ].x = -( faces.faces[ BOX_FACE_L ].x = v.x );
	faces.faces[ BOX_FACE_R ].y = -( faces.faces[ BOX_FACE_L ].y = v.y );
	faces.faces[ BOX_FACE_R ].z = -( faces.faces[ BOX_FACE_L ].z = v.z );
	faces.faces[ BOX_FACE_L ].w = -( ( bounds.min.x * dx ) + dw );
	faces.faces[ BOX_FACE_R ].w = ( ( bounds.max.x * dx ) + dw );
	v.x = ( ( y.x * z.z ) - ( y.z * z.x ) );
	v.y = ( ( z.x * x.z ) - ( z.z * x.x ) );
	v.z = ( ( x.x * y.z ) - ( x.z * y.x ) );
	n = ( 1.0f / ( sqrtf( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) ) + consts::FLOAT_MIN_NORM ) );
	v.x *= n;
	v.y *= n;
	v.z *= n;
	dw = ( ( x.w * v.x ) + ( y.w * v.y ) + ( z.w * v.z ) );
	dy = ( ( x.y * v.x ) + ( y.y * v.y ) + ( z.y * v.z ) );
	faces.faces[ BOX_FACE_T ].x = -( faces.faces[ BOX_FACE_B ].x = v.x );
	faces.faces[ BOX_FACE_T ].y = -( faces.faces[ BOX_FACE_B ].y = v.y );
	faces.faces[ BOX_FACE_T ].z = -( faces.faces[ BOX_FACE_B ].z = v.z );
	faces.faces[ BOX_FACE_B ].w = -( ( bounds.min.y * dy ) + dw );
	faces.faces[ BOX_FACE_T ].w = ( ( bounds.max.y * dy ) + dw );
	v.x = ( ( y.y * z.x ) - ( y.x * z.y ) );
	v.y = ( ( z.y * x.x ) - ( z.x * x.y ) );
	v.z = ( ( x.y * y.x ) - ( x.x * y.y ) );
	n = ( 1.0f / ( sqrtf( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) ) + consts::FLOAT_MIN_NORM ) );
	v.x *= n;
	v.y *= n;
	v.z *= n;
	dw = ( ( x.w * v.x ) + ( y.w * v.y ) + ( z.w * v.z ) );
	dz = ( ( x.z * v.x ) + ( y.z * v.y ) + ( z.z * v.z ) );
	faces.faces[ BOX_FACE_F ].x = -( faces.faces[ BOX_FACE_N ].x = v.x );
	faces.faces[ BOX_FACE_F ].y = -( faces.faces[ BOX_FACE_N ].y = v.y );
	faces.faces[ BOX_FACE_F ].z = -( faces.faces[ BOX_FACE_N ].z = v.z );
	faces.faces[ BOX_FACE_N ].w = -( ( bounds.min.z * dz ) + dw );
	faces.faces[ BOX_FACE_F ].w = ( ( bounds.max.z * dz ) + dw );
}

void mat43::GetDescFrustum( box_verts& verts, box_faces& faces ) const
{
	GetRawDescFrustum( faces );
	faces.GetVerts( verts );
	faces.Normalize();
}

void mat43::GetDescFrustum( box_verts& verts, box_faces& faces, const aabb& bounds ) const
{
	GetRawDescFrustum( faces, bounds );
	faces.GetVerts( verts );
	faces.Normalize();
}

void mat43::GetDescFrustum( box_verts& verts ) const
{
	mat43 tmp;
	tmp.SetInverse( *this );
	tmp.GetDescOBB( verts );
}

void mat43::GetDescFrustum( box_verts& verts, const aabb& bounds ) const
{
	mat43 tmp;
	tmp.SetInverse( *this );
	tmp.GetDescOBB( verts, bounds );
}

void mat43::GetDescFrustum( box_faces& faces ) const
{
	GetRawDescFrustum( faces );
	faces.Normalize();
}

void mat43::GetDescFrustum( box_faces& faces, const aabb& bounds ) const
{
	GetRawDescFrustum( faces, bounds );
	faces.Normalize();
}

void mat43::GetRawDescFrustum( box_faces& faces ) const
{
	faces.faces[ BOX_FACE_L ].x = x.x;
	faces.faces[ BOX_FACE_L ].y = x.y;
	faces.faces[ BOX_FACE_L ].z = x.z;
	faces.faces[ BOX_FACE_L ].w = ( 1.0f + x.w );
	faces.faces[ BOX_FACE_R ].x = -x.x;
	faces.faces[ BOX_FACE_R ].y = -x.y;
	faces.faces[ BOX_FACE_R ].z = -x.z;
	faces.faces[ BOX_FACE_R ].w = ( 1.0f - x.w );
	faces.faces[ BOX_FACE_B ].x = y.x;
	faces.faces[ BOX_FACE_B ].y = y.y;
	faces.faces[ BOX_FACE_B ].z = y.z;
	faces.faces[ BOX_FACE_B ].w = ( 1.0f + y.w );
	faces.faces[ BOX_FACE_T ].x = -y.x;
	faces.faces[ BOX_FACE_T ].y = -y.y;
	faces.faces[ BOX_FACE_T ].z = -y.z;
	faces.faces[ BOX_FACE_T ].w = ( 1.0f - y.w );
	faces.faces[ BOX_FACE_N ].x = z.x;
	faces.faces[ BOX_FACE_N ].y = z.y;
	faces.faces[ BOX_FACE_N ].z = z.z;
	faces.faces[ BOX_FACE_N ].w = ( 1.0f + z.w );
	faces.faces[ BOX_FACE_F ].x = -z.x;
	faces.faces[ BOX_FACE_F ].y = -z.y;
	faces.faces[ BOX_FACE_F ].z = -z.z;
	faces.faces[ BOX_FACE_F ].w = ( 1.0f - z.w );
}

void mat43::GetRawDescFrustum( box_faces& faces, const aabb& bounds ) const
{
	float s, t;
	s = ( bounds.max.x - bounds.min.x );
	ASSERT( fabsf( s ) >= consts::FLOAT_MIN_RCP );
	s = ( ( fabsf( s ) >= consts::FLOAT_MIN_RCP ) ? ( 2.0f / s ) : 0.0f );
	t = ( ( ( ( bounds.max.x + bounds.min.x ) * 0.5f ) - x.w ) * s );
	faces.faces[ BOX_FACE_R ].x = -( faces.faces[ BOX_FACE_L ].x = ( x.x * s ) );
	faces.faces[ BOX_FACE_R ].y = -( faces.faces[ BOX_FACE_L ].y = ( x.y * s ) );
	faces.faces[ BOX_FACE_R ].z = -( faces.faces[ BOX_FACE_L ].z = ( x.z * s ) );
	faces.faces[ BOX_FACE_L ].w = ( 1.0f - t );
	faces.faces[ BOX_FACE_R ].w = ( 1.0f + t );
	s = ( bounds.max.y - bounds.min.y );
	ASSERT( fabsf( s ) >= consts::FLOAT_MIN_RCP );
	s = ( ( fabsf( s ) >= consts::FLOAT_MIN_RCP ) ? ( 2.0f / s ) : 0.0f );
	t = ( ( ( ( bounds.max.y + bounds.min.y ) * 0.5f ) - y.w ) * s );
	faces.faces[ BOX_FACE_T ].x = -( faces.faces[ BOX_FACE_B ].x = ( y.x * s ) );
	faces.faces[ BOX_FACE_T ].y = -( faces.faces[ BOX_FACE_B ].y = ( y.y * s ) );
	faces.faces[ BOX_FACE_T ].z = -( faces.faces[ BOX_FACE_B ].z = ( y.z * s ) );
	faces.faces[ BOX_FACE_B ].w = ( 1.0f - t );
	faces.faces[ BOX_FACE_T ].w = ( 1.0f + t );
	s = ( bounds.max.z - bounds.min.z );
	ASSERT( fabsf( s ) >= consts::FLOAT_MIN_RCP );
	s = ( ( fabsf( s ) >= consts::FLOAT_MIN_RCP ) ? ( 2.0f / s ) : 0.0f );
	t = ( ( ( ( bounds.max.z + bounds.min.z ) * 0.5f ) - z.w ) * s );
	faces.faces[ BOX_FACE_F ].x = -( faces.faces[ BOX_FACE_N ].x = ( z.x * s ) );
	faces.faces[ BOX_FACE_F ].y = -( faces.faces[ BOX_FACE_N ].y = ( z.y * s ) );
	faces.faces[ BOX_FACE_F ].z = -( faces.faces[ BOX_FACE_N ].z = ( z.z * s ) );
	faces.faces[ BOX_FACE_N ].w = ( 1.0f - t );
	faces.faces[ BOX_FACE_F ].w = ( 1.0f + t );
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
