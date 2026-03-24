
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   renderables.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game renderable classes and structures
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

#include "renderables.h"
#include "generators.h"
#include "libs/system/debug/asserts.h"
#include "platforms/dx11/rendering/utils/standard_palette.h"

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
////    DoorRenderer door rendering static member data
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float DoorRenderer::s_frame_radius = ( 1.0f / 32.0f );
float DoorRenderer::s_frame_width = ( 1.0f / 32.0f );
float DoorRenderer::s_frame_depth = ( 1.0f / 64.0f );
float DoorRenderer::s_frame_sill = ( 1.0f / 64.0f );

float DoorRenderer::s_length = 2.0f;
float DoorRenderer::s_inset = ( 1.0f / 32.0f );
float DoorRenderer::s_zbias = ( 1.0f / 1024.0f );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DoorRenderer door rendering class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool DoorRenderer::create( const CoreSystem& system, const maths::vec2* const verts, const uint count )
{
    kill();
    if( init( system, verts, count ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void DoorRenderer::destroy()
{
    if( valid() )
    {
        kill();
    }
}

bool DoorRenderer::init( const CoreSystem& system, const maths::vec2* const verts, const uint count )
{
    if( !initResources( system ) )
    {
        return( false );
    }
    if( !buildGeometry( verts, count ) )
    {
        return( false );
    }
    m_valid = true;
    return( true );
}

void DoorRenderer::kill()
{
    m_valid = false;
    m_system.release();
    m_maskShader.release();
    m_fillShader.release();
    m_trimShader.release();
    m_bindObject.release();
    m_bindViewer.release();
    m_bindLintel.release();
    m_bindDepthState.release();
    m_bindRasterState.release();
    m_bindMaskVertices.release();
    m_bindFillVertices.release();
    m_bindTrimVertices.release();
    m_maskVertexResource.release();
    m_fillVertexResource.release();
    m_trimVertexResource.release();
    m_indexResource.release();
    m_maskVertexBuffer.destroy();
    m_fillVertexBuffer.destroy();
    m_trimVertexBuffer.destroy();
    m_indexBuffer.destroy();
    m_rasterFace.release();
    m_rasterBack.release();
    m_rasterBoth.release();
    m_stencilClip.release();
    m_stencilFlip.release();
    m_stencilFace.release();
    m_stencilMask.release();
    m_stencilSort.release();
    m_stencilBias.release();
    m_stencilKill.release();
    for( uint index = 0; index < EDrawCallCount; ++index )
    {
        m_commands[ index ].release();
    }
}

void DoorRenderer::render( const ResourceConsts& object, const ResourceConsts& viewer, const ResourceConsts& lintel, const UINT stencilRef, const UINT stencilNew, const EDoorRenderPass renderPass ) const
{
    if( valid() )
    {
        if( m_bindObject.bind( object ) && m_bindViewer.bind( viewer ) )
        {
            switch( renderPass )
            {
                case( EDoorBiasFace ):
                {
                    if( m_bindRasterState.bind( m_rasterFace ) )
                    {
                        m_stencilFlip.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilFlip ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawBiasFace1 ] );
                            }
                        }
                        m_stencilBias.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilBias ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawBiasFace2 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawBiasFace3 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorBiasBack ):
                {
                    if( m_bindRasterState.bind( m_rasterFace ) )
                    {
                        m_stencilFlip.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilFlip ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawBiasBack1 ] );
                            }
                        }
                        m_stencilBias.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilBias ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawBiasBack2 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawBiasBack3 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorBiasBoth ):
                {
                    m_stencilFlip.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterBoth ) && m_bindDepthState.bind( m_stencilFlip ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawBiasBoth1 ] );
                        }
                    }
                    m_stencilBias.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilBias ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawBiasBoth2 ] );
                        }
                    }
                    m_stencilKill.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterBoth ) && m_bindDepthState.bind( m_stencilKill ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawBiasBoth3 ] );
                        }
                    }
                    break;
                }
                case( EDoorSortNone ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_fillShader.update() && m_fillShader.submit() )
                        {
                            m_fillShader.execute( m_commands[ EDrawSortNone1 ] );
                        }
                    }
                    break;
                }
                case( EDoorSortFace ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawSortFace1 ] );
                        }
                        if( m_fillShader.update() && m_fillShader.submit() )
                        {
                            m_fillShader.execute( m_commands[ EDrawSortFace2 ] );
                        }
                    }
                    break;
                }
                case( EDoorSortBack ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawSortBack1 ] );
                        }
                        if( m_fillShader.update() && m_fillShader.submit() )
                        {
                            m_fillShader.execute( m_commands[ EDrawSortBack2 ] );
                        }
                    }
                    break;
                }
                case( EDoorSortBoth ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterBoth ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawSortBoth1 ] );
                        }
                    }
                    break;
                }
                case( EDoorSortFrom ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawSortFrom1 ] );
                        }
                    }
                    break;
                }
                case( EDoorViewFace ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawSortFace1 ] );
                        }
                    }
                    break;
                }
                case( EDoorViewBack ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_maskShader.update() && m_maskShader.submit() )
                        {
                            m_maskShader.execute( m_commands[ EDrawSortBack1 ] );
                        }
                    }
                    break;
                }
                case( EDoorMaskFace ):
                {
                    if( m_bindRasterState.bind( m_rasterFace ) )
                    {
                        m_stencilMask.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilMask ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawMaskFace1 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilNew & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawMaskFace2 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorMaskBack ):
                {
                    if( m_bindRasterState.bind( m_rasterFace ) )
                    {
                        m_stencilMask.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilMask ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawMaskBack1 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilNew & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawMaskBack2 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorMaskBoth ):
                {
                    if( m_bindRasterState.bind( m_rasterBoth ) )
                    {
                        m_stencilMask.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilMask ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawMaskBoth1 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilNew & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawMaskBoth2 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorFromFace ):
                {
                    if( m_bindRasterState.bind( m_rasterBoth ) )
                    {
                        m_stencilClip.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilClip ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFromFace1 ] );
                            }
                        }
                        m_stencilFace.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilFace ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFromFace2 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilNew & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFromFace3 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorFromBack ):
                {
                    if( m_bindRasterState.bind( m_rasterBoth ) )
                    {
                        m_stencilClip.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilClip ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFromBack1 ] );
                            }
                        }
                        m_stencilFace.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilFace ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFromBack2 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilNew & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFromBack3 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorFastFace ):
                {
                    if( m_bindRasterState.bind( m_rasterBoth ) )
                    {
                        m_stencilClip.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilClip ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFastFace1 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilNew & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFromFace3 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorFastBack ):
                {
                    if( m_bindRasterState.bind( m_rasterBoth ) )
                    {
                        m_stencilClip.setStencilRef( stencilRef & ~1 );
                        if( m_bindDepthState.bind( m_stencilClip ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFastBack1 ] );
                            }
                        }
                        m_stencilKill.setStencilRef( stencilNew & ~1 );
                        if( m_bindDepthState.bind( m_stencilKill ) )
                        {
                            if( m_maskShader.update() && m_maskShader.submit() )
                            {
                                m_maskShader.execute( m_commands[ EDrawFromBack3 ] );
                            }
                        }
                    }
                    break;
                }
                case( EDoorEdgeFace ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindLintel.bind( lintel ) && m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_trimShader.update() && m_trimShader.submit() )
                        {
                            m_trimShader.execute( m_commands[ EDrawEdgeFace1 ] );
                        }
                    }
                    break;
                }
                case( EDoorEdgeBack ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindLintel.bind( lintel ) && m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_trimShader.update() && m_trimShader.submit() )
                        {
                            m_trimShader.execute( m_commands[ EDrawEdgeBack1 ] );
                        }
                    }
                    break;
                }
                case( EDoorEdgeBoth ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindLintel.bind( lintel ) && m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_trimShader.update() && m_trimShader.submit() )
                        {
                            m_trimShader.execute( m_commands[ EDrawEdgeBoth1 ] );
                        }
                    }
                    break;
                }
                case( EDoorTrimFace ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindLintel.bind( lintel ) && m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_trimShader.update() && m_trimShader.submit() )
                        {
                            m_trimShader.execute( m_commands[ EDrawTrimFace1 ] );
                        }
                    }
                    break;
                }
                case( EDoorTrimBack ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindLintel.bind( lintel ) && m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_trimShader.update() && m_trimShader.submit() )
                        {
                            m_trimShader.execute( m_commands[ EDrawTrimBack1 ] );
                        }
                    }
                    break;
                }
                case( EDoorTrimBoth ):
                {
                    m_stencilSort.setStencilRef( stencilRef & ~1 );
                    if( m_bindLintel.bind( lintel ) && m_bindRasterState.bind( m_rasterFace ) && m_bindDepthState.bind( m_stencilSort ) )
                    {
                        if( m_trimShader.update() && m_trimShader.submit() )
                        {
                            m_trimShader.execute( m_commands[ EDrawTrimBoth1 ] );
                        }
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}

bool DoorRenderer::initResources( const CoreSystem& system )
{
    m_system = system;
    if( !m_system.shaders.create( "mask", m_maskShader ) ||
        !m_system.shaders.create( "fill", m_fillShader ) ||
        !m_system.shaders.create( "trim", m_trimShader ) )
    {   //  a shader was not found
        return( false );
    }
    m_maskShader.bind( system.defaults );
    m_fillShader.bind( system.defaults );
    m_trimShader.bind( system.defaults );
    if( 
        !m_system.shaders.create( "cb_object", m_bindObject ) ||
        !m_system.shaders.create( "cb_viewer", m_bindViewer ) ||
        !m_system.shaders.create( "cb_lintel", m_bindLintel ) ||
        !m_system.shaders.create( "locality_ds", m_bindDepthState ) ||
        !m_system.shaders.create( "locality_rs", m_bindRasterState ) ||
        !m_system.shaders.create( "mask", m_bindMaskVertices ) ||
        !m_system.shaders.create( "fill", m_bindFillVertices ) ||
        !m_system.shaders.create( "trim", m_bindTrimVertices ) )
    {   //  a binding was not found
        return( false );
    }
    if( !m_maskShader.bind( m_bindObject ) || !m_fillShader.bind( m_bindObject ) || !m_trimShader.bind( m_bindObject ) ||
        !m_maskShader.bind( m_bindViewer ) || !m_fillShader.bind( m_bindViewer ) || !m_trimShader.bind( m_bindViewer ) || !m_trimShader.bind( m_bindLintel ) ||
        !m_maskShader.bind( m_bindDepthState ) || !m_fillShader.bind( m_bindDepthState ) || !m_trimShader.bind( m_bindDepthState ) ||
        !m_maskShader.bind( m_bindRasterState ) || !m_fillShader.bind( m_bindRasterState ) || !m_trimShader.bind( m_bindRasterState ) ||
        !m_maskShader.bind( m_bindMaskVertices ) || !m_fillShader.bind( m_bindFillVertices ) || !m_trimShader.bind( m_bindTrimVertices ) )
    {   //  shader binding failed
        return( false );
    }
    if( !m_system.registry.find( "locality_face", m_rasterFace ) ||
        !m_system.registry.find( "locality_back", m_rasterBack ) ||
        !m_system.registry.find( "locality_both", m_rasterBoth ) ||
        !m_system.registry.find( "locality_door_clip", m_stencilClip ) ||
        !m_system.registry.find( "locality_door_flip", m_stencilFlip ) ||
        !m_system.registry.find( "locality_door_face", m_stencilFace ) ||
        !m_system.registry.find( "locality_door_mask", m_stencilMask ) ||
        !m_system.registry.find( "locality_door_sort", m_stencilSort ) ||
        !m_system.registry.find( "locality_door_bias", m_stencilBias ) ||
        !m_system.registry.find( "locality_door_kill", m_stencilKill ) )
    {   //  a required standard resource was not found
        return( false );
    }
    if( !m_system.factory.create( m_maskVertexResource ) || !m_system.factory.create( m_fillVertexResource ) || !m_system.factory.create( m_trimVertexResource ) || !m_system.factory.create( m_indexResource ) )
    {   //  buffer resource creation failed
        return( false );
    }
    if( !m_bindMaskVertices.bind( m_maskVertexResource ) || !m_bindFillVertices.bind( m_fillVertexResource ) || !m_bindTrimVertices.bind( m_trimVertexResource ) )
    {   //  buffer resource binding failed
        return( false );
    }
    for( uint index = 0; index < EDrawCallCount; ++index )
    {
        if( !m_system.factory.create( m_commands[ index ] ) )
        {   //  command resource creation failed
            return( false );
        }
    }
    return( true );
}

bool DoorRenderer::buildGeometry( const maths::vec2* const verts, const uint count )
{
    VertexStreamMask maskVertices;
    VertexStreamFill fillVertices;
    VertexStreamTrim trimVertices;
    IndexStream16 indices;
    uint mask_vertices = ( count * 10 );
    uint fill_vertices = ( count * 2 );
    uint edge_vertices = ( ( count + 1 ) * 10 );
    uint trim_vertices = ( ( count + 1 ) * 8 );
    uint draw_vertices = ( edge_vertices + trim_vertices );
    uint mask_indices = ( ( count - 1 ) * 48 );
    uint fill_indices = ( ( count - 2 ) * 6 );
    uint edge_indices = ( count * 48 );
    uint trim_indices = ( count * 36 );
    uint draw_indices = ( edge_indices + trim_indices );
    maskVertices.init( mask_vertices );
    fillVertices.init( fill_vertices );
    trimVertices.init( draw_vertices );
    indices.init( mask_indices + fill_indices + draw_indices );
    {   //  construct mask vertices
        MASK_VERTEX vertex;
        float behind_inset = s_inset;
        float behind_zbias = s_zbias;
        float before_inset = ( 0.0f - s_inset );
        float before_zbias = ( 0.0f - s_zbias );
        float pw[ 2 ] = { 1.0f, 0.0f };
        float pz[ 5 ] = { behind_inset, behind_zbias, 0.0f, before_zbias, before_inset };
        for( uint i = 0; i < 2; ++i )
        {
            vertex.position.w = pw[ i ];
            for( uint j = 0; j < 5; ++j )
            {
                vertex.position.z = pz[ j ];
                for( uint k = 0; k < count; ++k )
                {
                    vertex.position.x = verts[ k ].x;
                    vertex.position.y = verts[ k ].y;
                    maskVertices.append( vertex );
                }
            }
        }
    }
    {   //  construct fill vertices
        FILL_VERTEX vertex;
        memset( &vertex, 0, sizeof( vertex ) );
        vertex.colour = getStandardColour( EColourBlack, 0 );
        vertex.position.w = 1.0f;
        vertex.normal.w = 1.0f;
        for( uint i = 0; i < 2; ++i )
        {   //  construct face drawing vertices
            vertex.normal.z = ( i ? -1.0f : 1.0f );
            for( uint j = 0; j < count; ++j )
            {
                vertex.position.x = verts[ j ].x;
                vertex.position.y = verts[ j ].y;
                fillVertices.append( vertex );
            }
        }
    }
    {   //  construct trim vertices
        TRIM_VERTEX vertex;
        memset( &vertex, 0, sizeof( vertex ) );
        vertex.colour = getStandardColour( EColourBlack, 0 );
        vertex.position.z = 0.0f;
        vertex.position.w = 1.0f;
        vertex.offset.w = 1.0f;
        maths::vec2 pl, pc, pr, nl, nc, nr, pd;
        float length = 0.0f;
        for( uint i = 0; i < count; ++i )
        {
            uint l = ( i % count );
            uint c = ( ( l + 1 ) % count );
            uint r = ( ( c + 1 ) % count );
            pc.Set( verts[ c ] );
            pr.Set( verts[ r ] );
            pd.SetSub( pr, pc );
            length += pd.Length();
        }
        float rim_t[ 6 ];
        {
            rim_t[ 0 ] = maths::consts::FLOAT_EPSILON;
            rim_t[ 1 ] = sqrtf( ( s_frame_depth * s_frame_depth ) + ( s_frame_sill * s_frame_sill ) );
            rim_t[ 2 ] = ( rim_t[ 1 ] + s_frame_width );
            rim_t[ 3 ] = ( ( rim_t[ 2 ] + s_frame_depth ) * 2.0f );
            rim_t[ 1 ] /= rim_t[ 3 ];
            rim_t[ 2 ] /= rim_t[ 3 ];
            rim_t[ 5 ] = ( 1.0f - rim_t[ 0 ] );
            rim_t[ 4 ] = ( 1.0f - rim_t[ 1 ] );
            rim_t[ 3 ] = ( 1.0f - rim_t[ 2 ] );
            rim_t[ 2 ] = ( 1.0f - rim_t[ 3 ] );
            rim_t[ 1 ] = ( 1.0f - rim_t[ 4 ] );
            rim_t[ 0 ] = ( 1.0f - rim_t[ 5 ] );
        }
        float offset = 0.0f;
        for( uint i = 0; i <= count; ++i )
        {   //  output the edge frame vertices
            maths::vec2 dw, ds;
            uint l = ( i % count );
            uint c = ( ( l + 1 ) % count );
            uint r = ( ( c + 1 ) % count );
            pl.Set( verts[ l ] );
            pc.Set( verts[ c ] );
            pr.Set( verts[ r ] );
            nl.x = ( pl.y - pc.y );
            nl.y = ( pc.x - pl.x );
            nl.Normalize();
            nr.x = ( pc.y - pr.y );
            nr.y = ( pr.x - pc.x );
            nr.Normalize();
            nc.SetAdd( nl, nr );
            nc.Normalize();
            pd.SetSub( pr, pc );
            float unit = ( 1.0f / sqrtf( ( nl.Dot( nr ) * 0.5f ) + 0.5f ) );
            dw.SetMul( nc, ( ( s_frame_width + s_frame_sill ) * unit ) );
            ds.SetMul( nc, ( s_frame_sill * unit ) );
            vertex.config.x = ( offset / length );
            offset += pd.Length();
            vertex.position.x = pc.x;
            vertex.position.y = pc.y;
            vertex.offset.x = 0.0f;
            vertex.offset.y = 0.0f;
            vertex.offset.z = 0.0f;
            vertex.config.y = 0.0f;
            vertex.config.z = 0.0f;
            vertex.config.w = 0.0f;
            trimVertices.append( vertex );                          //  0 = bottom center null
            vertex.config.y = rim_t[ 5 ];
            vertex.config.z = rim_t[ 0 ];
            vertex.config.w = -1.0f;
            trimVertices.append( vertex );                          //  1 = bottom center back
            vertex.config.y = rim_t[ 0 ];
            vertex.config.z = rim_t[ 5 ];
            vertex.config.w = 1.0f;
            trimVertices.append( vertex );                          //  2 = bottom center front
            vertex.offset.x = ds.x;
            vertex.offset.y = ds.y;
            vertex.offset.z = ( 0.0f - s_frame_depth );
            vertex.config.y = rim_t[ 4 ];
            vertex.config.z = rim_t[ 1 ];
            vertex.config.w = -1.0f;
            trimVertices.append( vertex );                          //  3 = bottom back
            vertex.offset.z = s_frame_depth;
            vertex.config.y = rim_t[ 1 ];
            vertex.config.z = rim_t[ 4 ];
            vertex.config.w = 1.0f;
            trimVertices.append( vertex );                          //  4 = bottom front
            vertex.offset.x = dw.x;
            vertex.offset.y = dw.y;
            vertex.offset.z = ( 0.0f - s_frame_depth );
            vertex.config.y = rim_t[ 3 ];
            vertex.config.z = rim_t[ 2 ];
            vertex.config.w = -1.0f;
            trimVertices.append( vertex );                          //  5 = top back
            vertex.offset.z = s_frame_depth;
            vertex.config.y = rim_t[ 2 ];
            vertex.config.z = rim_t[ 3 ];
            vertex.config.w = 1.0f;
            trimVertices.append( vertex );                          //  6 = top front
            vertex.offset.z = 0.0f;
            vertex.config.y = 0.5f;
            vertex.config.z = 0.5f;
            vertex.config.w = 0.0f;
            trimVertices.append( vertex );                          //  7 = top center null
            vertex.config.w = -1.0f;
            trimVertices.append( vertex );                          //  8 = top center back
            vertex.config.w = 1.0f;
            trimVertices.append( vertex );                          //  9 = top center front
        }
        offset = 0.0f;
        vertex.config.w = 1.0f;
        for( uint i = 0; i <= count; ++i )
        {   //  output the trim frame vertices
            maths::vec2 dr;
            uint l = ( i % count );
            uint c = ( ( l + 1 ) % count );
            uint r = ( ( c + 1 ) % count );
            pl.Set( verts[ l ] );
            pc.Set( verts[ c ] );
            pr.Set( verts[ r ] );
            nl.x = ( pl.y - pc.y );
            nl.y = ( pc.x - pl.x );
            nl.Normalize();
            nr.x = ( pc.y - pr.y );
            nr.y = ( pr.x - pc.x );
            nr.Normalize();
            nc.SetAdd( nl, nr );
            nc.Normalize();
            pd.SetSub( pr, pc );
            float unit = ( 1.0f / sqrtf( ( nl.Dot( nr ) * 0.5f ) + 0.5f ) );
            dr.SetMul( nc, ( s_frame_radius * unit ) );
            vertex.config.x = ( offset / length );
            offset += pd.Length();
            vertex.position.x = pc.x;
            vertex.position.y = pc.y;
            vertex.offset.x = 0.0f;
            vertex.offset.y = 0.0f;
            vertex.offset.z = 0.0f;
            vertex.config.y = 0.0f;
            vertex.config.z = 0.0f;
            vertex.config.w = 0.0f;
            trimVertices.append( vertex );                          //  0 = bottom null
            vertex.config.y = rim_t[ 5 ];
            vertex.config.z = rim_t[ 0 ];
            vertex.config.w = -1.0f;
            trimVertices.append( vertex );                          //  1 = bottom back
            vertex.config.y = rim_t[ 0 ];
            vertex.config.z = rim_t[ 5 ];
            vertex.config.w = 1.0f;
            trimVertices.append( vertex );                          //  2 = bottom front
            vertex.offset.x = dr.x;
            vertex.offset.y = dr.y;
            vertex.offset.z = ( 0.0f - s_frame_radius );
            vertex.config.y = 0.75f;
            vertex.config.z = 0.25f;
            vertex.config.w = -1.0f;
            trimVertices.append( vertex );                          //  3 = back
            vertex.offset.z = s_frame_radius;
            vertex.config.y = 0.25f;
            vertex.config.z = 0.75f;
            vertex.config.w = 1.0f;
            trimVertices.append( vertex );                          //  4 = front
            vertex.offset.x = ( dr.x + dr.x );
            vertex.offset.y = ( dr.y + dr.y );
            vertex.offset.z = 0.0f;
            vertex.config.y = 0.5f;
            vertex.config.z = 0.5f;
            vertex.config.w = 0.0f;
            trimVertices.append( vertex );                          //  5 = top null
            vertex.config.w = -1.0f;
            trimVertices.append( vertex );                          //  6 = top back
            vertex.config.w = 1.0f;
            trimVertices.append( vertex );                          //  7 = top front
        }
    }
    IndexedParams indexed[ EDrawCallCount ];
    memset( indexed, 0, sizeof( indexed ) );
    UINT depth_clear_offset = ( count * 5 );
    UINT face_index_count = ( ( count - 2 ) * 3 );
    UINT band_index_count = ( count * 6 );
    indexed[ EDrawBiasFace3 ].indexOffset = depth_clear_offset;                                 //  use depth clearing vertices
    indexed[ EDrawBiasBack3 ].indexOffset = depth_clear_offset;                                 //  use depth clearing vertices
    indexed[ EDrawBiasBoth3 ].indexOffset = depth_clear_offset;                                 //  use depth clearing vertices
    indexed[ EDrawMaskFace2 ].indexOffset = depth_clear_offset;                                 //  use depth clearing vertices
    indexed[ EDrawMaskBack2 ].indexOffset = depth_clear_offset;                                 //  use depth clearing vertices
    indexed[ EDrawMaskBoth2 ].indexOffset = depth_clear_offset;                                 //  use depth clearing vertices
    indexed[ EDrawFromFace3 ].indexOffset = depth_clear_offset;                                 //  use depth clearing vertices
    indexed[ EDrawFromBack3 ].indexOffset = depth_clear_offset;                                 //  use depth clearing vertices
    indexed[ EDrawTrimFace1 ].indexOffset = edge_vertices;                                      //  offset to trim vertices
    indexed[ EDrawTrimBack1 ].indexOffset = edge_vertices;                                      //  offset to trim vertices
    indexed[ EDrawTrimBoth1 ].indexOffset = edge_vertices;                                      //  offset to trim vertices
    indexed[ EDrawBiasFace1 ].indexCount = face_index_count;                                    //  draw single sided front face
    indexed[ EDrawBiasFace2 ].indexCount = ( face_index_count + band_index_count );             //  draw single sided front z-bias cap
    indexed[ EDrawBiasFace3 ].indexCount = face_index_count;                                    //  draw single sided front face
    indexed[ EDrawBiasBack1 ].indexCount = face_index_count;                                    //  draw single sided back face
    indexed[ EDrawBiasBack2 ].indexCount = ( face_index_count + band_index_count );             //  draw single sided back z-bias cap
    indexed[ EDrawBiasBack3 ].indexCount = face_index_count;                                    //  draw single sided back face
    indexed[ EDrawBiasBoth1 ].indexCount = face_index_count;                                    //  draw double sided face
    indexed[ EDrawBiasBoth2 ].indexCount = ( ( face_index_count + band_index_count ) << 1 );    //  draw single sided z-bias box
    indexed[ EDrawBiasBoth3 ].indexCount = face_index_count;                                    //  draw double sided face
    indexed[ EDrawSortNone1 ].indexCount = ( face_index_count << 1 );                           //  draw single sided front and back faces
    indexed[ EDrawSortFace1 ].indexCount = face_index_count;                                    //  draw single sided front face
    indexed[ EDrawSortFace2 ].indexCount = face_index_count;                                    //  draw single sided back face
    indexed[ EDrawSortBack1 ].indexCount = face_index_count;                                    //  draw single sided back face
    indexed[ EDrawSortBack2 ].indexCount = face_index_count;                                    //  draw single sided front face
    indexed[ EDrawSortBoth1 ].indexCount = face_index_count;                                    //  draw double sided faces
    indexed[ EDrawSortFrom1 ].indexCount = ( face_index_count << 1 );                           //  draw single sided front and back faces
    indexed[ EDrawMaskFace1 ].indexCount = face_index_count;                                    //  draw single sided front face
    indexed[ EDrawMaskFace2 ].indexCount = face_index_count;                                    //  draw single sided front face
    indexed[ EDrawMaskBack1 ].indexCount = face_index_count;                                    //  draw single sided back face
    indexed[ EDrawMaskBack2 ].indexCount = face_index_count;                                    //  draw single sided back face
    indexed[ EDrawMaskBoth1 ].indexCount = face_index_count;                                    //  draw double sided face
    indexed[ EDrawMaskBoth2 ].indexCount = face_index_count;                                    //  draw double sided face
    indexed[ EDrawFastFace1 ].indexCount = ( face_index_count + band_index_count );             //  draw double sided front inset cup
    indexed[ EDrawFromFace1 ].indexCount = ( ( face_index_count << 1 ) + band_index_count );    //  draw double sided front inset box
    indexed[ EDrawFromFace2 ].indexCount = face_index_count;                                    //  draw double sided front face
    indexed[ EDrawFromFace3 ].indexCount = ( face_index_count + band_index_count );             //  draw double sided front inset cup
    indexed[ EDrawFastBack1 ].indexCount = ( face_index_count + band_index_count );             //  draw double sided back inset cup
    indexed[ EDrawFromBack1 ].indexCount = ( ( face_index_count << 1 ) + band_index_count );    //  draw double sided back inset box
    indexed[ EDrawFromBack2 ].indexCount = face_index_count;                                    //  draw double sided back face
    indexed[ EDrawFromBack3 ].indexCount = ( face_index_count + band_index_count );             //  draw double sided back inset cup
    indexed[ EDrawEdgeFace1 ].indexCount = ( count * 24 );                                      //  draw door edge frame front face
    indexed[ EDrawEdgeBack1 ].indexCount = ( count * 24 );                                      //  draw door edge frame back face
    indexed[ EDrawEdgeBoth1 ].indexCount = ( count * 36 );                                      //  draw door edge frame both faces
    indexed[ EDrawTrimFace1 ].indexCount = ( count * 18 );                                      //  draw door trim frame front face
    indexed[ EDrawTrimBack1 ].indexCount = ( count * 18 );                                      //  draw door trim frame back face
    indexed[ EDrawTrimBoth1 ].indexCount = ( count * 24 );                                      //  draw door trim frame both faces
    {   //  construct mask indices
        indexed[ EDrawBiasBoth1 ].firstIndexIndex =
        indexed[ EDrawBiasBoth3 ].firstIndexIndex =
        indexed[ EDrawBiasFace1 ].firstIndexIndex =
        indexed[ EDrawBiasFace3 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendFaceIndices( indices, false, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 2 ) );                                            //  origin pointing to before
        indexed[ EDrawBiasBoth2 ].firstIndexIndex =
        indexed[ EDrawBiasBack2 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendBandIndices( indices, false, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 1 ), static_cast< uint16_t >( count * 2 ), 1 );   //  behind_zbias->origin pointing out
        appendFaceIndices( indices,  true, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 1 ) );                                            //  behind_zbias pointing to behind
        indexed[ EDrawBiasFace2 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendFaceIndices( indices, false, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 3 ) );                                            //  before_zbias pointing to before
        appendBandIndices( indices,  true, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 3 ), static_cast< uint16_t >( count * 2 ), 1 );   //  before_zbias->origin pointing out
        indexed[ EDrawBiasBack1 ].firstIndexIndex =
        indexed[ EDrawBiasBack3 ].firstIndexIndex =
        indexed[ EDrawSortFrom1 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendFaceIndices( indices,  true, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 2 ) );                                            //  origin pointing to behind
        indexed[ EDrawSortFace1 ].firstIndexIndex =
        indexed[ EDrawSortBoth1 ].firstIndexIndex = 
        indexed[ EDrawMaskFace1 ].firstIndexIndex =
        indexed[ EDrawMaskFace2 ].firstIndexIndex =
        indexed[ EDrawMaskBoth1 ].firstIndexIndex =
        indexed[ EDrawMaskBoth2 ].firstIndexIndex =
        indexed[ EDrawFromFace1 ].firstIndexIndex =
        indexed[ EDrawFromFace2 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendFaceIndices( indices, false, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 2 ) );                                            //  origin pointing to before
        indexed[ EDrawFastFace1 ].firstIndexIndex =
        indexed[ EDrawFromFace3 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendBandIndices( indices, false, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 0 ), static_cast< uint16_t >( count * 2 ), 1 );   //  behind_inset->origin pointing out
        appendFaceIndices( indices,  true, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 0 ) );                                            //  behind_inset pointing to behind
        indexed[ EDrawFastBack1 ].firstIndexIndex =
        indexed[ EDrawFromBack1 ].firstIndexIndex =
        indexed[ EDrawFromBack3 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendFaceIndices( indices, false, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 4 ) );                                            //  before_inset pointing to before
        appendBandIndices( indices,  true, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 4 ), static_cast< uint16_t >( count * 2 ), 1 );   //  before_inset->origin pointing out
        indexed[ EDrawSortBack1 ].firstIndexIndex = 
        indexed[ EDrawMaskBack1 ].firstIndexIndex =
        indexed[ EDrawMaskBack2 ].firstIndexIndex = 
        indexed[ EDrawFromBack2 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendFaceIndices( indices,  true, static_cast< uint16_t >( count ), static_cast< uint16_t >( count * 2 ) );                                            //  origin pointing to behind
    }
    {   //  construct draw indices
        indexed[ EDrawSortNone1 ].firstIndexIndex =
        indexed[ EDrawSortBack2 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendFaceIndices( indices, false, static_cast< uint16_t >( count ), static_cast< uint16_t >( count *  1 ) );                                           //  origin pointing to before
        indexed[ EDrawSortFace2 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendFaceIndices( indices,  true, static_cast< uint16_t >( count ), static_cast< uint16_t >( count *  0 ) );                                           //  origin pointing to behind
        indexed[ EDrawEdgeBack1 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 0, 7, 10 );            //  0 ( bottom center null ) to 7 ( top center null )
        indexed[ EDrawEdgeBoth1 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 9, 6, 10 );            //  9 ( top center front ) to 6 ( top front )
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 6, 4, 10 );            //  6 ( top front ) to 4 ( bottom front )
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 4, 2, 10 );            //  4 ( bottom front ) to 2 ( bottom center front )
        indexed[ EDrawEdgeFace1 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 1, 3, 10 );            //  1 ( bottom center back ) to 3 ( bottom back )
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 3, 5, 10 );            //  3 ( bottom back ) to 5 ( top back )
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 5, 8, 10 );            //  5 ( top back ) to 8 ( top center back )
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 7, 0, 10 );            //  7 ( top center null ) to 0 ( bottom center null )
        indexed[ EDrawTrimBack1 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 0, 5, 8 );             //  0 ( bottom null ) to 5 ( top null )
        indexed[ EDrawTrimBoth1 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 7, 4, 8 );             //  7 ( top front ) to 4 ( front )
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 4, 2, 8 );             //  4 ( front ) to 2 ( bottom front )
        indexed[ EDrawTrimFace1 ].firstIndexIndex = static_cast< UINT >( indices.getCount() );
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 1, 3, 8 );             //  1 ( bottom back ) to 3 ( back )
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 3, 6, 8 );             //  3 ( back ) to 6 ( top back )
        appendEdgeIndices( indices, false, static_cast< uint16_t >( count ), 5, 0, 8 );             //  5 ( top null ) to 0 ( bottom null )
    }
    PrimDesc primDesc;
    memset( &primDesc, 0, sizeof( primDesc ) );
    primDesc.primType = EPrimTypeTriList;
    for( uint index = 0; index < EDrawCallCount; ++index )
    {
        m_commands[ index ].setPrim( primDesc );
        m_commands[ index ].setDraw( indexed[ index ], m_indexResource );
    }
    if( !m_maskVertexBuffer.create( maskVertices.getCount(), maskVertices.getStride(), maskVertices.getBuffer() ) )
    {
        return( false );
    }
    m_maskVertexResource.setStream( m_maskVertexBuffer.getBuffer() );
    m_maskVertexResource.setOffset( 0 );
    m_maskVertexResource.setStride( m_maskVertexBuffer.getStride() );
    if( !m_fillVertexBuffer.create( fillVertices.getCount(), fillVertices.getStride(), fillVertices.getBuffer() ) )
    {
        return( false );
    }
    m_fillVertexResource.setStream( m_fillVertexBuffer.getBuffer() );
    m_fillVertexResource.setOffset( 0 );
    m_fillVertexResource.setStride( m_fillVertexBuffer.getStride() );
    if( !m_trimVertexBuffer.create( trimVertices.getCount(), trimVertices.getStride(), trimVertices.getBuffer() ) )
    {
        return( false );
    }
    m_trimVertexResource.setStream( m_trimVertexBuffer.getBuffer() );
    m_trimVertexResource.setOffset( 0 );
    m_trimVertexResource.setStride( m_trimVertexBuffer.getStride() );
    if( !m_indexBuffer.create( indices.getCount(), indices.getStride(), indices.getBuffer() ) )
    {
        return( false );
    }
    m_indexResource.setIndices( m_indexBuffer.getBuffer() );
    m_indexResource.setOffset( 0 );
    m_indexResource.setStride( m_indexBuffer.getStride() );
    return( true );
}

