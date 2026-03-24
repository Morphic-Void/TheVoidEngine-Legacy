
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   generators.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game procedural geometry construction functions
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

#include "generators.h"
#include "libs/system/debug/asserts.h"
#include "libs/maths/consts.h"
#include "libs/maths/vec2.h"
#include "libs/utils/widgets.h"
#include "platforms/dx11/rendering/utils/standard_palette.h"
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
////    Outlining configurations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constructs::OutliningConfig g_usageEdgingConfigs[ EConstructUsageCount ] =
{
    {   //  EConstructUsageDefault
        1.0f,                   //  scale
        {   1.0f,               //  thickness[ EEdgingConvex ]
            1.0f,               //  thickness[ EEdgingConcave ]
            1.0f,               //  thickness[ EEdgingOutside ]
            1.0f,               //  thickness[ EEdgingSectionFlat ]
            1.0f,               //  thickness[ EEdgingSectionConvex ]
            1.0f },             //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableConvex |
            constructs::EOutlineEnableConcave |
            constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    },
    {   //  EConstructUsageCollision
        1.0f,                   //  scale
        {   1.0f,               //  thickness[ EEdgingConvex ]
            1.0f,               //  thickness[ EEdgingConcave ]
            1.0f,               //  thickness[ EEdgingOutside ]
            1.0f,               //  thickness[ EEdgingSectionFlat ]
            1.0f,               //  thickness[ EEdgingSectionConvex ]
            1.0f },             //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableConvex |
            constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionConvex )	//  outlineEnable
    },
    {   //  EConstructUsageWalls
        1.0f,                   //  scale
        {   1.0f,               //  thickness[ EEdgingConvex ]
            1.0f,               //  thickness[ EEdgingConcave ]
            1.0f,               //  thickness[ EEdgingOutside ]
            1.0f,               //  thickness[ EEdgingSectionFlat ]
            1.0f,               //  thickness[ EEdgingSectionConvex ]
            1.0f },             //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableConvex |
            constructs::EOutlineEnableConcave |
            constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    },
    {   //  EConstructUsageObject
        1.0f,                   //  scale
        {   ( 1.0f / 4.0f ),    //  thickness[ EEdgingConvex ]
            ( 1.0f / 4.0f ),    //  thickness[ EEdgingConcave ]
            ( 1.0f / 4.0f ),    //  thickness[ EEdgingOutside ]
            ( 1.0f / 32.0f ),   //  thickness[ EEdgingSectionFlat ]
            ( 1.0f / 32.0f ),   //  thickness[ EEdgingSectionConvex ]
            ( 1.0f / 32.0f ) }, //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableConvex |
            constructs::EOutlineEnableConcave |
            constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    },
    {   //  EConstructUsageLinkBody
        1.0f,                   //  scale
        {   ( 1.0f / 8.0f ),    //  thickness[ EEdgingConvex ]
            ( 1.0f / 8.0f ),    //  thickness[ EEdgingConcave ]
            ( 1.0f / 2.0f ),    //  thickness[ EEdgingOutside ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingSectionFlat ]
            ( 1.0f / 8.0f ),    //  thickness[ EEdgingSectionConvex ]
            ( 1.0f / 64.0f ) }, //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableConvex |
            constructs::EOutlineEnableConcave |
            constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    },
    {   //  EConstructUsageLinkHead
        1.0f,                   //  scale
        {   ( 1.0f / 8.0f ),    //  thickness[ EEdgingConvex ]
            ( 1.0f / 8.0f ),    //  thickness[ EEdgingConcave ]
            ( 1.0f / 2.0f ),    //  thickness[ EEdgingOutside ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingSectionFlat ]
            ( 1.0f / 8.0f ),    //  thickness[ EEdgingSectionConvex ]
            ( 1.0f / 64.0f ) }, //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableConvex |
            constructs::EOutlineEnableConcave |
            constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    },
    {   //  EConstructUsageLinkShim
        1.0f,                   //  scale
        {   ( 1.0f / 64.0f ),   //  thickness[ EEdgingConvex ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingConcave ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingOutside ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingSectionFlat ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingSectionConvex ]
            ( 1.0f / 64.0f ) }, //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    },
    {   //  EConstructUsageLinkBand
        1.0f,                   //  scale
        {   ( 1.0f / 64.0f ),   //  thickness[ EEdgingConvex ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingConcave ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingOutside ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingSectionFlat ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingSectionConvex ]
            ( 1.0f / 64.0f ) }, //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    },
    {   //  EConstructUsageLinkIcon
        1.0f,                   //  scale
        {   ( 1.0f / 64.0f ),   //  thickness[ EEdgingConvex ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingConcave ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingOutside ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingSectionFlat ]
            ( 1.0f / 64.0f ),   //  thickness[ EEdgingSectionConvex ]
            ( 1.0f / 64.0f ) }, //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    },
    {   //  EConstructUsageSky
        1.0f,                   //  scale
        {   ( 1.0f / 4.0f ),    //  thickness[ EEdgingConvex ]
            ( 1.0f / 4.0f ),    //  thickness[ EEdgingConcave ]
            ( 1.0f / 4.0f ),    //  thickness[ EEdgingOutside ]
            ( 1.0f / 32.0f ),   //  thickness[ EEdgingSectionFlat ]
            ( 1.0f / 32.0f ),   //  thickness[ EEdgingSectionConvex ]
            ( 1.0f / 32.0f ) }, //  thickness[ EEdgingSectionConcave ]
        0                       //  outlineEnable
    },
    {   //  EConstructUsageDebug
        1.0f,                   //  scale
        {   1.0f,               //  thickness[ EEdgingConvex ]
            1.0f,               //  thickness[ EEdgingConcave ]
            1.0f,               //  thickness[ EEdgingOutside ]
            1.0f,               //  thickness[ EEdgingSectionFlat ]
            1.0f,               //  thickness[ EEdgingSectionConvex ]
            1.0f },             //  thickness[ EEdgingSectionConcave ]
        (   constructs::EOutlineEnableConvex |
            constructs::EOutlineEnableConcave |
            constructs::EOutlineEnableOutside |
            constructs::EOutlineEnableSectionFlat |
            constructs::EOutlineEnableSectionConvex |
            constructs::EOutlineEnableSectionConcave )	//  outlineEnable
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    construct buffer classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ConstructIndices : public widgets::IIB
{
public:
    inline                          ConstructIndices() : m_indices() {};
    inline                          ConstructIndices( const uint count ) : m_indices( count ) {};
    inline                          ~ConstructIndices() { kill(); };
    inline void                     init( const uint count, const bool resize = false ) { m_indices.init( count, resize ); };
    inline void                     kill() { m_indices.kill(); };
    inline void                     reset() { m_indices.reset(); };
    inline uint                     getStride() const { return( m_indices.getStride() ); };
    inline uint                     getCount() const { return( m_indices.getCount() ); };
    inline uint                     getBytes() const { return( m_indices.getBytes() ); };
    inline uint16_t*                getData() { return( m_indices.getBuffer() ); };
    inline const uint16_t*          getData() const { return( m_indices.getBuffer() ); };
protected:
    virtual void                    Set( const uint32_t index );
    IndexStream16                   m_indices;
private:
    inline                          ConstructIndices( const ConstructIndices& ) {};
    inline void                     operator=( const ConstructIndices& ) {};
};

class ConstructVertices : public widgets::IVB
{
public:
    inline                          ConstructVertices() : m_vertices() {};
    inline                          ConstructVertices( const uint count ) : m_vertices( count ) {};
    inline                          ~ConstructVertices() { kill(); };
    inline void                     init( const uint count, const bool resize = false ) { m_vertices.init( count, resize ); };
    inline void                     kill() { m_vertices.kill(); m_bounds.min = m_bounds.max = maths::consts::VEC3::ZERO; };
    inline void                     reset() { m_vertices.reset(); m_bounds.min = m_bounds.max = maths::consts::VEC3::ZERO; };
    inline const maths::aabb&       getBounds() const { return( m_bounds ); };
    inline uint                     getStride() const { return( m_vertices.getStride() ); };
    inline uint                     getCount() const { return( m_vertices.getCount() ); };
    inline uint                     getBytes() const { return( m_vertices.getBytes() ); };
    inline DRAW_VERTEX*             getData() { return( m_vertices.getBuffer() ); };
    inline const DRAW_VERTEX*       getData() const { return( m_vertices.getBuffer() ); };
protected:
    virtual void                    Set( const float px, const float py, const float pz, const float nx, const float ny, const float nz, const uint32_t colour );
    VertexStreamDraw                m_vertices;
    maths::aabb                     m_bounds;
private:
    inline                          ConstructVertices( const ConstructVertices& ) {};
    inline void                     operator=( const ConstructVertices& ) {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DrawConstruct buffer classes function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConstructIndices::Set( const uint32_t index )
{
    m_indices.append( static_cast< uint16_t >( index ) );
}

void ConstructVertices::Set( const float px, const float py, const float pz, const float nx, const float ny, const float nz, const uint32_t colour )
{
    maths::vec3 position;
    position.Set( px, py, pz );
    if( m_vertices.getCount() == 0 )
    {
        m_bounds.Set( position );
    }
    else
    {
        m_bounds.Merge( position );
    }
    DRAW_VERTEX vertex;
    vertex.position.x = px;
    vertex.position.y = py;
    vertex.position.z = pz;
    vertex.position.w = 1.0f;
    vertex.normal.x = nx;
    vertex.normal.y = ny;
    vertex.normal.z = nz;
    vertex.normal.w = 0.0f;
    vertex.colour = colour;
    m_vertices.append( vertex );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    StairGen stair geometry generation class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StairGen::build( constructs::LayeringConstruct& construct )
{
    switch( m_style )
    {
        case( EStairsSolidStep ):
        {
            buildSolidStep( construct );
            break;
        }
        case( EStairsSolidRamp ):
        {
            buildSolidRamp( construct );
            break;
        }
        case( EStairsPlainRamp ):
        {
            buildPlainRamp( construct );
            break;
        }
        case( EStairsFlatUnder ):
        {
            buildFlatUnder( construct );
            break;
        }
        case( EStairsAdjoining ):
        {
            buildAdjoining( construct );
            break;
        }
        case( EStairsInterlock ):
        {
            buildInterlock( construct );
            break;
        }
        case( EStairsAlternate ):
        {
            buildAlternate( construct );
            break;
        }
        default:
        {
            break;
        }
    }
}

void StairGen::defaults()
{
    m_extents.max.x = 1.0f;
    m_extents.max.y = m_extents.max.z = 1.0f;
    m_extents.min.x = m_extents.min.y = m_extents.min.z = 0.0f;
    m_extents.Recenter();
    m_style = EStairsAlternate;
    m_flags = ( EStairFlagCloseLower | EStairFlagCloseUpper );
    m_count = 7;
    m_ratio = 0.0f; //1.5f;
    m_shift = ( 1.0f / 32.0f ); //0.0f;
    m_step_colour = getStandardColour( EColourAqua, 0 );
    m_side_colour = getStandardColour( EColourGrey, 0 );
    m_join_colour = getStandardColour( EColourGrey, 0 );
}

void StairGen::buildSolidStep( constructs::LayeringConstruct& construct )
{
    float dy = ( ( m_extents.max.y - m_extents.min.y ) / m_count );
    float dz = ( ( m_extents.max.z - m_extents.min.z ) / m_count );
    float x[ 2 ], y[ 2 ], z[ 2 ];
    x[ 0 ] = m_extents.min.x;
    x[ 1 ] = m_extents.max.x;
    for( uint32_t count = m_count; count; --count )
    {
        uint32_t i = ( m_count - count );
        uint32_t j = ( count - 1 );
        y[ 0 ] = ( m_extents.min.y + ( dy * i ) );
        y[ 1 ] = ( m_extents.max.y - ( dy * j ) );
        z[ 0 ] = ( m_extents.min.z + ( dz * i ) );
        z[ 1 ] = ( m_extents.max.z - ( dz * j ) );
        construct.setParams( m_step_colour );
        construct.beginStrip();
        construct.addPair( { x[ 0 ], y[ 1 ], z[ 1 ] }, { x[ 1 ], y[ 1 ], z[ 1 ] } );
        construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
        construct.addPair( { x[ 0 ], y[ 0 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 0 ] } );
        construct.setParams( m_side_colour );
        construct.beginFan();
        construct.addVert( { x[ 0 ], m_extents.min.y, m_extents.max.z } );
        construct.addVert( { x[ 0 ], y[ 1 ], z[ 1 ] } );
        construct.addVert( { x[ 0 ], y[ 1 ], z[ 0 ] } );
        construct.addVert( { x[ 0 ], y[ 0 ], z[ 0 ] } );
        construct.beginFan();
        construct.addVert( { x[ 1 ], m_extents.min.y, m_extents.max.z } );
        construct.addVert( { x[ 1 ], y[ 0 ], z[ 0 ] } );
        construct.addVert( { x[ 1 ], y[ 1 ], z[ 0 ] } );
        construct.addVert( { x[ 1 ], y[ 1 ], z[ 1 ] } );
    }
    if( m_flags & ( EStairFlagCloseLower | EStairFlagCloseUpper ) )
    {
        construct.setParams( m_join_colour );
        if( m_flags & EStairFlagCloseLower )
        {
            construct.addQuad( { m_extents.min.x, m_extents.min.y, m_extents.min.z }, { m_extents.max.x, m_extents.min.y, m_extents.min.z }, { m_extents.max.x, m_extents.min.y, m_extents.max.z }, { m_extents.min.x, m_extents.min.y, m_extents.max.z } );
        }
        if( m_flags & EStairFlagCloseUpper )
        {
            construct.addQuad( { m_extents.min.x, m_extents.min.y, m_extents.max.z }, { m_extents.max.x, m_extents.min.y, m_extents.max.z }, { m_extents.max.x, m_extents.max.y, m_extents.max.z }, { m_extents.min.x, m_extents.max.y, m_extents.max.z } );
        }
    }
}

void StairGen::buildSolidRamp( constructs::LayeringConstruct& construct )
{
    construct.setParams( m_step_colour );
    construct.addQuad( { m_extents.min.x, m_extents.max.y, m_extents.max.z }, { m_extents.max.x, m_extents.max.y, m_extents.max.z }, { m_extents.max.x, m_extents.min.y, m_extents.min.z }, { m_extents.min.x, m_extents.min.y, m_extents.min.z } );
    construct.setParams( m_side_colour );
    construct.addTri( { m_extents.min.x, m_extents.min.y, m_extents.max.z }, { m_extents.min.x, m_extents.max.y, m_extents.max.z }, { m_extents.min.x, m_extents.min.y, m_extents.min.z } );
    construct.addTri( { m_extents.max.x, m_extents.min.y, m_extents.max.z }, { m_extents.max.x, m_extents.min.y, m_extents.min.z }, { m_extents.max.x, m_extents.max.y, m_extents.max.z } );
    if( m_flags & ( EStairFlagCloseLower | EStairFlagCloseUpper ) )
    {
        construct.setParams( m_join_colour );
        if( m_flags & EStairFlagCloseLower )
        {
            construct.addQuad( { m_extents.min.x, m_extents.min.y, m_extents.min.z }, { m_extents.max.x, m_extents.min.y, m_extents.min.z }, { m_extents.max.x, m_extents.min.y, m_extents.max.z }, { m_extents.min.x, m_extents.min.y, m_extents.max.z } );
        }
        if( m_flags & EStairFlagCloseUpper )
        {
            construct.addQuad( { m_extents.min.x, m_extents.min.y, m_extents.max.z }, { m_extents.max.x, m_extents.min.y, m_extents.max.z }, { m_extents.max.x, m_extents.max.y, m_extents.max.z }, { m_extents.min.x, m_extents.max.y, m_extents.max.z } );
        }
    }
}

void StairGen::buildPlainRamp( constructs::LayeringConstruct& construct )
{
    float dy = ( ( ( m_extents.max.y - m_extents.min.y ) - m_shift ) / ( m_count + m_ratio ) );
    float dz = ( ( ( m_extents.max.z - m_extents.min.z ) - m_shift ) / ( m_count + m_ratio ) );
    float ty = ( ( dy * m_ratio ) + m_shift );
    float tz = ( ( dz * m_ratio ) + m_shift );
    float x[ 2 ], y[ 4 ], z[ 4 ];
    x[ 0 ] = m_extents.min.x;
    x[ 1 ] = m_extents.max.x;
    y[ 0 ] = m_extents.min.y;
    y[ 3 ] = m_extents.max.y;
    y[ 1 ] = ( y[ 0 ] + ty );
    y[ 2 ] = ( y[ 3 ] - ty );
    z[ 0 ] = m_extents.min.z;
    z[ 3 ] = m_extents.max.z;
    z[ 1 ] = ( z[ 0 ] + tz );
    z[ 2 ] = ( z[ 3 ] - tz );
    if( m_flags & EStairFlagCloseLower )
    {
        construct.setParams( m_join_colour );
        construct.addQuad( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } ,{ x[ 0 ], y[ 0 ], z[ 1 ] } );
    }
    construct.setParams( m_side_colour );
    construct.addQuad( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 0 ], y[ 2 ], z[ 3 ] }, { x[ 0 ], y[ 3 ], z[ 2 ] } );
    construct.addQuad( { x[ 1 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] } );
    construct.setParams( m_step_colour );
    construct.addQuad( { x[ 0 ], y[ 3 ], z[ 2 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 1 ], z[ 0 ] } );
    construct.addQuad( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] }, { x[ 0 ], y[ 2 ], z[ 3 ] } );
    if( m_flags & EStairFlagCloseUpper )
    {
        construct.setParams( m_join_colour );
        construct.addQuad( { x[ 0 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] } ,{ x[ 0 ], y[ 3 ], z[ 2 ] } );
    }
}

void StairGen::buildFlatUnder( constructs::LayeringConstruct& construct )
{
    float dy = ( ( ( m_extents.max.y - m_extents.min.y ) - m_shift ) / ( m_count + m_ratio ) );
    float dz = ( ( ( m_extents.max.z - m_extents.min.z ) - m_shift ) / ( m_count + m_ratio ) );
    float ty = ( ( dy * m_ratio ) + m_shift );
    float tz = ( ( dz * m_ratio ) + m_shift );
    float x[ 2 ], y[ 4 ], z[ 4 ];
    x[ 0 ] = m_extents.min.x;
    x[ 1 ] = m_extents.max.x;
    for( uint32_t count = m_count; count; --count )
    {
        uint32_t i = ( m_count - count );
        uint32_t j = ( count - 1 );
        y[ 0 ] = ( m_extents.min.y + ( dy * i ) );
        y[ 3 ] = ( m_extents.max.y - ( dy * j ) );
        y[ 1 ] = ( y[ 0 ] + ty );
        y[ 2 ] = ( y[ 3 ] - ty );
        z[ 0 ] = ( m_extents.min.z + ( dz * i ) );
        z[ 3 ] = ( m_extents.max.z - ( dz * j ) );
        z[ 1 ] = ( z[ 0 ] + tz );
        z[ 2 ] = ( z[ 3 ] - tz );
        if( i == 0 )
        {   //  start section
            if( m_flags & EStairFlagCloseLower )
            {
                construct.setParams( m_join_colour );
                construct.addQuad( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } ,{ x[ 0 ], y[ 0 ], z[ 1 ] } );
            }
        }
        construct.setParams( m_side_colour );
        construct.beginStrip();
        construct.addVert( { x[ 0 ], y[ 3 ], z[ 0 ] } );
        construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 3 ], z[ 2 ] } );
        construct.addPair( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 0 ], y[ 2 ], z[ 3 ] } );
        construct.beginStrip();
        construct.addVert( { x[ 1 ], y[ 3 ], z[ 0 ] } );
        construct.addPair( { x[ 1 ], y[ 3 ], z[ 2 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
        construct.addPair( { x[ 1 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } );
        construct.setParams( m_step_colour );
        construct.beginStrip();
        construct.addPair( { x[ 0 ], y[ 3 ], z[ 2 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] } );
        construct.addPair( { x[ 0 ], y[ 3 ], z[ 0 ] }, { x[ 1 ], y[ 3 ], z[ 0 ] } );
        construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
        construct.addQuad( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] } ,{ x[ 0 ], y[ 2 ], z[ 3 ] } );
        if( j == 0 )
        {   //  end-section
            if( m_flags & EStairFlagCloseUpper )
            {
                construct.setParams( m_join_colour );
                construct.addQuad( { x[ 0 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] } ,{ x[ 0 ], y[ 3 ], z[ 2 ] } );
            }
        }
    }
}

