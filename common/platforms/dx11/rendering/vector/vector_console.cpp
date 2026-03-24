
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vector_console.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Vector console class.
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
#include "vector_console.h"

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
////    Vector console class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CVectorConsole::create( const CoreSystem& system, const DIMS2D& display )
{   //  create with defaults
    CONFIG config;
    config.charScale = 1;
    config.charFont = vtext::VFONT_SMALL;
    config.descenders = true;
    config.windowChars.width = 80;
    config.windowChars.height = 32;
    config.bufferChars.width = config.windowChars.width;
    config.bufferChars.height = ( config.windowChars.height * 10 );
    return( create( system, display, config ) );
}

bool CVectorConsole::create( const CoreSystem& system, const DIMS2D& display, const CONFIG& config )
{
    kill();
    if( init( system, display, config ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void CVectorConsole::destroy()
{
    if( this )
    {
        kill();
    }
}

bool CVectorConsole::init( const CoreSystem& system, const DIMS2D& display, const CONFIG& config )
{
    m_cursor.x = m_cursor.y = 0;
    if( !CVectorDisplay::create( system, config ) )
    {
        return( false );
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
    if( !CVectorDisplay::bind( m_sceneBinding ) || !CVectorDisplay::bind( m_entityBinding ) )
    {
        return( false );
    }
    setup( display );
    m_state = 3;
    m_glide = 0.0f;
    m_lerp = false;
    m_show = true;
    m_hPos = ELeft; //ECenter;
    m_vPos = ETop;  //EMiddle;
    return( true );
}

void CVectorConsole::kill()
{
    CVectorDisplay::kill();
    m_blink = 0;
    m_cursor.x = m_cursor.y = 0;
    m_display.width = m_display.height = 0;
    m_sceneBinding.release();
    m_sceneResource.release();
    m_sceneBuffer.destroy();
    memset( &m_sceneData, 0, sizeof( m_sceneData ) );
    m_entityBinding.release();
    m_entityResource.release();
    m_entityBuffer.destroy();
    memset( &m_entityData, 0, sizeof( m_entityData ) );
    memset( &m_entityData, 0, sizeof( m_entityData ) );
    memset( &m_viewer, 0, sizeof( m_viewer ) );
    m_state = 0;
    m_glide = 0.0f;
    m_lerp = false;
    m_show = false;
    m_hPos = ECenter;
    m_vPos = EMiddle;
}

void CVectorConsole::setup( const DIMS2D& display )
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

void CVectorConsole::resize( const DIMS2D& display )
{
    if( valid() )
    {
        setup( display );
    }
}

void CVectorConsole::update( const float time )
{
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
            entity.v = { ( 0.5f - ( ( getWindowWidth() & ~1 ) * 0.5f ) ), ( 0.5f - ( ( getWindowHeight() & ~1 ) * 0.5f ) ), 0.0f, 1.0f };
            if( m_hPos != ECenter )
            {
                entity.v.x *= t;
                if( m_hPos == ELeft )
                {
                    entity.v.x += ( ( ( getSteppingWidth() << 1 ) - halfWidth + 0.5f ) * s );
                }
                else
                {
                    entity.v.x += ( ( halfWidth - ( getWindowWidth() + ( getSteppingWidth() << 1 ) ) + 0.5f ) * s );
                }
            }
            if( m_vPos != EMiddle )
            {
                entity.v.y *= t;
                if( m_vPos == ETop )
                {
                    entity.v.y += ( ( getSteppingHeight() - halfHeight + 0.5f ) * s );
                }
                else
                {
                    entity.v.y += ( ( halfHeight - ( getWindowHeight() + getSteppingHeight() ) + 0.5f ) * s );
                }
            }
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
            entity.v.w = 1.0f;
            entity.q.SetMul( zRotate, yRotate );
            setEntityConsts( m_entityData, entity );
            m_entityBuffer.create( static_cast< UINT >( sizeof( m_entityData ) ), &m_entityData );
            m_entityResource.setBuffer( m_entityBuffer.getBuffer() );
            m_entityResource.setOffset( 0 );
            m_entityResource.setStride( m_entityBuffer.getStride() );
        }
        uint offset = static_cast< uint >( m_cursor.x + ( m_cursor.y * m_vcmap.width ) );
        char character = m_vcmap.text[ offset ];
        char code = m_vcmap.codes[ offset ];
        if( ( m_ticks - m_blink ) & 0x00000200 )
        {   //  hide
            m_vcmap.codes[ offset ] = static_cast< char >( uint( EVTextHidden + 1 ) );
        }
        else if( character == 0 )
        {   //  show
            m_vcmap.text[ offset ] = '_';  //static_cast< char >( vtext::VCHAR_TAB );  //'_';    //0x1b;
            m_vcmap.codes[ offset ] = static_cast< char >( uint( EVTextGreen + 1 ) );
        }
        CVectorDisplay::update( time );
        m_vcmap.text[ offset ] = character;
        m_vcmap.codes[ offset ] = code;
    }
}

void CVectorConsole::render()
{
    if( valid() )
    {
        if( m_show )
        {
            CVectorDisplay::render();
        }
    }
}

void CVectorConsole::keyInput( const input::CKeyEvent keyEvent )
{
    if( valid() )
    {
        input::EKeyAction keyAction = keyEvent.getAction();
        if( ( keyAction == input::EKeyActionPressed ) || ( keyAction == input::EKeyActionRepeated ) )
        {
            uint vkey = keyEvent.getVKey();
            uint state = keyEvent.getState();
            uint navigation = getNavigation( vkey, state );
            if( navigation == ENavPause )
            {
                toggle();
            }
            else if( active() )
            {
                char ascii = keyEvent.getAscii();
                switch( navigation )
                {
                    case( ENavNone ):
                    {
                        if( ( vkey >= 0x00000070 ) && ( vkey <= 0x00000087 ) )
                        {   //  function key
                        }
                        else
                        {
                            if( ascii )
                            {
                                unsigned char glyphIndex = vtext::asciiToGlyph[ static_cast< unsigned char >( ascii ) ];
                                putc( m_cursor.x, m_cursor.y, glyphIndex, EVTextGreen );
                                ++m_cursor.x;
                                if( static_cast< uint >( m_cursor.x ) == m_config.bufferChars.width )
                                {
                                    m_cursor.x = 0;
                                    ++m_cursor.y;
                                    if( static_cast< uint >( m_cursor.y ) == m_config.bufferChars.height )
                                    {
                                        m_cursor.y = 0;
                                    }
                                }
                                m_blink = m_ticks;
                            }
                        }
                        break;
                    }
                    case( ENavPgUp ):
                    {
                        if( state & input::EKeyStateBitCtrl )
                        {
                            if( m_config.charScale < 5 )
                            {
                                ++m_config.charScale;
                                SetFont();
                            }
                        }
                        else if( state & input::EKeyStateBitAlt )
                        {
                            if( m_config.charFont < vtext::VFONT_LARGE )
                            {
                                ++m_config.charFont;
                                SetFont();
                            }
                        }
                        break;
                    }
                    case( ENavPgDown ):
                    {
                        if( state & input::EKeyStateBitCtrl )
                        {
                            if( m_config.charScale > 1 )
                            {
                                --m_config.charScale;
                                SetFont();
                            }
                        }
                        else if( state & input::EKeyStateBitAlt )
                        {
                            if( m_config.charFont > vtext::VFONT_TINY )
                            {
                                --m_config.charFont;
                                SetFont();
                            }
                        }
                        break;
                    }
                    case( ENavEnd ):
                    {
                        break;
                    }
                    case( ENavHome ):
                    {
                        break;
                    }
                    case( ENavLeft ):
                    {
                        if( state & input::EKeyStateBitCtrl )
                        {
                            if( m_hPos != ELeft )
                            {
                                m_hPos = ( ( m_hPos == ECenter ) ? ELeft : ECenter );
                            }
                        }
                        break;
                    }
                    case( ENavUp ):
                    {
                        if( state & input::EKeyStateBitCtrl )
                        {
                            if( m_vPos != ETop )
                            {
                                m_vPos = ( ( m_vPos == EMiddle ) ? ETop : EMiddle );
                            }
                        }
                        else if( state & input::EKeyStateBitAlt )
                        {
                            if( m_config.descenders == true )
                            {
                                m_config.descenders = false;
                                SetFont();
                            }
                        }
                        break;
                    }
                    case( ENavRight ):
                    {
                        if( state & input::EKeyStateBitCtrl )
                        {
                            if( m_hPos != ERight )
                            {
                                m_hPos = ( ( m_hPos == ECenter ) ? ERight : ECenter );
                            }
                        }
                        break;
                    }
                    case( ENavDown ):
                    {
                        if( state & input::EKeyStateBitCtrl )
                        {
                            if( m_vPos != EBottom )
                            {
                                m_vPos = ( ( m_vPos == EMiddle ) ? EBottom : EMiddle );
                            }
                        }
                        else if( state & input::EKeyStateBitAlt )
                        {
                            if( m_config.descenders == false )
                            {
                                m_config.descenders = true;
                                SetFont();
                            }
                        }
                        break;
                    }
                    case( ENavBack ):
                    {
                        if( m_cursor.x == 0 )
                        {
                            if( m_cursor.y == 0 )
                            {
                                m_cursor.y = m_config.bufferChars.height;
                            }
                            --m_cursor.y;
                            m_cursor.x = m_config.bufferChars.width;
                        }
                        --m_cursor.x;
                        putc( m_cursor.x, m_cursor.y, 0, EVTextNull );
                        m_blink = m_ticks;
                        break;
                    }
                    case( ENavTab ):
                    {
                        break;
                    }
                    case( ENavEnter ):
                    {
                        m_cursor.x = 0;
                        ++m_cursor.y;
                        if( static_cast< uint >( m_cursor.y ) == m_config.bufferChars.height )
                        {
                            m_cursor.y = 0;
                        }
                        putc( m_cursor.x, m_cursor.y, '>', EVTextGreen );
                        ++m_cursor.x;
                        m_blink = m_ticks;
                        break;
                    }
                    case( ENavIns ):
                    {
                        break;
                    }
                    case( ENavDel ):
                    {
                        break;
                    }
                    case( ENavPause ):
                    {
                        break;
                    }
                    case( ENavEscape ):
                    {
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
}

uint CVectorConsole::getNavigation( const uint vkey, const uint state ) const
{
    if( ( vkey >= 0x21 ) && ( vkey <= 0x28 ) )
    {
        static const uint decode[] = { ENavPgUp, ENavPgDown, ENavEnd, ENavHome, ENavLeft, ENavUp, ENavRight, ENavDown };
        return( decode[ vkey - 0x21 ] );
    }
    else if( ( vkey >= 0x60 ) && ( vkey <= 0x69 ) )
    {
        if( ( state & input::EKeyStateBitNumLock ) != input::EKeyStateBitNumLock )
        {   //  num-lock not down
            static const uint decode[] = { ENavIns, ENavEnd, ENavDown, ENavPgDown, ENavLeft, ENavNone, ENavRight, ENavHome, ENavUp, ENavPgUp };
            return( decode[ vkey - 0x60 ] );
        }
    }
    else if( vkey == 0x08 )
    {
        return( ENavBack );
    }
    else if( vkey == 0x09 )
    {
        return( ENavTab );
    }
    else if( vkey == 0x0d )
    {
        return( ENavEnter );
    }
    else if( vkey == 0x2d )
    {
        return( ENavIns );
    }
    else if( vkey == 0x2e )
    {
        return( ENavDel );
    }
    else if( vkey == 0x13 )
    {
        return( ENavPause );
    }
    else if( vkey == 0x1b )
    {
        return( ENavEscape );
    }
    return( ENavNone );
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

