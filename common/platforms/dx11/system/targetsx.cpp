
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   targetsx.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        DirectX 11 render target and depth-stencil creation and wrapper object classes.
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

#include "libs/system/debug/asserts.h"
#include "targetsx.h"

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
////    CSwapChain simple swap chain class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CSwapChain::create( HWND hWnd, const bool SRGB, const bool isShaderResource )
{
    if( CCoreX::acquire() )
    {
        destroy();
        m_valid = true;
        m_sourceViewDesc.Format = m_targetViewDesc.Format = ( SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM );
        m_sourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        m_sourceViewDesc.Texture2D.MostDetailedMip = 0;
        m_sourceViewDesc.Texture2D.MipLevels = 1;
        m_targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        m_targetViewDesc.Texture2D.MipSlice = 0;
        if( !setSwapChainDesc( hWnd, isShaderResource ) || !setWindowedModeDesc( hWnd ) || !setFullscreenModeDesc()  )
        {   //  failed to initialise the swap chain data descriptions
            destroy();
            return( false );
        }
        m_giWindowed.RefreshRate.Denominator = m_giFullscreen.RefreshRate.Denominator;
        m_giWindowed.RefreshRate.Numerator = m_giFullscreen.RefreshRate.Numerator;
        if( !createSwapChain() )
        {   //  failed to create the swap chain or the render target view
            destroy();
            return( false );
        }
        m_available = true;
        return( true );
    }
    return( false );
}

bool CSwapChain::resize( const UINT Width, const UINT Height )
{
    if( isValid() )
    {
        if( m_available )
        {
            if( isWindowed() )
            {
                m_available = false;
                m_giSwapDesc.BufferDesc.Width = m_giWindowed.Width = Width;
                m_giSwapDesc.BufferDesc.Height = m_giWindowed.Height = Height;
                if( !resizeBuffers() )
                {
                    return( false );
                }
                m_available = true;
            }
        }
        return( true );
    }
    return( false );
}

bool CSwapChain::present( const UINT SyncInterval, const UINT Flags )
{
    if( isValid() )
    {
        if( m_available )
        {
            HRESULT hr = m_giSwapChain->Present( SyncInterval, Flags );
            if( hr != S_OK )
            {
                LOG( "IDXGISwapChain::Present() failed!!!" );
                return( false );
            }
        }
        return( true );
    }
    return( false );
}

void CSwapChain::destroy()
{
    m_available = false;
    if( m_targetView != NULL )
    {
        m_targetView->Release();
        m_targetView = NULL;
    }
    if( m_sourceView != NULL )
    {
        m_sourceView->Release();
        m_sourceView = NULL;
    }
    if( m_giSwapChain != NULL )
    {
        m_giSwapChain->SetFullscreenState( FALSE, NULL );
        m_giSwapChain->Release();
        m_giSwapChain = NULL;
    }
    if( m_valid )
    {
        CCoreX::release();
        m_valid = false;
    }
    zero();
}

void CSwapChain::zero()
{
    m_valid = false;
    m_available = false ;
    memset( reinterpret_cast< void* >( &m_giWindowed ), 0, sizeof( m_giWindowed ) );
    memset( reinterpret_cast< void* >( &m_giFullscreen ), 0, sizeof( m_giFullscreen ) );
    memset( reinterpret_cast< void* >( &m_giSwapDesc ), 0, sizeof( m_giSwapDesc ) );
    memset( reinterpret_cast< void* >( &m_sourceViewDesc ), 0, sizeof( m_sourceViewDesc ) );
    memset( reinterpret_cast< void* >( &m_targetViewDesc ), 0, sizeof( m_targetViewDesc ) );
    m_giSwapChain = NULL;
    m_sourceView = NULL;
    m_targetView = NULL;
}

