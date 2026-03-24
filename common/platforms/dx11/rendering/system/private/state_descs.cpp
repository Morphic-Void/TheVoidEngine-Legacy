
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   state_descs.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        State description object classes.
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

#include "../public/state_descs.h"
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
////    Internal state bit usage enumerated types
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum ESamplerDescBits
{
    ESamplerDescMinLinearBit        = 0x00000001,
    ESamplerDescMagLinearBit        = 0x00000002,
    ESamplerDescMipLinearBit        = 0x00000004,
    ESamplerDescAnisotropicBit      = 0x00000008,
    ESamplerDescLoddingBit          = 0x00000010,
    ESamplerDescBorderBit           = 0x00000020,
    ESamplerDescModeBits            = 2,
    ESamplerDescCmpFuncBits         = 3,
    ESamplerDescWrapBitsU           = 3,
    ESamplerDescWrapBitsV           = 3,
    ESamplerDescWrapBitsW           = 3,
    ESamplerDescAnisotropyBits      = 4,
    ESamplerDescModeShift           = 6,
    ESamplerDescCmpFuncShift        = (    ESamplerDescModeShift +    ESamplerDescModeBits ),
    ESamplerDescWrapShiftU          = ( ESamplerDescCmpFuncShift + ESamplerDescCmpFuncBits ),
    ESamplerDescWrapShiftV          = (   ESamplerDescWrapShiftU +   ESamplerDescWrapBitsU ),
    ESamplerDescWrapShiftW          = (   ESamplerDescWrapShiftV +   ESamplerDescWrapBitsV ),
    ESamplerDescAnisotropyShift     = (   ESamplerDescWrapShiftW +   ESamplerDescWrapBitsW ),
    ESamplerDescModeMask            = ( ( ( 1 <<       ESamplerDescModeBits ) - 1 ) <<       ESamplerDescModeShift ),
    ESamplerDescCmpFuncMask         = ( ( ( 1 <<    ESamplerDescCmpFuncBits ) - 1 ) <<    ESamplerDescCmpFuncShift ),
    ESamplerDescWrapMaskU           = ( ( ( 1 <<      ESamplerDescWrapBitsU ) - 1 ) <<      ESamplerDescWrapShiftU ),
    ESamplerDescWrapMaskV           = ( ( ( 1 <<      ESamplerDescWrapBitsV ) - 1 ) <<      ESamplerDescWrapShiftV ),
    ESamplerDescWrapMaskW           = ( ( ( 1 <<      ESamplerDescWrapBitsW ) - 1 ) <<      ESamplerDescWrapShiftW ),
    ESamplerDescAnisotropyMask      = ( ( ( 1 << ESamplerDescAnisotropyBits ) - 1 ) << ESamplerDescAnisotropyShift ),
    ESamplerDescBitsForce32         = 0x7fffffff
};

enum EDepthDescBits
{
    EDepthDescDepthReadEnableBit    = 0x00000001,
    EDepthDescDepthWriteEnableBit   = 0x00000002,
    EDepthDescStencilEnableBit      = 0x00000004,
    EDepthDescDepthEnableBits       = ( EDepthDescDepthReadEnableBit | EDepthDescDepthWriteEnableBit ),
    EDepthDescDepthCmpFuncBits      = 3,
    EDepthDescFrontCmpFuncBits      = 3,
    EDepthDescFrontPassBothBits     = 3,
    EDepthDescFrontPassStencilBits  = 3,
    EDepthDescFrontFailStencilBits  = 3,
    EDepthDescBackCmpFuncBits       = 3,
    EDepthDescBackPassBothBits      = 3,
    EDepthDescBackPassStencilBits   = 3,
    EDepthDescBackFailStencilBits   = 3,
    EDepthDescDepthCmpFuncShift     = 3,
    EDepthDescFrontCmpFuncShift     = (     EDepthDescDepthCmpFuncShift +     EDepthDescDepthCmpFuncBits ),
    EDepthDescFrontPassBothShift    = (     EDepthDescFrontCmpFuncShift +     EDepthDescFrontCmpFuncBits ),
    EDepthDescFrontPassStencilShift = (    EDepthDescFrontPassBothShift +    EDepthDescFrontPassBothBits ),
    EDepthDescFrontFailStencilShift = ( EDepthDescFrontPassStencilShift + EDepthDescFrontPassStencilBits ),
    EDepthDescBackCmpFuncShift      = ( EDepthDescFrontFailStencilShift + EDepthDescFrontFailStencilBits ),
    EDepthDescBackPassBothShift     = (      EDepthDescBackCmpFuncShift +      EDepthDescBackCmpFuncBits ),
    EDepthDescBackPassStencilShift  = (     EDepthDescBackPassBothShift +     EDepthDescBackPassBothBits ),
    EDepthDescBackFailStencilShift  = (  EDepthDescBackPassStencilShift +  EDepthDescBackPassStencilBits ),
    EDepthDescDepthCmpFuncMask      = ( ( ( 1 <<     EDepthDescDepthCmpFuncBits ) - 1 ) <<     EDepthDescDepthCmpFuncShift ),
    EDepthDescFrontCmpFuncMask      = ( ( ( 1 <<     EDepthDescFrontCmpFuncBits ) - 1 ) <<     EDepthDescFrontCmpFuncShift ),
    EDepthDescFrontPassBothMask     = ( ( ( 1 <<    EDepthDescFrontPassBothBits ) - 1 ) <<    EDepthDescFrontPassBothShift ),
    EDepthDescFrontPassStencilMask  = ( ( ( 1 << EDepthDescFrontPassStencilBits ) - 1 ) << EDepthDescFrontPassStencilShift ),
    EDepthDescFrontFailStencilMask  = ( ( ( 1 << EDepthDescFrontFailStencilBits ) - 1 ) << EDepthDescFrontFailStencilShift ),
    EDepthDescBackCmpFuncMask       = ( ( ( 1 <<      EDepthDescBackCmpFuncBits ) - 1 ) <<      EDepthDescBackCmpFuncShift ),
    EDepthDescBackPassBothMask      = ( ( ( 1 <<     EDepthDescBackPassBothBits ) - 1 ) <<     EDepthDescBackPassBothShift ),
    EDepthDescBackPassStencilMask   = ( ( ( 1 <<  EDepthDescBackPassStencilBits ) - 1 ) <<  EDepthDescBackPassStencilShift ),
    EDepthDescBackFailStencilMask   = ( ( ( 1 <<  EDepthDescBackFailStencilBits ) - 1 ) <<  EDepthDescBackFailStencilShift ),
    EDepthDescFrontMask             = ( EDepthDescFrontCmpFuncMask | EDepthDescFrontPassBothMask | EDepthDescFrontPassStencilMask | EDepthDescFrontFailStencilMask ),
    EDepthDescBackMask              = ( EDepthDescBackCmpFuncMask | EDepthDescBackPassBothMask | EDepthDescBackPassStencilMask | EDepthDescBackFailStencilMask ),
    EDepthDescBitsForce32           = 0x7fffffff
};

enum ERasterDescBits
{
    ERasterDescAntiClockwiseBit     = 0x00000001,
    ERasterDescScissorBit           = 0x00000002,
    ERasterDescWireframeBit         = 0x00000004,
    ERasterDescDepthClipBit         = 0x00000008,
    ERasterDescMultiSampleBit       = 0x00000010,
    ERasterDescAntiAliasedBit       = 0x00000020,
    ERasterDescBiasEnabledBit       = 0x00000040,
    ERasterDescCullingBits          = 3,
    ERasterDescSamplesBits          = 3,
    ERasterDescDepthBiasBits        = 9,
    ERasterDescCullingShift         = 7,
    ERasterDescSamplesShift         = ( ERasterDescCullingShift + ERasterDescCullingBits ),
    ERasterDescDepthBiasShift       = ( ERasterDescSamplesShift + ERasterDescSamplesBits ),
    ERasterDescCullingMask          = ( ( ( 1 <<   ERasterDescCullingBits ) - 1 ) <<   ERasterDescCullingShift ),
    ERasterDescSamplesMask          = ( ( ( 1 <<   ERasterDescSamplesBits ) - 1 ) <<   ERasterDescSamplesShift ),
    ERasterDescDepthBiasMask        = ( ( ( 1 << ERasterDescDepthBiasBits ) - 1 ) << ERasterDescDepthBiasShift ),
    ERasterDescBitsForce32          = 0x7fffffff
};

