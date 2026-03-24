
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   ccorex.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        Core DirectX initialisation object.
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
#include "libs/system/debug/asserts.h"
#include "corex.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    XInputEnable function is missing on Windows 7
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if(_WIN32_WINNT < _WIN32_WINNT_WIN8)
#define XInputEnable(param)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CCoreX class static member initialisation
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT                    CCoreX::ms_refs( 0 );
D3D_FEATURE_LEVEL       CCoreX::ms_featureLevel( static_cast< D3D_FEATURE_LEVEL >( 0 ) );
IDXGIFactory1*          CCoreX::ms_giFactory( NULL );
IDXGIAdapter*           CCoreX::ms_giAdapter( NULL );
IDXGIDevice*            CCoreX::ms_giDevice( NULL );
IDXGIOutput*            CCoreX::ms_giOutput( NULL );
IDXGIInfoQueue*         CCoreX::ms_giInfoQueue( NULL );
ID3D11Device*           CCoreX::ms_device( NULL );
ID3D11DeviceContext*    CCoreX::ms_context( NULL );
CCoreX::CONTROLLER      CCoreX::ms_controllers[ 4 ] = { 0 };

#if ( USES_DX_DEBUG_OBJECTS == TRUE ) || ( DEBUG_DX_REF_COUNTING == TRUE )
IDXGIDebug*             CCoreX::ms_giDebug( NULL );
ID3D11Debug*            CCoreX::ms_debug( NULL );
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CCoreX class public function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CCoreX::update()
{
    memset( reinterpret_cast< void* const >( &ms_controllers ), 0, sizeof( ms_controllers ) );
    if( ms_refs )
    {
        for( DWORD controllerIndex = 0; controllerIndex < 4; ++controllerIndex )
        {
            ms_controllers[ controllerIndex ].Connected = ( ( XInputGetState( controllerIndex, &ms_controllers[ controllerIndex ].State ) == ERROR_SUCCESS ) ? true : false );
        }
    }
}

bool CCoreX::acquire()
{
    if( ms_refs == 0 )
    {
        if( !init() )
        {
            kill();
            return( false );
        }
    }
    ++ms_refs;
    return( true );
}

void CCoreX::release()
{
    if( ms_refs == 1 )
    {
        kill();
    }
    if( ms_refs != 0 )
    {
        --ms_refs;
    }
}

bool CCoreX::init()
{
    static const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };
    HRESULT hr = D3D11CreateDevice(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, ( D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG ),
        featureLevels, 7, D3D11_SDK_VERSION, &ms_device, &ms_featureLevel, &ms_context );
    if( hr != S_OK )
    {
        LOG( "D3D11CreateDevice() for DEBUG failed. Falling back to the RETAIL device." );
        hr = D3D11CreateDevice(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_SINGLETHREADED,
            featureLevels, 7, D3D11_SDK_VERSION, &ms_device, &ms_featureLevel, &ms_context );
        if( hr != S_OK )
        {
            LOG( "D3D11CreateDevice() for RETAIL failed!!!" );
            return( false );
        }
        LOG( "D3D11CreateDevice() for RETAIL succeeded" );
    }
#if ( USES_DX_DEBUG_OBJECTS == TRUE ) || ( DEBUG_DX_REF_COUNTING == TRUE )
    else
    {
        DXGIGetDebugInterfaceType DXGIGetDebugInterface = getDXGIGetDebugInterface();
        if( DXGIGetDebugInterface != NULL )
        {
            hr = DXGIGetDebugInterface( __uuidof( ms_giDebug ), (void**)&ms_giDebug );
            if( hr != S_OK )
            {
                LOG( "DXGIGetDebugInterface(IDXGIDebug) failed!!!" );
                ms_giDebug = NULL;
            }
            hr = DXGIGetDebugInterface( __uuidof( ms_giInfoQueue ), (void**)&ms_giInfoQueue );
            if( hr != S_OK )
            {
                LOG( "DXGIGetDebugInterface(IDXGIInfoQueue) failed!!!" );
                ms_giInfoQueue = NULL;
            }
        }
        hr = ms_device->QueryInterface( __uuidof( ms_debug ), (void**)&ms_debug );
        if( hr != S_OK )
        {
            LOG( "ID3D11Device::QueryInterface(ID3D11Debug) failed!!!" );
            ms_debug = NULL;
        }
    }