bool CSwapChain::setWindowed()
{
    if( isValid() )
    {
        if( isFullscreen() )
        {
            m_available = false;
            m_giSwapDesc.Windowed = TRUE;
            m_giSwapDesc.BufferDesc = m_giWindowed;
            m_giSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
            HRESULT hr = m_giSwapChain->SetFullscreenState( FALSE, NULL );
            if( hr != S_OK )
            {
                LOG( "IDXGISwapChain::SetFullscreenState() failed!!!" );
                return( false );
            }
            if( !resizeTarget() )
            {
                return( false );
            }
            m_available = true;
        }
        return( true );
    }
    return( false );
}

bool CSwapChain::setFullscreen()
{
    if( isValid() )
    {
        if( isWindowed() )
        {
            m_available = false;
            m_giSwapDesc.Windowed = FALSE;
            m_giSwapDesc.BufferDesc = m_giFullscreen;
            m_giSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            HRESULT hr = m_giSwapChain->SetFullscreenState( TRUE, CCoreX::getGIOutput() );
            if( hr != S_OK )
            {
                LOG( "IDXGISwapChain::SetFullscreenState() failed!!!" );
                return( false );
            }
            if( !resizeTarget() )
            {
                return( false );
            }
            SetFocus( m_giSwapDesc.OutputWindow );
            m_available = true;
        }
        return( true );
    }
    return( false );
}

bool CSwapChain::resizeTarget()
{
    HRESULT hr = m_giSwapChain->ResizeTarget( &m_giSwapDesc.BufferDesc );
    if( hr != S_OK )
    {
        LOG( "IDXGISwapChain::ResizeTarget() failed!!!" );
        return( false );
    }
    return( resizeBuffers() );
}

bool CSwapChain::resizeBuffers()
{
    if( m_targetView != NULL )
    {
        m_targetView->Release();
        m_targetView = NULL;
    }
    if( m_sourceView != NULL )
    {
        m_sourceView->Release();
        m_sourceView = NULL;
    }
    HRESULT hr = m_giSwapChain->ResizeBuffers( m_giSwapDesc.BufferCount, m_giSwapDesc.BufferDesc.Width, m_giSwapDesc.BufferDesc.Height, m_giSwapDesc.BufferDesc.Format, m_giSwapDesc.Flags );
    if( hr != S_OK )
    {
        LOG( "IDXGISwapChain::ResizeBuffers() failed!!!" );
        return( false );
    }
    return( createViews() );
}

bool CSwapChain::createSwapChain()
{
    HRESULT hr;
    if( m_giSwapChain )
    {
        m_giSwapChain->Release();
        m_giSwapChain = NULL;
    }
    hr = CCoreX::getGIFactory()->CreateSwapChain( CCoreX::getDevice(), &m_giSwapDesc, &m_giSwapChain );
    if( hr != S_OK )
    {
        LOG( "IDXGIFactory1::CreateSwapChain() failed!!!" );
        return( false );
    }
    hr = CCoreX::getGIFactory()->MakeWindowAssociation( m_giSwapDesc.OutputWindow, DXGI_MWA_NO_ALT_ENTER );
    if( hr != S_OK )
    {
        LOG( "IDXGIFactory1::MakeWindowAssociation() failed!!!" );
        m_giSwapChain->Release();
        m_giSwapChain = NULL;
        return( false );
    }
    return( createViews() );
}

bool CSwapChain::createViews()
{
    HRESULT hr;
    ID3D11Resource* buffer = NULL;
    hr = m_giSwapChain->GetBuffer( 0, __uuidof( buffer ), reinterpret_cast< void** >( &buffer ) );
    if( hr != S_OK )
    {
        LOG( "IDXGISwapChain::GetBuffer() failed!!!" );
        return( false );
    }
    if( m_giSwapDesc.BufferUsage & DXGI_USAGE_SHADER_INPUT )
    {
        hr = CCoreX::getDevice()->CreateShaderResourceView( buffer, &m_sourceViewDesc, &m_sourceView );
        if( hr != S_OK )
        {
            LOG( "ID3D11Device::CreateShaderResourceView() failed!!!" );
            buffer->Release();
            return( false );
        }
    }
    hr = CCoreX::getDevice()->CreateRenderTargetView( buffer, &m_targetViewDesc, &m_targetView );
    if( hr != S_OK )
    {
        LOG( "ID3D11Device::CreateRenderTargetView() failed!!!" );
        buffer->Release();
        return( false );
    }
    buffer->Release();
    return( true );
}