void StairGen::buildAdjoining( constructs::LayeringConstruct& construct )
{
    float h = ( m_extents.max.y - m_extents.min.y );
    float s = ( ( ( h * ( ( m_count + 2 ) - m_ratio ) ) + ( ( m_count + 2 ) * m_shift ) ) / ( m_count + 1 ) );
    float dy = ( s / ( m_count + 2 ) );
    float dz = ( ( m_extents.max.z - m_extents.min.z ) / m_count );
    float x[ 2 ], y[ 4 ], z[ 2 ];
    x[ 0 ] = m_extents.min.x;
    x[ 1 ] = m_extents.max.x;
    if( h <= s )
    {   //  the steps are discrete
        for( uint32_t count = m_count; count; --count )
        {
            uint32_t i = ( m_count - count );
            uint32_t j = ( count - 1 );
            y[ 0 ] = ( m_extents.min.y + ( dy * ( i + 1 ) ) );
            y[ 1 ] = ( m_extents.max.y - ( dy * ( j + 1 ) ) );
            z[ 0 ] = ( m_extents.min.z + ( dz * i ) );
            z[ 1 ] = ( m_extents.max.z - ( dz * j ) );
            construct.setParams( m_side_colour );
            construct.addQuad( { x[ 0 ], y[ 1 ], z[ 1 ] }, { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 1 ] } );
            construct.addQuad( { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 0 ] } );
            construct.setParams( m_step_colour );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 1 ] }, { x[ 1 ], y[ 1 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
        }
    }
    else
    {   //  the steps are touching
        for( uint32_t count = m_count; count; --count )
        {
            uint32_t i = ( m_count - count );
            uint32_t j = ( count - 1 );
            y[ 0 ] = ( m_extents.min.y + ( dy * ( i + 1 ) ) );
            y[ 3 ] = ( m_extents.max.y - ( dy * ( j + 1 ) ) );
            y[ 1 ] = ( y[ 3 ] - dy );
            y[ 2 ] = ( y[ 0 ] + dy );
            z[ 0 ] = ( m_extents.min.z + ( dz * i ) );
            z[ 1 ] = ( m_extents.max.z - ( dz * j ) );

            //construct.addTri( { x[ 1 ], y[ 3 ], z[ 0 ] }, { x[ 1 ], y[ 3 ], z[ 1 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] }, 0xff0000ff );
            //construct.addTri( { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 3 ], z[ 1 ] }, { x[ 1 ], y[ 2 ], z[ 1 ] }, 0xff00ff00 );
            //construct.addTri( { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 0 ] }, 0xffff0000 );
            //construct.addTri( { x[ 1 ], y[ 0 ], z[ 0 ] }, { x[ 1 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] }, 0xffff00ff );

            if( i == 0 )
            {   //  start section
                if( m_flags & EStairFlagCloseLower )
                {
                    construct.setParams( m_join_colour );
                    construct.addQuad( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 0 ] } ,{ x[ 0 ], y[ 0 ], z[ 0 ] } );
                }
            }
            construct.setParams( m_side_colour );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 1 ] }, { x[ 0 ], y[ 3 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 2 ], z[ 1 ] }, { x[ 0 ], y[ 1 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 0 ], y[ 0 ], z[ 0 ] } );
            construct.beginStrip();
            construct.addPair( { x[ 1 ], y[ 3 ], z[ 0 ] }, { x[ 1 ], y[ 3 ], z[ 1 ] } );
            construct.addPair( { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 2 ], z[ 1 ] } );
            construct.addPair( { x[ 1 ], y[ 0 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } );
            construct.setParams( m_step_colour );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 1 ] }, { x[ 1 ], y[ 3 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 0 ] }, { x[ 1 ], y[ 3 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 2 ], z[ 1 ] } );
            if( j == 0 )
            {   //  end-section
                if( m_flags & EStairFlagCloseUpper )
                {
                    construct.setParams( m_join_colour );
                    construct.addQuad( { x[ 0 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 3 ], z[ 1 ] } ,{ x[ 0 ], y[ 3 ], z[ 1 ] } );
                }
            }
        }
    }
}

