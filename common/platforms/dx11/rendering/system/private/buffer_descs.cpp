
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   buffer_descs.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        Memory buffer description object classes.
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

#include "../public/buffer_descs.h"
#include <memory.h>

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
////    CBufferDesc memory buffer description class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void CBufferDesc::set( const CBufferDesc& desc )
//{
//    m_usage = desc.m_usage;
//    m_format = desc.m_format;
//    m_mips = desc.m_mips;
//    m_stride = desc.m_stride;
//    m_width = desc.m_width;
//    m_height = desc.m_height;
//    m_depth = desc.m_depth;
//    m_count = desc.m_count;
//}
//
//
//
//
//
//    //EMemoryConfigReadable           = 0x00000040,           //  access rights: cpu readable (must be created as a readable resource)
//    //EMemoryConfigWritable           = 0x00000080,           //  access rights: cpu writable (must be created as a writable resource)
//
//
//
//bool CBufferDesc::setBuffer( const UINT usage, const UINT stride, const UINT count )
//{
//    zero();
//    if( usage == EBufferUsageUnknown )
//    {   //  no usage is specified
//        return( false );
//    }
//    if( ( usage & EMemoryConfigImmutable ) && ( usage & ( EMemoryConfigTransient | EMemoryConfigReadable | EMemoryConfigWritable ) ) )
//    {   //  immutable cannot be used with any other access rights
//        return( false );
//    }
//    if( usage & ( EMemoryConfigReadable | EMemoryConfigWritable ) )
//    {
//    }
//
//    UINT buffer_type = ( usage & EMemoryConfigMaskType );
//    if( ( buffer_type < EMemoryConfigMinBuffer ) || ( buffer_type > EMemoryConfigMaxBuffer ) )
//    {   //  not a buffer type
//        return( false );
//    }
//    if( usage & ( EMemoryConfigDSV | EMemoryConfigClampMips | EMemoryConfigMultiSampled | EMemoryConfigMaskSamples | EMemoryConfigMaskQuality ) )
//    {   //  these flags are invalid for use with buffers
//        return( false );
//    }
//    switch( usage & EMemoryConfigMaskBehaviour )
//    {
//        case( EMemoryConfigRandomAccess ):
//        {   //  random access requires a raw buffer
//            if( buffer_type != EMemoryConfigRaw )
//            {
//                return( false );
//            }
//            break;
//        }
//        case( EMemoryConfigStructuredRW ):
//        case( EMemoryConfigAppendConsume ):
//        {   //  StructuredRW and Append/Consume buffers must be structured
//            if( buffer_type != EMemoryConfigStruct )
//            {
//                return( false );
//            }
//            break;
//        }
//        default:
//        {
//            break;
//        }
//    }
//    if( ( usage & EMemoryConfigConsts ) && ( stride & 15 ) )
//    {   //  constant buffer stride must be a multiple of 16
//        return( false );
//    }
//    if( ( usage & ( EMemoryConfigStream | EMemoryConfigStreamOut ) ) && ( stride & 3 ) )
//    {   //  stream buffer stride must be a multiple of 4
//        return( false );
//    }
//    if( ( usage & EMemoryConfigIndex ) && ( stride != 2 ) && ( stride != 4 ) )
//    {   //  index buffer stride must be 2 or 4
//        return( false );
//    }
//    setUsage( usage );
//    setStride( stride );
//    setCount( count );
//    return( true );
//}
//
//bool CBufferDesc::setTexture( const UINT usage, const UINT format, const UINT count, const UINT mips, const UINT width, const UINT height, const UINT depth )
//{
//    zero();
//    if( usage == EBufferUsageUnknown )
//    {   //  no usage is specified
//        return( false );
//    }
//    if( ( usage & EMemoryConfigImmutable ) && ( usage & ( EMemoryConfigTransient | EMemoryConfigReadable | EMemoryConfigWritable ) ) )
//    {   //  immutable cannot be used with any other access rights
//        return( false );
//    }
//    UINT buffer_type = ( usage & EMemoryConfigMaskType );
//    if( ( buffer_type < EMemoryConfigMinTexture ) || ( buffer_type > EMemoryConfigMaxTexture ) )
//    {   //  not a texture type
//        return( false );
//    }
//    if( usage & ( EMemoryConfigConsts | EMemoryConfigStream | EMemoryConfigStreamOut | EMemoryConfigIndex | EMemoryConfigParams | EMemoryConfigMaskBehaviour ) )
//    {   //  these flags are invalid for use with textures
//        return( false );
//    }
//    if( ( ( usage & EMemoryConfigMaskViews ) == 0 ) || ( ( usage & EMemoryConfigMaskRasterViews ) == EMemoryConfigMaskRasterViews ) )
//    {   //  textures must have a view and cannot be both render targets and depth stencils
//        return( false );
//    }
//
//
//    if( ( usage != EBufferUsageUnknown ) && ( ( usage & ~( EBufferBindMaskUsage | EBufferBindMultiSampled ) ) == 0 ) )
//    {
//        if( ( ( usage & EBufferBindViews ) != 0 ) && ( ( usage & EBufferBindRasterViews ) != EBufferBindRasterViews ) )
//        {
//            switch( usage & EBufferBindTextureTypes )
//            {
//                case( EBufferBind1D ):
//                {   //  1D texture type
//                    if( ( ( usage & EBufferBindMultiSampled ) == 0 ) && ( ( height < 2 ) && ( depth < 2 ) ) )
//                    {
//                        setUsage( usage & ~EBufferBindArray );
//                        setFormat( format );
//                        setMips( mips );
//                        setStride( static_cast< uint32_t >( getETextureFormatStride( static_cast< ETextureFormat >( format ) ) ) );
//                        setWidth( width );
//                        setCount( 1 );
//                        return( true );
//                    }
//                    break;
//                }
//                case( EBufferBind2D ):
//                {   //  2D texture type
//                    if( depth < 2 )
//                    {
//                        setUsage( usage & ~EBufferBindArray );
//                        setFormat( format );
//                        setMips( mips );
//                        setStride( static_cast< uint32_t >( getETextureFormatStride( static_cast< ETextureFormat >( format ) ) ) );
//                        setWidth( width );
//                        setHeight( height );
//                        setCount( 1 );
//                        return( true );
//                    }
//                    break;
//                }
//                case( EBufferBind3D ):
//                {   //  3D texture type
//                    if( ( usage & EBufferBindMultiSampled ) == 0 )
//                    {
//                        setUsage( usage & ~EBufferBindArray );
//                        setFormat( format );
//                        setMips( mips );
//                        setStride( static_cast< uint32_t >( getETextureFormatStride( static_cast< ETextureFormat >( format ) ) ) );
//                        setWidth( width );
//                        setHeight( height );
//                        setDepth( depth );
//                        setCount( 1 );
//                        return( true );
//                    }
//                    break;
//                }
//                case( EBufferBindCube ):
//                {   //  Cube texture type
//                    if( ( ( usage & EBufferBindMultiSampled ) == 0 ) && ( ( ( height == 0 ) && ( depth == 0 ) ) || ( ( height == width ) && ( depth == width ) ) ) )
//                    {
//                        setUsage( usage & ~EBufferBindArray );
//                        setFormat( format );
//                        setMips( mips );
//                        setStride( static_cast< uint32_t >( getETextureFormatStride( static_cast< ETextureFormat >( format ) ) ) );
//                        setWidth( width );
//                        setCount( 1 );
//                        return( true );
//                    }
//                    break;
//                }
//                default:
//                {
//                    break;
//                }
//            }
//        }
//    }
//    return( false );
//}
//
//int CBufferDesc::compare( const CBufferDesc& desc ) const
//{
//    return( memcmp( reinterpret_cast< const void* >( this ), reinterpret_cast< const void* >( &desc ), sizeof( desc ) ) );
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CAccessDesc memory access description class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void CAccessDesc::set( const CAccessDesc& desc )
//{
//    m_usage = desc.m_usage;
//    m_format = desc.m_format;
//    m_plane = desc.m_plane;
//    m_mip_index = desc.m_mip_index;
//    m_mip_count = desc.m_mip_count;
//    m_array_start = desc.m_array_start;
//    m_array_count = desc.m_array_count;
//}
//
//void CAccessDesc::setBuffer( const UINT index, const uint count )
//{
//    zero();
//    m_usage = EAccessBindBuffer;
//    m_format = static_cast< uint8_t >( ETextureFormat_UNKNOWN );
//    m_plane = 0;
//    m_mip_index = 0;
//    m_mip_count = 1;
//    m_array_start = static_cast< uint16_t >( index );
//    m_array_count = static_cast< uint16_t >( count );
//}
//
//void CAccessDesc::setRawBuffer( const UINT index, const uint count )
//{
//    setBuffer( index, count );
//    m_usage |= EAccessBindRaw;
//}
//
//void CAccessDesc::setAppendBuffer( const UINT index, const uint count )
//{
//    setBuffer( index, count );
//    m_usage |= EAccessBindAppend;
//}
//
//void CAccessDesc::setCounterBuffer( const UINT index, const uint count )
//{
//    setBuffer( index, count );
//    m_usage |= EAccessBindCounter;
//}
//
//void CAccessDesc::setTexture1D( const UINT array_start, const UINT array_count )
//{
//    zero();
//    m_usage = ( ( array_count != 1 ) ? ( EAccessBind1D | EAccessBindArray ) : EAccessBind1D );
//    m_format = static_cast< uint8_t >( ETextureFormat_UNKNOWN );
//    m_plane = 0;
//    m_mip_index = 0;
//    m_mip_count = 1;
//    m_array_start = static_cast< uint16_t >( array_start );
//    m_array_count = static_cast< uint16_t >( array_count );
//}
//
//void CAccessDesc::setTexture2D( const UINT array_start, const UINT array_count )
//{
//    zero();
//    m_usage = ( ( array_count != 1 ) ? ( EAccessBind2D | EAccessBindArray ) : EAccessBind2D );
//    m_format = static_cast< uint8_t >( ETextureFormat_UNKNOWN );
//    m_plane = 0;
//    m_mip_index = 0;
//    m_mip_count = 1;
//    m_array_start = static_cast< uint16_t >( array_start );
//    m_array_count = static_cast< uint16_t >( array_count );
//}
//
//void CAccessDesc::setTexture3D( const UINT slice_start, const UINT slice_count )
//{
//    zero();
//    m_usage = EAccessBind3D;
//    m_format = static_cast< uint8_t >( ETextureFormat_UNKNOWN );
//    m_plane = 0;
//    m_mip_index = 0;
//    m_mip_count = 1;
//    m_array_start = static_cast< uint16_t >( slice_start );
//    m_array_count = static_cast< uint16_t >( slice_count );
//}
//
//void CAccessDesc::setTextureCube( const UINT array_start, const UINT cube_count )
//{
//    zero();
//    m_usage = ( ( cube_count != 1 ) ? ( EAccessBindCube | EAccessBindArray ) : EAccessBindCube );
//    m_format = static_cast< uint8_t >( ETextureFormat_UNKNOWN );
//    m_plane = 0;
//    m_mip_index = 0;
//    m_mip_count = 1;
//    m_array_start = static_cast< uint16_t >( array_start );
//    m_array_count = static_cast< uint16_t >( cube_count );
//}
//
//int CAccessDesc::compare( const CAccessDesc& desc ) const
//{
//    return( memcmp( reinterpret_cast< const void* >( this ), reinterpret_cast< const void* >( &desc ), sizeof( desc ) ) );
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end rendering namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    virtual bool                create( const CBufferDesc& desc, const CBufferData& data, ResourceInternal& resourceInternal );
//    virtual bool                create( const CBufferDesc& desc, ResourceInternal& resourceInternal );
//    virtual bool                create( const CBufferDesc& desc, ResourceReadable& resourceReadable );
//    virtual bool                create( const CBufferDesc& desc, ResourceWritable& resourceWritable );

