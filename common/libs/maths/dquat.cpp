
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   dquat.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Dual quaternion container and functions.
////
////    	Notes:
////
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

#include "dquat.h"
#include "mat43.h"
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
	namespace DQUAT
	{
		const dquat		IDENTITY = { { 0, 0, 0, 1 }, { 0, 0, 0, 0 } };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    dquat
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool dquat::IsNan( void ) const
{
	return( r.IsNan() || e.IsNan() );
}

bool dquat::IsReal( void ) const
{
	return( r.IsReal() && e.IsReal() );
}

void dquat::Transform( const vec3& src, vec3& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	{
		float xx = ( r.x * r.x );
		float yy = ( r.y * r.y );
		float zz = ( r.z * r.z );
		float ww = ( r.w * r.w );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( r.x + r.x );
		float y2 = ( r.y + r.y );
		float z2 = ( r.z + r.z );
		{
			float yz = ( r.y * z2 );
			float wx = ( r.w * x2 );
			tz += ( sy * ( yz + wx ) );
			ty += ( sz * ( yz - wx ) );
		}
		{
			float zx = ( r.z * x2 );
			float wy = ( r.w * y2 );
			tx += ( sz * ( zx + wy ) );
			tz += ( sx * ( zx - wy ) );
		}
		{
			float xy = ( r.x * y2 );
			float wz = ( r.w * z2 );
			ty += ( sx * ( xy + wz ) );
			tx += ( sy * ( xy - wz ) );
		}
	}
	tx += ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * 2.0f );
	ty += ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * 2.0f );
	tz += ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * 2.0f );
	trg.x = tx;
	trg.y = ty;
	trg.z = tz;
}

void dquat::Transform( const vec4& src, vec4& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	{
		float xx = ( r.x * r.x );
		float yy = ( r.y * r.y );
		float zz = ( r.z * r.z );
		float ww = ( r.w * r.w );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( r.x + r.x );
		float y2 = ( r.y + r.y );
		float z2 = ( r.z + r.z );
		{
			float yz = ( r.y * z2 );
			float wx = ( r.w * x2 );
			tz += ( sy * ( yz + wx ) );
			ty += ( sz * ( yz - wx ) );
		}
		{
			float zx = ( r.z * x2 );
			float wy = ( r.w * y2 );
			tx += ( sz * ( zx + wy ) );
			tz += ( sx * ( zx - wy ) );
		}
		{
			float xy = ( r.x * y2 );
			float wz = ( r.w * z2 );
			ty += ( sx * ( xy + wz ) );
			tx += ( sy * ( xy - wz ) );
		}
	}
	tx += ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * src.w * 2.0f );
	ty += ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * src.w * 2.0f );
	tz += ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * src.w * 2.0f );
	trg.x = tx;
	trg.y = ty;
	trg.z = tz;
	trg.w = src.w;
}

void dquat::UnscaledTransform( const vec3& src, vec3& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	float ll;
	{
		float xx = ( r.x * r.x );
		float yy = ( r.y * r.y );
		float zz = ( r.z * r.z );
		float ww = ( r.w * r.w );
		ll = ( xx + yy + zz + ww );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( r.x + r.x );
		float y2 = ( r.y + r.y );
		float z2 = ( r.z + r.z );
		{
			float yz = ( r.y * z2 );
			float wx = ( r.w * x2 );
			tz += ( sy * ( yz + wx ) );
			ty += ( sz * ( yz - wx ) );
		}
		{
			float zx = ( r.z * x2 );
			float wy = ( r.w * y2 );
			tx += ( sz * ( zx + wy ) );
			tz += ( sx * ( zx - wy ) );
		}
		{
			float xy = ( r.x * y2 );
			float wz = ( r.w * z2 );
			ty += ( sx * ( xy + wz ) );
			tx += ( sy * ( xy - wz ) );
		}
	}
	ll = ( 1.0f / ( ll + consts::FLOAT_MIN_NORM ) );
	tx += ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * 2.0f );
	ty += ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * 2.0f );
	tz += ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * 2.0f );
	trg.x = ( tx * ll );
	trg.y = ( ty * ll );
	trg.z = ( tz * ll );
}

