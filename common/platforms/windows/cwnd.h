
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:    cwnd.h
////    Author:    Ritchie Brannan
////    Date:    11 Nov 10
////
////    Description:
////
////        C++ wrapper for windows.
////        All windows should be derived from CWnd.
////
////    Notes:
////
////        WndCreateFont font name reference:
////
////            Standard Raster fonts:
////
////                "System"
////                "FixedSys"
////                "Terminal"
////                "Courier"
////                "MS Serif"
////                "MS Sans Serif"
////                "Small Fonts"
////
////            Standard TrueType fonts:
////
////                "Arial"
////                "Arial Bold"
////                "Arial Italic"
////                "Arial Bold Italic"
////                "Courier New"
////                "Courier New Bold"
////                "Courier New Italic"
////                "Courier New Bold Italic"
////                "Times New Roman"
////                "Times New Roman Bold"
////                "Times New Roman Italic"
////                "Times New Roman Bold Italic"
////                "Symbol"
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

#ifndef __CWND_INCLUDED__
#define __CWND_INCLUDED__

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
////    CENTERING enumeration for the behaviour of window resize operations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum CENTERING
{
    UNCENTERED = 0,         //    top left window coordinate unchanged after operation
    CENTER_WINDOW,          //    center resized window to the parent windows client area
    CENTER_CLIENT,          //    center resized client to the parent windows client area
    WINDOW_CENTER,          //    window center coordinate unchanged after operation
    CLIENT_CENTER,          //    client center coordinate unchanged after operation
    CENTERING_FORCE32 = 0x7fffffff
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    dwFlags window positioning and sizing control values
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define    WF_POSWINDOW         0        //    the position describes the top left coordinate of the window
#define    WF_POSCLIENT         1        //    the position describes the top left coordinate of the client
#define    WF_CENTERWINDOW      2        //    the window is centered relative to it's parent client (position ignored)
#define    WF_CENTERCLIENT      3        //    the client is centered relative to it's parent client (position ignored)
#define    WF_SIZEWINDOW        0        //    the width and height refer to the window
#define    WF_SIZECLIENT        4        //    the width and height refer to the client
#define    WF_CENTERMASK        2        //    a mask bit to check if dwFlags includes centering options

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    default values for dwFlags (WF_DEFAULT), dwStyle (WS_DEFAULT) and dwExStyle (WS_EX_DEFAULT)
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WF_DEFAULT      ( WF_POSWINDOW | WF_SIZEWINDOW )
#define WS_DEFAULT      ( WS_VISIBLE | WS_SYSMENU | WS_CAPTION )
#define WS_EX_DEFAULT   ( WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW )    //  ( WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW | WS_EX_TOPMOST )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWndConfig class, window creation configuration management
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CWndConfig
{
public:
    inline                      CWndConfig() { SetDefaults(); };
    inline                      CWndConfig( const CWndConfig& Config ) { Set( Config ); };
    inline                      CWndConfig(  const LONG left, const LONG top, const LONG width, const LONG height, const DWORD dwFlags, const DWORD dwStyle, const DWORD dwExStyle, const HMENU hMenu = NULL ) { Set(  left, top, width, height, dwFlags, dwStyle, dwExStyle, hMenu ); };
    inline                      ~CWndConfig() {};
    inline void                 SetDefaults( void ) { Set( mk_DEFAULT ); };
    static const CWndConfig&    GetDefaults( void ) { return( mk_DEFAULT ); };
    inline void                 SetToolWindowConfig( void ) { SetDefaults(); SetStyles( ( WS_VISIBLE | WS_SYSMENU | WS_CAPTION ), WS_EX_PALETTEWINDOW ); };
    inline void                 SetToolWindowConfig( const LONG width, const LONG height ) { SetToolWindowConfig(); CenterClient( width, height ); };
    inline void                 SetStandardAppConfig( void ) { SetDefaults(); SetStyles( ( WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX ), ( WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW ) ); };
    inline void                 SetStandardAppConfig( const LONG width, const LONG height ) { SetStandardAppConfig(); CenterClient( width, height ); };
    inline void                 SetFramelessAppConfig( void ) { SetDefaults(); SetStyles( ( WS_VISIBLE | WS_SYSMENU | WS_POPUP ), 0 ); };
    inline void                 SetFramelessAppConfig( const LONG width, const LONG height ) { SetFramelessAppConfig(); CenterClient( width, height ); };
    void                        Set( const CWndConfig& Config );
    void                        Set( const LONG left, const LONG top, const LONG width, const LONG height, const DWORD dwFlags, const DWORD dwStyle, const DWORD dwExStyle, const HMENU hMenu = NULL );
    inline void                 SetX( const LONG left ) { m_left = left; };
    inline LONG                 GetX( void ) const { return( m_left ); };
    inline void                 SetY( const LONG top ) { m_top = top; };
    inline LONG                 GetY( void ) const { return( m_top ); };
    inline void                 SetW( const LONG width ) { m_width = width; };
    inline LONG                 GetW( void ) const { return( m_width ); };
    inline void                 SetH( const LONG height ) { m_height = height; };
    inline LONG                 GetH( void ) const { return( m_height ); };
    inline void                 SetFlags( const DWORD dwFlags ) { m_dwFlags = dwFlags; };
    inline DWORD                GetFlags( void ) const { return( m_dwFlags ); };
    inline void                 SetStyle( const DWORD dwStyle ) { m_dwStyle = dwStyle; };
    inline DWORD                GetStyle( void ) const { return( m_dwStyle ); };
    inline void                 SetExStyle( const DWORD dwExStyle ) { m_dwExStyle = dwExStyle; };
    inline DWORD                GetExStyle( void ) const { return( m_dwExStyle ); };
    inline void                 SetStyles( const DWORD dwStyle, const DWORD dwExStyle ) { m_dwStyle = dwStyle; m_dwExStyle = dwExStyle; };
    inline void                 SetMenu( const HMENU hMenu ) { m_hMenu = hMenu; };
    inline HMENU                GetMenu( void ) const { return( m_hMenu ); };
    bool                        GetWindowSize( LONG& width, LONG& height ) const;
    void                        SetDefaultWindowPos( void );
    void                        SetDefaultWindowSize( void );
    void                        SetDefaultWindowRect( void );
    void                        SetWindow( const LONG left, const LONG top, const LONG width, const LONG height );
    void                        SetWindowPos( const LONG left, const LONG top );
    void                        SetWindowSize( const LONG width, const LONG height );
    void                        SetWindowRect( const LONG left, const LONG top, const LONG right, const LONG bottom );
    void                        SetWindowRect( const RECT& rectWindow );
    void                        CenterWindow( void );
    void                        CenterWindow( const LONG width, const LONG height );
    void                        SetClient( const LONG left, const LONG top, const LONG width, const LONG height );
    void                        SetClientPos( const LONG left, const LONG top );
    void                        SetClientSize( const LONG width, const LONG height );
    void                        SetClientRect( const LONG left, const LONG top, const LONG right, const LONG bottom );
    void                        SetClientRect( const RECT& rectClient );
    void                        CenterClient( void );
    void                        CenterClient( const LONG width, const LONG height );
    inline const CWndConfig&    operator=( const CWndConfig& Config ) { Set( Config ); return( *this ); };
protected:
    LONG                        m_left, m_top;
    LONG                        m_width, m_height;
    DWORD                       m_dwFlags;
    DWORD                       m_dwStyle;
    DWORD                       m_dwExStyle;
    HMENU                       m_hMenu;
private:
    static const CWndConfig     mk_DEFAULT;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CWnd class, base class for all windows
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CWnd
{
public:
    inline                      CWnd() : m_hWnd( NULL ), m_pWndParent( NULL ) {};
    inline                      CWnd( const LPCTSTR pszName, const LONG left, const LONG top, const LONG width, const LONG height, const DWORD dwFlags = WF_DEFAULT, const DWORD dwStyle = WS_DEFAULT, const DWORD dwExStyle = WS_EX_DEFAULT, const HMENU hMenu = NULL, CWnd* const pWndParent = NULL ) : m_hWnd( NULL ), m_pWndParent( NULL ) { WndInit( pszName, left, top, width, height, dwFlags, dwStyle, dwExStyle, hMenu, pWndParent ); };
    inline                      CWnd( const LPCTSTR pszName, const CWndConfig& Config, CWnd* const pWndParent = NULL ) : m_hWnd( NULL ), m_pWndParent( NULL ) { WndInit( pszName, Config, pWndParent ); };
    inline                      CWnd( const LPCTSTR pszName, CWnd* const pWndParent = NULL ) : m_hWnd( NULL ), m_pWndParent( NULL ) { WndInit( pszName, CWndConfig::GetDefaults(), pWndParent ); };
    virtual                     ~CWnd() { WndKill(); };
    inline bool                 WndIsOK( void ) const { return( m_hWnd ? true : false ); };
    bool                        WndInit( const LPCTSTR pszName, const LONG left, const LONG top, const LONG width, const LONG height, const DWORD dwFlags = WF_DEFAULT, const DWORD dwStyle = WS_DEFAULT, const DWORD dwExStyle = WS_EX_DEFAULT, const HMENU hMenu = NULL, CWnd* const pWndParent = NULL );
    bool                        WndInit( const LPCTSTR pszName, const CWndConfig& Config, CWnd* const pWndParent = NULL );
    inline bool                 WndInit( const LPCTSTR pszName, CWnd* const pWndParent = NULL ) { return( WndInit( pszName, CWndConfig::GetDefaults(), pWndParent ) ); };
    void                        WndKill( void );
    void                        WndSetActiveUpdates( const bool set = true );
    inline void                 WndSetPassiveUpdates( void ) { WndSetActiveUpdates( false ); };
    void                        WndSetExclusiveUpdates( const bool set = true );
    inline void                 WndEndExclusiveUpdates( void ) { WndSetExclusiveUpdates( false ); };
    inline void                 WndStopLoopUpdates( void ) { WndSetPassiveUpdates(); WndEndExclusiveUpdates(); };
    inline bool                 WndHasActiveUpdates( void ) const { return( ( m_pWndActiveNext != NULL ) ? true : false ); };
    inline bool                 WndHasPassiveUpdates( void ) const { return( !WndHasActiveUpdates() ); };
    inline bool                 WndHasExclusiveUpdates( void ) const { return( ( ms_pWndExclusive == this ) ? true : false ); };
    void                        WndRunApp( void );
    static void                 WndLoop( void );
    static bool                 WndPump( void );
    static inline WPARAM        WndCode( void ) { return( ms_wMsg.wParam ); };
    bool                        WndShow( bool bShow = true ) const;
    inline HWND                 WndHandle( void ) const { return( m_hWnd ); };
    HWND                        WndParentHandle( void ) const;
    inline CWnd*                WndParentObject( void ) const { return( m_pWndParent ); };
    bool                        WndCenterWindow( void ) const;
    bool                        WndCenterClient( void ) const;
    bool                        WndAdjustWindow( const LONG left, const LONG top, const LONG width = 0, const LONG height = 0 ) const;
    bool                        WndAdjustClient( const LONG left, const LONG top, const LONG width = 0, const LONG height = 0 ) const;
    bool                        WndResizeWindow( const LONG width, const LONG height, const CENTERING centering = UNCENTERED ) const;
    bool                        WndResizeClient( const LONG width, const LONG height, const CENTERING centering = UNCENTERED ) const;
    bool                        WndGetParentScreenRect( RECT& rectParent ) const;
    bool                        WndGetWindowScreenRect( RECT& rectWindow ) const;
    bool                        WndGetClientScreenRect( RECT& rectClient ) const;
protected:
    virtual bool                WndUpdate( void );
    virtual bool                WndDraw( void );
    virtual LRESULT             WndUser( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam );
    HWND                        m_hWnd;
    CWnd*                       m_pWndParent;
private:
    inline                      CWnd( const CWnd& ) {};
    inline void                 operator=( const CWnd& ) {};
    inline void                 WndSetAppObject( void ) { ms_pWndAppObject = this; };
    inline void                 WndClearAppObject( void ) { ms_pWndAppObject = NULL; };
    static LRESULT CALLBACK     WndProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam );
    static MSG                  ms_wMsg;
    static LPCSTR               ms_wAtom;
    static UINT                 ms_uiRefs;
    static UINT                 ms_uiOpen;
    static CWnd*                ms_pWndAppObject;
    static CWnd*                ms_pWndExclusive;
    static CWnd*                ms_pWndActiveList;
    CWnd*                       m_pWndActivePrev;
    CWnd*                       m_pWndActiveNext;
protected:
////    exclusive updates helper functions:
    static inline CWnd*         WndGetExclusiveUpdatesWindow( void ) { return( ms_pWndExclusive ); };
    static inline void          WndSetExclusiveUpdatesWindow( CWnd* wnd ) { ms_pWndExclusive = wnd; };

////    screen space window rectangle helper functions:
    static void                 WndGetParentScreenRect( const HWND hWnd, RECT& rectParent );
    static void                 WndGetWindowScreenRect( const HWND hWnd, RECT& rectWindow );
    static void                 WndGetClientScreenRect( const HWND hWnd, RECT& rectClient );

////    move or resize conditional on changed rectangle helper function:
    static void                 WndSetWindowScreenRect( const HWND hWnd, const RECT& rectWindow, const RECT& rectTarget );

////    relative space rectangle remapping helper functions:
    static void                 WndClientToWindowSpace( const RECT& rectWindow, const RECT& rectClient, RECT& rectTarget );
    static void                 WndClientToScreenSpace( const HWND hWnd, RECT& rectClient );

////    rectangle modification helper functions:
    static void                 WndCenterRect( const RECT& rectParent, const RECT& rectCenter, RECT& rectCentered );
    static void                 WndCenterRect( const RECT& rectParent, const LONG width, const LONG height, RECT& rectCentered );
    static void                 WndIndentRect( const RECT& rectIndent, const LONG indent, RECT& rectIndented );
    static void                 WndIndentRect( const RECT& rectIndent, const LONG indentTopLeft, const LONG indentBottomRight, RECT& rectIndented );
    static void                 WndClampRect( RECT& rectClamp );

////    rectangle drawing helper functions:
    static void                 WndDrawRectEtch( const HDC hDC, const RECT& rectEtch, const COLORREF uLight, const COLORREF uShade, const LONG indent = 0, const LONG width = 1 );
    static void                 WndDrawRectDrop( const HDC hDC, const RECT& rectDrop, const COLORREF uLight, const COLORREF uShade, const LONG indent = 0, const LONG width = 1 );
    static void                 WndDrawRectLift( const HDC hDC, const RECT& rectLift, const COLORREF uLight, const COLORREF uShade, const LONG indent = 0, const LONG width = 1 );
    static void                 WndDrawRectEdge( const HDC hDC, const RECT& rectEdge, const COLORREF uColour, const LONG indent = 0, const LONG width = 1 );
    static void                 WndDrawRectFill( const HDC hDC, const RECT& rectFill, const COLORREF uColour, const LONG indent = 0 );

////    windows font creation helper function:
    static HFONT                WndCreateFont( const char* pszName = "System", const int iPointSize = 11, const int iWeight = 0, const BOOL bItalic = FALSE, const BOOL bUnderline = FALSE, const BOOL bStrikeout = FALSE );

////    message debugging helper function:
#ifdef  _DEBUG
protected:
    static void                 WndTrace( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam );
private:
    struct                      WM_NAME { UINT uiMsg; const char* pszName; };
    static const WM_NAME        mk_WM_NAMES[];
#endif  //  #ifdef  _DEBUG
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end platform_windows namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};  //  namespace platform_windows

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //  #ifndef __CWND_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
