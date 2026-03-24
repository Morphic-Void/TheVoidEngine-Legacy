
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   cwndx.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        DirectX 11 window class.
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

#include "stdafx.h"
#include "libs/system/base/types.h"
#include "cwndx.h"
#include "timing.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin platform_windows namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace platform_windows
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWndX class static member initialisation
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndX* CWndX::ms_fullscreenWnd( NULL );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWndX class public function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CWndX::WndInit( const LPCTSTR pszName, const LONG left, const LONG top, const LONG width, const LONG height, const DWORD dwFlags, const DWORD dwStyle, const DWORD dwExStyle, const HMENU hMenu, CWnd* const pWndParent )
{
    return( m_hWnd ? true : CWnd::WndInit( pszName, left, top, width, height, dwFlags, dwStyle, dwExStyle, hMenu, pWndParent ) );
}

bool CWndX::WndInit( const LPCTSTR pszName, const CWndConfig& Config, CWnd* const pWndParent )
{
    return( m_hWnd ? true : CWnd::WndInit( pszName, Config, pWndParent ) );
}

void CWndX::WndRunApp( const bool fullscreen, const bool altEnter )
{   //  run this window as the main application
    if( fullscreen )
    {
        if( !SetFullscreen() )
        {
            return;
        }
    }
    WndEnableAltEnter( altEnter );
    CWnd::WndRunApp();
    SetWindowed();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWndX class protected virtual function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CWndX::WndUpdate()
{
    bool ret = true;
    if( !m_minimised )
    {
	    if( m_swapChain.isAvailable() )
	    {
            bool resize = false;
            if( m_resizeCount )
            {
                if( !m_swapChain.resize( m_resizeWidth, m_resizeHeight ) ) return( false );
                resize = true;
            }
            m_resizeCount = 0;
            if( m_toggleCount & 1 )
            {
                if( !ToggleMode()  ) return( false );
                resize = true;
            }
            m_toggleCount = 0;
            if( resize )
            {
                if( !ResizeDepthStencil() ) return( false );
                if( !Resize( GetDisplayWidth(), GetDisplayHeight() ) ) return( false );
                if( m_swapChain.isFullscreen() ) SetFocus( m_hWnd );
            }
            CCoreX::update();
		    if( !Update() ) return( false );
		    if( !Render() ) return( false );
            TIMING_END;
            ret = m_swapChain.present( 1, 0 );  //DXGI_PRESENT_RESTART );
            TIMING_BEGIN;
            m_posChanged = false;
	    }
    }
	return( ret );
}

bool CWndX::WndDraw()
{
    return( WndUpdate() );
}

LRESULT CWndX::WndUser( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uiMsg )
    {
        case( WM_CREATE ):
        {
    	    if( !m_swapChain.create( hWnd, true ) || !Create( CCoreX::getDevice(), CCoreX::getContext(), GetDisplayWidth(), GetDisplayHeight() ) )
            {
    	        PostQuitMessage( 0 );
            }
    	    return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
        case( WM_DESTROY ):
        {
    	    Finish();
            SetWindowed();
            DisableDepthStencil();
            m_swapChain.destroy();
    	    return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
        case( WM_PAINT ):
        {
    	    RECT rectUpdate;
    	    if( GetUpdateRect( hWnd, &rectUpdate, FALSE ) )
    	    {
    		    PAINTSTRUCT ps;
    		    HDC hdc = BeginPaint( hWnd, &ps );
    		    if( hdc )
    		    {
    			    EndPaint( hWnd, &ps );
    		    }
    	    }
    	    return( 0 );
        }
        case( WM_ACTIVATEAPP ):
        {
            Activate( ( wParam == TRUE ) ? true : false );
    	    return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
        case( WM_SIZE ):
        {
            m_minimised = ( ( wParam == SIZE_MINIMIZED ) ? true : false );
            if( lParam )
            {
                m_resizeWidth = static_cast< UINT >( LOWORD( lParam ) );
                m_resizeHeight = static_cast< UINT >( HIWORD( lParam ) );
                ++m_resizeCount;
            }
    	    return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
        case( WM_WINDOWPOSCHANGED ):
        {
            m_posChanged = true;
    	    return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
        case( WM_MENUCHAR ):
        {   //  suppress the alt-enter beep caused by the system menu accellerator handling
            return( ( wParam == MAKELONG( VK_RETURN, MF_SYSMENU ) ) ? MAKELONG( 0, MNC_CLOSE ) : DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
        case( WM_KEYDOWN ):
        {
            if( ( wParam == VK_ESCAPE ) && ( ( HIWORD( lParam ) & KF_REPEAT ) == 0 ) )
            {
                m_toggleCount = ( m_swapChain.isWindowed() ? 0 : 1 );
                SetFocus( NULL );
                return( 0 );
            }
            KeyInput( uiMsg, wParam, lParam );
    	    return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
        case( WM_SYSKEYDOWN ):
        {
            if( ( wParam == VK_F4 ) && ( ( HIWORD( lParam ) & ( KF_REPEAT | KF_ALTDOWN ) ) == KF_ALTDOWN ) )
            {
    	        PostQuitMessage( 0 );
                return( 0 );
            }
            if( m_enableAltEnter )
            {
                if( ( wParam == VK_RETURN ) && ( ( HIWORD( lParam ) & ( KF_REPEAT | KF_ALTDOWN ) ) == KF_ALTDOWN ) )
                {   //  alt-enter key combination has been entered and this is not an auto-repeat key-stroke
                    ++m_toggleCount;
                    return( 0 );
                }
            }
            KeyInput( uiMsg, wParam, lParam );
    	    return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
        default:
        {
            if( ( uiMsg >= WM_KEYFIRST ) && ( uiMsg <= WM_KEYLAST ) )
            {   //  process keyboard
                KeyInput( uiMsg, wParam, lParam );
            }
            if( ( uiMsg >= WM_MOUSEFIRST ) && ( uiMsg <= WM_MOUSELAST ) )
            {   //  process mouse
                if( !m_posChanged )
                {
                    if( GetFocus() == hWnd )
                    {
                        MouseInput( uiMsg, wParam, lParam );
                    }
                    else
                    {
                        switch( uiMsg )
                        {
                            case( WM_LBUTTONDOWN ):
                            case( WM_LBUTTONDBLCLK ):
                            case( WM_RBUTTONDOWN ):
                            case( WM_RBUTTONDBLCLK ):
                            case( WM_MBUTTONDOWN ):
                            case( WM_MBUTTONDBLCLK ):
                            case( WM_XBUTTONDOWN ):
                            case( WM_XBUTTONDBLCLK ):
                            {
                                SetFocus( hWnd );
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
    	    return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWndX class protected DirectX management function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CWndX::SetWindowed()
{
    if( m_swapChain.isFullscreen() )
    {
        WndEndExclusiveUpdates();
        ms_fullscreenWnd = NULL;
        return( m_swapChain.setWindowed() );
    }
    return( true );
}

bool CWndX::SetFullscreen()
{
    if( m_swapChain.isWindowed() )
    {
        if( ms_fullscreenWnd != NULL )
        {
            if( !ms_fullscreenWnd->SetWindowed() )
            {
                return( false );
            }
            ms_fullscreenWnd = NULL;
        }
        WndSetExclusiveUpdates();
        ms_fullscreenWnd = this;
        return( m_swapChain.setFullscreen() );
    }
    return( true );
}

bool CWndX::EnableDepthStencil()
{
    if( !m_enableDepthStencil )
    {
        m_enableDepthStencil = m_depthStencil.create( m_swapChain.getWidth(), m_swapChain.getHeight(), DXGI_FORMAT_D32_FLOAT_S8X24_UINT, false );
        if( m_enableDepthStencil )
        {
            m_enableDepthStencil = m_depthState.create( rendering::STANDARD_DEPTH_STATE_DESC );
            if( !m_enableDepthStencil )
            {
                m_depthStencil.destroy();
            }
        }
    }
    return( m_enableDepthStencil );
}

bool CWndX::ResizeDepthStencil()
{
    if( m_enableDepthStencil )
    {
        m_enableDepthStencil = m_depthStencil.resize( m_swapChain.getWidth(), m_swapChain.getHeight() );
        return( m_enableDepthStencil );
    }
    return( true );
}

void CWndX::DisableDepthStencil()
{
    if( m_enableDepthStencil )
    {
        m_enableDepthStencil = false;
        m_depthStencil.destroy();
        m_depthState.destroy();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWndX protected default virtual application function bodiess
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! initialise camera, objects etc.
bool CWndX::Create( ID3D11Device* device, ID3D11DeviceContext* context, const UINT Width, const UINT Height )
{
    UNUSED( device );
    UNUSED( context );
    UNUSED( Width );
    UNUSED( Height );
	return( true );
}

//! update cameras and buffers (other than the main swap-chain) to reflect window resizing and possible aspect ratio changes
bool CWndX::Resize( const UINT Width, const UINT Height )
{
    UNUSED( Width );
    UNUSED( Height );
	return( true );
}

//! update the scene for each frame
bool CWndX::Update()
{
	return( true );
}

//! render the scene
bool CWndX::Render()
{   //  test that the present function is operating correctly by clearing to a known colour
    CCoreX::getContext()->ClearRenderTargetView( m_swapChain.getTargetView(), DirectX::Colors::Black );
    if( m_enableDepthStencil )
    {
        CCoreX::getContext()->ClearDepthStencilView( m_depthStencil.getTargetView(), ( D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL ), 0.0f, 0x00 );
    }
	return( true );
}

//! tidy up any assets ready to exit the application
void CWndX::Finish()
{
}

//! process activation and deactivation
void CWndX::Activate( const bool state )
{
    UNUSED( state );
}

//! process the keyboard input stream
void CWndX::KeyInput( const UINT uiMsg, const WPARAM wParam, const LPARAM lParam )
{
    UNUSED( uiMsg );
    UNUSED( wParam );
    UNUSED( lParam );
}

//! process the mouse input stream
void CWndX::MouseInput( const UINT uiMsg, const WPARAM wParam, const LPARAM lParam )
{
    UNUSED( uiMsg );
    UNUSED( wParam );
    UNUSED( lParam );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end platform_windows namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace platform_windows

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
