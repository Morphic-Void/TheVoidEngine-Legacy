
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   physicals.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Private physical resource classes.
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

#include "factory.h"
#include "libs/system/debug/asserts.h"
#include <d3d11.h>

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
////    begin REMAP_D3D11 namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace REMAP_D3D11
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
////    DirectX desc conversion functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void SET_BUFFER_DESC( const EBufferAccess access, const CBufferDesc& desc, D3D11_BUFFER_DESC& d3d11Desc )
//{
//    memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
//    d3d11Desc.ByteWidth = ( desc.getStride() * desc.getCount() );
//    UINT usage = desc.getUsage();
//    switch( access )
//    {
//        case( EBufferReadable ):
//        {
//            d3d11Desc.Usage = D3D11_USAGE_STAGING;
//            d3d11Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
//            d3d11Desc.MiscFlags = 0;
//            break;
//        }
//        case( EBufferWritable ):
//        {
//            d3d11Desc.Usage = D3D11_USAGE_DYNAMIC;
//            d3d11Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//            d3d11Desc.MiscFlags = 0;
//            break;
//        }
//        case( EBufferConstant ):
//        {
//            d3d11Desc.Usage = D3D11_USAGE_IMMUTABLE;
//            d3d11Desc.CPUAccessFlags = 0;
//            d3d11Desc.MiscFlags = 0;
//            if( usage & EBufferBindCube ) d3d11Desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
//            if( usage & EBufferBindParams ) d3d11Desc.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
//            if( usage & EBufferBindStruct ) d3d11Desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
//            if( usage & EBufferBindClampMips ) d3d11Desc.MiscFlags |= D3D11_RESOURCE_MISC_RESOURCE_CLAMP;
//            if( usage & EBufferBindClampMips )
//
//  D3D11_RESOURCE_MISC_TEXTURECUBE                      = 0x4L,
//  D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS                = 0x10L,
//  D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS           = 0x20L,
//  D3D11_RESOURCE_MISC_BUFFER_STRUCTURED                = 0x40L,
//  D3D11_RESOURCE_MISC_RESOURCE_CLAMP                   = 0x80L,
//
//            break;
//        }
//        default:
//        {   //  EBufferInternal
//            d3d11Desc.Usage = D3D11_USAGE_DEFAULT;
//            d3d11Desc.CPUAccessFlags = 0;
//            d3d11Desc.MiscFlags = 0;
//            break;
//        }
//    }
//
////  need to be able to identify immutable for usage (must have initialisation)
//
////BindFlags
////Type: UINT
////Identify how the buffer will be bound to the pipeline. Flags (see D3D11_BIND_FLAG) can be combined with a logical OR.
////CPUAccessFlags
////Type: UINT
////CPU access flags (see D3D11_CPU_ACCESS_FLAG) or 0 if no CPU access is necessary. Flags can be combined with a logical OR.
////MiscFlags
////Type: UINT
////Miscellaneous flags (see D3D11_RESOURCE_MISC_FLAG) or 0 if unused. Flags can be combined with a logical OR.
////StructureByteStride
//}
//
//void SET_BUFFER_DESC( const EBufferAccess access, const CBufferDesc& desc, D3D11_TEXTURE1D_DESC& d3d11Desc )
//{
//    memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
//}
//
//void SET_BUFFER_DESC( const EBufferAccess access, const CBufferDesc& desc, D3D11_TEXTURE2D_DESC& d3d11Desc )
//{
//    memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
//}
//
//void SET_BUFFER_DESC( const EBufferAccess access, const CBufferDesc& desc, D3D11_TEXTURE3D_DESC& d3d11Desc )
//{
//    memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
//}