enum EBlendConfigBits
{
    EBlendConfigEnableBitRGB        = 0x00000001,
    EBlendConfigEnableBitA          = 0x00000002,
    EBlendConfigSourceMulBitsRGB    = 5,
    EBlendConfigTargetMulBitsRGB    = 5,
    EBlendConfigCombinerBitsRGB     = 3,
    EBlendConfigSourceMulBitsA      = 5,
    EBlendConfigTargetMulBitsA      = 5,
    EBlendConfigCombinerBitsA       = 3,
    EBlendConfigMaskBits            = 4,
    EBlendConfigSourceMulShiftRGB   = 2,
    EBlendConfigTargetMulShiftRGB   = ( EBlendConfigSourceMulShiftRGB + EBlendConfigSourceMulBitsRGB ),
    EBlendConfigCombinerShiftRGB    = ( EBlendConfigTargetMulShiftRGB + EBlendConfigTargetMulBitsRGB ),
    EBlendConfigSourceMulShiftA     = (  EBlendConfigCombinerShiftRGB +  EBlendConfigCombinerBitsRGB ),
    EBlendConfigTargetMulShiftA     = (   EBlendConfigSourceMulShiftA +   EBlendConfigSourceMulBitsA ),
    EBlendConfigCombinerShiftA      = (   EBlendConfigTargetMulShiftA +   EBlendConfigTargetMulBitsA ),
    EBlendConfigMaskShift           = (    EBlendConfigCombinerShiftA +    EBlendConfigCombinerBitsA ),
    EBlendConfigSourceMulMaskRGB    = ( ( ( 1 << EBlendConfigSourceMulBitsRGB ) - 1 ) << EBlendConfigSourceMulShiftRGB ),
    EBlendConfigTargetMulMaskRGB    = ( ( ( 1 << EBlendConfigTargetMulBitsRGB ) - 1 ) << EBlendConfigTargetMulShiftRGB ),
    EBlendConfigCombinerMaskRGB     = ( ( ( 1 <<  EBlendConfigCombinerBitsRGB ) - 1 ) <<  EBlendConfigCombinerShiftRGB ),
    EBlendConfigSourceMulMaskA      = ( ( ( 1 <<   EBlendConfigSourceMulBitsA ) - 1 ) <<   EBlendConfigSourceMulShiftA ),
    EBlendConfigTargetMulMaskA      = ( ( ( 1 <<   EBlendConfigTargetMulBitsA ) - 1 ) <<   EBlendConfigTargetMulShiftA ),
    EBlendConfigCombinerMaskA       = ( ( ( 1 <<    EBlendConfigCombinerBitsA ) - 1 ) <<    EBlendConfigCombinerShiftA ),
    EBlendConfigMaskMask            = ( ( ( 1 <<         EBlendConfigMaskBits ) - 1 ) <<         EBlendConfigMaskShift ),
    EBlendConfigEnableBits          = ( EBlendConfigEnableBitRGB | EBlendConfigEnableBitA ),
    EBlendConfigMaskRGB             = ( EBlendConfigSourceMulMaskRGB | EBlendConfigTargetMulMaskRGB | EBlendConfigCombinerMaskRGB ),
    EBlendConfigMaskA               = ( EBlendConfigSourceMulMaskA | EBlendConfigTargetMulMaskA | EBlendConfigCombinerMaskA ),
    EBlendConfigBitsForce32         = 0x7fffffff
};