//  these resources must be registerable

//  keep a list of display relative buffers?

//  resource registry:
//      add resource library
//      create bindings to group
//
//  resource library:
//      create resources to registry
//      create bindings to group
//      create both



//  buffer and texture types:

//  CBufferDesc     -   stride, count, flags
//  initialisation/update structure


//  CBufferDesc buffer description functions:
//      setArgsBuffer( const UINT stride, const UINT count );
//      setIndexBuffer( const UINT stride, const UINT count );
//      setStreamBuffer( const UINT stride, const UINT count );
//      setConstantBuffer( const UINT stride, const UINT count );

//      setSimpleBuffer( const UINT size, const UINT flags );


//      setBuffer( const UINT usage, const UINT stride, const UINT count );
//      setTexture( const UINT usage, const UINT format, const UINT mips, const UINT width, const UINT height, const UINT depth );
//      setTextureArray( const UINT usage, const UINT format, const UINT mips, const UINT count, const UINT width, const UINT height );

//  need to specify meaning of usage (including multi-sampling)




//  need to clarify binding flags
//      srv, uav, index, vertex, out

//  Buffer/texture objects:
//      CStaticBuffer - initialise once
//      CSystemBuffer - internal buffer
//      CUpdateBuffer - had update method
//      COutputBuffer - has output method

