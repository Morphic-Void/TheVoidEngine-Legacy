
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   quat.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Quaternion container and functions.
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

#include "quat.h"
#include "mat33.h"
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
	namespace QUAT
	{
		const quat		IDENTITY = { 0, 0, 0, 1 };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    quat
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool quat::IsNan( void ) const
{
	return( ::IsNan( x ) || ::IsNan( y ) || ::IsNan( z ) || ::IsNan( w ) );
}

bool quat::IsReal( void ) const
{
	return( ::IsReal( x ) && ::IsReal( y ) && ::IsReal( z ) && ::IsReal( w ) );
}

void quat::Transform( const vec3& src, vec3& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	{
		float xx = ( x * x );
		float yy = ( y * y );
		float zz = ( z * z );
		float ww = ( w * w );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( x + x );
		float y2 = ( y + y );
		float z2 = ( z + z );
		{
			float yz = ( y * z2 );
			float wx = ( w * x2 );
			tz += ( sy * ( yz + wx ) );
			ty += ( sz * ( yz - wx ) );
		}
		{
			float zx = ( z * x2 );
			float wy = ( w * y2 );
			tx += ( sz * ( zx + wy ) );
			tz += ( sx * ( zx - wy ) );
		}
		{
			float xy = ( x * y2 );
			float wz = ( w * z2 );
			ty += ( sx * ( xy + wz ) );
			tx += ( sy * ( xy - wz ) );
		}
	}
	trg.x = tx;
	trg.y = ty;
	trg.z = tz;
}

void quat::InverseTransform( const vec3& src, vec3& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	float ll;
	{
		float xx = ( x * x );
		float yy = ( y * y );
		float zz = ( z * z );
		float ww = ( w * w );
		ll = ( xx + yy + zz + ww );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( x + x );
		float y2 = ( y + y );
		float z2 = ( z + z );
		{
			float yz = ( y * z2 );
			float wx = ( w * x2 );
			tz += ( sy * ( yz - wx ) );
			ty += ( sz * ( yz + wx ) );
		}
		{
			float zx = ( z * x2 );
			float wy = ( w * y2 );
			tx += ( sz * ( zx - wy ) );
			tz += ( sx * ( zx + wy ) );
		}
		{
			float xy = ( x * y2 );
			float wz = ( w * z2 );
			ty += ( sx * ( xy - wz ) );
			tx += ( sy * ( xy + wz ) );
		}
	}
	ll = ( 1.0f / ( ( ll * ll ) + consts::FLOAT_MIN_NORM ) );
	trg.x = ( tx * ll );
	trg.y = ( ty * ll );
	trg.z = ( tz * ll );
}

void quat::UnscaledTransform( const vec3& src, vec3& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	float ll;
	{
		float xx = ( x * x );
		float yy = ( y * y );
		float zz = ( z * z );
		float ww = ( w * w );
		ll = ( xx + yy + zz + ww );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( x + x );
		float y2 = ( y + y );
		float z2 = ( z + z );
		{
			float yz = ( y * z2 );
			float wx = ( w * x2 );
			tz += ( sy * ( yz + wx ) );
			ty += ( sz * ( yz - wx ) );
		}
		{
			float zx = ( z * x2 );
			float wy = ( w * y2 );
			tx += ( sz * ( zx + wy ) );
			tz += ( sx * ( zx - wy ) );
		}
		{
			float xy = ( x * y2 );
			float wz = ( w * z2 );
			ty += ( sx * ( xy + wz ) );
			tx += ( sy * ( xy - wz ) );
		}
	}
	ll = ( 1.0f / ( ll + consts::FLOAT_MIN_NORM ) );
	trg.x = ( tx * ll );
	trg.y = ( ty * ll );
	trg.z = ( tz * ll );
}

void quat::UnscaledInverseTransform( const vec3& src, vec3& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	float ll;
	{
		float xx = ( x * x );
		float yy = ( y * y );
		float zz = ( z * z );
		float ww = ( w * w );
		ll = ( xx + yy + zz + ww );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( x + x );
		float y2 = ( y + y );
		float z2 = ( z + z );
		{
			float yz = ( y * z2 );
			float wx = ( w * x2 );
			tz += ( sy * ( yz - wx ) );
			ty += ( sz * ( yz + wx ) );
		}
		{
			float zx = ( z * x2 );
			float wy = ( w * y2 );
			tx += ( sz * ( zx - wy ) );
			tz += ( sx * ( zx + wy ) );
		}
		{
			float xy = ( x * y2 );
			float wz = ( w * z2 );
			ty += ( sx * ( xy - wz ) );
			tx += ( sy * ( xy + wz ) );
		}
	}
	ll = ( 1.0f / ( ll + consts::FLOAT_MIN_NORM ) );
	trg.x = ( tx * ll );
	trg.y = ( ty * ll );
	trg.z = ( tz * ll );
}

void quat::ConjugateTransform( const vec3& src, vec3& trg ) const
{
	float sx = src.x;
	float sy = src.y;
	float sz = src.z;
	float tx, ty, tz;
	{
		float xx = ( x * x );
		float yy = ( y * y );
		float zz = ( z * z );
		float ww = ( w * w );
		tx = ( sx * ( ww + xx - yy - zz ) );
		ty = ( sy * ( ww - xx + yy - zz ) );
		tz = ( sz * ( ww - xx - yy + zz ) );
	}
	{
		float x2 = ( x + x );
		float y2 = ( y + y );
		float z2 = ( z + z );
		{
			float yz = ( y * z2 );
			float wx = ( w * x2 );
			tz += ( sy * ( yz - wx ) );
			ty += ( sz * ( yz + wx ) );
		}
		{
			float zx = ( z * x2 );
			float wy = ( w * y2 );
			tx += ( sz * ( zx - wy ) );
			tz += ( sx * ( zx + wy ) );
		}
		{
			float xy = ( x * y2 );
			float wz = ( w * z2 );
			ty += ( sx * ( xy - wz ) );
			tx += ( sy * ( xy + wz ) );
		}
	}
	trg.x = tx;
	trg.y = ty;
	trg.z = tz;
}

float quat::Length( void ) const
{
	return( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ) );
}

