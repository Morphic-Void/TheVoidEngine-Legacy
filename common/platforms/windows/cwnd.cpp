
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   cwnd.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	C++ wrapper for windows.
////    	All windows should be derived from CWnd.
////
////    Notes:
////
////    	WndCreateFont font name reference:
////
////    		Standard Raster fonts:
////
////    			"System"
////    			"FixedSys"
////    			"Terminal"
////    			"Courier"
////    			"MS Serif"
////    			"MS Sans Serif"
////    			"Small Fonts"
////
////    		Standard TrueType fonts:
////
////    			"Arial"
////    			"Arial Bold"
////    			"Arial Italic"
////    			"Arial Bold Italic"
////    			"Courier New"
////    			"Courier New Bold"
////    			"Courier New Italic"
////    			"Courier New Bold Italic"
////    			"Times New Roman"
////    			"Times New Roman Bold"
////    			"Times New Roman Italic"
////    			"Times New Roman Bold Italic"
////    			"Symbol"
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
#include "cwnd.h"

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
////    CWndConfig class, window creation configuration management
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const CWndConfig CWndConfig::mk_DEFAULT( CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, WF_DEFAULT, WS_DEFAULT, WS_EX_DEFAULT, NULL );

void CWndConfig::Set( const CWndConfig& Config )
{
	m_left = Config.m_left;
	m_top = Config.m_top;
	m_width = Config.m_width;
	m_height = Config.m_height;
	m_dwFlags = Config.m_dwFlags;
	m_dwStyle = Config.m_dwStyle;
	m_dwExStyle = Config.m_dwExStyle;
	m_hMenu = Config.m_hMenu;
}

void CWndConfig::Set( const LONG left, const LONG top, const LONG width, const LONG height, const DWORD dwFlags, const DWORD dwStyle, const DWORD dwExStyle, const HMENU hMenu )
{
	m_left = left;
	m_top = top;
	m_width = width;
	m_height = height;
	m_dwFlags = dwFlags;
	m_dwStyle = dwStyle;
	m_dwExStyle = dwExStyle;
	m_hMenu = hMenu;
}

bool CWndConfig::GetWindowSize( LONG& width, LONG& height ) const
{
	if( ( m_width == CW_USEDEFAULT ) || ( m_height == CW_USEDEFAULT ) )
	{
		if( m_dwFlags & WF_SIZECLIENT )
		{
			RECT rectWindow = { 0, 0, m_width, m_height };
			if( AdjustWindowRectEx( &rectWindow, m_dwStyle, ( m_hMenu ? TRUE : FALSE ), m_dwExStyle ) )
			{
				width = ( rectWindow.right - rectWindow.left );
				height = ( rectWindow.bottom - rectWindow.top );
				return( true );
			}
		}
		else
		{
			width = m_width;
			height = m_height;
			return( true );
		}
	}
	width = height = 0;
	return( false );
}

void CWndConfig::SetDefaultWindowPos( void )
{
	m_dwFlags &= WF_SIZECLIENT;
	m_left = CW_USEDEFAULT;
	m_top = CW_USEDEFAULT;
}

void CWndConfig::SetDefaultWindowSize( void )
{
	m_dwFlags &= WF_CENTERCLIENT;
	m_width = CW_USEDEFAULT;
	m_height = CW_USEDEFAULT;
}

void CWndConfig::SetDefaultWindowRect( void )
{
	m_dwFlags = ( WF_POSWINDOW | WF_SIZEWINDOW );
	m_left = CW_USEDEFAULT;
	m_top = CW_USEDEFAULT;
	m_width = CW_USEDEFAULT;
	m_height = CW_USEDEFAULT;
}

void CWndConfig::SetWindow( const LONG left, const LONG top, const LONG width, const LONG height )
{
	m_dwFlags = ( WF_POSWINDOW | WF_SIZEWINDOW );
	m_left = left;
	m_top = top;
	m_width = width;
	m_height = height;
}

void CWndConfig::SetWindowPos( const LONG left, const LONG top )
{
	m_dwFlags &= WF_SIZECLIENT;
	m_left = left;
	m_top = top;
}

void CWndConfig::SetWindowSize( const LONG width, const LONG height )
{
	m_dwFlags &= WF_CENTERCLIENT;
	m_width = width;
	m_height = height;
}

void CWndConfig::SetWindowRect( const LONG left, const LONG top, const LONG right, const LONG bottom )
{
	m_dwFlags = ( WF_POSWINDOW | WF_SIZEWINDOW );
	m_left = left;
	m_top = top;
	m_width = ( right - left );
	m_height = ( bottom - top );
}

void CWndConfig::SetWindowRect( const RECT& rectWindow )
{
	m_dwFlags = ( WF_POSWINDOW | WF_SIZEWINDOW );
	m_left = rectWindow.left;
	m_top = rectWindow.top;
	m_width = ( rectWindow.right - rectWindow.left );
	m_height = ( rectWindow.bottom - rectWindow.top );
}

void CWndConfig::CenterWindow( void )
{
	m_dwFlags = ( ( m_dwFlags & WF_SIZECLIENT ) | WF_CENTERWINDOW );
}

void CWndConfig::CenterWindow( const LONG width, const LONG height )
{
	m_dwFlags = ( WF_CENTERWINDOW | WF_SIZEWINDOW );
	m_width = width;
	m_height = height;
}

void CWndConfig::SetClient( const LONG left, const LONG top, const LONG width, const LONG height )
{
	m_dwFlags = ( WF_POSCLIENT | WF_SIZECLIENT );
	m_left = left;
	m_top = top;
	m_width = width;
	m_height = height;
}

void CWndConfig::SetClientPos( const LONG left, const LONG top )
{
	m_dwFlags = ( ( m_dwFlags & WF_SIZECLIENT ) | WF_POSCLIENT );
	m_left = left;
	m_top = top;
}

void CWndConfig::SetClientSize( const LONG width, const LONG height )
{
	m_dwFlags = ( ( m_dwFlags & WF_CENTERCLIENT ) | WF_SIZECLIENT );
	m_width = width;
	m_height = height;
}

void CWndConfig::SetClientRect( const LONG left, const LONG top, const LONG right, const LONG bottom )
{
	m_dwFlags = ( WF_POSCLIENT | WF_SIZECLIENT );
	m_left = left;
	m_top = top;
	m_width = ( right - left );
	m_height = ( bottom - top );
}

