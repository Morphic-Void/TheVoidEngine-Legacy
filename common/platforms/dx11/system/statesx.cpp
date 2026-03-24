
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   statesx.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        DirectX 11 state creation and caching classes.
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
#include "statesx.h"

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
////    begin D3D11_REMAP namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace D3D11_REMAP
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    D3D11_LOGIC_OP is missing on Windows 7
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef D3D11_LOGIC_OP
typedef enum D3D11_LOGIC_OP { 
  D3D11_LOGIC_OP_CLEAR          = 0,
  D3D11_LOGIC_OP_SET            = ( D3D11_LOGIC_OP_CLEAR + 1 ),
  D3D11_LOGIC_OP_COPY           = ( D3D11_LOGIC_OP_SET + 1 ),
  D3D11_LOGIC_OP_COPY_INVERTED  = ( D3D11_LOGIC_OP_COPY + 1 ),
  D3D11_LOGIC_OP_NOOP           = ( D3D11_LOGIC_OP_COPY_INVERTED + 1 ),
  D3D11_LOGIC_OP_INVERT         = ( D3D11_LOGIC_OP_NOOP + 1 ),
  D3D11_LOGIC_OP_AND            = ( D3D11_LOGIC_OP_INVERT + 1 ),
  D3D11_LOGIC_OP_NAND           = ( D3D11_LOGIC_OP_AND + 1 ),
  D3D11_LOGIC_OP_OR             = ( D3D11_LOGIC_OP_NAND + 1 ),
  D3D11_LOGIC_OP_NOR            = ( D3D11_LOGIC_OP_OR + 1 ),
  D3D11_LOGIC_OP_XOR            = ( D3D11_LOGIC_OP_NOR + 1 ),
  D3D11_LOGIC_OP_EQUIV          = ( D3D11_LOGIC_OP_XOR + 1 ),
  D3D11_LOGIC_OP_AND_REVERSE    = ( D3D11_LOGIC_OP_EQUIV + 1 ),
  D3D11_LOGIC_OP_AND_INVERTED   = ( D3D11_LOGIC_OP_AND_REVERSE + 1 ),
  D3D11_LOGIC_OP_OR_REVERSE     = ( D3D11_LOGIC_OP_AND_INVERTED + 1 ),
  D3D11_LOGIC_OP_OR_INVERTED    = ( D3D11_LOGIC_OP_OR_REVERSE + 1 )
} D3D11_LOGIC_OP;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    static remapping of enumerated values and bits to DirectX types
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const D3D11_COMPARISON_FUNC REMAP_ECmpFunc[] =
{
    D3D11_COMPARISON_NEVER,                                     //  ECmpFuncNever
    D3D11_COMPARISON_ALWAYS,                                    //  ECmpFuncAlways
    D3D11_COMPARISON_EQUAL,                                     //  ECmpFuncEqual
    D3D11_COMPARISON_NOT_EQUAL,                                 //  ECmpFuncNotEqual
    D3D11_COMPARISON_LESS,                                      //  ECmpFuncLess
    D3D11_COMPARISON_LESS_EQUAL,                                //  ECmpFuncLessEqual
    D3D11_COMPARISON_GREATER,                                   //  ECmpFuncGreater
    D3D11_COMPARISON_GREATER_EQUAL                              //  ECmpFuncGreaterEqual
};