void SET_SAMPLER_DESC( const CSamplerDesc& desc, D3D11_SAMPLER_DESC& d3d11Desc )
{
    memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
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

void SET_RASTERIZER_DESC( const CRasterStateDesc& desc, D3D11_RASTERIZER_DESC& d3d11Desc )
{
    memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
    d3d11Desc.FillMode = ( desc.wireframeEnabled() ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID );
    d3d11Desc.CullMode = REMAP_ECullMode[ static_cast< int >( desc.getCulling() ) ];
    d3d11Desc.FrontCounterClockwise = ( desc.frontIsAntiClockwise() ? TRUE : FALSE );
    desc.getBiasing( d3d11Desc.DepthBias, d3d11Desc.SlopeScaledDepthBias, d3d11Desc.DepthBiasClamp );
    d3d11Desc.DepthClipEnable = ( desc.depthClipEnabled() ? TRUE : FALSE );
    d3d11Desc.ScissorEnable = ( desc.scissorEnabled() ? TRUE : FALSE );
    d3d11Desc.MultisampleEnable = ( desc.multiSampleEnabled() ? TRUE : FALSE );
    d3d11Desc.AntialiasedLineEnable = ( desc.antiAliasedLineEnabled() ? TRUE : FALSE );
}

void SET_DEPTH_STENCIL_DESC( const CDepthStateDesc& desc, D3D11_DEPTH_STENCIL_DESC& d3d11Desc )
{
    memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
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

void SET_BLEND_DESC_DEFAULTS( D3D11_BLEND_DESC& d3d11Desc )
{
    memset( &d3d11Desc, 0, sizeof( d3d11Desc ) );
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

void SET_BLEND_DESC( const CBlendStateConfig& config, D3D11_BLEND_DESC& d3d11Desc )
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

void SET_BLEND_DESC( const CBlendStateDesc& desc, D3D11_BLEND_DESC& d3d11Desc )
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

void SET_BLEND_DESC( const CMultiBlendStateDesc& desc, D3D11_BLEND_DESC& d3d11Desc )
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
////    end REMAP_D3D11 namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace REMAP_D3D11

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Physical resource type specific resource class generic function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//bool CPhysicalBuffer::create( CPhysicalBuffers& pool, const CBufferInitDesc& init )
//{
//    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
//    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
//    {
//        reset();
//        const CBufferDesc& desc = init.getDesc();
//        const CBufferData* data = init.getData();
//        const EBufferAccess access = init.getAccess();
//        switch( desc.getUsage() & EBufferBindTypes )
//        {
//            case( EBufferBindBuffer ):
//            {
//                D3D11_BUFFER_DESC d3d11Desc; 
//                REMAP_D3D11::SET_BUFFER_DESC( access, desc, d3d11Desc );
//                HRESULT hr = pool.getFactory()->getDevice()->CreateBuffer( &d3d11Desc, NULL, reinterpret_cast< ID3D11Buffer** >( &m_physical.unknown ) );
//                if( hr == S_OK )
//                {
//                    m_refs = 1;
//                    m_pool = &pool;
//                    m_physical.desc = desc;
//                    m_physical.desc.setAccess( access );
//                    return( true );
//                }
//                LOG( "ID3D11Device::CreateBuffer() failed!!!" );
//                break;
//            }
//            case( EBufferBind1D ):
//            {
//                D3D11_TEXTURE1D_DESC d3d11Desc; 
//                REMAP_D3D11::SET_BUFFER_DESC( access, desc, d3d11Desc );
//                HRESULT hr = pool.getFactory()->getDevice()->CreateTexture1D( &d3d11Desc, NULL, reinterpret_cast< ID3D11Texture1D** >( &m_physical.unknown ) );
//                if( hr == S_OK )
//                {
//                    m_refs = 1;
//                    m_pool = &pool;
//                    m_physical.desc = desc;
//                    m_physical.desc.setAccess( access );
//                    return( true );
//                }
//                LOG( "ID3D11Device::CreateTexture1D() failed!!!" );
//                break;
//            }
//            case( EBufferBind2D ):
//            case( EBufferBindCube ):
//            case( EBufferBind2D | EBufferBindCube ):
//            {
//                D3D11_TEXTURE2D_DESC d3d11Desc; 
//                REMAP_D3D11::SET_BUFFER_DESC( access, desc, d3d11Desc );
//                HRESULT hr = pool.getFactory()->getDevice()->CreateTexture2D( &d3d11Desc, NULL, reinterpret_cast< ID3D11Texture2D** >( &m_physical.unknown ) );
//                if( hr == S_OK )
//                {
//                    m_refs = 1;
//                    m_pool = &pool;
//                    m_physical.desc = desc;
//                    m_physical.desc.setAccess( access );
//                    return( true );
//                }
//                LOG( "ID3D11Device::CreateTexture2D() failed!!!" );
//                break;
//            }
//            case( EBufferBind3D ):
//            {
//                D3D11_TEXTURE3D_DESC d3d11Desc; 
//                REMAP_D3D11::SET_BUFFER_DESC( access, desc, d3d11Desc );
//                HRESULT hr = pool.getFactory()->getDevice()->CreateTexture3D( &d3d11Desc, NULL, reinterpret_cast< ID3D11Texture3D** >( &m_physical.unknown ) );
//                if( hr == S_OK )
//                {
//                    m_refs = 1;
//                    m_pool = &pool;
//                    m_physical.desc = desc;
//                    m_physical.desc.setAccess( access );
//                    return( true );
//                }
//                LOG( "ID3D11Device::CreateTexture3D() failed!!!" );
//                break;
//            }
//            default:
//            {
//                LOG( "Invalid physical buffer : create() failed!!!" );
//                break;
//            }
//        }
//    }
//    return( false );
//}
//
//bool CPhysicalAccess::create( CPhysicalAccessors& pool, const CAccessInitDesc& init )
//{
//    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
//    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
//    {
//        reset();
//
//        UNUSED( desc );
//
////  need the actual resource
//
//        //D3D11_SAMPLER_DESC d3d11Desc;
//        //REMAP_D3D11::SET_SAMPLER_DESC( desc, d3d11Desc );
//        //HRESULT hr = pool.getFactory()->getDevice()->CreateSamplerState( &d3d11Desc, reinterpret_cast< ID3D11SamplerState** >( &m_physical.unknown ) );
//        //if( hr == S_OK )
//        //{
//        //    m_refs = 1;
//        //    m_pool = &pool;
//        //    m_physical.desc = desc;
//        //    return( true );
//        //}
//        //LOG( "ID3D11Device::CreateSamplerState() failed!!!" );
//    }
//    return( false );
//}

bool CPhysicalSampler::create( CPhysicalSamplers& pool, const CSamplerDesc& desc )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        D3D11_SAMPLER_DESC d3d11Desc;
        REMAP_D3D11::SET_SAMPLER_DESC( desc, d3d11Desc );
        HRESULT hr = pool.getFactory()->getDevice()->CreateSamplerState( &d3d11Desc, reinterpret_cast< ID3D11SamplerState** >( &m_physical.unknown ) );
        if( hr == S_OK )
        {
            m_refs = 1;
            m_pool = &pool;
            m_physical.desc = desc;
            return( true );
        }
        LOG( "ID3D11Device::CreateSamplerState() failed!!!" );
    }
    return( false );
}

bool CPhysicalRasterState::create( CPhysicalRasterStates& pool, const CRasterStateDesc& desc )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        D3D11_RASTERIZER_DESC d3d11Desc;
        REMAP_D3D11::SET_RASTERIZER_DESC( desc, d3d11Desc );
        HRESULT hr = pool.getFactory()->getDevice()->CreateRasterizerState( &d3d11Desc, reinterpret_cast< ID3D11RasterizerState** >( &m_physical.unknown ) );
        if( hr == S_OK )
        {
            m_refs = 1;
            m_pool = &pool;
            m_physical.desc = desc;
            return( true );
        }
        LOG( "ID3D11Device::CreateRasterizerState() failed!!!" );
    }
    return( false );
}

