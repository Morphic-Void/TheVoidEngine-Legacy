
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   renderer.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        DirectX 11 renderer class.
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

#include "platforms/windows/stdafx.h"
#include "platforms/windows/timing.h"
#include "libs/system/base/types.h"
#include "libs/system/debug/asserts.h"
#include "libs/maths/consts.h"
#include "libs/maths/vec2.h"
#include "libs/maths/frect.h"
#include "libs/maths/bitops.h"
#include "renderer.h"

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
////    Temporary test strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* g_star_wars[] =
{
    "A long time ago, in a galaxy far,",
    "far away....",
    "",
    "",
    "         Episode IV",
    "",
    "         A NEW HOPE",
    "",
    "It is a period of civil war.",
    "Rebel spaceships, striking",
    "from a hidden base, have won",
    "their first victory against",
    "the evil Galactic Empire.",
    "",
    "During the battle, rebel",
    "spies managed to steal secret",
    "plans to the Empire's",
    "ultimate weapon, the DEATH",
    "STAR, an armored space",
    "station with enough power to",
    "destroy an entire planet.",
    "",
    "Pursued by the Empire's",
    "sinister agents, Princess",
    "Leia races home aboard her",
    "starship, custodian of the",
    "stolen plans that can save",
    "her people and restore",
    "freedom to the galaxy....",
    NULL
};

static const char* g_flash_gordon[] =
{
    "         Foreword",
    "      CHAPTER ELEVEN",
    "",
    "TORCH and Sonja, Ming's",
    "agents, confined in prison",
    "cells in Barin's palace,",
    "escape to the radio room,",
    "over-power the operator and",
    "communicate with Ming.",
    "",
    "Ming instructs Torch to send",
    "out a false order, grounding",
    "Barin's space ships, in order",
    "to clear the way for Ming's",
    "bombers, and to then destroy",
    "the radio to prevent Barin",
    "countermanding the order.",
    "",
    "DALE overhears the order and",
    "is captured by Torch and Sonja,",
    "who flee with her to the roof",
    "of the palace.",
    "",
    "FLASH pursues and corners",
    "them, and is battling with",
    "Torch when Ming's bombers...",
    NULL
};

static const char* g_introduction[] =
{
    "System console",
    "",
    "   Controls:",
    "      pause/break:  toggle console",
    "      ctrl+cursors: position console",
    "      ctrl+pgup:    larger console",
    "      ctrl+pgdn:    smaller console",
    "      alt+pgup:     larger font",
    "      alt+pgdn:     smaller font",
    "      alt+up:       block case font",
    "      alt+down:     mixed case font",
    "",
    "   Commands:",
    "      DENIED: INSUFFICIENT PRIVILEGES",
    "      No access at this terminal",
    "",
    ">",
    NULL
};