void DoorRenderer::appendFaceIndices( IndexStream16& indices, const bool flip, const uint16_t count, const uint16_t start )
{
    uint16_t i[ 3 ] = { start, uint16_t( start + 1 ), uint16_t( start + count - 1 ) };
    uint16_t s = ( flip ? 2 : 1 );
    uint16_t t = ( s ^ 3 );
    for( uint16_t index = 2; index < count; ++index )
    {
        indices.append( i[ 0 ] );
        indices.append( i[ s ] );
        indices.append( i[ t ] );
        if( index & 1 )
        {
            i[ 1 ] = i[ 2 ];
            i[ 2 ] = ( i[ 0 ] - 1 );
        }
        else
        {
            i[ 0 ] = i[ 2 ];
            i[ 2 ] = ( i[ 1 ] + 1 );
        }
    }
}

void DoorRenderer::appendBandIndices( IndexStream16& indices, const bool flip, const uint16_t count, const uint16_t row1, const uint16_t row2, const uint16_t step )
{
    uint16_t last = uint16_t( ( count * step ) - 1 );
    uint16_t i[ 4 ] = { uint16_t( row1 + last ), row1, uint16_t( row2 + last ), row2 };
    uint16_t s = ( flip ? 2 : 1 );
    uint16_t t = ( s ^ 3 );
    for( uint16_t index = 0; index < count; ++index )
    {
        indices.append( i[ 0 ] );
        indices.append( i[ s ] );
        indices.append( i[ t ] );
        indices.append( i[ t ] );
        indices.append( i[ s ] );
        indices.append( i[ 3 ] );
        i[ 1 ] += step;
        i[ 3 ] += step;
        i[ 0 ] = ( i[ 1 ] - 1 );
        i[ 2 ] = ( i[ 3 ] - 1 );
    }
}