bool CPhysicalDepthState::create( CPhysicalDepthStates& pool, const CDepthStateDesc& desc )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        D3D11_DEPTH_STENCIL_DESC d3d11Desc;
        REMAP_D3D11::SET_DEPTH_STENCIL_DESC( desc, d3d11Desc );
        HRESULT hr = pool.getFactory()->getDevice()->CreateDepthStencilState( &d3d11Desc, reinterpret_cast< ID3D11DepthStencilState** >( &m_physical.unknown ) );
        if( hr == S_OK )
        {
            m_refs = 1;
            m_pool = &pool;
            m_physical.desc = desc;
            return( true );
        }
        LOG( "ID3D11Device::CreateDepthStencilState() failed!!!" );
    }
    return( false );
}

bool CPhysicalBlendState::create( CPhysicalBlendStates& pool, const CMultiBlendStateDesc& desc )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        D3D11_BLEND_DESC d3d11Desc;
        REMAP_D3D11::SET_BLEND_DESC( desc, d3d11Desc );
        HRESULT hr = pool.getFactory()->getDevice()->CreateBlendState( &d3d11Desc, reinterpret_cast< ID3D11BlendState** >( &m_physical.unknown ) );
        if( hr == S_OK )
        {
            m_refs = 1;
            m_pool = &pool;
            m_physical.desc = desc;
            return( true );
        }
        LOG( "ID3D11Device::CreateBlendState() failed!!!" );
    }
    return( false );
}

