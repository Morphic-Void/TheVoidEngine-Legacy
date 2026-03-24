
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   key_mouse_win32.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Win32 keyboard and mouse input class.
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
#include "libs/maths/consts.h"
#include "key_mouse_win32.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    control mouse capturing
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CAPTURE_LOCKED  FALSE

#if ( CAPTURE_LOCKED == TRUE )
#define CAPTURE_MOUSE   SetCapture( m_hWnd )
#define RELEASE_MOUSE   ReleaseCapture()
#else
#define CAPTURE_MOUSE
#define RELEASE_MOUSE
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin input namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace input
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CKeyMouse keyboard and mouse control class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CKeyMouseInput::setWindow( const HWND hWnd )
{
    if( m_hWnd != hWnd )
    {
        if( hWnd != NULL )
        {   //  activating or switching window
            bool activating = ( ( m_hWnd == NULL ) ? true : false );
            m_hWnd = hWnd;
            if( m_active )
            {   //  activating
                refreshCoords();
                if( activating ) GetCursorPos( &m_screenCursor );
                m_clientCursor = m_screenCursor;
                ScreenToClient( m_hWnd, &m_clientCursor );
                if( m_locked )
                {
                    CAPTURE_MOUSE;
                    ShowCursor( FALSE );
                    ClipCursor( &m_screenRect );
                    SetCursorPos( m_screenCenter.x, m_screenCenter.y );
                }
            }
        }
        else
        {   //  deactivating
            m_hWnd = hWnd;
            zero();
            if( m_locked )
            {   //  deactivating from locked state
                RELEASE_MOUSE;
                ShowCursor( TRUE );
                ClipCursor( NULL );
                SetCursorPos( m_screenCursor.x, m_screenCursor.y );
            }
        }
    }
}

void CKeyMouseInput::setActive( const bool active )
{
    if( m_active != active )
    {
        m_active = active;
        if( m_hWnd != NULL )
        {
            if( m_active )
            {   //  activating
                refreshCoords();
                GetCursorPos( &m_screenCursor );
                m_clientCursor = m_screenCursor;
                ScreenToClient( m_hWnd, &m_clientCursor );
                if( m_locked )
                {
                    CAPTURE_MOUSE;
                    ShowCursor( FALSE );
                    ClipCursor( &m_screenRect );
                    SetCursorPos( m_screenCenter.x, m_screenCenter.y );
                }
            }
            else
            {   //  deactivating
                zero();
                if( m_locked )
                {   //  deactivating from locked state
                    RELEASE_MOUSE;
                    ShowCursor( TRUE );
                    ClipCursor( NULL );
                    SetCursorPos( m_screenCursor.x, m_screenCursor.y );
                }
            }
        }
    }
}

void CKeyMouseInput::setLocked( const bool locked )
{
    if( m_locked != locked )
    {
        m_locked = locked;
        if( m_hWnd != NULL )
        {
            if( m_active )
            {
                refreshCoords();
                if( m_locked )
                {   //  locking
                    GetCursorPos( &m_screenCursor );
                    m_clientCursor = m_screenCursor;
                    ScreenToClient( m_hWnd, &m_clientCursor );
                    CAPTURE_MOUSE;
                    ShowCursor( FALSE );
                    ClipCursor( &m_screenRect );
                    SetCursorPos( m_screenCenter.x, m_screenCenter.y );
                }
                else
                {   //  unlocking
                    RELEASE_MOUSE;
                    ShowCursor( TRUE );
                    ClipCursor( NULL );
                    SetCursorPos( m_screenCursor.x, m_screenCursor.y );
                }
            }
        }
    }
}

