
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   joint.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Quaternion and vector container and functions.
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

#include "joint.h"
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
	namespace JOINT
	{
		const joint		IDENTITY = { { 0, 0, 0, 1 }, { 0, 0, 0, 0 } };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    joint
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool joint::IsNan( void ) const
{
	return( q.IsNan() || v.IsNan() );
}

bool joint::IsReal( void ) const
{
	return( q.IsReal() && v.IsReal() );
}

void joint::Transform( const vec3& src, vec3& trg ) const
{
	vec3 tmp;
	q.Transform( src, tmp );
	trg.SetAdd( tmp, v );
}

void joint::Transform( const vec4& src, vec4& trg ) const
{
	vec3 tmp;
	q.Transform( src.t_vec3(), tmp );
	trg.x = ( tmp.x + ( v.x * src.w ) );
	trg.y = ( tmp.y + ( v.y * src.w ) );
	trg.z = ( tmp.z + ( v.z * src.w ) );
	trg.w = src.w;
}

void joint::UnscaledTransform( const vec3& src, vec3& trg ) const
{
	vec3 tmp;
	q.UnscaledTransform( src, tmp );
	trg.SetAdd( tmp, v );
}

void joint::UnscaledTransform( const vec4& src, vec4& trg ) const
{
	vec3 tmp;
	q.UnscaledTransform( src.t_vec3(), tmp );
	trg.x = ( tmp.x + ( v.x * src.w ) );
	trg.y = ( tmp.y + ( v.y * src.w ) );
	trg.z = ( tmp.z + ( v.z * src.w ) );
	trg.w = src.w;
}

void joint::InverseTransform( const vec3& src, vec3& trg ) const
{
	vec3 tmp;
    tmp.SetSub( src, v );
	q.InverseTransform( tmp, trg );
}

void joint::InverseTransform( const vec4& src, vec4& trg ) const
{
	vec3 tmp;
	tmp.x = ( src.x - ( v.x * src.w ) );
	tmp.y = ( src.y - ( v.y * src.w ) );
	tmp.z = ( src.z - ( v.z * src.w ) );
	q.InverseTransform( tmp, trg.t_vec3() );
	trg.w = src.w;
}

void joint::PointAt( const vec3& from, const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - from.x ), ( at.y - from.y ), ( at.z - from.z ) };
	q.PointTo( to, up );
	v.Set( from );
}

void joint::PointTo( const vec3& from, const vec3& to, const vec3& up )
{
	q.PointTo( to, up );
	v.Set( from );
}

void joint::PointAt( const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - v.x ), ( at.y - v.y ), ( at.z - v.z ) };
	q.PointTo( to, up );
}

void joint::PointTo( const vec3& to, const vec3& up )
{
	q.PointTo( to, up );
}

void joint::LookAt( const vec3& from, const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - from.x ), ( at.y - from.y ), ( at.z - from.z ) };
	q.LookTo( to, up );
	v.Set( from );
}

void joint::LookTo( const vec3& from, const vec3& to, const vec3& up )
{
	q.LookTo( to, up );
	v.Set( from );
}

void joint::LookAt( const vec3& at, const vec3& up )
{
	vec3 to = { ( at.x - v.x ), ( at.y - v.y ), ( at.z - v.z ) };
	q.LookTo( to, up );
}

void joint::LookTo( const vec3& to, const vec3& up )
{
	q.LookTo( to, up );
}

void joint::Scale( const float scale )
{
	q.Scale( scale );
	v.Mul( scale );
}

void joint::Twist( const float radians )
{
	q.Twist( radians );
	float s = sinf( radians );
	float c = cosf( radians );
	float _x = ( ( v.x * c ) - ( v.y * s ) );
	float _y = ( ( v.y * c ) + ( v.x * s ) );
	v.x = _x;
	v.y = _y;
}

void joint::Tilt( const float radians )
{
	q.Tilt( radians );
	float s = sinf( radians );
	float c = cosf( radians );
	float _y = ( ( v.y * c ) - ( v.z * s ) );
	float _z = ( ( v.z * c ) + ( v.y * s ) );
	v.y = _y;
	v.z = _z;
}

