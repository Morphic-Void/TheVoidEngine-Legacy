
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   camera.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Camera projection structure.
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

#include "camera.h"

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
////    camera
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool camera::IsNan( void ) const
{
	return( rotate.IsNan() || translate.IsNan() || project.IsNan() || unproject.IsNan() );
}

bool camera::IsReal( void ) const
{
	return( rotate.IsReal() && translate.IsReal() && project.IsReal() && unproject.IsReal() );
}

void camera::SetDefaults()
{
    rotate = consts::QUAT::IDENTITY;
    translate = consts::VEC3::ORIGIN;
    project = consts::MAT44::IDENTITY;
    unproject = consts::MAT44::IDENTITY;
}

void camera::Set( const joint& object, const clipper& clipping, const projector& projection )
{
    maths::vec3 origin;
    projection.GetOriginVector( origin );
    rotate.SetNormalized( object.q );
    rotate.Transform( origin, translate );
    translate.Add( object.v );
    translate.w = 1.0f;
    projection.GetOriginMatrix( project );
    clipping.Apply( project );
    unproject.SetInverse( project );
}

void camera::SetLeft( const joint& object, const clipper& clipping, const projector& projection )
{
    maths::vec3 origin;
    projection.GetOriginLeftVector( origin );
    rotate.SetNormalized( object.q );
    rotate.Transform( origin, translate );
    translate.Add( object.v );
    translate.w = 1.0f;
    projection.GetOriginLeftMatrix( project );
    clipping.Apply( project );
    unproject.SetInverse( project );
}

void camera::SetRight( const joint& object, const clipper& clipping, const projector& projection )
{
    maths::vec3 origin;
    projection.GetOriginRightVector( origin );
    rotate.SetNormalized( object.q );
    rotate.Transform( origin, translate );
    translate.Add( object.v );
    translate.w = 1.0f;
    projection.GetOriginRightMatrix( project );
    clipping.Apply( project );
    unproject.SetInverse( project );
}

void camera::Set( const viewer& setup )
{
    Set( setup.object, setup.clipping, setup.projection );
}

void camera::SetLeft( const viewer& setup )
{
    SetLeft( setup.object, setup.clipping, setup.projection );
}

void camera::SetRight( const viewer& setup )
{
    SetRight( setup.object, setup.clipping, setup.projection );
}

void camera::WorldToRelative( const vec3& world, vec3& relative )
{
    relative.SetSub( world, translate );
}

void camera::WorldToViewspace( const vec3& world, vec3& viewspace )
{
    vec3 relative;
    relative.SetSub( world, translate );
    rotate.InverseTransform( relative, viewspace );
}

void camera::WorldToClipspace( const vec3& world, vec4& clipspace )
{
    vec3 relative;
    relative.SetSub( world, translate );
    vec3 viewspace;
    rotate.InverseTransform( relative, viewspace );
    clipspace.SetMul( viewspace, project );
}

bool camera::WorldToProjected( const vec3& world, vec3& projected )
{
    vec3 relative;
    relative.SetSub( world, translate );
    vec3 viewspace;
    rotate.InverseTransform( relative, viewspace );
    return( projected.SetMul( viewspace, project ) );
}

void camera::RelativeToWorld( const vec3& relative, vec3& world )
{
    world.SetAdd( relative, translate );
}

void camera::RelativeToViewspace( const vec3& relative, vec3& viewspace )
{
    rotate.InverseTransform( relative, viewspace );
}

void camera::RelativeToClipspace( const vec3& relative, vec4& clipspace )
{
    vec3 viewspace;
    rotate.InverseTransform( relative, viewspace );
    clipspace.SetMul( viewspace, project );
}

bool camera::RelativeToProjected( const vec3& relative, vec3& projected )
{
    vec3 viewspace;
    rotate.InverseTransform( relative, viewspace );
    return( projected.SetMul( viewspace, project ) );
}

void camera::ViewspaceToWorld( const vec3& viewspace, vec3& world )
{
    vec3 relative;
    rotate.Transform( viewspace, relative );
    world.SetAdd( relative, translate );
}

void camera::ViewspaceToRelative( const vec3& viewspace, vec3& relative )
{
    rotate.Transform( viewspace, relative );
}

void camera::ViewspaceToClipspace( const vec3& viewspace, vec4& clipspace )
{
    clipspace.SetMul( viewspace, project );
}

bool camera::ViewspaceToProjected( const vec3& viewspace, vec3& projected )
{
    return( projected.SetMul( viewspace, project ) );
}

bool camera::ClipspaceToWorld( const vec4& clipspace, vec3& world )
{
    vec3 viewspace;
    if( viewspace.SetMul( clipspace, unproject ) )
    {
        vec3 relative;
        rotate.Transform( viewspace, relative );
        world.SetAdd( relative, translate );
        return( true );
    }
    return( false );
}

bool camera::ClipspaceToRelative( const vec4& clipspace, vec3& relative )
{
    vec3 viewspace;
    if( viewspace.SetMul( clipspace, unproject ) )
    {
        rotate.Transform( viewspace, relative );
        return( true );
    }
    return( false );
}

bool camera::ClipspaceToViewspace( const vec4& clipspace, vec3& viewspace )
{
    return( viewspace.SetMul( clipspace, unproject ) );
}

bool camera::ClipspaceToProjected( const vec4& clipspace, vec3& projected )
{
    return( projected.SetProjected( clipspace ) );
}

bool camera::ProjectedToWorld( const vec3& projected, vec3& world )
{
    vec3 viewspace;
    if( viewspace.SetMul( projected, unproject ) )
    {
        vec3 relative;
        rotate.Transform( viewspace, relative );
        world.SetAdd( relative, translate );
        return( true );
    }
    return( false );
}

bool camera::ProjectedToRelative( const vec3& projected, vec3& relative )
{
    vec3 viewspace;
    if( viewspace.SetMul( projected, unproject ) )
    {
        rotate.Transform( viewspace, relative );
        return( true );
    }
    return( false );
}

bool camera::ProjectedToViewspace( const vec3& projected, vec3& viewspace )
{
    return( viewspace.SetMul( projected, unproject ) );
}

void camera::ProjectedToClipspace( const vec3& projected, vec4& clipspace )
{
    clipspace.x = projected.x;
    clipspace.y = projected.y;
    clipspace.z = projected.z;
    clipspace.w = 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    stereo_camera
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool stereo_camera::IsNan( void ) const
{
	return( left.IsNan() || right.IsNan());
}

bool stereo_camera::IsReal( void ) const
{
	return( left.IsReal() && right.IsReal() );
}

void stereo_camera::SetDefaults()
{
    left.SetDefaults();
    right.SetDefaults();
}

void stereo_camera::Set( const joint& object, const clipper& clipping, const projector& projection )
{
    left.SetLeft( object, clipping, projection );
    right.SetRight( object, clipping, projection );
}

void stereo_camera::Set( const viewer& setup )
{
    left.SetLeft( setup );
    right.SetRight( setup );
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