void CWndConfig::SetClientRect( const RECT& rectClient )
{
	m_dwFlags = ( WF_POSCLIENT | WF_SIZECLIENT );
	m_left = rectClient.left;
	m_top = rectClient.top;
	m_width = ( rectClient.right - rectClient.left );
	m_height = ( rectClient.bottom - rectClient.top );
}

void CWndConfig::CenterClient( void )
{
	m_dwFlags = ( ( m_dwFlags & WF_SIZECLIENT ) | WF_CENTERCLIENT );
}

void CWndConfig::CenterClient( const LONG width, const LONG height )
{
	m_dwFlags = ( WF_CENTERCLIENT | WF_SIZECLIENT );
	m_width = width;
	m_height = height;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWnd class, base class for all windows
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MSG CWnd::ms_wMsg = { NULL, 0, 0, 0, 0, { 0, 0 } };
LPCSTR CWnd::ms_wAtom( NULL );
UINT CWnd::ms_uiRefs( 0 );
UINT CWnd::ms_uiOpen( 0 );
CWnd* CWnd::ms_pWndAppObject( NULL );
CWnd* CWnd::ms_pWndExclusive( NULL );
CWnd* CWnd::ms_pWndActiveList( NULL );

bool CWnd::WndInit( const LPCTSTR pszName, const LONG left, const LONG top, const LONG width, const LONG height, const DWORD dwFlags, const DWORD dwStyle, const DWORD dwExStyle, const HMENU hMenu, CWnd* const pWndParent )
{
	if( !ms_wAtom )
	{
		WNDCLASSEX wClass;
		memset( reinterpret_cast< void* >( &wClass ), 0, sizeof( WNDCLASSEX ) );
		wClass.cbSize = sizeof( WNDCLASSEX );
		wClass.style = CS_CLASSDC;
		wClass.lpfnWndProc = WndProc;
		wClass.cbClsExtra = 0;
		wClass.cbWndExtra = 0;
		wClass.hInstance = GetModuleHandle( NULL );
		wClass.hIcon = NULL;
		wClass.hCursor = LoadCursor( 0, IDC_ARROW );
		wClass.hbrBackground = NULL;
		wClass.lpszMenuName = NULL;
		wClass.lpszClassName = "CWnd";
		wClass.hIconSm = NULL;
		ms_wAtom = reinterpret_cast< LPCSTR >( RegisterClassEx( &wClass ) );
	}
	if( ms_wAtom )
	{
		if( m_hWnd )
		{
			DestroyWindow( m_hWnd );
			m_hWnd = NULL;
			m_pWndParent = NULL;
			m_pWndActivePrev = NULL;
			m_pWndActiveNext = NULL;
			ms_uiRefs--;
		}
		HWND hWndParent;
		if( pWndParent )
		{
			hWndParent = pWndParent->WndHandle();
			if( hWndParent == NULL )
			{
				hWndParent = GetDesktopWindow();
			}
		}
		else
		{
			hWndParent = GetDesktopWindow();
		}
		LONG x = left;
		LONG y = top;
		LONG w = width;
		LONG h = height;
		if( dwFlags )
		{
			RECT rectWindow = { 0, 0, 0, 0 };
			if( AdjustWindowRectEx( &rectWindow, dwStyle, ( hMenu ? TRUE : FALSE ), dwExStyle ) )
			{
				if( x != CW_USEDEFAULT )
				{
					if( ( dwFlags & WF_CENTERCLIENT ) == WF_POSCLIENT )
					{
						x += rectWindow.left;
						y += rectWindow.top;
					}
				}
				if( w != CW_USEDEFAULT )
				{
					if( dwFlags & WF_SIZECLIENT )
					{
						w += ( rectWindow.right - rectWindow.left );
						h += ( rectWindow.bottom - rectWindow.top );
					}
					if( dwFlags & WF_CENTERMASK )
					{
						RECT rectParent;
						GetClientRect( hWndParent, &rectParent );
						if( ( dwFlags & WF_CENTERCLIENT ) == WF_CENTERCLIENT )
						{
							x = ( ( ( rectParent.right - rectParent.left + rectWindow.right - rectWindow.left - w ) >> 1 ) + rectWindow.left );
							y = ( ( ( rectParent.bottom - rectParent.top + rectWindow.bottom - rectWindow.top - h ) >> 1 ) + rectWindow.top );
						}
						else
						{
							x = ( ( rectParent.right - rectParent.left - w ) >> 1 );
							y = ( ( rectParent.bottom - rectParent.top - h ) >> 1 );
						}
					}
				}
			}
		}
		m_hWnd = CreateWindowEx( dwExStyle, ms_wAtom, pszName, dwStyle, x, y, w, h, hWndParent, hMenu, GetModuleHandle( NULL ), reinterpret_cast< LPVOID >( this ) );
		if( m_hWnd )
		{
			m_pWndParent = pWndParent;
			ms_uiRefs++;
			return( true );
		}
		if( !ms_uiRefs )
		{
			UnregisterClass( ms_wAtom, GetModuleHandle( NULL ) );
			ms_wAtom = NULL;
		}
	}
	return( false );
}

bool CWnd::WndInit( const LPCTSTR pszName, const CWndConfig& Config, CWnd* const pWndParent )
{
	return( WndInit( pszName, Config.GetX(), Config.GetY(), Config.GetW(), Config.GetH(), Config.GetFlags(), Config.GetStyle(), Config.GetExStyle(), Config.GetMenu(), pWndParent ) );
}

void CWnd::WndKill( void )
{
	if( ms_wAtom )
	{
		if( m_hWnd )
		{
			DestroyWindow( m_hWnd );
			m_hWnd = NULL;
			m_pWndParent = NULL;
			ms_uiRefs--;
		}
		if( !ms_uiRefs )
		{
			UnregisterClass( ms_wAtom, GetModuleHandle( NULL ) );
			ms_wAtom = NULL;
		}
	}
}

void CWnd::WndSetActiveUpdates( const bool set )
{
    if( set )
    {   //  add the window to the active updates window list
        if( m_pWndActiveNext == NULL )
        {   //  the window is not already in the active updates window list, so add it
            if( ms_pWndActiveList != NULL )
            {   //  the active updates window list is not empty so add the window to the end
                m_pWndActiveNext = ms_pWndActiveList;
                m_pWndActivePrev = ms_pWndActiveList->m_pWndActivePrev;
                m_pWndActiveNext->m_pWndActivePrev = m_pWndActivePrev->m_pWndActiveNext = this;
            }
            else
            {   //  the active updates window list is empty so point it to this window
                ms_pWndActiveList = this;
                m_pWndActiveNext = m_pWndActivePrev = this;
            }
        }
    }
    else
    {   //  remove the window from the active updates window list
        if( m_pWndActiveNext != NULL )
        {   //  the window is in the active updates window list, so remove it
            if( ms_pWndActiveList == this )
            {
                ms_pWndActiveList = ( ( m_pWndActiveNext == this ) ? NULL : m_pWndActiveNext );
            }
            m_pWndActiveNext->m_pWndActivePrev = m_pWndActivePrev;
            m_pWndActivePrev->m_pWndActiveNext = m_pWndActiveNext;
            m_pWndActiveNext = m_pWndActivePrev = NULL;
        }
    }
}

void CWnd::WndSetExclusiveUpdates( const bool set )
{
    if( set )
    {   //  enable exclusive updates
        ms_pWndExclusive = this;
    }
    else if( ms_pWndExclusive == this )
    {   //  disable exclusive updates
        ms_pWndExclusive = NULL;
    }
}

void CWnd::WndRunApp( void )
{   //  run this window as the main application
    WndSetAppObject();
    WndSetActiveUpdates();
    WndLoop();
    WndClearAppObject();
}

void CWnd::WndLoop( void )
{   //  pump the message queue for any open windows
    while( WndPump() )
    {
        if( ms_pWndExclusive != NULL )
        {   //  update the exclusive window
            if( !ms_pWndExclusive->WndUpdate() )
            {   //  an error has occurred
                return;
            }
        }
        else
        {   //  round-robin update of active windows
            if( ms_pWndActiveList != NULL )
            {
                if( !ms_pWndActiveList->WndUpdate() )
                {   //  an error has occurred
                    return;
                }
                if( ms_pWndActiveList != NULL )
                {   //  needed to check that the active list was not empty in case the update modified the active list
                    ms_pWndActiveList = ms_pWndActiveList->m_pWndActiveNext;
                }
            }
        }
    }
}

bool CWnd::WndPump( void )
{
	Sleep( 0 );
	if( ms_wMsg.message == WM_QUIT )
	{
		return( false );
	}
	while( PeekMessage( &ms_wMsg, NULL, 0, 0, PM_REMOVE ) )
	{
		if( ms_wMsg.message == WM_QUIT )
		{
			return( false );
		}
		TranslateMessage( &ms_wMsg );
		DispatchMessage( &ms_wMsg );
	}
	return( true );
}

bool CWnd::WndShow( bool bShow ) const
{
	if( m_hWnd )
	{
		ShowWindow( m_hWnd, ( bShow ? SW_SHOW : SW_HIDE ) );
		return( true );
	}
	return( false );
}

HWND CWnd::WndParentHandle( void ) const
{
	HWND hParent = NULL;
	if( m_hWnd )
	{
		hParent = GetParent( m_hWnd );
		if( hParent == NULL )
		{
			hParent = GetDesktopWindow();
		}
	}
	return( hParent );
}

bool CWnd::WndCenterWindow( void ) const
{
	if( m_hWnd )
	{
		RECT rectParent, rectWindow, rectTarget;
		WndGetParentScreenRect( m_hWnd, rectParent );
		WndGetWindowScreenRect( m_hWnd, rectWindow );
		WndCenterRect( rectParent, rectWindow, rectTarget );
		WndClampRect( rectTarget );
		WndSetWindowScreenRect( m_hWnd, rectWindow, rectTarget );
		return( true );
	}
	return( false );
}

bool CWnd::WndCenterClient( void ) const
{
	if( m_hWnd )
	{
		RECT rectParent, rectWindow, rectClient, rectTarget;
		WndGetParentScreenRect( m_hWnd, rectParent );
		WndGetWindowScreenRect( m_hWnd, rectWindow );
		WndGetClientScreenRect( m_hWnd, rectClient );
		WndCenterRect( rectParent, rectClient, rectTarget );
		WndClientToWindowSpace( rectWindow, rectClient, rectTarget );
		WndClampRect( rectTarget );
		WndSetWindowScreenRect( m_hWnd, rectWindow, rectTarget );
		return( true );
	}
	return( false );
}

bool CWnd::WndAdjustWindow( const LONG left, const LONG top, const LONG width, const LONG height ) const
{
	if( m_hWnd )
	{
		RECT rectWindow, rectTarget;
		WndGetWindowScreenRect( m_hWnd, rectWindow );
		rectTarget.left = left;
		rectTarget.top = top;
		rectTarget.right = ( width ? ( left + width ) : ( left + rectWindow.right - rectWindow.left ) );
		rectTarget.bottom = ( height ? ( top + height ) : ( top + rectWindow.bottom - rectWindow.top ) );
		WndSetWindowScreenRect( m_hWnd, rectWindow, rectTarget );
		return( true );
	}
	return( false );
}

bool CWnd::WndAdjustClient( const LONG left, const LONG top, const LONG width, const LONG height ) const
{
	if( m_hWnd )
	{
		RECT rectWindow, rectClient, rectTarget;
		WndGetWindowScreenRect( m_hWnd, rectWindow );
		WndGetClientScreenRect( m_hWnd, rectClient );
		rectTarget.left = left;
		rectTarget.top = top;
		rectTarget.right = ( width ? ( left + width ) : ( left + rectClient.right - rectClient.left ) );
		rectTarget.bottom = ( height ? ( top + height ) : ( top + rectClient.bottom - rectClient.top ) );
		WndClientToWindowSpace( rectWindow, rectClient, rectTarget );
		WndSetWindowScreenRect( m_hWnd, rectWindow, rectTarget );
		return( true );
	}
	return( false );
}

bool CWnd::WndResizeWindow( const LONG width, const LONG height, const CENTERING centering ) const
{
	if( m_hWnd )
	{
		RECT rectParent, rectWindow, rectClient, rectTarget;
		WndGetWindowScreenRect( m_hWnd, rectWindow );
		switch( centering )
		{
		case( CENTER_WINDOW ):
			{	//	center resized window to the parent windows client area
				WndGetParentScreenRect( m_hWnd, rectParent );
				WndCenterRect( rectParent, width, height, rectTarget );
				break;
			}
		case( CENTER_CLIENT ):
			{	//	center resized client to the parent windows client area
				WndGetParentScreenRect( m_hWnd, rectParent );
				WndGetClientScreenRect( m_hWnd, rectClient );
				WndCenterRect( rectParent, ( width - rectWindow.right + rectWindow.left + rectClient.right - rectClient.left ), ( height - rectWindow.bottom + rectWindow.top + rectClient.bottom - rectClient.top ), rectTarget );
				WndClientToWindowSpace( rectWindow, rectClient, rectTarget );
				break;
			}
		case( WINDOW_CENTER ):
			{	//	window center coordinate unchanged after operation
				WndCenterRect( rectWindow, width, height, rectTarget );
				break;
			}
		case( CLIENT_CENTER ):
			{	//	client center coordinate unchanged after operation
				WndGetClientScreenRect( m_hWnd, rectClient );
				WndCenterRect( rectClient, ( width - rectWindow.right + rectWindow.left + rectClient.right - rectClient.left ), ( height - rectWindow.bottom + rectWindow.top + rectClient.bottom - rectClient.top ), rectTarget );
				WndClientToWindowSpace( rectWindow, rectClient, rectTarget );
				break;
			}
		default:
			{	//	top left window coordinate unchanged after operation
				rectTarget.left = rectWindow.left;
				rectTarget.top = rectWindow.top;
				rectTarget.right = ( rectWindow.left + width );
				rectTarget.bottom = ( rectWindow.top + height );
				break;
			}
		}
		WndClampRect( rectTarget );
		WndSetWindowScreenRect( m_hWnd, rectWindow, rectTarget );
		return( true );
	}
	return( false );
}

bool CWnd::WndResizeClient( const LONG width, const LONG height, const CENTERING centering ) const
{
	if( m_hWnd )
	{
		RECT rectParent, rectWindow, rectClient, rectTarget;
		WndGetWindowScreenRect( m_hWnd, rectWindow );
		WndGetClientScreenRect( m_hWnd, rectClient );
		switch( centering )
		{
		    case( CENTER_WINDOW ):
			{	//	center resized window to the parent windows client area
				WndGetParentScreenRect( m_hWnd, rectParent );
				WndCenterRect( rectParent, ( width + rectWindow.right - rectWindow.left - rectClient.right + rectClient.left ), ( height + rectWindow.bottom - rectWindow.top - rectClient.bottom + rectClient.top ), rectTarget );
				break;
			}
		    case( CENTER_CLIENT ):
			{	//	center resized client to the parent windows client area
				WndGetParentScreenRect( m_hWnd, rectParent );
				WndCenterRect( rectParent, width, height, rectTarget );
				WndClientToWindowSpace( rectWindow, rectClient, rectTarget );
				break;
			}
		    case( WINDOW_CENTER ):
			{	//	window center coordinate unchanged after operation
				WndCenterRect( rectClient, ( width + rectWindow.right - rectWindow.left - rectClient.right + rectClient.left ), ( height + rectWindow.bottom - rectWindow.top - rectClient.bottom + rectClient.top ), rectTarget );
				break;
			}
		    case( CLIENT_CENTER ):
			{	//	client center coordinate unchanged after operation
				WndCenterRect( rectClient, width, height, rectTarget );
				WndClientToWindowSpace( rectWindow, rectClient, rectTarget );
				break;
			}
		    default:
			{	//	top left window coordinate unchanged after operation
				rectTarget.left = rectWindow.left;
				rectTarget.top = rectWindow.top;
				rectTarget.right = ( rectWindow.left + rectClient.left - rectClient.right + width );
				rectTarget.bottom = ( rectWindow.top + rectClient.top - rectClient.bottom + height );
				break;
			}
		}
		WndClampRect( rectTarget );
		WndSetWindowScreenRect( m_hWnd, rectWindow, rectTarget );
		return( true );
	}
	return( false );
}

bool CWnd::WndGetParentScreenRect( RECT& rectParent ) const
{
	if( m_hWnd )
	{
		WndGetParentScreenRect( m_hWnd, rectParent );
		return( true );
	}
	return( false );
}

bool CWnd::WndGetWindowScreenRect( RECT& rectWindow ) const
{
	if( m_hWnd )
	{
		WndGetWindowScreenRect( m_hWnd, rectWindow );
		return( true );
	}
	return( false );
}

bool CWnd::WndGetClientScreenRect( RECT& rectClient ) const
{
	if( m_hWnd )
	{
		WndGetClientScreenRect( m_hWnd, rectClient );
		return( true );
	}
	return( false );
}

void CWnd::WndGetParentScreenRect( const HWND hWnd, RECT& rectParent )
{
	HWND hParent = GetParent( hWnd );
	if( hParent == NULL )
	{
		hParent = GetDesktopWindow();
	}
	WndGetClientScreenRect( hParent, rectParent );
}

void CWnd::WndGetWindowScreenRect( const HWND hWnd, RECT& rectWindow )
{
	GetWindowRect( hWnd, &rectWindow );
}

void CWnd::WndGetClientScreenRect( const HWND hWnd, RECT& rectClient )
{
	GetClientRect( hWnd, &rectClient );
	WndClientToScreenSpace( hWnd, rectClient );
}

void CWnd::WndSetWindowScreenRect( const HWND hWnd, const RECT& rectWindow, const RECT& rectTarget )
{
	if( ( rectWindow.left != rectTarget.left ) || ( rectWindow.top != rectTarget.top ) || ( rectWindow.right != rectTarget.right ) || ( rectWindow.bottom != rectTarget.bottom ) )
	{
		MoveWindow( hWnd, rectTarget.left, rectTarget.top, ( rectTarget.right - rectTarget.left ), ( rectTarget.bottom - rectTarget.top ), TRUE );
	}
}

void CWnd::WndClientToWindowSpace( const RECT& rectWindow, const RECT& rectClient, RECT& rectTarget )
{
	rectTarget.left += ( rectWindow.left - rectClient.left );
	rectTarget.top += ( rectWindow.top - rectClient.top );
	rectTarget.right += ( rectWindow.right - rectClient.right );
	rectTarget.bottom += ( rectWindow.bottom - rectClient.bottom );
}

void CWnd::WndClientToScreenSpace( const HWND hWnd, RECT& rectClient )
{
	ClientToScreen( hWnd, reinterpret_cast< POINT* >( &rectClient.left ) );
	ClientToScreen( hWnd, reinterpret_cast< POINT* >( &rectClient.right ) );
}

void CWnd::WndCenterRect( const RECT& rectParent, const RECT& rectCenter, RECT& rectCentered )
{
	WndCenterRect( rectParent, ( rectCenter.right - rectCenter.left ), ( rectCenter.bottom - rectCenter.top ), rectCentered );
}

void CWnd::WndCenterRect( const RECT& rectParent, const LONG width, const LONG height, RECT& rectCentered )
{
	rectCentered.left = ( ( rectParent.left + rectParent.right - width ) >> 1 );
	rectCentered.top = ( ( rectParent.top + rectParent.bottom - height ) >> 1 );
	rectCentered.right = ( rectCentered.left + width );
	rectCentered.bottom = ( rectCentered.top + height );
}

void CWnd::WndIndentRect( const RECT& rectIndent, const LONG indent, RECT& rectIndented )
{
	rectIndented.left = ( rectIndent.left + indent );
	rectIndented.top = ( rectIndent.top + indent );
	rectIndented.right = ( rectIndent.right - indent );
	rectIndented.bottom = ( rectIndent.bottom - indent );
}

void CWnd::WndIndentRect( const RECT& rectIndent, const LONG indentTopLeft, const LONG indentBottomRight, RECT& rectIndented )
{
	rectIndented.left = ( rectIndent.left + indentTopLeft );
	rectIndented.top = ( rectIndent.top + indentTopLeft );
	rectIndented.right = ( rectIndent.right - indentBottomRight );
	rectIndented.bottom = ( rectIndent.bottom - indentBottomRight );
}

void CWnd::WndClampRect( RECT& rectClamp )
{
	if( rectClamp.left < 0 )
	{
		rectClamp.right -= rectClamp.left;
		rectClamp.left = 0;
	}
	if( rectClamp.top < 0 )
	{
		rectClamp.bottom -= rectClamp.top;
		rectClamp.top = 0;
	}
}

void CWnd::WndDrawRectEtch( const HDC hDC, const RECT& rectEtch, const COLORREF uLight, const COLORREF uShade, const LONG indent, const LONG width )
{
	RECT rectDraw;
	WndIndentRect( rectEtch, indent, ( indent + 1 ), rectDraw );
	HPEN hPen = reinterpret_cast< HPEN >( SelectObject(hDC, CreatePen(PS_SOLID, 0, uShade ) ) );
	for( LONG shift = 0; shift < width; ++shift )
	{
		MoveToEx( hDC, ( rectDraw.right - shift - width ), ( rectDraw.top + shift ), NULL );
		LineTo( hDC, ( rectDraw.right - shift - width ), ( rectDraw.bottom - shift - width ) );
		LineTo( hDC, ( rectDraw.left + shift ), ( rectDraw.bottom - shift - width ) );
		LineTo( hDC, ( rectDraw.left + shift ), ( rectDraw.top + shift ) );
		LineTo( hDC, ( rectDraw.right - shift - width ), ( rectDraw.top + shift ) );
	}
	DeleteObject( SelectObject( hDC, hPen ) );
	hPen = reinterpret_cast< HPEN >( SelectObject( hDC, CreatePen( PS_SOLID, 0, uLight ) ) );
	for( LONG shift = 0; shift < width; ++shift )
	{
		MoveToEx( hDC, ( rectDraw.right - shift ), rectDraw.top, NULL );
		LineTo( hDC, ( rectDraw.right - shift ), ( rectDraw.bottom - shift ) );
		LineTo( hDC, ( rectDraw.left - 1 ), ( rectDraw.bottom - shift ) );
		MoveToEx( hDC, ( rectDraw.left + shift + width ), ( rectDraw.bottom - width - width ), NULL );
		LineTo( hDC, ( rectDraw.left + shift + width ), ( rectDraw.top + shift + width ) );
		LineTo( hDC, ( rectDraw.right - width - width + 1 ), ( rectDraw.top + shift + width ) );
	}
	DeleteObject( SelectObject( hDC, hPen ) );
}

void CWnd::WndDrawRectDrop( const HDC hDC, const RECT& rectDrop, const COLORREF uLight, const COLORREF uShade, const LONG indent, const LONG width )
{
	RECT rectDraw;
	WndIndentRect( rectDrop, indent, ( indent + 1 ), rectDraw );
	HPEN hPen = reinterpret_cast< HPEN >( SelectObject( hDC, CreatePen( PS_SOLID, 0, uShade ) ) );
	for( LONG shift = 0; shift < width; ++shift )
	{
		MoveToEx( hDC, ( rectDraw.left + shift ), ( rectDraw.bottom - shift ), NULL );
		LineTo( hDC, ( rectDraw.left + shift ), ( rectDraw.top + shift ) );
		LineTo( hDC, ( rectDraw.right - shift ), ( rectDraw.top + shift ) );
	}
	DeleteObject( SelectObject( hDC, hPen ) );
	hPen = reinterpret_cast< HPEN >( SelectObject( hDC, CreatePen( PS_SOLID, 0, uLight ) ) );
	for( LONG shift = 0; shift < width; ++shift )
	{
		MoveToEx( hDC, ( rectDraw.right - shift ), ( rectDraw.top + shift ), NULL );
		LineTo( hDC, ( rectDraw.right - shift ), ( rectDraw.bottom - shift ) );
		LineTo( hDC, ( rectDraw.left + shift ), ( rectDraw.bottom - shift ) );
	}
	DeleteObject( SelectObject( hDC, hPen ) );
}

void CWnd::WndDrawRectLift( const HDC hDC, const RECT& rectLift, const COLORREF uLight, const COLORREF uShade, const LONG indent, const LONG width )
{
	RECT rectDraw;
	WndIndentRect( rectLift, indent, ( indent + 1 ), rectDraw );
	HPEN hPen = reinterpret_cast< HPEN >( SelectObject( hDC, CreatePen( PS_SOLID, 0, uShade ) ) );
	for( LONG shift = 0; shift < width; ++shift )
	{
		MoveToEx( hDC, ( rectDraw.left + shift ), ( rectDraw.bottom - shift ), NULL );
		LineTo( hDC, ( rectDraw.right - shift ), ( rectDraw.bottom - shift ) );
		LineTo( hDC, ( rectDraw.right - shift ), ( rectDraw.top + shift ) );
	}
	DeleteObject( SelectObject( hDC, hPen ) );
	hPen = reinterpret_cast< HPEN >( SelectObject( hDC, CreatePen( PS_SOLID, 0, uLight ) ) );
	for( LONG shift = 0; shift < width; ++shift )
	{
		MoveToEx( hDC, ( rectDraw.right - shift ), ( rectDraw.top + shift ), NULL );
		LineTo( hDC, ( rectDraw.left + shift ), ( rectDraw.top + shift ) );
		LineTo( hDC, ( rectDraw.left + shift ), ( rectDraw.bottom - shift ) );
	}
	DeleteObject( SelectObject( hDC, hPen ) );
}

void CWnd::WndDrawRectEdge( const HDC hDC, const RECT& rectEdge, const COLORREF uColour, const LONG indent, const LONG width )
{
	RECT rectDraw;
	WndIndentRect( rectEdge, indent, ( indent + 1 ), rectDraw );
	HPEN hPen = reinterpret_cast< HPEN >( SelectObject( hDC, CreatePen( PS_SOLID, 0, uColour ) ) );
	for( LONG shift = 0; shift < width; ++shift )
	{
		MoveToEx( hDC, ( rectDraw.right - shift ), ( rectDraw.top + shift ), NULL );
		LineTo( hDC, ( rectDraw.right - shift ), ( rectDraw.bottom - shift ) );
		LineTo( hDC, ( rectDraw.left + shift ), ( rectDraw.bottom - shift ) );
		LineTo( hDC, ( rectDraw.left + shift ), ( rectDraw.top + shift ) );
		LineTo( hDC, ( rectDraw.right - shift ), ( rectDraw.top + shift ) );
	}
	DeleteObject( SelectObject( hDC, hPen ) );
}

void CWnd::WndDrawRectFill( const HDC hDC, const RECT& rectFill, const COLORREF uColour, const LONG indent )
{
	HPEN hPen = reinterpret_cast< HPEN >( SelectObject( hDC, CreatePen( PS_SOLID, 0, uColour ) ) );
	HBRUSH hBrush = reinterpret_cast< HBRUSH >( SelectObject( hDC, CreateSolidBrush( uColour ) ) );
	Rectangle( hDC, ( rectFill.left + indent ), ( rectFill.top + indent ), ( rectFill.right - indent ), ( rectFill.bottom - indent ) );
	DeleteObject( SelectObject( hDC, hBrush ) );
	DeleteObject( SelectObject( hDC, hPen ) );
}

HFONT CWnd::WndCreateFont( const char* pszName, const int iPointSize, const int iWeight, const BOOL bItalic, const BOOL bUnderline, const BOOL bStrikeout )
{
    HDC hDC = GetDC( NULL );
    int iMapMode = GetMapMode( hDC );
    SetMapMode( hDC, MM_TEXT );
	HFONT hFont = CreateFont( -MulDiv( iPointSize, GetDeviceCaps( hDC, LOGPIXELSY ), 72 ), 0, 0, 0,( iWeight * 100 ), bItalic, bUnderline, bStrikeout, DEFAULT_CHARSET, 0, 0, 0, 0, pszName );
    SetMapMode( hDC, iMapMode );
    ReleaseDC( NULL, hDC );
    return( hFont );
}

LRESULT CALLBACK CWnd::WndProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lr = 0;
	if( uiMsg == WM_CREATE )
	{
        ++ms_uiOpen;
#pragma warning( push )
#pragma warning( disable : 4244 )
		SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( reinterpret_cast< CREATESTRUCT* >( lParam )->lpCreateParams ) );
#pragma warning( pop )
	}
	CWnd* pWnd = reinterpret_cast< CWnd* >( static_cast< LONG_PTR >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) ) );
	if( pWnd )
	{
		if( uiMsg == WM_DESTROY )
		{
			SetWindowLongPtr( hWnd, GWLP_USERDATA, 0 );
            pWnd->WndStopLoopUpdates();
            --ms_uiOpen;
            if( ( ms_uiOpen == 0 ) || ( pWnd == ms_pWndAppObject ) )
            {
			    PostQuitMessage( 0 );
            }
		}
		lr = pWnd->WndUser( hWnd, uiMsg, wParam, lParam );
	}
	else
	{
		if( uiMsg == WM_DESTROY )
		{
            --ms_uiOpen;
            if( ms_uiOpen == 0 )
            {
			    PostQuitMessage( 0 );
            }
		}
		lr = DefWindowProc( hWnd, uiMsg, wParam, lParam );
	}
	return( lr );
}