enum EBlendDescBits
{
    EBlendDescA2CBit                = 0x00000001,
    EBlendDescTargetCountBits       = 3,
    EBlendDescLogicEnableBits       = 8,
    EBlendDescLogicOpBits           = 4,
    EBlendDescTargetCountShift      = 1,
    EBlendDescLogicEnableShift      = ( EBlendDescTargetCountShift + EBlendDescTargetCountBits ),
    EBlendDescLogicOpShift          = ( EBlendDescLogicEnableShift + EBlendDescLogicEnableBits ),
    EBlendDescTargetCountMask       = ( ( ( 1 << EBlendDescTargetCountBits ) - 1 ) << EBlendDescTargetCountShift ),
    EBlendDescLogicEnableMask       = ( ( ( 1 << EBlendDescLogicEnableBits ) - 1 ) << EBlendDescLogicEnableShift ),
    EBlendDescLogicOpMask           = ( ( ( 1 <<     EBlendDescLogicOpBits ) - 1 ) <<     EBlendDescLogicOpShift ),
    EBlendDescBitsForce32           = 0x7fffffff
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    SAMPLER_DEF sampler state definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SAMPLER_DEF::setDefault()
{
    SamplingMode = ESamplingFilter;
    LinearMin = LinearMag = LinearMip = TRUE;
    WrapU = WrapV = WrapW = EWrapModeClamp;
    AnisotropicMax = 0;
    EnableBorder = FALSE;
    Border[ 0 ] = Border[ 1 ] = Border[ 2 ] = Border[ 3 ] = 1.0f;
    EnableModifyLOD = FALSE;
    MinLOD = 0.0f;
    MaxLOD = 32.0f;
    BiasLOD = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CSamplerDesc sampler state description class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSamplerDesc::setDefault()
{
    m_desc = ( ESamplerDescMinLinearBit | ESamplerDescMagLinearBit | ESamplerDescMipLinearBit );
    m_border[ 0 ] = m_border[ 1 ] = m_border[ 2 ] = m_border[ 3 ] = 1.0f;
    m_minLOD = 0.0f;
    m_maxLOD = 32.0f;
    m_biasLOD = 0.0f;
}

void CSamplerDesc::set( const CSamplerDesc& desc )
{
    m_desc = desc.m_desc;
    m_border[ 0 ] = desc.m_border[ 0 ];
    m_border[ 1 ] = desc.m_border[ 1 ];
    m_border[ 2 ] = desc.m_border[ 2 ];
    m_border[ 3 ] = desc.m_border[ 3 ];
    m_minLOD = desc.m_minLOD;
    m_maxLOD = desc.m_maxLOD;
    m_biasLOD = desc.m_biasLOD;
}

void CSamplerDesc::set( const SAMPLER_DEF& samplerDef )
{
    setDefault();
    setMode( samplerDef.SamplingMode );
    setCmpFunc( samplerDef.CmpFunc );
    setLinear( ( samplerDef.LinearMin ? true : false ), ( samplerDef.LinearMag ? true : false ), ( samplerDef.LinearMip ? true : false ) );
    setWrapping( samplerDef.WrapU, samplerDef.WrapV, samplerDef.WrapW );
    setAnisotropic( static_cast< uint >( samplerDef.AnisotropicMax ) );
    if( samplerDef.EnableBorder )
    {
        setBorder( samplerDef.Border[ 0 ], samplerDef.Border[ 1 ], samplerDef.Border[ 2 ], samplerDef.Border[ 3 ] );
    }
    if( samplerDef.EnableModifyLOD )
    {
        setLodding( samplerDef.MinLOD, samplerDef.MaxLOD, samplerDef.BiasLOD );
    }
}

void CSamplerDesc::setMode( const ESamplingMode mode )
{
    m_desc &= ~ESamplerDescModeMask;
    m_desc |= ( ( static_cast< uint32_t >( mode ) << ESamplerDescModeShift ) & ESamplerDescModeMask );
}

void CSamplerDesc::setCmpFunc( const ECmpFunc func )
{
    m_desc &= ~ESamplerDescCmpFuncMask;
    m_desc |= ( ( static_cast< uint32_t >( func ) << ESamplerDescCmpFuncShift ) & ESamplerDescCmpFuncMask );
}

void CSamplerDesc::setLinear( const bool min, const bool mag, const bool mip )
{
    m_desc &= ~( ESamplerDescMinLinearBit | ESamplerDescMagLinearBit | ESamplerDescMipLinearBit );
    if( min )
    {
        m_desc |= ESamplerDescMinLinearBit;
    }
    if( mag )
    {
        m_desc |= ESamplerDescMagLinearBit;
    }
    if( mip )
    {
        m_desc |= ESamplerDescMipLinearBit;
    }
}

void CSamplerDesc::setWrapping( const EWrapMode u, const EWrapMode v, const EWrapMode w )
{
    m_desc &= ~( ESamplerDescWrapMaskU | ESamplerDescWrapMaskV | ESamplerDescWrapMaskW );
    m_desc |= ( ( static_cast< uint32_t >( u ) << ESamplerDescWrapShiftU ) & ESamplerDescWrapMaskU );
    m_desc |= ( ( static_cast< uint32_t >( v ) << ESamplerDescWrapShiftV ) & ESamplerDescWrapMaskV );
    m_desc |= ( ( static_cast< uint32_t >( w ) << ESamplerDescWrapShiftW ) & ESamplerDescWrapMaskW );
}

void CSamplerDesc::setAnisotropic( const uint max )
{
    m_desc &= ~( ESamplerDescAnisotropicBit | ESamplerDescAnisotropyMask );
    if( max )
    {
        m_desc |= ( ESamplerDescAnisotropicBit | ESamplerDescMinLinearBit | ESamplerDescMagLinearBit | ESamplerDescMipLinearBit );
        m_desc |= ( ( static_cast< uint32_t >( ( max > 16 ) ? 15 : ( max - 1 ) ) << ESamplerDescAnisotropyShift ) & ESamplerDescAnisotropyMask );
    }
}

void CSamplerDesc::setLodding( const float min, const float max, const float bias )
{
    m_desc |= ESamplerDescLoddingBit;
    m_minLOD = min;
    m_maxLOD = max;
    m_biasLOD = bias;
}

void CSamplerDesc::setBorder( const float r, const float g, const float b, const float a )
{
    m_desc |= ESamplerDescBorderBit;
    m_border[ 0 ] = r;
    m_border[ 1 ] = g;
    m_border[ 2 ] = b;
    m_border[ 3 ] = a;
}

ESamplingMode CSamplerDesc::getMode() const
{
    return( static_cast< ESamplingMode >(( m_desc & ESamplerDescModeMask ) >> ESamplerDescModeShift ) );
}

ECmpFunc CSamplerDesc::getCmpFunc() const
{
    return( static_cast< ECmpFunc >(( m_desc & ESamplerDescCmpFuncMask ) >> ESamplerDescCmpFuncShift ) );
}

bool CSamplerDesc::getLinear( bool& min, bool& mag, bool& mip ) const
{
    if( m_desc & ESamplerDescAnisotropicBit )
    {
        min = true;
        mag = true;
        mip = true;
        return( false );
    }
    else
    {
        min = ( ( m_desc & ESamplerDescMinLinearBit ) ? true : false );
        mag = ( ( m_desc & ESamplerDescMagLinearBit ) ? true : false );
        mip = ( ( m_desc & ESamplerDescMipLinearBit ) ? true : false );
        return( true );
    }
}

void CSamplerDesc::getWrapping( EWrapMode& u, EWrapMode& v, EWrapMode& w ) const
{
    u = static_cast< EWrapMode >( ( m_desc & ESamplerDescWrapMaskU ) >> ESamplerDescWrapShiftU );
    v = static_cast< EWrapMode >( ( m_desc & ESamplerDescWrapMaskV ) >> ESamplerDescWrapShiftV );
    w = static_cast< EWrapMode >( ( m_desc & ESamplerDescWrapMaskW ) >> ESamplerDescWrapShiftW );
}

uint CSamplerDesc::getAnisotropic() const
{
    return( ( m_desc & ESamplerDescAnisotropicBit ) ? static_cast< uint >( ( ( m_desc & ESamplerDescAnisotropyMask ) >> ESamplerDescAnisotropyShift ) + 1 ) : 0 );
}

bool CSamplerDesc::getLodding( float& min, float& max, float& bias ) const
{
    if( m_desc & ESamplerDescLoddingBit )
    {
        min = m_minLOD;
        max = m_maxLOD;
        bias = m_biasLOD;
        return( true );
    }
    else
    {
        min = 0.0f;
        max = 32.0f;
        bias = 0.0f;
        return( false );
    }
}

bool CSamplerDesc::getBorder( float& r, float& g, float& b, float& a ) const
{
    if( m_desc & ESamplerDescLoddingBit )
    {
        r = m_border[ 0 ];
        g = m_border[ 1 ];
        b = m_border[ 2 ];
        a = m_border[ 3 ];
        return( true );
    }
    else
    {
        r = g = b = a = 1.0f;
        return( false );
    }
}

bool CSamplerDesc::hasLodding() const
{
    return( ( m_desc & ESamplerDescLoddingBit ) ? true : false );
}

bool CSamplerDesc::hasBorder() const
{
    return( ( m_desc & ESamplerDescBorderBit ) ? true : false );
}

void CSamplerDesc::clearLodding()
{
    m_desc &= ~ESamplerDescLoddingBit;
    m_minLOD = 0.0f;
    m_maxLOD = 32.0f;
    m_biasLOD = 0.0f;
}

void CSamplerDesc::clearBorder()
{
    m_desc &= ~ESamplerDescBorderBit;
    m_border[ 0 ] = m_border[ 1 ] = m_border[ 2 ] = m_border[ 3 ] = 1.0f;
}

int CSamplerDesc::compare( const CSamplerDesc& desc ) const
{
    return( memcmp( reinterpret_cast< const void* >( this ), reinterpret_cast< const void* >( &desc ), sizeof( desc ) ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    RASTER_STATE_DEF raster state definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RASTER_STATE_DEF::setDefault()
{
    Culling = ECullModeNone;
    SamplesUAV = ESamplesUnforced;
    AntiClockwise = EnableScissor = EnableWireframe = EnableDepthClip = EnableMultiSample = EnableAntiAliasedLine = FALSE;
    EnableBiasing = FALSE;
    DepthBias = 0;
    SlopeBias = ClampBias = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CRasterStateDesc rasteriser state description class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CRasterStateDesc::setDefault()
{
    m_desc = 0;
    m_biasSlope = 0.0f;
    m_biasClamp = 0.0f;
}

void CRasterStateDesc::set( const CRasterStateDesc& desc )
{
    m_desc = desc.m_desc;
    m_biasSlope = desc.m_biasSlope;
    m_biasClamp = desc.m_biasClamp;
}

void CRasterStateDesc::set( const RASTER_STATE_DEF& def )
{
    setDefault();
    setCulling( def.Culling );
    setSamplesUAV( def.SamplesUAV );
    setFrontAntiClockwise( def.AntiClockwise ? true : false );
    enableScissor( def.EnableScissor ? true : false );
    enableWireframe( def.EnableWireframe ? true : false );
    enableDepthClip( def.EnableDepthClip ? true : false );
    enableMultiSample( def.EnableMultiSample ? true : false );
    enableAntiAliasedLine( def.EnableAntiAliasedLine ? true : false );
    if( def.EnableBiasing )
    {
        setBiasing( def.DepthBias, def.SlopeBias, def.ClampBias );
    }
}

void CRasterStateDesc::setFrontAntiClockwise( const bool antiClockwise )
{
    if( antiClockwise )
    {
        m_desc |= ERasterDescAntiClockwiseBit;
    }
    else
    {
        m_desc &= ~ERasterDescAntiClockwiseBit;
    }
}

void CRasterStateDesc::setCulling( const ECullMode culling )
{
    m_desc &= ~ERasterDescCullingMask;
    m_desc |= ( ( static_cast< uint32_t >( culling ) << ERasterDescCullingShift ) & ERasterDescCullingMask );
}

void CRasterStateDesc::setBiasing( const int depth, const float slope, const float clamp )
{
    int bias = ( depth + 256 );
    if( bias < 0 ) bias = 0;
    if( bias > 511 ) bias = 511;
    m_desc &= ~ERasterDescDepthBiasMask;
    m_desc |= ERasterDescBiasEnabledBit;
    m_desc |= ( ( static_cast< uint32_t >( bias ) << ERasterDescDepthBiasShift ) & ERasterDescDepthBiasMask );
    m_biasSlope = slope;
    m_biasClamp = clamp;
}

void CRasterStateDesc::setSamplesUAV( const ESamplesUAV samples )
{
    m_desc &= ~ERasterDescSamplesMask;
    m_desc |= ( ( static_cast< uint32_t >( samples ) << ERasterDescSamplesShift ) & ERasterDescSamplesMask );
}

ECullMode CRasterStateDesc::getCulling() const
{
    return( static_cast< ECullMode >( ( m_desc & ERasterDescCullingMask ) >> ERasterDescCullingShift ) );
}

bool CRasterStateDesc::getBiasing( int& depth, float& slope, float& clamp ) const
{
    if( m_desc & ERasterDescBiasEnabledBit )
    {
        depth = ( static_cast< int >( ( m_desc & ERasterDescDepthBiasMask ) >> ERasterDescDepthBiasShift ) - 256 );
        slope = m_biasSlope;
        clamp = m_biasClamp;
        return( true );
    }
    else
    {
        depth = 0;
        slope = 0.0f;
        clamp = 0.0f;
        return( false );
    }
}

ESamplesUAV CRasterStateDesc::getSamplesUAV() const
{
    return( static_cast< ESamplesUAV >( ( m_desc & ERasterDescSamplesMask ) >> ERasterDescSamplesShift ) );
}

void CRasterStateDesc::enableScissor( const bool enable )
{
    if( enable )
    {
        m_desc |= ERasterDescScissorBit;
    }
    else
    {
        m_desc &= ~ERasterDescScissorBit;
    }
}

void CRasterStateDesc::enableWireframe( const bool enable )
{
    if( enable )
    {
        m_desc |= ERasterDescWireframeBit;
    }
    else
    {
        m_desc &= ~ERasterDescWireframeBit;
    }
}

void CRasterStateDesc::enableDepthClip( const bool enable )
{
    if( enable )
    {
        m_desc |= ERasterDescDepthClipBit;
    }
    else
    {
        m_desc &= ~ERasterDescDepthClipBit;
    }
}

void CRasterStateDesc::enableMultiSample( const bool enable )
{
    if( enable )
    {
        m_desc |= ERasterDescMultiSampleBit;
    }
    else
    {
        m_desc &= ~ERasterDescMultiSampleBit;
    }
}

void CRasterStateDesc::enableAntiAliasedLine( const bool enable )
{
    if( enable )
    {
        m_desc |= ERasterDescAntiAliasedBit;
    }
    else
    {
        m_desc &= ~ERasterDescAntiAliasedBit;
    }
}

bool CRasterStateDesc::frontIsAntiClockwise() const
{
    return( ( m_desc & ERasterDescAntiClockwiseBit ) ? true : false );
}

bool CRasterStateDesc::scissorEnabled() const
{
    return( ( m_desc & ERasterDescScissorBit ) ? true : false );
}

bool CRasterStateDesc::wireframeEnabled() const
{
    return( ( m_desc & ERasterDescWireframeBit ) ? true : false );
}

bool CRasterStateDesc::depthClipEnabled() const
{
    return( ( m_desc & ERasterDescDepthClipBit ) ? true : false );
}

bool CRasterStateDesc::multiSampleEnabled() const
{
    return( ( m_desc & ERasterDescMultiSampleBit ) ? true : false );
}

bool CRasterStateDesc::antiAliasedLineEnabled() const
{
    return( ( m_desc & ERasterDescAntiAliasedBit ) ? true : false );
}

bool CRasterStateDesc::hasBiasing() const
{
    return( ( m_desc & ERasterDescBiasEnabledBit ) ? true : false );
}

void CRasterStateDesc::clearBiasing()
{
    m_desc &= ~( ERasterDescBiasEnabledBit | ERasterDescDepthBiasMask );
    m_biasSlope = 0.0f;
    m_biasClamp = 0.0f;
}

int CRasterStateDesc::compare( const CRasterStateDesc& desc ) const
{
    return( memcmp( reinterpret_cast< const void* >( this ), reinterpret_cast< const void* >( &desc ), sizeof( desc ) ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DEPTH_STATE_DEF depth state definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DEPTH_STATE_DEF::setDefault()
{
    EnableReads = EnableWrites = TRUE;
    DepthCmpFunc = ECmpFuncLess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    STENCIL_FACE_STATE_DEF stencil face state definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void STENCIL_FACE_STATE_DEF::setDefault()
{
    CmpFunc = ECmpFuncAlways;
    PassBoth = PassStencil = FailStencil = EStencilOpKeep;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    STENCIL_STATE_DEF stencil state definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void STENCIL_STATE_DEF::setDefault()
{
    EnableStencil = FALSE;
    FrontFace.setDefault();
    BackFace.setDefault();
    StencilReadMask = StencilWriteMask = 0x000000ff;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DEPTH_STENCIL_STATE_DEF depth-stencil state definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DEPTH_STENCIL_STATE_DEF::setDefault()
{
    DepthDef.setDefault();
    StencilDef.setDefault();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CDepthStateDesc depth-stencil state description class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDepthStateDesc::setDefault()
{
    m_desc = EDepthDescDepthEnableBits;
    m_desc |= ( ( static_cast< uint32_t >( ECmpFuncLess   ) <<     EDepthDescDepthCmpFuncShift ) &     EDepthDescDepthCmpFuncMask );
    m_desc |= ( ( static_cast< uint32_t >( ECmpFuncAlways ) <<     EDepthDescFrontCmpFuncShift ) &     EDepthDescFrontCmpFuncMask );
    m_desc |= ( ( static_cast< uint32_t >( ECmpFuncAlways ) <<      EDepthDescBackCmpFuncShift ) &      EDepthDescBackCmpFuncMask );
    m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) <<    EDepthDescFrontPassBothShift ) &    EDepthDescFrontPassBothMask );
    m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) <<     EDepthDescBackPassBothShift ) &     EDepthDescBackPassBothMask );
    m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) << EDepthDescFrontPassStencilShift ) & EDepthDescFrontPassStencilMask );
    m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) <<  EDepthDescBackPassStencilShift ) &  EDepthDescBackPassStencilMask );
    m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) << EDepthDescFrontFailStencilShift ) & EDepthDescFrontFailStencilMask );
    m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) <<  EDepthDescBackFailStencilShift ) &  EDepthDescBackFailStencilMask );
    m_masks = 0x0000ffff;
}