void StairGen::buildInterlock( constructs::LayeringConstruct& construct )
{
    float dy = ( ( ( m_extents.max.y - m_extents.min.y ) - m_shift ) / ( m_count + m_ratio ) );
    float dz = ( ( ( m_extents.max.z - m_extents.min.z ) - m_shift ) / ( m_count + m_ratio ) );
    float ty = ( ( dy * m_ratio ) + m_shift );
    float tz = ( ( dz * m_ratio ) + m_shift );
    float x[ 2 ], y[ 4 ], z[ 4 ];
    x[ 0 ] = m_extents.min.x;
    x[ 1 ] = m_extents.max.x;
    if( ty == 0.0f )
    {
        for( uint32_t count = m_count; count; --count )
        {
            uint32_t i = ( m_count - count );
            uint32_t j = ( count - 1 );
            y[ 0 ] = ( m_extents.min.y + ( dy * i ) );
            y[ 1 ] = ( m_extents.max.y - ( dy * j ) );
            z[ 0 ] = ( m_extents.min.z + ( dz * i ) );
            z[ 1 ] = ( m_extents.max.z - ( dz * j ) );
            construct.setParams( m_side_colour );
            construct.addQuad( { x[ 0 ], y[ 1 ], z[ 1 ] }, { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 1 ] } );
            construct.addQuad( { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 0 ] } );
            construct.setParams( m_step_colour );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 1 ] }, { x[ 1 ], y[ 1 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
        }
    }
    else
    {
        for( uint32_t count = m_count; count; --count )
        {
            uint32_t i = ( m_count - count );
            uint32_t j = ( count - 1 );
            y[ 0 ] = ( m_extents.min.y + ( dy * i ) );
            y[ 3 ] = ( m_extents.max.y - ( dy * j ) );
            y[ 1 ] = ( y[ 0 ] + ty );
            y[ 2 ] = ( y[ 3 ] - ty );
            z[ 0 ] = ( m_extents.min.z + ( dz * i ) );
            z[ 3 ] = ( m_extents.max.z - ( dz * j ) );
            z[ 1 ] = ( z[ 0 ] + tz );
            z[ 2 ] = ( z[ 3 ] - tz );
            if( i == 0 )
            {   //  start section
                if( m_flags & EStairFlagCloseLower )
                {
                    construct.setParams( m_join_colour );
                    construct.addQuad( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } ,{ x[ 0 ], y[ 0 ], z[ 1 ] } );
                }
            }
            construct.setParams( m_side_colour );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 3 ] } );
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 2 ] }, { x[ 0 ], y[ 2 ], z[ 3 ] } );
            construct.beginStrip();
            construct.addPair( { x[ 1 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 0 ], z[ 3 ] }, { x[ 1 ], y[ 3 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] } );
            construct.setParams( m_step_colour );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 2 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] } );
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 0 ] }, { x[ 1 ], y[ 3 ], z[ 0 ] } );
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 3 ] }, { x[ 1 ], y[ 0 ], z[ 3 ] } );
            construct.addPair( { x[ 0 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] } );
            if( j == 0 )
            {   //  end-section
                if( m_flags & EStairFlagCloseUpper )
                {
                    construct.setParams( m_join_colour );
                    construct.addQuad( { x[ 0 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] } ,{ x[ 0 ], y[ 3 ], z[ 2 ] } );
                }
            }
        }
    }
}

void StairGen::buildAlternate( constructs::LayeringConstruct& construct )
{
    float dy = ( ( ( m_extents.max.y - m_extents.min.y ) - m_shift ) / ( ( m_count + 1 ) + m_ratio ) );
    float dz = ( ( ( m_extents.max.z - m_extents.min.z ) - m_shift ) / ( m_count + m_ratio ) );
    float x[ 2 ], y[ 6 ], z[ 4 ];
    x[ 0 ] = m_extents.min.x;
    x[ 1 ] = m_extents.max.x;
    for( uint32_t count = m_count; count; --count )
    {
        uint32_t i = ( m_count - count );
        uint32_t j = ( count - 1 );
        y[ 0 ] = ( m_extents.min.y + ( dy * ( i + 0 ) ) );
        y[ 1 ] = ( m_extents.max.y - ( dy * ( j + 2 ) ) );
        y[ 2 ] = ( m_extents.min.y + ( dy * ( i + 1 ) ) );
        y[ 3 ] = ( m_extents.max.y - ( dy * ( j + 1 ) ) );
        y[ 4 ] = ( m_extents.min.y + ( dy * ( i + 2 ) ) );
        y[ 5 ] = ( m_extents.max.y - ( dy * ( j + 0 ) ) );
        z[ 0 ] = ( m_extents.min.z + ( dz * i ) );
        z[ 1 ] = ( m_extents.max.z - ( dz * ( j + 1 ) ) );
        z[ 2 ] = ( m_extents.min.z + ( dz * ( i + 1 ) ) );
        z[ 3 ] = ( m_extents.max.z - ( dz * j ) );
        if( i == 0 )
        {   //  start section
            if( m_flags & EStairFlagCloseLower )
            {
                construct.setParams( m_join_colour );
                construct.addQuad( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } ,{ x[ 0 ], y[ 0 ], z[ 1 ] } );
            }
        }
        if( j == 0 )
        {   //  end-section
            construct.setParams( m_side_colour );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 0 ] }, { x[ 0 ], y[ 2 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 2 ] }, { x[ 0 ], y[ 2 ], z[ 3 ] } );
            construct.addPair( { x[ 0 ], y[ 5 ], z[ 2 ] }, { x[ 0 ], y[ 4 ], z[ 3 ] } );
            construct.beginStrip();
            construct.addPair( { x[ 1 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 3 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] } );
            construct.addPair( { x[ 1 ], y[ 4 ], z[ 3 ] }, { x[ 1 ], y[ 5 ], z[ 2 ] } );
            construct.setParams( m_step_colour );
            construct.beginStrip();
            construct.addPair( { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 1 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 3 ], z[ 0 ] }, { x[ 0 ], y[ 3 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 3 ], z[ 2 ] }, { x[ 0 ], y[ 3 ], z[ 2 ] } );
            construct.addPair( { x[ 1 ], y[ 5 ], z[ 2 ] }, { x[ 0 ], y[ 5 ], z[ 2 ] } );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 2 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] } );
            construct.addPair( { x[ 0 ], y[ 4 ], z[ 3 ] }, { x[ 1 ], y[ 4 ], z[ 3 ] } );
            if( m_flags & EStairFlagCloseUpper )
            {
                construct.setParams( m_join_colour );
                construct.addQuad( { x[ 0 ], y[ 4 ], z[ 3 ] }, { x[ 1 ], y[ 4 ], z[ 3 ] }, { x[ 1 ], y[ 5 ], z[ 2 ] } ,{ x[ 0 ], y[ 5 ], z[ 2 ] } );
            }
        }
        else
        {   //  mid-section
            construct.setParams( m_side_colour );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 0 ] }, { x[ 0 ], y[ 2 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 3 ], z[ 2 ] }, { x[ 0 ], y[ 2 ], z[ 3 ] } );
            construct.beginStrip();
            construct.addPair( { x[ 1 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 1 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 3 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 3 ], z[ 2 ] } );
            construct.setParams( m_step_colour );
            construct.beginStrip();
            construct.addPair( { x[ 1 ], y[ 1 ], z[ 0 ] }, { x[ 0 ], y[ 1 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 3 ], z[ 0 ] }, { x[ 0 ], y[ 3 ], z[ 0 ] } );
            construct.addPair( { x[ 1 ], y[ 3 ], z[ 2 ] }, { x[ 0 ], y[ 3 ], z[ 2 ] } );
            construct.beginStrip();
            construct.addPair( { x[ 0 ], y[ 0 ], z[ 1 ] }, { x[ 1 ], y[ 0 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 2 ], z[ 1 ] }, { x[ 1 ], y[ 2 ], z[ 1 ] } );
            construct.addPair( { x[ 0 ], y[ 2 ], z[ 3 ] }, { x[ 1 ], y[ 2 ], z[ 3 ] } );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    SwitchGen switch geometry generation class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SwitchGen::buildMono( constructs::LayeringConstruct& construct )
{
    setup();
    buildBase( construct );
    construct.setUsage( EConstructUsageLinkBody );
    construct.setParams( m_face_colour );
    for( uint32_t i = 0; i < m_band_count; ++i )
    {   //  lower body
        construct.beginStrip();
        for( uint32_t j = 0; j < m_face_count; ++j )
        {
            construct.addPair( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVFace ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVTopB ], m_hpos[ EHPosL ][ j ].z } );
        }
        construct.addPair( { m_hpos[ EHPosL ][ 0 ].x, m_vpos[ EVFace ], m_hpos[ EHPosL ][ 0 ].z }, { m_hpos[ EHPosL ][ 0 ].x, m_vpos[ EVTopB ], m_hpos[ EHPosL ][ 0 ].z } );
    }
    construct.setUsage( EConstructUsageLinkHead );
    if( m_shim_top )
    {
        construct.beginStrip();
        for( uint32_t i = 0; i < m_face_count; ++i )
        {   //  top
            uint32_t j = ( i >> 1 );
            uint32_t k = ( m_face_count - j - 1 );
            if( i & 1 )
            {
                construct.addPair( { m_hpos[ EHCutR ][ j ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ j ].z }, { m_hpos[ EHCutL ][ k ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ k ].z } );
            }
            else
            {
                construct.addPair( { m_hpos[ EHCutL ][ j ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ j ].z }, { m_hpos[ EHCutR ][ k ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ k ].z } );
            }
        }
        for( uint32_t i = 0; i < m_face_count; ++i )
        {   //  build faces
            uint32_t bit = ( uint32_t( 1 ) << i );
            if( m_icon_enable & bit )
            {
                construct.addQuad( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ i ].z }, { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z } );
                if( m_icon_select & bit )
                {   //  the face has a diamond
                    construct.addTri( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemT ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemL ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addTri( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addTri( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemB ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemT ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemL ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemR ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemT ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemL ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemB ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemB ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemR ][ i ].z } );
                }
                else
                {   //  the face has a square
                    construct.addQuad( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxR ][ i ].z }, { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxL ][ i ].z } );
                    construct.addQuad( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxL ][ i ].z }, { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxL ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addQuad( { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxL ][ i ].z }, { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addQuad( { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxR ][ i ].z } );
                }
            }
            else
            {   //  the face is blank
                construct.beginStrip();
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ i ].z } );
            }
        }
        construct.setUsage( EConstructUsageLinkShim );
        construct.setParams( m_shim_colour );
        for( uint32_t i = 0; i < m_face_count; ++i )
        {
            uint32_t j = ( ( i + 1 ) % m_face_count );
            construct.addTri( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ i ].z }, { m_hpos[ EHCutL ][ j ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ j ].z } );
        }
    }
    else
    {
        construct.beginStrip();
        for( uint32_t i = 1; i <= m_face_count; ++i )
        {   //  top
            uint32_t j = ( ( i & 1 ) ? ( i >> 1 ) : ( m_face_count - ( i >> 1 ) ) );
            construct.addVert( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ j ].z } );
        }
        for( uint32_t i = 0; i < m_face_count; ++i )
        {   //  build faces
            uint32_t bit = ( uint32_t( 1 ) << i );
            if( m_icon_enable & bit )
            {
                if( m_icon_select & bit )
                {   //  the face has a diamond
                    construct.addTri( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemT ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemL ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addTri( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addTri( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemB ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemT ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemL ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemR ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemT ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemL ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemB ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addTri( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemB ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemR ][ i ].z } );
                }
                else
                {   //  the face has a square
                    construct.addQuad( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxR ][ i ].z }, { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxL ][ i ].z } );
                    construct.addQuad( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z }, { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxL ][ i ].z }, { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxL ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addQuad( { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxL ][ i ].z }, { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addQuad( { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z }, { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxR ][ i ].z } );
                }
            }
            else
            {   //  the face is blank
                construct.beginStrip();
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z } );
            }
        }
    }
    construct.setUsage( EConstructUsageLinkIcon );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {   //  icons
        uint32_t bit = ( uint32_t( 1 ) << i );
        if( m_icon_enable & bit )
        {
            construct.setParams( m_icon_colours[ i ] );
            if( m_icon_select & bit )
            {   //  the face has a diamond
                construct.addQuad( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemB ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemL ][ i ].z }, { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVGemT ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVHalf ], m_hpos[ EHGemR ][ i ].z } );
            }
            else
            {   //  the face has a square
                construct.addQuad( { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxL ][ i ].z }, { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxT ], m_hpos[ EHBoxR ][ i ].z }, { m_hpos[ EHBoxR ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxR ][ i ].z }, { m_hpos[ EHBoxL ][ i ].x, m_vpos[ EVBoxB ], m_hpos[ EHBoxL ][ i ].z } );
            }
        }
    }
}

