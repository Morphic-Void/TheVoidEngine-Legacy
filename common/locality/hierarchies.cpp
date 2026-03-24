
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   hierarchies.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game hierarchy classes and structures
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

#include "hierarchies.h"
#include "geometries.h"
#include "libs/system/debug/asserts.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin locality namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace locality
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    NodeLocality scene graph base class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NodeLocality::prepareNode()
{
    m_previous = m_location;
    NodeLocality* child = m_first_child;
    for( uint count = m_child_count; count; --count )
    {
        child->prepareNode();
        child = child->m_next_node;
    }
}

void NodeLocality::processNode()
{
    if( m_parent != NULL )
    {
        m_location.global.SetMul( m_location.local, m_parent->m_location.global );
    }
    else
    {
        m_location.global = m_location.local;
    }
    NodeLocality* child = m_first_child;
    for( uint count = m_child_count; count; --count )
    {
        child->processNode();
        child = child->m_next_node;
    }
}

void NodeLocality::reparentNode( NodeLocality* parent )
{
    if( m_parent != NULL )
    {
        if( m_parent->m_first_child == this )
        {
            m_parent->m_first_child = m_next_node;
        }
        --m_parent->m_child_count;
    }
    if( m_next_node ) m_next_node->m_prev_node = m_prev_node;
    if( m_prev_node ) m_prev_node->m_next_node = m_next_node;
    m_parent = parent;
    m_next_node = m_prev_node = NULL;
    if( m_parent != NULL )
    {
        m_next_node = m_parent->m_first_child;
        if( m_next_node ) m_next_node->m_prev_node = this;
        m_parent->m_first_child = this;
        ++m_parent->m_child_count;
    }
}