void CKeyMouseInput::reset()
{
    if( ( m_hWnd != NULL ) && m_active && m_locked )
    {
        RELEASE_MOUSE;
        ShowCursor( TRUE );
        ClipCursor( NULL );
        SetCursorPos( m_screenCursor.x, m_screenCursor.y );
    }
    m_active = false;
    m_locked = false;
    zero();
    m_automatic = false;
    m_lParam = 0;
    m_screenRect.left = m_screenRect.top = m_screenRect.right = m_screenRect.bottom = 0;
    m_screenCenter.x = m_screenCenter.y = 0;
    m_clientCenter.x = m_clientCenter.y = 0;
    m_screenCursor.x = m_screenCursor.y = 0;
    m_clientCursor.x = m_clientCursor.y = 0;
    m_sumDeltaX = m_sumDeltaY = 0;
    m_sumWheelV = m_sumWheelH = 0;
    m_hWnd = NULL;
}

void CKeyMouseInput::update()
{
    if( m_hWnd != NULL )
    {
        if( m_automatic )
        {
            setActive( ( ( GetFocus() == m_hWnd ) && ( GetForegroundWindow() == m_hWnd ) ) ? true : false );
        }
        if( m_active )
        {
            BYTE keys[ 256 ];
            if( GetKeyboardState( keys ) == TRUE )
            {
                for( int i = 0; i < 256; ++i )
                {   //  flag key transitions
                    uint8_t state = ( static_cast< uint8_t >( keys[ i ] ) & 0x81 );
                    uint8_t delta = ( ( m_keys[ i ] & 0x81 ) ^ state );
                    m_keys[ i ] = ( state | ( delta >> 1 ) | ( delta << 1 ) );
                }
            }
            else
            {
                for( int i = 0; i < 256; ++i )
                {   //  release all the keys
                    uint8_t delta = ( m_keys[ i ] & 0x81 );
                    m_keys[ i ] = ( ( delta >> 1 ) | ( delta << 1 ) );
                }
            }
            if( !m_locked )
            {
                POINT clientCursor;
                m_sumDeltaX = m_sumDeltaY = 0;
                if( GetCursorPos( &clientCursor ) && ScreenToClient( m_hWnd, &clientCursor ) )
                {
                    m_sumDeltaX = ( clientCursor.x - m_clientCursor.x );
                    m_sumDeltaY = ( clientCursor.y - m_clientCursor.y );
                    m_clientCursor = clientCursor;
                }
            }
            m_mouseX = m_clientCursor.x;
            m_mouseY = m_clientCursor.y;
            m_deltaX = m_sumDeltaX;
            m_deltaY = m_sumDeltaY;
            m_wheelV = m_sumWheelV;
            m_wheelH = m_sumWheelH;
            refreshCoords();
        }
    }
    else if( m_active )
    {
        for( int i = 0; i < 256; ++i )
        {   //  release all the keys
            uint8_t delta = ( m_keys[ i ] & 0x81 );
            m_keys[ i ] = ( ( delta >> 1 ) | ( delta << 1 ) );
        }
        m_mouseX = m_clientCursor.x;
        m_mouseY = m_clientCursor.y;
        m_deltaX = 0;
        m_deltaY = 0;
        m_wheelV = 0;
        m_wheelH = 0;
    }
}

void CKeyMouseInput::activate( const HWND hWnd, const bool state )
{
    if( hWnd == m_hWnd ) setActive( state );
}