//  






    //void                        setConstant( const UINT count );
    //void                        setVertex( const UINT count, const UINT stride );
    //void                        setIndex( const UINT count, const UINT stride );
    //void                        setStructured( const UINT count, const UINT stride );
    //void                        setRaw( const UINT bytes );
    //
    //void                        set1D( const UINT width, const UINT mips, const UINT count );
    //void                        set2D( const UINT width, const UINT height, const UINT mips, const UINT count );
    //void                        set3D( const UINT width, const UINT height, const UINT depth, const UINT count );
    //void                        setMS( const UINT width, const UINT height, const UINT mips, const UINT samples, const UINT quality, const UINT count );
    //void                        setCube( const UINT size, const UINT mips, const UINT count );

//  buffers are often dynamic, textures are rarely dynamic, render targets and depth stencils are not dynamic

//  everything is rarely CPU readable

//  CreateBuffer
//  CreateTexture1D
//  CreateTexture2D
//  CreateTexture3D

//  format (8 bits), stride(16 bits), width, height, depth, mips (8 bits), flags
//
//  flags needs to include:
//      4 bits: 1d, 2d, 3d, cube, raw, structured, args, const, vertex, index
//      binding (SO,SR,UA,RT,DS,DC,EC)
//      access (cpu read, cpu write, immutable, internal, dynamic, output, transient, mip limit)
//      layout (undefined, swizzle, row major)

