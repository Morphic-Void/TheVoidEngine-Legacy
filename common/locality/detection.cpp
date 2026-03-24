
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   detection.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Collision detection structures and functions
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

#include "detection.h"
#include "libs/system/debug/asserts.h"
#include "libs/maths/consts.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin detection namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detection
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Segment to plane intersection function
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  segment->plane
bool intersect( const Segment& aSegment, const Plane& aPlane, maths::vec3& intersection )
{
    float po = aSegment.offset.t_vec3().Dot( aPlane.t_vec3() );
    if( fabsf( po ) >= maths::consts::FLOAT_MIN_NORM )
    {
        float pc = aSegment.center.t_vec3().Dot( aPlane );
        float t = ( ( 0.0f - pc ) / po );
        if( fabsf( t ) <= aSegment.offset.w )
        {
            intersection.x = ( aSegment.center.x + ( aSegment.offset.x * t ) );
            intersection.y = ( aSegment.center.y + ( aSegment.offset.y * t ) );
            intersection.z = ( aSegment.center.z + ( aSegment.offset.z * t ) );
            return( true );
        }
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Plane relationship functions
////
////    return value meanings:
////
////        -1  :   behind the plane
////        0   :   astride the plane
////        1   :   in front of the plane
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  point->plane
int relate( const Point& aPoint, const Plane& aPlane )
{
    return( ( aPoint.Dot( aPlane ) < 0.0f ) ? -1 : 1 );
}

//  segment->plane
int relate( const Segment& aSegment, const Plane& aPlane )
{
    float c = aSegment.center.t_vec3().Dot( aPlane );
    float o = fabsf( aSegment.offset.t_vec3().Dot( aPlane.t_vec3() ) );
    float d = ( o * aSegment.offset.w );
    int relationship = 0;
    if( ( c + d ) > 0.0f ) ++relationship;
    if( ( c - d ) < 0.0f ) --relationship;
    return( relationship );
}

//  sphere->plane
int relate( const Sphere& aSphere, const Plane& aPlane )
{
    float c = aSphere.t_vec3().Dot( aPlane );
    float d = aSphere.r;
    int relationship = 0;
    if( ( c + d ) > 0.0f ) ++relationship;
    if( ( c - d ) < 0.0f ) --relationship;
    return( relationship );
}

//  capsule->plane
int relate( const Capsule& aCapsule, const Plane& aPlane )
{
    float c = aCapsule.center.t_vec3().Dot( aPlane );
    float o = fabsf( aCapsule.offset.t_vec3().Dot( aPlane.t_vec3() ) );
    float d = ( ( o * aCapsule.offset.w ) + aCapsule.center.r );
    int relationship = 0;
    if( ( c + d ) > 0.0f ) ++relationship;
    if( ( c - d ) < 0.0f ) --relationship;
    return( relationship );
}

//  cylinder->plane
int relate( const Cylinder& aCylinder, const Plane& aPlane )
{
    float c = aCylinder.center.t_vec3().Dot( aPlane );
    float o = fabsf( aCylinder.offset.t_vec3().Dot( aPlane.t_vec3() ) );
    float d = ( ( o * aCylinder.offset.w ) + ( aCylinder.center.r * sqrtf( fabsf( 1.0f - ( o * o ) ) ) ) );
    int relationship = 0;
    if( ( c + d ) > 0.0f ) ++relationship;
    if( ( c - d ) < 0.0f ) --relationship;
    return( relationship );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Plane collision detection functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  point->plane
bool collide( const Point& object, const Plane& target, const maths::vec3& direction, float& t )
{
    float pd = ( 0.0f - direction.Dot( target.t_vec3() ) );
    if( pd >= maths::consts::FLOAT_MIN_NORM )
    {
        t = ( object.Dot( target ) / pd );
        return( true );
    }
    return( false );
}

//  segment->plane
bool collide( const Segment& object, const Plane& target, const maths::vec3& direction, float& t )
{
    float pd = ( 0.0f - direction.Dot( target.t_vec3() ) );
    if( pd >= maths::consts::FLOAT_MIN_NORM )
    {
        float po = fabsf( object.offset.t_vec3().Dot( target.t_vec3() ) );
        t = ( ( object.center.t_vec3().Dot( target ) - ( po * object.offset.w ) ) / pd );
        return( true );
    }
    return( false );
}

//  sphere->plane
bool collide( const Sphere& object, const Plane& target, const maths::vec3& direction, float& t )
{
    float pd = ( 0.0f - direction.Dot( target.t_vec3() ) );
    if( pd >= maths::consts::FLOAT_MIN_NORM )
    {
        t = ( ( object.t_vec3().Dot( target ) - object.r ) / pd );
        return( true );
    }
    return( false );
}

//  capsule->plane
bool collide( const Capsule& object, const Plane& target, const maths::vec3& direction, float& t )
{
    float pd = ( 0.0f - direction.Dot( target.t_vec3() ) );
    if( pd >= maths::consts::FLOAT_MIN_NORM )
    {
        float po = fabsf( object.offset.t_vec3().Dot( target.t_vec3() ) );
        t = ( ( object.center.t_vec3().Dot( target ) - object.center.r - ( po * object.offset.w ) ) / pd );
        return( true );
    }
    return( false );
}

//  cylinder->plane
bool collide( const Cylinder& object, const Plane& target, const maths::vec3& direction, float& t )
{
    float pd = ( 0.0f - direction.Dot( target.t_vec3() ) );
    if( pd >= maths::consts::FLOAT_MIN_NORM )
    {
        float po = fabsf( object.offset.t_vec3().Dot( target.t_vec3() ) );
        t = ( ( object.center.t_vec3().Dot( target ) - ( object.center.r * sqrtf( fabsf( 1.0f - ( po * po ) ) ) ) - ( po * object.offset.w ) ) / pd );
        return( true );
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Point, sphere and capsule collision helper functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  point->point with radius
bool collide( const Point& object, const Point& target, const float radius, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    maths::vec3 separation;
    separation.x = ( target.x - object.x );
    separation.y = ( target.y - object.y );
    separation.z = ( target.z - object.z );
    float rr = ( radius * radius );
    float ss = separation.SqrLen();
    float sd = separation.Dot( direction );
    float rd = ( ( sd * sd ) + ( rr - ss ) );   //  if direction not a unit vector: float rd = ( ( sd * sd ) + ( ( rr - ss ) * dd ) );
    if( rd > 0.0f )
    {
        t = ( sd - sqrtf( rd ) );               //  if direction not a unit vector: t = ( ( sd - sqrtf( rd ) ) / dd ) );  
        normal.x = ( ( direction.x * t ) - separation.x );
        normal.y = ( ( direction.y * t ) - separation.y );
        normal.z = ( ( direction.z * t ) - separation.z );
        normal.Normalize();
        return( true );
    }
    return( false );
}

//  point->segment with radius
bool collide( const Point& object, const Segment& target, const float radius, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    maths::vec3 separation;
    separation.x = ( target.center.x - object.x );
    separation.y = ( target.center.y - object.y );
    separation.z = ( target.center.z - object.z );
    float rr = ( radius * radius );
    maths::vec3 uaxis;
    uaxis.SetCross( direction, target.offset.t_vec3() );
    float od = direction.Dot( target.offset.t_vec3() );
    float ot = ( ( od < 0.0f ) ? 1.0f : -1.0f );
    float uu = uaxis.SqrLen();
    float uo = ( uu * target.offset.w );
    if( uo >= maths::consts::FLOAT_MIN_NORM )
    {   //  the direction is not along the axis of the segment
        float su = uaxis.Dot( separation );
        float ru = ( ( rr * uu ) - ( su * su ) );
        if( ru <= 0.0f ) return( false );
        maths::vec3 vaxis;
        vaxis.SetCross( direction, uaxis );
        ot = ( ( vaxis.Dot( separation ) - ( od * sqrtf( ru ) ) ) / uo );
        if( ot < -1.0f ) ot = -1.0f;
        if( ot > 1.0f ) ot = 1.0f;
    }
    ot *= target.offset.w;
    separation.x += ( target.offset.x * ot );
    separation.y += ( target.offset.y * ot );
    separation.z += ( target.offset.z * ot );
    float ss = separation.SqrLen();
    float sd = separation.Dot( direction );
    float rd = ( ( sd * sd ) + ( rr - ss ) );
    if( rd > 0.0f )
    {
        t = ( sd - sqrtf( rd ) );
        normal.x = ( ( direction.x * t ) - separation.x );
        normal.y = ( ( direction.y * t ) - separation.y );
        normal.z = ( ( direction.z * t ) - separation.z );
        normal.Normalize();
        return( true );
    }
    return( false );
}

//  segment->point with radius
bool collide( const Segment& object, const Point& target, const float radius, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    maths::vec3 separation;
    separation.x = ( target.x - object.center.x );
    separation.y = ( target.y - object.center.y );
    separation.z = ( target.z - object.center.z );
    float rr = ( radius * radius );
    maths::vec3 uaxis;
    uaxis.SetCross( direction, object.offset.t_vec3() );
    float od = direction.Dot( object.offset.t_vec3() );
    float ot = ( ( od < 0.0f ) ? 1.0f : -1.0f );
    float uu = uaxis.SqrLen();
    float uo = ( uu * object.offset.w );
    if( uo >= maths::consts::FLOAT_MIN_NORM )
    {   //  the direction is not along the axis of the segment
        float su = uaxis.Dot( separation );
        float ru = ( ( rr * uu ) - ( su * su ) );
        if( ru <= 0.0f ) return( false );
        maths::vec3 vaxis;
        vaxis.SetCross( direction, uaxis );
        ot = ( ( vaxis.Dot( separation ) - ( od * sqrtf( ru ) ) ) / uo );
        if( ot < -1.0f ) ot = -1.0f;
        if( ot > 1.0f ) ot = 1.0f;
    }
    ot *= object.offset.w;
    separation.x += ( object.offset.x * ot );
    separation.y += ( object.offset.y * ot );
    separation.z += ( object.offset.z * ot );
    float ss = separation.SqrLen();
    float sd = separation.Dot( direction );
    float rd = ( ( sd * sd ) + ( rr - ss ) );
    if( rd > 0.0f )
    {
        t = ( sd - sqrtf( rd ) );
        normal.x = ( ( direction.x * t ) - separation.x );
        normal.y = ( ( direction.y * t ) - separation.y );
        normal.z = ( ( direction.z * t ) - separation.z );
        normal.Normalize();
        return( true );
    }
    return( false );
}

//  segment->segment with radius
bool collide( const Segment& object, const Segment& target, const float radius, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    maths::vec3 naxis;
    naxis.SetCross( object.offset.t_vec3(), target.offset.t_vec3() );
    float nn = naxis.SqrLen();
    if( nn >= maths::consts::FLOAT_MIN_NORM )
    {   //  the segments are not parallel (so we will be able to calculate a collision plane normal and we can treat this as a point to parallelogram collision)
        float t1, t2;
        Point point1, point2;
        maths::vec3 separation;
        separation.x = ( target.center.x - object.center.x );
        separation.y = ( target.center.y - object.center.y );
        separation.z = ( target.center.z - object.center.z );
        float uv = object.offset.t_vec3().Dot( target.offset.t_vec3() );
        naxis.Mul( 1.0f / sqrtf( nn ) );
        float nd = naxis.Dot( direction );
        float na = fabsf( nd );
        if( na >= maths::consts::FLOAT_MIN_NORM )
        {   //  the segments are not in the same plane relative to the direction (and by definition are not parallel to each other)
            t = ( ( naxis.Dot( separation ) / nd ) - ( radius / na ) );
            separation.x -= ( direction.x * t );
            separation.y -= ( direction.y * t );
            separation.z -= ( direction.z * t );
            float su = separation.Dot( object.offset.t_vec3() );
            float sv = separation.Dot( target.offset.t_vec3() );
            float du = ( su - ( sv * uv ) );
            float dv = ( sv - ( su * uv ) );
            float lu = ( nn * object.offset.w );
            float lv = ( nn * target.offset.w );
            uint flags = 0;
            if( fabsf( du ) > lu ) flags |= 1;
            if( fabsf( dv ) > lv ) flags |= 2;
            if( flags & 1 )
            {
                t1 = ( ( du < 0.0f ) ? ( 0.0f - object.offset.w ) : object.offset.w );
                point1.x = ( object.center.x + ( object.offset.x * t1 ) );
                point1.y = ( object.center.y + ( object.offset.y * t1 ) );
                point1.z = ( object.center.z + ( object.offset.z * t1 ) );
            }
            if( flags & 2 )
            {
                t2 = ( ( dv < 0.0f ) ? target.offset.w : ( 0.0f - target.offset.w ) );
                point2.x = ( target.center.x + ( target.offset.x * t2 ) );
                point2.y = ( target.center.y + ( target.offset.y * t2 ) );
                point2.z = ( target.center.z + ( target.offset.z * t2 ) );
            }
            switch( flags )
            {
                case( 3 ):
                {   //  treat as a single point->point with radius collision
                    return( collide( point1, point2, radius, direction, t, normal ) );
                }
                case( 2 ):
                {   //  treat as a single segment->point with radius collision
                    return( collide( object, point2, radius, direction, t, normal ) );
                }
                case( 1 ):
                {   //  treat as a single point->segment with radius collision
                    return( collide( point1, target, radius, direction, t, normal ) );
                }
                default:
                {   //  there is a collision with the parallelogram surface
                    ( nd > 0.0f ) ? normal.SetNegate( naxis ) : normal.Set( naxis );
                    return( true );
                }
            }
        }
        else if( fabsf( naxis.Dot( separation ) ) < radius )
        {   //  edge on to the parallelogram (treated as a point->segment with radius or a segment->point with radius collision)
            float su = separation.Dot( object.offset.t_vec3() );
            float sv = separation.Dot( target.offset.t_vec3() );
            float du = direction.Dot( object.offset.t_vec3() );
            float dv = direction.Dot( target.offset.t_vec3() );
            uint flags = ( ( ( uv * uv ) > 1.0f ) ? 0 : 3 );
	        if( ( dv * uv ) > du ) flags ^= 1;
	        if( ( du * uv ) > dv ) flags ^= 2;
            t1 = ( ( flags & 1 ) ? object.offset.w : ( 0.0f - object.offset.w ) );
            t2 = ( ( flags & 2 ) ? ( 0.0f - target.offset.w ) : target.offset.w );
            if( ( flags - 1 ) & 2 )
            {
                du = ( 0.0f - du );
                dv = ( 0.0f - dv );
            }
            if( ( ( su - t1 + ( uv * t2 ) ) * dv ) < ( ( sv + t2 - ( uv * t1 ) ) * du ) )
            {
                point1.x = ( object.center.x + ( object.offset.x * t1 ) );
                point1.y = ( object.center.y + ( object.offset.y * t1 ) );
                point1.z = ( object.center.z + ( object.offset.z * t1 ) );
                return( collide( point1, target, radius, direction, t, normal ) );
            }
            else
            {
                point2.x = ( target.center.x + ( target.offset.x * t2 ) );
                point2.y = ( target.center.y + ( target.offset.y * t2 ) );
                point2.z = ( target.center.z + ( target.offset.z * t2 ) );
                return( collide( object, point2, radius, direction, t, normal ) );
            }
        }
        return( false );
    }
    else
    {   //  treat as a single point->segment with radius collision
        Point aPoint;
        Segment aSegment;
        aPoint.x = object.center.x;
        aPoint.y = object.center.y;
        aPoint.z = object.center.z;
        aSegment = target;
        aSegment.offset.w += object.offset.w;
        return( collide( aPoint, aSegment, radius, direction, t, normal ) );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Specialised point, sphere and capsule collision functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  point->sphere
bool collide( const Point& object, const Sphere& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( object, *reinterpret_cast< const Point* const >( &target ), target.r, direction, t, normal ) );
}

//  point->capsule
bool collide( const Point& object, const Capsule& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( object, *reinterpret_cast< const Segment* const >( &target ), target.center.r, direction, t, normal ) );
}

//  sphere->sphere
bool collide( const Sphere& object, const Sphere& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( *reinterpret_cast< const Point* const >( &object ), *reinterpret_cast< const Point* const >( &target ), ( object.r + target.r ), direction, t, normal ) );
}

//  sphere->point
bool collide( const Sphere& object, const Point& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( *reinterpret_cast< const Point* const >( &object ), target, object.r, direction, t, normal ) );
}

//  sphere->segment
bool collide( const Sphere& object, const Segment& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( *reinterpret_cast< const Point* const >( &object ), target, object.r, direction, t, normal ) );
}

//  sphere->capsule
bool collide( const Sphere& object, const Capsule& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( *reinterpret_cast< const Point* const >( &object ), *reinterpret_cast< const Segment* const >( &target ), ( object.r + target.center.r ), direction, t, normal ) );
}

//  capsule->sphere
bool collide( const Capsule& object, const Sphere& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( *reinterpret_cast< const Segment* const >( &object ), *reinterpret_cast< const Point* const >( &target ), ( object.center.r + target.r ), direction, t, normal ) );
}