void joint::Turn( const float radians )
{
	q.Turn( radians );
	float s = sinf( radians );
	float c = cosf( radians );
	float _z = ( ( v.z * c ) - ( v.x * s ) );
	float _x = ( ( v.x * c ) + ( v.z * s ) );
	v.z = _z;
	v.x = _x;
}

void joint::Roll( const float radians )
{
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	quat j;
	j.x = ( ( ( q.z * q.x ) + ( q.w * q.y ) ) * s * 2.0f );
	j.y = ( ( ( q.y * q.z ) - ( q.w * q.x ) ) * s * 2.0f );
	j.z = ( ( ( q.w * q.w ) - ( q.x * q.x ) - ( q.y * q.y ) + ( q.z * q.z ) ) * s );
	j.w = ( q.SqrLen() * c );
	j.UnscaledTransform( v, v );
	j.x = ( q.x * s );
	j.y = ( q.y * s );
	j.z = ( q.z * s );
	j.w = ( q.w * s );
	q.x = ( ( q.x * c ) + j.y );
	q.y = ( ( q.y * c ) - j.x );
	q.z = ( ( q.z * c ) + j.w );
	q.w = ( ( q.w * c ) - j.z );
}

void joint::Pitch( const float radians )
{
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	quat j;
	j.x = ( ( ( q.w * q.w ) + ( q.x * q.x ) - ( q.y * q.y ) - ( q.z * q.z ) ) * s );
	j.y = ( ( ( q.x * q.y ) + ( q.w * q.z ) ) * s * 2.0f );
	j.z = ( ( ( q.z * q.x ) - ( q.w * q.y ) ) * s * 2.0f );
	j.w = ( q.SqrLen() * c );
	j.UnscaledTransform( v, v );
	j.x = ( q.x * s );
	j.y = ( q.y * s );
	j.z = ( q.z * s );
	j.w = ( q.w * s );
	q.x = ( ( q.x * c ) + j.w );
	q.y = ( ( q.y * c ) + j.z );
	q.z = ( ( q.z * c ) - j.y );
	q.w = ( ( q.w * c ) - j.x );
}

void joint::Yaw( const float radians )
{
	float a = ( radians * 0.5f );
	float s = sinf( a );
	float c = cosf( a );
	quat j;
	j.x = ( ( ( q.x * q.y ) - ( q.w * q.z ) ) * s * 2.0f );
	j.y = ( ( ( q.w * q.w ) - ( q.x * q.x ) + ( q.y * q.y ) - ( q.z * q.z ) ) * s );
	j.z = ( ( ( q.y * q.z ) + ( q.w * q.x ) ) * s * 2.0f );
	j.w = ( q.SqrLen() * c );
	j.UnscaledTransform( v, v );
	j.x = ( q.x * s );
	j.y = ( q.y * s );
	j.z = ( q.z * s );
	j.w = ( q.w * s );
	q.x = ( ( q.x * c ) - j.z );
	q.y = ( ( q.y * c ) + j.w );
	q.z = ( ( q.z * c ) + j.x );
	q.w = ( ( q.w * c ) - j.y );
}

void joint::EndScale( const float scale )
{
	q.Scale( scale );
}

void joint::EndTwist( const float radians )
{
	q.Twist( radians );
}

void joint::EndTilt( const float radians )
{
	q.Tilt( radians );
}

void joint::EndTurn( const float radians )
{
	q.Turn( radians );
}

void joint::EndRoll( const float radians )
{
	q.Roll( radians );
}

void joint::EndPitch( const float radians )
{
	q.Pitch( radians );
}

void joint::EndYaw( const float radians )
{
	q.Yaw( radians );
}

void joint::Move( const float move_x, const float move_y, const float move_z )
{
	v.x += move_x;
	v.y += move_y;
	v.z += move_z;
}

void joint::Move( const vec3& move )
{
	v.x += move.x;
	v.y += move.y;
	v.z += move.z;
}

void joint::Pan( const float pan_x, const float pan_y, const float pan_z )
{
	vec3 pan = { pan_x, pan_y, pan_z };
	Pan( pan );
}

void joint::Pan( const vec3& pan )
{
	vec3 move;
	q.Transform( pan, move );
	Move( move );
}

void joint::UnscaledPan( const float pan_x, const float pan_y, const float pan_z )
{
	vec3 pan = { pan_x, pan_y, pan_z };
	UnscaledPan( pan );
}

