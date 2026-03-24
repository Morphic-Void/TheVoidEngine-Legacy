
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   box_faces.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Container for 3d box face planes.
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

#include "box_faces.h"
#include "box_verts.h"
#include "aabb.h"
#include "sphere.h"
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
	namespace BOX_FACES
	{
		const box_faces	DEFAULT = { {
			{  1,  0,  0,  1 },		// BOX_FACE_L
			{ -1,  0,  0,  1 },		// BOX_FACE_R
			{  0, -1,  0,  1 },		// BOX_FACE_B
			{  0,  1,  0,  1 },		// BOX_FACE_T
			{  0,  0,  1,  1 },		// BOX_FACE_N
			{  0,  0, -1,  1 } } };	// BOX_FACE_F
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    box_faces
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool box_faces::IsNan( void ) const
{
	for( int face = 0; face < BOX_FACE_COUNT; ++face )
	{
		if( faces[ face ].IsNan() )
		{
			return( true );
		}
	}
	return( false );
}

bool box_faces::IsReal( void ) const
{
	for( int face = 0; face < BOX_FACE_COUNT; ++face )
	{
		if( !faces[ face ].IsReal() )
		{
			return( false );
		}
	}
	return( true );
}

const plane& box_faces::GetFace( const BOX_FACE face ) const
{
	ASSERT( face < BOX_FACE_COUNT );
#ifdef _DEBUG
	return( faces[ face < BOX_FACE_COUNT ? face : BOX_FACE_L ] );
#else
	return( faces[ face ] );
#endif
}

bool box_faces::GetVerts( box_verts& verts ) const
{
	int i = 0;
	vec3 zy, yx, xz;
	for( int lr = 0; lr < 2; ++lr )
	{
		const plane& x = faces[ BOX_FACE_L + lr ];
		for( int bt = 0; bt < 2; ++bt )
		{
			const plane& y = faces[ BOX_FACE_B + bt ];
			yx.x = ( ( y.y * x.z ) - ( y.z * x.y ) );
			yx.y = ( ( y.z * x.x ) - ( y.x * x.z ) );
			yx.z = ( ( y.x * x.y ) - ( y.y * x.x ) );
			for( int nf = 0; nf < 2; ++nf )
			{	   //	  -( d1( n2 x n3 ) + d2( n3 x n1 ) + d3( n1 x n2 ) ) / ( n1 . ( n2 x n3 ) )
				const plane& z = faces[ BOX_FACE_N + nf ];
				xz.x = ( ( x.y * z.z ) - ( x.z * z.y ) );
				xz.y = ( ( x.z * z.x ) - ( x.x * z.z ) );
				xz.z = ( ( x.x * z.y ) - ( x.y * z.x ) );
				zy.x = ( ( z.y * y.z ) - ( z.z * y.y ) );
				zy.y = ( ( z.z * y.x ) - ( z.x * y.z ) );
				zy.z = ( ( z.x * y.y ) - ( z.y * y.x ) );
				float n = ( ( z.x * yx.x ) + ( z.y * yx.y ) + ( z.z * yx.z ) );
				ASSERT( fabsf( n ) >= consts::FLOAT_MIN_RCP );
				if( fabsf( n ) >= consts::FLOAT_MIN_RCP )
				{
					n = ( -1.0f / n );
					vec3& vert = verts.verts[ i ];
					vert.x = ( ( ( x.w * zy.x ) + ( y.w * xz.x ) + ( z.w * yx.x ) ) * n );
					vert.y = ( ( ( x.w * zy.y ) + ( y.w * xz.y ) + ( z.w * yx.y ) ) * n );
					vert.z = ( ( ( x.w * zy.z ) + ( y.w * xz.z ) + ( z.w * yx.z ) ) * n );
				}
				else
				{
					return( false );
				}
				i ^= 4;
			}
			i ^= 2;
		}
		i ^= 1;
	}
	return( true );
}

void box_faces::Invert( void )
{
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		plane& face = faces[ i ];
		face.x = -face.x;
		face.y = -face.y;
		face.z = -face.z;
		face.w = -face.w;
	}
}

void box_faces::Normalize( void )
{
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		plane& face = faces[ i ];
		float n = ( 1.0f / ( sqrtf( ( face.x * face.x ) + ( face.y * face.y ) + ( face.z * face.z ) ) + consts::FLOAT_MIN_NORM ) );
		face.x *= n;
		face.y *= n;
		face.z *= n;
		face.w *= n;
	}
}

