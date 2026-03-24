
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:    buffer_descs.h
////    Author:  Ritchie Brannan
////    Date:    11 Nov 10
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
////    Manifest:
////
////        enum    EMemoryConfig               :   memory buffer configuration flags
////        enum    EBufferUsage                :   pre-defined buffer types (using EMemoryConfig flags)
////        macro   MultiSampledUsage()         :   multi-sampled usage creation helper macro
////        macro   UseDisplayWidth             :   flag indicating use of display width
////        macro   UseDisplayHeight            :   flag indicating use of display height
////        macro   UseDisplayRelativeWidth()   :   macro to indicate use of display relative width
////        macro   UseDisplayRelativeHeight()  :   macro to indicate use of display relative height
////        class   CBufferData                 :   data exchange description class
////        class   CBufferDesc                 :   memory buffer description class
////        class   CAccessDesc                 :   memory access description class
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

#ifndef __PUBLIC_BUFFER_DESCS_INCLUDED__
#define __PUBLIC_BUFFER_DESCS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "formats.h"

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
////    EMemoryConfig memory configuration flags
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//enum EMemoryConfig
//{
//    EMemoryConfigNone               = 0x00000000,           //  invalid
//    EMemoryConfig1D                 = 0x00000001,           //  base type: 1D texture
//    EMemoryConfig1D_Array           = 0x00000002,           //  base type: 1D texture array
//    EMemoryConfig2D                 = 0x00000003,           //  base type: 2D texture
//    EMemoryConfig2D_Array           = 0x00000004,           //  base type: 2D texture array
//    EMemoryConfigCube               = 0x00000005,           //  base type: Cube texture
//    EMemoryConfigCube_Array         = 0x00000006,           //  base type: Cube texture array
//    EMemoryConfig3D                 = 0x00000007,           //  base type: 3D texture
//    EMemoryConfigBuffer             = 0x00000008,           //  base type: standard buffer
//    EMemoryConfigStruct             = 0x00000009,           //  base type: structured buffer
//    EMemoryConfigRaw                = 0x0000000a,           //  base type: byte address buffer
//    EMemoryConfigMinTexture         = EMemoryConfig1D,      //  base type minimum texture value
//    EMemoryConfigMaxTexture         = EMemoryConfig3D,      //  base type maximum texture value
//    EMemoryConfigMinBuffer          = EMemoryConfigBuffer,  //  base type minimum buffer type value
//    EMemoryConfigMaxBuffer          = EMemoryConfigRaw,     //  base type maximum buffer type value
//    EMemoryConfigMinType            = EMemoryConfig1D,      //  base type minimum type value
//    EMemoryConfigMaxType            = EMemoryConfigRaw,     //  base type maximum type value
//    EMemoryConfigMaskType           = 0x0000000f,           //  base type mask
//    EMemoryConfigImmutable          = 0x00000010,           //  access rights: immutable (must be created with initial data)
//    EMemoryConfigTransient          = 0x00000020,           //  access rights: transient residence for index, stream and constant buffers
//    EMemoryConfigReadable           = 0x00000040,           //  access rights: cpu readable (must be created as a readable resource)
//    EMemoryConfigWritable           = 0x00000080,           //  access rights: cpu writable (must be created as a writable resource)
//    EMemoryConfigMaskAccess         = 0x000000f0,           //  access rights: mask (not all access flag combinations are valid)
//    EMemoryConfigSRV                = 0x00000100,           //  binding point: shader resource view
//    EMemoryConfigUAV                = 0x00000200,           //  binding point: unordered access view
//    EMemoryConfigRTV                = 0x00000400,           //  binding point: render target view
//    EMemoryConfigDSV                = 0x00000800,           //  binding point: depth stencil view
//    EMemoryConfigConsts             = 0x00001000,           //  binding point: constant buffer/constant buffer view
//    EMemoryConfigStream             = 0x00002000,           //  binding point: vertex stream
//    EMemoryConfigStreamOut          = 0x00004000,           //  binding point: vertex output stream
//    EMemoryConfigIndex              = 0x00008000,           //  binding point: index buffer
//    EMemoryConfigParams             = 0x00010000,           //  binding point: args buffer
//    EMemoryConfigMaskBinding        = 0x0001ff00,           //  binding points mask
//    EMemoryConfigMaskSimpleViews    = ( EMemoryConfigSRV | EMemoryConfigUAV ),
//    EMemoryConfigMaskRasterViews    = ( EMemoryConfigRTV | EMemoryConfigDSV ),
//    EMemoryConfigMaskViews          = ( EMemoryConfigMaskSimpleViews | EMemoryConfigMaskRasterViews ),
//    EMemoryConfigUseBehaviour       = 0x00080000,           //  behaviour enable flag
//    EMemoryConfigRandomAccess       = 0x000a0000,           //  behaviour: byte address buffer
//    EMemoryConfigStructuredRW       = 0x000c0000,           //  behaviour: structured read/write buffer
//    EMemoryConfigAppendConsume      = 0x000e0000,           //  behaviour: structured append/consume buffer
//    EMemoryConfigMaskBehaviour      = 0x000e0000,           //  behaviour mask
//    EMemoryConfigClampMips          = 0x00100000,           //  enable mip clamping of 1D, 2D and Cube resources (including arrays)
//    EMemoryConfigMultiSampled       = 0x00200000,           //  enable multi-sampling of 2D resources
//    EMemoryConfigMaskUsage          = 0x003fffff,           //  usage control mask
//    EMemoryConfigMaskSamples        = 0x0fc00000,           //  multi-sampling sample count mask
//    EMemoryConfigMaskQuality        = 0xf0000000,           //  multi-sampling quality level mask
//    EMemoryConfigShiftSamples       = 22,                   //  multi-sampling sample count bit shift
//    EMemoryConfigShiftQuality       = 30,                   //  multi-sampling quality level bit shift
//    EMemoryConfigMaskSampling       = ( EMemoryConfigMaskSamples | EMemoryConfigMaskQuality ),
//    EMemoryConfigForce32            = 0x7fffffff
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EBufferUsage buffer usage enumeration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//enum EBufferUsage
//{
//    EBufferUsageUnknown             = 0,
//    EBufferIndex                    = EMemoryConfigIndex,
//    EBufferStream                   = EMemoryConfigStream,
//    EBufferStreamOut                = EMemoryConfigStreamOut,
//    EBufferShaderConsts             = EMemoryConfigConsts,
//    EBufferShaderBuffer             = ( EMemoryConfigBuffer | EMemoryConfigSRV ),
//    EBufferShaderStruct             = ( EMemoryConfigStruct | EMemoryConfigSRV ),
//    EBufferShaderRaw                = ( EMemoryConfigRaw | EMemoryConfigSRV ),
//    EBufferTexture1D                = ( EMemoryConfig1D | EMemoryConfigSRV ),
//    EBufferTexture2D                = ( EMemoryConfig2D | EMemoryConfigSRV ),
//    EBufferTexture3D                = ( EMemoryConfig3D | EMemoryConfigSRV ),
//    EBufferTextureCube              = ( EMemoryConfigCube | EMemoryConfigSRV ),
//    EBufferCompute1D                = ( EMemoryConfig1D | EMemoryConfigUAV ),
//    EBufferCompute2D                = ( EMemoryConfig2D | EMemoryConfigUAV ),
//    EBufferCompute3D                = ( EMemoryConfig3D | EMemoryConfigUAV ),
//    EBufferComputeCube              = ( EMemoryConfigCube | EMemoryConfigUAV ),
//    EBufferSwapChain                = ( EMemoryConfig2D | EMemoryConfigRTV ),
//    EBufferRenderTargetTex2D        = ( EMemoryConfig2D | EMemoryConfigRTV | EMemoryConfigSRV ),
//    EBufferRenderTargetTex3D        = ( EMemoryConfig3D | EMemoryConfigRTV | EMemoryConfigSRV ),
//    EBufferRenderTargetTexCube      = ( EMemoryConfigCube | EMemoryConfigRTV | EMemoryConfigSRV ),
//    EBufferDepthStencilTex2D        = ( EMemoryConfig2D | EMemoryConfigDSV | EMemoryConfigSRV ),
//    EBufferDepthStencilTex3D        = ( EMemoryConfig3D | EMemoryConfigDSV | EMemoryConfigSRV ),
//    EBufferDepthStencilTexCube      = ( EMemoryConfigCube | EMemoryConfigDSV | EMemoryConfigSRV ),
//    EBufferDepthStencil2D           = ( EMemoryConfig2D | EMemoryConfigDSV ),
//    EBufferDepthStencil3D           = ( EMemoryConfig3D | EMemoryConfigDSV ),
//    EBufferDepthStencilCube         = ( EMemoryConfigCube | EMemoryConfigDSV ),
//    EBufferUsageForce32             = 0x7fffffff
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Helper macros:
////
////        MultiSampledUsage()         :   create multi-sampled usage
////        UseDisplayWidth             :   use to make width or height equal to the display width
////        UseDisplayHeight            :   use to make width or height equal to the display height
////        UseDisplayRelativeWidth()   :   use to make width or height relative to the display width
////        UseDisplayRelativeHeight()  :   use to make width or height relative to the display height
////
////    The display relative macros:
////
////        These divide the specified display dimension by the given amount and round up.
////        If pow2 is not specified the rounding is to an integer.
////        If pow2 is specified the rounding is to an integer power of 2.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define MultiSampledUsage( usage, samples, quality )    ( ( static_cast< UINT >( usage ) & EMemoryConfigMaskUsage ) | ( ( static_cast< UINT >( samples ) << EMemoryConfigShiftSamples ) & EMemoryConfigMaskSamples ) | ( ( static_cast< UINT >( quality ) << EMemoryConfigShiftQuality ) & EMemoryConfigMaskQuality ) | EMemoryConfigMultiSampled )
//#define UseDisplayWidth                                 0x00008001
//#define UseDisplayHeight                                0x0000c001
//#define UseDisplayRelativeWidth( divide, pow2 )         ( ( ( divide ) ? ( static_cast< UINT >( divide ) & 0x00001fff ) : 0x00000001 ) | UINT( ( pow2 ) ? 0x0000a000 : 0x00008000 ) )
//#define UseDisplayRelativeHeight( divide, pow2 )        ( ( ( divide ) ? ( static_cast< UINT >( divide ) & 0x00001fff ) : 0x00000001 ) | UINT( ( pow2 ) ? 0x0000e000 : 0x0000c000 ) )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBufferData memory buffer data exchange description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class CBufferData
//{
//public:
//    inline                      CBufferData() { zero(); };
//    inline                      CBufferData( const CBufferData& data ) { set( data ); };
//    inline                      CBufferData( void* const data, const uint32_t stepY, const uint32_t stepZ = 0 ) { set( data, stepY, stepZ ); };
//    inline                      ~CBufferData() { zero(); };
//    inline void                 set( const CBufferData& data ) { m_data = data.m_data; m_stepY = data.m_stepY; m_stepZ = data.m_stepZ; };
//    inline void                 set( void* const data, const uint32_t stepY, const uint32_t stepZ = 0 ) { m_data = data; m_stepY = stepY; m_stepZ = stepZ; };
//    inline void*                getData() const { return( m_data ); };
//    inline uint32_t             getStepY() const { return( m_stepY ); };
//    inline uint32_t             getStepZ() const { return( m_stepZ ); };
//    inline CBufferData&         operator=( const CBufferData& data ) { set( data ); return( *this ); };
//protected:
//    inline void                 zero() { m_data = NULL; m_stepY = 0; m_stepZ = 0; };
//    void*                       m_data;
//    uint32_t                    m_stepY;
//    uint32_t                    m_stepZ;
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBufferDesc memory buffer description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class CBufferDesc
//{
//public:
//    inline                      CBufferDesc() { zero(); };
//    inline                      CBufferDesc( const CBufferDesc& desc ) { set( desc ); };
//    inline                      ~CBufferDesc() { zero(); };
//    void                        setDefault() { zero(); };
//    void                        set( const CBufferDesc& desc );
//    bool                        setBuffer( const UINT usage, const UINT stride, const UINT count = 1 );
//    bool                        setTexture( const UINT usage, const UINT format, const UINT count, const UINT mips, const UINT width, const UINT height = 0, const UINT depth = 0 );
//    inline UINT                 getUsage() const { return( static_cast< UINT >( m_usage ) ); };
//    inline UINT                 getFormat() const { return( static_cast< UINT >( m_format ) ); };
//    inline UINT                 getCount() const { return( static_cast< UINT >( m_count ) ); };
//    inline UINT                 getMips() const { return( static_cast< UINT >( m_mips ) ); };
//    inline UINT                 getStride() const { return( static_cast< UINT >( m_stride ) ); };
//    inline UINT                 getWidth() const { return( static_cast< UINT >( m_width ) ); };
//    inline UINT                 getHeight() const { return( static_cast< UINT >( m_height ) ); };
//    inline UINT                 getDepth() const { return( static_cast< UINT >( m_depth ) ); };
//    int                         compare( const CBufferDesc& desc ) const;
//    inline CBufferDesc&         operator=( const CBufferDesc& desc ) { set( desc ); return( *this ); };
//protected:
//    inline void                 zero() { m_usage = 0; m_format = m_mips = 0; m_stride = m_count = 0; m_width = m_height = m_depth = 1; };
//    inline void                 setUsage( const UINT usage ) { m_usage = static_cast< uint32_t >( usage ); };
//    inline void                 setFormat( const UINT format ) { m_format = static_cast< uint8_t >( format ); };
//    inline void                 setCount( const UINT count ) { m_count = static_cast< uint16_t >( count ? count : 1 ); };
//    inline void                 setMips( const UINT mips ) { m_mips = static_cast< uint8_t >( mips ); };
//    inline void                 setStride( const UINT stride ) { m_stride = static_cast< uint16_t >( stride ); };
//    inline void                 setWidth( const UINT width ) { m_width = static_cast< uint16_t >( width ); };
//    inline void                 setHeight( const UINT height ) { m_height = static_cast< uint16_t >( height ? height : 1 ); };
//    inline void                 setDepth( const UINT depth ) { m_depth = static_cast< uint16_t >( depth ? depth : 1 ); };
//    uint32_t                    m_usage;
//    uint8_t                     m_format;
//    uint8_t                     m_mips;
//    uint16_t                    m_stride;
//    uint16_t                    m_width;
//    uint16_t                    m_height;
//    uint16_t                    m_depth;
//    uint16_t                    m_count;
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CAccessDesc memory access description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class CAccessDesc
//{
//public:
//    inline                      CAccessDesc() { zero(); };
//    inline                      CAccessDesc( const CAccessDesc& desc ) { set( desc ); };
//    inline                      ~CAccessDesc() { zero(); };
//    void                        setDefault() { zero(); };
//    void                        set( const CAccessDesc& desc );
//    void                        setBuffer( const UINT index = 0, const uint count = 0 );
//    void                        setRawBuffer( const UINT index = 0, const uint count = 0 );
//    void                        setAppendBuffer( const UINT index = 0, const uint count = 0 );
//    void                        setCounterBuffer( const UINT index = 0, const uint count = 0 );
//    void                        setTexture1D( const UINT array_start = 0, const UINT array_count = 1 );
//    void                        setTexture2D( const UINT array_start = 0, const UINT array_count = 1 );
//    void                        setTexture3D( const UINT slice_start = 0, const UINT slice_count = 0 );
//    void                        setTextureCube( const UINT array_start = 0, const UINT cube_count = 1 );
//    inline void                 setFormat( const UINT format ) { m_format = static_cast< uint8_t >( format ); };
//    inline void                 setPlane( const UINT plane ) { m_plane = static_cast< uint8_t >( plane ); };
//    inline void                 setMip( const UINT index ) { m_mip_index = static_cast< uint8_t >( index ); m_mip_count = 1; };
//    inline void                 setMips( const UINT start, const UINT count ) { m_mip_index = static_cast< uint8_t >( start ); m_mip_count = static_cast< uint8_t >( count ); };
//    inline UINT                 getUsage() const { return( static_cast< UINT >( m_usage ) ); };
//    inline UINT                 getFormat() const { return( static_cast< UINT >( m_format ) ); };
//    inline UINT                 getPlane() const { return( static_cast< UINT >( m_plane ) ); };
//    inline UINT                 getMip() const { return( static_cast< UINT >( m_mip_index ) ); };
//    inline UINT                 getStartMip() const { return( static_cast< UINT >( m_mip_index ) ); };
//    inline UINT                 getMipCount() const { return( static_cast< UINT >( m_mip_count ) ); };
//    inline UINT                 getArrayStart() const { return( static_cast< UINT >( m_array_start ) ); };
//    inline UINT                 getArrayCount() const { return( static_cast< UINT >( m_array_count ) ); };
//    int                         compare( const CAccessDesc& desc ) const;
//    inline CAccessDesc&         operator=( const CAccessDesc& desc ) { set( desc ); return( *this ); };
//protected:
//    inline void                 zero() { m_usage = 0; m_format = 0; m_plane = 0; m_mip_index = m_mip_count = 0; m_array_start = m_array_count = 0; };
//    uint32_t                    m_usage;
//    uint8_t                     m_format;
//    uint8_t                     m_plane;
//    uint8_t                     m_mip_index;
//    uint8_t                     m_mip_count;
//    uint16_t                    m_array_start;
//    uint16_t                    m_array_count;
//};

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
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //  #ifndef __PUBLIC_BUFFER_DESCS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