float quat::SqrLen( void ) const
{
	return( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
}

float quat::Dot( const quat& dot ) const
{
	return( ( w * dot.w ) - ( z * dot.z ) - ( y * dot.y ) - ( x * dot.x ) );
}

void quat::PointTo( const vec3& to, const vec3& up )
{
	mat33 m;
	m.PointTo( to, up );
	Set( m );
}

void quat::LookTo( const vec3& to, const vec3& up )
{
	mat33 m;
	m.LookTo( to, up );
	Set( m );
}

void quat::Scale( const float scale )
{
	ASSERT( scale > 0.0f );
	if( scale > 0.0f )
	{
		float n = sqrtf( scale );
		x *= n;
		y *= n;
		z *= n;
		w *= n;
	}
}

void quat::Twist( const float radians )
{	//	rotate about world Z
	float a = ( radians * 0.5f );
	float f;
	quat q;
	f = sinf( a );
	q.x = ( x * f );
	q.y = ( y * f );
	q.z = ( z * f );
	q.w = ( w * f );
	f = cosf( a );
	x = ( ( x * f ) - q.y );
	y = ( ( y * f ) + q.x );
	z = ( ( z * f ) + q.w );
	w = ( ( w * f ) - q.z );
}
 
void quat::Tilt( const float radians )
{	//	rotate about world X
	float a = ( radians * 0.5f );
	float f;
	quat q;
	f = sinf( a );
	q.x = ( x * f );
	q.y = ( y * f );
	q.z = ( z * f );
	q.w = ( w * f );
	f = cosf( a );
	x = ( ( x * f ) + q.w );
	y = ( ( y * f ) - q.z );
	z = ( ( z * f ) + q.y );
	w = ( ( w * f ) - q.x );
}
 
void quat::Turn( const float radians )
{	//	rotate about world Y
	float a = ( radians * 0.5f );
	float f;
	quat q;
	f = sinf( a );
	q.x = ( x * f );
	q.y = ( y * f );
	q.z = ( z * f );
	q.w = ( w * f );
	f = cosf( a );
	x = ( ( x * f ) + q.z );
	y = ( ( y * f ) + q.w );
	z = ( ( z * f ) - q.x );
	w = ( ( w * f ) - q.y );
}

void quat::Roll( const float radians )
{	//	rotate about local Z
	float a = ( radians * 0.5f );
	float f;
	quat q;
	f = sinf( a );
	q.x = ( x * f );
	q.y = ( y * f );
	q.z = ( z * f );
	q.w = ( w * f );
	f = cosf( a );
	x = ( ( x * f ) + q.y );
	y = ( ( y * f ) - q.x );
	z = ( ( z * f ) + q.w );
	w = ( ( w * f ) - q.z );
}
 
void quat::Pitch( const float radians )
{	//	rotate about local X
	float a = ( radians * 0.5f );
	float f;
	quat q;
	f = sinf( a );
	q.x = ( x * f );
	q.y = ( y * f );
	q.z = ( z * f );
	q.w = ( w * f );
	f = cosf( a );
	x = ( ( x * f ) + q.w );
	y = ( ( y * f ) + q.z );
	z = ( ( z * f ) - q.y );
	w = ( ( w * f ) - q.x );
}

void quat::Yaw( const float radians )
{	//	rotate about local Y
	float a = ( radians * 0.5f );
	float f;
	quat q;
	f = sinf( a );
	q.x = ( x * f );
	q.y = ( y * f );
	q.z = ( z * f );
	q.w = ( w * f );
	f = cosf( a );
	x = ( ( x * f ) - q.z );
	y = ( ( y * f ) + q.w );
	z = ( ( z * f ) + q.x );
	w = ( ( w * f ) - q.y );
}

void quat::Add( const quat& add )
{
	x += add.x;
	y += add.y;
	z += add.z;
	w += add.w;
}

void quat::Sub( const quat& sub )
{
	x -= sub.x;
	y -= sub.y;
	z -= sub.z;
	w -= sub.w;
}

void quat::Mul( const quat& mul )
{
	SetMul( *this, mul );
}

void quat::MulByConjugate( const quat& conj )
{
	SetMulByConjugate( *this, conj );
}

void quat::Mul( const float mul )
{
	x *= mul;
	y *= mul;
	z *= mul;
	w *= mul;
}

void quat::Div( const quat& div )
{
	SetDiv( *this, div );
}

void quat::Div( const float div )
{
	SetDiv( *this, div );
}

void quat::Pow( const quat& pow )
{
	Log();
	Mul( pow );
	Exp();
}

void quat::Pow( const float pow )
{
	float n = ( ( x * x ) + ( y * y ) + ( z * z ) );
	float e = powf( ( n + ( w * w ) ), ( pow * 0.5f ) );
	n = sqrtf( n );
	float a = ( atan2f( n, w ) * pow );
	float j = ( ( sinf( a ) * e ) / ( n + consts::FLOAT_MIN_NORM ) );
	x *= j;
	y *= j;
	z *= j;
	w = ( cosf( a ) * e );
}

void quat::Log( void )
{
	float n = ( ( x * x ) + ( y * y ) + ( z * z ) );
	float l =( logf( n + ( w * w ) + consts::FLOAT_MIN_NORM ) * 0.5f );
	n = sqrtf( n );
	float j = ( atan2f( n, w ) / ( n + consts::FLOAT_MIN_NORM ) );
	x *= j;
	y *= j;
	z *= j;
	w = l;
}

void quat::Exp( void )
{
	float e = expf( w );
	float a = sqrtf( ( x * x ) + ( y * y ) + ( z * z ) );
	float j = ( ( sinf( a ) * e ) / ( a + consts::FLOAT_MIN_NORM ) );
	x *= j;
	y *= j;
	z *= j;
	w = ( cosf( a ) * e );
}

void quat::Negate( void )
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
}