void SwitchGen::buildBody( constructs::LayeringConstruct& construct )
{
    setup();
    buildBase( construct );
    construct.setUsage( EConstructUsageLinkBody );
    construct.setParams( m_face_colour );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {   //  build faces
        uint32_t bit = ( uint32_t( 1 ) << i );
        if( m_icon_enable & bit )
        {
            if( m_icon_select & bit )
            {   //  the face has a triangle pointing up
                construct.beginFan();
                construct.addVert( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVTriB ], m_hpos[ EHGemR ][ i ].z } );
                construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHHalf ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHCutR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut0 ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVFace ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVFace ], m_hpos[ EHPosL ][ i ].z } );
                construct.beginFan();
                construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTriB ], m_hpos[ EHGemL ][ i ].z } );
                construct.addVert( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVTriB ], m_hpos[ EHGemR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVFace ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut0 ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHCutL ][ i ].z } );
                construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHHalf ][ i ].z } );
            }
            else
            {   //  the face has a triangle pointing down
                construct.beginFan();
                construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVTriB ], m_hpos[ EHHalf ][ i ].z } );
                construct.addVert( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHGemR ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHCutR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut0 ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVFace ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVFace ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut0 ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHCutL ][ i ].z } );
                construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHGemL ][ i ].z } );
            }
        }
        else
        {   //  the face is blank
            construct.beginStrip();
            construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVFace ], m_hpos[ EHPosR ][ i ].z } );
            construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVFace ], m_hpos[ EHPosL ][ i ].z } );
            construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut0 ], m_hpos[ EHPosR ][ i ].z } );
            construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut0 ], m_hpos[ EHPosL ][ i ].z } );
            construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHCutR ][ i ].z } );
            construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHCutL ][ i ].z } );
        }
    }
    construct.beginFan();
    construct.addVert( { 0.0f, m_vpos[ EVTopB ], 0.0f } );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {   //  top
        uint32_t j = ( ( m_face_count - i ) % m_face_count );
        uint32_t bit = ( uint32_t( 1 ) << j );
        construct.addVert( { m_hpos[ EHCutR ][ j ].x, m_vpos[ EVTopB ], m_hpos[ EHCutR ][ j ].z } );
        if( m_icon_enable & bit )
        {
            if( m_icon_select & bit )
            {   //  the face has a triangle pointing up
                construct.addVert( { m_hpos[ EHHalf ][ j ].x, m_vpos[ EVTopB ], m_hpos[ EHHalf ][ j ].z } );
            }
            else
            {   //  the face has a triangle pointing down
                construct.addVert( { m_hpos[ EHGemR ][ j ].x, m_vpos[ EVTopB ], m_hpos[ EHGemR ][ j ].z } );
                construct.addVert( { m_hpos[ EHGemL ][ j ].x, m_vpos[ EVTopB ], m_hpos[ EHGemL ][ j ].z } );
            }
        }
        construct.addVert( { m_hpos[ EHCutL ][ j ].x, m_vpos[ EVTopB ], m_hpos[ EHCutL ][ j ].z } );
    }
    construct.addVert( { m_hpos[ EHCutR ][ 0 ].x, m_vpos[ EVTopB ], m_hpos[ EHCutR ][ 0 ].z } );
    construct.setUsage( EConstructUsageLinkShim );
    construct.setParams( m_shim_colour );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {
        uint32_t j = ( ( i + 1 ) % m_face_count );
        construct.addTri( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVCut0 ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHCutR ][ i ].z }, { m_hpos[ EHCutL ][ j ].x, m_vpos[ EVTopB ], m_hpos[ EHCutL ][ j ].z } );
    }
    construct.setUsage( EConstructUsageLinkIcon );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {   //  icons
        uint32_t bit = ( uint32_t( 1 ) << i );
        if( m_icon_enable & bit )
        {
            construct.setParams( m_icon_colours[ i ] );
            if( m_icon_select & bit )
            {   //  the face has a triangle pointing up
                construct.addTri( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVTriB ], m_hpos[ EHGemR ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTriB ], m_hpos[ EHGemL ][ i ].z } );
            }
            else
            {   //  the face has a triangle pointing down
                construct.addTri( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVTriB ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHGemL ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVTopB ], m_hpos[ EHGemR ][ i ].z } );
            }
        }
    }
}

void SwitchGen::buildHead( constructs::LayeringConstruct& construct )
{
    setup();
    construct.setUsage( EConstructUsageLinkHead );
    construct.setParams( m_face_colour );
    construct.beginFan();
    construct.addVert( { 0.0f, m_vpos[ EVLowH ], 0.0f } );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {   //  build bottom
        uint32_t bit = ( uint32_t( 1 ) << i );
        construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ i ].z } );
        if( m_icon_enable & bit )
        {
            if( m_icon_select & bit )
            {   //  the face has a triangle pointing up
                construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHHalf ][ i ].z } );
            }
            else
            {   //  the face has a triangle pointing down
                construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHGemL ][ i ].z } );
                construct.addVert( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHGemR ][ i ].z } );
            }
        }
        construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
    }
    construct.addVert( { m_hpos[ EHCutL ][ 0 ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ 0 ].z } );
    if( m_shim_top )
    {
        construct.beginStrip();
        for( uint32_t i = 0; i < m_face_count; ++i )
        {   //  top
            uint32_t j = ( i >> 1 );
            uint32_t k = ( m_face_count - j - 1 );
            if( i & 1 )
            {
                construct.addPair( { m_hpos[ EHCutR ][ j ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ j ].z }, { m_hpos[ EHCutL ][ k ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ k ].z } );
            }
            else
            {
                construct.addPair( { m_hpos[ EHCutL ][ j ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ j ].z }, { m_hpos[ EHCutR ][ k ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ k ].z } );
            }
        }
        for( uint32_t i = 0; i < m_face_count; ++i )
        {   //  build faces
            uint32_t bit = ( uint32_t( 1 ) << i );
            if( m_icon_enable & bit )
            {
                if( m_icon_select & bit )
                {   //  the face has a triangle pointing down
                    construct.beginFan();
                    construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHGemL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ i ].z } );
                    construct.beginFan();
                    construct.addVert( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHGemR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHGemL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHHalf ][ i ].z } );
                }
                else
                {   //  the face has a triangle pointing up
                    construct.beginFan();
                    construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHGemL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHGemR ][ i ].z } );
                }
            }
            else
            {   //  the face is blank
                construct.beginStrip();
                construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ i ].z } );
            }
        }
        construct.setUsage( EConstructUsageLinkShim );
        construct.setParams( m_shim_colour );
        for( uint32_t i = 0; i < m_face_count; ++i )
        {
            uint32_t j = ( ( i + 1 ) % m_face_count );
            construct.addTri( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVCut2 ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHCutR ][ i ].z }, { m_hpos[ EHCutL ][ j ].x, m_vpos[ EVTopH ], m_hpos[ EHCutL ][ j ].z } );
            construct.addTri( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHCutL ][ j ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ j ].z }, { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
        }
    }
    else
    {
        construct.beginStrip();
        for( uint32_t i = 1; i <= m_face_count; ++i )
        {   //  top
            uint32_t j = ( ( i & 1 ) ? ( i >> 1 ) : ( m_face_count - ( i >> 1 ) ) );
            construct.addVert( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ j ].z } );
        }
        for( uint32_t i = 0; i < m_face_count; ++i )
        {   //  build faces
            uint32_t bit = ( uint32_t( 1 ) << i );
            if( m_icon_enable & bit )
            {
                if( m_icon_select & bit )
                {   //  the face has a triangle pointing down
                    construct.beginFan();
                    construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHGemL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z } );
                    construct.beginFan();
                    construct.addVert( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHGemR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHGemL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHHalf ][ i ].z } );
                }
                else
                {   //  the face has a triangle pointing up
                    construct.beginFan();
                    construct.addVert( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHHalf ][ i ].z } );
                    construct.addVert( { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHGemL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
                    construct.addVert( { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHGemR ][ i ].z } );
                }
            }
            else
            {   //  the face is blank
                construct.beginStrip();
                construct.addVert( { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
                construct.addVert( { m_hpos[ EHCutL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosL ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosR ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosR ][ i ].z } );
                construct.addVert( { m_hpos[ EHPosL ][ i ].x, m_vpos[ EVTopH ], m_hpos[ EHPosL ][ i ].z } );
            }
        }
        construct.setUsage( EConstructUsageLinkShim );
        construct.setParams( m_shim_colour );
        for( uint32_t i = 0; i < m_face_count; ++i )
        {
            uint32_t j = ( ( i + 1 ) % m_face_count );
            construct.addTri( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVCut1 ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHCutL ][ j ].x, m_vpos[ EVLowH ], m_hpos[ EHCutL ][ j ].z }, { m_hpos[ EHCutR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHCutR ][ i ].z } );
        }
    }
    construct.setUsage( EConstructUsageLinkIcon );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {   //  icons
        uint32_t bit = ( uint32_t( 1 ) << i );
        if( m_icon_enable & bit )
        {
            construct.setParams( m_icon_colours[ i ] );
            if( m_icon_select & bit )
            {   //  the face has a triangle pointing down
                construct.addTri( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHGemL ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHGemR ][ i ].z } );
            }
            else
            {   //  the face has a triangle pointing up
                construct.addTri( { m_hpos[ EHHalf ][ i ].x, m_vpos[ EVTriH ], m_hpos[ EHHalf ][ i ].z }, { m_hpos[ EHGemR ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHGemR ][ i ].z }, { m_hpos[ EHGemL ][ i ].x, m_vpos[ EVLowH ], m_hpos[ EHGemL ][ i ].z } );
            }
        }
    }
}