bool CSwapChain::setSwapChainDesc( HWND hWnd, const bool isShaderResource )
{
    memset( reinterpret_cast< void* >( &m_giSwapDesc ), 0, sizeof( m_giSwapDesc ) );
    if( hWnd )
    {
        RECT rect;
        GetClientRect( hWnd, &rect );
        m_giSwapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        m_giSwapDesc.BufferCount = 3;
        m_giSwapDesc.BufferUsage = ( ( isShaderResource ? DXGI_USAGE_SHADER_INPUT : 0 ) | DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER );
        m_giSwapDesc.BufferDesc.Width = ( rect.right - rect.left );
        m_giSwapDesc.BufferDesc.Height = ( rect.bottom - rect.top );
        m_giSwapDesc.BufferDesc.RefreshRate.Denominator = 0;
        m_giSwapDesc.BufferDesc.RefreshRate.Numerator = 0;
        m_giSwapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        m_giSwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        m_giSwapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        m_giSwapDesc.SampleDesc.Count = 1;
        m_giSwapDesc.SampleDesc.Quality = 0;
        m_giSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // DXGI_SWAP_EFFECT_DISCARD  DXGI_SWAP_EFFECT_SEQUENTIAL  DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        m_giSwapDesc.OutputWindow = hWnd;
        m_giSwapDesc.Windowed = TRUE;
        return( true );
    }
    return( false );
}

bool CSwapChain::setWindowedModeDesc( HWND hWnd )
{
    memset( reinterpret_cast< void* >( &m_giWindowed ), 0, sizeof( m_giWindowed ) );
    if( hWnd )
    {
        RECT rect;
        GetClientRect( hWnd, &rect );
        m_giWindowed.Width = ( rect.right - rect.left );
        m_giWindowed.Height = ( rect.bottom - rect.top );
        m_giWindowed.RefreshRate.Denominator = 0;
        m_giWindowed.RefreshRate.Numerator = 0;
        m_giWindowed.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        m_giWindowed.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        m_giWindowed.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        return( true );
    }
    return( false );
}