void quat::Invert( void )
{
	float n = ( 1.0f / ( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) + consts::FLOAT_MIN_NORM ) );
	x = -( x * n );
	y = -( y * n );
	z = -( z * n );
	w = ( w * n );
}

void quat::Conjugate( void )
{
	x = -x;
	y = -y;
	z = -z;
}

void quat::Normalize( void )
{
	float n = ( 1.0f / ( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ) + consts::FLOAT_MIN_NORM ) );
	x *= n;
	y *= n;
	z *= n;
	w *= n;
}

void quat::Lerp( const quat& trg, const float t )
{
	SetLerp( *this, trg, t );
}

void quat::Slerp( const quat& trg, const float t )
{
	SetSlerp( *this, trg, t );
}

void quat::Blend( const quat& trg, const float t )
{
	SetBlend( *this, trg, t );
}

void quat::SetAdd( const quat& src, const quat& add )
{
	x = ( src.x + add.x );
	y = ( src.y + add.y );
	z = ( src.z + add.z );
	w = ( src.w + add.w );
}

void quat::SetSub( const quat& src, const quat& sub )
{
	x = ( src.x - sub.x );
	y = ( src.y - sub.y );
	z = ( src.z - sub.z );
	w = ( src.w - sub.w );
}

void quat::SetMul( const quat& src, const quat& mul )
{
	quat m( mul );
	quat s( src );
	x = ( ( s.x * m.w ) - ( s.y * m.z ) + ( s.z * m.y ) + ( s.w * m.x ) );
	y = ( ( s.y * m.w ) + ( s.x * m.z ) + ( s.w * m.y ) - ( s.z * m.x ) );
	z = ( ( s.z * m.w ) + ( s.w * m.z ) - ( s.x * m.y ) + ( s.y * m.x ) );
	w = ( ( s.w * m.w ) - ( s.z * m.z ) - ( s.y * m.y ) - ( s.x * m.x ) );
}