bool CKeyMouseInput::processKey( const HWND hWnd, const UINT uiMsg, const WPARAM wParam, const LPARAM lParam, CKeyEvent& keyEvent )
{
    if( ( hWnd == m_hWnd ) && m_active )
    {
        EKeyAction action = EKeyActionNull;
        switch( uiMsg )
        {
            case( WM_KEYUP ):
            case( WM_SYSKEYUP ):
            {
                action = EKeyActionReleased;
                break;
            }
            case( WM_KEYDOWN ):
            case( WM_SYSKEYDOWN ):
            {
                action = ( ( HIWORD( lParam ) & KF_REPEAT ) ? EKeyActionRepeated : EKeyActionPressed );
                break;
            }
            default:
            {
                break;
            }
        }
        if( action != EKeyActionNull )
        {
            BYTE keys[ 256 ];
            if( GetKeyboardState( keys ) == TRUE )
            {
                WORD ascii = 0;
                if( ToAsciiEx( static_cast< UINT >( wParam ), static_cast< UINT >( ( lParam >> 16 ) & 0x000000ff ), keys, &ascii, 0, GetKeyboardLayout( 0 ) ) != 1 )
                {
                    ascii = 0;
                }
                uint state = 0;
                if( keys[ VK_CAPITAL ] & 0x01 ) state |= EKeyStateBitCapsLock;
                if( keys[ VK_NUMLOCK ] & 0x01 ) state |= EKeyStateBitNumLock;
                if( keys[ VK_SHIFT ] & 0x80 ) state |= EKeyStateBitShift;
                if( keys[ VK_CONTROL ] & 0x80 ) state |= EKeyStateBitCtrl;
                if( keys[ VK_MENU ] & 0x80 ) state |= EKeyStateBitAlt;
                keyEvent.setVKey( static_cast< uint >( wParam ) & 0x000000ff );
                keyEvent.setAscii( static_cast< char >( ascii ) );
                keyEvent.setState( state );
                keyEvent.setAction( action );
                return( true );
            }
        }
    }
    return( false );
}

bool CKeyMouseInput::processMouse( const HWND hWnd, const UINT uiMsg, const WPARAM wParam, const LPARAM lParam, CMouseEvent& mouseEvent, CMouseParam& mouseParam )
{
    if( ( hWnd == m_hWnd ) && m_active )
    {
        mouseEvent.setState( getMouseState( wParam ) );
        mouseParam.setPosParam( static_cast< UINT >( m_locked ? m_lParam : lParam ) );
        switch( uiMsg )
        {
            case( WM_MOUSEMOVE ):
            {
                mouseEvent.setPart( EMousePartNull );
                mouseEvent.setAction( EMouseActionMove );
                if( m_locked )
                {
                    if( m_lParam == lParam )
                    {
                        return( false );
                    }
                    int deltaX = ( static_cast< int >( static_cast< int16_t >( lParam ) ) - m_clientCenter.x );
                    int deltaY = ( ( static_cast< int >( lParam ) >> 16 ) - m_clientCenter.y );
                    m_sumDeltaX += deltaX;
                    m_sumDeltaY += deltaY;
                    SetCursorPos( m_screenCenter.x, m_screenCenter.y );
                }
                return( true );
            }
            case( WM_LBUTTONDOWN ):
            {
                mouseEvent.setPart( EMousePartKeyL );
                mouseEvent.setAction( EMouseActionClick );
                return( true );
            }
            case( WM_LBUTTONUP ):
            {
                mouseEvent.setPart( EMousePartKeyL );
                mouseEvent.setAction( EMouseActionRelease );
                return( true );
            }
            case( WM_LBUTTONDBLCLK ):
            {
                mouseEvent.setPart( EMousePartKeyL );
                mouseEvent.setAction( EMouseActionDblClick );
                return( true );
            }
            case( WM_RBUTTONDOWN ):
            {
                mouseEvent.setPart( EMousePartKeyR );
                mouseEvent.setAction( EMouseActionClick );
                return( true );
            }
            case( WM_RBUTTONUP ):
            {
                mouseEvent.setPart( EMousePartKeyR );
                mouseEvent.setAction( EMouseActionRelease );
                return( true );
            }
            case( WM_RBUTTONDBLCLK ):
            {
                mouseEvent.setPart( EMousePartKeyR );
                mouseEvent.setAction( EMouseActionDblClick );
                return( true );
            }
            case( WM_MBUTTONDOWN ):
            {
                mouseEvent.setPart( EMousePartKeyM );
                mouseEvent.setAction( EMouseActionClick );
                return( true );
            }
            case( WM_MBUTTONUP ):
            {
                mouseEvent.setPart( EMousePartKeyM );
                mouseEvent.setAction( EMouseActionRelease );
                return( true );
            }
            case( WM_MBUTTONDBLCLK ):
            {
                mouseEvent.setPart( EMousePartKeyM );
                mouseEvent.setAction( EMouseActionDblClick );
                return( true );
            }
            case( WM_MOUSEWHEEL ):
            {
                mouseEvent.setPart( EMousePartWheelV );
                mouseEvent.setAction( EMouseActionMove );
                mouseEvent.setWheelParam( static_cast< UINT >( wParam ) );
                m_sumWheelV += ( static_cast< int >( wParam ) >> 16 );
                return( true );
            }
            case( WM_XBUTTONDOWN ):
            {
                if( wParam & 0x00030000 )
                {
                    mouseEvent.setPart( ( wParam & 0x00010000 ) ? EMousePartKeyA : EMousePartKeyB );
                    mouseEvent.setAction( EMouseActionClick );
                    return( true );
                }
                break;
            }
            case( WM_XBUTTONUP ):
            {
                if( wParam & 0x00030000 )
                {
                    mouseEvent.setPart( ( wParam & 0x00010000 ) ? EMousePartKeyA : EMousePartKeyB );
                    mouseEvent.setAction( EMouseActionRelease );
                    return( true );
                }
                break;
            }
            case( WM_XBUTTONDBLCLK ):
            {
                if( wParam & 0x00030000 )
                {
                    mouseEvent.setPart( ( wParam & 0x00010000 ) ? EMousePartKeyA : EMousePartKeyB );
                    mouseEvent.setAction( EMouseActionDblClick );
                    return( true );
                }
                break;
            }
            case( WM_MOUSEHWHEEL ):
            {
                mouseEvent.setPart( EMousePartWheelH );
                mouseEvent.setAction( EMouseActionMove );
                mouseEvent.setWheelParam( static_cast< UINT >( wParam ) );
                m_sumWheelH += ( static_cast< int >( wParam ) >> 16 );
                return( true );
            }
            default:
            {
                break;
            }
        }
    }
    return( false );
}