void CDepthStateDesc::set( const CDepthStateDesc& desc )
{
    m_desc = desc.m_desc;
    m_masks = desc.m_masks;
}

void CDepthStateDesc::set( const DEPTH_STATE_DEF& def )
{
    setDefault();
    if( def.EnableReads )
    {
        setDepthTest( def.DepthCmpFunc );
    }
    else
    {
        disableDepthReads();
    }
    enableDepthWrites( def.EnableWrites ? true : false );
}

void CDepthStateDesc::set( const STENCIL_STATE_DEF& def )
{
    if( def.EnableStencil )
    {
        setFrontStencil( def.FrontFace.CmpFunc, def.FrontFace.PassBoth, def.FrontFace.PassStencil, def.FrontFace.FailStencil );
        setBackStencil( def.BackFace.CmpFunc, def.BackFace.PassBoth, def.BackFace.PassStencil, def.BackFace.FailStencil );
        setStencilMasks( static_cast< uint8_t >( def.StencilReadMask ), static_cast< uint8_t >( def.StencilWriteMask ) );
    }
    else
    {
        disableStencil();
    }
}

void CDepthStateDesc::set( const DEPTH_STENCIL_STATE_DEF& def )
{
    set( def.DepthDef );
    set( def.StencilDef );
}

void CDepthStateDesc::setDepthTest( const ECmpFunc cmpFunc )
{
    m_desc &= ~EDepthDescDepthCmpFuncMask;
    m_desc |= EDepthDescDepthReadEnableBit;
    m_desc |= ( ( static_cast< uint32_t >( cmpFunc ) << EDepthDescDepthCmpFuncShift ) & EDepthDescDepthCmpFuncMask );
}