void quat::SetMulByConjugate( const quat& src, const quat& conj )
{
	quat m( conj );
	quat s( src );
	x = ( ( s.x * m.w ) + ( s.y * m.z ) - ( s.z * m.y ) - ( s.w * m.x ) );
	y = ( ( s.y * m.w ) - ( s.x * m.z ) - ( s.w * m.y ) + ( s.z * m.x ) );
	z = ( ( s.z * m.w ) - ( s.w * m.z ) + ( s.x * m.y ) - ( s.y * m.x ) );
	w = ( ( s.w * m.w ) + ( s.z * m.z ) + ( s.y * m.y ) + ( s.x * m.x ) );
}

void quat::SetMul( const quat& src, const float mul )
{
	x = ( src.x * mul );
	y = ( src.y * mul );
	z = ( src.z * mul );
	w = ( src.w * mul );
}

void quat::SetDiv( const quat& src, const quat& div )
{
	quat d( div );
	float n = ( 1.0f / ( ( d.x * d.x ) + ( d.y * d.y ) + ( d.z * d.z ) + ( d.w * d.w ) + consts::FLOAT_MIN_NORM ) );
	quat s( src );
	x = ( ( ( s.x * d.w ) + ( s.y * d.z ) - ( s.z * d.y ) - ( s.w * d.x ) ) * n );
	y = ( ( ( s.y * d.w ) - ( s.x * d.z ) - ( s.w * d.y ) + ( s.z * d.x ) ) * n );
	z = ( ( ( s.z * d.w ) - ( s.w * d.z ) + ( s.x * d.y ) - ( s.y * d.x ) ) * n );
	w = ( ( ( s.w * d.w ) + ( s.z * d.z ) + ( s.y * d.y ) + ( s.x * d.x ) ) * n );
}

