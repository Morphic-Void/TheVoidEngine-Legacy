
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   buffersx.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        DirectX 11 buffer creation and wrapper object classes.
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
#include "buffersx.h"

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
////    CSimpleTexture class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CSimpleTexture::create( const UINT Width, const UINT Height, const void* const data )
{
    if( CCoreX::acquire() )
    {
        HRESULT hr;
        destroy();
        m_width = Width;
        m_height = Height;
        D3D11_TEXTURE2D_DESC textureDesc;
        memset( &textureDesc, 0, sizeof( textureDesc ) );
        textureDesc.Width = Width;
        textureDesc.Height = Height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA d3d11Data;
        memset( &d3d11Data, 0, sizeof( d3d11Data ) );
        d3d11Data.pSysMem = data;
        d3d11Data.SysMemPitch = ( Width << 2 );
        hr = CCoreX::getDevice()->CreateTexture2D( &textureDesc, &d3d11Data, &m_texture );
        if( hr == S_OK )
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC d3d11SrvDesc;
            memset( &d3d11SrvDesc, 0, sizeof( d3d11SrvDesc ) );
            d3d11SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            d3d11SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            hr = CCoreX::getDevice()->CreateShaderResourceView( m_texture, &d3d11SrvDesc, &m_view );
            if( hr == S_OK )
            {
                return( true );
            }
            else
            {
                LOG( "ID3D11Device::CreateShaderResourceView() failed!!!" );
            }
            m_view = NULL;
            m_texture->Release();
        }
        else
        {
            LOG( "ID3D11Device::CreateTexture2D() failed!!!" );
        }
        m_texture = NULL;
        CCoreX::release();
        m_width = 0;
        m_height = 0;
    }
    return( false );
}

