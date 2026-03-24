
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   collidables.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game collidable classes and structures
////
////    Notes:
////
////        'inset':
////
////            This is the geometry offset to use when close to doors to allow the view to pass through the door
////            without clipping against the near clip plane. 'inset' should be at least half the size of the diagonal
////            of the view at the near clip depth.
////
////        'zbias':
////
////            This is the geometry offset to use when cutting the doors into the scene.
////
////        'znear':
////
////            This is the near clip plane depth to use when calculating the extents of the door in clip space.
////            'znear' should be greater than zero and less than or equal to the camera near clip plane distance.
////
////        'zview':
////
////            This is a dual purpose tolerance value used for visibility clipping of the doors and identification of
////            doors which are being viewed from close to edge on.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard begin
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef	__COLLIDABLES_INCLUDED__
#define	__COLLIDABLES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/maths/vec2.h"
#include "libs/maths/vec3.h"
#include "libs/maths/mat44.h"
#include "libs/maths/frect.h"
#include "libs/maths/plane.h"
#include "libs/maths/joint.h"
#include "libs/maths/sphere.h"
#include "libs/memory/tblob.h"
#include "hierarchies.h"

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
////    Forward declaration of external classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace constructs
{
class GeometryConstruct;
class LayeringConstruct;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DoorCollider door view state enumeration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EViewState
{
    EViewNone = 0,                                      //  the view is invalid
    EViewFace = 1,                                      //  the view is from the front face
    EViewBack = 2,                                      //  the view is from the back face
    EViewNear = 4,                                      //  the view is near to the plane (within the tolerance range)
    EViewFrom = 8,                                      //  the view is inside the near clip safety region
    EViewFaceNear = ( EViewFace | EViewNear ),          //  the view is from or close to from the front face
    EViewBackNear = ( EViewBack | EViewNear ),          //  the view is from or close to from the back face
    EViewStateForce32 = 0x7fffffff
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Collision capsule type
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct capsule
{
    maths::sphere   center;     //  sphere at the center of the capsule
    maths::vec4     offset;     //  offset to the ends of the capsule and the square length of the offset in the w component
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BaseCollider base collisions class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BaseCollider
{
public:
    inline                  BaseCollider() {};
    inline                  ~BaseCollider() {};
protected:
    struct                  Vert { maths::vec4 cpoint; maths::plane nplane; };
    struct                  Edge { maths::vec4 cpoint; maths::vec4 cdelta; maths::plane tplane; maths::plane nplane; };
    bool                    edgeCollide( const maths::sphere& asphere, const maths::vec3& direction, const Edge& edge, float& t, maths::vec3& normal ) const;
    bool                    edgeCollide( const maths::sphere& asphere, const maths::vec3& direction, const Edge& edge, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    pointCollide( const maths::sphere& asphere, const maths::vec3& direction, const maths::vec3& point, float& t, maths::vec3& normal ) const;
    bool                    pointCollide( const maths::sphere& asphere, const maths::vec3& direction, const maths::vec3& point, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    edgeExclude( const maths::sphere& asphere, const Edge& edge, maths::vec3& reposition ) const;
    bool                    edgeExclude( const maths::sphere& asphere, const Edge& edge, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    pointExclude( const maths::sphere& asphere, const maths::vec3& point, maths::vec3& reposition ) const;
    bool                    pointExclude( const maths::sphere& asphere, const maths::vec3& point, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    edgeExclude( const capsule& acapsule, const Edge& edge, maths::vec3& reposition ) const;
    bool                    edgeExclude( const capsule& acapsule, const Edge& edge, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    pointExclude( const capsule& acapsule, const maths::vec3& point, maths::vec3& reposition ) const;
    bool                    pointExclude( const capsule& acapsule, const maths::vec3& point, maths::vec3& reposition, const maths::plane& reject ) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DoorCollider door collisions class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DoorCollider : protected BaseCollider
{
public:
    inline                  DoorCollider() : m_valid( false ), m_active( false ), m_count( 0 ), m_sides( NULL ) {};
    inline                  ~DoorCollider() { kill(); };
    static inline void      setConfig( const float inset, const float zbias, const float znear, const float zview ) { s_inset = inset; s_zbias = zbias; s_znear = znear; s_zview = zview; };
    inline bool             valid() const { return( this && m_valid ); };
    inline bool             active() const { return( this && m_valid && m_active ); };
    bool                    create( const maths::vec2* const verts, const uint count );
    bool                    create( const maths::vec2& scale, const maths::vec2& offset, const maths::vec2* const verts, const uint count );
    void                    destroy();
    bool                    relocate( const maths::vec2& scale, const maths::vec2& offset );
    bool                    isVisible( const maths::joint& localToView, const maths::plane& viewPlane ) const;
    EViewState              identifyView( const maths::joint& localToView ) const;
    bool                    containsPoint( const maths::vec3& point ) const;
    bool                    traversePoint( const EDoorState state, const maths::vec3& start, const maths::vec3& end ) const;
    bool                    touchesSphere( const EDoorState state, const maths::sphere& asphere ) const;
    bool                    collidePoint( const EDoorState state, const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const;
    bool                    collidePoint( const EDoorState state, const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    collideSphere( const EDoorState state, const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const;
    bool                    collideSphere( const EDoorState state, const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    excludeSphere( const EDoorState state, const maths::sphere& asphere, maths::vec3& reposition ) const;
    bool                    excludeSphere( const EDoorState state, const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    interceptPoint( const EDoorState state, const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, bool& face ) const;
    bool                    interceptSphere( const EDoorState state, const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, bool& face ) const;
    bool                    includingSphere( const EDoorState state, const maths::sphere& asphere, bool& face ) const;
    bool                    getViewExtents( const maths::joint& localToView, const maths::mat44& projection, maths::frect& extents ) const;
protected:
    struct                  Side { maths::vec4 dpoint; maths::vec4 ddelta; Edge edge; };
    bool                    init( const maths::vec2* const verts, const uint count );
    bool                    init( const maths::vec2& scale, const maths::vec2& offset );
    void                    kill();
    bool                    contains( const maths::vec2& point, const float radius = 0.0f ) const;
    bool                    intersects( const maths::vec2& start, const maths::vec2& end ) const;
    static float            s_inset;
    static float            s_zbias;
    static float            s_znear;
    static float            s_zview;
    bool                    m_valid;
    bool                    m_active;
    uint                    m_count;
    Side*                   m_sides;
    maths::frect            m_rect;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    RoomCollider room collisions class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomCollider : protected BaseCollider
{
public:
    inline                  RoomCollider() : m_memory( NULL ) { zero(); };
    inline                  ~RoomCollider() { kill(); };
    inline bool             valid() const { return( this && m_memory ); };
    bool                    create( const constructs::GeometryConstruct& construct );
    void                    destroy();
    inline void             setTolerance( const float tolerance ) { m_tolerance = tolerance; };
    inline bool             pointInAABB( const maths::vec3& point ) const { return( valid() ? m_aabbBounds.Contains( point ) : false ); };
    inline bool             pointInSphere( const maths::vec3& point ) const { return( valid() ? m_sphereBounds.Contains( point ) : false ); };
    inline bool             containsPoint( const maths::vec3& point ) const { return( valid() ? ( m_sphereBounds.Contains( point ) && m_aabbBounds.Contains( point ) ) : false ); };
    bool                    collidePoint( const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const;
    bool                    collidePoint( const maths::vec3& point, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    collideSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const;
    bool                    collideSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    excludeSphere( const maths::sphere& asphere, maths::vec3& reposition ) const;
    bool                    excludeSphere( const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    excludeSphereFaces( const maths::sphere& asphere, maths::vec3& reposition ) const;
    bool                    excludeSphereFaces( const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    excludeSphereEdges( const maths::sphere& asphere, maths::vec3& reposition ) const;
    bool                    excludeSphereEdges( const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    excludeSphereVerts( const maths::sphere& asphere, maths::vec3& reposition ) const;
    bool                    excludeSphereVerts( const maths::sphere& asphere, maths::vec3& reposition, const maths::plane& reject ) const;
protected:
    enum                    EConvex { EVert0 = 1, EVert1 = 2, EVert2 = 4, EEdge01 = 8, EEdge12 = 16, EEdge20 = 32, EPoly = 64, EConvexForce32 = 0x7fffffff };
    struct                  Surface { uint flags, plane_index, triangles_index, triangles_count; maths::plane nplane; };
    struct                  Triangle { uint flags, indices[ 3 ]; Edge edges[ 3 ]; };
    void                    zero();
    bool                    init( const constructs::GeometryConstruct& construct );
    void                    kill();
    void                    buildFlags();
    void                    buildBounds();
    void                    buildPrecalc();
    bool                    containsPoint( const Triangle& triangle, const maths::vec3& point ) const;
    bool                    rimCollide( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal ) const;
    bool                    rimCollide( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    rimCollideEdges( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal ) const;
    bool                    rimCollideEdges( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    rimCollideVerts( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal ) const;
    bool                    rimCollideVerts( const maths::sphere& asphere, const maths::vec3& direction, const Triangle& triangle, float& t, maths::vec3& normal, const maths::plane& reject ) const;
    bool                    rimExclude( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition ) const;
    bool                    rimExclude( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    rimExcludeEdges( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition ) const;
    bool                    rimExcludeEdges( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition, const maths::plane& reject ) const;
    bool                    rimExcludeVerts( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition ) const;
    bool                    rimExcludeVerts( const maths::sphere& asphere, const Triangle& triangle, maths::vec3& reposition, const maths::plane& reject ) const;
    uint8_t*                m_memory;
    maths::plane*           m_planes;
    maths::vec3*            m_vertices;
    Surface*                m_surfaces;
    Triangle*               m_triangles;
    uint                    m_plane_count;
    uint                    m_vertex_count;
    uint                    m_surface_count;
    uint                    m_triangle_count;
    maths::sphere           m_sphereBounds;
    maths::aabb             m_aabbBounds;
    float                   m_tolerance;
};

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
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //  #ifndef __COLLIDABLES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