static const char* g_controls[] =
{
    "Controls:",
    "",
    "   Keyboard:",
    "      tab:            show/hide these instructions",
    "      esc:            show cursor/exit full-screen",
    "      alt+enter:      toggle full-screen/windowed",
    "      a/s/d/w:        movement",
    "      q/e/r/f:        turn/look",
    "      cursors:        turn/look",
    "      shift:          move faster",
    "      home:           reset to start position",
    "      scroll lock:    toggle collisions and gravity",
    "",
    "   Mouse:",
    "      movement:       look direction",
    "      right button:   move faster with keyboard",
    "",
    "   Microsoft controller:",
    "      left thumb:     movement",
    "      right thumb:    turn/look",
    "      left trigger:   move slower",
    "      right trigger:  move faster",
    "      back:           reset to start position",
    "      start:          toggle collisions and gravity",
    NULL
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DirectX 11 renderer class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CRenderer::create( CSwapChain& swapChain, CDepthStencil& depthStencil, ID3D11Device* const device, ID3D11DeviceContext* const context )
{
    ASSERT( ( device != NULL ) && ( context != NULL ) );
    if( ( device != NULL ) && ( context != NULL ) )
    {
        if( init( swapChain, depthStencil, device, context ) )
        {
            setupScene();
            return( true );
        }
        destroy();
    }
    return( false );
}

bool CRenderer::resize()
{
    if( valid() )
    {
        updatePrimary();
        m_viewer.projection.SetVerticalFoV( ( 50.0f * maths::consts::DEGTORAD ), ( static_cast< float >( m_primary.dimensions.width ) / static_cast< float >( m_primary.dimensions.height ) ) );
        drawTabToggleAndTitle( m_showControls );
        drawControlMappings();
        m_console.resize( m_primary.dimensions );
        m_overlay.resize( m_primary.dimensions );
        m_gameWorld.resize( m_primary );
        return( true );
    }
    return( false );
}

bool CRenderer::update( const float time, const input::CGamePadState& gamePadState, const input::CKeyMouseState& keyMouseState )
{
    if( valid() )
    {
        drawTimingAndLocation();
        if( keyMouseState.keyPressed( input::EVKeyTab ) )
        {
            m_showControls = !m_showControls;
            drawTabToggleAndTitle( m_showControls );
        }
        if( m_showControls )
        {
            m_overlay.enableLayer( 10 );
            m_overlay.enableLayer( 11 );
        }
        else
        {
            m_overlay.disableLayer( 10 );
            m_overlay.disableLayer( 11 );
        }

        m_gameWorld.update( time, gamePadState, keyMouseState );

        static maths::quat obj_rotate = { 0.0f, 0.0f, 0.0f, 1.0f };
        static maths::vec4 obj_locate = { 0.0f, 8.0f, 0.0f, 0.0f };
        //float framesAt60fps = ( time * 0.06f );
        float framesAt60fps = 1.0f;
        obj_rotate.Yaw( framesAt60fps * 0.4f * maths::consts::DEGTORAD );
        obj_rotate.Pitch( framesAt60fps * 0.6f * maths::consts::DEGTORAD );

        m_viewer.object.Set( m_gameWorld.getViewer() );
      
        float width = static_cast< float >( m_primary.dimensions.width );
        float height = static_cast< float >( m_primary.dimensions.height );
        maths::camera camera;
        camera.Set( m_viewer );
        memset( &m_sceneData, 0, sizeof( m_sceneData ) );
        setTargetConsts( m_sceneData.target, width, height );
        setCameraConsts( m_sceneData.camera, camera );

        //m_wireEntityData.rotate.x = m_solidEntityData.rotate.x = obj_rotate.x;
        //m_wireEntityData.rotate.y = m_solidEntityData.rotate.y = obj_rotate.y;
        //m_wireEntityData.rotate.z = m_solidEntityData.rotate.z = obj_rotate.z;
        //m_wireEntityData.rotate.w = m_solidEntityData.rotate.w = obj_rotate.w;
        //m_wireEntityData.locate.x = m_solidEntityData.locate.x = obj_locate.x;
        //m_wireEntityData.locate.y = m_solidEntityData.locate.y = obj_locate.y;
        //m_wireEntityData.locate.z = m_solidEntityData.locate.z = obj_locate.z;

        if( m_scene.consts.resource.valid() )
        {
            UINT size = static_cast< UINT >( sizeof( m_sceneData ) );
            m_scene.buffer.create( size, &m_sceneData );
            m_scene.consts.resource.setBuffer( m_scene.buffer.getBuffer() );
            m_scene.consts.resource.setOffset( 0 );
            m_scene.consts.resource.setStride( size );
        }
        //if( m_gridEntity.consts.resource.valid() )
        //{
        //    UINT size = static_cast< UINT >( sizeof( m_gridEntityData ) );
        //    m_gridEntity.buffer.create( size, &m_gridEntityData );
        //    m_gridEntity.consts.resource.setBuffer( m_gridEntity.buffer.getBuffer() );
        //    m_gridEntity.consts.resource.setOffset( 0 );
        //    m_gridEntity.consts.resource.setStride( size );
        //}
        //if( m_wireEntity.consts.resource.valid() )
        //{
        //    UINT size = static_cast< UINT >( sizeof( m_wireEntityData ) );
        //    m_wireEntity.buffer.create( size, &m_wireEntityData );
        //    m_wireEntity.consts.resource.setBuffer( m_wireEntity.buffer.getBuffer() );
        //    m_wireEntity.consts.resource.setOffset( 0 );
        //    m_wireEntity.consts.resource.setStride( size );
        //}
        //if( m_solidEntity.consts.resource.valid() )
        //{
        //    UINT size = static_cast< UINT >( sizeof( m_solidEntityData ) );
        //    m_solidEntity.buffer.create( size, &m_solidEntityData );
        //    m_solidEntity.consts.resource.setBuffer( m_solidEntity.buffer.getBuffer() );
        //    m_solidEntity.consts.resource.setOffset( 0 );
        //    m_solidEntity.consts.resource.setStride( size );
        //}
        m_console.update( time );
        m_overlay.update( time );
        return( true );
    }
    return( false );
}

bool CRenderer::render()
{
    if( valid() )
    {
        //m_gridWidget.render();
        m_gameWorld.render();
        //m_solidWidget.render();
        //m_wireWidget.render();
        m_primary.primaryRTV.resource.setViewport( m_primary.viewport );
        m_primary.primaryDSV.resource.setViewport( m_primary.viewport );
        m_overlay.render();
        m_console.render();
        return( true );
    }
    return( false );
}

void CRenderer::destroy()
{
    if( valid() )
    {
        kill();
    }
}

void CRenderer::keyInput( const input::CKeyEvent keyEvent )
{
    if( valid() )
    {
        m_console.keyInput( keyEvent );
    }
}

bool CRenderer::init( CSwapChain& swapChain, CDepthStencil& depthStencil, ID3D11Device* const device, ID3D11DeviceContext* const context )
{
    m_swapChain = &swapChain;
    m_depthStencil = &depthStencil;
    if( !createSystem( device, context ) )
    {
        return( false );
    }
    m_scene.create( m_system, "cb_scene" );
    m_gridEntity.create( m_system, "cb_instance" );
    m_wireEntity.create( m_system, "cb_instance" );
    m_solidEntity.create( m_system, "cb_instance" );
    m_system.defaults.bind( m_scene.consts.binding );
    m_system.defaults.bind( m_solidEntity.consts.binding );
    return( true );
}

void CRenderer::kill()
{
    m_solidWidget.destroy();
    m_wireWidget.destroy();
    m_gridWidget.destroy();
    m_solidEntity.destroy();
    m_wireEntity.destroy();
    m_gridEntity.destroy();
    m_scene.destroy();
    removeSystem();
    m_depthStencil = NULL;
    m_swapChain = NULL;
}

void CRenderer::drawTimingAndLocation()
{
//  immediate timing:
    uint64_t cycles = platform_windows::CTiming::getCycles( platform_windows::ETimingUpdate );

//  smoothed timing:
    //static const int smoothingShift = 8;
    //static const int smoothingScale = ( ( 1 << smoothingShift ) - 1 );
    //static uint64_t smoothedCycles = 0;
    //smoothedCycles = ( ( ( ( smoothedCycles + 1 ) * smoothingScale ) + cycles ) >> smoothingShift );
    //cycles = smoothedCycles;

//  averaged timing:
    //static const int averagingShift = 1;
    //static const int averagingSize = ( 1 << averagingShift );
    //static const int averagingMask = ( averagingSize - 1 );
    //static uint64_t averagingBuffer[ averagingSize ] = { 0 };
    //static uint64_t averagingCycles = 0;
    //static int averagingIndex = 0;
    //static int averagingCount = 0;
    //averagingCycles -= averagingBuffer[ averagingIndex ];
    //averagingBuffer[ averagingIndex ] = platform_windows::CTiming::getCycles( platform_windows::ETimingUpdate );
    //averagingCycles += averagingBuffer[ averagingIndex ];
    //averagingIndex = ( ( averagingIndex + 1 ) & averagingMask );
    //averagingCount = ( ( averagingCount < averagingSize ) ? ( averagingCount + 1 ) : averagingSize );
    //cycles = ( ( averagingCycles + averagingCount - 1 ) / averagingCount );

//  convert cycles to milliseconds and frames per second and draw the timing and location:
    float ms = platform_windows::CTiming::getMS( cycles );
    float hz = platform_windows::CTiming::getHZ( cycles );
    drawTimingAndLocation( ms, hz, m_viewer.object.v );
}

void CRenderer::drawTimingAndLocation( const float ms, const float hz, const maths::vec3& location )
{
    short x, y;
    char text[ 64 ];
    int charWidth, charHeight;
    int displayWidth = static_cast< int >( m_primary.dimensions.width );
    int displayHeight = static_cast< int >( m_primary.dimensions.height );
    uint32_t colour = getStandardColour( EColourGreen, 0 );
    uint32_t shadow = getStandardColour( EColourBlack, 0 );
    CVectorDrawing& textLayer = m_overlay.getLayer( 15 );
    CVectorDrawing& dropLayer = m_overlay.getLayer( 14 );
    textLayer.clear();
    dropLayer.clear();
    textLayer.setFont( vtext::VFONT_MEDIUM, 1, true );
    dropLayer.setFont( vtext::VFONT_MEDIUM, 1, true );
    charWidth = textLayer.getSteppingWidth();
    charHeight = textLayer.getSteppingHeight();
    sprintf_s( text, sizeof( text ), "%#6.1fms %#6.1ffps", ms, hz );
    x = static_cast< short >( displayWidth - ( charWidth * ( strlen( text ) + 2 ) ) );
    y = static_cast< short >( charHeight );
    textLayer.drawText( x, y, text, colour );
    dropLayer.drawText( ( x + 1 ), ( y + 1 ), text, shadow );
    sprintf_s( text, sizeof( text ), "%5.2f, %5.2f, %5.2f", location.x, location.y, location.z );
    x = static_cast< short >( ( displayWidth - ( charWidth * strlen( text ) ) ) >> 1 );
    y = static_cast< short >( displayHeight - charHeight );
    textLayer.drawText( x, y, text, colour );
    dropLayer.drawText( ( x + 1 ), ( y + 1 ), text, shadow );
}

void CRenderer::drawTabToggleAndTitle( const bool show )
{
    short x, y;
    const char* text;
    int charWidth, charHeight;
    int displayWidth = static_cast< int >( m_primary.dimensions.width );
    int displayHeight = static_cast< int >( m_primary.dimensions.height );
    uint32_t colour = getStandardColour( EColourGreen, 0 );
    uint32_t shadow = getStandardColour( EColourBlack, 0 );
    CVectorDrawing& textLayer = m_overlay.getLayer( 13 );
    CVectorDrawing& dropLayer = m_overlay.getLayer( 12 );
    textLayer.clear();
    dropLayer.clear();
    textLayer.setFont( vtext::VFONT_MEDIUM, 1, true );
    dropLayer.setFont( vtext::VFONT_MEDIUM, 1, true );
    charWidth = textLayer.getSteppingWidth();
    charHeight = textLayer.getSteppingHeight();
    text = ( show ? "Tab: Hide controls" : "Tab: Show controls" );
    x = static_cast< short >( charWidth * 2 );
    y = static_cast< short >( displayHeight - charHeight );
    textLayer.drawText( x, y, text, colour );
    dropLayer.drawText( ( x + 1 ), ( y + 1 ), text, shadow );
    textLayer.setFont( vtext::VFONT_MEDIUM, 2, false );
    dropLayer.setFont( vtext::VFONT_MEDIUM, 2, false );
    charWidth = textLayer.getSteppingWidth();
    charHeight = textLayer.getSteppingHeight();
    text = "www.localitygame.com";
    x = static_cast< short >( displayWidth - ( charWidth * ( strlen( text ) + 1 ) ) );
    y = static_cast< short >( displayHeight - charHeight );
    textLayer.drawText( x, y, text, colour );
    dropLayer.drawText( ( x + 1 ), ( y + 1 ), text, shadow );
}

void CRenderer::drawControlMappings()
{
    int textWidth = 0;
    int textHeight = 0;
    while( g_controls[ textHeight ] )
    {
        const char* const text = g_controls[ textHeight ];
        int width = 0;
        while( text[ width ] ) ++width;
        if( textWidth < width ) textWidth = width;
        ++textHeight;
    }
    int displayWidth = static_cast< int >( m_primary.dimensions.width );
    int displayHeight = static_cast< int >( m_primary.dimensions.height );
    uint32_t colour = getStandardColour( EColourGreen, 0 );
    uint32_t shadow = getStandardColour( EColourBlack, 0 );
    CVectorDrawing& textLayer = m_overlay.getLayer( 11 );
    CVectorDrawing& dropLayer = m_overlay.getLayer( 10 );
    textLayer.clear();
    dropLayer.clear();
    textLayer.setFont( vtext::VFONT_SMALL, 2, false );
    dropLayer.setFont( vtext::VFONT_SMALL, 2, false );
    int charWidth = textLayer.getSteppingWidth();
    int charHeight = textLayer.getSteppingHeight();
    int pixelWidth = ( ( textWidth + 2 ) * charWidth );
    int pixelHeight = ( ( textHeight + 2 ) * charHeight );
    if( ( pixelWidth > displayWidth ) || ( pixelHeight > displayHeight ) )
    {
        textLayer.setFont( vtext::VFONT_SMALL, 1, false );
        dropLayer.setFont( vtext::VFONT_SMALL, 1, false );
        charWidth = textLayer.getSteppingWidth();
        charHeight = textLayer.getSteppingHeight();
        pixelWidth = ( ( textWidth + 2 ) * charWidth );
        pixelHeight = ( ( textHeight + 2 ) * charHeight );
        if( ( pixelWidth > displayWidth ) || ( pixelHeight > displayHeight ) )
        {
            textLayer.setFont( vtext::VFONT_TINY, 1, false );
            dropLayer.setFont( vtext::VFONT_TINY, 1, false );
            charWidth = textLayer.getSteppingWidth();
            charHeight = textLayer.getSteppingHeight();
            pixelWidth = ( ( textWidth + 2 ) * charWidth );
            pixelHeight = ( ( textHeight + 2 ) * charHeight );
        }
    }
    short x = static_cast< short >( ( displayWidth - ( textWidth * charWidth ) ) >> 1 );
    short y = static_cast< short >( ( displayHeight - ( textHeight * charHeight ) ) >> 1 );
    short dy = static_cast< short >( charHeight );
    textHeight = 0;
    while( g_controls[ textHeight ] )
    {
        const char* const text = g_controls[ textHeight ];
        if( text[ 0 ] )
        {
            textLayer.drawText( x, y, text, colour );
            dropLayer.drawText( ( x + 1 ), ( y + 1 ), text, shadow );
        }
        y += dy;
        ++textHeight;
    }
}

static int hbp = 0;

void hilbertExperiment()
{
    uint remaps1[ 8 ];
    for( uint i = 0; i < 8; ++i )
    {
        uint remap = 0;
        for( uint j = 0; j < 8; ++j )
        {
            remap |= ( ( ( maths::MortonToHilbert3D( ( ( i << 9 ) | ( j << 6 ) ), 4 ) >> 6 ) & 4 ) << ( j * 4 ) );
        }
        remaps1[ i ] = remap;
    }
    uint remaps2[ 8 ][ 8 ];
    for( uint i = 0; i < 8; ++i )
    {
        for( uint j = 0; j < 8; ++j )
        {
            uint remap = 0;
            for( uint k = 0; k < 8; ++k )
            {
                remap |= ( ( ( maths::MortonToHilbert3D( ( ( i << 9 ) | ( j << 6 ) | ( k << 3 ) ), 4 ) >> 3 ) & 4 ) << ( k * 4 ) );
            }
            remaps2[ i ][ j ] = remap;
        }
    }
    ++hbp;  //  0x10010110
}

void CRenderer::setupScene()
{
    hilbertExperiment();

    m_viewer.SetDefaults();
    m_viewer.projection.near_clip_z = ( 1.0f / 256.0f );
    m_viewer.projection.far_clip_z = 4096.0f;
    m_viewer.projection.SetVerticalFoV( ( 50.0f * maths::consts::DEGTORAD ), ( static_cast< float >( m_swapChain->getWidth() ) / static_cast< float >( m_swapChain->getHeight() ) ) );
    m_viewer.clipping.SetClipDX();
    m_viewer.clipping.DepthFlip();
    m_viewer.object.v.y = 1.0f;
    m_viewer.object.v.z = -8.0f;

    m_gameWorld.create( m_system, m_primary );

    m_console.create( m_system, m_primary.dimensions );
    //for( int i = 0; g_star_wars[ i ]; ++i )
    //{
    //    if( g_star_wars[ i ][ 0 ] )
    //    {
    //        m_console.print( 24, ( i + 1 ), g_star_wars[ i ], ( ( i > 1 ) ? EVTextGreen : EVTextAzure ) );
    //    }
    //}
    for( int i = 0; g_introduction[ i ]; ++i )
    {
        if( g_introduction[ i ][ 0 ] )
        {
            m_console.print( 0, ( i + 1 ), g_introduction[ i ], ( ( i == 13 ) ? ( EVTextDarkRed | EVTextFlash ) : EVTextGreen ) );
        }
    }
    m_console.setCursor( 1, 17 );

    m_overlay.create( m_system, m_primary.dimensions );

    //CVectorDrawing& layer = m_overlay.getLayer( 9 );
    //layer.clear();
    //uint32_t colour = getStandardColour( EColourGreen, 0 );
    //short w = static_cast< short >( m_overlay.getWidth() );
    //short h = static_cast< short >( m_overlay.getHeight() );
    
    //CVectorDrawing& layer = m_overlay.getLayer( 9 );
    //layer.clear();
    //layer.beginLines();
    //uint32_t colour = getStandardColour( EColourGreen, 0 );
    //for( unsigned int hilbert = 0; hilbert < 1024; ++hilbert )
    //{
    //    unsigned int index1, index2;
    //    maths::Morton_2D_Decode_16bit( maths::HilbertToMorton2D_Auto( hilbert ), index1, index2 );
    //    short x = static_cast< short >( ( index1 * 8 ) + 192 );
    //    short y = static_cast< short >( ( index2 * 8 ) + 64 );
    //    layer.draw( x, y, colour );
    //}

    m_gridWidget.create( m_system, widgets::Grid( 16, true, getStandardColour( EColourDarkBlue, 3 ), getStandardColour( EColourDarkBlue, 1 ) ), false );
    memset( &m_gridEntityData, 0, sizeof( m_gridEntityData ) );
    m_gridEntityData.rotate.x = maths::consts::RCPROOT2;
    m_gridEntityData.rotate.w = maths::consts::RCPROOT2;
    m_gridEntityData.scale.x = m_gridEntityData.scale.y = m_gridEntityData.scale.z = m_gridEntityData.scale.w = 1.0f;
    m_gridEntityData.locate.w = ( -1.0f / ( 32.0f * 32.0f ) );
    m_gridWidget.bind( m_gridEntity.consts.binding );

    m_wireWidget.create( m_system, widgets::Capsule( 8, 1.0f, getStandardColour( EColourAzure, 0 ), false, false ), false );
    memset( &m_wireEntityData, 0, sizeof( m_wireEntityData ) );
    m_wireEntityData.rotate.w = 1.0f;
    m_wireEntityData.scale.x = m_wireEntityData.scale.y = m_wireEntityData.scale.z = m_wireEntityData.scale.w = 1.0f;
    m_wireEntityData.locate.w = ( 1.0f / ( 128.0f * 128.0f ) );
    m_wireWidget.bind( m_wireEntity.consts.binding );

    m_solidWidget.create( m_system, widgets::Capsule( 8, 1.0f, getStandardColour( EColourAzure, 3 ), false, true ), true );
    memset( &m_solidEntityData, 0, sizeof( m_solidEntityData ) );
    m_solidEntityData.rotate.w = 1.0f;
    m_solidEntityData.scale.x = m_solidEntityData.scale.y = m_solidEntityData.scale.z = m_solidEntityData.scale.w = 1.0f;
    m_solidWidget.bind( m_solidEntity.consts.binding );
}

bool CRenderer::createSystem( ID3D11Device* const device, ID3D11DeviceContext* const context )
{
    if( createFactory( device, context, m_system.factory ) )
    {
        if( m_system.factory.create( m_system.registry ) )
        {
            if( loadLibrary( "libs\\slib.rbl", m_system.factory, m_system.shaders ) )
            {
                if( m_system.shaders.create( m_system.defaults ) )
                {
                    StringItem nameDefault( "default" );
                    m_primary.primaryRTV.create( m_system, nameDefault );
                    m_system.registry.add( nameDefault, m_primary.primaryRTV.resource );
                    if( m_primary.primaryRTV.binding.valid() )
                    {
                        m_system.defaults.bind( m_primary.primaryRTV.binding );
                    }
                    m_primary.primaryDSV.create( m_system, nameDefault );
                    m_system.registry.add( nameDefault, m_primary.primaryDSV.resource );
                    if( m_primary.primaryDSV.binding.valid() )
                    {
                        m_system.defaults.bind( m_primary.primaryDSV.binding );
                    }
                    updatePrimary();
                    if( loadResources() )
                    {
                        return( true );
                    }
                    m_system.defaults.release();
                }
                m_system.shaders.release();
            }
            m_system.registry.release();
        }
        m_system.factory.release();
    }
    return( false );
}

void CRenderer::removeSystem()
{
    m_primary.release();
    m_system.release();
}

void CRenderer::updatePrimary()
{
    if( m_swapChain && m_swapChain->isValid() )
    {
        UINT width = m_swapChain->getWidth();
        UINT height = m_swapChain->getHeight();
        m_primary.viewport.min.x = 0.0f;
        m_primary.viewport.min.y = 0.0f;
        m_primary.viewport.min.z = 0.0f;
        m_primary.viewport.max.x = static_cast< float >( width );
        m_primary.viewport.max.y = static_cast< float >( height );
        m_primary.viewport.max.z = 1.0f;
        m_primary.scissoring.left = 0;
        m_primary.scissoring.top = 0;
        m_primary.scissoring.right = static_cast< int >( width );
        m_primary.scissoring.bottom = static_cast< int >( height );
        m_primary.dimensions.width = static_cast< uint >( width );
        m_primary.dimensions.height = static_cast< uint >( height );
        m_primary.primaryRTV.resource.setRTV( m_swapChain->getTargetView() );
        m_primary.primaryRTV.resource.setViewport( m_primary.viewport );
        m_primary.primaryRTV.resource.setScissoring( m_primary.scissoring );
        if( m_depthStencil && m_depthStencil->isValid() )
        {
            m_primary.primaryDSV.resource.setDSV( m_depthStencil->getTargetView() );
            m_primary.primaryDSV.resource.setViewport( m_primary.viewport );
            m_primary.primaryDSV.resource.setScissoring( m_primary.scissoring );
        }
    }
    else
    {
        ASSERT_FAIL( "Main swap-chain is invalid" );
    }
}

bool CRenderer::loadResources()
{
    ResourceLibrary resourceLibrary;
    if( !loadLibrary( "libs\\rlib.rbl", m_system.factory, resourceLibrary ) )
    {
        return( false );
    }
    StringItem name;
    int count, index;
    count = resourceLibrary.getSamplerCount();
    for( index = 0; index < count; ++index )
    {
        resourceLibrary.getSamplerName( index, name );
        ResourceSampler resource;
        if( m_system.factory.create( *resourceLibrary.getSamplerDesc( index ), resource ) )
        {
            m_system.registry.add( name, resource );
            BindingSampler binding;
            if( m_system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    m_system.defaults.bind( binding );
                }
            }
        }
    }
    count = resourceLibrary.getRasterStateCount();
    for( index = 0; index < count; ++index )
    {
        resourceLibrary.getRasterStateName( index, name );
        ResourceRasterState resource;
        if( m_system.factory.create( *resourceLibrary.getRasterStateDesc( index ), resource ) )
        {
            m_system.registry.add( name, resource );
            BindingRasterState binding;
            if( m_system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    m_system.defaults.bind( binding );
                }
            }
        }
    }
    count = resourceLibrary.getDepthStateCount();
    for( index = 0; index < count; ++index )
    {
        resourceLibrary.getDepthStateName( index, name );
        ResourceDepthState resource;
        if( m_system.factory.create( *resourceLibrary.getDepthStateDesc( index ), resource ) )
        {
            m_system.registry.add( name, resource );
            BindingDepthState binding;
            if( m_system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    m_system.defaults.bind( binding );
                }
            }
        }
    }
    count = resourceLibrary.getBlendStateCount();
    for( index = 0; index < count; ++index )
    {
        resourceLibrary.getBlendStateName( index, name );
        ResourceBlendState resource;
        if( m_system.factory.create( *resourceLibrary.getBlendStateDesc( index ), resource ) )
        {
            m_system.registry.add( name, resource );
            BindingBlendState binding;
            if( m_system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    m_system.defaults.bind( binding );
                }
            }
        }
    }
    return( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end rendering namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace rendering

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

