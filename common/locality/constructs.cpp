
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   constructs.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game procedural geometry construction helper classes and structures
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

#include "constructs.h"
#include "libs/system/debug/asserts.h"
#include "libs/maths/consts.h"
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
////    begin constructs namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace constructs
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TriangleOutlining triangle edge outlining triangle vertex interpolation data structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TriangleOutlining::setDefault()
{
    for( uint32_t index = 0; index < 3; ++index )
    {
        for( uint32_t ioffset = 0; ioffset < 3; ++ioffset )
        {
            edgesT[ index ].offset[ ioffset ] = 0.0f;
        }
        edgesT[ index ].offset[ index ] = 1.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    GeometryOutlining general geometry edge outlining triangle vertex interpolation data structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GeometryOutlining::setDefault()
{
    for( uint32_t index = 0; index < 3; ++index )
    {
        for( uint32_t ioffset = 0; ioffset < 3; ++ioffset )
        {
            edgesT[ index ].offset[ ioffset ] = 0.0f;
            edgesC[ index ].offset[ ioffset ] = edgesA[ index ].offset[ ioffset ] = 512.0f;
        }
        edgesT[ index ].offset[ index ] = 1.0f;
        edgesC[ index ].offset[ ( index + 1 ) % 3 ] = edgesA[ index ].offset[ ( index + 2 ) % 3 ] = 1024.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    GeometryDescArray sorted GeometryDesc array class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t GeometryDescArray::insert( const uint32_t ivert, const uint32_t params )
{
    GeometryDesc desc;
    desc.ivert = ivert;
    desc.params = params;
    int index = ArraySort::find( &desc );
    if( index < 0 )
    {
        index = ArraySort::insert( &desc );
        if( index >= 0 )
        {
            m_items.setItem( static_cast< uint >( index ), desc );
        }
    }
    return( index );
}

int GeometryDescArray::cmpKey( const void* const key, const int index ) const
{
    const GeometryDesc& desc1 = *reinterpret_cast< const GeometryDesc* const >( key );
    const GeometryDesc& desc2 = m_items.getItem( index );
    int delta = static_cast< int >( desc1.ivert - desc2.ivert );
    if( delta == 0 )
    {
        delta = static_cast< int >( desc1.params - desc2.params );
    }
    return( delta );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    GeometryVertArray sorted GeometryVert array class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t GeometryVertArray::insert( const uint32_t iposition )
{
    GeometryVert vert;
    vert.flags = 0;
    vert.iposition = iposition;
    int index = ArraySort::find( &vert );
    if( index < 0 )
    {
        index = ArraySort::insert( &vert );
        if( index >= 0 )
        {
            m_items.setItem( static_cast< uint >( index ), vert );
        }
    }
    return( index );
}

int GeometryVertArray::cmpKey( const void* const key, const int index ) const
{
    const GeometryVert& vert1 = *reinterpret_cast< const GeometryVert* const >( key );
    const GeometryVert& vert2 = m_items.getItem( index );
    return( static_cast< int >( vert1.iposition - vert2.iposition ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    GeometryEdgeArray sorted GeometryEdge array class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t GeometryEdgeArray::insert( const uint32_t ivert1, const uint32_t ivert2, const uint32_t itriangle )
{
    GeometryEdge edge;
    edge.flags = 0;
    if( ivert1 < ivert2 )
    {
        edge.iverts[ 0 ] = ivert1;
        edge.iverts[ 1 ] = ivert2;
    }
    else
    {
        edge.iverts[ 0 ] = ivert2;
        edge.iverts[ 1 ] = ivert1;
    }
    edge.itriangles[ 0 ] = itriangle;
    edge.itriangles[ 1 ] = ~uint32_t( 0 );
    int index = ArraySort::find( &edge );
    if( index >= 0 )
    {
        m_items.getItem( static_cast< uint >( index ) ).itriangles[ 1 ] = itriangle;
    }
    else
    {
        index = ArraySort::insert( &edge );
        if( index >= 0 )
        {
            m_items.setItem( static_cast< uint >( index ), edge );
        }
    }
    return( index );
}

int GeometryEdgeArray::cmpKey( const void* const key, const int index ) const
{
    const GeometryEdge& edge1 = *reinterpret_cast< const GeometryEdge* const >( key );
    const GeometryEdge& edge2 = m_items.getItem( index );
    int delta = static_cast< int >( edge1.iverts[ 0 ] - edge2.iverts[ 0 ] );
    if( delta == 0 )
    {
        delta = static_cast< int >( edge1.iverts[ 1 ] - edge2.iverts[ 1 ] );
    }
    return( delta );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    GeometryConstruct geometry construction class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GeometryConstruct::init()
{
    kill();
    m_valid = true;
    m_planes.init( 32 );
    m_positions.init( 256 );
    m_descs.init( 256 );
    m_verts.init( 256 );
    m_edges.init( 256 );
    m_triangles.init( 128 );
    m_surfaces.init( 32 );
    reset();
}

void GeometryConstruct::kill()
{
    if( valid() )
    {
        reset();
        m_valid = false;
        m_planes.kill();
        m_positions.kill();
        m_descs.kill();
        m_verts.kill();
        m_edges.kill();
        m_triangles.kill();
        m_surfaces.kill();
    }
}

void GeometryConstruct::zero()
{
    m_chainStrip = false;
    m_chainIndex = 0;
    m_chainVerts[ 0 ] = m_chainVerts[ 1 ] = maths::consts::VEC3::ZERO;
    m_chainIndices[ 0 ] = m_chainIndices[ 1 ] = 0;
    m_usage = 0;
    m_params = ~uint32_t( 0 );
    m_tolerance = ( 1.0f / 1024.0f );
    m_sqrTolerance = ( m_tolerance * m_tolerance );
    m_sphereBounds.x = m_sphereBounds.y = m_sphereBounds.z = m_sphereBounds.r = 0.0f;
    m_aabbBounds.min.x = m_aabbBounds.min.y = m_aabbBounds.min.z = m_aabbBounds.min.w = 0.0f;
    m_aabbBounds.max.x = m_aabbBounds.max.y = m_aabbBounds.max.z = m_aabbBounds.max.w = 0.0f;
    m_offset = maths::consts::VEC3::ZERO;
    m_scale = maths::consts::VEC3::ONE;
}

void GeometryConstruct::reset()
{
    if( valid() )
    {
        m_planes.reset();
        m_positions.reset();
        m_descs.reset();
        m_verts.reset();
        m_edges.reset();
        m_triangles.reset();
        m_surfaces.reset();
        zero();
    }
}

void GeometryConstruct::addVert( const maths::vec3& position )
{
    if( valid() )
    {
        chainVert( position, m_params );
    }
}

void GeometryConstruct::addVert( const maths::vec3& position, const uint32_t params )
{
    if( valid() )
    {
        chainVert( position, params );
    }
}

void GeometryConstruct::addPair( const maths::vec3& position1, const maths::vec3& position2 )
{
    if( valid() )
    {
        chainVert( position1, m_params );
        chainVert( position2, m_params );
    }
}

void GeometryConstruct::addPair( const maths::vec3& position1, const maths::vec3& position2, const uint32_t params )
{
    if( valid() )
    {
        chainVert( position1, params );
        chainVert( position2, params );
    }
}

void GeometryConstruct::addPair( const maths::vec3& position1, const uint32_t params1, const maths::vec3& position2, const uint32_t params2 )
{
    if( valid() )
    {
        chainVert( position1, params1 );
        chainVert( position2, params2 );
    }
}

void GeometryConstruct::addTri( const maths::vec3& position1, const maths::vec3& position2, const maths::vec3& position3 )
{
    if( valid() )
    {
        bool strip = m_chainStrip;
        m_chainStrip = true;
        m_chainIndex = 0;
        chainVert( position1, m_params );
        chainVert( position2, m_params );
        chainVert( position3, m_params );
        m_chainStrip = strip;
        m_chainIndex = 0;
    }
}

void GeometryConstruct::addTri( const maths::vec3& position1, const maths::vec3& position2, const maths::vec3& position3, const uint32_t params )
{
    if( valid() )
    {
        bool strip = m_chainStrip;
        m_chainStrip = true;
        m_chainIndex = 0;
        chainVert( position1, params );
        chainVert( position2, params );
        chainVert( position3, params );
        m_chainStrip = strip;
        m_chainIndex = 0;
    }
}

void GeometryConstruct::addTri( const maths::vec3& position1, const uint32_t params1, const maths::vec3& position2, const uint32_t params2, const maths::vec3& position3, const uint32_t params3 )
{
    if( valid() )
    {
        bool strip = m_chainStrip;
        m_chainStrip = true;
        m_chainIndex = 0;
        chainVert( position1, params1 );
        chainVert( position2, params2 );
        chainVert( position3, params3 );
        m_chainStrip = strip;
        m_chainIndex = 0;
    }
}

void GeometryConstruct::addQuad( const maths::vec3& position1, const maths::vec3& position2, const maths::vec3& position3, const maths::vec3& position4 )
{
    if( valid() )
    {
        bool strip = m_chainStrip;
        m_chainStrip = true;
        m_chainIndex = 0;
        chainVert( position1, m_params );
        chainVert( position2, m_params );
        chainVert( position4, m_params );
        chainVert( position3, m_params );
        m_chainStrip = strip;
        m_chainIndex = 0;
    }
}

void GeometryConstruct::addQuad( const maths::vec3& position1, const maths::vec3& position2, const maths::vec3& position3, const maths::vec3& position4, const uint32_t params )
{
    if( valid() )
    {
        bool strip = m_chainStrip;
        m_chainStrip = true;
        m_chainIndex = 0;
        chainVert( position1, params );
        chainVert( position2, params );
        chainVert( position4, params );
        chainVert( position3, params );
        m_chainStrip = strip;
        m_chainIndex = 0;
    }
}

void GeometryConstruct::addQuad( const maths::vec3& position1, const uint32_t params1, const maths::vec3& position2, const uint32_t params2, const maths::vec3& position3, const uint32_t params3, const maths::vec3& position4, const uint32_t params4 )
{
    if( valid() )
    {
        bool strip = m_chainStrip;
        m_chainStrip = true;
        m_chainIndex = 0;
        chainVert( position1, params1 );
        chainVert( position2, params2 );
        chainVert( position4, params4 );
        chainVert( position3, params3 );
        m_chainStrip = strip;
        m_chainIndex = 0;
    }
}

void GeometryConstruct::chainVert( const maths::vec3& position, const uint32_t params )
{
    maths::vec3 chainVert;
    chainVert.x = ( ( position.x * m_scale.x ) + m_offset.x );
    chainVert.y = ( ( position.y * m_scale.y ) + m_offset.y );
    chainVert.z = ( ( position.z * m_scale.z ) + m_offset.z );
    if( m_chainIndex > 1 )
    {   //  begin or continue the strip or fan
        if( m_chainIndex == 2 )
        {   //  begin the strip or fan
            m_chainIndices[ 0 ] = insertDesc( m_chainVerts[ 0 ], m_chainParams[ 0 ] );
            m_chainIndices[ 1 ] = insertDesc( m_chainVerts[ 1 ], m_chainParams[ 1 ] );
        }
        uint32_t index = insertDesc( chainVert, params );
        appendTri( m_chainIndices[ 0 ], m_chainIndices[ 1 ], index );
        int swap = ( m_chainStrip ? ( m_chainIndex & 1 ) : 1 );
        m_chainIndices[ swap ] = index;
    }
    else
    {
        m_chainVerts[ m_chainIndex ] = chainVert;
        m_chainParams[ m_chainIndex ] = params;
    }
    ++m_chainIndex;
}

uint32_t GeometryConstruct::insertPos( const maths::vec3& position )
{
    uint positions = m_positions.getUsed();
    for( uint iposition = 0; iposition < positions; ++iposition )
    {
        maths::vec3 delta;
        delta.SetSub( position, m_positions.getItem( iposition ) );
        if( delta.SqrLen() < m_sqrTolerance )
        {
            return( static_cast< uint32_t >( iposition ) );
        }
    }
    return( static_cast< uint32_t >( m_positions.append( position ) ) );
}

uint32_t GeometryConstruct::insertVert( const maths::vec3& position )
{
    return( m_verts.insert( insertPos( position ) ) );
}

uint32_t GeometryConstruct::insertDesc( const maths::vec3& position, const uint32_t params )
{
    return( m_descs.insert( insertVert( position ), params ) );
}

uint32_t GeometryConstruct::insertEdge( const uint32_t ivert1, const uint32_t ivert2, const uint32_t itriangle )
{
    return( m_edges.insert( ivert1, ivert2, itriangle ) );
}

void GeometryConstruct::appendTri( const uint32_t idesc1, const uint32_t idesc2, const uint32_t idesc3 )
{
    uint32_t ivert1 = m_descs.getBlob().getItem( static_cast< uint >( idesc1 ) ).ivert;
    uint32_t ivert2 = m_descs.getBlob().getItem( static_cast< uint >( idesc2 ) ).ivert;
    uint32_t ivert3 = m_descs.getBlob().getItem( static_cast< uint >( idesc3 ) ).ivert;
    const GeometryVert& vert1 = m_verts.getBlob().getItem( static_cast< uint >( ivert1 ) );
    const GeometryVert& vert2 = m_verts.getBlob().getItem( static_cast< uint >( ivert2 ) );
    const GeometryVert& vert3 = m_verts.getBlob().getItem( static_cast< uint >( ivert3 ) );
    const maths::vec3& position1 = m_positions.getItem( static_cast< uint >( vert1.iposition ) );
    const maths::vec3& position2 = m_positions.getItem( static_cast< uint >( vert2.iposition ) );
    const maths::vec3& position3 = m_positions.getItem( static_cast< uint >( vert3.iposition ) );
    uint32_t itriangle = static_cast< uint32_t >( m_triangles.getUsed() );
    GeometryTriangle& newTriangle = m_triangles.fetch();
    newTriangle.flags = 0;
    newTriangle.usage = m_usage;
    newTriangle.idescs[ 0 ] = idesc1;
    newTriangle.idescs[ 1 ] = idesc2;
    newTriangle.idescs[ 2 ] = idesc3;
    newTriangle.iverts[ 0 ] = ivert1;
    newTriangle.iverts[ 1 ] = ivert2;
    newTriangle.iverts[ 2 ] = ivert3;
    newTriangle.iedges[ 0 ] = insertEdge( ivert1, ivert2, itriangle );
    newTriangle.iedges[ 1 ] = insertEdge( ivert2, ivert3, itriangle );
    newTriangle.iedges[ 2 ] = insertEdge( ivert3, ivert1, itriangle );
    maths::plane newPlane;
    newPlane.Set( position1, position2, position3 );
    newPlane.Normalize();
    uint surfaces = m_surfaces.getUsed();
    for( uint isurface = 0; isurface < surfaces; ++isurface )
    {   //  check if this triangle is part of an existing surface
        GeometrySurface& surface = m_surfaces.getItem( isurface );
        maths::plane check = m_planes.getItem( static_cast< uint >( surface.iplane ) );
        if( check.t_vec3().Dot( newPlane.t_vec3() ) > 0.0f )
        {
            float distance1 = position1.Dot( check );
            float distance2 = position2.Dot( check );
            float distance3 = position3.Dot( check );
            if( ( fabsf( distance1 ) < m_tolerance ) && ( fabsf( distance2 ) < m_tolerance ) && ( fabsf( distance3 ) < m_tolerance ) )
            {   //  found a surface to add this triangle to
                newTriangle.isurface = static_cast< uint32_t >( isurface );
                newTriangle.inext = surface.itriangle;
                surface.itriangle = itriangle;
                ++surface.triangles;
                return;
            }
        }
    }
    newTriangle.isurface = static_cast< uint32_t >( surfaces );
    newTriangle.inext = ~uint32_t( 0 );
    GeometrySurface& newSurface = m_surfaces.fetch();
    newSurface.flags = 0;
    newSurface.iplane = static_cast< uint32_t >( m_planes.append( newPlane ) );
    newSurface.itriangle = itriangle;
    newSurface.triangles = 1;
}

void GeometryConstruct::clearTopology()
{
    if( valid() )
    {
        resetTopology();
    }
}

void GeometryConstruct::setupTopology()
{
    if( valid() )
    {
        resetTopology();
        {   //  construct the edge flags
            uint32_t edges = static_cast< uint32_t >( m_edges.getBlob().getUsed() );
            for( uint32_t iedge = 0; iedge < edges; ++iedge )
            {
                GeometryEdge& edge = m_edges.getBlob().getItem( static_cast< uint >( iedge ) );
                if( edge.itriangles[ 0 ] != ~uint32_t( 0 ) )
                {
                    if( edge.itriangles[ 1 ] != ~uint32_t( 0 ) )
                    {
                        GeometryTriangle& triangle1 = m_triangles.getItem( static_cast< uint >( edge.itriangles[ 0 ] ) );
                        GeometryTriangle& triangle2 = m_triangles.getItem( static_cast< uint >( edge.itriangles[ 1 ] ) );
                        if( triangle1.isurface != triangle2.isurface )
                        {   //  the triangles are on different surfaces
                            GeometrySurface& surface1 = m_surfaces.getItem( static_cast< uint >( triangle1.isurface ) );
                            GeometrySurface& surface2 = m_surfaces.getItem( static_cast< uint >( triangle2.isurface ) );
                            maths::plane& plane1 = m_planes.getItem( static_cast< uint >( surface1.iplane ) );
                            maths::plane& plane2 = m_planes.getItem( static_cast< uint >( surface2.iplane ) );
                            uint32_t ivert1 = triangle1.iverts[ 0 ];
                            if( ( ivert1 == edge.iverts[ 0 ] ) || ( ivert1 == edge.iverts[ 1 ] ) )
                            {
                                ivert1 = triangle1.iverts[ 1 ];
                                if( ( ivert1 == edge.iverts[ 0 ] ) || ( ivert1 == edge.iverts[ 1 ] ) )
                                {
                                    ivert1 = triangle1.iverts[ 2 ];
                                }
                            }
                            uint32_t ivert2 = triangle2.iverts[ 0 ];
                            if( ( ivert2 == edge.iverts[ 0 ] ) || ( ivert2 == edge.iverts[ 1 ] ) )
                            {
                                ivert2 = triangle2.iverts[ 1 ];
                                if( ( ivert2 == edge.iverts[ 0 ] ) || ( ivert2 == edge.iverts[ 1 ] ) )
                                {
                                    ivert2 = triangle2.iverts[ 2 ];
                                }
                            }
                            GeometryVert& vert1 = m_verts.getBlob().getItem( static_cast< uint >( ivert1 ) );
                            GeometryVert& vert2 = m_verts.getBlob().getItem( static_cast< uint >( ivert2 ) );
                            maths::vec3& position1 = m_positions.getItem( static_cast< uint >( vert1.iposition ) );
                            maths::vec3& position2 = m_positions.getItem( static_cast< uint >( vert2.iposition ) );
                            float delta1 = position1.Dot( plane2 );
                            float delta2 = position2.Dot( plane1 );
                            if( fabsf( delta1 ) > fabsf( delta2 ) )
                            {
                                edge.flags |= ( ( delta1 > 0.0f ) ? EConcaveEdge : EConvexEdge );
                            }
                            else
                            {
                                edge.flags |= ( ( delta2 > 0.0f ) ? EConcaveEdge : EConvexEdge );
                            }
                        }
                        if( triangle1.usage != triangle2.usage )
                        {
                            edge.flags |= ESectionEdge;
                        }
                    }
                    else
                    {   //  unshared edge
                        edge.flags |= EOutsideEdge;
                    }
                }
            }
        }
        {   //  construct the point flags
            uint32_t edges = static_cast< uint32_t >( m_edges.getBlob().getUsed() );
            uint32_t verts = static_cast< uint32_t >( m_verts.getBlob().getUsed() );
            for( uint32_t ivert = 0; ivert < verts; ++ivert )
            {
                GeometryVert& vert = m_verts.getBlob().getItem( static_cast< uint >( ivert ) );
                uint32_t convex = 0;
                uint32_t concave = 0;
                uint32_t outside = 0;
                uint32_t section = 0;
                for( uint32_t iedge = 0; iedge < edges; ++iedge )
                {
                    GeometryEdge& edge = m_edges.getBlob().getItem( static_cast< uint >( iedge ) );
                    if( ( edge.iverts[ 0 ] == ivert ) || ( edge.iverts[ 1 ] == ivert ) )
                    {
                        if( edge.flags & EConvexEdge ) ++convex;
                        if( edge.flags & EConcaveEdge ) ++concave;
                        if( edge.flags & EOutsideEdge ) ++outside;
                        if( edge.flags & ESectionEdge ) ++section;
                    }
                }
                if( convex )
                {
                    vert.flags |= ( ( convex > 2 ) ? EConvexApex : EConvexVert );
                }
                if( concave )
                {
                    vert.flags |= ( ( concave > 2 ) ? EConcaveBase : EConcaveVert );
                }
                if( outside )
                {
                    vert.flags |= EOutsideVert;
                }
                if( section )
                {
                    vert.flags |= ESectionVert;
                }
            }
        }
        {   //  accumulate the triangle flags and calculate the outline code
            uint32_t triangles = static_cast< uint32_t >( m_triangles.getUsed() );
            for( uint32_t itriangle = 0; itriangle < triangles; ++itriangle )
            {
                GeometryTriangle& triangle = m_triangles.getItem( static_cast< uint >( itriangle ) );
                for( uint32_t index = 0; index < 3; ++index )
                {
                    triangle.flags |= m_verts.getBlob().getItem( static_cast< uint >( triangle.iverts[ index ] ) ).flags;
                    triangle.flags |= m_edges.getBlob().getItem( static_cast< uint >( triangle.iedges[ index ] ) ).flags;
                }
            }
        }
        {   //  accumulate the surface flags
            uint32_t surfaces = static_cast< uint32_t >( m_surfaces.getUsed() );
            for( uint32_t isurface = 0; isurface < surfaces; ++isurface )
            {
                GeometrySurface& surface = m_surfaces.getItem( static_cast< uint >( isurface ) );
                uint32_t itriangle = surface.itriangle;
                for( uint32_t triangles = surface.triangles; triangles; --triangles )
                {
                    GeometryTriangle& triangle = m_triangles.getItem( static_cast< uint >( itriangle ) );
                    surface.flags |= triangle.flags;
                    itriangle = triangle.inext;
                }
            }
        }
    }
}

void GeometryConstruct::resetTopology()
{
    uint verts = m_verts.getBlob().getUsed();
    for( uint ivert = 0; ivert < verts; ++ivert )
    {   //  clear the point flags
        GeometryVert& vert = m_verts.getBlob().getItem( ivert );
        vert.flags = ENoConvexity;
    }
    uint edges = m_edges.getBlob().getUsed();
    for( uint iedge = 0; iedge < edges; ++iedge )
    {   //  clear the edge flags
        GeometryEdge& edge = m_edges.getBlob().getItem( iedge );
        edge.flags = ENoConvexity;
    }
    uint triangles = m_triangles.getUsed();
    for( uint itriangle = 0; itriangle < triangles; ++itriangle )
    {   //  clear the triangle flags and outline
        GeometryTriangle& triangle = m_triangles.getItem( itriangle );
        triangle.flags = ENoConvexity;
    }
    uint surfaces = m_surfaces.getUsed();
    for( uint isurface = 0; isurface < surfaces; ++isurface )
    {   //  clear the surface flags
        GeometrySurface& surface = m_surfaces.getItem( isurface );
        surface.flags = ENoConvexity;
    }
}

void GeometryConstruct::setupBounds()
{
    if( valid() )
    {
        m_sphereBounds.x = m_sphereBounds.y = m_sphereBounds.z = m_sphereBounds.r = 0.0f;
        m_aabbBounds.min.x = m_aabbBounds.min.y = m_aabbBounds.min.z = 0.0f;
        m_aabbBounds.max.x = m_aabbBounds.max.y = m_aabbBounds.max.z = 0.0f;
        uint position_count = m_positions.getUsed();
        if( position_count )
        {
            m_aabbBounds.Set( m_positions.getItem( 0 ) );
            for( uint position_index = 1; position_index < position_count; ++position_index )
            {
                m_aabbBounds.Merge( m_positions.getItem( position_index ) );
            }
            m_sphereBounds.x = ( ( m_aabbBounds.min.x + m_aabbBounds.max.x ) * 0.5f );
            m_sphereBounds.y = ( ( m_aabbBounds.min.y + m_aabbBounds.max.y ) * 0.5f );
            m_sphereBounds.z = ( ( m_aabbBounds.min.z + m_aabbBounds.max.z ) * 0.5f );
            for( uint position_index = 0; position_index < position_count; ++position_index )
            {
                maths::vec3 delta;
                delta.SetSub( m_positions.getItem( position_index ), m_sphereBounds.t_vec3() );
                float r = delta.Length();
                if( m_sphereBounds.r < r ) m_sphereBounds.r = r;
            }
        }
    }
}

void GeometryConstruct::buildOutlining( GeometryOutlining& outlining, const uint32_t itriangle, const OutliningConfig& config ) const
{
    outlining.setDefault();
    if( valid() )
    {
        if( itriangle < m_triangles.getUsed() )
        {
			uint32_t mask = static_cast< uint32_t >( config.outlineEnable );
            float scaling[ EOutlineTypeCount ];
            for( uint32_t itype = 0; itype < EOutlineTypeCount; ++itype )
            {
                float thickness = fabsf( config.scale * config.thickness[ itype ] );
                scaling[ itype ] = ( ( thickness > ( 1.0f / 4096.0f ) ) ? ( 1.0f / thickness ) : 4096.0f );
            }
            const GeometryTriangle& triangle = m_triangles.getItem( static_cast< uint >( itriangle ) );
            uint32_t isurface = triangle.isurface;
            const GeometrySurface& surface = m_surfaces.getItem( static_cast< uint >( isurface ) );
            const maths::plane& plane = m_planes.getItem( static_cast< uint >( surface.iplane ) );
            for( uint32_t ioffset = 0; ioffset < 3; ++ioffset )
            {
                uint32_t ivert = triangle.iverts[ ioffset ];
                const GeometryVert& vert = m_verts.getBlob().getItem( static_cast< uint >( ivert ) );
                const maths::vec3& origin = m_positions.getItem( static_cast< uint >( vert.iposition ) );
                maths::vec3 offset, normal;
                uint32_t iedgeC = triangle.iedges[ ioffset ];
                iedgeC = findOutlineEdge( isurface, itriangle, iedgeC, ivert, mask );
                if( iedgeC != ~uint32_t( 0 ) )
                {
                    const GeometryEdge& lineC = m_edges.getBlob().getItem( static_cast< uint >( iedgeC ) );
                    offset.SetSub( getVertPosition( lineC.iverts[ ( lineC.iverts[ 0 ] == ivert ) ? 1 : 0 ] ), origin );
                    offset.Normalize();
                    normal.SetCross( plane.t_vec3(), offset );
                    normal.Normalize();
                    normal.Mul( scaling[ getOutlineType( lineC.flags ) ] );
                    for( uint32_t index = 0; index < 3; ++index )
                    {
                        offset.SetSub( getVertPosition( triangle.iverts[ index ] ), origin );
                        outlining.edgesC[ index ].offset[ ioffset ] = normal.Dot( offset );
                    }
                }
                uint32_t iedgeA = triangle.iedges[ ( ioffset + 2 ) % 3 ];
                iedgeA = findOutlineEdge( isurface, itriangle, iedgeA, ivert, mask );
                if( iedgeA != ~uint32_t( 0 ) )
                {
                    const GeometryEdge& lineA = m_edges.getBlob().getItem( static_cast< uint >( iedgeA ) );
                    offset.SetSub( getVertPosition( lineA.iverts[ ( lineA.iverts[ 0 ] == ivert ) ? 1 : 0 ] ), origin );
                    offset.Normalize();
                    normal.SetCross( offset, plane.t_vec3() );
                    normal.Normalize();
                    normal.Mul( scaling[ getOutlineType( lineA.flags ) ] );
                    for( uint32_t index = 0; index < 3; ++index )
                    {
                        offset.SetSub( getVertPosition( triangle.iverts[ index ] ), origin );
                        outlining.edgesA[ index ].offset[ ioffset ] = normal.Dot( offset );
                    }
                }
            }
        }
    }
}

uint32_t GeometryConstruct::findOutlineEdge( const uint32_t isurface, const uint32_t itriangle, const uint32_t iedge, const uint32_t ivert, const uint32_t mask ) const
{
    uint32_t scan_itriangle = itriangle;
    uint32_t scan_iedge = iedge;
    do
    {
        const GeometryEdge& edge = m_edges.getBlob().getItem( static_cast< uint >( scan_iedge ) );
        if( mask & ( uint32_t( 1 ) << getOutlineType( edge.flags ) ) )
        {
            return( scan_iedge );
        }
        scan_itriangle = edge.itriangles[ ( edge.itriangles[ 0 ] == scan_itriangle ) ? 1 : 0 ];
        if( scan_itriangle == ~uint32_t( 0 ) ) break;
        const GeometryTriangle& triangle = m_triangles.getItem( static_cast< uint >( scan_itriangle ) );
        if( triangle.isurface != isurface ) break;
        for( uint32_t index = 0; index < 3; ++index )
        {
            if( triangle.iverts[ index ] == ivert )
            {
                scan_iedge = triangle.iedges[ ( triangle.iedges[ index ] == scan_iedge ) ? ( ( index + 2 ) % 3 ) : index ];
                break;
            }
        }
    }
    while( scan_iedge != iedge );
    return( ~uint32_t( 0 ) );
}

EOutlineType GeometryConstruct::getOutlineType( const uint32_t flags ) const
{
    uint32_t outlineType = static_cast< uint32_t >( EOutlineNone );
    if( flags & EConvexEdge )
    {
		outlineType = ( ( flags & ESectionEdge ) ? EOutlineSectionConvex : EOutlineConvex );
    }
    else if( flags & EConcaveEdge )
    {
		outlineType = ( ( flags & ESectionEdge ) ? EOutlineSectionConcave : EOutlineConcave );
    }
    else if( flags & EOutsideEdge )
    {
		outlineType = EOutlineOutside;
    }
    else if( flags & ESectionEdge )
    {
		outlineType = EOutlineSectionFlat;
    }
	return( static_cast< EOutlineType >( outlineType ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    LayeringConstruct GeometryConstruct collection interface class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LayeringConstruct::setConstruct( const int layer, GeometryConstruct* const construct )
{
    if( valid() && ( static_cast< uint >( layer ) < 32 ) )
    {
        m_constructs[ layer ] = construct;
        if( construct != NULL )
        {
            uint32_t flag = ( uint32_t( 1 ) << layer );
            m_active |= flag;
            m_mask |= flag;
        }
        else
        {
            uint32_t mask = ~( uint32_t( 1 ) << layer );
            m_active &= mask;
            m_mask &= mask;
        }
    }
}

GeometryConstruct* LayeringConstruct::getConstruct( const int layer ) const
{
    return( ( valid() && ( static_cast< uint >( layer ) < 32 ) ) ? m_constructs[ layer ] : NULL );
}

void LayeringConstruct::init()
{
    kill();
}

void LayeringConstruct::kill()
{
    if( valid() )
    {
        zero();
    }
}

void LayeringConstruct::zero()
{
    m_mask = 0;
    m_active = 0;
    for( int layer = 0; layer < 32; ++layer ) m_constructs[ layer ] = NULL;
}

void LayeringConstruct::reset()
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->reset();
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::resetAll()
{
    if( valid() )
    {
        m_active = 0;
        for( int layer = 0; layer < 32; ++layer )
        {
            GeometryConstruct* construct = m_constructs[ layer ];
            if( construct )
            {
                construct->reset();
                m_active |= ( uint32_t( 1 ) << layer );
            }
        }
        m_mask = m_active;
    }
}

void LayeringConstruct::setTolerance( const float tolerance )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->setTolerance( tolerance );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::setOffset( const maths::vec3& offset )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->setOffset( offset );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::setScale( const maths::vec3& scale )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->setScale( scale );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::setScale( const float scale )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->setScale( scale );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::setUsage( const uint32_t usage )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->setUsage( usage );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::setParams( const uint32_t params )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->setParams( params );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::beginFan()
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->beginFan();
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::beginStrip()
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->beginStrip();
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addVert( const maths::vec3& position )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addVert( position );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addVert( const maths::vec3& position, const uint32_t params )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addVert( position, params );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addPair( const maths::vec3& position1, const maths::vec3& position2 )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addPair( position1, position2 );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addPair( const maths::vec3& position1, const maths::vec3& position2, const uint32_t params )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addPair( position1, position2, params );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addPair( const maths::vec3& position1, const uint32_t params1, const maths::vec3& position2, const uint32_t params2 )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addPair( position1, params1, position2, params2 );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addTri( const maths::vec3& position1, const maths::vec3& position2, const maths::vec3& position3 )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addTri( position1, position2, position3 );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addTri( const maths::vec3& position1, const maths::vec3& position2, const maths::vec3& position3, const uint32_t params )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addTri( position1, position2, position3, params );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addTri( const maths::vec3& position1, const uint32_t params1, const maths::vec3& position2, const uint32_t params2, const maths::vec3& position3, const uint32_t params3 )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addTri( position1, params1, position2, params2, position3, params3 );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addQuad( const maths::vec3& position1, const maths::vec3& position2, const maths::vec3& position3, const maths::vec3& position4 )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addQuad( position1, position2, position3, position4 );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addQuad( const maths::vec3& position1, const maths::vec3& position2, const maths::vec3& position3, const maths::vec3& position4, const uint32_t params )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addQuad( position1, position2, position3, position4, params );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::addQuad( const maths::vec3& position1, const uint32_t params1, const maths::vec3& position2, const uint32_t params2, const maths::vec3& position3, const uint32_t params3, const maths::vec3& position4, const uint32_t params4 )
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->addQuad( position1, params1, position2, params2, position3, params3, position4, params4 );
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::clearTopology()
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->clearTopology();
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::setupTopology()
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->setupTopology();
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::setupBounds()
{
    if( valid() )
    {
        uint32_t mask = m_mask;
        for( int layer = 0; mask && ( layer < 32 ); ++layer )
        {
            if( mask & 1 )
            {
                GeometryConstruct* construct = m_constructs[ layer ];
                if( construct )
                {
                    construct->setupBounds();
                }
            }
            mask >>= 1;
        }
    }
}

void LayeringConstruct::finalise()
{
    if( valid() )
    {
        m_active = 0;
        for( int layer = 0; layer < 32; ++layer )
        {
            GeometryConstruct* construct = m_constructs[ layer ];
            if( construct )
            {
                construct->finalise();
                m_active |= ( uint32_t( 1 ) << layer );
            }
        }
        m_mask = m_active;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end constructs namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};  //  namespace constructs

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end locality namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};  //  namespace locality

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