void SwitchGen::buildBase( constructs::LayeringConstruct& construct )
{
    construct.setUsage( EConstructUsageLinkBody );
    construct.setParams( m_face_colour );
    construct.beginStrip();
    for( uint32_t i = 0; i < m_face_count; ++i )
    {   //  bottom
        uint32_t j = ( ( i & 1 ) ? ( i >> 1 ) : ( m_face_count - ( i >> 1 ) - 1 ) );
        construct.addVert( { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVLowB ], m_hpos[ EHPosL ][ j ].z } );
    }
    uint32_t row0 = ( EVRow0 + ( ( 3 - m_band_count ) << 1 ) );
    uint32_t row1 = EVLowB;
    uint32_t row2 = row0;
    for( uint32_t i = 0; i < m_band_count; ++i )
    {
        construct.beginStrip();
        for( uint32_t j = 0; j < m_face_count; ++j )
        {
            construct.addPair( { m_hpos[ EHPosL ][ j ].x, m_vpos[ row1 ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHPosL ][ j ].x, m_vpos[ row2 ], m_hpos[ EHPosL ][ j ].z } );
        }
        construct.addPair( { m_hpos[ EHPosL ][ 0 ].x, m_vpos[ row1 ], m_hpos[ EHPosL ][ 0 ].z }, { m_hpos[ EHPosL ][ 0 ].x, m_vpos[ row2 ], m_hpos[ EHPosL ][ 0 ].z } );
        row1 = ( row2 + 2 );
        row2 += 4;
    }
    if( row1 != EVFace )
    {
        construct.beginStrip();
        for( uint32_t j = 0; j < m_face_count; ++j )
        {
            construct.addPair( { m_hpos[ EHPosL ][ j ].x, m_vpos[ row1 ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHPosL ][ j ].x, m_vpos[ EVFace ], m_hpos[ EHPosL ][ j ].z } );
        }
        construct.addPair( { m_hpos[ EHPosL ][ 0 ].x, m_vpos[ row1 ], m_hpos[ EHPosL ][ 0 ].z }, { m_hpos[ EHPosL ][ 0 ].x, m_vpos[ EVFace ], m_hpos[ EHPosL ][ 0 ].z } );
    }
    row1 = ( row0 + 1 );
    row2 = ( row1 + 1 );
    construct.setUsage( EConstructUsageLinkBand );
    for( uint32_t i = 0; i < m_band_count; ++i )
    {   //  bands
        construct.setParams( m_band_colours[ i ] );
        construct.beginStrip();
        for( uint32_t j = 0; j < m_face_count; ++j )
        {
            construct.addPair( { m_hpos[ EHPosL ][ j ].x, m_vpos[ row0 ], m_hpos[ EHPosL ][ j ].z }, { m_hpos[ EHOutL ][ j ].x, m_vpos[ row1 ], m_hpos[ EHOutL ][ j ].z } );
        }
        construct.addPair( { m_hpos[ EHPosL ][ 0 ].x, m_vpos[ row0 ], m_hpos[ EHPosL ][ 0 ].z }, { m_hpos[ EHOutL ][ 0 ].x, m_vpos[ row1 ], m_hpos[ EHOutL ][ 0 ].z } );
        construct.beginStrip();
        for( uint32_t j = 0; j < m_face_count; ++j )
        {
            construct.addPair( { m_hpos[ EHOutL ][ j ].x, m_vpos[ row1 ], m_hpos[ EHOutL ][ j ].z }, { m_hpos[ EHPosL ][ j ].x, m_vpos[ row2 ], m_hpos[ EHPosL ][ j ].z } );
        }
        construct.addPair( { m_hpos[ EHOutL ][ 0 ].x, m_vpos[ row1 ], m_hpos[ EHOutL ][ 0 ].z }, { m_hpos[ EHPosL ][ 0 ].x, m_vpos[ row2 ], m_hpos[ EHPosL ][ 0 ].z } );
        row0 += 4;
        row1 += 4;
        row2 += 4;
    }
}

void SwitchGen::defaults()
{
    m_face_count = 3;
    m_face_width = ( 1.0f / m_face_count );
    m_face_height = 1.0f;
    m_face_colour = getStandardColour( EColourGrey, 1 );
    m_band_count = 3;
    m_band_depth = ( 1.0f / 64.0f );
    m_band_colours[ 0 ] = getStandardColour( EColourAqua, 0 );
    m_band_colours[ 1 ] = getStandardColour( EColourViolet, 0 );
    m_band_colours[ 2 ] = getStandardColour( EColourGoldenBrown, 0 );
    m_icon_enable = 0x0000fffb;
    m_icon_select = 0x0000fff9;
    m_icon_colours[  0 ] = getStandardColour( EColourCyan, 0 );
    m_icon_colours[  1 ] = getStandardColour( EColourPurple, 0 );
    m_icon_colours[  2 ] = getStandardColour( EColourEmerald, 0 );
    m_icon_colours[  3 ] = getStandardColour( EColourDarkBlue, 0 );
    m_icon_colours[  4 ] = getStandardColour( EColourTeal, 0 );
    m_icon_colours[  5 ] = getStandardColour( EColourDarkBrown, 0 );
    m_icon_colours[  6 ] = getStandardColour( EColourRichBlue, 0 );
    m_icon_colours[  7 ] = getStandardColour( EColourRichPink, 0 );
    m_icon_colours[  8 ] = getStandardColour( EColourCerise, 0 );
    m_icon_colours[  9 ] = getStandardColour( EColourPaleYellow, 0 );
    m_icon_colours[ 10 ] = getStandardColour( EColourPastelPink, 0 );
    m_icon_colours[ 11 ] = getStandardColour( EColourLime, 0 );
    m_icon_colours[ 12 ] = getStandardColour( EColourPastelPurple, 0 );
    m_icon_colours[ 13 ] = getStandardColour( EColourOlive, 0 );
    m_icon_colours[ 14 ] = getStandardColour( EColourLemon, 0 );
    m_icon_colours[ 15 ] = getStandardColour( EColourSteelBlue, 0 );
    m_icon_ratio = 0.5f;
    m_shim_depth = ( 1.0f / 64.0f );
    m_shim_colour = getStandardColour( EColourGrey, 0 );
    m_shim_top = true;
}