void CSimpleTexture::destroy()
{
    if( m_view != NULL )
    {
        m_view->Release();
        m_view = NULL;
    }
    if( m_texture != NULL )
    {
        m_texture->Release();
        m_texture = NULL;
        CCoreX::release();
    }
    m_width = 0;
    m_height = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuffer class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBuffer::destroy()
{
    if( m_buffer != NULL )
    {
        m_buffer->Release();
        m_buffer = NULL;
        CCoreX::release();
    }
    m_bytes = 0;
    m_stride = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBufferView class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBufferView::destroy()
{
    if( m_view != NULL )
    {
        m_view->Release();
        m_view = NULL;
    }
    CBuffer::destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CIndices immutable index buffer class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CIndices::create( const UINT count, const UINT stride, const void* const data )
{
    if( ( stride == 2 ) || ( stride == 4 ) )
    {
        if( CCoreX::acquire() )
        {
            destroy();
            m_bytes = ( count * stride );
            m_stride = stride;
            D3D11_BUFFER_DESC d3d11Desc;
            D3D11_SUBRESOURCE_DATA d3d11Data;
            memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
            memset( &d3d11Data, 0, sizeof( d3d11Data ) );
            d3d11Data.pSysMem = data;
            d3d11Desc.ByteWidth = m_bytes;
            d3d11Desc.Usage = D3D11_USAGE_IMMUTABLE;
            d3d11Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            HRESULT hr = CCoreX::getDevice()->CreateBuffer( &d3d11Desc, &d3d11Data, &m_buffer );
            if( hr == S_OK )
            {   //  buffer created successfully
                return( true );
            }
            else
            {
                LOG( "ID3D11Device::CreateBuffer() failed!!!" );
            }
            m_buffer = NULL;
            CCoreX::release();
            m_bytes = 0;
            m_stride = 0;
        }
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CVertices immutable vertex buffer class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CVertices::create( const UINT count, const UINT stride, const void* const data )
{
    if( CCoreX::acquire() )
    {
        destroy();
        m_bytes = ( count * stride );
        m_stride = stride;
        D3D11_BUFFER_DESC d3d11Desc;
        D3D11_SUBRESOURCE_DATA d3d11Data;
        memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
        memset( &d3d11Data, 0, sizeof( d3d11Data ) );
        d3d11Data.pSysMem = data;
        d3d11Desc.ByteWidth = m_bytes;
        d3d11Desc.Usage = D3D11_USAGE_IMMUTABLE;
        d3d11Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        HRESULT hr = CCoreX::getDevice()->CreateBuffer( &d3d11Desc, &d3d11Data, &m_buffer );
        if( hr == S_OK )
        {   //  buffer created successfully
            return( true );
        }
        else
        {
            LOG( "ID3D11Device::CreateBuffer() failed!!!" );
        }
        m_buffer = NULL;
        CCoreX::release();
        m_bytes = 0;
        m_stride = 0;
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CConstants immutable constant buffer class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CConstants::create( const UINT bytes, const void* const data )
{
    if( CCoreX::acquire() )
    {
        destroy();
        m_bytes = bytes;
        D3D11_BUFFER_DESC d3d11Desc;
        D3D11_SUBRESOURCE_DATA d3d11Data;
        memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
        memset( &d3d11Data, 0, sizeof( d3d11Data ) );
        d3d11Data.pSysMem = data;
        d3d11Desc.ByteWidth = m_bytes;
        d3d11Desc.Usage = D3D11_USAGE_IMMUTABLE;
        d3d11Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        HRESULT hr = CCoreX::getDevice()->CreateBuffer( &d3d11Desc, &d3d11Data, &m_buffer );
        if( hr == S_OK )
        {   //  buffer created successfully
            return( true );
        }
        else
        {
            LOG( "ID3D11Device::CreateBuffer() failed!!!" );
        }
        m_buffer = NULL;
        CCoreX::release();
        m_bytes = 0;
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CStructured immutable structured buffer class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CStructured::create( const UINT count, const UINT stride, const void* const data )
{
    if( CCoreX::acquire() )
    {
        destroy();
        m_bytes = ( count * stride );
        m_stride = stride;
        D3D11_BUFFER_DESC d3d11Desc;
        D3D11_SUBRESOURCE_DATA d3d11Data;
        memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
        memset( &d3d11Data, 0, sizeof( d3d11Data ) );
        d3d11Data.pSysMem = data;
        d3d11Desc.ByteWidth = m_bytes;
        d3d11Desc.Usage = D3D11_USAGE_IMMUTABLE;
        d3d11Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3d11Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        d3d11Desc.StructureByteStride = stride;
        HRESULT hr = CCoreX::getDevice()->CreateBuffer( &d3d11Desc, &d3d11Data, &m_buffer );
        if( hr == S_OK )
        {   //  buffer created successfully, now create a default view
            D3D11_SHADER_RESOURCE_VIEW_DESC d3d11SrvDesc;
            memset( &d3d11SrvDesc, 0, sizeof( d3d11SrvDesc ) );
            d3d11SrvDesc.Format = DXGI_FORMAT_UNKNOWN;
            d3d11SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            hr = CCoreX::getDevice()->CreateShaderResourceView( m_buffer, &d3d11SrvDesc, &m_view );
            if( hr == S_OK )
            {
                return( true );
            }
            else
            {
                LOG( "ID3D11Device::CreateShaderResourceView() failed!!!" );
            }
            m_view = NULL;
            m_buffer->Release();
        }
        else
        {
            LOG( "ID3D11Device::CreateBuffer() failed!!!" );
        }
        m_buffer = NULL;
        CCoreX::release();
        m_bytes = 0;
        m_stride = 0;
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CByteAddress immutable byte address buffer class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CByteAddress::create( const UINT count, const UINT stride, const void* const data )
{
    if( ( ( stride - 4 ) & ~0x0000000c ) == 0 )
    {
        if( CCoreX::acquire() )
        {
            destroy();
            m_bytes = ( count * stride );
            m_stride = stride;
            D3D11_BUFFER_DESC d3d11Desc;
            D3D11_SUBRESOURCE_DATA d3d11Data;
            memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
            memset( &d3d11Data, 0, sizeof( d3d11Data ) );
            d3d11Data.pSysMem = data;
            d3d11Desc.ByteWidth = m_bytes;
            d3d11Desc.Usage = D3D11_USAGE_IMMUTABLE;
            d3d11Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            d3d11Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
            HRESULT hr = CCoreX::getDevice()->CreateBuffer( &d3d11Desc, &d3d11Data, &m_buffer );
            if( hr == S_OK )
            {   //  buffer created successfully, now create a default view
                static const DXGI_FORMAT formats[ 4 ] = { DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32G32_TYPELESS, DXGI_FORMAT_R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32A32_TYPELESS };
                D3D11_SHADER_RESOURCE_VIEW_DESC d3d11SrvDesc;
                memset( &d3d11SrvDesc, 0, sizeof( d3d11SrvDesc ) );
                d3d11SrvDesc.Format = formats[ ( stride >> 2 ) - 1 ];
                d3d11SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
                d3d11SrvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
                hr = CCoreX::getDevice()->CreateShaderResourceView( m_buffer, &d3d11SrvDesc, &m_view );
                if( hr == S_OK )
                {
                    return( true );
                }
                else
                {
                    LOG( "ID3D11Device::CreateShaderResourceView() failed!!!" );
                }
                m_view = NULL;
                m_buffer->Release();
            }
            else
            {
                LOG( "ID3D11Device::CreateBuffer() failed!!!" );
            }
            m_buffer = NULL;
            CCoreX::release();
            m_bytes = 0;
            m_stride = 0;
        }
    }
    return( false );
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
