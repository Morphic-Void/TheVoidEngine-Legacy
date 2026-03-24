
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vector_overlay.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Vector overlay class.
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

#include "libs/system/base/types.h"
#include "libs/system/debug/asserts.h"
#include "libs/maths/consts.h"
#include "vector_overlay.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin rendering namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rendering
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Vector overlay class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CVectorOverlay::create( const CoreSystem& system, const DIMS2D& display )
{
    kill();
    if( init( system, display ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void CVectorOverlay::destroy()
{
    if( this )
    {
        kill();
    }
}

bool CVectorOverlay::init( const CoreSystem& system, const DIMS2D& display )
{
    m_enable = 0x0000ffff;
    for( uint index = 0; index < 16; ++index )
    {
        if( !m_layers[ index ].create( system ) )
        {
            return( false );
        }
    }
    if( !system.shaders.create( "cb_scene", m_sceneBinding ) || !system.shaders.create( "cb_instance", m_entityBinding ) )
    {
        return( false );
    }
    if( !system.factory.create( m_sceneResource ) || !system.factory.create( m_entityResource ) )
    {
        return( false );
    }
    m_sceneBinding.bind( m_sceneResource );
    m_entityBinding.bind( m_entityResource );
    for( uint index = 0; index < 16; ++index )
    {
        if( !m_layers[ index ].bind( m_sceneBinding ) || !m_layers[ index ].bind( m_entityBinding ) )
        {
            return( false );
        }
    }
    setup( display );
    m_state = 0;
    m_glide = 0.0f;
    m_lerp = false;
    m_show = true;
    m_valid = true;
    return( true );
}

void CVectorOverlay::kill()
{
    m_valid = false;
    m_enable = 0;
    for( uint index = 0; index < 16; ++index )
    {
        m_layers[ index ].destroy();
    }
    m_display.width = m_display.height = 0;
    m_sceneBinding.release();
    m_sceneResource.release();
    m_sceneBuffer.destroy();
    memset( &m_sceneData, 0, sizeof( m_sceneData ) );
    m_entityBinding.release();
    m_entityResource.release();
    m_entityBuffer.destroy();
    memset( &m_entityData, 0, sizeof( m_entityData ) );
    memset( &m_viewer, 0, sizeof( m_viewer ) );
    m_state = 0;
    m_glide = 0.0f;
    m_lerp = false;
    m_show = false;
}

void CVectorOverlay::setup( const DIMS2D& display )
{
    m_display = display;
    float width = static_cast< float >( m_display.width );
    float height = static_cast< float >( m_display.height );
    float maxAxis = ( width > height ? width : height );
    memset( &m_sceneData, 0, sizeof( m_sceneData ) );
    maths::viewer viewer;
    viewer.SetDefaults();
    viewer.projection.near_clip_z = 1.0f;
    viewer.projection.far_clip_z = ( maxAxis * 4.0f );
    viewer.projection.viewport_width = width;
    viewer.projection.viewport_height = height;
    viewer.projection.viewport_center.z = maxAxis;
    viewer.clipping.SetClipDX();
    viewer.clipping.DepthFlip();
    viewer.clipping.VerticalFlip();
    setTargetConsts( m_sceneData.target, width, height );
    setCameraConsts( m_sceneData.camera, viewer );
    m_sceneBuffer.create( static_cast< UINT >( sizeof( m_sceneData ) ), &m_sceneData );
    m_sceneResource.setBuffer( m_sceneBuffer.getBuffer() );
    m_sceneResource.setOffset( 0 );
    m_sceneResource.setStride( m_sceneBuffer.getStride() );
    memcpy( &m_viewer, &viewer, sizeof( viewer ) );
}

void CVectorOverlay::resize( const DIMS2D& display )
{
    if( valid() )
    {
        setup( display );
    }
}

void CVectorOverlay::update( const float time )
{
    UNUSED( time );
    if( valid() )
    {
        if( m_lerp )
        {
            m_glide += ( time * ( 1.0f / ( 0.5f * 1000.0f ) ) );
            if( m_glide >= 1.0f )
            {
                m_glide = 0.0f;
                m_state = ( ( ( m_state & 4 ) ? ( m_state ^ 1 ) : ( m_state + 1 ) ) & 3 );
                m_lerp = false;
                m_show = ( ( m_state & 1 ) ? false : true );
            }
        }
        if( m_show )
        {
            const float t = ( ( m_state & 1 ) ? ( 1.0f - m_glide ) : m_glide );
            const float s = ( 1.0f - t );
            const float width = m_viewer.projection.viewport_width;
            const float height = m_viewer.projection.viewport_height;
            const float depth = m_viewer.projection.viewport_center.z;
            const float sqrWidth = ( width * width );
            const float sqrDepth = ( depth * depth );
            const float halfWidth = ( width * 0.5f );
            const float halfHeight = ( height * 0.5f );
            const float edgeCos = sqrtf( sqrWidth / ( sqrWidth + ( sqrDepth * 4.0f ) ) );
            const float halfCos = sqrtf( ( edgeCos + 1.0f ) * 0.5f );
            const float halfSin = sqrtf( ( 1.0f - edgeCos ) * 0.5f );
            const float qs = ( halfCos * t );
            const float qc = ( halfSin * t );
            const float qz = ( maths::consts::RCPROOT2 * t );
            maths::joint entity;
            entity.v = { ( 0.5f - halfWidth ), ( 0.5f - halfHeight ), 0.0f, 1.0f };
            maths::quat zRotate = { 0.0f, 0.0f, qz, ( qz + s ) };
            maths::quat yRotate = { 0.0f, qs, 0.0f, ( qc + s ) };
            float pivot = ( ( ( depth - halfWidth ) * t ) + halfWidth + 0.5f );
            if( m_state & 2 )
            {
                zRotate.z = ( 0.0f - zRotate.z );
                yRotate.y = ( 0.0f - yRotate.y );
                pivot = ( 0.0f - pivot );
            }
            zRotate.Normalize();
            yRotate.Normalize();
            entity.v.Mul( zRotate );
            entity.v.x -= pivot;
            entity.v.Mul( yRotate );
            entity.v.x += pivot;
            entity.v.z += depth;
            entity.q.SetMul( zRotate, yRotate );
            setEntityConsts( m_entityData, entity );
            m_entityBuffer.create( static_cast< UINT >( sizeof( m_entityData ) ), &m_entityData );
            m_entityResource.setBuffer( m_entityBuffer.getBuffer() );
            m_entityResource.setOffset( 0 );
            m_entityResource.setStride( m_entityBuffer.getStride() );
        }
        for( uint index = 0; index < 16; ++index )
        {
            m_layers[ index ].update();
        }
    }
}

void CVectorOverlay::render()
{
    if( valid() )
    {
        if( m_show )
        {
            for( uint index = 0; index < 16; ++index )
            {
                if( ( m_enable >> index ) & 1 )
                {
                    m_layers[ index ].render();
                }
            }
        }
    }
}

bool CVectorOverlay::bind( const Binding& binding )
{
    bool ret = false;
    if( valid() )
    {
        ret = true;
        for( uint index = 0; index < 16; ++index )
        {
            if( !m_layers[ index ].bind( binding ) )
            {
                ret = false;
            }
        }
    }
    return( ret );
}

bool CVectorOverlay::unbind( const Binding& binding )
{
    bool ret = false;
    if( valid() )
    {
        ret = true;
        for( uint index = 0; index < 16; ++index )
        {
            if( !m_layers[ index ].unbind( binding ) )
            {
                ret = false;
            }
        }
    }
    return( ret );
}

bool CVectorOverlay::bind( const BindingGroup& bindingGroup )
{
    if( valid() )
    {
        for( uint index = 0; index < 16; ++index )
        {
            m_layers[ index ].bind( bindingGroup );
        }
        return true;
    }
    return false;
}

bool CVectorOverlay::unbind( const BindingGroup& bindingGroup )
{
    if( valid() )
    {
        for( uint index = 0; index < 16; ++index )
        {
            m_layers[ index ].unbind( bindingGroup );
        }
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end rendering namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};  //  namespace rendering

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

