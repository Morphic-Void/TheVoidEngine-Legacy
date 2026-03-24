
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   ray.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Ray structure and functions.
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

#include "ray.h"
#include "aabb.h"
#include "plane.h"
#include "sphere.h"
#include "consts.h"
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
////    ray
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
bool ray::IsNan( void ) const
{
	return( pos.IsNan() || dir.IsNan() );
}

bool ray::IsReal( void ) const
{
	return( pos.IsReal() && dir.IsReal() );
}

bool ray::GetIntersect( const plane& aplane, float& t ) const
{
	t = 0;
	float d = ( ( dir.x * aplane.x ) + ( dir.y * aplane.y ) + ( dir.z * aplane.z ) );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{	//	intersects plane
		t = -( ( ( pos.x * aplane.x ) + ( pos.y * aplane.y ) + ( pos.z * aplane.z ) + aplane.w ) / d );
		return( true );
	}
	return( false );
}

bool ray::GetIntersect( const sphere& asphere, float& t ) const
{
	return( GetIntersect( asphere.t_vec3(), asphere.r, t ) );
}

bool ray::GetIntersect( const vec3& apoint, const float radius, float& t ) const
{
	t = 0;
	float d = dir.SqrLen();
	if( d >= consts::FLOAT_MIN_RCP )
	{	//	ray has length
		vec3 gap;
		gap.SetSub( apoint, pos );
		t = ( dir.Dot( gap ) / d );
		gap.x -= ( dir.x * t );
		gap.y -= ( dir.y * t );
		gap.z -= ( dir.z * t );
		float r = ( ( radius * radius ) - gap.SqrLen() );
		if( r >= 0 )
		{	//	ray intersects sphere
			t -= sqrtf( r / d );
			return( true );
		}
	}
	return( false );
}

bool ray::GetIntersect( const plane& aplane, const float radius, float& t ) const
{
	t = 0;
	float d = ( ( dir.x * aplane.x ) + ( dir.y * aplane.y ) + ( dir.z * aplane.z ) );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{	//	intersects plane
		float p = ( ( pos.x * aplane.x ) + ( pos.y * aplane.y ) + ( pos.z * aplane.z ) + aplane.w );
		float r = ( radius * sqrtf( aplane.t_vec3().SqrLen() ) );
		if( p < 0 )
		{
			t = -( ( p + r ) / d );
		}
		else
		{
			t = -( ( p - r ) / d );
		}
		return( true );
	}
	return( false );
}

bool ray::GetNormalizedPlaneIntersect( const plane& aplane, const float radius, float& t ) const
{
	t = 0;
	float d = ( ( dir.x * aplane.x ) + ( dir.y * aplane.y ) + ( dir.z * aplane.z ) );
	if( fabsf( d ) >= consts::FLOAT_MIN_RCP )
	{	//	intersects plane
		float p = ( ( pos.x * aplane.x ) + ( pos.y * aplane.y ) + ( pos.z * aplane.z ) + aplane.w );
		if( p < 0 )
		{
			t = -( ( p + radius ) / d );
		}
		else
		{
			t = -( ( p - radius ) / d );
		}
		return( true );
	}
	return( false );
}