bool CSwapChain::setFullscreenModeDesc()
{
    memset( reinterpret_cast< void* >( &m_giFullscreen ), 0, sizeof( m_giFullscreen ) );
    HRESULT hr;
    DXGI_OUTPUT_DESC outputDesc;
    hr = CCoreX::getGIOutput()->GetDesc( &outputDesc );
    if( hr != S_OK )
    {
        LOG( "IDXGIOutput::GetDesc() failed!!!" );
        return( false );
    }
    MONITORINFOEX monitorInfo;
    memset( reinterpret_cast< void* const >( &monitorInfo ), 0, sizeof( monitorInfo ) );
    monitorInfo.cbSize = sizeof( monitorInfo );
    if( GetMonitorInfo( outputDesc.Monitor, &monitorInfo ) == 0 )
    {
        LOG( "GetMonitorInfo() failed!!!" );
        return( false );
    }
    DXGI_MODE_DESC giFindDesc;
    memset( reinterpret_cast< void* >( &giFindDesc ), 0, sizeof( giFindDesc ) );
    giFindDesc.Width = ( monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left );
    giFindDesc.Height = ( monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top );;
    giFindDesc.RefreshRate.Numerator = 60;
    giFindDesc.RefreshRate.Denominator = 1;
    giFindDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    giFindDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    giFindDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    hr = CCoreX::getGIOutput()->FindClosestMatchingMode( &giFindDesc, &m_giFullscreen, CCoreX::getDevice() );
    if( hr != S_OK )
    {
        LOG( "IDXGIOutput::FindClosestMatchingMode() failed!!!" );
        memset( reinterpret_cast< void* >( &m_giFullscreen ), 0, sizeof( m_giFullscreen ) );
        return( false );
    }
    return( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CRenderTarget simple render target class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CRenderTarget::create( const UINT Width, const UINT Height, const DXGI_FORMAT Format )
{
    if( CCoreX::acquire() )
    {
        destroy();
        m_valid = true;
        m_textureDesc.Width = Width;
        m_textureDesc.Height = Height;
        m_textureDesc.MipLevels = 1;
        m_textureDesc.ArraySize = 1;
        m_textureDesc.Format = Format;
        m_textureDesc.SampleDesc.Count = 1;
        m_textureDesc.SampleDesc.Quality = 0;
        m_textureDesc.Usage = D3D11_USAGE_DEFAULT;
        m_textureDesc.BindFlags = ( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
        m_textureDesc.CPUAccessFlags = 0;
        m_textureDesc.MiscFlags = 0;
        m_sourceViewDesc.Format = Format;
        m_sourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        m_sourceViewDesc.Texture2D.MostDetailedMip = 0;
        m_sourceViewDesc.Texture2D.MipLevels = 1;
        m_targetViewDesc.Format = Format;
        m_targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        m_targetViewDesc.Texture2D.MipSlice = 0;
        if( resize() )
        {
            return( true );
        }
        destroy();
    }
    return( false );
}

bool CRenderTarget::resize( const UINT Width, const UINT Height )
{
    if( isValid() )
    {
        m_textureDesc.Width = Width;
        m_textureDesc.Height = Height;
        if( resize() )
        {
            return( true );
        }
        destroy();
    }
    return( false );
}

void CRenderTarget::destroy()
{
    if( m_targetView != NULL )
    {
        m_targetView->Release();
        m_targetView = NULL;
    }
    if( m_sourceView != NULL )
    {
        m_sourceView->Release();
        m_sourceView = NULL;
    }
    if( m_texture != NULL )
    {
        m_texture->Release();
        m_texture = NULL;
    }
    if( m_valid )
    {
        CCoreX::release();
        m_valid = false;
    }
    zero();
}

bool CRenderTarget::resize()
{
    HRESULT hr;
    if( m_targetView != NULL )
    {
        m_targetView->Release();
        m_targetView = NULL;
    }
    if( m_sourceView != NULL )
    {
        m_sourceView->Release();
        m_sourceView = NULL;
    }
    if( m_texture != NULL )
    {
        m_texture->Release();
        m_texture = NULL;
    }
    hr = CCoreX::getDevice()->CreateTexture2D( &m_textureDesc, NULL, &m_texture );
    if( hr != S_OK )
    {
        LOG( "ID3D11Device::CreateTexture2D() failed!!!" );
        return( false );
    }
    hr = CCoreX::getDevice()->CreateShaderResourceView( m_texture, &m_sourceViewDesc, &m_sourceView );
    if( hr != S_OK )
    {
        LOG( "ID3D11Device::CreateShaderResourceView() failed!!!" );
        return( false );
    }
    hr = CCoreX::getDevice()->CreateRenderTargetView( m_texture, &m_targetViewDesc, &m_targetView );
    if( hr != S_OK )
    {
        LOG( "ID3D11Device::CreateRenderTargetView() failed!!!" );
        return( false );
    }
    return( true );
}

void CRenderTarget::zero()
{
    m_valid = false;
    memset( reinterpret_cast< void* >( &m_textureDesc ), 0, sizeof( m_textureDesc ) );
    memset( reinterpret_cast< void* >( &m_sourceViewDesc ), 0, sizeof( m_sourceViewDesc ) );
    memset( reinterpret_cast< void* >( &m_targetViewDesc ), 0, sizeof( m_targetViewDesc ) );
    m_texture = NULL;
    m_sourceView = NULL;
    m_targetView = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CDepthStencil simple depth stencil class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDepthStencil::create( const UINT Width, const UINT Height, const DXGI_FORMAT Format, const bool isShaderResource )
{
    if( CCoreX::acquire() )
    {
        destroy();
        m_valid = true;
        m_textureDesc.Width = Width;
        m_textureDesc.Height = Height;
        m_textureDesc.MipLevels = 1;
        m_textureDesc.ArraySize = 1;
        m_textureDesc.Format = Format;
        m_textureDesc.SampleDesc.Count = 1;
        m_textureDesc.SampleDesc.Quality = 0;
        m_textureDesc.Usage = D3D11_USAGE_DEFAULT;
        m_textureDesc.BindFlags = ( ( isShaderResource ? D3D11_BIND_SHADER_RESOURCE : 0 ) | D3D11_BIND_DEPTH_STENCIL );
        m_textureDesc.CPUAccessFlags = 0;
        m_textureDesc.MiscFlags = 0;
        m_sourceViewDesc.Format = Format;
        m_sourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        m_sourceViewDesc.Texture2D.MostDetailedMip = 0;
        m_sourceViewDesc.Texture2D.MipLevels = 1;
        m_targetViewDesc.Format = Format;
        m_targetViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        m_targetViewDesc.Flags = 0;
        m_targetViewDesc.Texture2D.MipSlice = 0;
        if( resize() )
        {
            return( true );
        }
        destroy();
    }
    return( false );
}

bool CDepthStencil::resize( const UINT Width, const UINT Height )
{
    if( isValid() )
    {
        m_textureDesc.Width = Width;
        m_textureDesc.Height = Height;
        if( resize() )
        {
            return( true );
        }
        destroy();
    }
    return( false );
}

void CDepthStencil::destroy()
{
    if( m_targetView != NULL )
    {
        m_targetView->Release();
        m_targetView = NULL;
    }
    if( m_sourceView != NULL )
    {
        m_sourceView->Release();
        m_sourceView = NULL;
    }
    if( m_texture != NULL )
    {
        m_texture->Release();
        m_texture = NULL;
    }
    if( m_valid )
    {
        CCoreX::release();
        m_valid = false;
    }
    zero();
}

bool CDepthStencil::resize()
{
    HRESULT hr;
    if( m_targetView != NULL )
    {
        m_targetView->Release();
        m_targetView = NULL;
    }
    if( m_sourceView != NULL )
    {
        m_sourceView->Release();
        m_sourceView = NULL;
    }
    if( m_texture != NULL )
    {
        m_texture->Release();
        m_texture = NULL;
    }
    hr = CCoreX::getDevice()->CreateTexture2D( &m_textureDesc, NULL, &m_texture );
    if( hr != S_OK )
    {
        LOG( "ID3D11Device::CreateTexture2D() failed!!!" );
        return( false );
    }
    if( m_textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE )
    {
        hr = CCoreX::getDevice()->CreateShaderResourceView( m_texture, &m_sourceViewDesc, &m_sourceView );
        if( hr != S_OK )
        {
            LOG( "ID3D11Device::CreateShaderResourceView() failed!!!" );
            return( false );
        }
    }
    hr = CCoreX::getDevice()->CreateDepthStencilView( m_texture, &m_targetViewDesc, &m_targetView );
    if( hr != S_OK )
    {
        LOG( "ID3D11Device::CreateDepthStencilView() failed!!!" );
        return( false );
    }
    return( true );
}

void CDepthStencil::zero()
{
    m_valid = false;
    memset( reinterpret_cast< void* >( &m_textureDesc ), 0, sizeof( m_textureDesc ) );
    memset( reinterpret_cast< void* >( &m_sourceViewDesc ), 0, sizeof( m_sourceViewDesc ) );
    memset( reinterpret_cast< void* >( &m_targetViewDesc ), 0, sizeof( m_targetViewDesc ) );
    m_texture = NULL;
    m_sourceView = NULL;
    m_targetView = NULL;
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