void SwitchGen::setup()
{
    float pos_scale = ( ( m_face_width * 0.5f ) / sinf( maths::consts::PI / m_face_count ) );
    float out_scale = ( pos_scale + ( m_band_depth / cosf( maths::consts::PI / m_face_count ) ) );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {
        float angle = ( ( ( ( i << 1 ) + 1 ) * maths::consts::PI ) / m_face_count );
        float x = ( 0.0f - sinf( angle ) );
        float z = cosf( angle );
        m_hpos[ EHPosL ][ i ].x = ( x * pos_scale );
        m_hpos[ EHPosL ][ i ].z = ( z * pos_scale );
        m_hpos[ EHOutL ][ i ].x = ( x * out_scale );
        m_hpos[ EHOutL ][ i ].z = ( z * out_scale );
    }
    for( uint32_t i = 0; i < m_face_count; ++i )
    {
        uint32_t j = ( ( i + 1 ) % m_face_count );
        m_hpos[ EHPosR ][ i ] = m_hpos[ EHPosL ][ j ];
        m_hpos[ EHOutR ][ i ] = m_hpos[ EHOutL ][ j ];
    }
    float cut_scale = ( m_shim_depth / m_face_width );
    float gem_scale = ( ( 1.0f - m_icon_ratio ) * 0.5f );
    float box_scale = ( ( 1.0f - ( m_icon_ratio * maths::consts::RCPROOT2 ) ) * 0.5f );
    for( uint32_t i = 0; i < m_face_count; ++i )
    {
        HPOS& posl = m_hpos[ EHPosL ][ i ];
        HPOS& posr = m_hpos[ EHPosR ][ i ];
        m_hpos[ EHCutL ][ i ].x = ( ( ( posr.x - posl.x ) * cut_scale ) + posl.x );
        m_hpos[ EHCutL ][ i ].z = ( ( ( posr.z - posl.z ) * cut_scale ) + posl.z );
        m_hpos[ EHCutR ][ i ].x = ( ( ( posl.x - posr.x ) * cut_scale ) + posr.x );
        m_hpos[ EHCutR ][ i ].z = ( ( ( posl.z - posr.z ) * cut_scale ) + posr.z );
        m_hpos[ EHGemL ][ i ].x = ( ( ( posr.x - posl.x ) * gem_scale ) + posl.x );
        m_hpos[ EHGemL ][ i ].z = ( ( ( posr.z - posl.z ) * gem_scale ) + posl.z );
        m_hpos[ EHGemR ][ i ].x = ( ( ( posl.x - posr.x ) * gem_scale ) + posr.x );
        m_hpos[ EHGemR ][ i ].z = ( ( ( posl.z - posr.z ) * gem_scale ) + posr.z );
        m_hpos[ EHBoxL ][ i ].x = ( ( ( posr.x - posl.x ) * box_scale ) + posl.x );
        m_hpos[ EHBoxL ][ i ].z = ( ( ( posr.z - posl.z ) * box_scale ) + posl.z );
        m_hpos[ EHBoxR ][ i ].x = ( ( ( posl.x - posr.x ) * box_scale ) + posr.x );
        m_hpos[ EHBoxR ][ i ].z = ( ( ( posl.z - posr.z ) * box_scale ) + posr.z );
        m_hpos[ EHHalf ][ i ].x = ( ( posl.x + posr.x ) * 0.5f );
        m_hpos[ EHHalf ][ i ].z = ( ( posl.z + posr.z ) * 0.5f );
    }
    float box_offset = ( m_face_width * m_icon_ratio * 0.5f );
    float gem_offset = ( box_offset * maths::consts::ROOT2 );
    m_vpos[ EVLowB ] = 0.0f;
    m_vpos[ EVTopH ] = ( m_vpos[ EVLowB ] + m_face_height );
    m_vpos[ EVLowH ] = m_vpos[ EVTopB ] = ( m_vpos[ EVTopH ] - m_face_width );
    m_vpos[ EVCut0 ] = ( m_vpos[ EVTopB ] - m_shim_depth );
    m_vpos[ EVCut1 ] = ( m_vpos[ EVLowH ] + m_shim_depth );
    m_vpos[ EVCut2 ] = ( m_vpos[ EVTopH ] - m_shim_depth );
    m_vpos[ EVRow0 ] = ( m_vpos[ EVLowB ] + ( 1.0f / 8.0f ) );
    m_vpos[ EVRow1 ] = ( m_vpos[ EVRow0 ] + m_band_depth );
    m_vpos[ EVRow2 ] = ( m_vpos[ EVRow1 ] + m_band_depth );
    m_vpos[ EVRow3 ] = ( m_vpos[ EVRow2 ] + m_band_depth );
    m_vpos[ EVRow4 ] = ( m_vpos[ EVRow3 ] + m_band_depth );
    m_vpos[ EVRow5 ] = ( m_vpos[ EVRow4 ] + m_band_depth );
    m_vpos[ EVRow6 ] = ( m_vpos[ EVRow5 ] + m_band_depth );
    m_vpos[ EVRow7 ] = ( m_vpos[ EVRow6 ] + m_band_depth );
    m_vpos[ EVRow8 ] = ( m_vpos[ EVRow7 ] + m_band_depth );
    m_vpos[ EVRow9 ] = ( m_vpos[ EVRow8 ] + m_band_depth );
    m_vpos[ EVFace ] = ( m_vpos[ EVRow9 ] + m_band_depth );
    m_vpos[ EVHalf ] = ( ( m_vpos[ EVLowH ] + m_vpos[ EVTopH ] ) * 0.5f );
    m_vpos[ EVGemB ] = ( m_vpos[ EVHalf ] - gem_offset );
    m_vpos[ EVGemT ] = ( m_vpos[ EVHalf ] + gem_offset );
    m_vpos[ EVBoxB ] = ( m_vpos[ EVHalf ] - box_offset );
    m_vpos[ EVBoxT ] = ( m_vpos[ EVHalf ] + box_offset );
    m_vpos[ EVTriB ] = ( m_vpos[ EVTopB ] - gem_offset );
    m_vpos[ EVTriH ] = ( m_vpos[ EVLowH ] + gem_offset );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Assorted geometry generation functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void constructPlinth8( constructs::LayeringConstruct& construct, const float width, const float height )
{
    float outer_x = ( width * 0.5f );
    float outer_z = ( outer_x * tanf( 22.5f * maths::consts::DEGTORAD ) );
    float inner_x = ( outer_x * ( 3.0f / 4.0f ) );
    float inner_z = ( outer_z * ( 3.0f / 4.0f ) );
    float lower_y = -1.0f;
    float upper_y = ( lower_y + height );
    float ridge_y = ( upper_y + inner_x - outer_x );
    construct.beginStrip();
    construct.addPair( { -outer_x,  lower_y,  outer_z }, { -outer_x,  lower_y, -outer_z } );
    construct.addPair( { -outer_z,  lower_y,  outer_x }, { -outer_z,  lower_y, -outer_x } );
    construct.addPair( {  outer_z,  lower_y,  outer_x }, {  outer_z,  lower_y, -outer_x } );
    construct.addPair( {  outer_x,  lower_y,  outer_z }, {  outer_x,  lower_y, -outer_z } );
    construct.beginStrip();
    construct.addPair( { -inner_x,  upper_y, -inner_z }, { -inner_x,  upper_y,  inner_z } );
    construct.addPair( { -inner_z,  upper_y, -inner_x }, { -inner_z,  upper_y,  inner_x } );
    construct.addPair( {  inner_z,  upper_y, -inner_x }, {  inner_z,  upper_y,  inner_x } );
    construct.addPair( {  inner_x,  upper_y, -inner_z }, {  inner_x,  upper_y,  inner_z } );
    construct.beginStrip();
    construct.addPair( { -outer_x,  lower_y,  outer_z }, { -outer_x,  ridge_y,  outer_z } );
    construct.addPair( { -outer_x,  lower_y, -outer_z }, { -outer_x,  ridge_y, -outer_z } );
    construct.addPair( { -outer_z,  lower_y, -outer_x }, { -outer_z,  ridge_y, -outer_x } );
    construct.addPair( {  outer_z,  lower_y, -outer_x }, {  outer_z,  ridge_y, -outer_x } );
    construct.addPair( {  outer_x,  lower_y, -outer_z }, {  outer_x,  ridge_y, -outer_z } );
    construct.addPair( {  outer_x,  lower_y,  outer_z }, {  outer_x,  ridge_y,  outer_z } );
    construct.addPair( {  outer_z,  lower_y,  outer_x }, {  outer_z,  ridge_y,  outer_x } );
    construct.addPair( { -outer_z,  lower_y,  outer_x }, { -outer_z,  ridge_y,  outer_x } );
    construct.addPair( { -outer_x,  lower_y,  outer_z }, { -outer_x,  ridge_y,  outer_z } );
    construct.beginStrip();
    construct.addPair( { -outer_x,  ridge_y,  outer_z }, { -inner_x,  upper_y,  inner_z } );
    construct.addPair( { -outer_x,  ridge_y, -outer_z }, { -inner_x,  upper_y, -inner_z } );
    construct.addPair( { -outer_z,  ridge_y, -outer_x }, { -inner_z,  upper_y, -inner_x } );
    construct.addPair( {  outer_z,  ridge_y, -outer_x }, {  inner_z,  upper_y, -inner_x } );
    construct.addPair( {  outer_x,  ridge_y, -outer_z }, {  inner_x,  upper_y, -inner_z } );
    construct.addPair( {  outer_x,  ridge_y,  outer_z }, {  inner_x,  upper_y,  inner_z } );
    construct.addPair( {  outer_z,  ridge_y,  outer_x }, {  inner_z,  upper_y,  inner_x } );
    construct.addPair( { -outer_z,  ridge_y,  outer_x }, { -inner_z,  upper_y,  inner_x } );
    construct.addPair( { -outer_x,  ridge_y,  outer_z }, { -inner_x,  upper_y,  inner_z } );
}

void constructCubeObject( constructs::LayeringConstruct& construct )
{
    construct.addQuad( {  1.0f,  1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f } );    //  +ve x
    construct.addQuad( { -1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f,  1.0f } );    //  -ve x
    construct.addQuad( {  1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f } );    //  +ve y
    construct.addQuad( {  1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f, -1.0f } );    //  -ve y
    construct.addQuad( { -1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f } );    //  +ve z
    construct.addQuad( { -1.0f, -1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f } );    //  -ve z
}

void constructCubeVolume( constructs::LayeringConstruct& construct )
{
    construct.addQuad( {  1.0f, -1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f } );    //  +ve x
    construct.addQuad( { -1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f, -1.0f } );    //  -ve x
    construct.addQuad( { -1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f } );    //  +ve y
    construct.addQuad( {  1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f,  1.0f } );    //  -ve y
    construct.addQuad( {  1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f } );    //  +ve z
    construct.addQuad( { -1.0f,  1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f } );    //  -ve z
}

void constructMultiColourCubeVolume( constructs::LayeringConstruct& construct )
{
    construct.addQuad( {  1.0f, -1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, getStandardColour( EColourEmerald, 2 ) );        //  +ve x
    construct.addQuad( { -1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f, -1.0f }, getStandardColour( EColourCyan, 2 ) );           //  -ve x
    construct.addQuad( { -1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }, getStandardColour( EColourPastelPurple, 2 ) );   //  +ve y
    construct.addQuad( {  1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f,  1.0f }, getStandardColour( EColourMustard, 2 ) );        //  -ve y
    construct.addQuad( {  1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f }, getStandardColour( EColourTeal, 2 ) );           //  +ve z
    construct.addQuad( { -1.0f,  1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, getStandardColour( EColourCerise, 0 ) );         //  -ve z
}

void constructTempleRoom( constructs::LayeringConstruct& construct )
{
    const float sky_y = ( 1.0f / 4.0f );
    const float plinth_outer = ( 16.0f / 256.0f );
    const float plinth_step_y = ( 1.0f / 1024.0f );
    const float sanctum_outer = ( 12.0f / 256.0f );
    const float sanctum_court = ( 11.0f / 256.0f );
    const float sanctum_inner = ( 8.0f / 256.0f );
    const float sanctum_step_y = ( 2.0f / 1024.0f );
    const float sanctum_dome_y = ( 11.0f / 256.0f );
    const float sanctum_arch_y = ( 10.0f / 256.0f );
    const float sanctum_roof_y = ( 12.0f / 256.0f );

//  set usage to the sky shader:
    construct.setUsage( EConstructUsageSky );

//  chamber walls and ceiling:
    construct.setParams( 0 );
    construct.addQuad( {  1.0f,  0.0f,  1.0f }, {  1.0f, sky_y,  1.0f }, {  1.0f, sky_y, -1.0f }, {  1.0f,  0.0f, -1.0f } );    //  +ve x
    construct.addQuad( { -1.0f, sky_y, -1.0f }, { -1.0f, sky_y,  1.0f }, { -1.0f,  0.0f,  1.0f }, { -1.0f,  0.0f, -1.0f } );    //  -ve x
    construct.addQuad( {  1.0f,  0.0f,  1.0f }, { -1.0f,  0.0f,  1.0f }, { -1.0f, sky_y,  1.0f }, {  1.0f, sky_y,  1.0f } );    //  +ve z
    construct.addQuad( { -1.0f, sky_y, -1.0f }, { -1.0f,  0.0f, -1.0f }, {  1.0f,  0.0f, -1.0f }, {  1.0f, sky_y, -1.0f } );    //  -ve z
    construct.addQuad( { -1.0f, sky_y, -1.0f }, {  1.0f, sky_y, -1.0f }, {  1.0f, sky_y,  1.0f }, { -1.0f, sky_y,  1.0f } );    //  +ve y

//  set usage to the block shader:
    construct.setUsage( EConstructUsageWalls );

//  chamber floor:
    construct.setParams( getStandardColour( EColourPaleGreen, 2 ) );
    construct.beginStrip();
    construct.addVert( {          1.0f,  0.0f,          1.0f } );
    construct.addVert( {  plinth_outer,  0.0f,  plinth_outer } );
    construct.addVert( {         -1.0f,  0.0f,          1.0f } );
    construct.addVert( { -plinth_outer,  0.0f,  plinth_outer } );
    construct.addVert( {         -1.0f,  0.0f,         -1.0f } );
    construct.addVert( { -plinth_outer,  0.0f, -plinth_outer } );
    construct.addVert( {          1.0f,  0.0f,         -1.0f } );
    construct.addVert( {  plinth_outer,  0.0f, -plinth_outer } );
    construct.addVert( {          1.0f,  0.0f,          1.0f } );
    construct.addVert( {  plinth_outer,  0.0f,  plinth_outer } );

//  sanctum and plinth colour:
    construct.setParams( getStandardColour( EColourGrey, 2 ) );

//  sanctum roof:
    construct.addQuad( {  sanctum_outer,  sanctum_roof_y,  sanctum_outer }, {  sanctum_outer,  sanctum_roof_y, -sanctum_outer }, { -sanctum_outer,  sanctum_roof_y, -sanctum_outer }, { -sanctum_outer,  sanctum_roof_y,  sanctum_outer } );

//  sanctum outer walls:
    construct.beginStrip();
    construct.addVert( { -sanctum_outer,   plinth_step_y,  sanctum_outer } );
    construct.addVert( { -sanctum_inner,   plinth_step_y,  sanctum_outer } );
    construct.addVert( { -sanctum_outer,  sanctum_roof_y,  sanctum_outer } );
    construct.addVert( { -sanctum_inner,  sanctum_arch_y,  sanctum_outer } );
    construct.addVert( {  sanctum_outer,  sanctum_roof_y,  sanctum_outer } );
    construct.addVert( {  sanctum_inner,  sanctum_arch_y,  sanctum_outer } );
    construct.addVert( {  sanctum_outer,   plinth_step_y,  sanctum_outer } );
    construct.addVert( {  sanctum_inner,   plinth_step_y,  sanctum_outer } );
    construct.beginStrip();
    construct.addVert( {  sanctum_outer,   plinth_step_y, -sanctum_outer } );
    construct.addVert( {  sanctum_inner,   plinth_step_y, -sanctum_outer } );
    construct.addVert( {  sanctum_outer,  sanctum_roof_y, -sanctum_outer } );
    construct.addVert( {  sanctum_inner,  sanctum_arch_y, -sanctum_outer } );
    construct.addVert( { -sanctum_outer,  sanctum_roof_y, -sanctum_outer } );
    construct.addVert( { -sanctum_inner,  sanctum_arch_y, -sanctum_outer } );
    construct.addVert( { -sanctum_outer,   plinth_step_y, -sanctum_outer } );
    construct.addVert( { -sanctum_inner,   plinth_step_y, -sanctum_outer } );
    construct.beginStrip();
    construct.addVert( { -sanctum_outer,   plinth_step_y, -sanctum_outer } );
    construct.addVert( { -sanctum_outer,   plinth_step_y, -sanctum_inner } );
    construct.addVert( { -sanctum_outer,  sanctum_roof_y, -sanctum_outer } );
    construct.addVert( { -sanctum_outer,  sanctum_arch_y, -sanctum_inner } );
    construct.addVert( { -sanctum_outer,  sanctum_roof_y,  sanctum_outer } );
    construct.addVert( { -sanctum_outer,  sanctum_arch_y,  sanctum_inner } );
    construct.addVert( { -sanctum_outer,   plinth_step_y,  sanctum_outer } );
    construct.addVert( { -sanctum_outer,   plinth_step_y,  sanctum_inner } );
    construct.beginStrip();
    construct.addVert( {  sanctum_outer,   plinth_step_y,  sanctum_outer } );
    construct.addVert( {  sanctum_outer,   plinth_step_y,  sanctum_inner } );
    construct.addVert( {  sanctum_outer,  sanctum_roof_y,  sanctum_outer } );
    construct.addVert( {  sanctum_outer,  sanctum_arch_y,  sanctum_inner } );
    construct.addVert( {  sanctum_outer,  sanctum_roof_y, -sanctum_outer } );
    construct.addVert( {  sanctum_outer,  sanctum_arch_y, -sanctum_inner } );
    construct.addVert( {  sanctum_outer,   plinth_step_y, -sanctum_outer } );
    construct.addVert( {  sanctum_outer,   plinth_step_y, -sanctum_inner } );

//  sanctum pillar walls:
    construct.beginFan();
    construct.addVert( {  sanctum_court,  sanctum_step_y,  sanctum_inner } );
    construct.addVert( {  sanctum_inner,  sanctum_step_y,  sanctum_inner } );
    construct.addVert( {  sanctum_inner,  sanctum_arch_y,  sanctum_inner } );
    construct.addVert( {  sanctum_outer,  sanctum_arch_y,  sanctum_inner } );
    construct.addVert( {  sanctum_outer,   plinth_step_y,  sanctum_inner } );
    construct.addVert( {  sanctum_court,   plinth_step_y,  sanctum_inner } );
    construct.beginFan();
    construct.addVert( {  sanctum_inner,  sanctum_step_y,  sanctum_court } );
    construct.addVert( {  sanctum_inner,   plinth_step_y,  sanctum_court } );
    construct.addVert( {  sanctum_inner,   plinth_step_y,  sanctum_outer } );
    construct.addVert( {  sanctum_inner,  sanctum_arch_y,  sanctum_outer } );
    construct.addVert( {  sanctum_inner,  sanctum_arch_y,  sanctum_inner } );
    construct.addVert( {  sanctum_inner,  sanctum_step_y,  sanctum_inner } );
    construct.beginFan();
    construct.addVert( { -sanctum_court,  sanctum_step_y, -sanctum_inner } );
    construct.addVert( { -sanctum_inner,  sanctum_step_y, -sanctum_inner } );
    construct.addVert( { -sanctum_inner,  sanctum_arch_y, -sanctum_inner } );
    construct.addVert( { -sanctum_outer,  sanctum_arch_y, -sanctum_inner } );
    construct.addVert( { -sanctum_outer,   plinth_step_y, -sanctum_inner } );
    construct.addVert( { -sanctum_court,   plinth_step_y, -sanctum_inner } );
    construct.beginFan();
    construct.addVert( { -sanctum_inner,  sanctum_step_y, -sanctum_court } );
    construct.addVert( { -sanctum_inner,   plinth_step_y, -sanctum_court } );
    construct.addVert( { -sanctum_inner,   plinth_step_y, -sanctum_outer } );
    construct.addVert( { -sanctum_inner,  sanctum_arch_y, -sanctum_outer } );
    construct.addVert( { -sanctum_inner,  sanctum_arch_y, -sanctum_inner } );
    construct.addVert( { -sanctum_inner,  sanctum_step_y, -sanctum_inner } );
    construct.beginFan();
    construct.addVert( { -sanctum_court,  sanctum_step_y,  sanctum_inner } );
    construct.addVert( { -sanctum_court,   plinth_step_y,  sanctum_inner } );
    construct.addVert( { -sanctum_outer,   plinth_step_y,  sanctum_inner } );
    construct.addVert( { -sanctum_outer,  sanctum_arch_y,  sanctum_inner } );
    construct.addVert( { -sanctum_inner,  sanctum_arch_y,  sanctum_inner } );
    construct.addVert( { -sanctum_inner,  sanctum_step_y,  sanctum_inner } );
    construct.beginFan();
    construct.addVert( { -sanctum_inner,  sanctum_step_y,  sanctum_court } );
    construct.addVert( { -sanctum_inner,  sanctum_step_y,  sanctum_inner } );
    construct.addVert( { -sanctum_inner,  sanctum_arch_y,  sanctum_inner } );
    construct.addVert( { -sanctum_inner,  sanctum_arch_y,  sanctum_outer } );
    construct.addVert( { -sanctum_inner,   plinth_step_y,  sanctum_outer } );
    construct.addVert( { -sanctum_inner,   plinth_step_y,  sanctum_court } );
    construct.beginFan();
    construct.addVert( {  sanctum_court,  sanctum_step_y, -sanctum_inner } );
    construct.addVert( {  sanctum_court,   plinth_step_y, -sanctum_inner } );
    construct.addVert( {  sanctum_outer,   plinth_step_y, -sanctum_inner } );
    construct.addVert( {  sanctum_outer,  sanctum_arch_y, -sanctum_inner } );
    construct.addVert( {  sanctum_inner,  sanctum_arch_y, -sanctum_inner } );
    construct.addVert( {  sanctum_inner,  sanctum_step_y, -sanctum_inner } );
    construct.beginFan();
    construct.addVert( {  sanctum_inner,  sanctum_step_y, -sanctum_court } );
    construct.addVert( {  sanctum_inner,  sanctum_step_y, -sanctum_inner } );
    construct.addVert( {  sanctum_inner,  sanctum_arch_y, -sanctum_inner } );
    construct.addVert( {  sanctum_inner,  sanctum_arch_y, -sanctum_outer } );
    construct.addVert( {  sanctum_inner,   plinth_step_y, -sanctum_outer } );
    construct.addVert( {  sanctum_inner,   plinth_step_y, -sanctum_court } );

//  sanctum arch ceilings:
    construct.addQuad( {  sanctum_inner, sanctum_arch_y, -sanctum_inner }, {  sanctum_outer, sanctum_arch_y, -sanctum_inner }, {  sanctum_outer, sanctum_arch_y,  sanctum_inner }, {  sanctum_inner, sanctum_arch_y,  sanctum_inner } );
    construct.addQuad( { -sanctum_outer, sanctum_arch_y, -sanctum_inner }, { -sanctum_inner, sanctum_arch_y, -sanctum_inner }, { -sanctum_inner, sanctum_arch_y,  sanctum_inner }, { -sanctum_outer, sanctum_arch_y,  sanctum_inner } );
    construct.addQuad( { -sanctum_inner, sanctum_arch_y,  sanctum_inner }, {  sanctum_inner, sanctum_arch_y,  sanctum_inner }, {  sanctum_inner, sanctum_arch_y,  sanctum_outer }, { -sanctum_inner, sanctum_arch_y,  sanctum_outer } );
    construct.addQuad( { -sanctum_inner, sanctum_arch_y, -sanctum_outer }, {  sanctum_inner, sanctum_arch_y, -sanctum_outer }, {  sanctum_inner, sanctum_arch_y, -sanctum_inner }, { -sanctum_inner, sanctum_arch_y, -sanctum_inner } );

//  sanctum dome ceiling:
    construct.addQuad( { -sanctum_inner, sanctum_dome_y, -sanctum_inner }, {  sanctum_inner, sanctum_dome_y, -sanctum_inner }, {  sanctum_inner, sanctum_dome_y,  sanctum_inner }, { -sanctum_inner, sanctum_dome_y,  sanctum_inner } );

//  sanctum dome walls:
    construct.addQuad( {  sanctum_inner, sanctum_arch_y,  sanctum_inner }, {  sanctum_inner, sanctum_dome_y,  sanctum_inner }, {  sanctum_inner, sanctum_dome_y, -sanctum_inner }, {  sanctum_inner, sanctum_arch_y, -sanctum_inner } );
    construct.addQuad( { -sanctum_inner, sanctum_dome_y, -sanctum_inner }, { -sanctum_inner, sanctum_dome_y,  sanctum_inner }, { -sanctum_inner, sanctum_arch_y,  sanctum_inner }, { -sanctum_inner, sanctum_arch_y, -sanctum_inner } );
    construct.addQuad( {  sanctum_inner, sanctum_arch_y,  sanctum_inner }, { -sanctum_inner, sanctum_arch_y,  sanctum_inner }, { -sanctum_inner, sanctum_dome_y,  sanctum_inner }, {  sanctum_inner, sanctum_dome_y,  sanctum_inner } );
    construct.addQuad( { -sanctum_inner, sanctum_dome_y, -sanctum_inner }, { -sanctum_inner, sanctum_arch_y, -sanctum_inner }, {  sanctum_inner, sanctum_arch_y, -sanctum_inner }, {  sanctum_inner, sanctum_dome_y, -sanctum_inner } );

//  sanctum step sides:
    construct.addQuad( {  sanctum_court,  sanctum_step_y,  sanctum_inner }, {  sanctum_court,   plinth_step_y,  sanctum_inner }, {  sanctum_court,   plinth_step_y, -sanctum_inner }, {  sanctum_court,  sanctum_step_y, -sanctum_inner } );
    construct.addQuad( { -sanctum_court,  sanctum_step_y,  sanctum_inner }, { -sanctum_court,  sanctum_step_y, -sanctum_inner }, { -sanctum_court,   plinth_step_y, -sanctum_inner }, { -sanctum_court,   plinth_step_y,  sanctum_inner } );
    construct.addQuad( { -sanctum_inner,   plinth_step_y,  sanctum_court }, {  sanctum_inner,   plinth_step_y,  sanctum_court }, {  sanctum_inner,  sanctum_step_y,  sanctum_court }, { -sanctum_inner,  sanctum_step_y,  sanctum_court } );
    construct.addQuad( { -sanctum_inner,   plinth_step_y, -sanctum_court }, { -sanctum_inner,  sanctum_step_y, -sanctum_court }, {  sanctum_inner,  sanctum_step_y, -sanctum_court }, {  sanctum_inner,   plinth_step_y, -sanctum_court } );

//  sanctum step floor:
    construct.addQuad( {  sanctum_inner,  sanctum_step_y,  sanctum_inner }, {  sanctum_inner,  sanctum_step_y, -sanctum_inner }, { -sanctum_inner,  sanctum_step_y, -sanctum_inner }, { -sanctum_inner,  sanctum_step_y,  sanctum_inner } );
    construct.addQuad( {  sanctum_court,  sanctum_step_y,  sanctum_inner }, {  sanctum_court,  sanctum_step_y, -sanctum_inner }, {  sanctum_inner,  sanctum_step_y, -sanctum_inner }, {  sanctum_inner,  sanctum_step_y,  sanctum_inner } );
    construct.addQuad( { -sanctum_inner,  sanctum_step_y,  sanctum_inner }, { -sanctum_inner,  sanctum_step_y, -sanctum_inner }, { -sanctum_court,  sanctum_step_y, -sanctum_inner }, { -sanctum_court,  sanctum_step_y,  sanctum_inner } );
    construct.addQuad( {  sanctum_inner,  sanctum_step_y,  sanctum_court }, {  sanctum_inner,  sanctum_step_y,  sanctum_inner }, { -sanctum_inner,  sanctum_step_y,  sanctum_inner }, { -sanctum_inner,  sanctum_step_y,  sanctum_court } );
    construct.addQuad( {  sanctum_inner,  sanctum_step_y, -sanctum_inner }, {  sanctum_inner,  sanctum_step_y, -sanctum_court }, { -sanctum_inner,  sanctum_step_y, -sanctum_court }, { -sanctum_inner,  sanctum_step_y, -sanctum_inner } );

//  plinth step sides:
    construct.addQuad( {  plinth_outer,  plinth_step_y,  plinth_outer }, {  plinth_outer,           0.0f,  plinth_outer }, {  plinth_outer,           0.0f, -plinth_outer }, {  plinth_outer,  plinth_step_y, -plinth_outer } );
    construct.addQuad( { -plinth_outer,  plinth_step_y,  plinth_outer }, { -plinth_outer,  plinth_step_y, -plinth_outer }, { -plinth_outer,           0.0f, -plinth_outer }, { -plinth_outer,           0.0f,  plinth_outer } );
    construct.addQuad( { -plinth_outer,           0.0f,  plinth_outer }, {  plinth_outer,           0.0f,  plinth_outer }, {  plinth_outer,  plinth_step_y,  plinth_outer }, { -plinth_outer,  plinth_step_y,  plinth_outer } );
    construct.addQuad( { -plinth_outer,           0.0f, -plinth_outer }, { -plinth_outer,  plinth_step_y, -plinth_outer }, {  plinth_outer,  plinth_step_y, -plinth_outer }, {  plinth_outer,           0.0f, -plinth_outer } );

//  plinth step floor:
    construct.addQuad( {  sanctum_outer,  plinth_step_y,  sanctum_inner }, {  sanctum_outer,  plinth_step_y, -sanctum_inner }, {  sanctum_court,  plinth_step_y, -sanctum_inner }, {  sanctum_court,  plinth_step_y,  sanctum_inner } );
    construct.addQuad( { -sanctum_court,  plinth_step_y,  sanctum_inner }, { -sanctum_court,  plinth_step_y, -sanctum_inner }, { -sanctum_outer,  plinth_step_y, -sanctum_inner }, { -sanctum_outer,  plinth_step_y,  sanctum_inner } );
    construct.addQuad( {  sanctum_inner,  plinth_step_y,  sanctum_outer }, {  sanctum_inner,  plinth_step_y,  sanctum_court }, { -sanctum_inner,  plinth_step_y,  sanctum_court }, { -sanctum_inner,  plinth_step_y,  sanctum_outer } );
    construct.addQuad( {  sanctum_inner,  plinth_step_y, -sanctum_court }, {  sanctum_inner,  plinth_step_y, -sanctum_outer }, { -sanctum_inner,  plinth_step_y, -sanctum_outer }, { -sanctum_inner,  plinth_step_y, -sanctum_court } );
    construct.addQuad( {   plinth_outer,  plinth_step_y,   plinth_outer }, {   plinth_outer,  plinth_step_y,  -plinth_outer }, {  sanctum_outer,  plinth_step_y, -sanctum_inner }, {  sanctum_outer,  plinth_step_y,  sanctum_inner } );
    construct.addQuad( { -sanctum_outer,  plinth_step_y,  sanctum_inner }, { -sanctum_outer,  plinth_step_y, -sanctum_inner }, {  -plinth_outer,  plinth_step_y,  -plinth_outer }, {  -plinth_outer,  plinth_step_y,   plinth_outer } );
    construct.addQuad( {   plinth_outer,  plinth_step_y,   plinth_outer }, {  sanctum_inner,  plinth_step_y,  sanctum_outer }, { -sanctum_inner,  plinth_step_y,  sanctum_outer }, {  -plinth_outer,  plinth_step_y,   plinth_outer } );
    construct.addQuad( {  sanctum_inner,  plinth_step_y, -sanctum_outer }, {   plinth_outer,  plinth_step_y,  -plinth_outer }, {  -plinth_outer,  plinth_step_y,  -plinth_outer }, { -sanctum_inner,  plinth_step_y, -sanctum_outer } );
    construct.beginFan();
    construct.addVert( {  sanctum_outer,  plinth_step_y,  sanctum_outer } );
    construct.addVert( {  sanctum_inner,  plinth_step_y,  sanctum_outer } );
    construct.addVert( {   plinth_outer,  plinth_step_y,   plinth_outer } );
    construct.addVert( {  sanctum_outer,  plinth_step_y,  sanctum_inner } );
    construct.beginFan();                 
    construct.addVert( { -sanctum_outer,  plinth_step_y, -sanctum_outer } );
    construct.addVert( { -sanctum_inner,  plinth_step_y, -sanctum_outer } );
    construct.addVert( {  -plinth_outer,  plinth_step_y,  -plinth_outer } );
    construct.addVert( { -sanctum_outer,  plinth_step_y, -sanctum_inner } );
    construct.beginFan();                 
    construct.addVert( { -sanctum_outer,  plinth_step_y,  sanctum_outer } );
    construct.addVert( { -sanctum_outer,  plinth_step_y,  sanctum_inner } );
    construct.addVert( {  -plinth_outer,  plinth_step_y,   plinth_outer } );
    construct.addVert( { -sanctum_inner,  plinth_step_y,  sanctum_outer } );
    construct.beginFan();                 
    construct.addVert( {  sanctum_outer,  plinth_step_y, -sanctum_outer } );
    construct.addVert( {  sanctum_outer,  plinth_step_y, -sanctum_inner } );
    construct.addVert( {   plinth_outer,  plinth_step_y,  -plinth_outer } );
    construct.addVert( {  sanctum_inner,  plinth_step_y, -sanctum_outer } );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Item geometry construction functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void constructItem( const EBuildItemID id, constructs::LayeringConstruct& construct )
{
    switch( id )
    {
        case( EBuildItemDoor ):
        {
            const float width = 18;
            const float height = 34;
            const float outerL = ( 0.0f - ( width * 0.5f ) );
            const float outerR = ( width * 0.5f );
            const float outerT = ( height * 0.5f );
            const float outerB = ( 0.0f - ( height * 0.5f ) );
            const float innerL = ( outerL + 1.0f );
            const float innerR = ( outerR - 1.0f );
            const float innerT = ( outerT - 1.0f );
            const float innerB = ( outerB + 1.0f );
            const float front = -1.0f;
            const float back = 0.0f;
            construct.setUsage( EConstructUsageObject );
            construct.setParams( getStandardColour( EColourViolet, 0 ) );
            construct.setScale( 1.0f / 16.0f );
            construct.setOffset( { 0, ( 17.0f / 16.0f ), 0 } );
            construct.addQuad( { innerL, innerB, front }, { innerR, innerB, front }, { outerR, outerB, front }, { outerL, outerB, front } );
            construct.addQuad( { outerL, outerT, front }, { outerR, outerT, front }, { innerR, innerT, front }, { innerL, innerT, front } );
            construct.addQuad( { outerL, outerT, front }, { innerL, innerT, front }, { innerL, innerB, front }, { outerL, outerB, front } );
            construct.addQuad( { innerR, outerT, front }, { outerR, outerT, front }, { outerR, outerB, front }, { innerR, innerB, front } );
            construct.addQuad( { outerL, outerT,  back }, { outerR, outerT,  back }, { outerR, outerT, front }, { outerL, outerT, front } );
            construct.addQuad( { outerL, outerB, front }, { outerR, outerB, front }, { outerR, outerB,  back }, { outerL, outerB,  back } );
            construct.addQuad( { outerL, outerT,  back }, { outerL, outerT, front }, { outerL, outerB, front }, { outerL, outerB,  back } );
            construct.addQuad( { outerR, outerT, front }, { outerR, outerT,  back }, { outerR, outerB,  back }, { outerR, outerB, front } );
            construct.addQuad( { innerL, innerT, front }, { innerR, innerT, front }, { innerR, innerT,  back }, { innerL, innerT,  back } );
            construct.addQuad( { innerL, innerB,  back }, { innerR, innerB,  back }, { innerR, innerB, front }, { innerL, innerB, front } );
            construct.addQuad( { innerL, innerT, front }, { innerL, innerT,  back }, { innerL, innerB,  back }, { innerL, innerB, front } );
            construct.addQuad( { innerR, innerT,  back }, { innerR, innerT, front }, { innerR, innerB, front }, { innerR, innerB,  back } );
            construct.addQuad( { outerR, outerT,  back }, { outerL, outerT,  back }, { outerL, outerB,  back }, { outerR, outerB,  back } );
            construct.addQuad( { innerL, innerT,  back }, { innerR, innerT,  back }, { innerR, innerB,  back }, { innerL, innerB,  back }, getStandardColour( EColourBlack, 0 ) );
            break;
        }
        case( EBuildItemPole ):
        {
            //construct.setUsage( EConstructUsageObject );
            //construct.setScale( { ( 1.0f / 32.0f ), ( 1.0f / 32.0f ), ( 1.0f / 2.0f ) } );
            //construct.setOffset( { 0.0f, 0.0f, 0.0f } );
            //construct.setParams( getStandardColour( EColourGrey, 2 ) );
            //constructCubeObject( construct );

            construct.setUsage( EConstructUsageObject );
            construct.setScale( 1.0f );
            construct.setOffset( { 0.0f, 0.0f, 0.0f } );
            construct.setParams( getStandardColour( EColourGrey, 2 ) );
            ////constructPlinth8( construct, 0.25f, 1.0f );

            //SwitchGen switchGen;
            //switchGen.buildBody( construct );
            //switchGen.buildHead( construct );

            StairGen stairGen;
            stairGen.build( construct );

            //switchGen.buildMono( construct );

            break;
        }
        case( EBuildItemBox ):
        {
            construct.setUsage( EConstructUsageObject );
            construct.setScale( 1.0f );
            construct.setOffset( { 0.0f, 0.0f, 0.0f } );
            construct.setParams( getStandardColour( EColourGrey, 2 ) );
            constructCubeObject( construct );
            break;
        }
        default:
        {
            construct.setUsage( EConstructUsageObject );
            construct.setScale( 1.0f );
            construct.setOffset( { 0.0f, 0.0f, 0.0f } );
            construct.setParams( getStandardColour( EColourGrey, 2 ) );
            constructCubeObject( construct );
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Room geometry construction functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void constructRoom( const EBuildRoomID id, constructs::LayeringConstruct& construct )
{
    switch( id )
    {
        case( EBuildRoomType1 ):
        {
            construct.setUsage( EConstructUsageWalls );
            construct.setScale( { 4.0f, 1.5f, 4.0f } );
            construct.setOffset( { 0.0f, 1.5f, 0.0f } );
            construct.setParams( getStandardColour( EColourGoldenBrown, 2 ) );
            //construct.setParams( getStandardColour( EColourBlack, 0 ) );
            constructCubeVolume( construct );
            break;
        }
        case( EBuildRoomType2 ):
        {
            construct.setUsage( EConstructUsageWalls );
            construct.setScale( { 4.0f, 3.0f, 4.0f } );
            construct.setOffset( { 0.0f, 3.0f, 0.0f } );
            //construct.setParams( getStandardColour( EColourAqua, 2 ) );
            construct.setParams( getStandardColour( EColourBlack, 0 ) );
            constructCubeVolume( construct );
            break;
        }
        case( EBuildRoomType3 ):
        {
            uint32_t colour1 = getStandardColour( EColourWhite, 2 );
            uint32_t colour2 = getStandardColour( EColourPaleStraw, 2 );
            construct.setUsage( EConstructUsageWalls );
            construct.setScale( 1.0f );
            construct.setOffset( { -4.0f, 0.0f, 0.0f } );
            construct.setParams( colour1 );
            construct.addQuad( {  0.0f,  4.0f, -4.0f }, {  0.0f,  4.0f, 12.0f }, {  0.0f,  0.0f, 12.0f }, {  0.0f,  0.0f, -4.0f } );
            construct.addQuad( {  0.0f,  4.0f, -4.0f }, {  8.0f,  4.0f, -4.0f }, {  8.0f,  4.0f, 12.0f }, {  0.0f,  4.0f, 12.0f } );
            construct.addQuad( {  0.0f,  4.0f, -4.0f }, {  0.0f,  0.0f, -4.0f }, {  8.0f,  0.0f, -4.0f }, {  8.0f,  4.0f, -4.0f } );
            construct.setParams( colour2 );
            construct.addQuad( {  6.0f,  0.0f,  2.0f }, {  6.0f,  1.0f,  6.0f }, {  8.0f,  1.0f,  6.0f }, {  8.0f,  0.0f,  2.0f } );
            construct.addQuad( {  6.0f,  1.0f,  6.0f }, {  6.0f,  1.0f, 12.0f }, {  8.0f,  1.0f, 12.0f }, {  8.0f,  1.0f,  6.0f } );
            construct.setParams( colour1 );
            construct.addQuad( {  6.0f,  0.0f, 12.0f }, {  6.0f,  1.0f, 12.0f }, {  6.0f,  1.0f,  6.0f }, {  6.0f,  0.0f,  2.0f } );
            construct.beginFan();
            construct.setParams( colour2 );
            construct.addVert( {  0.0f,  0.0f, -4.0f } );
            construct.addVert( {  0.0f,  0.0f, 12.0f } );
            construct.addVert( {  6.0f,  0.0f, 12.0f } );
            construct.addVert( {  6.0f,  0.0f,  2.0f } );
            construct.addVert( {  8.0f,  0.0f,  2.0f } );
            construct.addVert( {  8.0f,  0.0f, -4.0f } );
            construct.beginFan();
            construct.setParams( colour1 );
            construct.addVert( {  0.0f,  4.0f, 12.0f } );
            construct.addVert( {  8.0f,  4.0f, 12.0f } );
            construct.addVert( {  8.0f,  1.0f, 12.0f } );
            construct.addVert( {  6.0f,  1.0f, 12.0f } );
            construct.addVert( {  6.0f,  0.0f, 12.0f } );
            construct.addVert( {  0.0f,  0.0f, 12.0f } );
            construct.beginFan();
            construct.setParams( colour1 );
            construct.addVert( {  8.0f,  4.0f, -4.0f } );
            construct.addVert( {  8.0f,  0.0f, -4.0f } );
            construct.addVert( {  8.0f,  0.0f,  2.0f } );
            construct.addVert( {  8.0f,  1.0f,  6.0f } );
            construct.addVert( {  8.0f,  1.0f, 12.0f } );
            construct.addVert( {  8.0f,  4.0f, 12.0f } );
            break;
        }
        case( EBuildRoomType4 ):
        {
            construct.setUsage( EConstructUsageWalls );
            construct.setScale( { 12.0f, 4.5f, 5.0f } );
            construct.setOffset( { -4.0f, 4.5f, 0.0f } );
            construct.setParams( getStandardColour( EColourOlive, 2 ) );
            //construct.setParams( getStandardColour( EColourBlack, 0 ) );
            constructCubeVolume( construct );
            break;
        }
        case( EBuildRoomType5 ):
        {
            construct.setUsage( EConstructUsageWalls );
            construct.setScale( { 10.0f, 8.0f, 12.0f } );
            construct.setOffset( { 0.0f, 8.0f, 4.0f } );
            constructMultiColourCubeVolume( construct );
            break;
        }
        case( EBuildRoomType6 ):
        {
            construct.setUsage( EConstructUsageWalls );
            construct.setScale( { 10.0f, 4.0f, 14.0f } );
            construct.setOffset( { 4.0f, 4.0f, -9.0f } );
            construct.setParams( getStandardColour( EColourAzure, 2 ) );
            constructCubeVolume( construct );
            break;
        }
        case( EBuildRoomType7 ):
        {
            construct.setUsage( EConstructUsageWalls );
            construct.setScale( { 1.0f, 1.5f, 16.0f } );
            construct.setOffset( { 0.0f, 1.5f, 0.0f } );
            construct.setParams( getStandardColour( EColourPastelPink, 2 ) );
            constructCubeVolume( construct );
            break;
        }
        case( EBuildRoomType8 ):
        {
            construct.setUsage( EConstructUsageWalls );
            construct.setScale( 128.0f );
            construct.setOffset( { 0.0f, 0.0f, 0.0f } );
            constructTempleRoom( construct );
            break;
        }
        default:
        {
            break;
        }
    }
}

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