void CDepthStateDesc::setDepthTest( const ECmpFunc cmpFunc, const bool write )
{
    m_desc &= ~( EDepthDescDepthEnableBits | EDepthDescDepthCmpFuncMask );
    m_desc |= ( write ? EDepthDescDepthEnableBits : EDepthDescDepthReadEnableBit );
    m_desc |= ( ( static_cast< uint32_t >( cmpFunc ) << EDepthDescDepthCmpFuncShift ) & EDepthDescDepthCmpFuncMask );
}

void CDepthStateDesc::setStencil( const ECmpFunc cmpFunc, const EStencilOp passBoth, const EStencilOp passStencil, const EStencilOp failStencil )
{
    m_desc &= ~( EDepthDescFrontMask | EDepthDescBackMask );
    m_desc |= EDepthDescStencilEnableBit;
    m_desc |= ( ( static_cast< uint32_t >( cmpFunc     ) <<     EDepthDescFrontCmpFuncShift ) &     EDepthDescFrontCmpFuncMask );
    m_desc |= ( ( static_cast< uint32_t >( cmpFunc     ) <<      EDepthDescBackCmpFuncShift ) &      EDepthDescBackCmpFuncMask );
    m_desc |= ( ( static_cast< uint32_t >( passBoth    ) <<    EDepthDescFrontPassBothShift ) &    EDepthDescFrontPassBothMask );
    m_desc |= ( ( static_cast< uint32_t >( passBoth    ) <<     EDepthDescBackPassBothShift ) &     EDepthDescBackPassBothMask );
    m_desc |= ( ( static_cast< uint32_t >( passStencil ) << EDepthDescFrontPassStencilShift ) & EDepthDescFrontPassStencilMask );
    m_desc |= ( ( static_cast< uint32_t >( passStencil ) <<  EDepthDescBackPassStencilShift ) &  EDepthDescBackPassStencilMask );
    m_desc |= ( ( static_cast< uint32_t >( failStencil ) << EDepthDescFrontFailStencilShift ) & EDepthDescFrontFailStencilMask );
    m_desc |= ( ( static_cast< uint32_t >( failStencil ) <<  EDepthDescBackFailStencilShift ) &  EDepthDescBackFailStencilMask );
}

void CDepthStateDesc::setFrontStencil( const ECmpFunc cmpFunc, const EStencilOp passBoth, const EStencilOp passStencil, const EStencilOp failStencil )
{
    m_desc &= ~EDepthDescFrontMask;
    m_desc |= EDepthDescStencilEnableBit;
    m_desc |= ( ( static_cast< uint32_t >( cmpFunc     ) <<     EDepthDescFrontCmpFuncShift ) &     EDepthDescFrontCmpFuncMask );
    m_desc |= ( ( static_cast< uint32_t >( passBoth    ) <<    EDepthDescFrontPassBothShift ) &    EDepthDescFrontPassBothMask );
    m_desc |= ( ( static_cast< uint32_t >( passStencil ) << EDepthDescFrontPassStencilShift ) & EDepthDescFrontPassStencilMask );
    m_desc |= ( ( static_cast< uint32_t >( failStencil ) << EDepthDescFrontFailStencilShift ) & EDepthDescFrontFailStencilMask );
}

void CDepthStateDesc::setBackStencil( const ECmpFunc cmpFunc, const EStencilOp passBoth, const EStencilOp passStencil, const EStencilOp failStencil )
{
    m_desc &= ~EDepthDescBackMask;
    m_desc |= EDepthDescStencilEnableBit;
    m_desc |= ( ( static_cast< uint32_t >( cmpFunc     ) <<     EDepthDescBackCmpFuncShift ) &     EDepthDescBackCmpFuncMask );
    m_desc |= ( ( static_cast< uint32_t >( passBoth    ) <<    EDepthDescBackPassBothShift ) &    EDepthDescBackPassBothMask );
    m_desc |= ( ( static_cast< uint32_t >( passStencil ) << EDepthDescBackPassStencilShift ) & EDepthDescBackPassStencilMask );
    m_desc |= ( ( static_cast< uint32_t >( failStencil ) << EDepthDescBackFailStencilShift ) & EDepthDescBackFailStencilMask );
}

void CDepthStateDesc::setStencilMasks( const uint8_t read, const uint8_t write )
{
    m_masks = ( static_cast< uint32_t >( read ) | ( static_cast< uint32_t >( write ) << 8 ) );
}

bool CDepthStateDesc::getDepthTest( ECmpFunc& cmpFunc ) const
{
    if( m_desc & EDepthDescDepthReadEnableBit )
    {
        cmpFunc = static_cast< ECmpFunc >( ( m_desc & EDepthDescDepthCmpFuncMask ) >> EDepthDescDepthCmpFuncShift );
        return( true );
    }
    else
    {
        cmpFunc = ECmpFuncAlways;
        return( false );
    }
}

ECmpFunc CDepthStateDesc::getDepthTest() const
{
    ECmpFunc cmpFunc;
    getDepthTest( cmpFunc );
    return( cmpFunc );
}

bool CDepthStateDesc::getFrontStencil( ECmpFunc& cmpFunc, EStencilOp& passBoth, EStencilOp& passStencil, EStencilOp& failStencil ) const
{
    if( m_desc & EDepthDescStencilEnableBit )
    {
        cmpFunc = static_cast< ECmpFunc >( ( m_desc & EDepthDescFrontCmpFuncMask ) >> EDepthDescFrontCmpFuncShift );
        passBoth = static_cast< EStencilOp >( ( m_desc & EDepthDescFrontPassBothMask ) >> EDepthDescFrontPassBothShift );
        passStencil = static_cast< EStencilOp >( ( m_desc & EDepthDescFrontPassStencilMask ) >> EDepthDescFrontPassStencilShift );
        failStencil = static_cast< EStencilOp >( ( m_desc & EDepthDescFrontFailStencilMask ) >> EDepthDescFrontFailStencilShift );
        return( true );
    }
    else
    {
        cmpFunc = ECmpFuncAlways;
        passBoth = EStencilOpKeep;
        passStencil = EStencilOpKeep;
        failStencil = EStencilOpKeep;
        return( false );
    }
}

bool CDepthStateDesc::getBackStencil( ECmpFunc& cmpFunc, EStencilOp& passBoth, EStencilOp& passStencil, EStencilOp& failStencil ) const
{
    if( m_desc & EDepthDescStencilEnableBit )
    {
        cmpFunc = static_cast< ECmpFunc >( ( m_desc & EDepthDescBackCmpFuncMask ) >> EDepthDescBackCmpFuncShift );
        passBoth = static_cast< EStencilOp >( ( m_desc & EDepthDescBackPassBothMask ) >> EDepthDescBackPassBothShift );
        passStencil = static_cast< EStencilOp >( ( m_desc & EDepthDescBackPassStencilMask ) >> EDepthDescBackPassStencilShift );
        failStencil = static_cast< EStencilOp >( ( m_desc & EDepthDescBackFailStencilMask ) >> EDepthDescBackFailStencilShift );
        return( true );
    }
    else
    {
        cmpFunc = ECmpFuncAlways;
        passBoth = EStencilOpKeep;
        passStencil = EStencilOpKeep;
        failStencil = EStencilOpKeep;
        return( false );
    }
}