void NodeLocality::zeroNode()
{
    m_previous.local = m_previous.global = m_location.local = m_location.global = maths::consts::JOINT::IDENTITY;
    m_parent = m_prev_node = m_next_node = m_first_child = NULL;
    m_child_count = 0;
    m_node_type = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ItemLocality item scene graph node class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ItemLocality::setRoom( RoomLocality* room, const bool reparent )
{
    if( m_room != NULL )
    {
        if( m_room->m_first_item == this )
        {
            m_room->m_first_item = m_next_by_room;
        }
        --m_room->m_item_count;
    }
    if( m_next_by_room ) m_next_by_room->m_prev_by_room = m_prev_by_room;
    if( m_prev_by_room ) m_prev_by_room->m_next_by_room = m_next_by_room;
    m_room = room;
    m_next_by_room = m_prev_by_room = NULL;
    if( m_room != NULL )
    {
        m_next_by_room = m_room->m_first_item;
        if( m_next_by_room ) m_next_by_room->m_prev_by_room = this;
        m_room->m_first_item = this;
        ++m_room->m_item_count;
    }
    if( reparent )
    {
        reparentNode( room );
    }
}

void ItemLocality::setDoor( DoorLocality* door, const bool face )
{
    if( m_door != NULL )
    {
        if( m_door_face )
        {
            if( m_door->m_face_first_item == this )
            {
                m_door->m_face_first_item = m_next_by_door;
            }
            --m_door->m_face_item_count;
        }
        else
        {
            if( m_door->m_back_first_item == this )
            {
                m_door->m_back_first_item = m_next_by_door;
            }
            --m_door->m_back_item_count;
        }
    }
    if( m_next_by_door ) m_next_by_door->m_prev_by_door = m_prev_by_door;
    if( m_prev_by_door ) m_prev_by_door->m_next_by_door = m_next_by_door;
    m_next_by_door = m_prev_by_door = NULL;
    m_door = door;
    m_door_face = face;
    if( m_door != NULL )
    {
        if( m_door_face )
        {
            m_next_by_door = m_door->m_face_first_item;
            if( m_next_by_door ) m_next_by_door->m_prev_by_door = this;
            m_door->m_face_first_item = this;
            ++m_door->m_face_item_count;
        }
        else
        {
            m_next_by_door = m_door->m_back_first_item;
            if( m_next_by_door ) m_next_by_door->m_prev_by_door = this;
            m_door->m_back_first_item = this;
            ++m_door->m_back_item_count;
        }
    }
}

void ItemLocality::zeroItem()
{
    m_geometry = NULL;
    m_room = NULL;
    m_door = NULL;
    m_prev_by_room = m_next_by_room = NULL;
    m_prev_by_door = m_next_by_door = NULL;
    m_door_face = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DoorLocality door scene graph node class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DoorLocality::setFrameAuto()
{
    if( m_face_target_door != NULL )
    {
        if( m_back_target_door != NULL )
        {
            m_frame_type = ( m_embedded ? EDoorChamferBoth : EDoorDiamondBoth );
        }
        else
        {
            m_frame_type = ( m_embedded ? EDoorChamferFace : EDoorDiamondFace );
        }
    }
    else
    {
        if( m_back_target_door != NULL )
        {
            m_frame_type = ( m_embedded ? EDoorChamferBack : EDoorDiamondBack );
        }
        else
        {
            m_frame_type = EDoorUnframed;
        }
    }
}

void DoorLocality::setRoom( RoomLocality* room, const bool reparent )
{
    if( m_room != NULL )
    {
        if( m_room->m_first_door == this )
        {
            m_room->m_first_door = m_next_by_room;
        }
        --m_room->m_door_count;
    }
    if( m_next_by_room ) m_next_by_room->m_prev_by_room = m_prev_by_room;
    if( m_prev_by_room ) m_prev_by_room->m_next_by_room = m_next_by_room;
    m_room = room;
    m_next_by_room = m_prev_by_room = NULL;
    if( m_room != NULL )
    {
        m_next_by_room = m_room->m_first_door;
        if( m_next_by_room ) m_next_by_room->m_prev_by_room = this;
        m_room->m_first_door = this;
        ++m_room->m_door_count;
    }
    if( reparent )
    {
        reparentNode( room );
    }
}

void DoorLocality::zeroDoor()
{
    m_geometry = NULL;
    m_room = NULL;
    m_prev_by_room = m_next_by_room = NULL;
    m_face_target_door = m_back_target_door = NULL;
    m_face_first_item = m_back_first_item = NULL;
    m_face_item_count = m_back_item_count = 0;
    m_frame_type = EDoorUnframed;
    m_blocking = false;
    m_embedded = false;
}

bool DoorLocality::isFastPass( const bool face ) const
{
    if( face )
    {
        return( ( m_face_item_count == 0 ) && ( ( m_face_target_door == NULL ) || ( m_face_target_door->m_back_item_count == 0 ) ) );
    }
    else
    {
        return( ( m_back_item_count == 0 ) && ( ( m_back_target_door == NULL ) || ( m_back_target_door->m_face_item_count == 0 ) ) );
    }
}

float DoorLocality::getRelativeZ( const maths::vec3& point ) const
{
    const maths::quat& q = m_location.global.q;
    const maths::vec3& v = m_location.global.v;
	float dz = ( ( ( point.y - v.y ) * ( ( q.y * q.z ) - ( q.w * q.x ) ) ) + ( ( point.x - v.x ) * ( ( q.z * q.x ) + ( q.w * q.y ) ) ) );
    dz += dz;
    dz += ( ( point.z - v.z ) * ( ( q.w * q.w ) - ( q.x * q.x ) - ( q.y * q.y ) + ( q.z * q.z ) ) );
    return( dz );
}

void DoorLocality::getFacePlane( maths::plane& aplane ) const
{
    const maths::quat& q = m_location.global.q;
    aplane.x = ( ( ( q.x * q.z ) + ( q.y * q.w ) ) * -2.0f );
    aplane.y = ( ( ( q.y * q.z ) - ( q.x * q.w ) ) * -2.0f );
	aplane.z = ( ( q.x * q.x ) + ( q.y * q.y ) - ( q.z * q.z ) - ( q.w * q.w ) );
    aplane.t_vec3().Normalize();
    aplane.w = ( 0.0f - aplane.t_vec3().Dot( m_location.global.v ) );
}

void DoorLocality::getBackPlane( maths::plane& aplane ) const
{
    const maths::quat& q = m_location.global.q;
    aplane.x = ( ( ( q.x * q.z ) + ( q.y * q.w ) ) * 2.0f );
    aplane.y = ( ( ( q.y * q.z ) - ( q.x * q.w ) ) * 2.0f );
	aplane.z = ( ( q.w * q.w ) + ( q.z * q.z ) - ( q.y * q.y ) - ( q.x * q.x ) );
    aplane.t_vec3().Normalize();
    aplane.w = ( 0.0f - aplane.t_vec3().Dot( m_location.global.v ) );
}

void DoorLocality::getDoorRemap( const DoorLocality& target, maths::joint& remap ) const
{
    remapDoor( getObject(), target.getObject(), remap );
}

void DoorLocality::getDoorRemap( const DoorLocality* const target, maths::joint& remap ) const
{
    if( target == NULL )
    {
        remap = maths::consts::JOINT::IDENTITY;
    }
    else
    {
        remapDoor( getObject(), target->getObject(), remap );
    }
}

//  create the transform required to move from the portal space
//  described by src to the portal space described by dst.
//
//      to create the remap:
//          remap.q = mul( invert( src.q ), dst.q )
//          remap.v = dst.v - mul( src.v, remap.q )
//
//          this is equivalent to invert( src ) * dst but takes fewer operations
//
void DoorLocality::remapDoor( const maths::joint& src, const maths::joint& dst, maths::joint& remap )
{
    maths::joint j;
    float s = ( 1.0f / ( ( src.q.x * src.q.x ) + ( src.q.y * src.q.y ) + ( src.q.z * src.q.z ) + ( src.q.w * src.q.w ) ) );
    j.q.x = ( ( ( src.q.w * dst.q.x ) - ( src.q.z * dst.q.y ) + ( src.q.y * dst.q.z ) - ( src.q.x * dst.q.w ) ) * s );
    j.q.y = ( ( ( src.q.w * dst.q.y ) + ( src.q.z * dst.q.x ) - ( src.q.y * dst.q.w ) - ( src.q.x * dst.q.z ) ) * s );
    j.q.z = ( ( ( src.q.w * dst.q.z ) - ( src.q.z * dst.q.w ) - ( src.q.y * dst.q.x ) + ( src.q.x * dst.q.y ) ) * s );
    j.q.w = ( ( ( src.q.w * dst.q.w ) + ( src.q.z * dst.q.z ) + ( src.q.y * dst.q.y ) + ( src.q.x * dst.q.x ) ) * s );
    j.v.SetMul( src.v, j.q );
    j.v.x = ( dst.v.x - j.v.x );
    j.v.y = ( dst.v.y - j.v.y );
    j.v.z = ( dst.v.z - j.v.z );
    remap.Set( j );
}

bool DoorLocality::excludeSphere( const maths::sphere& asphere, maths::vec3& reposition ) const
{
    if( m_geometry != NULL )
    {
        maths::sphere collider_asphere;
        maths::vec3 collider_reposition;
        collider_asphere.r = ( asphere.r / m_location.global.q.GetScaling() );
        m_location.global.InverseTransform( asphere.t_vec3(), collider_asphere.t_vec3() );
        if( m_geometry->getCollider().excludeSphere( getState(), collider_asphere, collider_reposition ) )
        {
            m_location.global.Transform( collider_reposition, reposition );
            return( true );
        }
    }
    return( false );
}

bool DoorLocality::collideSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const
{
    if( m_geometry != NULL )
    {
        maths::sphere collider_asphere;
        maths::vec3 collider_direction;
        maths::vec3 collider_normal;
        collider_asphere.r = ( asphere.r / m_location.global.q.GetScaling() );
        m_location.global.InverseTransform( asphere.t_vec3(), collider_asphere.t_vec3() );
        m_location.global.q.InverseTransform( direction, collider_direction );
        if( m_geometry->getCollider().collideSphere( getState(), collider_asphere, collider_direction, min_t, max_t, t, collider_normal ) )
        {
            m_location.global.q.UnscaledTransform( collider_normal, normal );
            return( true );
        }
    }
    return( false );
}

bool DoorLocality::interceptSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, bool& face ) const
{
    if( m_geometry != NULL )
    {
        maths::sphere collider_asphere;
        maths::vec3 collider_direction;
        collider_asphere.r = ( asphere.r / m_location.global.q.GetScaling() );
        m_location.global.InverseTransform( asphere.t_vec3(), collider_asphere.t_vec3() );
        m_location.global.q.InverseTransform( direction, collider_direction );
        return( m_geometry->getCollider().interceptSphere( getState(), collider_asphere, collider_direction, min_t, max_t, t, face ) );
    }
    return( false );
}

bool DoorLocality::includingSphere( const maths::sphere& asphere, bool& face ) const
{
    if( m_geometry != NULL )
    {
        maths::sphere collider_asphere;
        collider_asphere.r = ( asphere.r / m_location.global.q.GetScaling() );
        m_location.global.InverseTransform( asphere.t_vec3(), collider_asphere.t_vec3() );
        return( m_geometry->getCollider().includingSphere( getState(), collider_asphere, face ) );
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    RoomLocality room scene graph node class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RoomLocality::zeroRoom()
{
    m_geometry = NULL;
    m_first_item = NULL;
    m_first_door = NULL;
    m_item_count = 0;
    m_door_count = 0;
    m_reject_bias = ( maths::consts::FLOAT_EPSILON * 8.0f );  //( 1.0f / 1024.0f );
}

bool RoomLocality::containsPoint( const maths::vec3& point ) const
{
    if( m_geometry != NULL )
    {
        maths::vec3 collider_point;
        m_location.global.InverseTransform( point, collider_point );
        return( m_geometry->getCollider().containsPoint( collider_point ) );
    }
    return( false );
}

bool RoomLocality::excludeSphere( const maths::sphere& asphere, maths::vec3& reposition ) const
{
    if( m_geometry != NULL )
    {
        maths::sphere collider_asphere;
        collider_asphere.r = ( asphere.r / m_location.global.q.GetScaling() );
        m_location.global.InverseTransform( asphere.t_vec3(), collider_asphere.t_vec3() );
        if( m_geometry->getCollider().excludeSphere( collider_asphere, collider_asphere.t_vec3() ) )
        {
            m_location.global.Transform( collider_asphere.t_vec3(), reposition );
            return( true );
        }
    }
    return( false );
}

bool RoomLocality::excludeSphere( const maths::sphere& asphere, const maths::plane& reject, maths::vec3& reposition ) const
{
    if( m_geometry != NULL )
    {
        maths::sphere collider_asphere;
        maths::plane collider_reject;
        collider_asphere.r = ( asphere.r / m_location.global.q.GetScaling() );
        m_location.global.InverseTransform( asphere.t_vec3(), collider_asphere.t_vec3() );
        collider_reject.w = ( ( m_location.global.v.Dot( reject ) / reject.t_vec3().Length() ) - m_reject_bias );
        m_location.global.q.InverseTransform( reject.t_vec3(), collider_reject.t_vec3() );
        collider_reject.t_vec3().Normalize();
        if( m_geometry->getCollider().excludeSphere( collider_asphere, collider_asphere.t_vec3(), collider_reject ) )
        {
            m_location.global.Transform( collider_asphere.t_vec3(), reposition );
            return( true );
        }
    }
    return( false );
}

bool RoomLocality::collideSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const
{
    if( m_geometry != NULL )
    {
        maths::sphere collider_asphere;
        maths::vec3 collider_direction;
        maths::vec3 collider_normal;
        collider_asphere.r = ( asphere.r / m_location.global.q.GetScaling() );
        m_location.global.InverseTransform( asphere.t_vec3(), collider_asphere.t_vec3() );
        m_location.global.q.InverseTransform( direction, collider_direction );
        if( m_geometry->getCollider().collideSphere( collider_asphere, collider_direction, min_t, max_t, t, collider_normal ) )
        {
            m_location.global.q.UnscaledTransform( collider_normal, normal );
            return( true );
        }
    }
    return( false );
}

bool RoomLocality::sphereExclude( const maths::sphere& asphere, maths::vec3& reposition ) const
{
    if( m_geometry != NULL )
    {
        bool excluded = false;
        maths::sphere collision_asphere;
        collision_asphere = asphere;
        {   //  check if the sphere is touching a door
            const DoorLocality* door = m_first_door;
            for( uint count = m_door_count; count; --count )
            {
                bool face;
                if( door->includingSphere( collision_asphere, face ) )
                {
                    maths::plane collision_reject;
                    door->getDoorPlane( collision_reject, face );
                    if( excludeSphere( collision_asphere, collision_reject, collision_asphere.t_vec3() ) )
                    {
                        excluded = true;
                    }
                    if( door->excludeSphere( collision_asphere, collision_asphere.t_vec3() ) )
                    {
                        excluded = true;
                    }
                    if( excluded ) reposition = collision_asphere.t_vec3();
                    return( excluded );
                }
                door = door->getNextByRoom();
            }
        }
        if( excludeSphere( collision_asphere, collision_asphere.t_vec3() ) )
        {
            excluded = true;
        }
        {   //  check the sphere against the doors
            const DoorLocality* door = m_first_door;
            for( uint count = m_door_count; count; --count )
            {
                if( door->excludeSphere( collision_asphere, collision_asphere.t_vec3() ) )
                {
                    excluded = true;
                }
                door = door->getNextByRoom();
            }
        }
        if( excluded ) reposition = collision_asphere.t_vec3();
        return( excluded );
    }
    return( false );
}

bool RoomLocality::sphereCollide( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const
{
    if( m_geometry != NULL )
    {
        maths::sphere collider_asphere;
        maths::vec3 collider_direction;
        maths::vec3 collider_normal;
        collider_asphere.r = ( asphere.r / m_location.global.q.GetScaling() );
        m_location.global.InverseTransform( asphere.t_vec3(), collider_asphere.t_vec3() );
        m_location.global.q.InverseTransform( direction, collider_direction );
        if( m_geometry->getCollider().collideSphere( collider_asphere, collider_direction, min_t, max_t, t, collider_normal ) )
        {
            m_location.global.q.UnscaledTransform( collider_normal, normal );
            return( true );
        }
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ViewLocality view scene graph node class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ViewLocality::zeroView()
{
    m_room = NULL;
    m_prev_view = m_next_view = NULL;
    m_targetRTV.release();
    m_targetDSV.release();
    m_projector.SetDefaults();
    m_clipper.SetDefaults();
    m_viewport.min.x = m_viewport.min.y = m_viewport.min.z = 0.0f;
    m_viewport.max.x = m_viewport.max.y = m_viewport.max.z = 0.0f;
    m_scissoring.left = m_scissoring.top = m_scissoring.right = m_scissoring.bottom = 0;
    m_dimensions.width = m_dimensions.height = 0;
    m_clipPlanes = maths::consts::BOX_FACES::DEFAULT;
    m_projection = maths::consts::MAT44::IDENTITY;
    m_viewToWorld = maths::consts::JOINT::IDENTITY;
    m_stencilRef = 0;
    m_freeCam = false;
    m_radius = ( 32767.0f / 65536.0f );
}

void ViewLocality::setWindow( const SRECT& window )
{
    m_scissoring = window;
    m_dimensions.width = static_cast< uint >( m_scissoring.right - m_scissoring.left );
    m_dimensions.height = static_cast< uint >( m_scissoring.bottom - m_scissoring.top );
    m_viewport.min.x = static_cast< float >( m_scissoring.left );
    m_viewport.min.y = static_cast< float >( m_scissoring.top );
    m_viewport.min.z = 0.0f;
    m_viewport.max.x = static_cast< float >( m_scissoring.right );
    m_viewport.max.y = static_cast< float >( m_scissoring.bottom );
    m_viewport.max.z = 1.0f;
}

void ViewLocality::setWindow( const DIMS2D& dimensions )
{
    m_scissoring.left = m_scissoring.top = 0;
    m_scissoring.right = static_cast< int >( dimensions.width );
    m_scissoring.bottom = static_cast< int >( dimensions.height );
    m_dimensions = dimensions;
    m_viewport.min.x = 0.0f;
    m_viewport.min.y = 0.0f;
    m_viewport.min.z = 0.0f;
    m_viewport.max.x = static_cast< float >( dimensions.width );
    m_viewport.max.y = static_cast< float >( dimensions.height );
    m_viewport.max.z = 1.0f;
}

void ViewLocality::setWindow( const uint width, const uint height )
{
    m_scissoring.left = m_scissoring.top = 0;
    m_scissoring.right = static_cast< int >( width );
    m_scissoring.bottom = static_cast< int >( height );
    m_dimensions.width = width;
    m_dimensions.height = height;
    m_viewport.min.x = 0.0f;
    m_viewport.min.y = 0.0f;
    m_viewport.min.z = 0.0f;
    m_viewport.max.x = static_cast< float >( width );
    m_viewport.max.y = static_cast< float >( height );
    m_viewport.max.z = 1.0f;
}

bool ViewLocality::adjustWindow( const maths::frect& extents, SRECT& window ) const
{
    SRECT rect;
    rect.left =   ( ( extents.l > -1.0f ) ? ( ( static_cast< int >( floorf( m_dimensions.width  + ( extents.l * m_dimensions.width  ) ) ) >> 1 ) - 1 ) : 0 );
    rect.right =  ( ( extents.r <  1.0f ) ? ( ( static_cast< int >(  ceilf( m_dimensions.width  + ( extents.r * m_dimensions.width  ) ) ) >> 1 ) + 1 ) : m_dimensions.width  );
    rect.top =    ( ( extents.t <  1.0f ) ? ( ( static_cast< int >( floorf( m_dimensions.height - ( extents.t * m_dimensions.height ) ) ) >> 1 ) - 1 ) : 0 );
    rect.bottom = ( ( extents.b > -1.0f ) ? ( ( static_cast< int >(  ceilf( m_dimensions.height - ( extents.b * m_dimensions.height ) ) ) >> 1 ) + 1 ) : m_dimensions.height );
    if( window.left < rect.left ) window.left = rect.left;
    if( window.right > rect.right ) window.right = rect.right;
    if( window.top < rect.top ) window.top = rect.top;
    if( window.bottom > rect.bottom ) window.bottom = rect.bottom;
    return( ( window.left < window.right ) && ( window.top < window.bottom ) );
}

void ViewLocality::getWindowed( const SRECT& window, maths::mat44& projection ) const
{
    const float rcp_width = ( 1.0f / m_dimensions.width );
    const float rcp_height = ( 1.0f / m_dimensions.height );
    maths::frect zoom;
    zoom.l = ( window.left * rcp_width );
    zoom.r = ( window.right * rcp_width );
    zoom.t = ( 1.0f - ( window.top * rcp_height ) );
    zoom.b = ( 1.0f - ( window.bottom * rcp_height ) );
    maths::clipper clip;
    clip.SetZoom( zoom );
    clip.Apply( m_projection, projection );
}

void ViewLocality::getOrigin( maths::vec3& origin )
{
    maths::vec3 offset;
    m_projector.GetOriginVector( offset );
    m_location.global.Transform( offset, origin );
}

void ViewLocality::evaluate()
{
    maths::vec3 offset;
    m_projector.GetOriginVector( offset );
    m_viewToWorld.q.Set( m_location.global.q );
    m_location.global.Transform( offset, m_viewToWorld.v );
    m_projector.GetOriginMatrix( m_projection );
    maths::clipper clipper;
    clipper.SetDX( m_clipper );
    clipper.DepthFlip();
    clipper.Apply( m_projection );
    m_projection.GetPositiveFrustumPlaneX( -1.0f, m_clipPlanes.faces[ maths::BOX_FACE_L ] );
    m_projection.GetNegativeFrustumPlaneX(  1.0f, m_clipPlanes.faces[ maths::BOX_FACE_R ] );
    m_projection.GetPositiveFrustumPlaneY( -1.0f, m_clipPlanes.faces[ maths::BOX_FACE_B ] );
    m_projection.GetNegativeFrustumPlaneY(  1.0f, m_clipPlanes.faces[ maths::BOX_FACE_T ] );
    m_projection.GetPositiveFrustumPlaneZ(  0.0f, m_clipPlanes.faces[ maths::BOX_FACE_F ] );
    m_projection.GetNegativeFrustumPlaneZ(  1.0f, m_clipPlanes.faces[ maths::BOX_FACE_N ] );
    m_clipPlanes.Normalize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end locality namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace locality

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