bool CWnd::WndUpdate( void )
{
    return( true );
}

bool CWnd::WndDraw( void )
{
    return( true );
}

LRESULT CWnd::WndUser( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	return( DefWindowProc( hWnd, uiMsg, wParam, lParam ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWnd debug and trace functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

void CWnd::WndTrace( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    (void)( hWnd );
    (void)( lParam );
	char szTempString[ 256 ];
	switch( uiMsg )
	{
	case( WM_SHOWWINDOW ):
		{
			sprintf_s( szTempString, 256, "WM_SHOWWINDOW = %s\n", ( ( wParam == TRUE ) ? "TRUE" : "FALSE" ) );
			OutputDebugString( szTempString );
			break;
		}
	case( WM_ACTIVATEAPP ):
		{
			sprintf_s( szTempString, 256, "WM_ACTIVATEAPP = %s\n", ( ( wParam == TRUE ) ? "TRUE" : "FALSE" ) );
			OutputDebugString( szTempString );
			break;
		}
	case( WM_NCACTIVATE ):
		{
			sprintf_s( szTempString, 256, "WM_NCACTIVATE = %s\n", ( ( wParam == TRUE ) ? "TRUE" : "FALSE" ) );
			OutputDebugString( szTempString );
			break;
		}
	case( WM_ACTIVATE ):
		{
			sprintf_s( szTempString, 256, "WM_ACTIVATE = %s\n", ( ( wParam == TRUE ) ? "TRUE" : "FALSE" ) );
			OutputDebugString( szTempString );
			break;
		}
	default:
		{
			for( const WM_NAME* pWM_NAME = mk_WM_NAMES; pWM_NAME->pszName; ++pWM_NAME )
			{
				if( uiMsg == pWM_NAME->uiMsg )
				{
			        sprintf_s( szTempString, 256, "%s wParam=0x%08x lParam=0x%08x\n", pWM_NAME->pszName, wParam, lParam );
			        OutputDebugString( szTempString );
					break;
				}
			}
			break;
		}
	}
}

#define MK_WM_NAME( wm )    { wm, #wm }

const CWnd::WM_NAME CWnd::mk_WM_NAMES[] =
{
    MK_WM_NAME( WM_NULL ),
    MK_WM_NAME( WM_CREATE ),
    MK_WM_NAME( WM_DESTROY ),
    MK_WM_NAME( WM_MOVE ),
    MK_WM_NAME( WM_SIZE ),
    MK_WM_NAME( WM_ACTIVATE ),
    MK_WM_NAME( WM_SETFOCUS ),
    MK_WM_NAME( WM_KILLFOCUS ),
    MK_WM_NAME( WM_ENABLE ),
    MK_WM_NAME( WM_SETREDRAW ),
    MK_WM_NAME( WM_SETTEXT ),
    MK_WM_NAME( WM_GETTEXT ),
    MK_WM_NAME( WM_GETTEXTLENGTH ),
    MK_WM_NAME( WM_PAINT ),
    MK_WM_NAME( WM_CLOSE ),
    MK_WM_NAME( WM_QUERYENDSESSION ),
    MK_WM_NAME( WM_QUIT ),
    MK_WM_NAME( WM_QUERYOPEN ),
    MK_WM_NAME( WM_ERASEBKGND ),
    MK_WM_NAME( WM_SYSCOLORCHANGE ),
    MK_WM_NAME( WM_ENDSESSION ),
    MK_WM_NAME( WM_SHOWWINDOW ),
    MK_WM_NAME( WM_WININICHANGE ),
    MK_WM_NAME( WM_DEVMODECHANGE ),
    MK_WM_NAME( WM_ACTIVATEAPP ),
    MK_WM_NAME( WM_FONTCHANGE ),
    MK_WM_NAME( WM_TIMECHANGE ),
    MK_WM_NAME( WM_CANCELMODE ),
    MK_WM_NAME( WM_SETCURSOR ),
    MK_WM_NAME( WM_MOUSEACTIVATE ),
    MK_WM_NAME( WM_CHILDACTIVATE ),
    MK_WM_NAME( WM_QUEUESYNC ),
    MK_WM_NAME( WM_GETMINMAXINFO ),
    MK_WM_NAME( WM_PAINTICON ),
    MK_WM_NAME( WM_ICONERASEBKGND ),
    MK_WM_NAME( WM_NEXTDLGCTL ),
    MK_WM_NAME( WM_SPOOLERSTATUS ),
    MK_WM_NAME( WM_DRAWITEM ),
    MK_WM_NAME( WM_MEASUREITEM ),
    MK_WM_NAME( WM_DELETEITEM ),
    MK_WM_NAME( WM_VKEYTOITEM ),
    MK_WM_NAME( WM_CHARTOITEM ),
    MK_WM_NAME( WM_SETFONT ),
    MK_WM_NAME( WM_GETFONT ),
    MK_WM_NAME( WM_SETHOTKEY ),
    MK_WM_NAME( WM_GETHOTKEY ),
    MK_WM_NAME( WM_QUERYDRAGICON ),
    MK_WM_NAME( WM_COMPAREITEM ),
    MK_WM_NAME( WM_COMPACTING ),
    MK_WM_NAME( WM_COMMNOTIFY ),
    MK_WM_NAME( WM_WINDOWPOSCHANGING ),
    MK_WM_NAME( WM_WINDOWPOSCHANGED ),
    MK_WM_NAME( WM_POWER ),
    MK_WM_NAME( WM_COPYDATA ),
    MK_WM_NAME( WM_CANCELJOURNAL ),
    MK_WM_NAME( WM_NOTIFY ),
    MK_WM_NAME( WM_INPUTLANGCHANGEREQUEST ),
    MK_WM_NAME( WM_INPUTLANGCHANGE ),
    MK_WM_NAME( WM_TCARD ),
    MK_WM_NAME( WM_HELP ),
    MK_WM_NAME( WM_USERCHANGED ),
    MK_WM_NAME( WM_NOTIFYFORMAT ),
    MK_WM_NAME( WM_CONTEXTMENU ),
    MK_WM_NAME( WM_STYLECHANGING ),
    MK_WM_NAME( WM_STYLECHANGED ),
    MK_WM_NAME( WM_DISPLAYCHANGE ),
    MK_WM_NAME( WM_GETICON ),
    MK_WM_NAME( WM_SETICON ),
    MK_WM_NAME( WM_NCCREATE ),
    MK_WM_NAME( WM_NCDESTROY ),
    MK_WM_NAME( WM_NCCALCSIZE ),
    MK_WM_NAME( WM_NCHITTEST ),
    MK_WM_NAME( WM_NCPAINT ),
    MK_WM_NAME( WM_NCACTIVATE ),
    MK_WM_NAME( WM_GETDLGCODE ),
    MK_WM_NAME( WM_SYNCPAINT ),
    MK_WM_NAME( WM_NCMOUSEMOVE ),
    MK_WM_NAME( WM_NCLBUTTONDOWN ),
    MK_WM_NAME( WM_NCLBUTTONUP ),
    MK_WM_NAME( WM_NCLBUTTONDBLCLK ),
    MK_WM_NAME( WM_NCRBUTTONDOWN ),
    MK_WM_NAME( WM_NCRBUTTONUP ),
    MK_WM_NAME( WM_NCRBUTTONDBLCLK ),
    MK_WM_NAME( WM_NCMBUTTONDOWN ),
    MK_WM_NAME( WM_NCMBUTTONUP ),
    MK_WM_NAME( WM_NCMBUTTONDBLCLK ),
    MK_WM_NAME( WM_NCXBUTTONDOWN ),
    MK_WM_NAME( WM_NCXBUTTONUP ),
    MK_WM_NAME( WM_NCXBUTTONDBLCLK ),
    MK_WM_NAME( WM_INPUT_DEVICE_CHANGE ),
    MK_WM_NAME( WM_INPUT ),
    MK_WM_NAME( WM_KEYDOWN ),
    MK_WM_NAME( WM_KEYUP ),
    MK_WM_NAME( WM_CHAR ),
    MK_WM_NAME( WM_DEADCHAR ),
    MK_WM_NAME( WM_SYSKEYDOWN ),
    MK_WM_NAME( WM_SYSKEYUP ),
    MK_WM_NAME( WM_SYSCHAR ),
    MK_WM_NAME( WM_SYSDEADCHAR ),
    MK_WM_NAME( WM_UNICHAR ),
    MK_WM_NAME( WM_IME_STARTCOMPOSITION ),
    MK_WM_NAME( WM_IME_ENDCOMPOSITION ),
    MK_WM_NAME( WM_IME_COMPOSITION ),
    MK_WM_NAME( WM_INITDIALOG ),
    MK_WM_NAME( WM_COMMAND ),
    MK_WM_NAME( WM_SYSCOMMAND ),
    MK_WM_NAME( WM_TIMER ),
    MK_WM_NAME( WM_HSCROLL ),
    MK_WM_NAME( WM_VSCROLL ),
    MK_WM_NAME( WM_INITMENU ),
    MK_WM_NAME( WM_INITMENUPOPUP ),
    MK_WM_NAME( WM_MENUSELECT ),
    MK_WM_NAME( WM_MENUCHAR ),
    MK_WM_NAME( WM_ENTERIDLE ),
    MK_WM_NAME( WM_MENURBUTTONUP ),
    MK_WM_NAME( WM_MENUDRAG ),
    MK_WM_NAME( WM_MENUGETOBJECT ),
    MK_WM_NAME( WM_UNINITMENUPOPUP ),
    MK_WM_NAME( WM_MENUCOMMAND ),
    MK_WM_NAME( WM_CHANGEUISTATE ),
    MK_WM_NAME( WM_UPDATEUISTATE ),
    MK_WM_NAME( WM_QUERYUISTATE ),
    MK_WM_NAME( WM_CTLCOLORMSGBOX ),
    MK_WM_NAME( WM_CTLCOLOREDIT ),
    MK_WM_NAME( WM_CTLCOLORLISTBOX ),
    MK_WM_NAME( WM_CTLCOLORBTN ),
    MK_WM_NAME( WM_CTLCOLORDLG ),
    MK_WM_NAME( WM_CTLCOLORSCROLLBAR ),
    MK_WM_NAME( WM_CTLCOLORSTATIC ),
    MK_WM_NAME( MN_GETHMENU ),
    MK_WM_NAME( WM_MOUSEMOVE ),
    MK_WM_NAME( WM_LBUTTONDOWN ),
    MK_WM_NAME( WM_LBUTTONUP ),
    MK_WM_NAME( WM_LBUTTONDBLCLK ),
    MK_WM_NAME( WM_RBUTTONDOWN ),
    MK_WM_NAME( WM_RBUTTONUP ),
    MK_WM_NAME( WM_RBUTTONDBLCLK ),
    MK_WM_NAME( WM_MBUTTONDOWN ),
    MK_WM_NAME( WM_MBUTTONUP ),
    MK_WM_NAME( WM_MBUTTONDBLCLK ),
    MK_WM_NAME( WM_MOUSEWHEEL ),
    MK_WM_NAME( WM_XBUTTONDOWN ),
    MK_WM_NAME( WM_XBUTTONUP ),
    MK_WM_NAME( WM_XBUTTONDBLCLK ),
    MK_WM_NAME( WM_MOUSEHWHEEL ),
    MK_WM_NAME( WM_PARENTNOTIFY ),
    MK_WM_NAME( WM_ENTERMENULOOP ),
    MK_WM_NAME( WM_EXITMENULOOP ),
    MK_WM_NAME( WM_NEXTMENU ),
    MK_WM_NAME( WM_SIZING ),
    MK_WM_NAME( WM_CAPTURECHANGED ),
    MK_WM_NAME( WM_MOVING ),
    MK_WM_NAME( WM_POWERBROADCAST ),
    MK_WM_NAME( WM_DEVICECHANGE ),
    MK_WM_NAME( WM_MDICREATE ),
    MK_WM_NAME( WM_MDIDESTROY ),
    MK_WM_NAME( WM_MDIACTIVATE ),
    MK_WM_NAME( WM_MDIRESTORE ),
    MK_WM_NAME( WM_MDINEXT ),
    MK_WM_NAME( WM_MDIMAXIMIZE ),
    MK_WM_NAME( WM_MDITILE ),
    MK_WM_NAME( WM_MDICASCADE ),
    MK_WM_NAME( WM_MDIICONARRANGE ),
    MK_WM_NAME( WM_MDIGETACTIVE ),
    MK_WM_NAME( WM_MDISETMENU ),
    MK_WM_NAME( WM_ENTERSIZEMOVE ),
    MK_WM_NAME( WM_EXITSIZEMOVE ),
    MK_WM_NAME( WM_DROPFILES ),
    MK_WM_NAME( WM_MDIREFRESHMENU ),
    MK_WM_NAME( WM_IME_SETCONTEXT ),
    MK_WM_NAME( WM_IME_NOTIFY ),
    MK_WM_NAME( WM_IME_CONTROL ),
    MK_WM_NAME( WM_IME_COMPOSITIONFULL ),
    MK_WM_NAME( WM_IME_SELECT ),
    MK_WM_NAME( WM_IME_CHAR ),
    MK_WM_NAME( WM_IME_REQUEST ),
    MK_WM_NAME( WM_IME_KEYDOWN ),
    MK_WM_NAME( WM_IME_KEYUP ),
    MK_WM_NAME( WM_MOUSEHOVER ),
    MK_WM_NAME( WM_MOUSELEAVE ),
    MK_WM_NAME( WM_NCMOUSEHOVER ),
    MK_WM_NAME( WM_NCMOUSELEAVE ),
    MK_WM_NAME( WM_WTSSESSION_CHANGE ),
    MK_WM_NAME( WM_CUT ),
    MK_WM_NAME( WM_COPY ),
    MK_WM_NAME( WM_PASTE ),
    MK_WM_NAME( WM_CLEAR ),
    MK_WM_NAME( WM_UNDO ),
    MK_WM_NAME( WM_RENDERFORMAT ),
    MK_WM_NAME( WM_RENDERALLFORMATS ),
    MK_WM_NAME( WM_DESTROYCLIPBOARD ),
    MK_WM_NAME( WM_DRAWCLIPBOARD ),
    MK_WM_NAME( WM_PAINTCLIPBOARD ),
    MK_WM_NAME( WM_VSCROLLCLIPBOARD ),
    MK_WM_NAME( WM_SIZECLIPBOARD ),
    MK_WM_NAME( WM_ASKCBFORMATNAME ),
    MK_WM_NAME( WM_CHANGECBCHAIN ),
    MK_WM_NAME( WM_HSCROLLCLIPBOARD ),
    MK_WM_NAME( WM_QUERYNEWPALETTE ),
    MK_WM_NAME( WM_PALETTEISCHANGING ),
    MK_WM_NAME( WM_PALETTECHANGED ),
    MK_WM_NAME( WM_HOTKEY ),
    MK_WM_NAME( WM_PRINT ),
    MK_WM_NAME( WM_PRINTCLIENT ),
    MK_WM_NAME( WM_APPCOMMAND ),
    MK_WM_NAME( WM_THEMECHANGED ),
    MK_WM_NAME( WM_CLIPBOARDUPDATE ),
    MK_WM_NAME( WM_DWMCOMPOSITIONCHANGED ),
    MK_WM_NAME( WM_DWMNCRENDERINGCHANGED ),
    MK_WM_NAME( WM_DWMCOLORIZATIONCOLORCHANGED ),
    MK_WM_NAME( WM_DWMWINDOWMAXIMIZEDCHANGE ),
    MK_WM_NAME( WM_GETTITLEBARINFOEX ),
    MK_WM_NAME( WM_USER ),
    MK_WM_NAME( WM_APP ),
    { 0xFFFF, NULL }
};

#endif	//	#ifdef _DEBUG

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
