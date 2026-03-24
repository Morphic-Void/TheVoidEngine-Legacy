
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   collidables.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game collidable classes and structures
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

#include "collidables.h"
#include "generators.h"
#include "libs/system/debug/asserts.h"
#include "libs/maths/consts.h"
#include <math.h>
#include <memory.h>

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
////    BaseCollider base collisions class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BaseCollider::edgeCollide( const maths::sphere& asphere, const maths::vec3& direction, const Edge& edge, float& t, maths::vec3& normal ) const
{
    maths::vec3 uaxis;
    uaxis.SetCross( edge.tplane.t_vec3(), direction );  //  closest points axis, magnitude is length of direction * sine of angle
    float uu = uaxis.SqrLen();
    float dd = direction.SqrLen();
    float ud = ( uu * ( dd - uu ) );
    if( ud >= maths::consts::FLOAT_MIN_NORM )
    {   //  the edge is not aligned with the direction and the direction has sufficient magnitude
        maths::vec3 separation;
        separation.SetSub( edge.cpoint.t_vec3(), asphere.t_vec3() );
        float rr = ( asphere.r * asphere.r );
        float us = uaxis.Dot( separation );
        float ru = ( ( rr * uu ) - ( us * us ) );
        if( ru > 0.0f )
        {   //  there is a collision with the infinitely extended edge
            maths::vec3 vaxis;
            vaxis.SetCross( edge.tplane.t_vec3(), uaxis );  //  approach axis, magnitude is length of direction * sine of angle
            float vd = vaxis.Dot( direction );              //  length of direction * sine of angle * length of direction * cosine of angle
            if( fabsf( vd ) >= maths::consts::FLOAT_MIN_NORM )
            {   //  the edge is not aligned with the direction (second check to be certain that there will not be a division by zero)
                t = ( ( vaxis.Dot( separation ) / vd ) - sqrtf( ru / ud ) );
                separation.x -= ( direction.x * t );
                separation.y -= ( direction.y * t );
                separation.z -= ( direction.z * t );
                float s = separation.Dot( edge.tplane.t_vec3() );
                if( fabsf( s - edge.cpoint.w ) <= edge.cpoint.w )
                {   //  the collision is within the extent of the edge
                    normal.x = ( ( edge.tplane.x * s ) - separation.x );
                    normal.y = ( ( edge.tplane.y * s ) - separation.y );
                    normal.z = ( ( edge.tplane.z * s ) - separation.z );
                    normal.Normalize();
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool BaseCollider::edgeCollide( const maths::sphere& asphere, const maths::vec3& direction, const Edge& edge, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    maths::vec3 uaxis;
    uaxis.SetCross( edge.tplane.t_vec3(), direction );  //  closest points axis, magnitude is length of direction * sine of angle
    float uu = uaxis.SqrLen();
    float dd = direction.SqrLen();
    float ud = ( uu * ( dd - uu ) );
    if( ud >= maths::consts::FLOAT_MIN_NORM )
    {   //  the edge is not aligned with the direction
        maths::vec3 separation;
        separation.SetSub( edge.cpoint.t_vec3(), asphere.t_vec3() );
        float rr = ( asphere.r * asphere.r );
        float us = uaxis.Dot( separation );
        float ru = ( ( rr * uu ) - ( us * us ) );
        if( ru > 0.0f )
        {   //  there is a collision with the infinitely extended edge
            maths::vec3 vaxis;
            vaxis.SetCross( edge.tplane.t_vec3(), uaxis );  //  approach axis, magnitude is length of direction * sine of angle
            float vd = vaxis.Dot( direction );              //  length of direction * sine of angle * length of direction * cosine of angle
            if( fabsf( vd ) >= maths::consts::FLOAT_MIN_NORM )
            {   //  the edge is not aligned with the direction (second check to be certain that there will not be a division by zero)
                t = ( ( vaxis.Dot( separation ) / vd ) - sqrtf( ru / ud ) );
                separation.x -= ( direction.x * t );
                separation.y -= ( direction.y * t );
                separation.z -= ( direction.z * t );
                float s = separation.Dot( edge.tplane.t_vec3() );
                if( fabsf( s - edge.cpoint.w ) <= edge.cpoint.w )
                {   //  the collision is within the extent of the edge
                    maths::vec3 point;
                    point.x = ( edge.cpoint.x - ( edge.tplane.x * s ) );
                    point.y = ( edge.cpoint.y - ( edge.tplane.y * s ) );
                    point.z = ( edge.cpoint.z - ( edge.tplane.z * s ) );
                    if( point.Dot( reject ) > 0.0f )
                    {   //  the collision point is not rejected
                        normal.x = ( ( edge.tplane.x * s ) - separation.x );
                        normal.y = ( ( edge.tplane.y * s ) - separation.y );
                        normal.z = ( ( edge.tplane.z * s ) - separation.z );
                        normal.Normalize();
                        return( true );
                    }
                }
            }
        }
    }
    return( false );
}

bool BaseCollider::pointCollide( const maths::sphere& asphere, const maths::vec3& direction, const maths::vec3& point, float& t, maths::vec3& normal ) const
{
    float dd = direction.SqrLen();
    if( dd >= maths::consts::FLOAT_MIN_NORM )
    {   //  the movement vector is long enough to calculate a collision
        maths::vec3 separation;
        separation.SetSub( point, asphere.t_vec3() );
        float rr = ( asphere.r * asphere.r );
        float ss = separation.SqrLen();
        float sd = separation.Dot( direction );
        ss = ( ( sd * sd ) - ( ( ss - rr ) * dd ) );
        if( ss > 0.0f )
        {
            t = ( ( sd - sqrtf( ss ) ) / dd );
            normal.x = ( ( direction.x * t ) - separation.x );
            normal.y = ( ( direction.y * t ) - separation.y );
            normal.z = ( ( direction.z * t ) - separation.z );
            normal.Normalize();
            return( true );
        }
    }
    return( false );
}

bool BaseCollider::pointCollide( const maths::sphere& asphere, const maths::vec3& direction, const maths::vec3& point, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    return( ( point.Dot( reject ) > 0.0f ) ? pointCollide( asphere, direction, point, t, normal ) : false );
}

bool BaseCollider::edgeExclude( const maths::sphere& asphere, const Edge& edge, maths::vec3& reposition ) const
{
    float s = asphere.t_vec3().Dot( edge.tplane );
    if( fabsf( s ) < edge.cpoint.w )
    {
        s += edge.cpoint.w;
        maths::vec3 point;
        point.x = ( edge.cpoint.x + ( edge.tplane.x * s ) );
        point.y = ( edge.cpoint.y + ( edge.tplane.y * s ) );
        point.z = ( edge.cpoint.z + ( edge.tplane.z * s ) );
        return( pointExclude( asphere, point, reposition ) );
    }
    return( false );
}

bool BaseCollider::edgeExclude( const maths::sphere& asphere, const Edge& edge, maths::vec3& reposition, const maths::plane& reject ) const
{
    float s = asphere.t_vec3().Dot( edge.tplane );
    if( fabsf( s ) < edge.cpoint.w )
    {
        s += edge.cpoint.w;
        maths::vec3 point;
        point.x = ( edge.cpoint.x + ( edge.tplane.x * s ) );
        point.y = ( edge.cpoint.y + ( edge.tplane.y * s ) );
        point.z = ( edge.cpoint.z + ( edge.tplane.z * s ) );
        return( pointExclude( asphere, point, reposition, reject ) );
    }
    return( false );
}

bool BaseCollider::pointExclude( const maths::sphere& asphere, const maths::vec3& point, maths::vec3& reposition ) const
{
    maths::vec3 delta;
    delta.x = ( asphere.x - point.x );
    delta.y = ( asphere.y - point.y );
    delta.z = ( asphere.z - point.z );
    float ss = delta.SqrLen();
    if( ss >= maths::consts::FLOAT_MIN_NORM )
    {   //  it will be possible to determine an offset direction
        float rr = ( asphere.r * asphere.r );
        if( ss < rr )
        {   //  there is a collision
            float t = ( sqrtf( rr / ss ) - 1.0f );
            reposition.x = ( asphere.x + ( delta.x * t ) );
            reposition.y = ( asphere.y + ( delta.y * t ) );
            reposition.z = ( asphere.z + ( delta.z * t ) );
            return( true );
        }
    }
    return( false );
}

bool BaseCollider::pointExclude( const maths::sphere& asphere, const maths::vec3& point, maths::vec3& reposition, const maths::plane& reject ) const
{
    return( ( point.Dot( reject ) > 0.0f ) ? pointExclude( asphere, point, reposition ) : false );
}

bool BaseCollider::edgeExclude( const capsule& acapsule, const Edge& edge, maths::vec3& reposition ) const
{
    float s = acapsule.center.t_vec3().Dot( edge.tplane );
    float o = acapsule.offset.t_vec3().Dot( edge.tplane.t_vec3() );
    float a = ( fabsf( o ) + edge.cpoint.w );
    if( ( ( s - a  ) < 0.0f ) && ( ( s + a ) > 0.0f ) )
    {   //  the capsule 'overlaps' the edge
        maths::sphere asphere = acapsule.center;
        maths::vec3 triple;
        triple.SetTriple( edge.cdelta.t_vec3(), edge.cdelta.t_vec3(), acapsule.offset.t_vec3() );
        float t = acapsule.offset.t_vec3().Dot( triple );
        if( fabsf( t ) >= maths::consts::FLOAT_MIN_NORM )
        {   //  the capsule is not a sphere and not parallel to the edge
            t = ( ( acapsule.center.t_vec3().Dot( triple ) - edge.cpoint.t_vec3().Dot( triple ) ) / t );
            if( t < -1.0f ) t = -1.0f;
            if( t > 1.0f ) t = 1.0f;
            asphere.x -= ( acapsule.offset.x * t );
            asphere.y -= ( acapsule.offset.y * t );
            asphere.z -= ( acapsule.offset.z * t );
            s -= ( o * t );
        }
        s += edge.cpoint.w;
        maths::vec3 point;
        point.x = ( edge.cpoint.x + ( edge.tplane.x * s ) );
        point.y = ( edge.cpoint.y + ( edge.tplane.y * s ) );
        point.z = ( edge.cpoint.z + ( edge.tplane.z * s ) );
        return( pointExclude( asphere, point, reposition ) );
    }
    return( false );
}

bool BaseCollider::edgeExclude( const capsule& acapsule, const Edge& edge, maths::vec3& reposition, const maths::plane& reject ) const
{
    float s = acapsule.center.t_vec3().Dot( edge.tplane );
    float o = acapsule.offset.t_vec3().Dot( edge.tplane.t_vec3() );
    float a = ( fabsf( o ) + edge.cpoint.w );
    if( ( ( s - a  ) < 0.0f ) && ( ( s + a ) > 0.0f ) )
    {   //  the capsule 'overlaps' the edge
        maths::sphere asphere = acapsule.center;
        maths::vec3 triple;
        triple.SetTriple( edge.cdelta.t_vec3(), edge.cdelta.t_vec3(), acapsule.offset.t_vec3() );
        float t = acapsule.offset.t_vec3().Dot( triple );
        if( fabsf( t ) >= maths::consts::FLOAT_MIN_NORM )
        {   //  the capsule is not a sphere and not parallel to the edge
            t = ( ( acapsule.center.t_vec3().Dot( triple ) - edge.cpoint.t_vec3().Dot( triple ) ) / t );
            if( t < -1.0f ) t = -1.0f;
            if( t > 1.0f ) t = 1.0f;
            asphere.x -= ( acapsule.offset.x * t );
            asphere.y -= ( acapsule.offset.y * t );
            asphere.z -= ( acapsule.offset.z * t );
            s -= ( o * t );
        }
        s += edge.cpoint.w;
        maths::vec3 point;
        point.x = ( edge.cpoint.x + ( edge.tplane.x * s ) );
        point.y = ( edge.cpoint.y + ( edge.tplane.y * s ) );
        point.z = ( edge.cpoint.z + ( edge.tplane.z * s ) );
        return( pointExclude( asphere, point, reposition, reject ) );
    }
    return( false );
}

bool BaseCollider::pointExclude( const capsule& acapsule, const maths::vec3& point, maths::vec3& reposition ) const
{
    maths::vec3 delta;
    delta.x = ( acapsule.center.x - point.x );
    delta.y = ( acapsule.center.y - point.y );
    delta.z = ( acapsule.center.z - point.z );
    if( acapsule.offset.w >= maths::consts::FLOAT_MIN_NORM )
    {   //  the capsule is not a sphere
        float t = ( delta.Dot( acapsule.offset.t_vec3() ) / acapsule.offset.w );
        if( t < -1.0f ) t = -1.0f;
        if( t > 1.0f ) t = 1.0f;
        delta.x -= ( acapsule.offset.x * t );
        delta.y -= ( acapsule.offset.y * t );
        delta.z -= ( acapsule.offset.z * t );
    }
    float ss = delta.SqrLen();
    if( ss >= maths::consts::FLOAT_MIN_NORM )
    {   //  it will be possible to determine an offset direction
        float rr = ( acapsule.center.r * acapsule.center.r );
        if( ss < rr )
        {   //  there is a collision
            float t = ( sqrtf( rr / ss ) - 1.0f );
            reposition.x = ( acapsule.center.x + ( delta.x * t ) );
            reposition.y = ( acapsule.center.y + ( delta.y * t ) );
            reposition.z = ( acapsule.center.z + ( delta.z * t ) );
            return( true );
        }
    }
    return( false );
}

bool BaseCollider::pointExclude( const capsule& acapsule, const maths::vec3& point, maths::vec3& reposition, const maths::plane& reject ) const
{
    return( ( point.Dot( reject ) > 0.0f ) ? pointExclude( acapsule, point, reposition ) : false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DoorCollider door collisions class static member data
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float DoorCollider::s_inset = ( 1.0f / 32.0f );
float DoorCollider::s_zbias = ( 1.0f / 1024.0f );
float DoorCollider::s_znear = ( 1.0f / 256.0f );
float DoorCollider::s_zview = ( 1.0f / 2048.0f );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DoorCollider door collisions class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool DoorCollider::create( const maths::vec2* const verts, const uint count )
{
    kill();
    if( init( verts, count ) )
    {
        return( true );
    }
    kill();
    return( false );
}

bool DoorCollider::create( const maths::vec2& scale, const maths::vec2& offset, const maths::vec2* const verts, const uint count )
{
    kill();
    if( init( verts, count ) && init( scale, offset ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void DoorCollider::destroy()
{
    if( valid() )
    {
        kill();
    }
}

bool DoorCollider::relocate( const maths::vec2& scale, const maths::vec2& offset )
{
    return( valid() ? init( scale, offset ) : false );
}

bool DoorCollider::isVisible( const maths::joint& localToView, const maths::plane& viewPlane ) const
{
    if( active() )
    {
        maths::plane clipPlane;
        clipPlane.w = localToView.v.Dot( viewPlane );
        localToView.q.ConjugateTransform( viewPlane.t_vec3(), clipPlane.t_vec3() );
        for( uint index = 0; index < m_count; ++index )
        {   //  check if any point of the door is in front of the clip plane
            Edge& edge = m_sides[ index ].edge;
            if( edge.cpoint.t_vec3().Dot( clipPlane ) > s_zview )
            {
                return( true );
            }
        }
    }
    return( false );
}

EViewState DoorCollider::identifyView( const maths::joint& localToView ) const
{
    uint flags = 0;
    if( active() )
    {   //  the door is 'open'
        maths::joint viewToLocal;
        viewToLocal.SetInverse( localToView );
        flags = ( ( viewToLocal.v.z <= 0.0f ) ? EViewFace : EViewBack );
        if( fabsf( viewToLocal.v.z ) <= s_zview )
        {   //  the view position z is within the tolerance zone
            flags |= EViewNear;
        }
        if( fabsf( viewToLocal.v.z ) <= s_inset )
        {   //  the view position z is within the near clip safety zone
            if( ( viewToLocal.v.x > m_rect.l ) && ( viewToLocal.v.x < m_rect.r ) && ( viewToLocal.v.y > m_rect.b ) && ( viewToLocal.v.y < m_rect.t ) )
            {   //  the view position is within the collidable rectangle
                if( contains( viewToLocal.v.t_vec2() ) )
                {   //  the view position is within the door area
                    flags |= EViewFrom;
                }
            }
        }
    }
    return( static_cast< EViewState >( flags ) );
}

bool DoorCollider::containsPoint( const maths::vec3& point ) const
{
    if( active() )
    {
        if( fabsf( point.z ) <= s_inset )
        {
            if( ( point.x > m_rect.l ) && ( point.x < m_rect.r ) && ( point.y > m_rect.b ) && ( point.y < m_rect.t ) )
            {   //  the point is within the collidable rectangle
                return( contains( point.t_vec2() ) );
            }
        }
    }
    return( false );
}

bool DoorCollider::traversePoint( const EDoorState state, const maths::vec3& start, const maths::vec3& end ) const
{
    if( active() )
    {
        if( ( start.z < end.z ) ? ( ( state & EDoorFaceOpen ) && ( start.z <= 0.0f ) && ( ( end.z + maths::consts::FLOAT_EPSILON ) > 0.0f ) ) : ( ( state & EDoorBackOpen ) && ( start.z >= 0.0f ) && ( ( end.z - maths::consts::FLOAT_EPSILON ) < 0.0f ) ) )
        {
            maths::vec2 point;
            float i = ( end.z - start.z );
            if( i >= maths::consts::FLOAT_MIN_RCP )
            {
                point.x = ( ( ( start.x * end.z ) - ( end.x * start.z ) ) / i );
                point.y = ( ( ( start.y * end.z ) - ( end.y * start.z ) ) / i );
            }
            else
            {
                point.x = ( ( start.x + end.x ) * 0.5f );
                point.y = ( ( start.y + end.y ) * 0.5f );
            }
            if( ( point.x > m_rect.l ) && ( point.x < m_rect.r ) && ( point.y > m_rect.b ) && ( point.y < m_rect.t ) )
            {   //  the point is within the collidable rectangle
                return( contains( point ) );
            }
        }
    }
    return( false );
}

bool DoorCollider::touchesSphere( const EDoorState state, const maths::sphere& asphere ) const
{
    if( active() )
    {   //  the door is 'open'
        if( ( ( asphere.z + asphere.r + s_zbias ) > 0.0f ) && ( ( asphere.z - asphere.r - s_zbias ) < 0.0f ) )
        {   //  the sphere is within the touchable z range
            if( ( ( ( asphere.z - s_zbias ) <= 0.0f ) && ( state & EDoorFaceOpen ) ) || ( ( ( asphere.z + s_zbias ) >= 0.0f ) && ( state & EDoorBackOpen ) ) )
            {   //  the sphere is on an open side of the door
                if( ( asphere.x > m_rect.l ) && ( asphere.x < m_rect.r ) && ( asphere.y > m_rect.b ) && ( asphere.y < m_rect.t ) )
                {   //  the sphere center is within the collidable rectangle
                    return( contains( asphere.t_vec2() ) );
                }
            }
        }
    }
    return( false );
}

bool DoorCollider::collidePoint( const EDoorState state, const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const
{
    bool collision = false;
    float collision_t = max_t;
    maths::vec3 collision_normal = maths::consts::VEC3::UNIT_Z;
    if( active() )
    {   //  the door is 'open'
        if( ( point.z <= 0.0f ) ? ( ( state & EDoorFaceOpen ) != EDoorFaceOpen ) : ( ( state & EDoorBackOpen ) != EDoorBackOpen ) )
        {   //  the point is on a closed side of the door
            if( fabsf( direction.z ) >= maths::consts::FLOAT_MIN_NORM )
            {   //  the point is moving perpendicular to the door sufficiently to test for a collision with the surface
                float intersect_t = ( point.z / direction.z );
                if( ( intersect_t >= min_t ) && ( intersect_t <= max_t ) )
                {   //  the intersection point is within the specified limits of t
                    maths::vec2 intersect;
                    intersect.x = ( point.x + ( direction.x * intersect_t ) );
                    intersect.y = ( point.y + ( direction.y * intersect_t ) );
                    if( ( intersect.x > m_rect.l ) && ( intersect.x < m_rect.r ) && ( intersect.y > m_rect.b ) && ( intersect.y < m_rect.t ) )
                    {   //  the intersect point is within the collidable rectangle
                        if( contains( intersect ) )
                        {   //  the intersect point is within the door
                            collision = true;
                            collision_t = intersect_t;
                            collision_normal.z = ( ( point.z <= 0.0f ) ? -1.0f : 1.0f );
                        }
                    }
                }
            }
        }
    }
    t = collision_t;
    normal = collision_normal;
    return( collision );
}

bool DoorCollider::collidePoint( const EDoorState state, const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    bool collision = false;
    float collision_t = max_t;
    maths::vec3 collision_normal = maths::consts::VEC3::UNIT_Z;
    if( active() )
    {   //  the door is 'open'
        if( ( point.z <= 0.0f ) ? ( ( state & EDoorFaceOpen ) != EDoorFaceOpen ) : ( ( state & EDoorBackOpen ) != EDoorBackOpen ) )
        {   //  the point is on a closed side of the door
            if( fabsf( direction.z ) >= maths::consts::FLOAT_MIN_NORM )
            {   //  the point is moving perpendicular to the door sufficiently to test for a collision with the surface
                float intersect_t = ( point.z / direction.z );
                if( ( intersect_t >= min_t ) && ( intersect_t <= max_t ) )
                {   //  the intersection point is within the specified limits of t
                    maths::vec2 intersect;
                    intersect.x = ( point.x + ( direction.x * intersect_t ) );
                    intersect.y = ( point.y + ( direction.y * intersect_t ) );
                    if( ( ( intersect.x * reject.x ) + ( intersect.y * reject.y ) + reject.w ) > 0.0f )
                    {   //  the surface is not rejected
                        if( ( intersect.x > m_rect.l ) && ( intersect.x < m_rect.r ) && ( intersect.y > m_rect.b ) && ( intersect.y < m_rect.t ) )
                        {   //  the intersect point is within the collidable rectangle
                            if( contains( intersect ) )
                            {   //  the intersect point is within the door
                                collision = true;
                                collision_t = intersect_t;
                                collision_normal.z = ( ( point.z <= 0.0f ) ? -1.0f : 1.0f );
                            }
                        }
                    }
                }
            }
        }
    }
    t = collision_t;
    normal = collision_normal;
    return( collision );
}

bool DoorCollider::collideSphere( const EDoorState state, const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const
{
    bool collision = false;
    float collision_t = max_t;
    maths::vec3 collision_normal = maths::consts::VEC3::UNIT_Y;
    if( active() )
    {   //  the door is 'open'
        float dd = direction.SqrLen();
        if( dd >= maths::consts::FLOAT_MIN_NORM )
        {   //  the sphere is moving sufficiently to test for an intersection
            if( fabsf( direction.z ) >= maths::consts::FLOAT_MIN_NORM )
            {   //  the sphere is moving perpendicular to the door sufficiently to test for a collision with the surface
                if( asphere.z <= 0.0f  )
                {   //  the sphere is on the 'face' side of the door
                    if( ( state & EDoorFaceOpen ) != EDoorFaceOpen )
                    {   //  the face of the door is closed so we need to test for collision with the surface
                        float intercept_t = ( ( asphere.z + asphere.r ) / direction.z );
                        if( ( intercept_t >= min_t ) && ( intercept_t <= max_t ) )
                        {
                            maths::vec2 intercept;
                            intercept.x = ( asphere.x + ( direction.x * intercept_t ) );
                            intercept.y = ( asphere.y + ( direction.y * intercept_t ) );
                            if( ( intercept.x > m_rect.l ) && ( intercept.x < m_rect.r ) && ( intercept.y > m_rect.b ) && ( intercept.y < m_rect.t ) )
                            {   //  the sphere center is within the collidable rectangle
                                if( contains( intercept ) )
                                {
                                    collision = true;
                                    collision_t = intercept_t;
                                    collision_normal.x = collision_normal.y = 0.0f;
                                    collision_normal.z = -1.0f;
                                }
                            }
                        }
                    }
                }
                else
                {   //  the sphere is on the 'back' side of the door
                    if( ( state & EDoorBackOpen ) != EDoorBackOpen )
                    {   //  the back of the door is closed so we need to test for collision with the surface
                        float intercept_t = ( ( asphere.z - asphere.r ) / direction.z );
                        if( ( intercept_t >= min_t ) && ( intercept_t <= max_t ) )
                        {
                            maths::vec2 intercept;
                            intercept.x = ( asphere.x + ( direction.x * intercept_t ) );
                            intercept.y = ( asphere.y + ( direction.y * intercept_t ) );
                            if( ( intercept.x > m_rect.l ) && ( intercept.x < m_rect.r ) && ( intercept.y > m_rect.b ) && ( intercept.y < m_rect.t ) )
                            {   //  the sphere center is within the collidable rectangle
                                if( contains( intercept ) )
                                {
                                    collision = true;
                                    collision_t = intercept_t;
                                    collision_normal.x = collision_normal.y = 0.0f;
                                    collision_normal.z = 1.0f;
                                }
                            }
                        }
                    }
                }
            }
            if( !collision )
            {   //  check for collision with all the edges and verts
                for( uint index = 0; index < m_count; ++index )
                {
                    const Edge& edge = m_sides[ index ].edge;
                    float point_t = 0.0f;
                    maths::vec3 point_normal;
                    if( edgeCollide( asphere, direction, edge, point_t, point_normal ) || pointCollide( asphere, direction, edge.cpoint.t_vec3(), point_t, point_normal ) )
                    {
                        if( ( point_t >= min_t ) && ( point_t <= max_t ) )
                        {
                            if( ( !collision ) || ( collision_t > t ) )
                            {
                                collision = true;
                                collision_t = point_t;
                                collision_normal = point_normal;
                            }
                        }
                    }
                }
            }
        }
    }
    t = collision_t;
    normal = collision_normal;
    return( collision );
}

bool DoorCollider::collideSphere( const EDoorState state, const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    bool collision = false;
    float collision_t = max_t;
    maths::vec3 collision_normal = maths::consts::VEC3::UNIT_Y;
    if( active() )
    {   //  the door is 'open'
        float dd = direction.SqrLen();
        if( dd >= maths::consts::FLOAT_MIN_NORM )
        {   //  the sphere is moving sufficiently to test for an intception
            if( fabsf( direction.z ) >= maths::consts::FLOAT_MIN_NORM )
            {   //  the sphere is moving perpendicular to the door sufficiently to test for a collision with the surface
                if( asphere.z <= 0.0f  )
                {   //  the sphere is on the 'face' side of the door
                    if( ( state & EDoorFaceOpen ) != EDoorFaceOpen )
                    {   //  the face of the door is closed so we need to test for collision with the surface
                        float intercept_t = ( ( asphere.z + asphere.r ) / direction.z );
                        if( ( intercept_t >= min_t ) && ( intercept_t <= max_t ) )
                        {
                            maths::vec2 intercept;
                            intercept.x = ( asphere.x + ( direction.x * intercept_t ) );
                            intercept.y = ( asphere.y + ( direction.y * intercept_t ) );
                            if( ( ( intercept.x * reject.x ) + ( intercept.y * reject.y ) + reject.w ) > 0.0f )
                            {   //  the surface is not rejected
                                if( ( intercept.x > m_rect.l ) && ( intercept.x < m_rect.r ) && ( intercept.y > m_rect.b ) && ( intercept.y < m_rect.t ) )
                                {   //  the sphere center is within the collidable rectangle
                                    if( contains( intercept ) )
                                    {
                                        collision = true;
                                        collision_t = intercept_t;
                                        collision_normal.x = collision_normal.y = 0.0f;
                                        collision_normal.z = -1.0f;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {   //  the sphere is on the 'back' side of the door
                    if( ( state & EDoorBackOpen ) != EDoorBackOpen )
                    {   //  the back of the door is closed so we need to test for collision with the surface
                        float intercept_t = ( ( asphere.z - asphere.r ) / direction.z );
                        if( ( intercept_t >= min_t ) && ( intercept_t <= max_t ) )
                        {
                            maths::vec2 intercept;
                            intercept.x = ( asphere.x + ( direction.x * intercept_t ) );
                            intercept.y = ( asphere.y + ( direction.y * intercept_t ) );
                            if( ( ( intercept.x * reject.x ) + ( intercept.y * reject.y ) + reject.w ) > 0.0f )
                            {   //  the surface is not rejected
                                if( ( intercept.x > m_rect.l ) && ( intercept.x < m_rect.r ) && ( intercept.y > m_rect.b ) && ( intercept.y < m_rect.t ) )
                                {   //  the sphere center is within the collidable rectangle
                                    if( contains( intercept ) )
                                    {
                                        collision = true;
                                        collision_t = intercept_t;
                                        collision_normal.x = collision_normal.y = 0.0f;
                                        collision_normal.z = 1.0f;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if( !collision )
            {   //  check for collision with all the edges and verts
                for( uint index = 0; index < m_count; ++index )
                {
                    const Edge& edge = m_sides[ index ].edge;
                    float point_t = 0.0f;
                    maths::vec3 point_normal;
                    if( edgeCollide( asphere, direction, edge, point_t, point_normal, reject ) || pointCollide( asphere, direction, edge.cpoint.t_vec3(), point_t, point_normal, reject ) )
                    {
                        if( ( point_t >= min_t ) && ( point_t <= max_t ) )
                        {
                            if( ( !collision ) || ( collision_t > t ) )
                            {
                                collision = true;
                                collision_t = point_t;
                                collision_normal = point_normal;
                            }
                        }
                    }
                }
            }
        }
    }
    t = collision_t;
    normal = collision_normal;
    return( collision );
}

bool DoorCollider::excludeSphere( const EDoorState state, const maths::sphere& asphere, maths::vec3& reposition ) const
{
    reposition = asphere.t_vec3();
    if( active() )
    {   //  the door is 'open'
        if( ( ( asphere.z + asphere.r ) > 0.0f ) && ( ( asphere.z - asphere.r ) < 0.0f ) )
        {   //  the sphere is within the collidable z range
            if( ( ( asphere.x + asphere.r ) > m_rect.l ) && ( ( asphere.x - asphere.r ) < m_rect.r ) && ( ( asphere.y + asphere.r ) > m_rect.b ) && ( ( asphere.y - asphere.r ) < m_rect.t ) )
            {   //  the sphere is within the collidable rectangle
                if( contains( asphere.t_vec2(), asphere.r ) )
                {   //  the sphere is within the collidable volume
                    if( contains( asphere.t_vec2() ) )
                    {   //  the sphere is within the face area
                        float epsilon = 0.0f;   //  epsilon value to ensure that the sphere does not end up stuck inside the door
                        if( asphere.z <= 0.0f )
                        {   //  the sphere is on the 'face' side of the door
                            if( ( state & EDoorFaceOpen ) != EDoorFaceOpen )
                            {   //  the sphere is colliding with the closed 'face' of the door
                                reposition.z = ( 0.0f - asphere.r );
                                return( true );
                            }
                            epsilon = ( 0.0f - maths::consts::FLOAT_EPSILON );
                        }
                        else
                        {   //  the sphere is on the 'back' side of the door
                            if( ( state & EDoorBackOpen ) != EDoorBackOpen )
                            {   //  the sphere is colliding with the closed 'back' of the door
                                reposition.z = asphere.r;
                                return( true );
                            }
                            epsilon = maths::consts::FLOAT_EPSILON;
                        }
                        if( !contains( asphere.t_vec2(), ( 0.0f - asphere.r ) ) )
                        {   //  the sphere has not trivially entered the door without colliding
                            bool collision = false;
                            uint index = 0;
                            maths::sphere bounce = asphere;
                            for( uint count = m_count; count; --count )
                            {
                                if( edgeExclude( bounce, m_sides[ index ].edge, bounce.t_vec3() ) )
                                {
                                    collision = true;
                                    bounce.z += epsilon;
                                    epsilon += epsilon;
                                    count = ( m_count + 1 );
                                }
                                index = ( ( index + 1 ) % m_count );
                            }
                            if( collision )
                            {
                                reposition = bounce.t_vec3();
                            }
                            return( collision );
                        }
                    }
                    else
                    {
                        for( uint index = 0; index < m_count; ++index )
                        {   //  collide with edges
                            if( edgeExclude( asphere, m_sides[ index ].edge, reposition ) )
                            {
                                return( true );
                            }
                        }
                        for( uint index = 0; index < m_count; ++index )
                        {   //  collide with points
                            if( pointExclude( asphere, m_sides[ index ].edge.cpoint.t_vec3(), reposition ) )
                            {
                                return( true );
                            }
                        }
                    }
                }
            }
        }
    }
    return( false );
}

bool DoorCollider::excludeSphere( const EDoorState state, const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const
{
    reposition = asphere.t_vec3();
    if( active() )
    {   //  the door is 'open'
        if( ( ( asphere.x * reject.x ) + ( asphere.y * reject.y ) + reject.w ) > 0.0f )
        {   //  the surface is not rejected
            if( ( ( asphere.z + asphere.r ) > 0.0f ) && ( ( asphere.z - asphere.r ) < 0.0f ) )
            {   //  the sphere is within the collidable z range
                if( ( ( asphere.x + asphere.r ) > m_rect.l ) && ( ( asphere.x - asphere.r ) < m_rect.r ) && ( ( asphere.y + asphere.r ) > m_rect.b ) && ( ( asphere.y - asphere.r ) < m_rect.t ) )
                {   //  the sphere is within the collidable rectangle
                    if( contains( asphere.t_vec2(), asphere.r ) )
                    {   //  the sphere is within the collidable volume
                        if( contains( asphere.t_vec2() ) )
                        {   //  the sphere is within the face area
                            float epsilon = 0.0f;   //  epsilon value to ensure that the sphere does not end up stuck inside the door
                            if( asphere.z <= 0.0f )
                            {   //  the sphere is on the 'face' side of the door
                                if( ( state & EDoorFaceOpen ) != EDoorFaceOpen )
                                {   //  the sphere is colliding with the closed 'face' of the door
                                    reposition.z = ( 0.0f - asphere.r );
                                    return( true );
                                }
                                epsilon = ( 0.0f - maths::consts::FLOAT_EPSILON );
                            }
                            else
                            {   //  the sphere is on the 'back' side of the door
                                if( ( state & EDoorBackOpen ) != EDoorBackOpen )
                                {   //  the sphere is colliding with the closed 'back' of the door
                                    reposition.z = asphere.r;
                                    return( true );
                                }
                                epsilon = maths::consts::FLOAT_EPSILON;
                            }
                            if( !contains( asphere.t_vec2(), ( 0.0f - asphere.r ) ) )
                            {   //  the sphere has not trivially entered the door without colliding
                                bool collision = false;
                                uint index = 0;
                                maths::sphere bounce = asphere;
                                for( uint count = m_count; count; --count )
                                {
                                    if( edgeExclude( bounce, m_sides[ index ].edge, bounce.t_vec3(), reject ) )
                                    {
                                        collision = true;
                                        bounce.z += epsilon;
                                        epsilon += epsilon;
                                        count = ( m_count + 1 );
                                    }
                                    index = ( ( index + 1 ) % m_count );
                                }
                                if( collision )
                                {
                                    reposition = bounce.t_vec3();
                                }
                                return( collision );
                            }
                        }
                        else
                        {
                            for( uint index = 0; index < m_count; ++index )
                            {   //  collide with edges
                                if( edgeExclude( asphere, m_sides[ index ].edge, reposition, reject ) )
                                {
                                    return( true );
                                }
                            }
                            for( uint index = 0; index < m_count; ++index )
                            {   //  collide with points
                                if( pointExclude( asphere, m_sides[ index ].edge.cpoint.t_vec3(), reposition, reject ) )
                                {
                                    return( true );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return( false );
}

bool DoorCollider::interceptPoint( const EDoorState state, const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, bool& face ) const
{
    maths::sphere asphere;
    asphere.t_vec3() = point;
    asphere.r = 0.0f;
    return( interceptSphere( state, asphere, direction, min_t, max_t, t, face ) );
}

bool DoorCollider::interceptSphere( const EDoorState state, const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, bool& face ) const
{
    if( active() && ( state != EDoorClosed ) )
    {   //  the door is 'open'
        if( fabsf( direction.z ) >= maths::consts::FLOAT_MIN_NORM )
        {   //  the sphere is moving perpendicular to the door sufficiently to test for an intersection
            float intercept_t = asphere.z;
            if( direction.z > 0.0f )
            {   //  the sphere is moving towards the face of the door
                if( ( state & EDoorFaceOpen ) != EDoorFaceOpen )
                {   //  the face of the door is closed
                    return( false );
                }
                face = true;
                intercept_t += asphere.r;
            }
            else
            {   //  the sphere is moving towards the back of the door
                if( ( state & EDoorBackOpen ) != EDoorBackOpen )
                {   //  the back of the door is closed
                    return( false );
                }
                face = false;
                intercept_t -= asphere.r;
            }
            intercept_t /= direction.z;
            if( ( intercept_t >= min_t ) && ( intercept_t <= max_t ) )
            {
                maths::vec2 intercept;
                intercept.x = ( asphere.x + ( direction.x * intercept_t ) );
                intercept.y = ( asphere.y + ( direction.y * intercept_t ) );
                if( ( intercept.x > m_rect.l ) && ( intercept.x < m_rect.r ) && ( intercept.y > m_rect.b ) && ( intercept.y < m_rect.t ) )
                {   //  the sphere center is within the collidable rectangle
                    if( contains( intercept ) )
                    {
                        t = intercept_t;
                        return( true );
                    }
                }
            }
        }
    }
    return( false );
}

bool DoorCollider::includingSphere( const EDoorState state, const maths::sphere& asphere, bool& face ) const
{
    if( active() && ( state != EDoorClosed ) )
    {   //  the door is 'open'
        if( ( ( asphere.z + asphere.r + maths::consts::FLOAT_EPSILON ) > 0.0f ) && ( ( asphere.z - asphere.r - maths::consts::FLOAT_EPSILON ) < 0.0f ) )
        {   //  the sphere is within the touchable z range
            if( ( ( ( asphere.z - maths::consts::FLOAT_EPSILON ) <= 0.0f ) && ( state & EDoorFaceOpen ) ) || ( ( ( asphere.z + maths::consts::FLOAT_EPSILON ) >= 0.0f ) && ( state & EDoorBackOpen ) ) )
            {   //  the sphere is on an open side of the door
                if( ( asphere.x > m_rect.l ) && ( asphere.x < m_rect.r ) && ( asphere.y > m_rect.b ) && ( asphere.y < m_rect.t ) )
                {   //  the sphere center is within the collidable rectangle
                    if( contains( asphere.t_vec2() ) )
                    {
                        face = ( ( state & EDoorFaceOpen ) ? ( ( state & EDoorBackOpen ) ? ( asphere.z <= 0.0f ) : true ) : false );
                        return( true );
                    }
                }
            }
        }
    }
    return( false );
}

bool DoorCollider::getViewExtents( const maths::joint& localToView, const maths::mat44& projection, maths::frect& extents ) const
{
    if( active() )
    {   //  the door is 'open'
        bool valid = false;
        maths::vec3 clipped;
        maths::vec3 projected;
        maths::vec3 verts[ 2 ];
        localToView.Transform( m_sides[ m_count - 1 ].edge.cpoint.t_vec3(), verts[ 1 ] );
        uint flags = ( ( verts[ 1 ].z >= s_znear ) ? 1 : 0 );
        for( uint index = 0; index < m_count; ++index )
        {
            verts[ 0 ] = verts[ 1 ];
            localToView.Transform( m_sides[ index ].edge.cpoint.t_vec3(), verts[ 1 ] );
            if( verts[ 1 ].z >= s_znear ) flags |= 2;
            if( flags )
            {
                if( flags != 3 )
                {   //  output clipped edge
                    float delta = ( verts[ 1 ].z - verts[ 0 ].z );
                    if( fabsf( delta ) >= maths::consts::FLOAT_MIN_NORM )
                    {
                        float s = ( ( verts[ 1 ].z - s_znear ) / delta );
                        float t = ( 1.0f - s );
                        clipped.x = ( ( verts[ 0 ].x * s ) + ( verts[ 1 ].x * t ) );
                        clipped.y = ( ( verts[ 0 ].y * s ) + ( verts[ 1 ].y * t ) );
                        clipped.z = s_znear;
                        projected.SetMul( clipped, projection );
                        if( valid )
                        {
                            if( extents.l > projected.x ) extents.l = projected.x;
                            if( extents.r < projected.x ) extents.r = projected.x;
                            if( extents.b > projected.y ) extents.b = projected.y;
                            if( extents.t < projected.y ) extents.t = projected.y;
                        }
                        else
                        {
                            extents.l = extents.r = projected.x;
                            extents.t = extents.b = projected.y;
                            valid = true;
                        }
                    }
                }
                flags >>= 1;
                if( flags )
                {   //  output current point
                    projected.SetMul( verts[ 1 ], projection );
                    if( valid )
                    {
                        if( extents.l > projected.x ) extents.l = projected.x;
                        if( extents.r < projected.x ) extents.r = projected.x;
                        if( extents.b > projected.y ) extents.b = projected.y;
                        if( extents.t < projected.y ) extents.t = projected.y;
                    }
                    else
                    {
                        extents.l = extents.r = projected.x;
                        extents.t = extents.b = projected.y;
                        valid = true;
                    }
                }
            }
        }
        return( valid );
    }
    return( false );
}

bool DoorCollider::init( const maths::vec2* const verts, const uint count )
{
    if( m_sides != NULL )
    {
        delete[] m_sides;
    }
    m_count = count;
    m_sides = new Side[ count ];
    for( uint index = 0; index < count; ++index )
    {
        const maths::vec2& vert0 = verts[ index ];
        const maths::vec2& vert1 = verts[ ( index + 1 ) % count ];
        Side& side = m_sides[ index ];
        Edge& edge = side.edge;
        side.ddelta.x = ( vert1.x - vert0.x );
        side.ddelta.y = ( vert1.y - vert0.y );
        side.ddelta.w = sqrtf( ( side.ddelta.x * side.ddelta.x ) + ( side.ddelta.y * side.ddelta.y ) );
        side.dpoint.x = vert0.x;
        side.dpoint.y = vert0.y;
        side.dpoint.w = ( side.ddelta.w * 0.5f );
        side.dpoint.z = side.ddelta.z = 0.0f;
        edge.cpoint.z = edge.cdelta.z = edge.tplane.z = edge.nplane.z = 0.0f;
    }
    maths::vec2 scale = { 1.0f, 1.0f };
    maths::vec2 offset = { 0.0f, 0.0f };
    m_valid = init( scale, offset );
    return( m_valid );
}

bool DoorCollider::init( const maths::vec2& scale, const maths::vec2& offset )
{
    m_active = false;
    for( uint index = 0; index < m_count; ++index )
    {
        Side& side = m_sides[ index ];
        Edge& edge = side.edge;
        edge.cdelta.x = ( side.ddelta.x * scale.x );
        edge.cdelta.y = ( side.ddelta.y * scale.y );
        edge.cdelta.w = sqrtf( ( edge.cdelta.x * edge.cdelta.x ) + ( edge.cdelta.y * edge.cdelta.y ) );
        edge.cpoint.x = ( ( side.dpoint.x * scale.x ) + offset.x );
        edge.cpoint.y = ( ( side.dpoint.y * scale.y ) + offset.y );
        edge.cpoint.w = ( edge.cdelta.w * 0.5f );
        if( ( ( edge.cdelta.x * edge.cdelta.x ) + ( edge.cdelta.y * edge.cdelta.y ) ) < maths::consts::FLOAT_MIN_NORM )
        {
            return( false );
        }
        edge.tplane.x = ( edge.cdelta.x / edge.cdelta.w );
        edge.tplane.y = ( edge.cdelta.y / edge.cdelta.w );
        edge.tplane.w = ( 0.0f - ( ( edge.cpoint.x * edge.tplane.x ) + ( edge.cpoint.y * edge.tplane.y ) ) - edge.cpoint.w );
        edge.nplane.x = edge.tplane.y;
        edge.nplane.y = ( 0.0f - edge.tplane.x );
        edge.nplane.w = ( 0.0f - ( ( edge.cpoint.x * edge.nplane.x ) + ( edge.cpoint.y * edge.nplane.y ) ) );
        if( index )
        {
            if( m_rect.l > edge.cpoint.x ) m_rect.l = edge.cpoint.x;
            if( m_rect.r < edge.cpoint.x ) m_rect.r = edge.cpoint.x;
            if( m_rect.b > edge.cpoint.y ) m_rect.b = edge.cpoint.y;
            if( m_rect.t < edge.cpoint.y ) m_rect.t = edge.cpoint.y;
        }
        else
        {
            m_rect.l = m_rect.r = edge.cpoint.x;
            m_rect.b = m_rect.t = edge.cpoint.y;
        }
    }
    m_active = true;
    return( true );
}

void DoorCollider::kill()
{
    m_valid = false;
    m_active = false;
    m_count = 0;
    if( m_sides != NULL )
    {
        delete[] m_sides;
        m_sides = NULL;
    }
    m_rect.l = m_rect.t = m_rect.r = m_rect.b = 0.0f;
}

bool DoorCollider::contains( const maths::vec2& point, const float radius ) const
{
    float x = point.x;
    float y = point.y;
    for( uint index = 0; index < m_count; ++index )
    {
        const Edge& edge = m_sides[ index ].edge;
        float n = ( ( x * edge.nplane.x ) + ( y * edge.nplane.y ) + edge.nplane.w + radius );
        if( n < 0.0f )
        {
            return( false );
        }
    }
    return( true );
}

bool DoorCollider::intersects( const maths::vec2& start, const maths::vec2& end ) const
{
    float x1 = start.x;
    float y1 = start.y;
    float x2 = end.x;
    float y2 = end.y;
    for( uint index = 0; index < m_count; ++index )
    {
        const Edge& edge = m_sides[ index ].edge;
        float n1 = ( ( x1 * edge.nplane.x ) + ( y1 * edge.nplane.y ) + edge.nplane.w );
        float n2 = ( ( x2 * edge.nplane.x ) + ( y2 * edge.nplane.y ) + edge.nplane.w );
        if( n1 < 0.0f )
        {   //  point 1 is clipped
            if( n2 < 0.0f )
            {   //  both points are clipped
                return( false );
            }
            float d = ( n2 - n1 );
            if( d >= maths::consts::FLOAT_MIN_RCP )
            {   //  not parallel
                x1 = ( ( ( x1 * n2 ) - ( x2 * n1 ) ) / d );
                y1 = ( ( ( y1 * n2 ) - ( y2 * n1 ) ) / d );
            }
        }
        else if( n2 < 0.0f )
        {   //  point 2 is clipped
            float d = ( n1 - n2 );
            if( d >= maths::consts::FLOAT_MIN_RCP )
            {   //  not parallel
                x2 = ( ( ( x2 * n1 ) - ( x1 * n2 ) ) / d );
                y2 = ( ( ( y2 * n1 ) - ( y1 * n2 ) ) / d );
            }
        }
    }
    return( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    RoomCollider room collisions class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RoomCollider::create( const constructs::GeometryConstruct& construct )
{
    kill();
    if( init( construct ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void RoomCollider::destroy()
{
    if( valid() )
    {
        kill();
    }
}

void RoomCollider::zero()
{
    m_memory = NULL;
    m_planes = NULL;
    m_vertices = NULL;
    m_surfaces = NULL;
    m_triangles = NULL;
    m_plane_count = NULL;
    m_vertex_count = NULL;
    m_surface_count = NULL;
    m_triangle_count = NULL;
    m_sphereBounds.x = m_sphereBounds.y = m_sphereBounds.z = m_sphereBounds.r = 0.0f;
    m_aabbBounds.min.x = m_aabbBounds.min.y = m_aabbBounds.min.z = 0.0f;
    m_aabbBounds.max.x = m_aabbBounds.max.y = m_aabbBounds.max.z = 0.0f;
    m_tolerance = maths::consts::FLOAT_EPSILON;
}

bool RoomCollider::init( const constructs::GeometryConstruct& construct )
{
    const constructs::GeometrySurfaces& src_surfaces = construct.getSurfaces();
    if( src_surfaces.getUsed() > 0 )
    {
        const constructs::GeometryTriangles& src_triangles = construct.getTriangles();
        //const constructs::GeometryEdges& src_edges = construct.getEdges();
        const constructs::GeometryVerts& src_verts = construct.getVerts();
        const constructs::GeometryPositions& src_positions = construct.getPositions();
        const constructs::GeometryPlanes& src_planes = construct.getPlanes();
        m_plane_count = src_planes.getUsed();
        m_vertex_count = src_verts.getUsed();
        m_surface_count = src_surfaces.getUsed();
        m_triangle_count = src_triangles.getUsed();
        m_memory = new uint8_t[ ( m_plane_count * sizeof( maths::plane ) ) + ( m_vertex_count * sizeof( maths::vec3 ) ) + ( m_surface_count * sizeof( Surface ) ) + ( m_triangle_count * sizeof( Triangle ) ) ];
        m_planes = reinterpret_cast< maths::plane* >( m_memory );
        m_vertices = reinterpret_cast< maths::vec3* >( m_planes + m_plane_count );
        m_surfaces = reinterpret_cast< Surface* >( m_vertices + m_vertex_count );
        m_triangles = reinterpret_cast< Triangle* >( m_surfaces + m_surface_count );
        uint trg_triangle_index = 0;
        for( uint vertex_index = 0; vertex_index < m_vertex_count; ++vertex_index )
        {
            m_vertices[ vertex_index ] = src_positions.getItem( vertex_index );
        }
        for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
        {
            const constructs::GeometrySurface& src_surface = src_surfaces.getItem( surface_index );
            m_planes[ surface_index ] = src_planes.getItem( src_surface.iplane );
            uint src_triangle_count = src_surface.triangles;
            uint src_triangle_index = src_surface.itriangle;
            Surface& trg_surface = m_surfaces[ surface_index ];
            trg_surface.plane_index = surface_index;
            trg_surface.triangles_index = trg_triangle_index;
            trg_surface.triangles_count = src_triangle_count;
            while( src_triangle_count )
            {
                const constructs::GeometryTriangle& src_triangle = src_triangles.getItem( src_triangle_index );
                const constructs::GeometryVert& src_vert1 = src_verts.getItem( src_triangle.iverts[ 0 ] );
                const constructs::GeometryVert& src_vert2 = src_verts.getItem( src_triangle.iverts[ 1 ] );
                const constructs::GeometryVert& src_vert3 = src_verts.getItem( src_triangle.iverts[ 2 ] );
                Triangle& trg_triangle = m_triangles[ trg_triangle_index ];
                trg_triangle.indices[ 0 ] = src_vert1.iposition;
                trg_triangle.indices[ 1 ] = src_vert2.iposition;
                trg_triangle.indices[ 2 ] = src_vert3.iposition;
                src_triangle_index = src_triangle.inext;
                --src_triangle_count;
                ++trg_triangle_index;
            }
        }
        buildPrecalc();
        buildBounds();
        buildFlags();
        return( true );
    }
    return( false );
}

void RoomCollider::kill()
{
    if( m_memory != NULL )
    {
        delete[] m_memory;
    }
    zero();
}

bool RoomCollider::collidePoint( const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const
{
    bool collision = false;
    float collision_t = max_t;
    maths::vec3 collision_normal = maths::consts::VEC3::UNIT_Y;
    if( valid() )
    {
        float dd = direction.SqrLen();
        if( dd >= maths::consts::FLOAT_MIN_NORM )
        {   //  the point is moving sufficiently to test for an intersection
            for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
            {
                const Surface& surface = m_surfaces[ surface_index ];
                float distance = point.Dot( surface.nplane );
                if( distance >= 0.0f )
                {   //  the point is on or above the surface
                    float relative = ( 0.0f - direction.Dot( surface.nplane.t_vec3() ) );
                    if( relative >= maths::consts::UINT_FLOAT_MIN_NORM )
                    {   //  movement is not parallel to or away from the plane
                        float plane_t = ( ( distance * dd ) / relative );
                        if( ( plane_t >= min_t ) && ( plane_t <= max_t ) )
                        {
                            if( ( !collision ) || ( collision_t > plane_t ) )
                            {   //  potential closer collision
                                if( surface.flags )
                                {   //  the surface has some convex edges so we need to run full geometry tests
                                    maths::vec3 intersect;
                                    intersect.x = ( point.x + ( direction.x * plane_t ) );
                                    intersect.y = ( point.y + ( direction.y * plane_t ) );
                                    intersect.z = ( point.z + ( direction.z * plane_t ) );
                                    uint triangle_index = surface.triangles_index;
                                    for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                                    {
                                        const Triangle& triangle = m_triangles[ triangle_index ];
                                        if( containsPoint( triangle, intersect ) )
                                        {   //  inside the triangle
                                            collision = true;
                                            collision_t = plane_t;
                                            collision_normal = surface.nplane.t_vec3();
                                            break;
                                        }
                                        ++triangle_index;
                                    }
                                }
                                else
                                {   //  the surface has no convex edges or verts so the test against the surface plane is sufficient
                                    collision = true;
                                    collision_t = plane_t;
                                    collision_normal = surface.nplane.t_vec3();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    t = collision_t;
    normal = collision_normal;
    return( collision );
}

bool RoomCollider::collidePoint( const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    bool collision = false;
    float collision_t = max_t;
    maths::vec3 collision_normal = maths::consts::VEC3::UNIT_Y;
    if( valid() )
    {
        float dd = direction.SqrLen();
        if( dd >= maths::consts::FLOAT_MIN_NORM )
        {   //  the point is moving sufficiently to test for an intersection
            for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
            {
                const Surface& surface = m_surfaces[ surface_index ];
                float distance = point.Dot( surface.nplane );
                if( distance >= 0.0f )
                {   //  the point is on or above the surface
                    float relative = ( 0.0f - direction.Dot( surface.nplane.t_vec3() ) );
                    if( relative >= maths::consts::UINT_FLOAT_MIN_NORM )
                    {   //  movement is not parallel to or away from the plane
                        float plane_t = ( ( distance * dd ) / relative );
                        if( ( plane_t >= min_t ) && ( plane_t <= max_t ) )
                        {
                            if( ( !collision ) || ( collision_t > plane_t ) )
                            {   //  potential closer collision
                                maths::vec3 intersect;
                                intersect.x = ( point.x + ( direction.x * plane_t ) );
                                intersect.y = ( point.y + ( direction.y * plane_t ) );
                                intersect.z = ( point.z + ( direction.z * plane_t ) );
                                if( intersect.Dot( reject ) > 0.0f )
                                {   //  the surface is not rejected
                                    if( surface.flags )
                                    {   //  the surface has some convex edges so we need to run full geometry tests
                                        uint triangle_index = surface.triangles_index;
                                        for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                                        {
                                            const Triangle& triangle = m_triangles[ triangle_index ];
                                            if( containsPoint( triangle, intersect ) )
                                            {   //  inside the triangle
                                                collision = true;
                                                collision_t = plane_t;
                                                collision_normal = surface.nplane.t_vec3();
                                                break;
                                            }
                                            ++triangle_index;
                                        }
                                    }
                                    else
                                    {   //  the surface has no convex edges or verts so the test against the surface plane is sufficient
                                        collision = true;
                                        collision_t = plane_t;
                                        collision_normal = surface.nplane.t_vec3();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    t = collision_t;
    normal = collision_normal;
    return( collision );
}

bool RoomCollider::collideSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const
{
    bool collision = false;
    float collision_t = max_t;
    maths::vec3 collision_normal = maths::consts::VEC3::UNIT_Y;
    if( valid() )
    {
        float dd = direction.SqrLen();
        if( dd >= maths::consts::FLOAT_MIN_NORM )
        {   //  the sphere is moving sufficiently to test for an intersection
            for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
            {
                const Surface& surface = m_surfaces[ surface_index ];
                float distance = asphere.t_vec3().Dot( surface.nplane );
                if( ( distance + asphere.r ) >= 0.0f )
                {   //  part of the sphere is on or above the surface
                    float relative = ( 0.0f - direction.Dot( surface.nplane.t_vec3() ) );
                    if( relative >= maths::consts::UINT_FLOAT_MIN_NORM )
                    {   //  movement is not parallel to or away from the plane
                        float plane_t = ( ( ( distance - asphere.r ) * dd ) / relative );
                        if( ( plane_t >= min_t ) && ( plane_t <= max_t ) )
                        {
                            if( ( !collision ) || ( collision_t > plane_t ) )
                            {   //  potential closer collision
                                if( surface.flags )
                                {   //  the surface has some convex edges so we need to run full geometry tests
                                    maths::vec3 point;
                                    point.x = ( asphere.x + ( direction.x * plane_t ) );
                                    point.y = ( asphere.y + ( direction.y * plane_t ) );
                                    point.z = ( asphere.z + ( direction.z * plane_t ) );
                                    uint triangle_index = surface.triangles_index;
                                    for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                                    {
                                        const Triangle& triangle = m_triangles[ triangle_index ];
                                        if( containsPoint( triangle, point ) )
                                        {   //  inside the triangle
                                            collision = true;
                                            collision_t = plane_t;
                                            collision_normal = surface.nplane.t_vec3();
                                            break;
                                        }
                                        if( triangle.flags )
                                        {   //  we need to check for an edge or vert collision
                                            float point_t = 0.0f;
                                            maths::vec3 point_normal;
                                            if( rimCollide( asphere, direction, triangle, point_t, point_normal ) )
                                            {
                                                if( ( point_t >= min_t ) && ( point_t <= max_t ) )
                                                {
                                                    if( ( !collision ) || ( collision_t > point_t ) )
                                                    {
                                                        collision = true;
                                                        collision_t = point_t;
                                                        collision_normal = point_normal;
                                                    }
                                                }
                                            }
                                        }
                                        ++triangle_index;
                                    }
                                }
                                else
                                {   //  the surface has no convex edges or verts so the test against the surface plane is sufficient
                                    collision = true;
                                    collision_t = plane_t;
                                    collision_normal = surface.nplane.t_vec3();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    t = collision_t;
    normal = collision_normal;
    return( collision );
}

bool RoomCollider::collideSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    bool collision = false;
    float collision_t = max_t;
    maths::vec3 collision_normal = maths::consts::VEC3::UNIT_Y;
    if( valid() )
    {
        float dd = direction.SqrLen();
        if( dd >= maths::consts::FLOAT_MIN_NORM )
        {   //  the sphere is moving sufficiently to test for an intersection
            for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
            {
                const Surface& surface = m_surfaces[ surface_index ];
                float distance = asphere.t_vec3().Dot( surface.nplane );
                if( ( distance + asphere.r ) >= 0.0f )
                {   //  part of the sphere is on or above the surface
                    float relative = ( 0.0f - direction.Dot( surface.nplane.t_vec3() ) );
                    if( relative >= maths::consts::UINT_FLOAT_MIN_NORM )
                    {   //  movement is not parallel to or away from the plane
                        float plane_t = ( ( ( distance - asphere.r ) * dd ) / relative );
                        if( ( plane_t >= min_t ) && ( plane_t <= max_t ) )
                        {
                            if( ( !collision ) || ( collision_t > plane_t ) )
                            {   //  potential closer collision
                                maths::vec3 point;
                                point.x = ( asphere.x + ( direction.x * plane_t ) - ( surface.nplane.x * asphere.r ) );
                                point.y = ( asphere.y + ( direction.y * plane_t ) - ( surface.nplane.y * asphere.r ) );
                                point.z = ( asphere.z + ( direction.z * plane_t ) - ( surface.nplane.z * asphere.r ) );
                                if( point.Dot( reject ) > 0.0f )
                                {   //  the surface is not rejected
                                    if( surface.flags )
                                    {   //  the surface has some convex edges so we need to run full geometry tests
                                        uint triangle_index = surface.triangles_index;
                                        for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                                        {
                                            const Triangle& triangle = m_triangles[ triangle_index ];
                                            if( containsPoint( triangle, point ) )
                                            {   //  inside the triangle
                                                collision = true;
                                                collision_t = plane_t;
                                                collision_normal = surface.nplane.t_vec3();
                                                break;
                                            }
                                            if( triangle.flags )
                                            {   //  we need to check for an edge or vert collision
                                                float point_t = 0.0f;
                                                maths::vec3 point_normal;
                                                if( rimCollide( asphere, direction, triangle, point_t, point_normal, reject ) )
                                                {
                                                    if( ( point_t >= min_t ) && ( point_t <= max_t ) )
                                                    {
                                                        if( ( !collision ) || ( collision_t > point_t ) )
                                                        {
                                                            collision = true;
                                                            collision_t = point_t;
                                                            collision_normal = point_normal;
                                                        }
                                                    }
                                                }
                                            }
                                            ++triangle_index;
                                        }
                                    }
                                    else
                                    {   //  the surface has no convex edges or verts so the test against the surface plane is sufficient
                                        collision = true;
                                        collision_t = plane_t;
                                        collision_normal = surface.nplane.t_vec3();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    t = collision_t;
    normal = collision_normal;
    return( collision );
}

bool RoomCollider::excludeSphere( const maths::sphere& asphere, maths::vec3& reposition ) const
{
    bool collision = false;
    maths::sphere excluded = asphere;
    if( valid() )
    {
        if( excludeSphereFaces( excluded, excluded.t_vec3() ) )
        {
            collision = true;
        }
        if( excludeSphereEdges( excluded, excluded.t_vec3() ) )
        {
            collision = true;
        }
        if( excludeSphereVerts( excluded, excluded.t_vec3() ) )
        {
            collision = true;
        }
    }
    reposition = excluded.t_vec3();
    return( collision );
}

bool RoomCollider::excludeSphere( const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const
{
    bool collision = false;
    maths::sphere excluded = asphere;
    if( valid() )
    {
        if( excludeSphereFaces( excluded, excluded.t_vec3(), reject ) )
        {
            collision = true;
        }
        if( excludeSphereEdges( excluded, excluded.t_vec3(), reject ) )
        {
            collision = true;
        }
        if( excludeSphereVerts( excluded, excluded.t_vec3(), reject ) )
        {
            collision = true;
        }
    }
    reposition = excluded.t_vec3();
    return( collision );
}

bool RoomCollider::excludeSphereFaces( const maths::sphere& asphere, maths::vec3& reposition ) const
{
    bool collision = false;
    maths::sphere excluded = asphere;
    if( valid() )
    {
        for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
        {
            const Surface& surface = m_surfaces[ surface_index ];
            float distance = excluded.t_vec3().Dot( surface.nplane );
            if( ( ( distance + excluded.r ) >= 0.0f ) && ( ( distance - excluded.r ) <= 0.0f ) )
            {   //  the sphere is spanning the surface plane
                bool contact = true;
                if( surface.flags )
                {   //  the surface has some convex edges so we need to run basic geometry tests
                    contact = false;
                    uint triangle_index = surface.triangles_index;
                    for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                    {   //  check for triangle face penetration
                        const Triangle& triangle = m_triangles[ triangle_index ];
                        if( containsPoint( triangle, excluded.t_vec3() ) )
                        {   //  inside the triangle
                            contact = true;
                            break;
                        }
                        ++triangle_index;
                    }
                }
                if( contact )
                {
                    float s = ( excluded.r - distance );
                    collision = true;
                    excluded.x += ( surface.nplane.x * s );
                    excluded.y += ( surface.nplane.y * s );
                    excluded.z += ( surface.nplane.z * s );
                }
            }
        }
    }
    reposition = excluded.t_vec3();
    return( collision );
}

bool RoomCollider::excludeSphereFaces( const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const
{
    bool collision = false;
    maths::sphere excluded = asphere;
    if( valid() )
    {
        for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
        {
            const Surface& surface = m_surfaces[ surface_index ];
            float distance = excluded.t_vec3().Dot( surface.nplane );
            if( ( ( distance + excluded.r ) >= 0.0f ) && ( ( distance - excluded.r ) <= 0.0f ) )
            {   //  the sphere is spanning the surface plane
                maths::vec3 point;
                point.x = ( excluded.x - ( surface.nplane.x * distance ) );
                point.y = ( excluded.y - ( surface.nplane.y * distance ) );
                point.z = ( excluded.z - ( surface.nplane.z * distance ) );
                if( point.Dot( reject ) > 0.0f )
                {   //  the surface is not rejected
                    bool contact = true;
                    if( surface.flags )
                    {   //  the surface has some convex edges so we need to run basic geometry tests
                        contact = false;
                        uint triangle_index = surface.triangles_index;
                        for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                        {   //  check for triangle face penetration
                            const Triangle& triangle = m_triangles[ triangle_index ];
                            if( containsPoint( triangle, excluded.t_vec3() ) )
                            {   //  inside the triangle
                                contact = true;
                                break;
                            }
                            ++triangle_index;
                        }
                    }
                    if( contact )
                    {
                        float s = ( excluded.r - distance );
                        collision = true;
                        excluded.x += ( surface.nplane.x * s );
                        excluded.y += ( surface.nplane.y * s );
                        excluded.z += ( surface.nplane.z * s );
                    }
                }
            }
        }
    }
    reposition = excluded.t_vec3();
    return( collision );
}

bool RoomCollider::excludeSphereEdges( const maths::sphere& asphere, maths::vec3& reposition ) const
{
    bool collision = false;
    maths::sphere excluded = asphere;
    if( valid() )
    {
        for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
        {
            const Surface& surface = m_surfaces[ surface_index ];
            if( surface.flags & ( EEdge01 | EEdge12 | EEdge20 ) )
            {
                float distance = excluded.t_vec3().Dot( surface.nplane );
                if( ( ( distance + excluded.r ) >= 0.0f ) && ( ( distance - excluded.r ) <= 0.0f ) )
                {   //  the sphere is spanning the surface plane
                    uint triangle_index = surface.triangles_index;
                    for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                    {   //  check for triangle face penetration
                        const Triangle& triangle = m_triangles[ triangle_index ];
                        if( rimExcludeEdges( excluded, triangle, excluded.t_vec3() ) )
                        {
                            collision = true;
                        }
                        ++triangle_index;
                    }
                }
            }
        }
    }
    reposition = excluded.t_vec3();
    return( collision );
}

bool RoomCollider::excludeSphereEdges( const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const
{
    bool collision = false;
    maths::sphere excluded = asphere;
    if( valid() )
    {
        for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
        {
            const Surface& surface = m_surfaces[ surface_index ];
            if( surface.flags & ( EEdge01 | EEdge12 | EEdge20 ) )
            {
                float distance = excluded.t_vec3().Dot( surface.nplane );
                if( ( ( distance + excluded.r ) >= 0.0f ) && ( ( distance - excluded.r ) <= 0.0f ) )
                {   //  the sphere is spanning the surface plane
                    maths::vec3 point;
                    point.x = ( excluded.x - ( surface.nplane.x * distance ) );
                    point.y = ( excluded.y - ( surface.nplane.y * distance ) );
                    point.z = ( excluded.z - ( surface.nplane.z * distance ) );
                    if( point.Dot( reject ) > 0.0f )
                    {   //  the surface is not rejected
                        uint triangle_index = surface.triangles_index;
                        for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                        {   //  check for triangle face penetration
                            const Triangle& triangle = m_triangles[ triangle_index ];
                            if( rimExcludeEdges( excluded, triangle, excluded.t_vec3(), reject ) )
                            {
                                collision = true;
                            }
                            ++triangle_index;
                        }
                    }
                }
            }
        }
    }
    reposition = excluded.t_vec3();
    return( collision );
}

bool RoomCollider::excludeSphereVerts( const maths::sphere& asphere, maths::vec3& reposition ) const
{
    bool collision = false;
    maths::sphere excluded = asphere;
    if( valid() )
    {
        for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
        {
            const Surface& surface = m_surfaces[ surface_index ];
            if( surface.flags & ( EVert0 | EVert1 | EVert2 ) )
            {
                float distance = excluded.t_vec3().Dot( surface.nplane );
                if( ( ( distance + excluded.r ) >= 0.0f ) && ( ( distance - excluded.r ) <= 0.0f ) )
                {   //  the sphere is spanning the surface plane
                    uint triangle_index = surface.triangles_index;
                    for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                    {   //  check for triangle face penetration
                        const Triangle& triangle = m_triangles[ triangle_index ];
                        if( rimExcludeVerts( excluded, triangle, excluded.t_vec3() ) )
                        {
                            collision = true;
                        }
                        ++triangle_index;
                    }
                }
            }
        }
    }
    reposition = excluded.t_vec3();
    return( collision );
}

bool RoomCollider::excludeSphereVerts( const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const
{
    bool collision = false;
    maths::sphere excluded = asphere;
    if( valid() )
    {
        for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
        {
            const Surface& surface = m_surfaces[ surface_index ];
            if( surface.flags & ( EVert0 | EVert1 | EVert2 ) )
            {
                float distance = excluded.t_vec3().Dot( surface.nplane );
                if( ( ( distance + excluded.r ) >= 0.0f ) && ( ( distance - excluded.r ) <= 0.0f ) )
                {   //  the sphere is spanning the surface plane
                    maths::vec3 point;
                    point.x = ( excluded.x - ( surface.nplane.x * distance ) );
                    point.y = ( excluded.y - ( surface.nplane.y * distance ) );
                    point.z = ( excluded.z - ( surface.nplane.z * distance ) );
                    if( point.Dot( reject ) > 0.0f )
                    {   //  the surface is not rejected
                        uint triangle_index = surface.triangles_index;
                        for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
                        {   //  check for triangle face penetration
                            const Triangle& triangle = m_triangles[ triangle_index ];
                            if( rimExcludeVerts( excluded, triangle, excluded.t_vec3(), reject ) )
                            {
                                collision = true;
                            }
                            ++triangle_index;
                        }
                    }
                }
            }
        }
    }
    reposition = excluded.t_vec3();
    return( collision );
}

void RoomCollider::buildFlags()
{
    for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
    {   //  clear the surface edge flags
        m_surfaces[ surface_index ].flags = 0;
    }
    for( uint triangle_index = 0; triangle_index < m_triangle_count; ++triangle_index )
    {   //  clear the triangle edge flags
        m_triangles[ triangle_index ].flags = 0;
    }
    for( uint outer_surface_index = 1; outer_surface_index < m_surface_count; ++outer_surface_index )
    {   //  flag the convex edges
        Surface& outer_surface = m_surfaces[ outer_surface_index ];
        uint outer_triangle_index = outer_surface.triangles_index;
        for( uint outer_triangle_count = outer_surface.triangles_count; outer_triangle_count; --outer_triangle_count )
        {
            Triangle& outer_triangle = m_triangles[ outer_triangle_index ];
            uint outer_indices[ 4 ] = { outer_triangle.indices[ 0 ], outer_triangle.indices[ 1 ], outer_triangle.indices[ 2 ], 0 };
            for( uint inner_surface_index = 0; inner_surface_index < outer_surface_index; ++inner_surface_index )
            {
                Surface& inner_surface = m_surfaces[ inner_surface_index ];
                uint inner_triangle_index = inner_surface.triangles_index;
                for( uint inner_triangle_count = inner_surface.triangles_count; inner_triangle_count; --inner_triangle_count )
                {
                    Triangle& inner_triangle = m_triangles[ inner_triangle_index ];
                    uint inner_indices[ 4 ] = { inner_triangle.indices[ 0 ], inner_triangle.indices[ 2 ], inner_triangle.indices[ 1 ], 0 };
                    for( uint outer_shift_count = 3; outer_shift_count; --outer_shift_count )
                    {
                        for( uint inner_shift_count = 3; inner_shift_count; --inner_shift_count )
                        {
                            if( ( inner_indices[ 0 ] == outer_indices[ 0 ] ) && ( inner_indices[ 1 ] == outer_indices[ 1 ] ) )
                            {   //  found a shared edge with the correct winding
                                float inner_offset = m_vertices[ inner_indices[ 2 ] ].Dot( outer_surface.nplane );
                                float outer_offset = m_vertices[ outer_indices[ 2 ] ].Dot( inner_surface.nplane );
                                float offset = ( ( inner_offset < outer_offset ) ? inner_offset : outer_offset );
                                if( offset < 0.0f )
                                {   //  found a convex edge, so set the flags
                                    inner_triangle.flags |= ( 1 << ( inner_shift_count + 2 ) );
                                    outer_triangle.flags |= ( 1 << ( 6 - outer_shift_count ) );
                                }
                            }
                            inner_indices[ 3 ] = inner_indices[ 0 ];
                            inner_indices[ 0 ] = inner_indices[ 1 ];
                            inner_indices[ 1 ] = inner_indices[ 2 ];
                            inner_indices[ 2 ] = inner_indices[ 3 ];
                        }
                        outer_indices[ 3 ] = outer_indices[ 0 ];
                        outer_indices[ 0 ] = outer_indices[ 1 ];
                        outer_indices[ 1 ] = outer_indices[ 2 ];
                        outer_indices[ 2 ] = outer_indices[ 3 ];
                    }
                    ++inner_triangle_index;
                }
            }
            ++outer_triangle_index;
        }
    }
    for( uint outer_surface_index = 0; outer_surface_index < m_surface_count; ++outer_surface_index )
    {   //  flag the unshared edges
        Surface& outer_surface = m_surfaces[ outer_surface_index ];
        uint outer_triangle_index = outer_surface.triangles_index;
        for( uint outer_triangle_count = outer_surface.triangles_count; outer_triangle_count; --outer_triangle_count )
        {
            Triangle& outer_triangle = m_triangles[ outer_triangle_index ];
            uint outer_indices[ 4 ] = { outer_triangle.indices[ 0 ], outer_triangle.indices[ 1 ], outer_triangle.indices[ 2 ], 0 };
            for( uint outer_shift_count = 3; outer_shift_count; --outer_shift_count )
            {
                bool found = true;
                for( uint inner_surface_index = 0; inner_surface_index < m_surface_count; ++inner_surface_index )
                {
                    Surface& inner_surface = m_surfaces[ ( inner_surface_index + outer_surface_index ) % m_surface_count ];
                    uint inner_triangle_index = inner_surface.triangles_index;
                    for( uint inner_triangle_count = inner_surface.triangles_count; inner_triangle_count; --inner_triangle_count )
                    {
                        if( inner_triangle_index != outer_triangle_index )
                        {
                            Triangle& inner_triangle = m_triangles[ inner_triangle_index ];
                            uint inner_indices[ 4 ] = { inner_triangle.indices[ 0 ], inner_triangle.indices[ 2 ], inner_triangle.indices[ 1 ], 0 };
                            for( uint inner_shift_count = 3; inner_shift_count; --inner_shift_count )
                            {
                                if( ( inner_indices[ 0 ] == outer_indices[ 0 ] ) && ( inner_indices[ 1 ] == outer_indices[ 1 ] ) )
                                {   //  found a shared edge with the correct winding
                                    found = false;
                                    break;
                                }
                                inner_indices[ 3 ] = inner_indices[ 0 ];
                                inner_indices[ 0 ] = inner_indices[ 1 ];
                                inner_indices[ 1 ] = inner_indices[ 2 ];
                                inner_indices[ 2 ] = inner_indices[ 3 ];
                            }
                            if( !found )
                            {
                                break;
                            }
                        }
                        ++inner_triangle_index;
                    }
                    if( !found )
                    {
                        break;
                    }
                }
                if( found )
                {   //  found an unshared edge
                    outer_triangle.flags |= ( 1 << ( 6 - outer_shift_count ) );
                }
                outer_indices[ 3 ] = outer_indices[ 0 ];
                outer_indices[ 0 ] = outer_indices[ 1 ];
                outer_indices[ 1 ] = outer_indices[ 2 ];
                outer_indices[ 2 ] = outer_indices[ 3 ];
            }
            ++outer_triangle_index;
        }
    }
    for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
    {   //  flag fragment surfaces
        Surface& surface = m_surfaces[ surface_index ];
        float tolerance = 0.0f;
        uint triangle_index = surface.triangles_index;
        for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
        {
            Triangle& triangle = m_triangles[ triangle_index ];
            for( uint index_index = 0; index_index < 3; ++index_index )
            {
                float check = fabsf( m_vertices[ triangle.indices[ index_index ] ].Dot( surface.nplane ) );
                if( tolerance < check )
                {
                    tolerance = check;
                }
            }
        }
        tolerance += tolerance;
        for( uint vertex_index = 0; vertex_index < m_vertex_count; ++vertex_index )
        {
            if( ( m_vertices[ vertex_index ].Dot( surface.nplane ) + tolerance ) < 0.0f )
            {
                surface.flags |= EPoly;
                break;
            }
        }
    }
    for( uint vertex_index = 0; vertex_index < m_vertex_count; ++vertex_index )
    {   //  flag the convex verts
        uint count = 0;
        for( uint triangle_index = 0; triangle_index < m_triangle_count; ++triangle_index )
        {
            Triangle& triangle = m_triangles[ triangle_index ];
            if( triangle.flags )
            {
                uint flags = ( ( ( triangle.flags >> 3 ) & 0x0007 ) | ( ( triangle.flags >> 2 ) & 0x0006 ) | ( ( triangle.flags >> 5 ) &0x0001 ) );
                for( uint index_index = 0; flags; ++index_index )
                {
                    if( flags & 1 )
                    {
                        if( triangle.indices[ index_index ] == vertex_index )
                        {
                            ++count;
                        }
                    }
                    flags >>= 1;
                }
            }
        }
        if( count > 2 )
        {
            for( uint triangle_index = 0; triangle_index < m_triangle_count; ++triangle_index )
            {
                Triangle& triangle = m_triangles[ triangle_index ];
                if( triangle.flags )
                {
                    for( uint index_index = 0; index_index < 3; ++index_index )
                    {
                        if( triangle.indices[ index_index ] == vertex_index )
                        {
                            triangle.flags |= ( 1 << index_index );
                        }
                    }
                }
            }
        }
    }
    for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
    {   //  accumulate the triangle edge flags to construct the surface edge flags
        Surface& surface = m_surfaces[ surface_index ];
        uint triangle_index = surface.triangles_index;
        for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
        {
            surface.flags |= m_triangles[ triangle_index ].flags;
            ++triangle_index;
        }
    }
}

void RoomCollider::buildBounds()
{
    m_sphereBounds.x = m_sphereBounds.y = m_sphereBounds.z = m_sphereBounds.r = 0.0f;
    m_aabbBounds.min.x = m_aabbBounds.min.y = m_aabbBounds.min.z = 0.0f;
    m_aabbBounds.max.x = m_aabbBounds.max.y = m_aabbBounds.max.z = 0.0f;
    if( m_vertex_count )
    {
        m_aabbBounds.Set( m_vertices[ 0 ] );
        for( uint vertex_index = 1; vertex_index < m_vertex_count; ++vertex_index )
        {
            m_aabbBounds.Merge( m_vertices[ vertex_index ] );
        }
        m_sphereBounds.x = ( ( m_aabbBounds.min.x + m_aabbBounds.max.x ) * 0.5f );
        m_sphereBounds.y = ( ( m_aabbBounds.min.y + m_aabbBounds.max.y ) * 0.5f );
        m_sphereBounds.z = ( ( m_aabbBounds.min.z + m_aabbBounds.max.z ) * 0.5f );
        for( uint vertex_index = 0; vertex_index < m_vertex_count; ++vertex_index )
        {
            maths::vec3 delta;
            delta.SetSub( m_vertices[ vertex_index ], m_sphereBounds.t_vec3() );
            float r = delta.Length();
            if( m_sphereBounds.r < r ) m_sphereBounds.r = r;
        }
    }
}

void RoomCollider::buildPrecalc()
{
    for( uint surface_index = 0; surface_index < m_surface_count; ++surface_index )
    {
        Surface& surface = m_surfaces[ surface_index ];
        surface.nplane.SetNormalized( m_planes[ surface.plane_index ] );
        uint triangle_index = surface.triangles_index;
        for( uint triangle_count = surface.triangles_count; triangle_count; --triangle_count )
        {
            Triangle& triangle = m_triangles[ triangle_index ];
            for( uint edge_index = 0; edge_index < 3; ++edge_index )
            {
                Edge& edge = triangle.edges[ edge_index ];
                edge.cpoint.t_vec3() = m_vertices[ triangle.indices[ edge_index ] ];
                edge.cdelta.t_vec3().SetSub( m_vertices[ triangle.indices[ ( edge_index + 1 ) % 3 ] ], edge.cpoint.t_vec3() );
                edge.cdelta.w = edge.cdelta.t_vec3().Length();
                edge.cpoint.w = ( edge.cdelta.w * 0.5f );
                edge.tplane.t_vec3().SetNormalized( edge.cdelta.t_vec3() );
                edge.tplane.w = ( 0.0f - edge.tplane.t_vec3().Dot( edge.cpoint.t_vec3() ) - edge.cpoint.w );
                edge.nplane.t_vec3().SetCross( edge.cdelta.t_vec3(), surface.nplane.t_vec3() );
                edge.nplane.t_vec3().Normalize();
                edge.nplane.w = ( 0.0f - edge.nplane.t_vec3().Dot( edge.cpoint.t_vec3() ) );
            }
            ++triangle_index;
        }
    }
}

bool RoomCollider::containsPoint( const Triangle& triangle, const maths::vec3& point ) const
{
    float distances[ 3 ];
    distances[ 0 ] = point.Dot( triangle.edges[ 0 ].nplane );
    if( distances[ 0 ] >= m_tolerance )
    {   //  trivially outside the triangle at edge 1
        return( false );
    }
    distances[ 1 ] = point.Dot( triangle.edges[ 1 ].nplane );
    if( distances[ 1 ] >= m_tolerance )
    {   //  trivially outside the triangle at edge 2
        return( false );
    }
    distances[ 2 ] = point.Dot( triangle.edges[ 2 ].nplane );
    if( distances[ 2 ] >= m_tolerance )
    {   //  trivially outside the triangle at edge 3
        return( false );
    }
    if( ( distances[ 0 ] <= 0.0f ) && ( distances[ 1 ] <= 0.0f ) && ( distances[ 2 ] <= 0.0f ) )
    {   //  trivially inside the triangle
        return( true );
    }
    float tangents[ 3 ];
    tangents[ 0 ] = ( fabsf( point.Dot( triangle.edges[ 0 ].tplane ) ) - triangle.edges[ 2 ].cpoint.w );
    if( ( distances[ 0 ] >= 0.0f ) && ( tangents[ 0 ] <= m_tolerance ) )
    {   //  inside edge 1 tolerance zone
        return( true );
    }
    tangents[ 1 ] = ( fabsf( point.Dot( triangle.edges[ 1 ].tplane ) ) - triangle.edges[ 2 ].cpoint.w );
    if( ( distances[ 1 ] >= 0.0f ) && ( tangents[ 1 ] <= m_tolerance ) )
    {   //  inside edge 2 tolerance
        return( true );
    }
    tangents[ 2 ] = ( fabsf( point.Dot( triangle.edges[ 2 ].tplane ) ) - triangle.edges[ 2 ].cpoint.w );
    if( ( distances[ 2 ] >= 0.0f ) && ( tangents[ 2 ] <= m_tolerance ) )
    {   //  inside edge 3 tolerance
        return( true );
    }
    return( false );
}

bool RoomCollider::rimCollide( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal ) const
{
    return( rimCollideEdges( asphere, direction, triangle, t, normal ) || rimCollideVerts( asphere, direction, triangle, t, normal ) );
}

bool RoomCollider::rimCollide( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    return( rimCollideEdges( asphere, direction, triangle, t, normal, reject ) || rimCollideVerts( asphere, direction, triangle, t, normal, reject ) );
}

bool RoomCollider::rimCollideEdges( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal ) const
{
    if( triangle.flags & EEdge01 )
    {
        if( edgeCollide( asphere, direction, triangle.edges[ 0 ], t, normal ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EEdge12 )
    {
        if( edgeCollide( asphere, direction, triangle.edges[ 1 ], t, normal ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EEdge20 )
    {
        if( edgeCollide( asphere, direction, triangle.edges[ 2 ], t, normal ) )
        {
            return( true );
        }
    }
    return( false );
}

bool RoomCollider::rimCollideEdges( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    if( triangle.flags & EEdge01 )
    {
        if( edgeCollide( asphere, direction, triangle.edges[ 0 ], t, normal, reject ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EEdge12 )
    {
        if( edgeCollide( asphere, direction, triangle.edges[ 1 ], t, normal, reject ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EEdge20 )
    {
        if( edgeCollide( asphere, direction, triangle.edges[ 2 ], t, normal, reject ) )
        {
            return( true );
        }
    }
    return( false );
}

bool RoomCollider::rimCollideVerts( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal ) const
{
    if( triangle.flags & EVert0 )
    {
        if( pointCollide( asphere, direction, triangle.edges[ 0 ].cpoint.t_vec3(), t, normal ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EVert1 )
    {
        if( pointCollide( asphere, direction, triangle.edges[ 1 ].cpoint.t_vec3(), t, normal ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EVert2 )
    {
        if( pointCollide( asphere, direction, triangle.edges[ 2 ].cpoint.t_vec3(), t, normal ) )
        {
            return( true );
        }
    }
    return( false );
}

bool RoomCollider::rimCollideVerts( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal, const maths::plane& reject ) const
{
    if( triangle.flags & EVert0 )
    {
        if( pointCollide( asphere, direction, triangle.edges[ 0 ].cpoint.t_vec3(), t, normal, reject ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EVert1 )
    {
        if( pointCollide( asphere, direction, triangle.edges[ 1 ].cpoint.t_vec3(), t, normal, reject ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EVert2 )
    {
        if( pointCollide( asphere, direction, triangle.edges[ 2 ].cpoint.t_vec3(), t, normal, reject ) )
        {
            return( true );
        }
    }
    return( false );
}

bool RoomCollider::rimExclude( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition ) const
{
    return( rimExcludeEdges( asphere, triangle, reposition ) || rimExcludeVerts( asphere, triangle, reposition ) );
}

bool RoomCollider::rimExclude( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition, const maths::plane& reject ) const
{
    return( rimExcludeEdges( asphere, triangle, reposition, reject ) || rimExcludeVerts( asphere, triangle, reposition, reject ) );
}

bool RoomCollider::rimExcludeEdges( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition ) const
{
    if( triangle.flags & EEdge01 )
    {
        if( edgeExclude( asphere, triangle.edges[ 0 ], reposition ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EEdge12 )
    {
        if( edgeExclude( asphere, triangle.edges[ 1 ], reposition ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EEdge20 )
    {
        if( edgeExclude( asphere, triangle.edges[ 2 ], reposition ) )
        {
            return( true );
        }
    }
    return( false );
}

bool RoomCollider::rimExcludeEdges( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition, const maths::plane& reject ) const
{
    if( triangle.flags & EEdge01 )
    {
        if( edgeExclude( asphere, triangle.edges[ 0 ], reposition, reject ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EEdge12 )
    {
        if( edgeExclude( asphere, triangle.edges[ 1 ], reposition, reject ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EEdge20 )
    {
        if( edgeExclude( asphere, triangle.edges[ 2 ], reposition, reject ) )
        {
            return( true );
        }
    }
    return( false );
}

bool RoomCollider::rimExcludeVerts( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition ) const
{
    if( triangle.flags & EVert0 )
    {
        if( pointExclude( asphere, triangle.edges[ 0 ].cpoint.t_vec3(), reposition ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EVert1 )
    {
        if( pointExclude( asphere, triangle.edges[ 1 ].cpoint.t_vec3(), reposition ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EVert2 )
    {
        if( pointExclude( asphere, triangle.edges[ 2 ].cpoint.t_vec3(), reposition ) )
        {
            return( true );
        }
    }
    return( false );
}

bool RoomCollider::rimExcludeVerts( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition, const maths::plane& reject ) const
{
    if( triangle.flags & EVert0 )
    {
        if( pointExclude( asphere, triangle.edges[ 0 ].cpoint.t_vec3(), reposition, reject ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EVert1 )
    {
        if( pointExclude( asphere, triangle.edges[ 1 ].cpoint.t_vec3(), reposition, reject ) )
        {
            return( true );
        }
    }
    if( triangle.flags & EVert2 )
    {
        if( pointExclude( asphere, triangle.edges[ 2 ].cpoint.t_vec3(), reposition, reject ) )
        {
            return( true );
        }
    }
    return( false );
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