//  buffer access types
//      raw write once
//      raw dynamic
//      raw dynamic transient
//      structured write once
//      structured dynamic
//      structured dynamic transient
//      stream write once
//      stream dynamic
//      stream dynamic transient
//      stream internal
//      stream output
//      index write once
//      index dynamic
//      consts write once
//      consts dynamic
//      args internal

//  raw, structured, consts, stream, index, args
//  immutable, dynamic, transient, internal, output
//  uav, srv
//  uav append/consume etc

//  texture access types
//      texture write once
//      texture dynamic
//      texture output

//      render target (srv, uav, rtv, readback flags)       any combination but must be at least 1 flag set, readback can only be set with rtv
//      depth stencil (srv, uav, dsv, readback flags)       any combination but must be at least 1 flag set, readback can only be set with dsv
//      texture       (srv, uav, flags)                     either or both flags ok but at least 1 must be specified








//typedef struct D3D11_BUFFER_DESC {
//  UINT        ByteWidth;
//  D3D11_USAGE Usage;
//  UINT        BindFlags;
//  UINT        CPUAccessFlags;
//  UINT        MiscFlags;
//  UINT        StructureByteStride;
//} D3D11_BUFFER_DESC;
//
//typedef struct D3D11_TEXTURE1D_DESC {
//  UINT        Width;
//  UINT        MipLevels;
//  UINT        ArraySize;
//  DXGI_FORMAT Format;
//  D3D11_USAGE Usage;
//  UINT        BindFlags;
//  UINT        CPUAccessFlags;
//  UINT        MiscFlags;
//} D3D11_TEXTURE1D_DESC;
//
//typedef struct D3D11_TEXTURE2D_DESC {
//  UINT             Width;
//  UINT             Height;
//  UINT             MipLevels;
//  UINT             ArraySize;
//  DXGI_FORMAT      Format;
//  DXGI_SAMPLE_DESC SampleDesc;
//  D3D11_USAGE      Usage;
//  UINT             BindFlags;
//  UINT             CPUAccessFlags;
//  UINT             MiscFlags;
//} D3D11_TEXTURE2D_DESC;
//
//typedef struct D3D11_TEXTURE3D_DESC {
//  UINT        Width;
//  UINT        Height;
//  UINT        Depth;
//  UINT        MipLevels;
//  DXGI_FORMAT Format;
//  D3D11_USAGE Usage;
//  UINT        BindFlags;
//  UINT        CPUAccessFlags;
//  UINT        MiscFlags;
//} D3D11_TEXTURE3D_DESC;



