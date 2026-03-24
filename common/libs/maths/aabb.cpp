
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   aabb.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Axis aligned bounding box.
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
#include "frect.h"
#include "sphere.h"
#include "box_verts.h"
#include "box_faces.h"

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
	namespace AABB
	{
		const aabb		UNIT = { { -1, -1, -1,  0 }, {  1,  1,  1,  0 } };
		const aabb		PORT = { {  0,  0,  0,  0 }, {  1,  1,  1,  0 } };
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    aabb
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool aabb::IsNan( void ) const
{
	return( min.IsNan() || max.IsNan() );
}

bool aabb::IsReal( void ) const
{
	return( min.IsReal() && max.IsReal() );
}

void aabb::Recenter( void )
{
    float x = ( ( max.x - min.x ) * 0.5f );
    float y = ( ( max.y - min.y ) * 0.5f );
    float z = ( ( max.z - min.z ) * 0.5f );
    max.x = x;
    max.y = y;
    max.z = z;
    min.x = ( 0.0f - x );
    min.y = ( 0.0f - y );
    min.z = ( 0.0f - z );
}

void aabb::Set( const vec3& apoint )
{
	min.x = max.x = apoint.x;
	min.y = max.y = apoint.y;
	min.z = max.z = apoint.z;
}

void aabb::Set( const sphere& asphere )
{
	min.x = ( asphere.x - asphere.r );
	min.y = ( asphere.y - asphere.r );
	min.z = ( asphere.z - asphere.r );
	max.x = ( asphere.x + asphere.r );
	max.y = ( asphere.y + asphere.r );
	max.z = ( asphere.z + asphere.r );
}

void aabb::Merge( const vec3& apoint )
{
    if( min.x > apoint.x ) min.x = apoint.x;
    if( min.y > apoint.y ) min.y = apoint.y;
    if( min.z > apoint.z ) min.z = apoint.z;
    if( max.x < apoint.x ) max.x = apoint.x;
    if( max.y < apoint.y ) max.y = apoint.y;
    if( max.z < apoint.z ) max.z = apoint.z;
}

void aabb::Merge( const sphere& asphere )
{
    aabb abox;
    abox.Set( asphere );
    Merge( abox );
}

void aabb::Merge( const aabb& abox )
{
	if( min.x > abox.min.x ) min.x = abox.min.x;
	if( min.y > abox.min.y ) min.y = abox.min.y;
	if( min.z > abox.min.z ) min.z = abox.min.z;
	if( max.x < abox.max.x ) max.x = abox.max.x;
	if( max.y < abox.max.y ) max.y = abox.max.y;
	if( max.z < abox.max.z ) max.z = abox.max.z;
}

bool aabb::Union( const aabb& abox )
{
	if( min.x < abox.min.x ) min.x = abox.min.x;
	if( min.y < abox.min.y ) min.y = abox.min.y;
	if( min.z < abox.min.z ) min.z = abox.min.z;
	if( max.x > abox.max.x ) max.x = abox.max.x;
	if( max.y > abox.max.y ) max.y = abox.max.y;
	if( max.z > abox.max.z ) max.z = abox.max.z;
	if( ( min.x > max.x ) || ( min.y > max.y ) || ( min.z > max.z ) )
	{
		min.x = min.y = min.z = max.x = max.y = max.z = 0.0f;
		return( false );
	}
	return( true );
}

void aabb::SetMerge( const vec3& point1, const vec3& point2 )
{
	min.x = ( ( point1.x < point2.x ) ? point1.x : point2.x );
	min.y = ( ( point1.y < point2.y ) ? point1.y : point2.y );
	min.z = ( ( point1.z < point2.z ) ? point1.z : point2.z );
	max.x = ( ( point1.x > point2.x ) ? point1.x : point2.x );
	max.y = ( ( point1.y > point2.y ) ? point1.y : point2.y );
	max.z = ( ( point1.z > point2.z ) ? point1.z : point2.z );
}

void aabb::SetMerge( const sphere& sphere1, const sphere& sphere2 )
{
    aabb box1, box2;
    box1.Set( sphere1 );
    box2.Set( sphere2 );
    SetMerge( box1, box2 );
}

void aabb::SetMerge( const sphere& asphere, const vec3& apoint )
{
    aabb abox;
    abox.Set( asphere );
    SetMerge( abox, apoint );
}

void aabb::SetMerge( const aabb& abox, const vec3& apoint )
{
	min.x = ( ( abox.min.x < apoint.x ) ? abox.min.x : apoint.x );
	min.y = ( ( abox.min.y < apoint.y ) ? abox.min.y : apoint.y );
	min.z = ( ( abox.min.z < apoint.z ) ? abox.min.z : apoint.z );
	max.x = ( ( abox.max.x > apoint.x ) ? abox.max.x : apoint.x );
	max.y = ( ( abox.max.y > apoint.y ) ? abox.max.y : apoint.y );
	max.z = ( ( abox.max.z > apoint.z ) ? abox.max.z : apoint.z );
}

void aabb::SetMerge( const aabb& abox, const sphere& asphere )
{
    aabb box2;
    box2.Set( asphere );
    SetMerge( abox, box2 );
}

void aabb::SetMerge( const aabb& box1, const aabb& box2 )
{
	min.x = ( ( box1.min.x < box2.min.x ) ? box1.min.x : box2.min.x );
	min.y = ( ( box1.min.y < box2.min.y ) ? box1.min.y : box2.min.y );
	min.z = ( ( box1.min.z < box2.min.z ) ? box1.min.z : box2.min.z );
	max.x = ( ( box1.max.x > box2.max.x ) ? box1.max.x : box2.max.x );
	max.y = ( ( box1.max.y > box2.max.y ) ? box1.max.y : box2.max.y );
	max.z = ( ( box1.max.z > box2.max.z ) ? box1.max.z : box2.max.z );
}

bool aabb::SetUnion( const aabb& box1, const aabb& box2 )
{
	min.x = ( ( box1.min.x > box2.min.x ) ? box1.min.x : box2.min.x );
	min.y = ( ( box1.min.y > box2.min.y ) ? box1.min.y : box2.min.y );
	min.z = ( ( box1.min.z > box2.min.z ) ? box1.min.z : box2.min.z );
	max.x = ( ( box1.max.x < box2.max.x ) ? box1.max.x : box2.max.x );
	max.y = ( ( box1.max.y < box2.max.y ) ? box1.max.y : box2.max.y );
	max.z = ( ( box1.max.z < box2.max.z ) ? box1.max.z : box2.max.z );
	if( ( min.x > max.x ) || ( min.y > max.y ) || ( min.z > max.z ) )
	{
		min.x = min.y = min.z = max.x = max.y = max.z = 0.0f;
		return( false );
	}
	return( true );
}

bool aabb::Contains( const vec3& apoint ) const
{
	return( (
		( min.x <= apoint.x ) && ( max.x >= apoint.x ) &&
		( min.y <= apoint.y ) && ( max.y >= apoint.y ) &&
		( min.z <= apoint.z ) && ( max.z >= apoint.z ) ) ? true : false );
}

bool aabb::Contains( const sphere& asphere ) const
{
	return( (
		( min.x <= ( asphere.x - asphere.r ) ) && ( max.x >= ( asphere.x + asphere.r ) ) &&
		( min.y <= ( asphere.y - asphere.r ) ) && ( max.y >= ( asphere.y + asphere.r ) ) &&
		( min.z <= ( asphere.z - asphere.r ) ) && ( max.z >= ( asphere.z + asphere.r ) ) ) ? true : false );
}

bool aabb::Contains( const aabb& abox ) const
{
	return( (
		( min.x <= abox.min.x ) && ( max.x >= abox.max.x ) &&
		( min.y <= abox.min.y ) && ( max.y >= abox.max.y ) &&
		( min.z <= abox.min.z ) && ( max.z >= abox.max.z ) ) ? true : false );
}

bool aabb::Intersects( const sphere& asphere ) const
{
	float d = 0;
	if( min.x > asphere.x )
	{
		const float c = ( min.x - asphere.x );
		d += ( c * c );
	}
	else if( asphere.x > max.x )
	{
		const float c = ( asphere.x - max.x );
		d += ( c * c );
	}
	if( min.y > asphere.y )
	{
		const float c = ( min.y - asphere.y );
		d += ( c * c );
	}
	else if( asphere.y > max.y )
	{
		const float c = ( asphere.y - max.y );
		d += ( c * c );
	}
	if( min.z > asphere.z )
	{
		const float c = ( min.z - asphere.z );
		d += ( c * c );
	}
	else if( asphere.z > max.z )
	{
		const float c = ( asphere.z - max.z );
		d += ( c * c );
	}
	return( ( d <= ( asphere.r * asphere.r ) ) ? true : false );
}

bool aabb::Intersects( const aabb& abox ) const
{
	return( (
		( min.x <= abox.max.x ) && ( max.x >= abox.min.x ) &&
		( min.y <= abox.max.y ) && ( max.y >= abox.min.y ) &&
		( min.z <= abox.max.z ) && ( max.z >= abox.min.z ) ) ? true : false );
}

void aabb::GetVerts( box_verts& verts ) const
{
	verts.verts[ BOX_VERT_NBL ].x = verts.verts[ BOX_VERT_NTL ].x = verts.verts[ BOX_VERT_FBL ].x = verts.verts[ BOX_VERT_FTL ].x = min.x;
	verts.verts[ BOX_VERT_NBR ].x = verts.verts[ BOX_VERT_NTR ].x = verts.verts[ BOX_VERT_FBR ].x = verts.verts[ BOX_VERT_FTR ].x = max.x;
	verts.verts[ BOX_VERT_NBL ].y = verts.verts[ BOX_VERT_NBR ].y = verts.verts[ BOX_VERT_FBL ].y = verts.verts[ BOX_VERT_FBR ].y = min.y;
	verts.verts[ BOX_VERT_NTL ].y = verts.verts[ BOX_VERT_NTR ].y = verts.verts[ BOX_VERT_FTL ].y = verts.verts[ BOX_VERT_FTR ].y = max.y;
	verts.verts[ BOX_VERT_NBL ].z = verts.verts[ BOX_VERT_NBR ].z = verts.verts[ BOX_VERT_NTL ].z = verts.verts[ BOX_VERT_NTR ].z = min.z;
	verts.verts[ BOX_VERT_FBL ].z = verts.verts[ BOX_VERT_FBR ].z = verts.verts[ BOX_VERT_FTL ].z = verts.verts[ BOX_VERT_FTR ].z = max.z;
}

void aabb::GetFaces( box_faces& faces ) const
{
	faces.faces[ BOX_FACE_L ].x = faces.faces[ BOX_FACE_B ].y = faces.faces[ BOX_FACE_N ].z = -1.0f;
	faces.faces[ BOX_FACE_R ].x = faces.faces[ BOX_FACE_T ].y = faces.faces[ BOX_FACE_F ].z = 1.0f;
	faces.faces[ BOX_FACE_L ].y = faces.faces[ BOX_FACE_B ].z = faces.faces[ BOX_FACE_N ].x = 0.0f;
	faces.faces[ BOX_FACE_R ].y = faces.faces[ BOX_FACE_T ].z = faces.faces[ BOX_FACE_F ].x = 0.0f;
	faces.faces[ BOX_FACE_L ].z = faces.faces[ BOX_FACE_B ].x = faces.faces[ BOX_FACE_N ].y = 0.0f;
	faces.faces[ BOX_FACE_R ].z = faces.faces[ BOX_FACE_T ].x = faces.faces[ BOX_FACE_F ].y = 0.0f;
	faces.faces[ BOX_FACE_L ].w = min.x;
	faces.faces[ BOX_FACE_B ].w = min.y;
	faces.faces[ BOX_FACE_N ].w = min.z;
	faces.faces[ BOX_FACE_R ].w = -max.x;
	faces.faces[ BOX_FACE_T ].w = -max.y;
	faces.faces[ BOX_FACE_F ].w = -max.z;
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