void dquat::UnscaledTransform( const vec4& src, vec4& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	float ll;
	{
		float xx = ( r.x * r.x );
		float yy = ( r.y * r.y );
		float zz = ( r.z * r.z );
		float ww = ( r.w * r.w );
		ll = ( xx + yy + zz + ww );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( r.x + r.x );
		float y2 = ( r.y + r.y );
		float z2 = ( r.z + r.z );
		{
			float yz = ( r.y * z2 );
			float wx = ( r.w * x2 );
			tz += ( sy * ( yz + wx ) );
			ty += ( sz * ( yz - wx ) );
		}
		{
			float zx = ( r.z * x2 );
			float wy = ( r.w * y2 );
			tx += ( sz * ( zx + wy ) );
			tz += ( sx * ( zx - wy ) );
		}
		{
			float xy = ( r.x * y2 );
			float wz = ( r.w * z2 );
			ty += ( sx * ( xy + wz ) );
			tx += ( sy * ( xy - wz ) );
		}
	}
	ll = ( 1.0f / ( ll + consts::FLOAT_MIN_NORM ) );
	tx += ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * src.w * 2.0f );
	ty += ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * src.w * 2.0f );
	tz += ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * src.w * 2.0f );
	trg.x = ( tx * ll );
	trg.y = ( ty * ll );
	trg.z = ( tz * ll );
	trg.w = src.w;
}

void dquat::PointAt( const vec3& from, const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - from.x ), ( at.y - from.y ), ( at.z - from.z ) };
	PointTo( from, to, up );
}

void dquat::PointTo( const vec3& from, const vec3& to, const vec3& up )
{
	r.PointTo( to, up );
	e.x =  ( ( ( r.w * from.x ) - ( r.y * from.z ) + ( r.z * from.y ) ) * 0.5f );
	e.y =  ( ( ( r.w * from.y ) - ( r.z * from.x ) + ( r.x * from.z ) ) * 0.5f );
	e.z =  ( ( ( r.w * from.z ) - ( r.x * from.y ) + ( r.y * from.x ) ) * 0.5f );
	e.w = -( ( ( r.z * from.z ) + ( r.y * from.y ) + ( r.x * from.x ) ) * 0.5f );
}

void dquat::PointAt( const vec3& at, const vec3& up )
{
	vec3 from;
	GetTranslation( from );
	PointAt( from, at, up );
}

void dquat::PointTo( const vec3& to, const vec3& up )
{
	vec3 from;
	GetTranslation( from );
	PointTo( from, to, up );
}

void dquat::LookAt( const vec3& from, const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - from.x ), ( at.y - from.y ), ( at.z - from.z ) };
	LookTo( from, to, up );
}

void dquat::LookTo( const vec3& from, const vec3& to, const vec3& up )
{
	r.LookTo( to, up );
	e.x =  ( ( ( r.w * from.x ) - ( r.y * from.z ) + ( r.z * from.y ) ) * 0.5f );
	e.y =  ( ( ( r.w * from.y ) - ( r.z * from.x ) + ( r.x * from.z ) ) * 0.5f );
	e.z =  ( ( ( r.w * from.z ) - ( r.x * from.y ) + ( r.y * from.x ) ) * 0.5f );
	e.w = -( ( ( r.z * from.z ) + ( r.y * from.y ) + ( r.x * from.x ) ) * 0.5f );
}

void dquat::LookAt( const vec3& at, const vec3& up )
{
	vec3 from;
	GetTranslation( from );
	LookAt( from, at, up );
}

void dquat::LookTo( const vec3& to, const vec3& up )
{
	vec3 from;
	GetTranslation( from );
	LookTo( from, to, up );
}

void dquat::Scale( const float scale )
{
	ASSERT( scale > 0.0f );
	if( scale > 0.0f )
	{
		float n = sqrtf( scale );
		r.x *= n;
		r.y *= n;
		r.z *= n;
		r.w *= n;
		e.x *= n;
		e.y *= n;
		e.z *= n;
		e.w *= n;
	}
}

void dquat::Twist( const float radians )
{	//	rotate about world Z
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	dquat d;
	d.r.x = ( r.x * s );
	d.r.y = ( r.y * s );
	d.r.z = ( r.z * s );
	d.r.w = ( r.w * s );
	d.e.x = ( e.x * s );
	d.e.y = ( e.y * s );
	d.e.z = ( e.z * s );
	d.e.w = ( e.w * s );
	r.x = ( ( r.x * c ) - d.r.y );
	r.y = ( ( r.y * c ) + d.r.x );
	r.z = ( ( r.z * c ) + d.r.w );
	r.w = ( ( r.w * c ) - d.r.z );
	e.x = ( ( e.x * c ) - d.e.y );
	e.y = ( ( e.y * c ) + d.e.x );
	e.z = ( ( e.z * c ) + d.e.w );
	e.w = ( ( e.w * c ) - d.e.z );
}
 