#endif
    hr = ms_device->QueryInterface( __uuidof( ms_giDevice ), (void**)&ms_giDevice );
    if( hr != S_OK )
    {
        LOG( "ID3D11Device::QueryInterface(IDXGIDevice) failed!!!" );
        return( false );
    }
    hr = ms_giDevice->GetParent( __uuidof( ms_giAdapter ), (void**)&ms_giAdapter );
    if( hr != S_OK )
    {
        LOG( "IDXGIDevice::GetParent(IDXGIAdapter) failed!!!" );
        return( false );
    }
    hr = ms_giAdapter->GetParent( __uuidof( ms_giFactory ), (void**)&ms_giFactory );
    if( hr != S_OK )
    {
        LOG( "IDXGIAdapter::GetParent(IDXGIFactory1) failed!!!" );
        return( false );
    }
    hr = ms_giAdapter->EnumOutputs( 0, &ms_giOutput );
    if( hr != S_OK )
    {
        LOG( "IDXGIAdapter::EnumOutputs() failed!!!" );
        return( false );
    }
    memset( reinterpret_cast< void* const >( &ms_controllers ), 0, sizeof( ms_controllers ) );
    XInputEnable( TRUE );   //  enable XInput
    return( true );
}

void CCoreX::kill()
{
    XInputEnable( FALSE );  //  disable XInput and any rumble effects
    memset( reinterpret_cast< void* const >( &ms_controllers ), 0, sizeof( ms_controllers ) );
    if( ms_giOutput != NULL )
    {
        ms_giOutput->Release();
        ms_giOutput = NULL;
    }
    if( ms_giFactory != NULL )
    {
        ms_giFactory->Release();
        ms_giFactory = NULL;
    }
    if( ms_giAdapter != NULL )
    {
        ms_giAdapter->Release();
        ms_giAdapter = NULL;
    }
    if( ms_giDevice != NULL )
    {
        ms_giDevice->Release();
        ms_giDevice = NULL;
    }
    if( ms_context != NULL )
    {
#if ( DEBUG_DX_REF_COUNTING == TRUE )
        ms_context->ClearState();
        ms_context->Flush();
#endif
        ms_context->Release();
        ms_context = NULL;
    }
    if( ms_device != NULL )
    {
        ms_device->Release();
        ms_device = NULL;
    }
#if ( USES_DX_DEBUG_OBJECTS == TRUE ) || ( DEBUG_DX_REF_COUNTING == TRUE )
    if( ms_giDebug != NULL )
    {
#if ( DEBUG_DX_REF_COUNTING == TRUE )
        OutputDebugString( "ReportLiveObjects()\n" );
        ms_giDebug->ReportLiveObjects( DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL );
#endif
        ms_giDebug->Release();
        ms_giDebug = NULL;
    }
    if( ms_debug != NULL )
    {
#if ( DEBUG_DX_REF_COUNTING == TRUE )
        OutputDebugString( "ReportLiveDeviceObjects()\n" );
        ms_debug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL | D3D11_RLDO_SUMMARY );
#endif
        ms_debug->Release();
        ms_debug = NULL;
    }
#endif
    ms_featureLevel = static_cast< D3D_FEATURE_LEVEL >( 0 );
}

const XINPUT_GAMEPAD* CCoreX::getFirstConnectedController()
{
    const XINPUT_GAMEPAD* controller = NULL;
    for( UINT index = 0; index < 4; ++index )
    {
        const CCoreX::CONTROLLER& check = CCoreX::getController( index );
        if( check.Connected )
        {
            controller = &check.State.Gamepad;
            break;
        }
    }
    return( controller );
}

#if ( USES_DX_DEBUG_OBJECTS == TRUE ) || ( DEBUG_DX_REF_COUNTING == TRUE )
CCoreX::DXGIGetDebugInterfaceType CCoreX::getDXGIGetDebugInterface()
{
    DXGIGetDebugInterfaceType DXGIGetDebugInterface = reinterpret_cast< DXGIGetDebugInterfaceType >( GetProcAddress( GetModuleHandle( "Dxgidebug.dll" ), "DXGIGetDebugInterface" ) );
    if( DXGIGetDebugInterface == NULL )
    {
        LOG( "DXGIGetDebugInterface() load failed!!!" );
    }
    return( DXGIGetDebugInterface );
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