//  CreateRenderTargetView
//  CreateDepthStencilView
//  CreateShaderResourceView
//  CreateUnorderedAccessView


//typedef struct VIEW_DESC {
//  DXGI_FORMAT         Format;
//  D3D11_DIMENSION       ViewDimension;
//  buffer/subset description
//} VIEW_DESC;
//
//typedef struct D3D11_BUFFER {
//
//    //  buffer type dependent
//  union {
//    UINT FirstElement;
//    UINT ElementOffset;
//  };
//  union {
//    UINT NumElements;
//    UINT ElementWidth;
//  };
//
//  UINT FirstElement;
//  UINT NumElements;
//  UINT Flags;
//} D3D11_BUFFER_SRV;
//
//typedef struct D3D11_SUBSET {
////  1d, 2d, 3d, cube + ms + array
//  UINT MostDetailedMip;     // srv only, not multi-sampled
//  UINT MipLevels;           // srv only, not multi-sampled
//  UINT MipSlice;            // not srv , not multi-sampled
//  UINT FirstArraySlice;     // array only
//  UINT ArraySize;           // array only
//  UINT FirstWSlice;         // 3d only, not srv
//  UINT WSize;               // 3d only, not srv
//  UINT First2DArrayFace;    // cube only, srv only
//  UINT NumCubes;            // cube only, srv only
//} D3D11_TEX1D_ARRAY_UAV;
//
//typedef enum D3D11_DIMENSION {
//  D3D11_DIMENSION_UNKNOWN            = 0,   //  srv, rtv, dsv, uav
//  D3D11_DIMENSION_BUFFER             = 1,   //  srv, rtv,      uav
//  D3D11_DIMENSION_TEXTURE1D          = 2,   //  srv, rtv, dsv, uav
//  D3D11_DIMENSION_TEXTURE1DARRAY     = 3,   //  srv, rtv, dsv, uav
//  D3D11_DIMENSION_TEXTURE2D          = 4,   //  srv, rtv, dsv, uav
//  D3D11_DIMENSION_TEXTURE2DARRAY     = 5,   //  srv, rtv, dsv, uav
//  D3D11_DIMENSION_TEXTURE2DMS        = 6,   //  srv, rtv, dsv
//  D3D11_DIMENSION_TEXTURE2DMSARRAY   = 7,   //  srv, rtv, dsv
//  D3D11_DIMENSION_TEXTURE3D          = 8,   //  srv, rtv,      uav
//  D3D11_DIMENSION_TEXTURECUBE        = 9,   //  srv
//  D3D11_DIMENSION_TEXTURECUBEARRAY   = 10,  //  srv
//  D3D11_DIMENSION_BUFFEREX           = 11   //  srv
//} D3D11_DIMENSION;


//  DBuffer -   generic data buffer
//  RBuffer -   raw/byte address buffer
//  SBuffer -   structured buffer
//  CBuffer -   constant buffer
//  TBuffer -   texture constant buffer
//  Texture1D
//  TextureArray1D
//  Texture2D
//  TextureArray2D
//  Texture3D
//  TextureArray3D
//  TextureCube
//  TextureArrayCube
//  TextureMS
//  TextureArrayMS

//  buffer write methods:
//      map/unmap dynamic or staging
//      CopyStructureCount      -   target: default or staging or dynamic
//      CopyResource            -   target: default or staging or dynamic
//      CopySubresourceRegion   -   target: default or staging or dynamic
//      UpdateSubresource       -   target: default or staging

//  buffer read methods:
//      map/unmap staging
//      CopyStructureCount      -   target: default or staging or dynamic
//      CopyResource            -   target: default or staging or dynamic
//      CopySubresourceRegion   -   target: default or staging or dynamic
//      ResolveSubResource      -   target: default

//  map/unmap sync all others async

//  read/write normally requires multiple staging buffers
//  UpdateSubresource creates temporary storage for writes

//  cpu access flags only apply to mappable resources (staging or dynamic)
//  cpu access not allowed for structured, raw, cubmap, args and clamped textures

//  stream outputs, render targets and depth stencils must be in default memory

//  raw/counter/append-consume

//  dot(a,a)*dot(b,b)-dot(a,b)*dot(a,b) = 0




};	//	namespace rendering

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