static const D3D11_FILTER REMAP_Filtering[] =
{
                             D3D11_FILTER_MIN_MAG_MIP_POINT,    //  min point,  mag point,  mip point,  ESamplingFilter
                     D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT,    //  min point,  mag point,  mip point,  ESamplingMinimum
                     D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,    //  min point,  mag point,  mip point,  ESamplingMaximum
                  D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,    //  min point,  mag point,  mip point,  ESamplingCompare
                      D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,    //  min linear, mag point,  mip point,  ESamplingFilter
              D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,    //  min linear, mag point,  mip point,  ESamplingMinimum
              D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,    //  min linear, mag point,  mip point,  ESamplingMaximum
           D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,    //  min linear, mag point,  mip point,  ESamplingCompare
                D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,    //  min point,  mag linear, mip point,  ESamplingFilter
        D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,    //  min point,  mag linear, mip point,  ESamplingMinimum
        D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,    //  min point,  mag linear, mip point,  ESamplingMaximum
     D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,    //  min point,  mag linear, mip point,  ESamplingCompare
                      D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,    //  min linear, mag linear, mip point,  ESamplingFilter
              D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,    //  min linear, mag linear, mip point,  ESamplingMinimum
              D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,    //  min linear, mag linear, mip point,  ESamplingMaximum
           D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,    //  min linear, mag linear, mip point,  ESamplingCompare
              D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,    //  min point,  mag point,  mip linear, ESamplingFilter
              D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,    //  min point,  mag point,  mip linear, ESamplingMinimum
           D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,    //  min point,  mag point,  mip linear, ESamplingMaximum
                      D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,    //  min point,  mag point,  mip linear, ESamplingCompare
       D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,    //  min linear, mag point,  mip linear, ESamplingFilter
       D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,    //  min linear, mag point,  mip linear, ESamplingMinimum
    D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,    //  min linear, mag point,  mip linear, ESamplingMaximum
               D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,    //  min linear, mag point,  mip linear, ESamplingCompare
              D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,    //  min point,  mag linear, mip linear, ESamplingFilter
              D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,    //  min point,  mag linear, mip linear, ESamplingMinimum
                      D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,    //  min point,  mag linear, mip linear, ESamplingMaximum
           D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,    //  min point,  mag linear, mip linear, ESamplingCompare
                            D3D11_FILTER_MIN_MAG_MIP_LINEAR,    //  min linear, mag linear, mip linear, ESamplingFilter
                    D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,    //  min linear, mag linear, mip linear, ESamplingMinimum
                    D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,    //  min linear, mag linear, mip linear, ESamplingMaximum
                 D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,    //  min linear, mag linear, mip linear, ESamplingCompare
                                   D3D11_FILTER_ANISOTROPIC,    //  anisotropic, ESamplingFilter
                           D3D11_FILTER_MINIMUM_ANISOTROPIC,    //  anisotropic, ESamplingMinimum
                           D3D11_FILTER_MAXIMUM_ANISOTROPIC,    //  anisotropic, ESamplingMaximum
                        D3D11_FILTER_COMPARISON_ANISOTROPIC     //  anisotropic, ESamplingCompare
};

static const D3D11_TEXTURE_ADDRESS_MODE REMAP_EWrapMode[] =
{
    D3D11_TEXTURE_ADDRESS_CLAMP,                                //  EWrapModeClamp
    D3D11_TEXTURE_ADDRESS_WRAP,                                 //  EWrapModeRepeat
    D3D11_TEXTURE_ADDRESS_MIRROR,                               //  EWrapModeMirror
    D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,                          //  EWrapModeMirrorOnce
    D3D11_TEXTURE_ADDRESS_BORDER                                //  EWrapModeBorder
};

static const D3D11_STENCIL_OP REMAP_EStencilOp[] =
{
     D3D11_STENCIL_OP_KEEP,                                     //  EStencilOpKeep
     D3D11_STENCIL_OP_ZERO,                                     //  EStencilOpZero
     D3D11_STENCIL_OP_INVERT,                                   //  EStencilOpInv
     D3D11_STENCIL_OP_INCR,                                     //  EStencilOpInc
     D3D11_STENCIL_OP_DECR,                                     //  EStencilOpDec
     D3D11_STENCIL_OP_INCR_SAT,                                 //  EStencilOpIncSat
     D3D11_STENCIL_OP_DECR_SAT,                                 //  EStencilOpDecSat
     D3D11_STENCIL_OP_REPLACE                                   //  EStencilOpReplace
};

static const D3D11_CULL_MODE REMAP_ECullMode[] =
{
    D3D11_CULL_NONE,                                            //  ECullModeNone
    D3D11_CULL_BACK,                                            //  ECullModeBack
    D3D11_CULL_FRONT                                            //  ECullModeFront
};

static const UINT REMAP_ESamplesUAV[] =
{
    0,                                                          //  ESamplesUnforced
    1,                                                          //  ESamplesForce1
    2,                                                          //  ESamplesForce2
    4,                                                          //  ESamplesForce4
    8,                                                          //  ESamplesForce8
    16                                                          //  ESamplesForce16
};