void CDepthStateDesc::getStencilMasks( uint8_t& read, uint8_t& write ) const
{
    read  = static_cast< uint8_t >(   m_masks & 0x000000ff );
    write = static_cast< uint8_t >( ( m_masks & 0x0000ff00 ) >> 8 );
}

void CDepthStateDesc::enableDepth( const bool read, const bool write )
{
    if( read )
    {
        m_desc |= EDepthDescDepthReadEnableBit;
    }
    else
    {
        m_desc &= ~( EDepthDescDepthReadEnableBit | EDepthDescDepthCmpFuncMask );
        m_desc |= ( ( static_cast< uint32_t >( ECmpFuncAlways ) << EDepthDescDepthCmpFuncShift ) & EDepthDescDepthCmpFuncMask );
    }
    if( write )
    {
        m_desc |= EDepthDescDepthWriteEnableBit;
    }
    else
    {
        m_desc &= ~EDepthDescDepthWriteEnableBit;
    }
}

void CDepthStateDesc::enableDepthReads( const bool enable )
{
    if( enable )
    {
        m_desc |= EDepthDescDepthReadEnableBit;
    }
    else
    {
        m_desc &= ~( EDepthDescDepthReadEnableBit | EDepthDescDepthCmpFuncMask );
        m_desc |= ( ( static_cast< uint32_t >( ECmpFuncAlways ) << EDepthDescDepthCmpFuncShift ) & EDepthDescDepthCmpFuncMask );
    }
}

void CDepthStateDesc::enableDepthWrites( const bool enable )
{
    if( enable )
    {
        m_desc |= EDepthDescDepthWriteEnableBit;
    }
    else
    {
        m_desc &= ~EDepthDescDepthWriteEnableBit;
    }
}

void CDepthStateDesc::enableStencil( const bool enable )
{
    if( enable )
    {
        m_desc |= EDepthDescStencilEnableBit;
    }
    else
    {
        m_desc &= ~( EDepthDescStencilEnableBit | EDepthDescFrontMask | EDepthDescBackMask );
        m_desc |= ( ( static_cast< uint32_t >( ECmpFuncAlways ) <<     EDepthDescFrontCmpFuncShift ) &     EDepthDescFrontCmpFuncMask );
        m_desc |= ( ( static_cast< uint32_t >( ECmpFuncAlways ) <<      EDepthDescBackCmpFuncShift ) &      EDepthDescBackCmpFuncMask );
        m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) <<    EDepthDescFrontPassBothShift ) &    EDepthDescFrontPassBothMask );
        m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) <<     EDepthDescBackPassBothShift ) &     EDepthDescBackPassBothMask );
        m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) << EDepthDescFrontPassStencilShift ) & EDepthDescFrontPassStencilMask );
        m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) <<  EDepthDescBackPassStencilShift ) &  EDepthDescBackPassStencilMask );
        m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) << EDepthDescFrontFailStencilShift ) & EDepthDescFrontFailStencilMask );
        m_desc |= ( ( static_cast< uint32_t >( EStencilOpKeep ) <<  EDepthDescBackFailStencilShift ) &  EDepthDescBackFailStencilMask );
        m_masks = 0x0000ffff;
    }
}

bool CDepthStateDesc::depthEnabled() const
{
    return( ( m_desc & EDepthDescDepthEnableBits ) ? true : false );
}

bool CDepthStateDesc::depthReadsEnabled() const
{
    return( ( m_desc & EDepthDescDepthReadEnableBit ) ? true : false );
}

bool CDepthStateDesc::depthWritesEnabled() const
{
    return( ( m_desc & EDepthDescDepthWriteEnableBit ) ? true : false );
}

bool CDepthStateDesc::stencilEnabled() const
{
    return( ( m_desc & EDepthDescStencilEnableBit ) ? true : false );
}