void dquat::Tilt( const float radians )
{	//	rotate about world X
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	dquat d;
	d.r.x = ( r.x * s );
	d.r.y = ( r.y * s );
	d.r.z = ( r.z * s );
	d.r.w = ( r.w * s );
	d.e.x = ( e.x * s );
	d.e.y = ( e.y * s );
	d.e.z = ( e.z * s );
	d.e.w = ( e.w * s );
	r.x = ( ( r.x * c ) + d.r.w );
	r.y = ( ( r.y * c ) - d.r.z );
	r.z = ( ( r.z * c ) + d.r.y );
	r.w = ( ( r.w * c ) - d.r.x );
	e.x = ( ( e.x * c ) + d.e.w );
	e.y = ( ( e.y * c ) - d.e.z );
	e.z = ( ( e.z * c ) + d.e.y );
	e.w = ( ( e.w * c ) - d.e.x );
}
 
void dquat::Turn( const float radians )
{	//	rotate about world Y
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	dquat d;
	d.r.x = ( r.x * s );
	d.r.y = ( r.y * s );
	d.r.z = ( r.z * s );
	d.r.w = ( r.w * s );
	d.e.x = ( e.x * s );
	d.e.y = ( e.y * s );
	d.e.z = ( e.z * s );
	d.e.w = ( e.w * s );
	r.x = ( ( r.x * c ) + d.r.z );
	r.y = ( ( r.y * c ) + d.r.w );
	r.z = ( ( r.z * c ) - d.r.x );
	r.w = ( ( r.w * c ) - d.r.y );
	e.x = ( ( e.x * c ) + d.e.z );
	e.y = ( ( e.y * c ) + d.e.w );
	e.z = ( ( e.z * c ) - d.e.x );
	e.w = ( ( e.w * c ) - d.e.y );
}

void dquat::Roll( const float radians )
{	//	rotate about local Z
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	vec3 t;
	t.x = ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) );
	t.y = ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) );
	t.z = ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) );
	quat q;
	q.w = r.SqrLen();
	q.z = ( 1.0f / ( ( q.w * q.w ) + consts::FLOAT_MIN_NORM ) );
	q.w = ( q.z * c );
	q.z = ( q.z * s );
	q.x = q.y = ( q.z * 2 );
	q.x *= ( ( r.z * r.x ) + ( r.w * r.y ) );
	q.y *= ( ( r.y * r.z ) - ( r.w * r.x ) );
	q.z *= ( ( r.w * r.w ) + ( r.z * r.z ) - ( r.y * r.y ) - ( r.x * r.x ) );
	q.Transform( t, t );
	q.x = ( r.x * s );
	q.y = ( r.y * s );
	q.z = ( r.z * s );
	q.w = ( r.w * s );
	r.x = ( ( r.x * c ) + q.y );
	r.y = ( ( r.y * c ) - q.x );
	r.z = ( ( r.z * c ) + q.w );
	r.w = ( ( r.w * c ) - q.z );
	e.x =  ( ( r.w * t.x ) - ( r.y * t.z ) + ( r.z * t.y ) );
	e.y =  ( ( r.w * t.y ) - ( r.z * t.x ) + ( r.x * t.z ) );
	e.z =  ( ( r.w * t.z ) - ( r.x * t.y ) + ( r.y * t.x ) );
	e.w = -( ( r.z * t.z ) + ( r.y * t.y ) + ( r.x * t.x ) );
}
 
void dquat::Pitch( const float radians )
{	//	rotate about local X
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	vec3 t;
	t.x = ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) );
	t.y = ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) );
	t.z = ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) );
	quat q;
	q.w = r.SqrLen();
	q.x = ( 1.0f / ( ( q.w * q.w ) + consts::FLOAT_MIN_NORM ) );
	q.w = ( q.x * c );
	q.x = ( q.x * s );
	q.y = q.z = ( q.x * 2 );
	q.x *= ( ( r.w * r.w ) - ( r.z * r.z ) - ( r.y * r.y ) + ( r.x * r.x ) );
	q.y *= ( ( r.x * r.y ) + ( r.w * r.z ) );
	q.z *= ( ( r.z * r.x ) - ( r.w * r.y ) );
	q.Transform( t, t );
	q.x = ( r.x * s );
	q.y = ( r.y * s );
	q.z = ( r.z * s );
	q.w = ( r.w * s );
	r.x = ( ( r.x * c ) + q.w );
	r.y = ( ( r.y * c ) + q.z );
	r.z = ( ( r.z * c ) - q.y );
	r.w = ( ( r.w * c ) - q.x );
	e.x =  ( ( r.w * t.x ) - ( r.y * t.z ) + ( r.z * t.y ) );
	e.y =  ( ( r.w * t.y ) - ( r.z * t.x ) + ( r.x * t.z ) );
	e.z =  ( ( r.w * t.z ) - ( r.x * t.y ) + ( r.y * t.x ) );
	e.w = -( ( r.z * t.z ) + ( r.y * t.y ) + ( r.x * t.x ) );
}