void CKeyMouseInput::refreshCoords()
{
    RECT rect;
    GetClientRect( m_hWnd, &rect );
    ClientToScreen( m_hWnd, &reinterpret_cast< POINT* >( &rect )[ 0 ] );
    ClientToScreen( m_hWnd, &reinterpret_cast< POINT* >( &rect )[ 1 ] );
    if( ( m_screenRect.left != rect.left ) || ( m_screenRect.top != rect.top ) || ( m_screenRect.right != rect.right ) || ( m_screenRect.bottom != rect.bottom ) )
    {
        m_screenRect = rect;
        m_screenCenter.x = ( ( m_screenRect.right + m_screenRect.left ) >> 1 );
        m_screenCenter.y = ( ( m_screenRect.bottom + m_screenRect.top ) >> 1 );
        m_clientCenter.x = ( m_screenCenter.x - m_screenRect.left );
        m_clientCenter.y = ( m_screenCenter.y - m_screenRect.top );
        m_lParam = ( ( static_cast< LPARAM >( m_clientCenter.x ) & 0x0000ffff ) | ( ( static_cast< LPARAM >( m_clientCenter.y ) & 0x0000ffff ) << 16 ) );
        if( m_locked )
        {
            ClipCursor( &m_screenRect );
            SetCursorPos( m_screenCenter.x, m_screenCenter.y );
        }
    }
    m_sumDeltaX = m_sumDeltaY = 0;
    m_sumWheelV = m_sumWheelH = 0;
}

uint32_t CKeyMouseInput::getMouseState( const WPARAM wParam ) const
{
    uint32_t state = 0;
    if( wParam & MK_SHIFT ) state |= EKeyStateBitShift;
    if( wParam & MK_CONTROL ) state |= EKeyStateBitCtrl;
    if( wParam & MK_LBUTTON ) state |= EKeyStateBitL;
    if( wParam & MK_MBUTTON ) state |= EKeyStateBitM;
    if( wParam & MK_RBUTTON ) state |= EKeyStateBitR;
    if( wParam & MK_XBUTTON1 ) state |= EKeyStateBitA;
    if( wParam & MK_XBUTTON2 ) state |= EKeyStateBitB;
    return( state );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end input namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace input

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