static const D3D11_BLEND REMAP_EBlendMul[] =
{
    D3D11_BLEND_ZERO,                                           //  EBlendMulZero
    D3D11_BLEND_ONE,                                            //  EBlendMulOne
    D3D11_BLEND_BLEND_FACTOR,                                   //  EBlendMulFactor
    D3D11_BLEND_INV_BLEND_FACTOR,                               //  EBlendMulInvFactor
    D3D11_BLEND_SRC_ALPHA,                                      //  EBlendMulSrcA
    D3D11_BLEND_INV_SRC_ALPHA,                                  //  EBlendMulInvSrcA
    D3D11_BLEND_DEST_ALPHA,                                     //  EBlendMulTrgA
    D3D11_BLEND_INV_DEST_ALPHA,                                 //  EBlendMulInvTrgA
    D3D11_BLEND_SRC1_ALPHA,                                     //  EBlendMulAltA
    D3D11_BLEND_INV_SRC1_ALPHA,                                 //  EBlendMulInvAltA
    D3D11_BLEND_SRC_ALPHA_SAT,                                  //  EBlendMulSrcSatA
    D3D11_BLEND_SRC_COLOR,                                      //  EBlendMulSrcRGB
    D3D11_BLEND_INV_SRC_COLOR,                                  //  EBlendMulInvSrcRGB
    D3D11_BLEND_DEST_COLOR,                                     //  EBlendMulTrgRGB
    D3D11_BLEND_INV_DEST_COLOR,                                 //  EBlendMulInvTrgRGB
    D3D11_BLEND_SRC1_COLOR,                                     //  EBlendMulAltRGB
    D3D11_BLEND_INV_SRC1_COLOR                                  //  EBlendMulInvAltRGB
};

static const D3D11_BLEND_OP REMAP_ECombiner[] =
{
    D3D11_BLEND_OP_ADD,                                         //  ECombinerAdd
    D3D11_BLEND_OP_SUBTRACT,                                    //  ECombinerSub
    D3D11_BLEND_OP_REV_SUBTRACT,                                //  ECombinerRevSub
    D3D11_BLEND_OP_MIN,                                         //  ECombinerMin
    D3D11_BLEND_OP_MAX                                          //  ECombinerMax
};