void dquat::Yaw( const float radians )
{	//	rotate about local Y
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	vec3 t;
	t.x = ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) );
	t.y = ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) );
	t.z = ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) );
	quat q;
	q.w = r.SqrLen();
	q.y = ( 1.0f / ( ( q.w * q.w ) + consts::FLOAT_MIN_NORM ) );
	q.w = ( q.y * c );
	q.y = ( q.y * s );
	q.z = q.x = ( q.y * 2 );
	q.x *= ( ( r.x * r.y ) - ( r.w * r.z ) );
	q.y *= ( ( r.w * r.w ) - ( r.z * r.z ) + ( r.y * r.y ) - ( r.x * r.x ) );
	q.z *= ( ( r.y * r.z ) + ( r.w * r.x ) );
	q.Transform( t, t );
	q.x = ( r.x * s );
	q.y = ( r.y * s );
	q.z = ( r.z * s );
	q.w = ( r.w * s );
	r.x = ( ( r.x * c ) - q.z );
	r.y = ( ( r.y * c ) + q.w );
	r.z = ( ( r.z * c ) + q.x );
	r.w = ( ( r.w * c ) - q.y );
	e.x =  ( ( r.w * t.x ) - ( r.y * t.z ) + ( r.z * t.y ) );
	e.y =  ( ( r.w * t.y ) - ( r.z * t.x ) + ( r.x * t.z ) );
	e.z =  ( ( r.w * t.z ) - ( r.x * t.y ) + ( r.y * t.x ) );
	e.w = -( ( r.z * t.z ) + ( r.y * t.y ) + ( r.x * t.x ) );
}

void dquat::EndScale( const float scale )
{
	ASSERT( scale > 0.0f );
	if( scale > 0.0f )
	{
		float n = sqrtf( scale );
		r.x *= n;
		r.y *= n;
		r.z *= n;
		r.w *= n;
		n = ( 1.0f / n );
		e.x *= n;
		e.y *= n;
		e.z *= n;
		e.w *= n;
	}
}

void dquat::EndTwist( const float radians )
{	//	rotate about world Z
	float n = ( 1.0f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	vec3 t;
	t.x = ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * n );
	t.y = ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * n );
	t.z = ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * n );
	quat q;
	q.x = ( r.x * s );
	q.y = ( r.y * s );
	q.z = ( r.z * s );
	q.w = ( r.w * s );
	r.x = ( ( r.x * c ) - q.y );
	r.y = ( ( r.y * c ) + q.x );
	r.z = ( ( r.z * c ) + q.w );
	r.w = ( ( r.w * c ) - q.z );
	e.x =  ( ( r.w * t.x ) - ( r.y * t.z ) + ( r.z * t.y ) );
	e.y =  ( ( r.w * t.y ) - ( r.z * t.x ) + ( r.x * t.z ) );
	e.z =  ( ( r.w * t.z ) - ( r.x * t.y ) + ( r.y * t.x ) );
	e.w = -( ( r.z * t.z ) + ( r.y * t.y ) + ( r.x * t.x ) );
}
 
void dquat::EndTilt( const float radians )
{	//	rotate about world X
	float n = ( 1.0f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	vec3 t;
	t.x = ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * n );
	t.y = ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * n );
	t.z = ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * n );
	quat q;
	q.x = ( r.x * s );
	q.y = ( r.y * s );
	q.z = ( r.z * s );
	q.w = ( r.w * s );
	r.x = ( ( r.x * c ) + q.w );
	r.y = ( ( r.y * c ) - q.z );
	r.z = ( ( r.z * c ) + q.y );
	r.w = ( ( r.w * c ) - q.x );
	e.x =  ( ( r.w * t.x ) - ( r.y * t.z ) + ( r.z * t.y ) );
	e.y =  ( ( r.w * t.y ) - ( r.z * t.x ) + ( r.x * t.z ) );
	e.z =  ( ( r.w * t.z ) - ( r.x * t.y ) + ( r.y * t.x ) );
	e.w = -( ( r.z * t.z ) + ( r.y * t.y ) + ( r.x * t.x ) );
}
 