void quat::SetDiv( const quat& src, const float div )
{
	ASSERT( fabsf( div ) >= consts::FLOAT_MIN_RCP );
	if( fabsf( div ) >= consts::FLOAT_MIN_RCP )
	{
		SetMul( src, ( 1.0f / div ) );
	}
	else
	{
		x = y = z = 0.0f;
		w = 1.0f;
	}
}

void quat::SetPow( const quat& src, const quat& pow )
{
	Set( src );
	Pow( pow );
}

void quat::SetPow( const quat& src, const float pow )
{
	Set( src );
	Pow( pow );
}

void quat::SetLog( const quat& src )
{
	Set( src );
	Log();
}

void quat::SetExp( const quat& src )
{
	Set( src );
	Exp();
}

void quat::SetNegate( const quat& src )
{
	x = -src.x;
	y = -src.y;
	z = -src.z;
	w = -src.w;
}

void quat::SetInverse( const quat& src )
{
	Set( src );
	Invert();
}

void quat::SetConjugate( const quat& src )
{
	x = -src.x;
	y = -src.y;
	z = -src.z;
	w = src.w;
}

void quat::SetNormalized( const quat& src )
{
	Set( src );
	Normalize();
}

void quat::SetLerp( const quat& src, const quat& trg, const float t )
{
	float s = ( 1.0f - t );
	if( ( ( src.x * trg.x ) + ( src.y * trg.y ) + ( src.z * trg.z ) + ( src.w * trg.w ) ) < 0.0f )
	{
		x = ( ( src.x * s ) - ( trg.x * t ) );
		y = ( ( src.y * s ) - ( trg.y * t ) );
		z = ( ( src.z * s ) - ( trg.z * t ) );
		w = ( ( src.w * s ) - ( trg.w * t ) );
	}
	else
	{
		x = ( ( src.x * s ) + ( trg.x * t ) );
		y = ( ( src.y * s ) + ( trg.y * t ) );
		z = ( ( src.z * s ) + ( trg.z * t ) );
		w = ( ( src.w * s ) + ( trg.w * t ) );
	}
}

void quat::SetSlerp( const quat& src, const quat& trg, const float t )
{
	quat mul;
	mul.SetDelta( src, trg );
	mul.Pow( t );
	SetMul( src, mul );
}

void quat::SetBlend( const quat& src, const quat& trg, const float t )
{
	float s = ( 1.0f - t );
	x = ( ( src.x * s ) + ( trg.x * t ) );
	y = ( ( src.y * s ) + ( trg.y * t ) );
	z = ( ( src.z * s ) + ( trg.z * t ) );
	w = ( ( src.w * s ) + ( trg.w * t ) );
}

void quat::SetDelta( const quat& src, const quat& trg )
{
	SetInverse( src );
	Mul( trg );
}

void quat::SetPoint( const float px, const float py, const float pz )
{
	x = px;
	y = py;
	z = pz;
	w = 0.0f;
}

void quat::SetPoint( const vec3& point )
{
	x = point.x;
	y = point.y;
	z = point.z;
	w = 0.0f;
}