bool ray::GetIntersect( const aabb& abox, vec3& intersection, vec3& normal, float& t ) const
{
	t = 0;
	normal.x = normal.y = normal.z = 0;
	if( fabsf( dir.x ) >= consts::FLOAT_MIN_RCP )
	{	//	possible x intersect
		if( dir.x < 0 )
		{
			t = ( ( pos.x - abox.max.x ) / dir.x );
			intersection.y = ( pos.y + ( dir.y * t ) );
			if( ( intersection.y >= abox.min.y ) && ( intersection.y <= abox.max.y ) )
			{
				intersection.z = ( pos.z + ( dir.z * t ) );
				if( ( intersection.z >= abox.min.z ) && ( intersection.z <= abox.max.z ) )
				{
					intersection.x = abox.max.x;
					normal.x = 1;
					return( true );
				}
			}
		}
		else
		{
			t = ( ( pos.x - abox.min.x ) / dir.x );
			intersection.y = ( pos.y + ( dir.y * t ) );
			if( ( intersection.y >= abox.min.y ) && ( intersection.y <= abox.max.y ) )
			{
				intersection.z = ( pos.z + ( dir.z * t ) );
				if( ( intersection.z >= abox.min.z ) && ( intersection.z <= abox.max.z ) )
				{
					intersection.x = abox.min.x;
					normal.x = -1;
					return( true );
				}
			}
		}
	}
	if( fabsf( dir.y ) >= consts::FLOAT_MIN_RCP )
	{	//	possible y intersect
		if( dir.y < 0 )
		{
			t = ( ( pos.y - abox.max.y ) / dir.y );
			intersection.z = ( pos.z + ( dir.z * t ) );
			if( ( intersection.z >= abox.min.z ) && ( intersection.z <= abox.max.z ) )
			{
				intersection.x = ( pos.x + ( dir.x * t ) );
				if( ( intersection.x >= abox.min.x ) && ( intersection.x <= abox.max.x ) )
				{
					intersection.y = abox.max.y;
					normal.y = 1;
					return( true );
				}
			}
		}
		else
		{
			t = ( ( pos.y - abox.min.y ) / dir.y );
			intersection.z = ( pos.z + ( dir.z * t ) );
			if( ( intersection.z >= abox.min.z ) && ( intersection.z <= abox.max.z ) )
			{
				intersection.x = ( pos.x + ( dir.x * t ) );
				if( ( intersection.x >= abox.min.x ) && ( intersection.x <= abox.max.x ) )
				{
					intersection.y = abox.min.y;
					normal.y = -1;
					return( true );
				}
			}
		}
	}
	if( fabsf( dir.z ) >= consts::FLOAT_MIN_RCP )
	{	//	possible z intersect
		if( dir.z < 0 )
		{
			t = ( ( pos.z - abox.max.z ) / dir.z );
			intersection.x = ( pos.x + ( dir.x * t ) );
			if( ( intersection.x >= abox.min.x ) && ( intersection.x <= abox.max.x ) )
			{
				intersection.y = ( pos.y + ( dir.y * t ) );
				if( ( intersection.y >= abox.min.y ) && ( intersection.y <= abox.max.y ) )
				{
					intersection.z = abox.max.z;
					normal.z = 1;
					return( true );
				}
			}
		}
		else
		{
			t = ( ( pos.z - abox.min.z ) / dir.z );
			intersection.x = ( pos.x + ( dir.x * t ) );
			if( ( intersection.x >= abox.min.x ) && ( intersection.x <= abox.max.x ) )
			{
				intersection.y = ( pos.y + ( dir.y * t ) );
				if( ( intersection.y >= abox.min.y ) && ( intersection.y <= abox.max.y ) )
				{
					intersection.z = abox.min.z;
					normal.z = -1;
					return( true );
				}
			}
		}
	}
	return( false );
}

bool ray::GetIntersect( const vec3& vtx1, const vec3& vtx2, const vec3& vtx3, vec3& intersection, vec3& normal, float& t ) const
{	//	check intersect with triangle (either face)
	vec3 du, dv;
	du.SetSub( vtx2, vtx1 );
	dv.SetSub( vtx3, vtx1 );
	plane aplane;
	aplane.t_vec3().SetCross( du, dv );
	aplane.w = -aplane.t_vec3().Dot( vtx1 );
	if( GetIntersect( aplane, t ) )
	{
		intersection.SetMul( dir, t );
		intersection.Add( pos );
		vec3 dp;
		dp.SetSub( intersection, vtx1 );
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
                    normal.Set( aplane.t_vec3() );
			        normal.Mul( sqrtf( normal.SqrLen() ) ); 
					return( true );
				}
			}
		}
	}
	return( false );
}

bool ray::GetIntersect( const vec3& apoint, const float radius, vec3& intersection, vec3& normal, float& t ) const
{
	t = 0;
	float d = dir.SqrLen();
	if( d >= consts::FLOAT_MIN_RCP )
	{	//	ray has length
		vec3 gap;
		gap.SetSub( apoint, pos );
		t = ( dir.Dot( gap ) / d );
		gap.x -= ( dir.x * t );
		gap.y -= ( dir.y * t );
		gap.z -= ( dir.z * t );
		float r = ( ( radius * radius ) - gap.SqrLen() );
		if( r >= 0 )
		{	//	ray either intersects sphere or starts inside the sphere
			t -= sqrtf( r / d );
			intersection.SetMul( dir, t );
			intersection.Add( pos );
			normal.SetSub( intersection, apoint );
			normal.Mul( sqrtf( normal.SqrLen() ) ); 
			return( true );
		}
	}
	return( false );
}