void dquat::EndTurn( const float radians )
{	//	rotate about world Y
	float n = ( 1.0f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	vec3 t;
	t.x = ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * n );
	t.y = ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * n );
	t.z = ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * n );
	quat q;
	q.x = ( r.x * s );
	q.y = ( r.y * s );
	q.z = ( r.z * s );
	q.w = ( r.w * s );
	r.x = ( ( r.x * c ) + q.z );
	r.y = ( ( r.y * c ) + q.w );
	r.z = ( ( r.z * c ) - q.x );
	r.w = ( ( r.w * c ) - q.y );
	e.x =  ( ( r.w * t.x ) - ( r.y * t.z ) + ( r.z * t.y ) );
	e.y =  ( ( r.w * t.y ) - ( r.z * t.x ) + ( r.x * t.z ) );
	e.z =  ( ( r.w * t.z ) - ( r.x * t.y ) + ( r.y * t.x ) );
	e.w = -( ( r.z * t.z ) + ( r.y * t.y ) + ( r.x * t.x ) );
}

void dquat::EndRoll( const float radians )
{	//	rotate about local Z
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	dquat d;
	d.r.x = ( r.x * s );
	d.r.y = ( r.y * s );
	d.r.z = ( r.z * s );
	d.r.w = ( r.w * s );
	d.e.x = ( e.x * s );
	d.e.y = ( e.y * s );
	d.e.z = ( e.z * s );
	d.e.w = ( e.w * s );
	r.x = ( ( r.x * c ) + d.r.y );
	r.y = ( ( r.y * c ) - d.r.x );
	r.z = ( ( r.z * c ) + d.r.w );
	r.w = ( ( r.w * c ) - d.r.z );
	e.x = ( ( e.x * c ) + d.e.y );
	e.y = ( ( e.y * c ) - d.e.x );
	e.z = ( ( e.z * c ) + d.e.w );
	e.w = ( ( e.w * c ) - d.e.z );
}
 
void dquat::EndPitch( const float radians )
{	//	rotate about local X
	float a = ( radians * 0.5f );
	float f;
	dquat d;
	f = sinf( a );
	d.r.x = ( r.x * f );
	d.r.y = ( r.y * f );
	d.r.z = ( r.z * f );
	d.r.w = ( r.w * f );
	d.e.x = ( e.x * f );
	d.e.y = ( e.y * f );
	d.e.z = ( e.z * f );
	d.e.w = ( e.w * f );
	f = cosf( a );
	r.x = ( ( r.x * f ) + d.r.w );
	r.y = ( ( r.y * f ) + d.r.z );
	r.z = ( ( r.z * f ) - d.r.y );
	r.w = ( ( r.w * f ) - d.r.x );
	e.x = ( ( e.x * f ) + d.e.w );
	e.y = ( ( e.y * f ) + d.e.z );
	e.z = ( ( e.z * f ) - d.e.y );
	e.w = ( ( e.w * f ) - d.e.x );
}

void dquat::EndYaw( const float radians )
{	//	rotate about local Y
	float a = ( radians * 0.5f );
	float f;
	dquat d;
	f = sinf( a );
	d.r.x = ( r.x * f );
	d.r.y = ( r.y * f );
	d.r.z = ( r.z * f );
	d.r.w = ( r.w * f );
	d.e.x = ( e.x * f );
	d.e.y = ( e.y * f );
	d.e.z = ( e.z * f );
	d.e.w = ( e.w * f );
	f = cosf( a );
	r.x = ( ( r.x * f ) - d.r.z );
	r.y = ( ( r.y * f ) + d.r.w );
	r.z = ( ( r.z * f ) + d.r.x );
	r.w = ( ( r.w * f ) - d.r.y );
	e.x = ( ( e.x * f ) - d.e.z );
	e.y = ( ( e.y * f ) + d.e.w );
	e.z = ( ( e.z * f ) + d.e.x );
	e.w = ( ( e.w * f ) - d.e.y );
}

void dquat::Move( const float move_x, const float move_y, const float move_z )
{
	vec3 move = { move_x, move_y, move_z };
	Move( move );
}