void quat::SetRemap( const vec3& src, const vec3& trg )
{
	x = ( ( src.y * trg.z ) - ( src.z * trg.y ) );
	y = ( ( src.z * trg.x ) - ( src.x * trg.z ) );
	z = ( ( src.x * trg.y ) - ( src.y * trg.x ) );
	float n = ( ( x * x ) + ( y * y ) + ( z * z ) );
	ASSERT( n >= consts::FLOAT_MIN_RCP );
	if( n >= consts::FLOAT_MIN_RCP )
	{	//	rotation and scaling remap
		float s = ( 1.0f / ( ( src.x * src.x ) + ( src.y * src.y ) + ( src.z * src.z ) ) );
		float c = ( ( ( src.x * trg.x ) + ( src.y * trg.y ) + ( src.z * trg.z ) ) * s );
		s = sqrtf( ( ( trg.x * trg.x ) + ( trg.y * trg.y ) + ( trg.z * trg.z ) ) * s );
		n = sqrtf( ( ( s - c ) * 0.5f ) / n );
		x *= n;
		y *= n;
		z *= n;
		w = sqrtf( ( s + c ) * 0.5f );
	}
	else
	{	//	zero rotation remap may be possible
		float t = ( ( trg.x * trg.x ) + ( trg.y * trg.y ) + ( trg.z * trg.z ) );
		if( t > consts::EPSILON )
		{	//	zero rotation remap may be possible
			float s = ( ( src.x * src.x ) + ( src.y * src.y ) + ( src.z * src.z ) );
			if( s > consts::EPSILON )
			{	//	zero rotation remap
				if( ( ( src.x * trg.x ) + ( src.y * trg.y ) + ( src.z * trg.z ) ) < 0.0f )
				{	//	inversion scaling remap
					if( ( trg.x > trg.y ) && ( trg.x > trg.z ) )
					{
						x = ( trg.y - trg.z );
						y = -trg.x;
						z = trg.x;
					}
					else if( trg.y > trg.z )
					{
						x = trg.y;
						y = ( trg.z - trg.x );
						z = -trg.y;
					}
					else
					{
						x = -trg.z;
						y = trg.z;
						z = ( trg.x - trg.y );
					}
					n = sqrtf( sqrtf( 4.0f / ( s * t ) ) );
					x *= n;
					y *= n;
					z *= n;
					w = 0.0f;
				}
				else
				{	//	scaling remap
					x = y = z = 0.0f;
					w = sqrtf( sqrtf( t / s ) );
				}
			}
			else
			{	//	no remap possible
				ASSERT( s > consts::EPSILON );	//	guaranteed assert
				x = y = z = 0.0f;
				w = 1.0f;
			}
		}
		else
		{	//	remap to zero vector
			x = y = z = w = 0.0f;
		}
	}
}

void quat::SetNormalized( const float ni, const float nj, const float nk, const float nr )
{
	Set( ni, nj, nk, nr );
	Normalize();
}

void quat::SetRollPitchYaw( const float roll, const float pitch, const float yaw )
{
	float a;
	a = ( roll * 0.5f );
	float cr = cosf( a );
	float sr = sinf( a );
	a = ( pitch * 0.5f );
	float cp = cosf( a );
	float sp = sinf( a );
	a = ( yaw * 0.5f );
	float cy = cosf( a );
	float sy = sinf( a );
	float srsp = ( sr * sp );
	float crsp = ( cr * sp );
	float srcp = ( sr * cp );
	float crcp = ( cr * cp );
	x = ( ( crsp * cy ) - ( srcp * sy ) );
	y = ( ( srsp * cy ) + ( crcp * sy ) );
	z = ( ( srcp * cy ) + ( crsp * sy ) );
	w = ( ( crcp * cy ) - ( srsp * sy ) );
}

void quat::SetYawPitchRoll( const float yaw, const float pitch, const float roll )
{
	float a;
	a = ( roll * 0.5f );
	float cr = cosf( a );
	float sr = sinf( a );
	a = ( pitch * 0.5f );
	float cp = cosf( a );
	float sp = sinf( a );
	a = ( yaw * 0.5f );
	float cy = cosf( a );
	float sy = sinf( a );
	float srsy = ( sr * sy );
	float crsy = ( cr * sy );
	float srcy = ( sr * cy );
	float crcy = ( cr * cy );
	x = ( ( srsy * cp ) + ( crcy * sp ) );
	y = ( ( crsy * cp ) - ( srcy * sp ) );
	z = ( ( srcy * cp ) - ( crsy * sp ) );
	w = ( ( crcy * cp ) + ( srsy * sp ) );
}

void quat::SetPitchYaw( const float pitch, const float yaw )
{
	float a;
	a = ( pitch * 0.5f );
	float cp = cosf( a );
	float sp = sinf( a );
	a = ( yaw * 0.5f );
	float cy = cosf( a );
	float sy = sinf( a );
	x = ( sp * cy );
	y = ( cp * sy );
	z = ( sp * sy );
	w = ( cp * cy );
}