bool ray::GetIntersect( const vec3& start, const vec3& end, const float radius, vec3& intersection, vec3& normal, float& t ) const
{
	t = 0;
	vec3 axis;
	axis.SetSub( end, start );
	vec3 nu;
	nu.SetCross( dir, axis );
	float a = nu.SqrLen();
	if( a >= consts::FLOAT_MIN_RCP )
	{	///	the ray and axis are neither parallel nor zero length
		vec3 gap;
		gap.SetSub( pos, start );
		float b = nu.Dot( gap );
		a = ( ( radius * radius ) - ( ( b * b ) / a ) );
		if( a >= 0 )
		{	///	closest distance is less than or equal to the radius
			vec3 nv;
			nv.SetCross( nu, axis );
			t = ( ( nv.Dot( gap ) - sqrtf( a * nv.SqrLen() ) ) / nv.Dot( dir ) );
			gap.SetMul( dir, t );
			a = axis.Dot( gap );
			if( a < 0 ) return( GetIntersect( start, radius, intersection, normal, t ) );
			a *= a;
			b = axis.SqrLen();
			if( a > b ) return( GetIntersect( end, radius, intersection, normal, t ) );
			axis.Mul( sqrtf( a / b ) );
			intersection.SetAdd( gap, pos );
			normal.SetSub( gap, axis );
			normal.Mul( sqrtf( normal.SqrLen() ) ); 
			return( true );
		}
		return( false );
	}
	return( GetIntersect( ( ( axis.Dot( dir ) < 0 ) ? end : start ), radius, intersection, normal, t ) );
}

bool ray::GetIntersect( const vec3& vtx1, const vec3& vtx2, const vec3& vtx3, const float radius, vec3& intersection, vec3& normal, float& t ) const
{	//	check intersect with triangle (either face)
	vec3 du, dv;
	du.SetSub( vtx2, vtx1 );
	dv.SetSub( vtx3, vtx1 );
	plane aplane;
	aplane.t_vec3().SetCross( du, dv );
	aplane.w = -aplane.t_vec3().Dot( vtx1 );
	if( GetIntersect( aplane, radius, t ) )
	{
		intersection.SetMul( dir, t );
		intersection.Add( pos );
		vec3 dp;
		dp.SetSub( intersection, vtx1 );
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
					normal.Mul( 1.0f / sqrtf( normal.SqrLen() ) );
					return( true );
				}
			}
		}
		const vec3* const vtx[ 4 ] = { &vtx1, &vtx2, &vtx3, &vtx1 };
		float rr = ( radius * radius );
		for( int i = 0; i < 3; ++i )
		{	///	no intersect within triangle, test edges
			vec3 axis;
			axis.SetSub( *vtx[ i + 1 ], *vtx[ i ] );
			du.SetCross( dir, axis );
			float a = du.SqrLen();
			if( a >= consts::FLOAT_MIN_RCP )
			{	///	the ray and axis are neither parallel nor zero length
				vec3 gap;
				gap.SetSub( pos, *vtx[ i ] );
				float b = du.Dot( gap );
				a = ( rr - ( ( b * b ) / a ) );
				if( a >= 0 )
				{	///	closest distance is less than or equal to the radius
					dv.SetCross( du, axis );
					t = ( ( dv.Dot( gap ) - sqrtf( a * dv.SqrLen() ) ) / dv.Dot( dir ) );
					gap.SetMul( dir, t );
					a = axis.Dot( gap );
					if( a >= 0 )
					{	///	intersect is after segment start
						a *= a;
						b = axis.SqrLen();
						if( a <= b )
						{	///	intersect is before segment end
							axis.Mul( sqrtf( a / b ) );
							intersection.SetAdd( gap, pos );
							normal.SetSub( gap, axis );
							normal.Mul( sqrtf( normal.SqrLen() ) ); 
							return( true );
						}
					}
				}
			}
		}
		float d = dir.SqrLen();
		for( int i = 0; i < 3; ++i )
		{	///	no intersect within triangle or with edges, test vertices
			vec3 gap;
			gap.SetSub( *vtx[ i ], pos );
			t = ( dir.Dot( gap ) / d );
			gap.x -= ( dir.x * t );
			gap.y -= ( dir.y * t );
			gap.z -= ( dir.z * t );
			float r = ( ( radius * radius ) - gap.SqrLen() );
			if( r >= 0 )
			{	//	ray either intersects sphere or starts inside the sphere
				t -= sqrtf( r / d );
				intersection.SetMul( dir, t );
				intersection.Add( pos );
				normal.SetSub( intersection, *vtx[ i ] );
				normal.Mul( sqrtf( normal.SqrLen() ) ); 
				return( true );
			}
		}
	}
	return( false );
}

void ray::Get( plane& aplane ) const
{
	aplane.x = dir.x;
	aplane.y = dir.y;
	aplane.z = dir.z;
	aplane.w = -( ( pos.x * dir.x ) + ( pos.y * dir.y ) + ( pos.z * dir.z ) );
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
