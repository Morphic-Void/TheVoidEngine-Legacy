
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   clipper.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Frustum clip space manipulation class.
////
////    Notes:
////
////    	Lerping always takes the handedness of the target clipper.
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

#include "clipper.h"
#include "libs/maths/frect.h"
#include "libs/maths/mat43.h"
#include "libs/maths/mat44.h"
#include "libs/maths/consts.h"
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
////    clipper
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool clipper::IsNan( void ) const
{
	return( o.IsNan() || r.IsNan() );
}

bool clipper::IsReal( void ) const
{
	return( o.IsReal() && r.IsReal() );
}

void clipper::ClipDX( void )
{
	o.z = ( ( o.z * 0.5f ) + 0.5f );
	r.z *= 0.5f;
}

void clipper::Zoom( const frect& zoom )
{
	float x = ( zoom.r - zoom.l );
	float y = ( zoom.t - zoom.b );
	ASSERT( ( fabsf( x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( y ) >= consts::FLOAT_MIN_RCP ) );
	if( fabsf( x ) >= consts::FLOAT_MIN_RCP )
	{
		x = ( 1 / x );
		o.x = ( ( o.x + 1 - zoom.r - zoom.l ) * x );
		r.x *= x;
	}
	if( fabsf( y ) >= consts::FLOAT_MIN_RCP )
	{
		y = ( 1 / y );
		o.y = ( ( o.y + 1 - zoom.t - zoom.b ) * y );
		r.y *= y;
	}	
}

void clipper::Lerp( const clipper& trg, const float t )
{
	SetLerp( *this, trg, t );
}
 
void clipper::Chain( const clipper& chain )
{
	SetChain( *this, chain );
}

void clipper::Remap( const aabb& src, const aabb& trg )
{
	clipper tmp;
	tmp.SetRemap( src, trg );
	SetChain( *this, tmp );
}

void clipper::SetDX( const clipper& src )
{
	o.x = src.o.x;
	o.y = src.o.y;
	o.z = ( ( src.o.z * 0.5f ) + 0.5f );
	r.x = src.r.x;
	r.y = src.r.y;
	r.z = ( src.r.z * 0.5f );
	r.w = src.r.w;
}

void clipper::SetZoom( const frect& zoom )
{
	float x = ( zoom.r - zoom.l );
	float y = ( zoom.t - zoom.b );
	ASSERT( ( fabsf( x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( y ) >= consts::FLOAT_MIN_RCP ) );
	if( fabsf( x ) >= consts::FLOAT_MIN_RCP )
	{
		x = ( 1 / x );
		o.x = ( ( 1 - zoom.r - zoom.l ) * x );
		r.x = x;
	}
	else
	{
		o.x = 0;
		r.x = 1;
	}
	if( fabsf( y ) >= consts::FLOAT_MIN_RCP )
	{
		y = ( 1 / y );
		o.y = ( ( 1 - zoom.t - zoom.b ) * y );
		r.y = y;
	}
	else
	{
		o.y = 0;
		r.y = 1;
	}
	o.z = 0;
	r.z = r.w = 1;
}

void clipper::SetLerp( const clipper& src, const clipper& trg, const float t )
{
	float s = ( 1 - t );
	o.x = ( ( src.o.x * s ) + ( trg.o.x * t ) );
	o.y = ( ( src.o.y * s ) + ( trg.o.y * t ) );
	o.z = ( ( src.o.z * s ) + ( trg.o.z * t ) );
	r.x = ( ( src.r.x * s ) + ( trg.r.x * t ) );
	r.y = ( ( src.r.y * s ) + ( trg.r.y * t ) );
	r.z = ( ( src.r.z * s ) + ( trg.r.z * t ) );
	r.w = trg.r.w;
}

void clipper::SetChain( const clipper& first, const clipper& chain )
{
	o.x = ( ( first.o.x * chain.r.x ) + chain.o.x );
	o.y = ( ( first.o.y * chain.r.y ) + chain.o.y );
	o.z = ( ( first.o.z * chain.r.z ) + chain.o.z );
	r.x = ( first.r.x * chain.r.x );
	r.y = ( first.r.y * chain.r.y );
	r.z = ( first.r.z * chain.r.z );
	r.w = ( first.r.w * chain.r.w );
}

void clipper::SetRemap( const aabb& src, const aabb& trg )
{
	float x = ( src.max.x - src.min.x );
	float y = ( src.max.y - src.min.y );
	float z = ( src.max.z - src.min.z );
	ASSERT( ( fabsf( x ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( y ) >= consts::FLOAT_MIN_RCP ) && ( fabsf( z ) >= consts::FLOAT_MIN_RCP ) );
	if( fabsf( x ) >= consts::FLOAT_MIN_RCP )
	{
		o.x = ( ( ( src.max.x * trg.min.x ) - ( trg.max.x * src.min.x ) ) / x );
		r.x = ( ( trg.max.x - trg.min.x ) / x );
	}
	else
	{
		o.x = 0;
		r.x = 1;
	}
	if( fabsf( y ) >= consts::FLOAT_MIN_RCP )
	{
		o.y = ( ( ( src.max.y * trg.min.y ) - ( trg.max.y * src.min.y ) ) / y );
		r.y = ( ( trg.max.y - trg.min.y ) / y );
	}
	else
	{
		o.y = 0;
		r.y = 1;
	}
	if( fabsf( z ) >= consts::FLOAT_MIN_RCP )
	{
		o.z = ( ( ( src.max.z * trg.min.z ) - ( trg.max.z * src.min.z ) ) / z );
		r.z = ( ( trg.max.z - trg.min.z ) / z );
	}
	else
	{
		o.z = 0;
		r.z = 1;
	}
	r.w = 1;
}

void clipper::SetDefaults( void )
{
	o.x = o.y = o.z = 0;
	r.x = r.y = r.z = 1;
	r.w = 1;
}

void clipper::SetClip( const aabb& clip )
{
	o.x = ( ( clip.min.x + clip.max.x ) * 0.5f );
	o.y = ( ( clip.min.y + clip.max.y ) * 0.5f );
	o.z = ( ( clip.min.z + clip.max.z ) * 0.5f );
	r.x = ( ( clip.max.x - clip.min.x ) * 0.5f );
	r.y = ( ( clip.max.y - clip.min.y ) * 0.5f );
	r.z = ( ( clip.max.z - clip.min.z ) * 0.5f );
}

void clipper::SetClipGL( void )
{
	o.x = o.y = o.z = 0;
	r.x = r.y = r.z = 1;
	r.w = 1;
}

void clipper::SetClipDX( void )
{
	o.x = o.y = 0;
	r.x = r.y = 1;
	o.z = r.z = 0.5f;
	r.w = 1;
}

void clipper::GetRemapAsSource( aabb& remap ) const
{
	if( fabsf( r.x ) >= consts::FLOAT_MIN_RCP )
	{
		float d = ( 1 / r.x );
		float c = ( o.x * d );
		remap.min.x = -( c + d );
		remap.max.x = -( c - d );
	}
	else
	{
		remap.min.x = remap.max.x = -o.x;
	}
	if( fabsf( r.y ) >= consts::FLOAT_MIN_RCP )
	{
		float d = ( 1 / r.y );
		float c = ( o.y * d );
		remap.min.y = -( c + d );
		remap.max.y = -( c - d );
	}
	else
	{
		remap.min.y = remap.max.y = -o.y;
	}
	if( fabsf( r.z ) >= consts::FLOAT_MIN_RCP )
	{
		float d = ( 1 / r.z );
		float c = ( o.z * d );
		remap.min.z = -( c + d );
		remap.max.z = -( c - d );
	}
	else
	{
		remap.min.z = remap.max.z = -o.z;
	}
}
 
void clipper::GetRemapAsTarget( aabb& remap ) const
{
	remap.min.x = ( o.x - r.x );
	remap.min.y = ( o.y - r.y );
	remap.min.z = ( o.z - r.z );
	remap.max.x = ( o.x + r.x );
	remap.max.y = ( o.y + r.y );
	remap.max.z = ( o.z + r.z );
}
 
void clipper::Apply( mat43& trg ) const
{
	trg.x.x = ( trg.x.x * r.x );
	trg.x.y = ( trg.x.y * r.x );
	trg.x.z = ( trg.x.z * r.x * r.w );
	trg.x.w = ( ( trg.x.w * r.x ) + o.x );
	trg.y.x = ( trg.y.x * r.y );
	trg.y.y = ( trg.y.y * r.y );
	trg.y.z = ( trg.y.z * r.y * r.w );
	trg.y.w = ( ( trg.y.w * r.y ) + o.y );
	trg.z.x = ( trg.z.x * r.z );
	trg.z.y = ( trg.z.y * r.z );
	trg.z.z = ( trg.z.z * r.z * r.w );
	trg.z.w = ( ( trg.z.w * r.z ) + o.z );
}
 
void clipper::Apply( mat44& trg ) const
{
	trg.x.x = ( ( trg.x.x * r.x ) + ( trg.w.x * o.x ) );
	trg.x.y = ( ( trg.x.y * r.x ) + ( trg.w.y * o.x ) );
	trg.x.z = ( ( ( trg.x.z * r.x ) + ( trg.w.z * o.x ) ) * r.w );
	trg.x.w = ( ( trg.x.w * r.x ) + ( trg.w.w * o.x ) );
	trg.y.x = ( ( trg.y.x * r.y ) + ( trg.w.x * o.y ) );
	trg.y.y = ( ( trg.y.y * r.y ) + ( trg.w.y * o.y ) );
	trg.y.z = ( ( ( trg.y.z * r.y ) + ( trg.w.z * o.y ) ) * r.w );
	trg.y.w = ( ( trg.y.w * r.y ) + ( trg.w.w * o.y ) );
	trg.z.x = ( ( trg.z.x * r.z ) + ( trg.w.x * o.z ) );
	trg.z.y = ( ( trg.z.y * r.z ) + ( trg.w.y * o.z ) );
	trg.z.z = ( ( ( trg.z.z * r.z ) + ( trg.w.z * o.z ) ) * r.w );
	trg.z.w = ( ( trg.z.w * r.z ) + ( trg.w.w * o.z ) );
	trg.w.z *= r.w;
}
 
void clipper::Apply( const mat43& src, mat43& trg ) const
{
	trg.x.x = ( src.x.x * r.x );
	trg.x.y = ( src.x.y * r.x );
	trg.x.z = ( src.x.z * r.x * r.w );
	trg.x.w = ( ( src.x.w * r.x ) + o.x );
	trg.y.x = ( src.y.x * r.y );
	trg.y.y = ( src.y.y * r.y );
	trg.y.z = ( src.y.z * r.y * r.w );
	trg.y.w = ( ( src.y.w * r.y ) + o.y );
	trg.z.x = ( src.z.x * r.z );
	trg.z.y = ( src.z.y * r.z );
	trg.z.z = ( src.z.z * r.z * r.w );
	trg.z.w = ( ( src.z.w * r.z ) + o.z );
}

void clipper::Apply( const mat44& src, mat44& trg ) const
{
	trg.x.x = ( ( src.x.x * r.x ) + ( src.w.x * o.x ) );
	trg.x.y = ( ( src.x.y * r.x ) + ( src.w.y * o.x ) );
	trg.x.z = ( ( ( src.x.z * r.x ) + ( src.w.z * o.x ) ) * r.w );
	trg.x.w = ( ( src.x.w * r.x ) + ( src.w.w * o.x ) );
	trg.y.x = ( ( src.y.x * r.y ) + ( src.w.x * o.y ) );
	trg.y.y = ( ( src.y.y * r.y ) + ( src.w.y * o.y ) );
	trg.y.z = ( ( ( src.y.z * r.y ) + ( src.w.z * o.y ) ) * r.w );
	trg.y.w = ( ( src.y.w * r.y ) + ( src.w.w * o.y ) );
	trg.z.x = ( ( src.z.x * r.z ) + ( src.w.x * o.z ) );
	trg.z.y = ( ( src.z.y * r.z ) + ( src.w.y * o.z ) );
	trg.z.z = ( ( ( src.z.z * r.z ) + ( src.w.z * o.z ) ) * r.w );
	trg.z.w = ( ( src.z.w * r.z ) + ( src.w.w * o.z ) );
	trg.w.x = src.w.x;
	trg.w.y = src.w.y;
	trg.w.z = ( src.w.z * r.w );
	trg.w.w = src.w.w;
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