bool box_faces::Contains( const vec3& apoint ) const
{
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		if( ( ( apoint.x * faces[ i ].x ) + ( apoint.y * faces[ i ].y ) + ( apoint.z * faces[ i ].z ) + faces[ i ].w ) > 0 ) return( false ); 
	}
	return( true );
}

bool box_faces::Contains( const sphere& asphere ) const
{
	float r = -asphere.r;
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		if( ( ( asphere.x * faces[ i ].x ) + ( asphere.y * faces[ i ].y ) + ( asphere.z * faces[ i ].z ) + faces[ i ].w ) > r ) return( false ); 
	}
	return( true );
}

bool box_faces::Contains( const aabb& abox ) const
{
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		float x = ( ( faces[ i ].x < 0 ) ? abox.min.x : abox.max.x );
		float y = ( ( faces[ i ].y < 0 ) ? abox.min.y : abox.max.y );
		float z = ( ( faces[ i ].z < 0 ) ? abox.min.z : abox.max.z );
		if( ( ( x * faces[ i ].x ) + ( y * faces[ i ].y ) + ( z * faces[ i ].z ) + faces[ i ].w ) > 0 ) return( false ); 
	}
	return( true );
}

bool box_faces::QuickIntersects( const sphere& asphere ) const
{	///	can give false positives
	float r = asphere.r;
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		if( ( ( asphere.x * faces[ i ].x ) + ( asphere.y * faces[ i ].y ) + ( asphere.z * faces[ i ].z ) + faces[ i ].w ) > r ) return( false ); 
	}
	return( true );
}

bool box_faces::QuickIntersects( const aabb& abox ) const
{	///	can give false positives and in some cases where the aabb spans the frustum false negatives
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		float x = ( ( faces[ i ].x < 0 ) ? abox.max.x : abox.min.x );
		float y = ( ( faces[ i ].y < 0 ) ? abox.max.y : abox.min.y );
		float z = ( ( faces[ i ].z < 0 ) ? abox.max.z : abox.min.z );
		if( ( ( x * faces[ i ].x ) + ( y * faces[ i ].y ) + ( z * faces[ i ].z ) + faces[ i ].w ) > 0 ) return( false ); 
	}
	return( true );
}

bool box_faces::FrustumContains( const vec3& apoint ) const
{
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		if( ( ( apoint.x * faces[ i ].x ) + ( apoint.y * faces[ i ].y ) + ( apoint.z * faces[ i ].z ) + faces[ i ].w ) < 0 ) return( false ); 
	}
	return( true );
}

bool box_faces::FrustumContains( const sphere& asphere ) const
{
	float r = asphere.r;
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		if( ( ( asphere.x * faces[ i ].x ) + ( asphere.y * faces[ i ].y ) + ( asphere.z * faces[ i ].z ) + faces[ i ].w ) < r ) return( false ); 
	}
	return( true );
}

bool box_faces::FrustumContains( const aabb& abox ) const
{
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		float x = ( ( faces[ i ].x < 0 ) ? abox.max.x : abox.min.x );
		float y = ( ( faces[ i ].y < 0 ) ? abox.max.y : abox.min.y );
		float z = ( ( faces[ i ].z < 0 ) ? abox.max.z : abox.min.z );
		if( ( ( x * faces[ i ].x ) + ( y * faces[ i ].y ) + ( z * faces[ i ].z ) + faces[ i ].w ) < 0 ) return( false ); 
	}
	return( true );
}

bool box_faces::FrustumQuickIntersects( const sphere& asphere ) const
{	///	can give false positives
	float r = -asphere.r;
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		if( ( ( asphere.x * faces[ i ].x ) + ( asphere.y * faces[ i ].y ) + ( asphere.z * faces[ i ].z ) + faces[ i ].w ) < r ) return( false ); 
	}
	return( true );
}

bool box_faces::FrustumQuickIntersects( const aabb& abox ) const
{	///	can give false positives and in some cases where the aabb spans the frustum false negatives
	for( int i = 0; i < BOX_FACE_COUNT; ++i )
	{
		float x = ( ( faces[ i ].x < 0 ) ? abox.min.x : abox.max.x );
		float y = ( ( faces[ i ].y < 0 ) ? abox.min.y : abox.max.y );
		float z = ( ( faces[ i ].z < 0 ) ? abox.min.z : abox.max.z );
		if( ( ( x * faces[ i ].x ) + ( y * faces[ i ].y ) + ( z * faces[ i ].z ) + faces[ i ].w ) < 0 ) return( false ); 
	}
	return( true );
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