void DoorRenderer::appendEdgeIndices( IndexStream16& indices, const bool flip, const uint16_t count, const uint16_t row1, const uint16_t row2, const uint16_t step )
{
    uint16_t i[ 4 ];
    if( flip )
    {
        i[ 2 ] = row2;
        i[ 3 ] = row1;
    }
    else
    {
        i[ 2 ] = row1;
        i[ 3 ] = row2;
    }
    for( uint16_t index = 0; index < count; ++index )
    {
        i[ 0 ] = i[ 2 ];
        i[ 1 ] = i[ 3 ];
        i[ 2 ] += step;
        i[ 3 ] += step;
        indices.append( i[ 0 ] );
        indices.append( i[ 1 ] );
        indices.append( i[ 2 ] );
        indices.append( i[ 2 ] );
        indices.append( i[ 1 ] );
        indices.append( i[ 3 ] );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ItemRenderer item rendering class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ItemRenderer::create( const CoreSystem& system, const constructs::GeometryConstruct& construct )
{
    kill();
    if( init( system, construct ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void ItemRenderer::destroy()
{
    if( valid() )
    {
        kill();
    }
}

bool ItemRenderer::init( const CoreSystem& system, const constructs::GeometryConstruct& construct )
{
    if( !initResources( system ) )
    {
        return( false );
    }
    if( !buildGeometry( construct ) )
    {
        return( false );
    }
    m_valid = true;
    return( true );
}

void ItemRenderer::kill()
{
    m_valid = false;
    m_system.release();
    m_skyShader.release();
    m_drawShader.release();
    m_bindObject.release();
    m_bindViewer.release();
    m_bindDepthState.release();
    m_bindRasterState.release();
    m_bindSkyVertices.release();
    m_bindDrawVertices.release();
    m_skyVertexResource.release();
    m_drawVertexResource.release();
    m_indexResource.release();
    m_skyVertexBuffer.destroy();
    m_drawVertexBuffer.destroy();
    m_indexBuffer.destroy();
    m_rasterFace.release();
    m_rasterBack.release();
    m_rasterBoth.release();
    m_stencilMask.release();
    m_stencilDraw.release();
    m_skyCommand.release();
    m_drawCommand.release();
    m_skyValid = false;
    m_drawValid = false;
}

void ItemRenderer::render( const ResourceConsts& object, const ResourceConsts& viewer, const UINT stencilRef, const bool doubleSided, const bool writeStencil ) const
{
    if( valid() )
    {
        if( m_bindObject.bind( object ) && m_bindViewer.bind( viewer ) )
        {
            m_stencilMask.setStencilRef( stencilRef & ~1 );
            m_stencilDraw.setStencilRef( stencilRef & ~1 );
            if( m_bindDepthState.bind( writeStencil ? m_stencilMask : m_stencilDraw ) && m_bindRasterState.bind( doubleSided ? m_rasterBoth : m_rasterFace ) )
            {
                if( m_skyValid )
                {
                    if( m_skyShader.update() && m_skyShader.submit() )
                    {
                        m_skyShader.execute( m_skyCommand );
                    }
                }
                if( m_drawValid )
                {
                    if( m_drawShader.update() && m_drawShader.submit() )
                    {
                        m_drawShader.execute( m_drawCommand );
                    }
                }
            }
        }
    }
}

bool ItemRenderer::initResources( const CoreSystem& system )
{
    m_system = system;
    if( !m_system.shaders.create( "sky", m_skyShader ) ||
        !m_system.shaders.create( "draw", m_drawShader ) )
    {   //  a shader was not found
        return( false );
    }
    m_skyShader.bind( system.defaults );
    m_drawShader.bind( system.defaults );
    if( 
        !m_system.shaders.create( "cb_object", m_bindObject ) ||
        !m_system.shaders.create( "cb_viewer", m_bindViewer ) ||
        !m_system.shaders.create( "locality_ds", m_bindDepthState ) ||
        !m_system.shaders.create( "locality_rs", m_bindRasterState ) ||
        !m_system.shaders.create( "sky", m_bindSkyVertices ) ||
        !m_system.shaders.create( "draw", m_bindDrawVertices ) )
    {   //  a binding was not found
        return( false );
    }
    if( !m_skyShader.bind( m_bindObject ) || !m_drawShader.bind( m_bindObject ) ||
        !m_skyShader.bind( m_bindViewer ) || !m_drawShader.bind( m_bindViewer ) ||
        !m_skyShader.bind( m_bindDepthState ) || !m_drawShader.bind( m_bindDepthState ) ||
        !m_skyShader.bind( m_bindRasterState ) || !m_drawShader.bind( m_bindRasterState ) ||
        !m_skyShader.bind( m_bindSkyVertices ) || !m_drawShader.bind( m_bindDrawVertices ) )
    {   //  shader binding failed
        return( false );
    }
    if( !m_system.registry.find( "locality_face", m_rasterFace ) ||
        !m_system.registry.find( "locality_back", m_rasterBack ) ||
        !m_system.registry.find( "locality_both", m_rasterBoth ) ||
        !m_system.registry.find( "locality_item_mask", m_stencilMask ) ||
        !m_system.registry.find( "locality_item_draw", m_stencilDraw ) )
    {   //  a required standard resource was not found
        return( false );
    }
    if( !m_system.factory.create( m_skyVertexResource ) || !m_system.factory.create( m_drawVertexResource ) || !m_system.factory.create( m_indexResource ) )
    {   //  buffer resource creation failed
        return( false );
    }
    if( !m_bindSkyVertices.bind( m_skyVertexResource ) || !m_bindDrawVertices.bind( m_drawVertexResource ) )
    {   //  buffer resource binding failed
        return( false );
    }
    if( !m_system.factory.create( m_skyCommand ) || !m_system.factory.create( m_drawCommand ) )
    {   //  command resource creation failed
        return( false );
    }
    return( true );
}

bool ItemRenderer::buildGeometry( const constructs::GeometryConstruct& construct )
{
    m_skyValid = false;
    m_drawValid = false;
    m_indexBuffer.destroy();
    m_skyVertexBuffer.destroy();
    m_drawVertexBuffer.destroy();
    const constructs::GeometrySurfaces& surfaces = construct.getSurfaces();
    if( surfaces.getUsed() > 0 )
    {
        const constructs::GeometryTriangles& triangles = construct.getTriangles();
        const constructs::GeometryDescs& descs = construct.getDescs();
        const constructs::GeometryVerts& verts = construct.getVerts();
        const constructs::GeometryPositions& positions = construct.getPositions();
        const constructs::GeometryPlanes& planes = construct.getPlanes();
        IndexStream16 indexArray;
        IndexedStreamSky skyVertexArray;
        IndexedStreamDraw drawVertexArray;
        indexArray.init();
        skyVertexArray.init();
        drawVertexArray.init();
        uint skyIndex = indexArray.getCount();
        {   //  build sky primitives
            SKY_VERTEX vertex;
            vertex.position.w = 1.0f;
            uint32_t surface_count = surfaces.getUsed();
            for( uint32_t surface_index = 0; surface_index < surface_count; ++surface_index )
            {
                const constructs::GeometrySurface& surface = surfaces.getItem( static_cast< uint >( surface_index ) );
                uint32_t triangle_index = surface.itriangle;
                for( uint32_t triangle_count = surface.triangles; triangle_count; --triangle_count )
                {
                    const constructs::GeometryTriangle& triangle = triangles.getItem( static_cast< uint >( triangle_index ) );
                    if( triangle.usage == EConstructUsageSky )
                    {
                        for( uint32_t vertex_index = 0; vertex_index < 3; ++vertex_index )
                        {
                            const constructs::GeometryDesc& desc = descs.getItem( static_cast< uint >( triangle.idescs[ vertex_index ] ) );
                            const constructs::GeometryVert& vert = verts.getItem( static_cast< uint >( desc.ivert ) );
                            const maths::vec3& position = positions.getItem( static_cast< uint >( vert.iposition ) );
                            vertex.position.x = position.x;
                            vertex.position.y = position.y;
                            vertex.position.z = position.z;
                            indexArray.append( static_cast< uint16_t >( skyVertexArray.insert( vertex ) ) );
                        }
                    }
                    triangle_index = triangle.inext;
                }
            }
        }
        uint skyCount = ( indexArray.getCount() - skyIndex );
        uint drawIndex = indexArray.getCount();
        {   //  build draw primitives
            DRAW_VERTEX vertex;
            vertex.position.w = 1.0f;
            vertex.normal.w = 0.0f;
            vertex.edgesT.w = 0.0f;
            vertex.edgesC.w = 0.0f;
            vertex.edgesA.w = 0.0f;
            uint32_t surface_count = surfaces.getUsed();
            for( uint32_t surface_index = 0; surface_index < surface_count; ++surface_index )
            {
                const constructs::GeometrySurface& surface = surfaces.getItem( static_cast< uint >( surface_index ) );
                const maths::plane& plane = planes.getItem( static_cast< uint >( surface.iplane ) );
                vertex.normal.x = plane.x;
                vertex.normal.y = plane.y;
                vertex.normal.z = plane.z;
                uint32_t triangle_index = surface.itriangle;
                for( uint32_t triangle_count = surface.triangles; triangle_count; --triangle_count )
                {
                    const constructs::GeometryTriangle& triangle = triangles.getItem( static_cast< uint >( triangle_index ) );
                    if( ( triangle.usage == EConstructUsageWalls ) ||
                        ( triangle.usage == EConstructUsageObject ) ||
                        ( triangle.usage == EConstructUsageLinkBody ) ||
                        ( triangle.usage == EConstructUsageLinkHead ) ||
                        ( triangle.usage == EConstructUsageLinkShim ) ||
                        ( triangle.usage == EConstructUsageLinkBand ) ||
                        ( triangle.usage == EConstructUsageLinkIcon ) )
                    {
                        constructs::GeometryOutlining outlining;
                        construct.buildOutlining( outlining, triangle_index, g_usageEdgingConfigs[ triangle.usage ] );
                        for( uint32_t vertex_index = 0; vertex_index < 3; ++vertex_index )
                        {
                            const constructs::GeometryDesc& desc = descs.getItem( static_cast< uint >( triangle.idescs[ vertex_index ] ) );
                            const constructs::GeometryVert& vert = verts.getItem( static_cast< uint >( desc.ivert ) );
                            const maths::vec3& position = positions.getItem( static_cast< uint >( vert.iposition ) );
                            vertex.position.x = position.x;
                            vertex.position.y = position.y;
                            vertex.position.z = position.z;
                            vertex.edgesT.x = outlining.edgesT[ vertex_index ].offset[ 0 ];
                            vertex.edgesT.y = outlining.edgesT[ vertex_index ].offset[ 1 ];
                            vertex.edgesT.z = outlining.edgesT[ vertex_index ].offset[ 2 ];
                            vertex.edgesC.x = outlining.edgesC[ vertex_index ].offset[ 0 ];
                            vertex.edgesC.y = outlining.edgesC[ vertex_index ].offset[ 1 ];
                            vertex.edgesC.z = outlining.edgesC[ vertex_index ].offset[ 2 ];
                            vertex.edgesA.x = outlining.edgesA[ vertex_index ].offset[ 0 ];
                            vertex.edgesA.y = outlining.edgesA[ vertex_index ].offset[ 1 ];
                            vertex.edgesA.z = outlining.edgesA[ vertex_index ].offset[ 2 ];
                            vertex.colour = desc.params;
                            indexArray.append( static_cast< uint16_t >( drawVertexArray.insert( vertex ) ) );
                        }
                    }
                    triangle_index = triangle.inext;
                }
            }
        }
        uint drawCount = ( indexArray.getCount() - drawIndex );
        if( skyCount || drawCount )
        {
            if( m_indexBuffer.create( indexArray.getCount(), indexArray.getStride(), indexArray.getBuffer() ) )
            {   //  index buffer creation succeeded
                m_indexResource.setIndices( reinterpret_cast< void* >( m_indexBuffer.getBuffer() ) );
                m_indexResource.setOffset( 0 );
                m_indexResource.setStride( m_indexBuffer.getStride() );
                bool success = true;
                if( skyCount )
                {
                    if( m_skyVertexBuffer.create( skyVertexArray.getCount(), skyVertexArray.getStride(), skyVertexArray.getBuffer() ) )
                    {   //  sky vertex buffer creation succeeded
                        m_skyVertexResource.setStream( reinterpret_cast< void* >( m_skyVertexBuffer.getBuffer() ) );
                        m_skyVertexResource.setOffset( 0 );
                        m_skyVertexResource.setStride( m_skyVertexBuffer.getStride() );
                        PrimDesc primDesc;
                        memset( &primDesc, 0, sizeof( primDesc ) );
                        primDesc.primType = EPrimTypeTriList;
                        IndexedParams indexed;
                        memset( &indexed, 0, sizeof( indexed ) );
                        indexed.indexCount = static_cast< UINT >( skyCount );
                        indexed.firstIndexIndex = static_cast< UINT >( skyIndex );
                        m_skyCommand.setPrim( primDesc );
                        m_skyCommand.setDraw( indexed, m_indexResource );
                        m_skyValid = true;
                    }
                    else
                    {   //  sky vertex buffer creation failed
                        success = false;
                    }
                }
                if( drawCount )
                {
                    if( m_drawVertexBuffer.create( drawVertexArray.getCount(), drawVertexArray.getStride(), drawVertexArray.getBuffer() ) )
                    {   //  draw vertex buffer creation succeeded
                        m_drawVertexResource.setStream( reinterpret_cast< void* >( m_drawVertexBuffer.getBuffer() ) );
                        m_drawVertexResource.setOffset( 0 );
                        m_drawVertexResource.setStride( m_drawVertexBuffer.getStride() );
                        PrimDesc primDesc;
                        memset( &primDesc, 0, sizeof( primDesc ) );
                        primDesc.primType = EPrimTypeTriList;
                        IndexedParams indexed;
                        memset( &indexed, 0, sizeof( indexed ) );
                        indexed.indexCount = static_cast< UINT >( drawCount );
                        indexed.firstIndexIndex = static_cast< UINT >( drawIndex );
                        m_drawCommand.setPrim( primDesc );
                        m_drawCommand.setDraw( indexed, m_indexResource );
                        m_drawValid = true;
                    }
                    else
                    {   //  draw vertex buffer creation failed
                        success = false;
                    }
                }
                if( success )
                {
                    return( true );
                }
            }
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