void joint::UnscaledPan( const vec3& pan )
{
	vec3 move;
	q.UnscaledTransform( pan, move );
	Move( move );
}

void joint::PreMul( const joint& mul )
{
	SetMul( mul, *this );
}

void joint::Mul( const joint& mul )
{
	SetMul( *this, mul );
}

void joint::Mul( const quat& mul )
{
	q.Mul( mul );
	v.Mul( mul );
}

void joint::Mul( const float mul )
{
	q.Mul( mul );
	v.Mul( mul );
}

void joint::Div( const joint& div )
{
	SetDiv( *this, div );
}

void joint::Div( const quat& div )
{
	SetDiv( *this, div );
}

void joint::Div( const float div )
{
	SetDiv( *this, div );
}

void joint::Invert( void )
{
	q.Invert();
	v.Negate();
	q.Transform( v, v );
}

void joint::Lerp( const joint& trg, const float t )
{
	q.Lerp( trg.q, t );
	v.Lerp( trg.v, t );
}

void joint::Slerp( const joint& trg, const float t )
{
	q.Slerp( trg.q, t );
	v.Lerp( trg.v, t );
}

void joint::SetMul( const joint& src, const joint& mul )
{
	vec3 tmp;
	mul.q.Transform( src.v, tmp );
	v.SetAdd( tmp, mul.v );
	q.SetMul( src.q, mul.q );
}

void joint::SetMul( const joint& src, const quat& mul )
{
	mul.Transform( src.v, v );
	q.SetMul( src.q, mul );
}

void joint::SetMul( const quat& src, const joint& mul )
{
	v.Set( mul.v );
	q.SetMul( src, mul.q );
}

void joint::SetMul( const joint& src, const float mul )
{
	q.SetMul( src.q, mul );
	v.SetMul( src.v, mul );
}

void joint::SetDiv( const joint& src, const joint& div )
{
	joint mul;
	mul.SetInverse( div );
	SetMul( src, mul );
}

void joint::SetDiv( const joint& src, const quat& div )
{
	quat mul;
	mul.SetInverse( div );
	SetMul( src, mul );
}

void joint::SetDiv( const joint& src, const float div )
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

void joint::SetDelta( const joint& src, const joint& trg )
{
	SetInverse( src );
	Mul( trg );
}

void joint::SetInverse( const joint& src )
{
	Set( src );
	Invert();
}

void joint::SetLerp( const joint& src, const joint& trg, const float t )
{
	q.SetLerp( src.q, trg.q, t );
	v.SetLerp( src.v, trg.v, t );
}

void joint::SetSlerp( const joint& src, const joint& trg, const float t )
{
	q.SetSlerp( src.q, trg.q, t );
	v.SetLerp( src.v, trg.v, t );
}

void joint::Set( const joint& j )
{
	q.Set( j.q );
	v.Set( j.v );
}

void joint::Set( const mat43& m )
{
	q.Set( m.t_mat33() );
	v.Set( m.x.w, m.y.w, m.z.w );
}

void joint::Set( const quat& rotate, const vec3& translate )
{
	q.Set( rotate );
	v.Set( translate );
}

void joint::Get( joint& j ) const
{
	q.Get( j.q );
	v.Get( j.v );
}

void joint::Get( mat43& m ) const
{
	q.Get( m.t_mat33() );
	m.x.w = v.x;
	m.y.w = v.y;
	m.z.w = v.z;
}

void joint::Get( quat& rotate, vec3& translate ) const
{
	q.Get( rotate );
	v.Get( translate );
}

void joint::GetInverse( joint& j ) const
{
	j.SetInverse( *this );
}

void joint::GetInverse( mat43& m ) const
{
	q.GetInverse( m.t_mat33() );
	m.x.w = -( ( v.x * m.x.x ) + ( v.y * m.x.y ) + ( v.z * m.x.z ) );
	m.y.w = -( ( v.x * m.y.x ) + ( v.y * m.y.y ) + ( v.z * m.y.z ) );
	m.z.w = -( ( v.x * m.z.x ) + ( v.y * m.z.y ) + ( v.z * m.z.z ) );
}

void joint::GetInverse( quat& rotate, vec3& translate ) const
{
	joint j;
	j.SetInverse( *this );
	j.Get( rotate, translate );
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