void quat::SetYawPitch( const float yaw, const float pitch )
{
	float a;
	a = ( pitch * 0.5f );
	float cp = cosf( a );
	float sp = sinf( a );
	a = ( yaw * 0.5f );
	float cy = cosf( a );
	float sy = sinf( a );
	x =  ( cy * sp );
	y =  ( sy * cp );
	z = -( sy * sp );
	w =  ( cy * cp );
}

void quat::SetAxisAngle( const float axis_x, const float axis_y, const float axis_z, const float radians )
{
	float a = ( radians * 0.5f );
	float s = ( sinf( a ) / ( sqrtf( ( axis_x * axis_x ) + ( axis_y * axis_y ) + ( axis_z * axis_z ) ) + consts::FLOAT_MIN_NORM ) );
	x = ( axis_x * s );
	y = ( axis_y * s );
	z = ( axis_z * s );
	w = cosf( a );
}

void quat::SetAxisAngle( const vec3& axis, const float radians )
{
	SetAxisAngle( axis.x, axis.y, axis.z, radians );
}

void quat::SetScaling( const float scale )
{
	ASSERT( scale >= 0.0f );
	if( scale >= 0.0f )
	{
		float n = sqrtf( scale / ( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) + consts::FLOAT_MIN_NORM ) );
		x *= n;
		y *= n;
		z *= n;
		w *= n;
	}
}

void quat::Set( const quat& q )
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

void quat::Set( const mat33& m )
{
	float d = fabsf( m.z.Triple( m.x, m.y ) );
	ASSERT( d > consts::FLOAT_MAX_DENORM );
	d = ( ( d > consts::FLOAT_MAX_DENORM ) ? expf( logf( d ) / 3.0f ) : 1.0f );
	if( ( m.x.x + m.y.y + m.z.z ) > 0.0f )
	{
		float t = ( d + m.x.x + m.y.y + m.z.z );
		float s = ( 0.5f / sqrtf( t ) );
		w = ( t * s );
		z = ( ( m.y.x - m.x.y ) * s );
		y = ( ( m.x.z - m.z.x ) * s );
		x = ( ( m.z.y - m.y.z ) * s );
	}
	else if( ( m.x.x > m.y.y ) && ( m.x.x > m.z.z ) )
	{
		float t = ( d + m.x.x - m.y.y - m.z.z );
		float s = ( 0.5f / sqrtf( t ) );
		x = ( t * s );
		y = ( ( m.y.x + m.x.y ) * s );
		z = ( ( m.x.z + m.z.x ) * s );
		w = ( ( m.z.y - m.y.z ) * s );
	}
	else if( m.y.y > m.z.z )
	{
		float t = ( d - m.x.x + m.y.y - m.z.z );
		float s = ( 0.5f / sqrtf( t ) );
		y = ( t * s );
		x = ( ( m.y.x + m.x.y ) * s );
		w = ( ( m.x.z - m.z.x ) * s );
		z = ( ( m.z.y + m.y.z ) * s );
	}
	else
	{
		float t = ( d - m.x.x - m.y.y + m.z.z );
		float s = ( 0.5f / sqrtf( t ) );
		z = ( t * s );
		w = ( ( m.y.x - m.x.y ) * s );
		x = ( ( m.x.z + m.z.x ) * s );
		y = ( ( m.z.y + m.y.z ) * s );
	}
}

void quat::Set( const float i, const float j, const float k, const float r )
{
	x = i;
	y = j;
	z = k;
	w = r;
}

void quat::GetNormalized( float& ni, float& nj, float& nk, float& nr ) const
{
	float n = ( 1.0f / ( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ) + consts::FLOAT_MIN_NORM ) );
	ni = ( x * n );
	nj = ( y * n );
	nk = ( z * n );
	nr = ( w * n );
}