int CDepthStateDesc::compare( const CDepthStateDesc& desc ) const
{
    return( ( m_desc == desc.m_desc ) ? ( ( m_masks == desc.m_masks ) ? 0 : ( ( m_masks < desc.m_masks ) ? -1  : 1 ) ) : ( ( m_desc < desc.m_desc ) ? -1  : 1 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BLEND_EFFECT_STATE_DEF blending configuration definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BLEND_EFFECT_STATE_DEF::setDefault()
{
    Enable =  FALSE;
    SourceMul =  EBlendMulOne;
    TargetMul =  EBlendMulZero;
    Combiner =  ECombinerAdd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BLEND_CONFIG_STATE_DEF blending configuration definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BLEND_CONFIG_STATE_DEF::setDefault()
{
    Colour.setDefault();
    Alpha.setDefault();
    Mask = 0x0000000f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    LOGIC_CONFIG_STATE_DEF logic configuration definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LOGIC_CONFIG_STATE_DEF::setDefault()
{
    EnableOp = FALSE;
    LogicOp = ELogicOpNop;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BLEND_TARGET_STATE_DEF render target blending definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BLEND_TARGET_STATE_DEF::setDefault()
{
    BlendConfig.setDefault();
    LogicConfig.setDefault();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BLEND_STATE_DEF blend state definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BLEND_STATE_DEF::setDefault()
{
    EnableAlphaToCoverage = FALSE;
    BlendTarget.setDefault();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    MULTI_BLEND_STATE_DEF blend state definition structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MULTI_BLEND_STATE_DEF::setDefault()
{
    EnableAlphaToCoverage = false;
    RenderTargetCount = 1;
    BlendTargets[ 0 ].setDefault();
    BlendTargets[ 1 ].setDefault();
    BlendTargets[ 2 ].setDefault();
    BlendTargets[ 3 ].setDefault();
    BlendTargets[ 4 ].setDefault();
    BlendTargets[ 5 ].setDefault();
    BlendTargets[ 6 ].setDefault();
    BlendTargets[ 7 ].setDefault();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBlendStateConfig blend configuration class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBlendStateConfig::setDefault()
{
    m_config = 0;
    m_config |= ( ( static_cast< uint32_t >( EBlendMulOne  ) << EBlendConfigSourceMulShiftRGB ) & EBlendConfigSourceMulMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( EBlendMulZero ) << EBlendConfigTargetMulShiftRGB ) & EBlendConfigTargetMulMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( ECombinerAdd  ) <<  EBlendConfigCombinerShiftRGB ) &  EBlendConfigCombinerMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( EBlendMulOne  ) <<   EBlendConfigSourceMulShiftA ) &   EBlendConfigSourceMulMaskA );
    m_config |= ( ( static_cast< uint32_t >( EBlendMulZero ) <<   EBlendConfigTargetMulShiftA ) &   EBlendConfigTargetMulMaskA );
    m_config |= ( ( static_cast< uint32_t >( ECombinerAdd  ) <<    EBlendConfigCombinerShiftA ) &    EBlendConfigCombinerMaskA );
    m_config |= ( ( static_cast< uint32_t >( 0x0f          ) <<         EBlendConfigMaskShift ) &         EBlendConfigMaskMask );
}

void CBlendStateConfig::set( const CBlendStateConfig& config )
{
    m_config = config.m_config;
}

void CBlendStateConfig::set( const BLEND_CONFIG_STATE_DEF& def )
{
    setDefault();
    if( def.Colour.Enable )
    {
        setColourBlend( def.Colour.SourceMul, def.Colour.TargetMul, def.Colour.Combiner );
    }
    if( def.Alpha.Enable )
    {
        setAlphaBlend( def.Alpha.SourceMul, def.Alpha.TargetMul, def.Alpha.Combiner );
    }
    setMask( static_cast< uint8_t >( def.Mask ) );
}

void CBlendStateConfig::setBlend( const EBlendMul sourceMul, const EBlendMul targetMul, const ECombiner combiner )
{
    m_config &= ~( EBlendConfigMaskRGB | EBlendConfigMaskA );
    m_config |= EBlendConfigEnableBits;
    m_config |= ( ( static_cast< uint32_t >( sourceMul ) << EBlendConfigSourceMulShiftRGB ) & EBlendConfigSourceMulMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( targetMul ) << EBlendConfigTargetMulShiftRGB ) & EBlendConfigTargetMulMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( combiner  ) <<  EBlendConfigCombinerShiftRGB ) &  EBlendConfigCombinerMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( sourceMul ) <<   EBlendConfigSourceMulShiftA ) &   EBlendConfigSourceMulMaskA );
    m_config |= ( ( static_cast< uint32_t >( targetMul ) <<   EBlendConfigTargetMulShiftA ) &   EBlendConfigTargetMulMaskA );
    m_config |= ( ( static_cast< uint32_t >( combiner  ) <<    EBlendConfigCombinerShiftA ) &    EBlendConfigCombinerMaskA );
}

void CBlendStateConfig::setColourBlend( const EBlendMul sourceMul, const EBlendMul targetMul, const ECombiner combiner )
{
    m_config &= ~EBlendConfigMaskRGB;
    m_config |= EBlendConfigEnableBitRGB;
    m_config |= ( ( static_cast< uint32_t >( sourceMul ) << EBlendConfigSourceMulShiftRGB ) & EBlendConfigSourceMulMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( targetMul ) << EBlendConfigTargetMulShiftRGB ) & EBlendConfigTargetMulMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( combiner  ) <<  EBlendConfigCombinerShiftRGB ) &  EBlendConfigCombinerMaskRGB );
}

void CBlendStateConfig::setAlphaBlend( const EBlendMul sourceMul, const EBlendMul targetMul, const ECombiner combiner )
{
    m_config &= ~EBlendConfigMaskA;
    m_config |= EBlendConfigEnableBitA;
    m_config |= ( ( static_cast< uint32_t >( sourceMul ) << EBlendConfigSourceMulShiftA ) & EBlendConfigSourceMulMaskA );
    m_config |= ( ( static_cast< uint32_t >( targetMul ) << EBlendConfigTargetMulShiftA ) & EBlendConfigTargetMulMaskA );
    m_config |= ( ( static_cast< uint32_t >( combiner  ) <<  EBlendConfigCombinerShiftA ) &  EBlendConfigCombinerMaskA );
}

void CBlendStateConfig::setMask( const uint8_t mask )
{
    m_config &= ~EBlendConfigMaskMask;
    m_config |= ( ( static_cast< uint32_t >( mask ) << EBlendConfigMaskShift ) & EBlendConfigMaskMask );
}

bool CBlendStateConfig::getColourBlend( EBlendMul& sourceMul, EBlendMul& targetMul, ECombiner& combiner ) const
{
    if( m_config & EBlendConfigEnableBitRGB )
    {
        sourceMul = static_cast< EBlendMul >( ( m_config & EBlendConfigSourceMulMaskRGB ) >> EBlendConfigSourceMulShiftRGB );
        targetMul = static_cast< EBlendMul >( ( m_config & EBlendConfigTargetMulMaskRGB ) >> EBlendConfigTargetMulShiftRGB );
        combiner  = static_cast< ECombiner >( ( m_config &  EBlendConfigCombinerMaskRGB ) >>  EBlendConfigCombinerShiftRGB );
        return( true );
    }
    else
    {
        sourceMul = EBlendMulOne;
        targetMul = EBlendMulZero;
        combiner  = ECombinerAdd;
        return( false );
    }
}

bool CBlendStateConfig::getAlphaBlend( EBlendMul& sourceMul, EBlendMul& targetMul, ECombiner& combiner ) const
{
    if( m_config & EBlendConfigEnableBitA )
    {
        sourceMul = static_cast< EBlendMul >( ( m_config & EBlendConfigSourceMulMaskA ) >> EBlendConfigSourceMulShiftA );
        targetMul = static_cast< EBlendMul >( ( m_config & EBlendConfigTargetMulMaskA ) >> EBlendConfigTargetMulShiftA );
        combiner  = static_cast< ECombiner >( ( m_config &  EBlendConfigCombinerMaskA ) >>  EBlendConfigCombinerShiftA );
        return( true );
    }
    else
    {
        sourceMul = EBlendMulOne;
        targetMul = EBlendMulZero;
        combiner  = ECombinerAdd;
        return( false );
    }
}

uint8_t CBlendStateConfig::getMask() const
{
    return( static_cast< uint8_t >( ( m_config & EBlendConfigMaskMask ) >> EBlendConfigMaskShift ) );
}

bool CBlendStateConfig::blendEnabled() const
{
    return( ( m_config & EBlendConfigEnableBits ) ? true : false );
}

bool CBlendStateConfig::hasColourBlend() const
{
    return( ( m_config & EBlendConfigEnableBitRGB ) ? true : false );
}

bool CBlendStateConfig::hasAlphaBlend() const
{
    return( ( m_config & EBlendConfigEnableBitA ) ? true : false );
}

void CBlendStateConfig::clearColourBlend()
{
    m_config &= ~( EBlendConfigEnableBitRGB | EBlendConfigMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( EBlendMulOne  ) << EBlendConfigSourceMulShiftRGB ) & EBlendConfigSourceMulMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( EBlendMulZero ) << EBlendConfigTargetMulShiftRGB ) & EBlendConfigTargetMulMaskRGB );
    m_config |= ( ( static_cast< uint32_t >( ECombinerAdd  ) <<  EBlendConfigCombinerShiftRGB ) &  EBlendConfigCombinerMaskRGB );
}