//void CPhysicalBuffer::destroy()
//{
//    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
//    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
//    {
//        m_pool = NULL;
//        if( m_physical.unknown != NULL )
//        {
//            reinterpret_cast< IUnknown* >( m_physical.unknown )->Release();
//        }
//        reset();
//    }
//}
//
//void CPhysicalAccess::destroy()
//{
//    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
//    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
//    {
//        m_pool = NULL;
//        if( m_physical.unknown != NULL )
//        {
//            reinterpret_cast< IUnknown* >( m_physical.unknown )->Release();
//        }
//        reset();
//    }
//}

void CPhysicalSampler::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        if( m_physical.unknown != NULL )
        {
            reinterpret_cast< IUnknown* >( m_physical.unknown )->Release();
        }
        reset();
    }
}

void CPhysicalRasterState::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        if( m_physical.unknown != NULL )
        {
            reinterpret_cast< IUnknown* >( m_physical.unknown )->Release();
        }
        reset();
    }
}

void CPhysicalDepthState::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        if( m_physical.unknown != NULL )
        {
            reinterpret_cast< IUnknown* >( m_physical.unknown )->Release();
        }
        reset();
    }
}

void CPhysicalBlendState::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        if( m_physical.unknown != NULL )
        {
            reinterpret_cast< IUnknown* >( m_physical.unknown )->Release();
        }
        reset();
    }
}

//void CPhysicalBuffer::addRef()
//{
//    ASSERT( valid() );
//    if( valid() )
//    {
//        ++m_refs;
//    }
//}
//
//void CPhysicalAccess::addRef()
//{
//    ASSERT( valid() );
//    if( valid() )
//    {
//        ++m_refs;
//    }
//}

void CPhysicalSampler::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CPhysicalRasterState::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CPhysicalDepthState::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CPhysicalBlendState::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

//void CPhysicalBuffer::release()
//{
//    ASSERT( valid() );
//    if( valid() )
//    {
//        --m_refs;
//        if( !m_refs )
//        {
//            m_pool->destroy( *this );
//        }
//    }
//}
//
//void CPhysicalAccess::release()
//{
//    ASSERT( valid() );
//    if( valid() )
//    {
//        --m_refs;
//        if( !m_refs )
//        {
//            m_pool->destroy( *this );
//        }
//    }
//}

void CPhysicalSampler::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            m_pool->destroy( *this );
        }
    }
}

void CPhysicalRasterState::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            m_pool->destroy( *this );
        }
    }
}

void CPhysicalDepthState::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            m_pool->destroy( *this );
        }
    }
}

void CPhysicalBlendState::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            m_pool->destroy( *this );
        }
    }
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