void dquat::Move( const vec3& move )
{
	float n = ( 0.5f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	e.x += ( ( ( r.w * move.x ) - ( r.y * move.z ) + ( r.z * move.y ) ) * n );
	e.y += ( ( ( r.w * move.y ) - ( r.z * move.x ) + ( r.x * move.z ) ) * n );
	e.z += ( ( ( r.w * move.z ) - ( r.x * move.y ) + ( r.y * move.x ) ) * n );
	e.w -= ( ( ( r.z * move.z ) + ( r.y * move.y ) + ( r.x * move.x ) ) * n );
}

void dquat::Pan( const float pan_x, const float pan_y, const float pan_z )
{
	vec3 pan = { pan_x, pan_y, pan_z };
	Pan( pan );
}

void dquat::Pan( const vec3& pan )
{
	float n = ( 0.5f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	e.x += ( ( ( r.w * pan.x ) + ( r.y * pan.z ) - ( r.z * pan.y ) ) * n );
	e.y += ( ( ( r.w * pan.y ) + ( r.z * pan.x ) - ( r.x * pan.z ) ) * n );
	e.z += ( ( ( r.w * pan.z ) + ( r.x * pan.y ) - ( r.y * pan.x ) ) * n );
	e.w -= ( ( ( r.z * pan.z ) + ( r.y * pan.y ) + ( r.x * pan.x ) ) * n );
}

void dquat::Add( const dquat& add )
{
	r.Add( add.r );
	e.Add( add.e );
}

void dquat::Sub( const dquat& sub )
{
	r.Sub( sub.r );
	e.Sub( sub.e );
}

void dquat::Mul( const dquat& mul )
{
	SetMul( *this, mul );
}

void dquat::Mul( const quat& mul )
{
	e.Mul( mul );
	r.Mul( mul );
}

void dquat::Mul( const float mul )
{
	e.Mul( mul );
	r.Mul( mul );
}

void dquat::Div( const dquat& div )
{
	SetDiv( *this, div );
}

void dquat::Div( const quat& div )
{
	SetDiv( *this, div );
}

void dquat::Div( const float div )
{
	SetDiv( *this, div );
}

void dquat::Pow( const dquat& pow )
{
	Log();
	Mul( pow );
	Exp();
}

void dquat::Pow( const float pow )
{
	Log();
	Mul( pow );
	Exp();
}

void dquat::Log( void )
{
	float rr3 = ( ( r.x * r.x ) + ( r.y * r.y ) + ( r.z * r.z ) + consts::FLOAT_MIN_NORM );
	float re3 = ( ( r.x * e.x ) + ( r.y * e.y ) + ( r.z * e.z ) + consts::FLOAT_MIN_NORM );
	float rr4 = ( ( r.w * r.w ) + rr3 );
	float re4 = ( ( r.w * e.w ) + re3 );
	float i = ( re4 / rr4 );
	float j = ( acosf( r.w / sqrtf( rr4 ) ) / sqrtf( rr3 ) );
	float k = ( ( ( re3 * j ) - ( r.w * i ) + e.w ) / rr3 );
	e.x = ( ( e.x * j ) - ( r.x * k ) );
	e.y = ( ( e.y * j ) - ( r.y * k ) );
	e.z = ( ( e.z * j ) - ( r.z * k ) );
	e.w = i;
	r.x *= j;
	r.y *= j;
	r.z *= j;
	r.w = ( logf( rr4 ) * 0.5f );
}

void dquat::Exp( void )
{
	float rr3 = ( ( r.x * r.x ) + ( r.y * r.y ) + ( r.z * r.z ) + consts::FLOAT_MIN_NORM );
	float re3 = ( ( r.x * e.x ) + ( r.y * e.y ) + ( r.z * e.z ) + consts::FLOAT_MIN_NORM );
	float x = expf( r.w );
	float a = sqrtf( rr3 );
	float i = ( cosf( a ) * x );
	float j = ( ( sinf( a ) * x ) / a );
	float k =  ( ( ( re3 * ( j - i ) ) / rr3 ) - ( e.w * j ) );
	e.x = ( ( e.x * j ) - ( r.x * k ) );
	e.y = ( ( e.y * j ) - ( r.y * k ) );
	e.z = ( ( e.z * j ) - ( r.z * k ) );
	e.w = ( ( e.w * i ) - ( re3 * j ) );
	r.x *= j;
	r.y *= j;
	r.z *= j;
	r.w = i;
}

void dquat::Negate( void )
{
	r.Negate();
	e.Negate();
}

void dquat::Invert( void )
{
	Conjugate();
	Mul( 1.0f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
}

void dquat::Conjugate( void )
{
	r.Conjugate();
	e.Conjugate();
}

void dquat::Normalize( void )
{
	Mul( 1.0f / ( r.Length() + consts::FLOAT_MIN_NORM ) );
}

void dquat::Lerp( const dquat& trg, const float t )
{
	SetLerp( *this, trg, t );
}

void dquat::Sclerp( const dquat& trg, const float t )
{
	SetSclerp( *this, trg, t );
}

void dquat::Blend( const dquat& trg, const float t )
{
	SetBlend( *this, trg, t );
}

void dquat::SetAdd( const dquat& src, const dquat& add )
{
	r.SetAdd( src.r, add.r );
	e.SetAdd( src.e, add.e );
}

void dquat::SetSub( const dquat& src, const dquat& sub )
{
	r.SetSub( src.r, sub.r );
	e.SetSub( src.e, sub.e );
}

void dquat::SetMul( const dquat& src, const dquat& mul )
{
	maths::quat es, em;
	es.SetMul( src.e, mul.r );
	em.SetMul( src.r, mul.e );
	em.Mul( 1.0f / ( src.r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	r.SetMul( src.r, mul.r );
	e.SetAdd( es, em );
}

void dquat::SetMul( const dquat& src, const quat& mul )
{
	e.SetMul( src.e, mul );
	r.SetMul( src.r, mul );
}

void dquat::SetMul( const quat& src, const dquat& mul )
{
	e.SetMul( src, mul.e );
	r.SetMul( src, mul.r );
}

void dquat::SetMul( const dquat& src, const float mul )
{
	r.SetMul( src.r, mul );
	e.SetMul( src.e, mul );
}

void dquat::SetDiv( const dquat& src, const dquat& div )
{
	dquat mul;
	mul.SetInverse( div );
	SetMul( src, mul );
}

void dquat::SetDiv( const dquat& src, const quat& div )
{
	quat mul;
	mul.SetInverse( div );
	SetMul( src, mul );
}

void dquat::SetDiv( const dquat& src, const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( div ) >= consts::FLOAT_MIN_RCP )
	{
		SetMul( src, ( 1.0f / div ) );
	}
	else
	{
		SetIdentity();
	}
}

void dquat::SetPow( const dquat& src, const dquat& pow )
{
	Set( src );
	Pow( pow );
}

void dquat::SetPow( const dquat& src, const float pow )
{
	Set( src );
	Pow( pow );
}

void dquat::SetLog( const dquat& src )
{
	Set( src );
	Log();
}

void dquat::SetExp( const dquat& src )
{
	Set( src );
	Exp();
}

void dquat::SetNegate( const dquat& src )
{
	r.SetNegate( src.r );
	e.SetNegate( src.e );
}

void dquat::SetInverse( const dquat& src )
{
	SetConjugate( src );
	Mul( 1.0f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
}

void dquat::SetConjugate( const dquat& src )
{
	r.SetConjugate( src.r );
	e.SetConjugate( src.e );
}

void dquat::SetNormalized( const dquat& src )
{
	SetMul( src, ( 1.0f / ( src.r.Length() + consts::FLOAT_MIN_NORM ) ) );
}

void dquat::SetLerp( const dquat& src, const dquat& trg, const float t )
{
	float s = ( 1.0f - t );
	if( ( ( src.r.x * trg.r.x ) + ( src.r.y * trg.r.y ) + ( src.r.z * trg.r.z ) + ( src.r.w * trg.r.w ) ) < 0.0f )
	{
		r.x = ( ( src.r.x * s ) - ( trg.r.x * t ) );
		r.y = ( ( src.r.y * s ) - ( trg.r.y * t ) );
		r.z = ( ( src.r.z * s ) - ( trg.r.z * t ) );
		r.w = ( ( src.r.w * s ) - ( trg.r.w * t ) );
		e.x = ( ( src.e.x * s ) - ( trg.e.x * t ) );
		e.y = ( ( src.e.y * s ) - ( trg.e.y * t ) );
		e.z = ( ( src.e.z * s ) - ( trg.e.z * t ) );
		e.w = ( ( src.e.w * s ) - ( trg.e.w * t ) );
	}
	else
	{
		r.x = ( ( src.r.x * s ) + ( trg.r.x * t ) );
		r.y = ( ( src.r.y * s ) + ( trg.r.y * t ) );
		r.z = ( ( src.r.z * s ) + ( trg.r.z * t ) );
		r.w = ( ( src.r.w * s ) + ( trg.r.w * t ) );
		e.x = ( ( src.e.x * s ) + ( trg.e.x * t ) );
		e.y = ( ( src.e.y * s ) + ( trg.e.y * t ) );
		e.z = ( ( src.e.z * s ) + ( trg.e.z * t ) );
		e.w = ( ( src.e.w * s ) + ( trg.e.w * t ) );
	}
}

void dquat::SetSclerp( const dquat& src, const dquat& trg, const float t )
{
	dquat mul;
	mul.SetDelta( src, trg );
	mul.Pow( t );
	SetMul( src, mul );
}

void dquat::SetBlend( const dquat& src, const dquat& trg, const float t )
{
	r.SetBlend( src.r, trg.r, t );
	e.SetBlend( src.e, trg.e, t );
}

void dquat::SetDelta( const dquat& src, const dquat& trg )
{
	SetInverse( src );
	Mul( trg );
}

void dquat::Set( const dquat& d )
{
	r.Set( d.r );
	e.Set( d.e );
}

void dquat::Set( const mat43& m )
{
	r.Set( m.t_mat33() );
	float n = ( 0.5f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	e.x =  ( ( ( r.w * m.x.w ) - ( r.y * m.z.w ) + ( r.z * m.y.w ) ) * n );
	e.y =  ( ( ( r.w * m.y.w ) - ( r.z * m.x.w ) + ( r.x * m.z.w ) ) * n );
	e.z =  ( ( ( r.w * m.z.w ) - ( r.x * m.y.w ) + ( r.y * m.x.w ) ) * n );
	e.w = -( ( ( r.z * m.z.w ) + ( r.y * m.y.w ) + ( r.x * m.x.w ) ) * n );
}

void dquat::Set( const quat& rotate, const vec3& translate )
{
	r.Set( rotate );
	float n = ( 0.5f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	e.x =  ( ( ( r.w * translate.x ) - ( r.y * translate.z ) + ( r.z * translate.y ) ) * n );
	e.y =  ( ( ( r.w * translate.y ) - ( r.z * translate.x ) + ( r.x * translate.z ) ) * n );
	e.z =  ( ( ( r.w * translate.z ) - ( r.x * translate.y ) + ( r.y * translate.x ) ) * n );
	e.w = -( ( ( r.z * translate.z ) + ( r.y * translate.y ) + ( r.x * translate.x ) ) * n );
}

void dquat::Get( dquat& d ) const
{
	r.Get( d.r );
	e.Get( d.e );
}

void dquat::Get( mat43& m ) const
{
	r.Get( m.t_mat33() );
	m.x.w = ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * 2.0f );
	m.y.w = ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * 2.0f );
	m.z.w = ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * 2.0f );
}

void dquat::Get( quat& rotate, vec3& translate ) const
{
	rotate.Set( r );
	GetTranslation( translate );
}

void dquat::GetTranslation( vec3& translate ) const
{
	translate.x = ( ( ( r.w * e.x ) - ( r.x * e.w ) + ( r.y * e.z ) - ( r.z * e.y ) ) * 2.0f );
	translate.y = ( ( ( r.w * e.y ) - ( r.y * e.w ) + ( r.z * e.x ) - ( r.x * e.z ) ) * 2.0f );
	translate.z = ( ( ( r.w * e.z ) - ( r.z * e.w ) + ( r.x * e.y ) - ( r.y * e.x ) ) * 2.0f );
}

void dquat::GetInverse( dquat& d ) const
{
	d.SetInverse( *this );
}

void dquat::GetInverse( mat43& m ) const
{
	dquat d;
	d.SetInverse( *this );
	Get( m );
}

void dquat::GetInverse( quat& rotate, vec3& translate ) const
{
	float n = ( 1.0f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	rotate.x = -( r.x * n );
	rotate.y = -( r.y * n );
	rotate.z = -( r.z * n );
	rotate.w =  ( r.w * n );
	n += n;
	translate.x = ( ( ( r.x * e.w ) - ( r.w * e.x ) + ( r.y * e.z ) - ( r.z * e.y ) ) * n );
	translate.y = ( ( ( r.y * e.w ) - ( r.w * e.y ) + ( r.z * e.x ) - ( r.x * e.z ) ) * n );
	translate.z = ( ( ( r.z * e.w ) - ( r.w * e.z ) + ( r.x * e.y ) - ( r.y * e.x ) ) * n );
}

void dquat::GetInverseTranslation( vec3& translate ) const
{
	float n = ( 2.0f / ( r.SqrLen() + consts::FLOAT_MIN_NORM ) );
	translate.x = ( ( ( r.x * e.w ) - ( r.w * e.x ) + ( r.y * e.z ) - ( r.z * e.y ) ) * n );
	translate.y = ( ( ( r.y * e.w ) - ( r.w * e.y ) + ( r.z * e.x ) - ( r.x * e.z ) ) * n );
	translate.z = ( ( ( r.z * e.w ) - ( r.w * e.z ) + ( r.x * e.y ) - ( r.y * e.x ) ) * n );
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