//  capsule->point
bool collide( const Capsule& object, const Point& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( *reinterpret_cast< const Segment* const >( &object ), target, object.center.r, direction, t, normal ) );
}

//  capsule->segment
bool collide( const Capsule& object, const Segment& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( *reinterpret_cast< const Segment* >( &object ), target, object.center.r, direction, t, normal ) );
}

//  capsule->capsule
bool collide( const Capsule& object, const Capsule& target, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collide( *reinterpret_cast< const Segment* >( &object ), *reinterpret_cast< const Segment* >( &target ), ( object.center.r + target.center.r ), direction, t, normal ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Tube collision functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  point->tube
bool collide( const Point& aPoint, const Tube& aTube, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collideSide( aPoint, *reinterpret_cast< const Segment* const >( &aTube ), aTube.center.r, direction, t, normal ) );
}

//  sphere->tube
bool collide( const Sphere& aSphere, const Tube& aTube, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    return( collideSide( *reinterpret_cast< const Point* const >( &aSphere ), *reinterpret_cast< const Segment* const >( &aTube ), ( aSphere.r + aTube.center.r ), direction, t, normal ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Segment and edge collision helper function
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  segment->segment with tolerance
bool collideEdges( const Segment& segment1, const Segment& segment2, const float tolerance, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    maths::vec3 naxis;
    naxis.SetCross( segment1.offset.t_vec3(), segment2.offset.t_vec3() );
    float nn = naxis.SqrLen();
    if( nn >= maths::consts::FLOAT_MIN_NORM )
    {   //  the segments are not parallel (so we will be able to calculate a collision plane normal and we can treat this as a point to parallelogram collision)
        naxis.Mul( 1.0f / sqrtf( nn ) );
        float nd = naxis.Dot( direction );
        float na = fabsf( nd );
        if( na >= maths::consts::FLOAT_MIN_NORM )
        {   //  the segments are not in the same plane relative to the direction (and by definition are not parallel to each other)
            maths::vec3 separation;
            separation.x = ( segment2.center.x - segment1.center.x );
            separation.y = ( segment2.center.y - segment1.center.y );
            separation.z = ( segment2.center.z - segment1.center.z );
            t = ( naxis.Dot( separation ) / nd );
            separation.x -= ( direction.x * t );
            separation.y -= ( direction.y * t );
            separation.z -= ( direction.z * t );
            float uv = segment1.offset.t_vec3().Dot( segment2.offset.t_vec3() );
            float su = separation.Dot( segment1.offset.t_vec3() );
            float sv = separation.Dot( segment2.offset.t_vec3() );
            float du = ( su - ( sv * uv ) );
            float dv = ( sv - ( su * uv ) );
            float lu = ( nn * ( segment1.offset.w + tolerance ) );
            float lv = ( nn * ( segment2.offset.w + tolerance ) );
            if( ( fabsf( du ) <= lu ) && ( fabsf( dv ) <= lv ) )
            {   //  there is a collision with the parallelogram surface
                ( nd > 0.0f ) ? normal.SetNegate( naxis ) : normal.Set( naxis );
                return( true );
            }
        }
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Cylinder and tube collision helper functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  point->segment with radius but no end caps
bool collideSide( const Point& aPoint, const Segment& aSegment, const float radius, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    maths::vec3 uaxis;
    uaxis.SetCross( direction, aSegment.offset.t_vec3() );
    float uu = uaxis.SqrLen();
    if( uu >= maths::consts::FLOAT_MIN_NORM )
    {   //  the direction is not along the axis of the tube
        maths::vec3 separation;
        separation.SetSub( aSegment.center.t_vec3(), aPoint );
        float rr = ( radius * radius );
        float su = uaxis.Dot( separation );
        float ru = ( ( rr * uu ) - ( su * su ) );
        if( ru > 0.0f )
        {
            maths::vec3 vaxis;
            vaxis.SetCross( direction, uaxis );
            float ot = ( ( vaxis.Dot( separation ) - ( direction.Dot( aSegment.offset.t_vec3() ) * sqrtf( ru ) ) ) / uu );
            if( fabsf( ot ) <= aSegment.offset.w )
            {
                separation.x += ( aSegment.offset.x * ot );
                separation.y += ( aSegment.offset.y * ot );
                separation.z += ( aSegment.offset.z * ot );
                float ss = separation.SqrLen();
                float sd = separation.Dot( direction );
                float rd = ( ( sd * sd ) + ( rr - ss ) );
                if( rd > 0.0f )
                {
                    t = ( sd - sqrtf( rd ) );
                    normal.x = ( ( direction.x * t ) - separation.x );
                    normal.y = ( ( direction.y * t ) - separation.y );
                    normal.z = ( ( direction.z * t ) - separation.z );
                    normal.Normalize();
                    return( true );
                }
            }
        }
    }
    return( false );
}

//  segment->point with radius but no end caps
bool collideSide( const Segment& aSegment, const Point& aPoint, const float radius, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    maths::vec3 uaxis;
    uaxis.SetCross( direction, aSegment.offset.t_vec3() );
    float uu = uaxis.SqrLen();
    if( uu >= maths::consts::FLOAT_MIN_NORM )
    {   //  the direction is not along the axis of the segment
        maths::vec3 separation;
        separation.SetSub( aPoint, aSegment.center.t_vec3() );
        float rr = ( radius * radius );
        float su = uaxis.Dot( separation );
        float ru = ( ( rr * uu ) - ( su * su ) );
        if( ru > 0.0f )
        {
            maths::vec3 vaxis;
            vaxis.SetCross( direction, uaxis );
            float ot = ( ( vaxis.Dot( separation ) - ( direction.Dot( aSegment.offset.t_vec3() ) * sqrtf( ru ) ) ) / uu );
            if( fabsf( ot ) <= aSegment.offset.w )
            {
                separation.x += ( aSegment.offset.x * ot );
                separation.y += ( aSegment.offset.y * ot );
                separation.z += ( aSegment.offset.z * ot );
                float ss = separation.SqrLen();
                float sd = separation.Dot( direction );
                float rd = ( ( sd * sd ) + ( rr - ss ) );
                if( rd > 0.0f )
                {
                    t = ( sd - sqrtf( rd ) );
                    normal.x = ( ( direction.x * t ) - separation.x );
                    normal.y = ( ( direction.y * t ) - separation.y );
                    normal.z = ( ( direction.z * t ) - separation.z );
                    normal.Normalize();
                    return( true );
                }
            }
        }
    }
    return( false );
}

//  segment->segment with radius but no end caps
//  important: this is an optimised specialist function that will not perform collisions between parallel segments (which is assumed to be covered by other testing)
bool collideSide( const Segment& segment1, const Segment& segment2, const float radius, const maths::vec3& direction, float& t, maths::vec3& normal )
{
    maths::vec3 naxis;
    naxis.SetCross( segment1.offset.t_vec3(), segment2.offset.t_vec3() );
    float nn = naxis.SqrLen();
    if( nn >= maths::consts::FLOAT_MIN_NORM )
    {   //  the segments are not parallel (so we will be able to calculate a collision plane normal and we can treat this as a point to parallelogram collision)
        naxis.Mul( 1.0f / sqrtf( nn ) );
        float nd = naxis.Dot( direction );
        float na = fabsf( nd );
        if( na >= maths::consts::FLOAT_MIN_NORM )
        {   //  the segments are not in the same plane relative to the direction (and by definition are not parallel to each other)
            maths::vec3 point;
            point.x = ( segment2.center.x - segment1.center.x );
            point.y = ( segment2.center.y - segment1.center.y );
            point.z = ( segment2.center.z - segment1.center.z );
            t = ( ( naxis.Dot( point ) / nd ) - ( radius / na ) );
            point.x -= ( direction.x * t );
            point.y -= ( direction.y * t );
            point.z -= ( direction.z * t );
            float uv = segment1.offset.t_vec3().Dot( segment2.offset.t_vec3() );
            float ou = point.Dot( segment1.offset.t_vec3() );
            float ov = point.Dot( segment2.offset.t_vec3() );
            float du = ( ou - ( ov * uv ) );
            float dv = ( ov - ( ou * uv ) );
            float lu = ( nn * segment1.offset.w );
            float lv = ( nn * segment2.offset.w );
            if( ( fabsf( du ) <= lu ) && ( fabsf( dv ) <= lv ) )
            {   //  there is a collision with the parallelogram surface
                ( nd > 0.0f ) ? normal.SetNegate( naxis ) : normal.Set( naxis );
                return( true );
            }
        }
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Specialised sphere, capsule and cylinder exclusion functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  sphere->point
bool exclude( const Sphere& object, const Point& target, maths::vec3& reposition )
{
    maths::vec3 separation;
    separation.x = ( object.x - target.x );
    separation.y = ( object.y - target.y );
    separation.z = ( object.z - target.z );
    float ss = separation.SqrLen();
    if( ss >= maths::consts::FLOAT_MIN_NORM )
    {
        float rr = ( object.r * object.r );
        if( ss < rr )
        {
            float t = sqrtf( rr / ss );
            reposition.x = ( target.x + ( separation.x * t ) );
            reposition.y = ( target.y + ( separation.y * t ) );
            reposition.z = ( target.z + ( separation.z * t ) );
            return( true );
        }
    }
    return( false );
}

//  capsule->point
bool exclude( const Capsule& object, const Point& target, maths::vec3& reposition )
{
    maths::vec3 separation;
    separation.x = ( object.center.x - target.x );
    separation.y = ( object.center.y - target.y );
    separation.z = ( object.center.z - target.z );
    float d = separation.Dot( object.offset.t_vec3() );
    float a = ( object.offset.w + object.center.r );
    if( fabsf( d ) < a )
    {
        if( d > object.offset.w ) d = object.offset.w;
        if( d < ( 0.0f - object.offset.w ) ) d = ( 0.0f - object.offset.w );
        separation.x -= ( object.offset.x * d );
        separation.y -= ( object.offset.y * d );
        separation.z -= ( object.offset.z * d );
        float ss = separation.SqrLen();
        float rr = ( object.center.r * object.center.r );
        if( ss < rr )
        {
            if( ss >= maths::consts::FLOAT_MIN_NORM )
            {
                float t = sqrtf( rr / ss );
                reposition.x = ( target.x + ( separation.x * t ) );
                reposition.y = ( target.y + ( separation.y * t ) );
                reposition.z = ( target.z + ( separation.z * t ) );
                return( true );
            }
            float t = ( ( d < 0.0f ) ? ( d + a ) : ( d - a ) );
            reposition.x = ( object.center.x - ( object.offset.x * t ) );
            reposition.y = ( object.center.y - ( object.offset.y * t ) );
            reposition.z = ( object.center.z - ( object.offset.z * t ) );
            return( true );
        }
    }
    return( false );
}

//  cylinder->point
bool exclude( const Cylinder& object, const Point& target, maths::vec3& reposition )
{
    maths::vec3 separation;
    separation.x = ( object.center.x - target.x );
    separation.y = ( object.center.y - target.y );
    separation.z = ( object.center.z - target.z );
    float d = separation.Dot( object.offset.t_vec3() );
    if( fabsf( d ) < object.offset.w )
    {
        separation.x -= ( object.offset.x * d );
        separation.y -= ( object.offset.y * d );
        separation.z -= ( object.offset.z * d );
        float ss = separation.SqrLen();
        float rr = ( object.center.r * object.center.r );
        if( ss < rr )
        {
            if( ss >= maths::consts::FLOAT_MIN_NORM )
            {
                float s = sqrtf( ss );
                if( ( object.center.r - s ) < ( object.offset.w - fabsf( d ) ) )
                {
                    float t = ( object.center.r / s );
                    reposition.x = ( target.x + ( separation.x * t ) );
                    reposition.y = ( target.y + ( separation.y * t ) );
                    reposition.z = ( target.z + ( separation.z * t ) );
                    return( true );
                }
            }
            float a = object.offset.w;
            float t = ( ( d < 0.0f ) ? ( d + a ) : ( d - a ) );
            reposition.x = ( object.center.x - ( object.offset.x * t ) );
            reposition.y = ( object.center.y - ( object.offset.y * t ) );
            reposition.z = ( object.center.z - ( object.offset.z * t ) );
            return( true );
        }
    }
    return( false );
}

//  sphere->plane
bool exclude( const Sphere& object, const Plane& target, maths::vec3& reposition )
{
    float pc = object.t_vec3().Dot( target );
    if( pc >= maths::consts::FLOAT_MIN_NORM )
    {
        float t = ( pc - object.r );
        if( t < 0.0f )
        {
            reposition.x = ( object.x - ( target.x * t ) );
            reposition.y = ( object.y - ( target.y * t ) );
            reposition.z = ( object.z - ( target.z * t ) );
            return( true );
        }
    }
    return( false );
}

//  capsule->plane
bool exclude( const Capsule& object, const Plane& target, maths::vec3& reposition )
{
    float pc = object.center.t_vec3().Dot( target );
    if( pc >= maths::consts::FLOAT_MIN_NORM )
    {
        float po = fabsf( object.offset.t_vec3().Dot( target.t_vec3() ) );
        float t = ( pc - object.center.r - ( po * object.offset.w ) );
        if( t < 0.0f )
        {
            reposition.x = ( object.center.x - ( target.x * t ) );
            reposition.y = ( object.center.y - ( target.y * t ) );
            reposition.z = ( object.center.z - ( target.z * t ) );
            return( true );
        }
    }
    return( false );
}

//  cylinder->plane
bool exclude( const Cylinder& object, const Plane& target, maths::vec3& reposition )
{
    float pc = object.center.t_vec3().Dot( target );
    if( pc >= maths::consts::FLOAT_MIN_NORM )
    {
        float po = fabsf( object.offset.t_vec3().Dot( target.t_vec3() ) );
        float t = ( pc - ( object.center.r * sqrtf( fabsf( 1.0f - ( po * po ) ) ) ) - ( po * object.offset.w ) );
        if( t < 0.0f )
        {
            reposition.x = ( object.center.x - ( target.x * t ) );
            reposition.y = ( object.center.y - ( target.y * t ) );
            reposition.z = ( object.center.z - ( target.z * t ) );
            return( true );
        }
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Test object
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CollisionTests
{
public:
    inline          CollisionTests() : m_passed( true ) { runTests(); };
    inline          ~CollisionTests() { m_passed = false; };
    inline bool     passed() const { return( m_passed ); };
    bool            runTests();
protected:
    bool            passed();
    bool            failed();
    bool            m_passed;
};

bool CollisionTests::passed()
{
    m_passed = true;
    return( true );
}

bool CollisionTests::failed()
{
    m_passed = false;
    return( false );
}

bool CollisionTests::runTests()
{
    m_passed = false;
    float t = 0.0f;
    maths::vec3 normal = maths::consts::VEC3::ZERO;
    maths::vec3 reposition = maths::consts::VEC3::ZERO;
    {   //  test point->point with radius
        maths::vec3 direction;
        Point point1, point2;
        direction.x = direction.y = direction.z = 1.0f;
        direction.Normalize();
        point1.x = point1.y = point1.z = 1.0f;
        point2.x = point2.y = point2.z = 4.0f;
        if( !collide( point1, point2, 1.0f, direction, t, normal ) )
        {   //  main collision failed
            return( failed() );
        }
        if( fabsf( ( t * t ) + t + t - 26.0f ) > ( maths::consts::FLOAT_EPSILON * 256.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( 1.0f + direction.Dot( normal ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        direction.x = direction.y = 0.0f;
        direction.z = 1.0f;
        point2.x = point2.y = ( 0.0f - maths::consts::FLOAT_EPSILON );
        if( collide( point1, point2, 1.0f, direction, t, normal ) )
        {   //  collision incorrectly detected
            return( failed() );
        }
        point2.x = point2.y = 0.5f;
        if( !collide( point1, point2, 1.0f, direction, t, normal ) )
        {   //  secondary collision failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT2 - 3.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        normal.x -= 0.5f;
        normal.y -= 0.5f;
        normal.z += maths::consts::RCPROOT2;
        if( normal.SqrLen() > maths::consts::FLOAT_EPSILON )
        {   //  collision normal incorrect
            return( failed() );
        }
    }
    {   //  test point->segment with radius
        maths::vec3 direction;
        Point aPoint;
        Segment aSegment;
        aSegment.center.x = aSegment.center.y = aSegment.center.z = 0.0f;
        aSegment.offset.x = aSegment.offset.z = 0.0f;
        aSegment.offset.y = 1.0f;
        aSegment.offset.w = 2.0f;
        aPoint.x = aPoint.y = aPoint.z = 0.0f;
        direction.x = direction.z = 0.0f;
        direction.y = -1.0f;
        if( !collide( aPoint, aSegment, 1.0f, direction, t, normal ) )
        {   //  upper end test failed
            return( failed() );
        }
        if( fabsf( t + 3.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( false );
        }
        if( fabsf( 1.0f + direction.Dot( normal ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        direction.y = 1.0f;
        if( !collide( aPoint, aSegment, 1.0f, direction, t, normal ) )
        {   //  lower end test failed
            return( failed() );
        }
        if( fabsf( t + 3.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( 1.0f + direction.Dot( normal ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        aSegment.offset.z = 1.0f;
        aSegment.offset.t_vec3().Normalize();
        direction.y = 0.0f;
        direction.z = 1.0f;
        if( !collide( aPoint, aSegment, 1.0f, direction, t, normal ) )
        {   //  mid-point test failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::ROOT2 ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( normal.x + ( normal.y * aSegment.offset.y ) - ( normal.z * aSegment.offset.z ) - 1.0f ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        aPoint.y = 0.5f;
        if( !collide( aPoint, aSegment, 1.0f, direction, t, normal ) )
        {   //  upper test failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::ROOT2 - 0.5f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( normal.x + ( normal.y * aSegment.offset.y ) - ( normal.z * aSegment.offset.z ) - 1.0f ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        aPoint.y = -0.5f;
        if( !collide( aPoint, aSegment, 1.0f, direction, t, normal ) )
        {   //  lower test failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::ROOT2 + 0.5f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( normal.x + ( normal.y * aSegment.offset.y ) - ( normal.z * aSegment.offset.z ) - 1.0f ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        aPoint.y = ( ( maths::consts::RCPROOT2 * 2.0f ) + 1.0f );
        if( collide( aPoint, aSegment, ( 1.0f - maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  false positive collision
            return( failed() );
        }
        if( !collide( aPoint, aSegment, ( 1.0f + maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  failed upper limit collision
            return( failed() );
        }
        aPoint.y = -( ( maths::consts::RCPROOT2 * 2.0f ) + 1.0f );
        if( collide( aPoint, aSegment, ( 1.0f - maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  false positive collision
            return( failed() );
        }
        if( !collide( aPoint, aSegment, ( 1.0f + maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  failed lower limit collision
            return( failed() );
        }
    }
    {   //  test segment->point with radius
        maths::vec3 direction;
        Point aPoint;
        Segment aSegment;
        aSegment.center.x = aSegment.center.y = aSegment.center.z = 0.0f;
        aSegment.offset.x = aSegment.offset.z = 0.0f;
        aSegment.offset.y = 1.0f;
        aSegment.offset.w = 2.0f;
        aPoint.x = aPoint.y = aPoint.z = 0.0f;
        direction.x = direction.z = 0.0f;
        direction.y = -1.0f;
        if( !collide( aSegment, aPoint, 1.0f, direction, t, normal ) )
        {   //  upper end test failed
            return( failed() );
        }
        if( fabsf( t + 3.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( 1.0f + direction.Dot( normal ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        direction.y = 1.0f;
        if( !collide( aSegment, aPoint, 1.0f, direction, t, normal ) )
        {   //  lower end test failed
            return( failed() );
        }
        if( fabsf( t + 3.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( 1.0f + direction.Dot( normal ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        aSegment.offset.z = 1.0f;
        aSegment.offset.t_vec3().Normalize();
        direction.y = 0.0f;
        direction.z = 1.0f;
        if( !collide( aSegment, aPoint, 1.0f, direction, t, normal ) )
        {   //  mid-point test failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::ROOT2 ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( normal.x + ( normal.y * aSegment.offset.y ) - ( normal.z * aSegment.offset.z ) - 1.0f ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        aPoint.y = 0.5f;
        if( !collide( aSegment, aPoint, 1.0f, direction, t, normal ) )
        {   //  upper test failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::ROOT2 + 0.5f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( normal.x + ( normal.y * aSegment.offset.y ) - ( normal.z * aSegment.offset.z ) - 1.0f ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        aPoint.y = -0.5f;
        if( !collide( aSegment, aPoint, 1.0f, direction, t, normal ) )
        {   //  lower test failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::ROOT2 - 0.5f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( normal.x + ( normal.y * aSegment.offset.y ) - ( normal.z * aSegment.offset.z ) - 1.0f ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        aPoint.y = ( ( maths::consts::RCPROOT2 * 2.0f ) + 1.0f );
        if( collide( aSegment, aPoint, ( 1.0f - maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  false positive collision
            return( failed() );
        }
        if( !collide( aSegment, aPoint, ( 1.0f + maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  false negative collision
            return( failed() );
        }
        aPoint.y = -( ( maths::consts::RCPROOT2 * 2.0f ) + 1.0f );
        if( collide( aSegment, aPoint, ( 1.0f - maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  false positive collision
            return( failed() );
        }
        if( !collide( aSegment, aPoint, ( 1.0f + maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  false negative collision
            return( failed() );
        }
    }
    {   //  test segment->segment with radius
        maths::vec3 direction;
        Segment segment1, segment2;
        direction.x = direction.y = 0.0f;
        direction.z = 1.0f;
        segment1.center.x = 0.0f;
        segment1.center.y = segment1.center.z = -1.0f;
        segment2.center.x = 0.0f;
        segment2.center.y = segment2.center.z = 1.0f;
        segment1.offset.x = 0.0f;
        segment1.offset.y = 1.0f;
        segment1.offset.z = 1.0f;
        segment1.offset.t_vec3().Normalize();
        segment1.offset.w = 2.0f;
        segment2.offset = segment1.offset;
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  parallel collision test failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::ROOT2 ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x ) + fabsf( normal.y - segment1.offset.y ) + fabsf( normal.z + segment1.offset.z ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment1.offset.x = segment2.offset.x = 0.0f;
        segment1.offset.y = -1.0f;
        segment2.offset.y = 1.0f;
        segment1.offset.z = segment2.offset.z = 0.0f;
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  parallel collision test failed
            return( failed() );
        }
        if( fabsf( t - 1.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x ) + fabsf( normal.y ) + fabsf( normal.z + 1.0f ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment1.center.y = -2.5f;
        segment2.center.y = 2.5f;
        if( collide( segment1, segment2, ( 1.0f - maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  false positive collision
            return( failed() );
        }
        if( !collide( segment1, segment2, ( 1.0f + maths::consts::FLOAT_EPSILON ), direction, t, normal ) )
        {   //  false negative collision
            return( failed() );
        }
        segment1.offset.x = 1.0f;
        segment1.offset.y = segment1.offset.z = 0.0f;
        segment1.offset.w = 2.0f;
        segment2.offset.y = 1.0f;
        segment2.offset.z = segment2.offset.x = 0.0f;
        segment2.offset.w = 3.0f;
        segment1.center.z = -4.0f;
        segment2.center.z = 4.0f;
        segment1.center.x = 0.0f;
        segment1.center.y = 0.0f;
        segment2.center.x = 0.0f;
        segment2.center.y = 0.0f;
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  mid-line collision test failed
            return( failed() );
        }
        if( fabsf( t - 7.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x ) + fabsf( normal.y ) + fabsf( normal.z + 1.0f ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = ( 2.0f + maths::consts::RCPROOT2 );
        segment2.center.y = 0.0f;
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  point edge collision test 1 failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT2 - 8.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x + maths::consts::RCPROOT2 ) + fabsf( normal.y ) + fabsf( normal.z + maths::consts::RCPROOT2 ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = -( 2.0f + maths::consts::RCPROOT2 );
        segment2.center.y = 0.0f;
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  point edge collision test 2 failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT2 - 8.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x - maths::consts::RCPROOT2 ) + fabsf( normal.y ) + fabsf( normal.z + maths::consts::RCPROOT2 ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = 0.0f;
        segment2.center.y = ( 3.0f + maths::consts::RCPROOT2 );
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  point edge collision test 3 failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT2 - 8.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x ) + fabsf( normal.y + maths::consts::RCPROOT2 ) + fabsf( normal.z + maths::consts::RCPROOT2 ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = 0.0f;
        segment2.center.y = -( 3.0f + maths::consts::RCPROOT2 );
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  point edge collision test 4 failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT2 - 8.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x ) + fabsf( normal.y - maths::consts::RCPROOT2 ) + fabsf( normal.z + maths::consts::RCPROOT2 ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = ( 2.0f + maths::consts::RCPROOT3 );
        segment2.center.y = ( 3.0f + maths::consts::RCPROOT3 );
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  point point collision test 1 failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT3 - 8.0f ) > ( maths::consts::FLOAT_EPSILON * 32.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x + maths::consts::RCPROOT3 ) + fabsf( normal.y + maths::consts::RCPROOT3 ) + fabsf( normal.z + maths::consts::RCPROOT3 ) ) > ( maths::consts::FLOAT_EPSILON * 32.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = -( 2.0f + maths::consts::RCPROOT3 );
        segment2.center.y = ( 3.0f + maths::consts::RCPROOT3 );
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  point point collision test 2 failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT3 - 8.0f ) > ( maths::consts::FLOAT_EPSILON * 32.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x - maths::consts::RCPROOT3 ) + fabsf( normal.y + maths::consts::RCPROOT3 ) + fabsf( normal.z + maths::consts::RCPROOT3 ) ) > ( maths::consts::FLOAT_EPSILON * 32.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = ( 2.0f + maths::consts::RCPROOT3 );
        segment2.center.y = -( 3.0f + maths::consts::RCPROOT3 );
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  point point collision test 3 failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT3 - 8.0f ) > ( maths::consts::FLOAT_EPSILON * 32.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x + maths::consts::RCPROOT3 ) + fabsf( normal.y - maths::consts::RCPROOT3 ) + fabsf( normal.z + maths::consts::RCPROOT3 ) ) > ( maths::consts::FLOAT_EPSILON * 32.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = -( 2.0f + maths::consts::RCPROOT3 );
        segment2.center.y = -( 3.0f + maths::consts::RCPROOT3 );
        if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
        {   //  point point collision test 4 failed
            return( failed() );
        }
        if( fabsf( t + maths::consts::RCPROOT3 - 8.0f ) > ( maths::consts::FLOAT_EPSILON * 32.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( ( fabsf( normal.x - maths::consts::RCPROOT3 ) + fabsf( normal.y - maths::consts::RCPROOT3 ) + fabsf( normal.z + maths::consts::RCPROOT3 ) ) > ( maths::consts::FLOAT_EPSILON * 32.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment1.offset.w = segment2.offset.w = 4.0f;
        segment1.offset.x = segment2.offset.x = 0.0f;
        segment1.center.x = segment2.center.x = 0.0f;
        segment1.offset.y = 2.0f;
        segment1.offset.z = 1.0f;
        segment1.offset.t_vec3().Normalize();
        segment2.offset.y = 1.0f;
        segment2.offset.z = 1.0f;
        segment2.offset.t_vec3().Normalize();
        segment1.center.y = 2.0f;
        segment2.center.y = 0.0f;
        segment1.center.z = -4.0f;
        segment2.center.z = 4.0f;
        for( int i = 0; i < 4; ++i )
        {
            segment1.offset.z = ( 0.0f - segment1.offset.z );
            if( i & 1 ) segment2.offset.z = ( 0.0f - segment2.offset.z );
            float s = 0.0f;
            for( int j = 0; j < 2; ++j )
            {
                segment1.center.y = ( 0.0f - segment1.center.y );
                segment1.offset.z = ( 0.0f - segment1.offset.z );
                segment2.offset.z = ( 0.0f - segment2.offset.z );
                for( int k = 0; k < 4; ++k )
                {
                    segment1.offset.t_vec3().Negate();
                    if( k & 1 ) segment2.offset.t_vec3().Negate();
                    if( !collide( segment1, segment2, 1.0f, direction, t, normal ) )
                    {   //  in-plane collision test failed
                        return( failed() );
                    }
                    if( j + k )
                    {
                        if( fabsf( t - s ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
                        {   //  collision distance incorrect
                            return( failed() );
                        }
                    }
                    else
                    {
                        s = t;
                    }
                }
            }
        }
    }
    {   //  test segment->segment
        maths::vec3 direction;
        Segment segment1, segment2;
        direction.x = direction.y = 0.0f;
        direction.z = 1.0f;
        segment1.center.x = segment1.center.y = 0.0f;
        segment2.center.x = segment2.center.y = 0.0f;
        segment1.center.z = -1.0f;
        segment2.center.z = 1.0f;
        segment1.offset.x = 1.0f;
        segment1.offset.y = -1.0f;
        segment1.offset.z = 0.0f;
        segment2.offset.x = -1.0f;
        segment2.offset.y = 0.0f;
        segment2.offset.z = 0.0f;
        segment1.offset.t_vec3().Normalize();
        segment2.offset.t_vec3().Normalize();
        segment1.offset.w = 0.5f;
        segment2.offset.w = 3.0f;
        if( !collideEdges( segment1, segment2, 0.0f, direction, t, normal ) )
        {   //  failed basic test
            return( failed() );
        }
        if( fabsf( t - 2.0f ) > ( maths::consts::FLOAT_EPSILON * 16.0f ) )
        {   //  collision distance incorrect
            return( failed() );
        }
        if( fabsf( normal.x + normal.y + normal.z + 1.0f ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  collision normal incorrect
            return( failed() );
        }
        segment2.center.x = ( ( 1.0f + maths::consts::FLOAT_EPSILON ) * 3.0f );
        if( collideEdges( segment1, segment2, 0.0f, direction, t, normal ) )
        {   //  failed() positive collision
            return( failed() );
        }
        segment2.center.x = ( ( 1.0f - maths::consts::FLOAT_EPSILON ) * 3.0f );
        if( !collideEdges( segment1, segment2, 0.0f, direction, t, normal ) )
        {   //  failed() negative collision
            return( failed() );
        }
    }
    {   //  test exclusion functions
        Point aPoint;
        Capsule aCapsule;
        Cylinder aCylinder;
        aPoint.x = aPoint.y = aPoint.z = 0.0f;
        aCapsule.offset.y = aCylinder.offset.y = 1.0f;
        aCapsule.center.x = aCapsule.center.z = aCylinder.center.x = aCylinder.center.z = 0.0f;
        aCapsule.offset.x = aCapsule.offset.z = aCylinder.offset.x = aCylinder.offset.z = 0.0f;
        aCapsule.center.r = aCylinder.center.r = 1.0f;
        aCapsule.offset.w = aCylinder.offset.w = 2.0f;
        aCapsule.center.y = aCylinder.center.y = 1.0f;
        if( !exclude( aCapsule, aPoint, reposition ) )
        {   //  failed() first capsule exclusion
            return( failed() );
        }
        if( ( fabsf( reposition.x ) + fabsf( reposition.y - 3.0f ) + fabsf( reposition.z ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  failed() repositioning
            return( failed() );
        }
        if( !exclude( aCylinder, aPoint, reposition ) )
        {   //  failed() first cylinder exclusion
            return( failed() );
        }
        if( ( fabsf( reposition.x ) + fabsf( reposition.y - 2.0f ) + fabsf( reposition.z ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  failed() repositioning
            return( failed() );
        }
        aCapsule.center.y = aCylinder.center.y = -1.0f;
        if( !exclude( aCapsule, aPoint, reposition ) )
        {   //  failed() second capsule exclusion
            return( failed() );
        }
        if( ( fabsf( reposition.x ) + fabsf( reposition.y + 3.0f ) + fabsf( reposition.z ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  failed() repositioning
            return( failed() );
        }
        if( !exclude( aCylinder, aPoint, reposition ) )
        {   //  failed() second cylinder exclusion
            return( failed() );
        }
        if( ( fabsf( reposition.x ) + fabsf( reposition.y + 2.0f ) + fabsf( reposition.z ) ) > ( maths::consts::FLOAT_EPSILON * 4.0f ) )
        {   //  failed() repositioning
            return( failed() );
        }
    }
    return( passed() );
}

CollisionTests testCollisions;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end detection namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace detection

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