void quat::GetRollPitchYaw( float& roll, float& pitch, float& yaw ) const
{
	float n = ( 1.0f / ( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ) + consts::FLOAT_MIN_NORM ) );
	quat q = { ( x * n ), ( y * n ), ( z * n ), ( w * n ) };
	yaw = atan2f( ( ( q.w * q.y ) - ( q.z * q.x ) ), ( 0.5f - ( q.x * q.x ) - ( q.y * q.y ) ) );
	q.Yaw( -yaw );
	pitch = asinf( ( ( q.w * q.x ) + ( q.y * q.z ) ) * 2.0f );
	q.Pitch( -pitch );
	roll = atan2f( ( ( q.w * q.z ) + ( q.x * q.y ) ), ( 0.5f - ( q.z * q.z ) - ( q.x * q.x ) ) );
}

void quat::GetYawPitchRoll( float& yaw, float& pitch, float& roll ) const
{
	float n = ( 1.0f / ( sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ) + consts::FLOAT_MIN_NORM ) );
	quat q = { ( x * n ), ( y * n ), ( z * n ), ( w * n ) };
	roll = atan2f( ( ( q.w * q.z ) + ( q.x * q.y ) ), ( 0.5f - ( q.z * q.z ) - ( q.x * q.x ) ) );
	q.Roll( -roll );
	pitch = asinf( ( ( q.w * q.x ) - ( q.y * q.z ) ) * 2.0f );
	q.Pitch( -pitch );
	yaw = atan2f( ( ( q.w * q.y ) - ( q.z * q.x ) ), ( 0.5f - ( q.x * q.x ) - ( q.y * q.y ) ) );
}

void quat::GetAxisAngle( float& axis_x, float& axis_y, float& axis_z, float& radians ) const
{
	float n = ( ( x * x ) + ( y * y ) + ( z * z ) );
	radians = ( acosf( w / ( sqrtf( n + ( w * w ) ) + consts::FLOAT_MIN_NORM ) ) * 2.0f );
	n = sqrtf( n );
	if( n >= consts::FLOAT_MIN_RCP )
	{
		n = ( 1.0f / n );
		axis_x = ( x * n );
		axis_y = ( y * n );
		axis_z = ( z * n );
	}
	else
	{
		radians = 0.0f;
		axis_x = 0.0f;
		axis_y = 0.0f;
		axis_z = 1.0f;
	}
}

void quat::GetAxisAngle( vec3& axis, float& radians ) const
{
	GetAxisAngle( axis.x, axis.y, axis.z, radians );
}

void quat::Get( quat& q ) const
{
	q.x = x;
	q.y = y;
	q.z = z;
	q.w = w;
}

void quat::Get( mat33& m ) const
{
	{
		float xx = ( x * x );
		float yy = ( y * y );
		float zz = ( z * z );
		float ww = ( w * w );
		m.x.x = ( ww + xx - yy - zz );
		m.y.y = ( ww - xx + yy - zz );
		m.z.z = ( ww - xx - yy + zz );
	}
	{
		float x2 = ( x + x );
		float y2 = ( y + y );
		float z2 = ( z + z );
		{
			float yz = ( y * z2 );
			float wx = ( w * x2 );
			m.z.y = ( yz + wx );
			m.y.z = ( yz - wx );
		}
		{
			float zx = ( z * x2 );
			float wy = ( w * y2 );
			m.x.z = ( zx + wy );
			m.z.x = ( zx - wy );
		}
		{
			float xy = ( x * y2 );
			float wz = ( w * z2 );
			m.y.x = ( xy + wz );
			m.x.y = ( xy - wz );
		}
	}
}

void quat::Get( float& i, float& j, float& k, float& r ) const
{
	i = x;
	j = y;
	k = z;
	r = w;
}

void quat::GetInverse( mat33& m ) const
{
	quat q;
	q.SetInverse( *this );
	q.Get( m );
}

void quat::GetInverseTranspose( mat33& m ) const
{
	quat q;
	q.SetInverse( *this );
	q.Conjugate();
	q.Get( m );
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