static const D3D11_LOGIC_OP REMAP_ELogicOp[] =
{
    D3D11_LOGIC_OP_NOOP,                                        //  ELogicOpNop
    D3D11_LOGIC_OP_CLEAR,                                       //  ELogicOpZero
    D3D11_LOGIC_OP_SET,                                         //  ELogicOpSet
    D3D11_LOGIC_OP_COPY,                                        //  ELogicOpCopy
    D3D11_LOGIC_OP_COPY_INVERTED,                               //  ELogicOpCopyInv
    D3D11_LOGIC_OP_INVERT,                                      //  ELogicOpInv
    D3D11_LOGIC_OP_AND,                                         //  ELogicOpAnd
    D3D11_LOGIC_OP_NAND,                                        //  ELogicOpNand
    D3D11_LOGIC_OP_OR,                                          //  ELogicOpOr
    D3D11_LOGIC_OP_NOR,                                         //  ELogicOpNor
    D3D11_LOGIC_OP_XOR,                                         //  ELogicOpXor
    D3D11_LOGIC_OP_EQUIV,                                       //  ELogicOpEquiv
    D3D11_LOGIC_OP_AND_REVERSE,                                 //  ELogicOpAndRev
    D3D11_LOGIC_OP_AND_INVERTED,                                //  ELogicOpAndInv
    D3D11_LOGIC_OP_OR_REVERSE,                                  //  ELogicOpOrRev
    D3D11_LOGIC_OP_OR_INVERTED                                  //  ELogicOpOrInv
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    static functions to convert to DirectX state descs
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void SET_SAMPLER_DESC( const CSamplerDesc& desc, D3D11_SAMPLER_DESC& d3d11Desc )
{
    bool linearMin, linearMag, linearMip;
    desc.getLinear( linearMin, linearMag, linearMip );
    d3d11Desc.MaxAnisotropy = desc.getAnisotropic();
    uint filterIndex = static_cast< uint >( desc.getMode() );
    if( d3d11Desc.MaxAnisotropy )
    {
        filterIndex |= 0x00000020;
    }
    else
    {
        if( linearMin ) filterIndex |= 0x00000004;
        if( linearMag ) filterIndex |= 0x00000008;
        if( linearMip ) filterIndex |= 0x00000010;
    }
    d3d11Desc.Filter = REMAP_Filtering[ filterIndex ];
    EWrapMode wrapU, wrapV, wrapW;
    desc.getWrapping( wrapU, wrapV, wrapW );
    d3d11Desc.AddressU = REMAP_EWrapMode[ static_cast< int >( wrapU ) ];
    d3d11Desc.AddressV = REMAP_EWrapMode[ static_cast< int >( wrapV ) ];
    d3d11Desc.AddressW = REMAP_EWrapMode[ static_cast< int >( wrapW ) ];
    desc.getLodding( d3d11Desc.MinLOD, d3d11Desc.MaxLOD, d3d11Desc.MipLODBias );
    desc.getBorder( d3d11Desc.BorderColor[ 0 ], d3d11Desc.BorderColor[ 1 ], d3d11Desc.BorderColor[ 2 ], d3d11Desc.BorderColor[ 3 ] );
    d3d11Desc.ComparisonFunc = REMAP_ECmpFunc[ static_cast< int >( desc.getCmpFunc() ) ];
}

static void SET_RASTERIZER_DESC( const CRasterStateDesc& desc, D3D11_RASTERIZER_DESC& d3d11Desc )
{
    d3d11Desc.FillMode = ( desc.wireframeEnabled() ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID );
    d3d11Desc.CullMode = REMAP_ECullMode[ static_cast< int >( desc.getCulling() ) ];
    d3d11Desc.FrontCounterClockwise = ( desc.frontIsAntiClockwise() ? TRUE : FALSE );
    desc.getBiasing( d3d11Desc.DepthBias, d3d11Desc.SlopeScaledDepthBias, d3d11Desc.DepthBiasClamp );
    d3d11Desc.DepthClipEnable = ( desc.depthClipEnabled() ? TRUE : FALSE );
    d3d11Desc.ScissorEnable = ( desc.scissorEnabled() ? TRUE : FALSE );
    d3d11Desc.MultisampleEnable = ( desc.multiSampleEnabled() ? TRUE : FALSE );
    d3d11Desc.AntialiasedLineEnable = ( desc.antiAliasedLineEnabled() ? TRUE : FALSE );
}

static void SET_DEPTH_STENCIL_DESC( const CDepthStateDesc& desc, D3D11_DEPTH_STENCIL_DESC& d3d11Desc )
{
    ECmpFunc cmpFunc;
    EStencilOp passBoth, passStencil, failStencil;
    d3d11Desc.DepthEnable = ( desc.depthEnabled() ? TRUE : FALSE );
    d3d11Desc.DepthWriteMask = ( desc.depthWritesEnabled() ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO );
    desc.getDepthTest( cmpFunc );
    d3d11Desc.DepthFunc = REMAP_ECmpFunc[ static_cast< int >( cmpFunc ) ];
    d3d11Desc.StencilEnable = ( desc.stencilEnabled() ? TRUE : FALSE );
    desc.getStencilMasks( d3d11Desc.StencilReadMask, d3d11Desc.StencilWriteMask );
    desc.getFrontStencil( cmpFunc, passBoth, passStencil, failStencil );
    d3d11Desc.FrontFace.StencilPassOp = REMAP_EStencilOp[ static_cast< int >( passBoth ) ];
    d3d11Desc.FrontFace.StencilDepthFailOp = REMAP_EStencilOp[ static_cast< int >( passStencil ) ];
    d3d11Desc.FrontFace.StencilFailOp = REMAP_EStencilOp[ static_cast< int >( failStencil ) ];
    d3d11Desc.FrontFace.StencilFunc = REMAP_ECmpFunc[ static_cast< int >( cmpFunc ) ];
    desc.getBackStencil( cmpFunc, passBoth, passStencil, failStencil );
    d3d11Desc.BackFace.StencilPassOp = REMAP_EStencilOp[ static_cast< int >( passBoth ) ];
    d3d11Desc.BackFace.StencilDepthFailOp = REMAP_EStencilOp[ static_cast< int >( passStencil ) ];
    d3d11Desc.BackFace.StencilFailOp = REMAP_EStencilOp[ static_cast< int >( failStencil ) ];
    d3d11Desc.BackFace.StencilFunc = REMAP_ECmpFunc[ static_cast< int >( cmpFunc ) ];
}

static void SET_BLEND_DESC_DEFAULTS( D3D11_BLEND_DESC& d3d11Desc )
{
    d3d11Desc.AlphaToCoverageEnable = FALSE;
    d3d11Desc.IndependentBlendEnable = FALSE;
    D3D11_RENDER_TARGET_BLEND_DESC& RenderTarget = d3d11Desc.RenderTarget[ 0 ];
    RenderTarget.BlendEnable = FALSE;
    RenderTarget.SrcBlend = D3D11_BLEND_ONE;
    RenderTarget.DestBlend = D3D11_BLEND_ZERO;
    RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
    RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
    RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
    RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    for( uint index = 1; index < 8; ++index )
    {
        d3d11Desc.RenderTarget[ index ] = RenderTarget;
    }
}

static void SET_BLEND_DESC( const CBlendStateConfig& config, D3D11_BLEND_DESC& d3d11Desc )
{
    SET_BLEND_DESC_DEFAULTS( d3d11Desc );
    d3d11Desc.AlphaToCoverageEnable = FALSE;
    d3d11Desc.IndependentBlendEnable = FALSE;
    D3D11_RENDER_TARGET_BLEND_DESC& RenderTarget = d3d11Desc.RenderTarget[ 0 ];
    EBlendMul sourceMul, targetMul;
    ECombiner combiner;
    RenderTarget.BlendEnable = ( config.blendEnabled() ? TRUE : FALSE );
    config.getColourBlend( sourceMul, targetMul, combiner );
    RenderTarget.SrcBlend = REMAP_EBlendMul[ static_cast< int >( sourceMul ) ];
    RenderTarget.DestBlend = REMAP_EBlendMul[ static_cast< int >( targetMul ) ];
    RenderTarget.BlendOp = REMAP_ECombiner[ static_cast< int >( combiner ) ];
    config.getAlphaBlend( sourceMul, targetMul, combiner );
    RenderTarget.SrcBlendAlpha = REMAP_EBlendMul[ static_cast< int >( sourceMul ) ];
    RenderTarget.DestBlendAlpha = REMAP_EBlendMul[ static_cast< int >( targetMul ) ];
    RenderTarget.BlendOpAlpha = REMAP_ECombiner[ static_cast< int >( combiner ) ];
    RenderTarget.RenderTargetWriteMask = config.getMask();
}

static void SET_BLEND_DESC( const CBlendStateDesc& desc, D3D11_BLEND_DESC& d3d11Desc )
{
    SET_BLEND_DESC_DEFAULTS( d3d11Desc );
    d3d11Desc.AlphaToCoverageEnable = ( desc.alphaToCoverageEnabled() ? TRUE : FALSE );
    d3d11Desc.IndependentBlendEnable = FALSE;
    const CBlendStateConfig& config = desc.getConfig();
    D3D11_RENDER_TARGET_BLEND_DESC& RenderTarget = d3d11Desc.RenderTarget[ 0 ];
    EBlendMul sourceMul, targetMul;
    ECombiner combiner;
    RenderTarget.BlendEnable = ( config.blendEnabled() ? TRUE : FALSE );
    config.getColourBlend( sourceMul, targetMul, combiner );
    RenderTarget.SrcBlend = REMAP_EBlendMul[ static_cast< int >( sourceMul ) ];
    RenderTarget.DestBlend = REMAP_EBlendMul[ static_cast< int >( targetMul ) ];
    RenderTarget.BlendOp = REMAP_ECombiner[ static_cast< int >( combiner ) ];
    config.getAlphaBlend( sourceMul, targetMul, combiner );
    RenderTarget.SrcBlendAlpha = REMAP_EBlendMul[ static_cast< int >( sourceMul ) ];
    RenderTarget.DestBlendAlpha = REMAP_EBlendMul[ static_cast< int >( targetMul ) ];
    RenderTarget.BlendOpAlpha = REMAP_ECombiner[ static_cast< int >( combiner ) ];
    RenderTarget.RenderTargetWriteMask = config.getMask();
}

static void SET_BLEND_DESC( const CMultiBlendStateDesc& desc, D3D11_BLEND_DESC& d3d11Desc )
{
    SET_BLEND_DESC_DEFAULTS( d3d11Desc );
    uint count = desc.getTargetCount();
    d3d11Desc.AlphaToCoverageEnable = ( desc.alphaToCoverageEnabled() ? TRUE : FALSE );
    d3d11Desc.IndependentBlendEnable = ( ( count > 1 ) ? TRUE : FALSE );
    for( uint index = 0; index < count; ++index )
    {
        const CBlendStateConfig& config = desc.getConfig( index );
        D3D11_RENDER_TARGET_BLEND_DESC& RenderTarget = d3d11Desc.RenderTarget[ index ];
        EBlendMul sourceMul, targetMul;
        ECombiner combiner;
        RenderTarget.BlendEnable = ( config.blendEnabled() ? TRUE : FALSE );
        config.getColourBlend( sourceMul, targetMul, combiner );
        RenderTarget.SrcBlend = REMAP_EBlendMul[ static_cast< int >( sourceMul ) ];
        RenderTarget.DestBlend = REMAP_EBlendMul[ static_cast< int >( targetMul ) ];
        RenderTarget.BlendOp = REMAP_ECombiner[ static_cast< int >( combiner ) ];
        config.getAlphaBlend( sourceMul, targetMul, combiner );
        RenderTarget.SrcBlendAlpha = REMAP_EBlendMul[ static_cast< int >( sourceMul ) ];
        RenderTarget.DestBlendAlpha = REMAP_EBlendMul[ static_cast< int >( targetMul ) ];
        RenderTarget.BlendOpAlpha = REMAP_ECombiner[ static_cast< int >( combiner ) ];
        RenderTarget.RenderTargetWriteMask = config.getMask();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end D3D11_REMAP namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace D3D11_REMAP

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CSamplerStateX sampler state class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSamplerStateX::clone( const CSamplerStateX& state )
{
    if( m_state )
    {
        m_state->Release();
        CCoreX::release();
    }
    m_state = state.m_state;
    if( m_state )
    {
        CCoreX::acquire();
        m_state->AddRef();
    }
}

bool CSamplerStateX::create( const CSamplerDesc& desc )
{
    if( CCoreX::acquire() )
    {
        destroy();
        D3D11_SAMPLER_DESC d3d11Desc;
        D3D11_REMAP::SET_SAMPLER_DESC( desc, d3d11Desc );
        HRESULT hr = CCoreX::getDevice()->CreateSamplerState( &d3d11Desc, &m_state );
        if( hr == S_OK )
        {
            return( true );
        }
        LOG( "ID3D11Device::CreateSamplerState() failed!!!" );
        m_state = NULL;
        CCoreX::release();
    }
    return( false );
}

void CSamplerStateX::destroy()
{
    if( m_state != NULL )
    {
        m_state->Release();
        m_state = NULL;
        CCoreX::release();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CRasterStateX rasteriser state class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CRasterStateX::clone( const CRasterStateX& state )
{
    if( m_state )
    {
        m_state->Release();
        CCoreX::release();
    }
    m_state = state.m_state;
    if( m_state )
    {
        CCoreX::acquire();
        m_state->AddRef();
    }
}

bool CRasterStateX::create( const CRasterStateDesc& desc )
{
    if( CCoreX::acquire() )
    {
        destroy();
        D3D11_RASTERIZER_DESC d3d11Desc;
        D3D11_REMAP::SET_RASTERIZER_DESC( desc, d3d11Desc );
        HRESULT hr = CCoreX::getDevice()->CreateRasterizerState( &d3d11Desc, &m_state );
        if( hr == S_OK )
        {
            return( true );
        }
        LOG( "ID3D11Device::CreateRasterizerState() failed!!!" );
        m_state = NULL;
        CCoreX::release();
    }
    return( false );
}

void CRasterStateX::destroy()
{
    if( m_state != NULL )
    {
        m_state->Release();
        m_state = NULL;
        CCoreX::release();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CDepthStateX depth state class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDepthStateX::clone( const CDepthStateX& state )
{
    if( m_state )
    {
        m_state->Release();
        CCoreX::release();
    }
    m_state = state.m_state;
    if( m_state )
    {
        CCoreX::acquire();
        m_state->AddRef();
    }
}

bool CDepthStateX::create( const CDepthStateDesc& desc )
{
    if( CCoreX::acquire() )
    {
        destroy();
        D3D11_DEPTH_STENCIL_DESC d3d11Desc;
        D3D11_REMAP::SET_DEPTH_STENCIL_DESC( desc, d3d11Desc );
        HRESULT hr = CCoreX::getDevice()->CreateDepthStencilState( &d3d11Desc, &m_state );
        if( hr == S_OK )
        {
            return( true );
        }
        LOG( "ID3D11Device::CreateDepthStencilState() failed!!!" );
        m_state = NULL;
        CCoreX::release();
    }
    return( false );
}

void CDepthStateX::destroy()
{
    if( m_state != NULL )
    {
        m_state->Release();
        m_state = NULL;
        CCoreX::release();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBlendStateX blend state class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBlendStateX::clone( const CBlendStateX& state )
{
    if( m_state )
    {
        m_state->Release();
        CCoreX::release();
    }
    m_state = state.m_state;
    if( m_state )
    {
        CCoreX::acquire();
        m_state->AddRef();
    }
}

bool CBlendStateX::create( const CBlendStateConfig& config )
{
    if( CCoreX::acquire() )
    {
        destroy();
        D3D11_BLEND_DESC d3d11Desc;
        D3D11_REMAP::SET_BLEND_DESC( config, d3d11Desc );
        HRESULT hr = CCoreX::getDevice()->CreateBlendState( &d3d11Desc, &m_state );
        if( hr == S_OK )
        {
            return( true );
        }
        LOG( "ID3D11Device::CreateBlendState() failed!!!" );
        m_state = NULL;
        CCoreX::release();
    }
    return( false );
}

bool CBlendStateX::create( const CBlendStateDesc& desc )
{
    if( CCoreX::acquire() )
    {
        destroy();
        D3D11_BLEND_DESC d3d11Desc;
        D3D11_REMAP::SET_BLEND_DESC( desc, d3d11Desc );
        HRESULT hr = CCoreX::getDevice()->CreateBlendState( &d3d11Desc, &m_state );
        if( hr == S_OK )
        {
            return( true );
        }
        LOG( "ID3D11Device::CreateBlendState() failed!!!" );
        m_state = NULL;
        CCoreX::release();
    }
    return( false );
}

bool CBlendStateX::create( const CMultiBlendStateDesc& desc )
{
    if( CCoreX::acquire() )
    {
        destroy();
        D3D11_BLEND_DESC d3d11Desc;
        D3D11_REMAP::SET_BLEND_DESC( desc, d3d11Desc );
        HRESULT hr = CCoreX::getDevice()->CreateBlendState( &d3d11Desc, &m_state );
        if( hr == S_OK )
        {
            return( true );
        }
        LOG( "ID3D11Device::CreateBlendState() failed!!!" );
        m_state = NULL;
        CCoreX::release();
    }
    return( false );
}

void CBlendStateX::destroy()
{
    if( m_state != NULL )
    {
        m_state->Release();
        m_state = NULL;
        CCoreX::release();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CStatesX state creation and caching class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CStatesX::init( const uint count )
{
    kill();
    uint size = ( ( count < 4 ) ? 4 : count );
    if( m_samplers.init( size ) && m_depthStates.init( size ) && m_rasterStates.init( size ) && m_blendStates.init( size ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void CStatesX::kill()
{
    m_samplers.kill();
    m_depthStates.kill();
    m_rasterStates.kill();
    m_blendStates.kill();
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