void CBlendStateConfig::clearAlphaBlend()
{
    m_config &= ~( EBlendConfigEnableBitA | EBlendConfigMaskA );
    m_config |= ( ( static_cast< uint32_t >( EBlendMulOne  ) << EBlendConfigSourceMulShiftA ) & EBlendConfigSourceMulMaskA );
    m_config |= ( ( static_cast< uint32_t >( EBlendMulZero ) << EBlendConfigTargetMulShiftA ) & EBlendConfigTargetMulMaskA );
    m_config |= ( ( static_cast< uint32_t >( ECombinerAdd  ) <<  EBlendConfigCombinerShiftA ) &  EBlendConfigCombinerMaskA );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBlendStateDesc blend state description class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBlendStateDesc::setDefault()
{
    m_desc = 0;
    m_config.setDefault();
}

void CBlendStateDesc::set( const CBlendStateDesc& desc )
{
    m_desc = desc.m_desc;
    m_config.set( desc.m_config );
}

void CBlendStateDesc::set( const CBlendStateConfig& config )
{
    setDefault();
    m_config.set( config );
}

void CBlendStateDesc::set( const BLEND_STATE_DEF& def )
{
    setDefault();
    if( def.EnableAlphaToCoverage )
    {
        enableAlphaToCoverage();
    }
    m_config.set( def.BlendTarget.BlendConfig );
    if( def.BlendTarget.LogicConfig.EnableOp )
    {
        setLogic( def.BlendTarget.LogicConfig.LogicOp );
    }
}

void CBlendStateDesc::set( const BLEND_CONFIG_STATE_DEF& configDef )
{
    setDefault();
    m_config.set( configDef );
}

void CBlendStateDesc::set( const BLEND_TARGET_STATE_DEF& targetDef )
{
    setDefault();
    m_config.set( targetDef.BlendConfig );
    if( targetDef.LogicConfig.EnableOp )
    {
        setLogic( targetDef.LogicConfig.LogicOp );
    }
}

void CBlendStateDesc::enableAlphaToCoverage( const bool enable )
{
    if( enable )
    {
        m_desc |= EBlendDescA2CBit;
    }
    else
    {
        m_desc &= ~EBlendDescA2CBit;
    }
}

bool CBlendStateDesc::alphaToCoverageEnabled() const
{
    return( ( m_desc & EBlendDescA2CBit ) ? true : false );
}

void CBlendStateDesc::setLogic( const ELogicOp op )
{
    m_desc &= ~EBlendDescLogicOpMask;
    m_desc |= ( 1 << EBlendDescLogicEnableShift );
    m_desc |= ( ( static_cast< uint32_t >( op ) << EBlendDescLogicOpShift ) & EBlendDescLogicOpMask );
}

bool CBlendStateDesc::getLogic( ELogicOp& op ) const
{
    if( m_desc & ( 1 << EBlendDescLogicEnableShift ) )
    {
        op = static_cast< ELogicOp >( ( m_desc & EBlendDescLogicOpMask ) >> EBlendDescLogicOpShift );
        return( true );
    }
    else
    {
        op = ELogicOpNop;
        return( false );
    }
}

ELogicOp CBlendStateDesc::getLogic() const
{
    ELogicOp op;
    getLogic( op );
    return( op );
}

bool CBlendStateDesc::logicEnabled() const
{
    return( ( m_desc & ( 1 << EBlendDescLogicEnableShift ) ) ? true : false );
}

void CBlendStateDesc::clearLogic()
{
    m_desc &= ~( ( 1 << EBlendDescLogicEnableShift ) | EBlendDescLogicOpMask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CMultiBlendStateDesc blend state description class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMultiBlendStateDesc::setDefault()
{
    m_desc = 0;
    m_configs[ 0 ].setDefault();
    m_configs[ 1 ].setDefault();
    m_configs[ 2 ].setDefault();
    m_configs[ 3 ].setDefault();
    m_configs[ 4 ].setDefault();
    m_configs[ 5 ].setDefault();
    m_configs[ 6 ].setDefault();
    m_configs[ 7 ].setDefault();
    m_logicOps = 0;
}

void CMultiBlendStateDesc::set( const CBlendStateDesc& desc )
{
    setDefault();
    if( desc.alphaToCoverageEnabled() )
    {
        enableAlphaToCoverage();
    }
    m_configs[ 0 ].set( desc.getConfig() );
    if( desc.logicEnabled() )
    {
        setLogic( desc.getLogic() );
    }
}

void CMultiBlendStateDesc::set( const CMultiBlendStateDesc& desc )
{
    m_desc = desc.m_desc;
    m_configs[ 0 ].set( desc.m_configs[ 0 ] );
    m_configs[ 1 ].set( desc.m_configs[ 1 ] );
    m_configs[ 2 ].set( desc.m_configs[ 2 ] );
    m_configs[ 3 ].set( desc.m_configs[ 3 ] );
    m_configs[ 4 ].set( desc.m_configs[ 4 ] );
    m_configs[ 5 ].set( desc.m_configs[ 5 ] );
    m_configs[ 6 ].set( desc.m_configs[ 6 ] );
    m_configs[ 7 ].set( desc.m_configs[ 7 ] );
    m_logicOps = desc.m_logicOps;
}

void CMultiBlendStateDesc::set( const CBlendStateConfig& config )
{
    setDefault();
    m_configs[ 0 ].set( config );
}

void CMultiBlendStateDesc::set( const BLEND_STATE_DEF& def )
{
    setDefault();
    if( def.EnableAlphaToCoverage )
    {
        enableAlphaToCoverage();
    }
    m_configs[ 0 ].set( def.BlendTarget.BlendConfig );
    if( def.BlendTarget.LogicConfig.EnableOp )
    {
        setLogic( def.BlendTarget.LogicConfig.LogicOp );
    }
}

void CMultiBlendStateDesc::set( const MULTI_BLEND_STATE_DEF& def )
{
    setDefault();
    if( def.EnableAlphaToCoverage )
    {
        enableAlphaToCoverage();
    }
    uint count = def.RenderTargetCount;
    setTargetCount( count );
    for( uint index = 0; index < count; ++index )
    {
        m_configs[ index ].set( def.BlendTargets[ index ].BlendConfig );
        if( def.BlendTargets[ index ].LogicConfig.EnableOp )
        {
            setLogic( index, def.BlendTargets[ index ].LogicConfig.LogicOp );
        }
    }
}

void CMultiBlendStateDesc::set( const BLEND_CONFIG_STATE_DEF& configDef )
{
    setDefault();
    m_configs[ 0 ].set( configDef );
}

void CMultiBlendStateDesc::set( const BLEND_TARGET_STATE_DEF& targetDef )
{
    setDefault();
    m_configs[ 0 ].set( targetDef.BlendConfig );
    if( targetDef.LogicConfig.EnableOp )
    {
        setLogic( targetDef.LogicConfig.LogicOp );
    }
}

void CMultiBlendStateDesc::setTargetCount( const uint count )
{
    m_desc &= ~EBlendDescTargetCountMask;
    m_desc |= ( ( ( count < 1 ) ? 0 : ( ( count > 8 ) ? 7 : static_cast< uint32_t >( count - 1 ) ) ) << EBlendDescTargetCountShift );
}

uint CMultiBlendStateDesc::getTargetCount() const
{
    return( static_cast< uint >( ( m_desc & EBlendDescTargetCountMask ) >> EBlendDescTargetCountShift ) + 1 );
}

void CMultiBlendStateDesc::enableAlphaToCoverage( const bool enable )
{
    if( enable )
    {
        m_desc |= EBlendDescA2CBit;
    }
    else
    {
        m_desc &= ~EBlendDescA2CBit;
    }
}

bool CMultiBlendStateDesc::alphaToCoverageEnabled() const
{
    return( ( m_desc & EBlendDescA2CBit ) ? true : false );
}

void CMultiBlendStateDesc::setLogic( const uint index, const ELogicOp op )
{
    uint id = ( index & 7 );
    uint shift = ( id << 2 );
    m_desc |= ( 1 << ( EBlendDescLogicEnableShift + id ) );
    m_logicOps &= ~( 15 << shift );
    m_logicOps |= ( ( static_cast< uint32_t >( op ) & 15 ) << shift );
    if( shift == 0 )
    {
        m_desc &= ~EBlendDescLogicOpMask;
        m_desc |= ( ( static_cast< uint32_t >( op ) << EBlendDescLogicOpShift ) & EBlendDescLogicOpMask );
    }
}

bool CMultiBlendStateDesc::getLogic( const uint index, ELogicOp& op ) const
{
    uint id = ( index & 7 );
    uint shift = ( id << 2 );
    if( m_desc & ( 1 << ( EBlendDescLogicEnableShift + id ) ) )
    {
        op = static_cast< ELogicOp >( ( m_logicOps >> shift ) & 15 );
        return( true );
    }
    else
    {
        op = ELogicOpNop;
        return( false );
    }
}

ELogicOp CMultiBlendStateDesc::getLogic( const uint index ) const
{
    ELogicOp op;
    getLogic( index, op );
    return( op );
}

bool CMultiBlendStateDesc::logicEnabled( const uint index ) const
{
    uint shift = ( index & 7 );
    return( ( m_desc & ( 1 << ( EBlendDescLogicEnableShift + shift ) ) ) ? true : false );
}

void CMultiBlendStateDesc::clearLogic( const uint index )
{
    uint id = ( index & 7 );
    uint shift = ( id << 2 );
    m_desc &= ~( 1 << ( EBlendDescLogicEnableShift + id ) );
    m_logicOps &= ~( 15 << shift );
    if( shift == 0 )
    {
        m_desc &= ~EBlendDescLogicOpMask;
    }
}

int CMultiBlendStateDesc::compare( const CMultiBlendStateDesc& desc ) const
{
    return( memcmp( reinterpret_cast< const void* >( this ), reinterpret_cast< const void* >( &desc ), sizeof( desc ) ) );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Default state configurations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSamplerDesc            DEFAULT_SAMPLER_DESC;
CRasterStateDesc        DEFAULT_RASTER_STATE_DESC;
CDepthStateDesc         DEFAULT_DEPTH_STATE_DESC;
CBlendStateDesc         DEFAULT_BLEND_STATE_DESC;
CMultiBlendStateDesc    DEFAULT_MULTIBLEND_STATE_DESC;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Standard depth state configuration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DEPTH_STATE_DEF         STANDARD_DEPTH_STATE_DEF = { TRUE, TRUE, ECmpFuncGreaterEqual };
CDepthStateDesc         STANDARD_DEPTH_STATE_DESC( STANDARD_DEPTH_STATE_DEF );

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
