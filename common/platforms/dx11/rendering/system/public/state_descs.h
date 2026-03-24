
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:    state_descs.h
////    Author:  Ritchie Brannan
////    Date:    11 Nov 10
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
////    Manifest:
////
////        enum   ECmpFunc                 :   comparison function types (8 functions)
////        enum   ESamplingMode            :   sampler state filtering mode (4 modes)
////        enum   EWrapMode                :   sampler state wrapping mode (5 modes)
////        enum   EStencilOp               :   depth-stencil state stencil operations (8 operations)
////        enum   ECullMode                :   raster state culling mode (3 modes)
////        enum   ESamplesUAV              :   raster state sample count for UAV rendering or UAV rasterisation (6 settings)
////        enum   EBlendMul                :   blend state blend data modulation (17 options)
////        enum   ECombiner                :   blend state combiner operation (5 operations)
////        enum   ELogicOp                 :   blend state logic operation (16 operations)
////        struct SAMPLER_DEF              :   sampler state definition structure
////        class  CSamplerDesc             :   sampler state description class
////        struct RASTER_STATE_DEF         :   raster state definition structure
////        class  CRasterStateDesc         :   rasteriser state description class
////        struct DEPTH_STATE_DEF          :   depth state definition structure
////        struct STENCIL_FACE_STATE_DEF   :   stencil face state definition structure
////        struct STENCIL_STATE_DEF        :   stencil state definition structure
////        struct DEPTH_STENCILSTATE__DEF  :   depth-stencil state definition structure
////        class  CDepthStateDesc          :   depth-stencil state description class
////        struct BLEND_EFFECT_STATE_DEF   :   blend effect configuration definition structure
////        struct BLEND_CONFIG_STATE_DEF   :   blending configuration definition structure
////        struct LOGIC_CONFIG_STATE_DEF   :   logic configuration definition structure
////        struct BLEND_TARGET_STATE_DEF   :   render target blending definition structure
////        struct BLEND_STATE_DEF          :   blend state definition structure
////        struct MULTI_BLEND_STATE_DEF    :   blend state definition structure
////        class  CBlendStateConfig        :   blend configuration class
////        class  CBlendStateDesc          :   blend state description class
////        class  CMultiBlendStateDesc     :   blend state description class
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

#ifndef __PUBLIC_STATE_DESCS_INCLUDED__
#define __PUBLIC_STATE_DESCS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"

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
////    Enumerated types
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum ECmpFunc
{   //  comparison function types (8 functions)
    ECmpFuncNever,
    ECmpFuncAlways,
    ECmpFuncEqual,
    ECmpFuncNotEqual,
    ECmpFuncLess,
    ECmpFuncLessEqual,
    ECmpFuncGreater,
    ECmpFuncGreaterEqual,
    ECmpFuncCount,
    ECmpFuncForce32 = 0x7fffffff,
};

enum ESamplingMode
{   //  sampler state filtering mode (4 modes)
    ESamplingFilter = 0,
    ESamplingMinimum,
    ESamplingMaximum,
    ESamplingCompare,
    ESamplingModeCount,
    ESamplingModeForce32 = 0x7fffffff,
};

enum EWrapMode
{   //  sampler state wrapping mode (5 modes)
    EWrapModeClamp = 0,
    EWrapModeRepeat,
    EWrapModeMirror,
    EWrapModeMirrorOnce,
    EWrapModeBorder,
    EWrapModeCount,
    EWrapModeForce32 = 0x7fffffff,
};

enum EStencilOp
{   //  depth-stencil state stencil operations (8 operations)
    EStencilOpKeep = 0,
    EStencilOpZero,
    EStencilOpInv,
    EStencilOpInc,
    EStencilOpDec,
    EStencilOpIncSat,
    EStencilOpDecSat,
    EStencilOpReplace,
    EStencilOpCount,
    EStencilOpForce32 = 0x7fffffff,
};

enum ECullMode
{   //  raster state culling mode (3 modes)
    ECullModeNone = 0,
    ECullModeBack,
    ECullModeFront,
    ECullModeCount,
    ECullModeForce32 = 0x7fffffff,
};

enum ESamplesUAV
{   //  raster state sample count for UAV rendering or UAV rasterisation (6 settings)
    ESamplesUnforced = 0,
    ESamplesForce1,
    ESamplesForce2,
    ESamplesForce4,
    ESamplesForce8,
    ESamplesForce16,
    ESamplesUAVCount,
    ESamplesUAVForce32 = 0x7fffffff,
};

enum EBlendMul
{   //  blend state blend data modulation (17 options)
    EBlendMulZero = 0,
    EBlendMulOne,
    EBlendMulFactor,
    EBlendMulInvFactor,
    EBlendMulSrcA,
    EBlendMulInvSrcA,
    EBlendMulTrgA,
    EBlendMulInvTrgA,
    EBlendMulAltA,
    EBlendMulInvAltA,
    EBlendMulSrcSatA,       //  this is the last blend mul which can be used for blending of alpha values
    EBlendMulSrcRGB,
    EBlendMulInvSrcRGB,
    EBlendMulTrgRGB,
    EBlendMulInvTrgRGB,
    EBlendMulAltRGB,
    EBlendMulInvAltRGB,
    EBlendMulCount,
    EBlendMulForce32 = 0x7fffffff,
};

enum ECombiner
{   //  blend state combiner operation (5 operations)
    ECombinerAdd = 0,       //  src1 + src2
    ECombinerSub,           //  src1 - src2
    ECombinerRevSub,        //  src2 - src1
    ECombinerMin,           //  min( src1, src2 )
    ECombinerMax,           //  max( src1, src2 )
    ECombinerCount,
    ECombinerForce32 = 0x7fffffff,
};

enum ELogicOp
{   //  blend state logic operation (16 operations)
    ELogicOpNop = 0,
    ELogicOpZero,
    ELogicOpSet,
    ELogicOpCopy,
    ELogicOpCopyInv,
    ELogicOpInv,
    ELogicOpAnd,
    ELogicOpNand,
    ELogicOpOr,
    ELogicOpNor,
    ELogicOpXor,
    ELogicOpEquiv,
    ELogicOpAndRev,
    ELogicOpAndInv,
    ELogicOpOrRev,
    ELogicOpOrInv,
    ELogicOpCount,
    ELogicOpForce32 = 0x7fffffff,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    SAMPLER_DEF sampler state definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SAMPLER_DEF
{
    ESamplingMode               SamplingMode;
    ECmpFunc                    CmpFunc;
    BOOL                        LinearMin, LinearMag, LinearMip;
    EWrapMode                   WrapU, WrapV, WrapW;
    UINT                        AnisotropicMax;
    BOOL                        EnableBorder;
    FLOAT                       Border[ 4 ];
    BOOL                        EnableModifyLOD;
    FLOAT                       MinLOD;
    FLOAT                       MaxLOD;
    FLOAT                       BiasLOD;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CSamplerDesc sampler state description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSamplerDesc
{
public:
    inline                      CSamplerDesc() { setDefault(); };
    inline                      CSamplerDesc( const CSamplerDesc& desc ) { set( desc ); };
    inline                      CSamplerDesc( const SAMPLER_DEF& samplerDef ) { set( samplerDef ); };
    inline                      ~CSamplerDesc() {};
    void                        setDefault();
    void                        set( const CSamplerDesc& desc );
    void                        set( const SAMPLER_DEF& samplerDef );
    void                        setMode( const ESamplingMode mode );
    void                        setCmpFunc( const ECmpFunc func );
    void                        setLinear( const bool min, const bool mag, const bool mip );
    void                        setWrapping( const EWrapMode u = EWrapModeRepeat, const EWrapMode v = EWrapModeRepeat, const EWrapMode w = EWrapModeClamp );
    void                        setAnisotropic( const uint max );
    void                        setLodding( const float min, const float max, const float bias );
    void                        setBorder( const float r, const float g, const float b, const float a );
    ESamplingMode               getMode() const;
    ECmpFunc                    getCmpFunc() const;
    bool                        getLinear( bool& min, bool& mag, bool& mip ) const;
    void                        getWrapping( EWrapMode& u, EWrapMode& v, EWrapMode& w ) const;
    uint                        getAnisotropic() const;
    bool                        getLodding( float& min, float& max, float& bias ) const;
    bool                        getBorder( float& r, float& g, float& b, float& a ) const;
    bool                        hasLodding() const;
    bool                        hasBorder() const;
    void                        clearLodding();
    void                        clearBorder();
    int                         compare( const CSamplerDesc& desc ) const;
    inline CSamplerDesc&        operator=( const CSamplerDesc& desc ) { set( desc ); return( *this ); };
    inline CSamplerDesc&        operator=( const SAMPLER_DEF& samplerDef ) { set( samplerDef ); return( *this ); };
protected:
    uint32_t                    m_desc;
    float                       m_border[ 4 ];
    float                       m_minLOD, m_maxLOD, m_biasLOD;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    RASTER_STATE_DEF raster state definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct RASTER_STATE_DEF
{
    ECullMode                   Culling;
    ESamplesUAV                 SamplesUAV;
    BOOL                        AntiClockwise;
    BOOL                        EnableScissor;
    BOOL                        EnableWireframe;
    BOOL                        EnableDepthClip;
    BOOL                        EnableMultiSample;
    BOOL                        EnableAntiAliasedLine;
    BOOL                        EnableBiasing;
    INT                         DepthBias;
    FLOAT                       SlopeBias;
    FLOAT                       ClampBias;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CRasterStateDesc rasteriser state description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CRasterStateDesc
{
public:
    inline                      CRasterStateDesc() { setDefault(); };
    inline                      CRasterStateDesc( const CRasterStateDesc& desc ) { set( desc ); };
    inline                      CRasterStateDesc( const RASTER_STATE_DEF& def ) { set( def ); };
    inline                      ~CRasterStateDesc() {};
    void                        setDefault();
    void                        set( const CRasterStateDesc& desc );
    void                        set( const RASTER_STATE_DEF& def );
    void                        setFrontAntiClockwise( const bool antiClockwise = true );
    inline void                 setFrontClockwise() { setFrontAntiClockwise( false ); };
    void                        setCulling( const ECullMode culling );
    void                        setBiasing( const int depth, const float slope, const float clamp );
    void                        setSamplesUAV( const ESamplesUAV samples );
    ECullMode                   getCulling() const;
    bool                        getBiasing( int& depth, float& slope, float& clamp ) const;
    ESamplesUAV                 getSamplesUAV() const;
    void                        enableScissor( const bool enable = true );
    void                        enableWireframe( const bool enable = true );
    void                        enableDepthClip( const bool enable = true );
    void                        enableMultiSample( const bool enable = true );
    void                        enableAntiAliasedLine( const bool enable = true );
    inline void                 disableScissor() { enableScissor( false ); };
    inline void                 disableWireframe() { enableWireframe( false ); };
    inline void                 disableDepthClip() { enableDepthClip( false ); };
    inline void                 disableMultiSample() { enableMultiSample( false ); };
    inline void                 disableAntiAliasedLine() { enableAntiAliasedLine( false ); };
    bool                        frontIsAntiClockwise() const;
    inline bool                 frontIsClockwise() const { return( !frontIsAntiClockwise() ); };
    bool                        scissorEnabled() const;
    bool                        wireframeEnabled() const;
    bool                        depthClipEnabled() const;
    bool                        multiSampleEnabled() const;
    bool                        antiAliasedLineEnabled() const;
    bool                        hasBiasing() const;
    void                        clearBiasing();
    int                         compare( const CRasterStateDesc& desc ) const;
    inline CRasterStateDesc&    operator=( const CRasterStateDesc& desc ) { set( desc ); return( *this ); };
    inline CRasterStateDesc&    operator=( const RASTER_STATE_DEF& def ) { set( def ); return( *this ); };
protected:
    uint32_t                    m_desc;
    float                       m_biasSlope, m_biasClamp;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DEPTH_STATE_DEF depth state definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DEPTH_STATE_DEF
{
    BOOL                        EnableReads;
    BOOL                        EnableWrites;
    ECmpFunc                    DepthCmpFunc;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    STENCIL_FACE_STATE_DEF stencil face state definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct STENCIL_FACE_STATE_DEF
{
    ECmpFunc                    CmpFunc;
    EStencilOp                  PassBoth;
    EStencilOp                  PassStencil;
    EStencilOp                  FailStencil;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    STENCIL_STATE_DEF stencil state definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct STENCIL_STATE_DEF
{
    BOOL                        EnableStencil;
    UINT                        StencilReadMask;
    UINT                        StencilWriteMask;
    STENCIL_FACE_STATE_DEF      FrontFace;
    STENCIL_FACE_STATE_DEF      BackFace;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DEPTH_STENCIL_STATE_DEF depth-stencil state definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DEPTH_STENCIL_STATE_DEF
{
    DEPTH_STATE_DEF             DepthDef;
    STENCIL_STATE_DEF           StencilDef;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CDepthStateDesc depth-stencil state description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CDepthStateDesc
{
public:
    inline                      CDepthStateDesc() { setDefault(); };
    inline                      CDepthStateDesc( const CDepthStateDesc& desc ) { set( desc ); };
    inline                      CDepthStateDesc( const DEPTH_STATE_DEF& def ) { set( def ); };
    inline                      CDepthStateDesc( const DEPTH_STENCIL_STATE_DEF& def ) { set( def ); };
    inline                      ~CDepthStateDesc() {};
    void                        setDefault();
    void                        set( const CDepthStateDesc& desc );
    void                        set( const DEPTH_STATE_DEF& def );
    void                        set( const STENCIL_STATE_DEF& def );
    void                        set( const DEPTH_STENCIL_STATE_DEF& def );
    void                        setDepthTest( const ECmpFunc cmpFunc );
    void                        setDepthTest( const ECmpFunc cmpFunc, const bool write );
    void                        setStencil( const ECmpFunc cmpFunc, const EStencilOp passBoth, const EStencilOp passStencil, const EStencilOp failStencil );
    void                        setFrontStencil( const ECmpFunc cmpFunc, const EStencilOp passBoth, const EStencilOp passStencil, const EStencilOp failStencil );
    void                        setBackStencil( const ECmpFunc cmpFunc, const EStencilOp passBoth, const EStencilOp passStencil, const EStencilOp failStencil );
    void                        setStencilMasks( const uint8_t read = 0xff, const uint8_t write = 0xff );
    bool                        getDepthTest( ECmpFunc& cmpFunc ) const;
    ECmpFunc                    getDepthTest() const;
    bool                        getFrontStencil( ECmpFunc& cmpFunc, EStencilOp& passBoth, EStencilOp& passStencil, EStencilOp& failStencil ) const;
    bool                        getBackStencil( ECmpFunc& cmpFunc, EStencilOp& passBoth, EStencilOp& passStencil, EStencilOp& failStencil ) const;
    void                        getStencilMasks( uint8_t& read, uint8_t& write ) const;
    void                        enableDepth( const bool read = true, const bool write = true );
    void                        enableDepthReads( const bool enable = true );
    void                        enableDepthWrites( const bool enable = true );
    void                        enableStencil( const bool enable = true );
    inline void                 disableDepth() { enableDepth( false, false ); };
    inline void                 disableDepthReads() { enableDepthReads( false ); };
    inline void                 disableDepthWrites() { enableDepthWrites( false ); };
    inline void                 disableStencil() { enableStencil( false ); };
    bool                        depthEnabled() const;
    bool                        depthReadsEnabled() const;
    bool                        depthWritesEnabled() const;
    bool                        stencilEnabled() const;
    int                         compare( const CDepthStateDesc& desc ) const;
    inline CDepthStateDesc&     operator=( const CDepthStateDesc& desc ) { set( desc ); return( *this ); };
    inline CDepthStateDesc&     operator=( const DEPTH_STATE_DEF& def ) { set( def ); return( *this ); };
    inline CDepthStateDesc&     operator=( const DEPTH_STENCIL_STATE_DEF& def ) { set( def ); return( *this ); };
protected:
    uint32_t                    m_desc;
    uint32_t                    m_masks;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BLEND_EFFECT_STATE_DEF blending configuration definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BLEND_EFFECT_STATE_DEF
{
    BOOL                        Enable;
    EBlendMul                   SourceMul;
    EBlendMul                   TargetMul;
    ECombiner                   Combiner;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BLEND_CONFIG_STATE_DEF blending configuration definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BLEND_CONFIG_STATE_DEF
{
    BLEND_EFFECT_STATE_DEF      Colour;
    BLEND_EFFECT_STATE_DEF      Alpha;
    UINT                        Mask;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    LOGIC_CONFIG_STATE_DEF logic configuration definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct LOGIC_CONFIG_STATE_DEF
{
    BOOL                        EnableOp;
    ELogicOp                    LogicOp;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BLEND_TARGET_STATE_DEF render target blending definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BLEND_TARGET_STATE_DEF
{
    BLEND_CONFIG_STATE_DEF      BlendConfig;
    LOGIC_CONFIG_STATE_DEF      LogicConfig;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////RASTER_
////    BLEND_STATE_DEF blend state definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BLEND_STATE_DEF
{
    BOOL                        EnableAlphaToCoverage;
    BLEND_TARGET_STATE_DEF      BlendTarget;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    MULTI_BLEND_STATE_DEF blend state definition structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct MULTI_BLEND_STATE_DEF
{
    BOOL                        EnableAlphaToCoverage;
    BLEND_TARGET_STATE_DEF      BlendTargets[ 8 ];
    UINT                        RenderTargetCount;
    void                        setDefault();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBlendStateConfig blend configuration class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBlendStateConfig
{
public:
    inline                      CBlendStateConfig() { setDefault(); };
    inline                      CBlendStateConfig( const CBlendStateConfig& config ) { set( config ); };
    inline                      CBlendStateConfig( const BLEND_CONFIG_STATE_DEF& def ) { set( def ); };
    inline                      ~CBlendStateConfig() {};
    void                        setDefault();
    void                        set( const CBlendStateConfig& config );
    void                        set( const BLEND_CONFIG_STATE_DEF& def );
    void                        setBlend( const EBlendMul sourceMul, const EBlendMul targetMul, const ECombiner combiner );
    void                        setColourBlend( const EBlendMul sourceMul, const EBlendMul targetMul, const ECombiner combiner );
    void                        setAlphaBlend( const EBlendMul sourceMul, const EBlendMul targetMul, const ECombiner combiner );
    void                        setMask( const uint8_t mask );
    bool                        getColourBlend( EBlendMul& sourceMul, EBlendMul& targetMul, ECombiner& combiner ) const;
    bool                        getAlphaBlend( EBlendMul& sourceMul, EBlendMul& targetMul, ECombiner& combiner ) const;
    uint8_t                     getMask() const;
    bool                        blendEnabled() const;
    bool                        hasColourBlend() const;
    bool                        hasAlphaBlend() const;
    void                        clearColourBlend();
    void                        clearAlphaBlend();
    inline int                  compare( const CBlendStateConfig& config ) const { return( ( m_config == config.m_config ) ? 0 : ( ( m_config < config.m_config ) ? -1  : 1 ) ); };
    inline CBlendStateConfig&   operator=( const CBlendStateConfig& config ) { set( config ); return( *this ); };
    inline CBlendStateConfig&   operator=( const BLEND_CONFIG_STATE_DEF& def ) { set( def ); return( *this ); };
private:
    uint32_t                    m_config;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBlendStateDesc blend state description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBlendStateDesc
{
public:
    inline                          CBlendStateDesc() { setDefault(); };
    inline                          CBlendStateDesc( const CBlendStateDesc& desc ) { set( desc ); };
    inline                          CBlendStateDesc( const CBlendStateConfig& config ) { set( config ); };
    inline                          CBlendStateDesc( const BLEND_STATE_DEF& def ) { set( def ); };
    inline                          CBlendStateDesc( const BLEND_CONFIG_STATE_DEF& configDef ) { set( configDef ); };
    inline                          CBlendStateDesc( const BLEND_TARGET_STATE_DEF& targetDef ) { set( targetDef ); };
    inline                          ~CBlendStateDesc() {};
    void                            setDefault();
    void                            set( const CBlendStateDesc& desc );
    void                            set( const CBlendStateConfig& config );
    void                            set( const BLEND_STATE_DEF& def );
    void                            set( const BLEND_CONFIG_STATE_DEF& configDef );
    void                            set( const BLEND_TARGET_STATE_DEF& targetDef );
    void                            enableAlphaToCoverage( const bool enable = true );
    inline void                     disableAlphaToCoverage() { enableAlphaToCoverage( false ); };
    bool                            alphaToCoverageEnabled() const;
    void                            setLogic( const ELogicOp op );
    bool                            getLogic( ELogicOp& op ) const;
    ELogicOp                        getLogic() const;
    bool                            logicEnabled() const;
    void                            clearLogic();
    inline CBlendStateConfig&       getConfig() { return( m_config ); };
    inline const CBlendStateConfig& getConfig() const { return( m_config ); };
    inline int                      compare( const CBlendStateDesc& desc ) const { return( ( m_desc == desc.m_desc ) ? m_config.compare( desc.m_config ) : ( ( m_desc < desc.m_desc ) ? -1  : 1 ) ); };
    inline CBlendStateDesc&         operator=( const CBlendStateDesc& desc ) { set( desc ); return( *this ); };
    inline CBlendStateDesc&         operator=( const BLEND_STATE_DEF& def ) { set( def ); return( *this ); };
    inline CBlendStateDesc&         operator=( const BLEND_CONFIG_STATE_DEF& configDef ) { set( configDef ); return( *this ); };
    inline CBlendStateDesc&         operator=( const BLEND_TARGET_STATE_DEF& targetDef ) { set( targetDef ); return( *this ); };
private:
    uint32_t                        m_desc;
    CBlendStateConfig               m_config;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CMultiBlendStateDesc blend state description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CMultiBlendStateDesc
{
public:
    inline                          CMultiBlendStateDesc() { setDefault(); };
    inline                          CMultiBlendStateDesc( const CBlendStateDesc& desc ) { set( desc ); };
    inline                          CMultiBlendStateDesc( const CMultiBlendStateDesc& desc ) { set( desc ); };
    inline                          CMultiBlendStateDesc( const CBlendStateConfig& config ) { set( config ); };
    inline                          CMultiBlendStateDesc( const BLEND_STATE_DEF& def ) { set( def ); };
    inline                          CMultiBlendStateDesc( const MULTI_BLEND_STATE_DEF& def ) { set( def ); };
    inline                          CMultiBlendStateDesc( const BLEND_CONFIG_STATE_DEF& configDef ) { set( configDef ); };
    inline                          CMultiBlendStateDesc( const BLEND_TARGET_STATE_DEF& targetDef ) { set( targetDef ); };
    inline                          ~CMultiBlendStateDesc() {};
    void                            setDefault();
    void                            set( const CBlendStateDesc& desc );
    void                            set( const CMultiBlendStateDesc& desc );
    void                            set( const CBlendStateConfig& config );
    void                            set( const BLEND_STATE_DEF& def );
    void                            set( const MULTI_BLEND_STATE_DEF& def );
    void                            set( const BLEND_CONFIG_STATE_DEF& configDef );
    void                            set( const BLEND_TARGET_STATE_DEF& targetDef );
    void                            setTargetCount( const uint count );
    uint                            getTargetCount() const;
    void                            enableAlphaToCoverage( const bool enable = true );
    inline void                     disableAlphaToCoverage() { enableAlphaToCoverage( false ); };
    bool                            alphaToCoverageEnabled() const;
    void                            setLogic( const uint index, const ELogicOp op );
    inline void                     setLogic( const ELogicOp op ) { setLogic( 0, op ); };
    bool                            getLogic( const uint index, ELogicOp& op ) const;
    ELogicOp                        getLogic( const uint index ) const;
    inline bool                     getLogic( ELogicOp& op ) const { return( getLogic( 0, op ) ); };
    inline ELogicOp                 getLogic() const { return( getLogic( 0 ) ); };
    bool                            logicEnabled( const uint index = 0 ) const;
    void                            clearLogic( const uint index = 0 );
    inline CBlendStateConfig&       getConfig( const uint index = 0 ) { return( m_configs[ index & 7 ] ); };
    inline const CBlendStateConfig& getConfig( const uint index = 0 ) const { return( m_configs[ index & 7 ] ); };
    int                             compare( const CMultiBlendStateDesc& desc ) const;
    inline int                      compare( const CBlendStateDesc& desc ) const { return( reinterpret_cast< const CBlendStateDesc* >( this )->compare( desc ) ); };
    inline CMultiBlendStateDesc&    operator=( const CBlendStateDesc& desc ) { set( desc ); return( *this ); };
    inline CMultiBlendStateDesc&    operator=( const BLEND_STATE_DEF& def ) { set( def ); return( *this ); };
    inline CMultiBlendStateDesc&    operator=( const MULTI_BLEND_STATE_DEF& def ) { set( def ); return( *this ); };
    inline CMultiBlendStateDesc&    operator=( const BLEND_CONFIG_STATE_DEF& configDef ) { set( configDef ); return( *this ); };
    inline CMultiBlendStateDesc&    operator=( const BLEND_TARGET_STATE_DEF& targetDef ) { set( targetDef ); return( *this ); };
private:
    uint32_t                        m_desc;
    CBlendStateConfig               m_configs[ 8 ];
    uint32_t                        m_logicOps;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Default state configurations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern CSamplerDesc         DEFAULT_SAMPLER_STATE_DESC;
extern CRasterStateDesc     DEFAULT_RASTER_STATE_DESC;
extern CDepthStateDesc      DEFAULT_DEPTH_STATE_DESC;
extern CBlendStateDesc      DEFAULT_BLEND_STATE_DESC;
extern CMultiBlendStateDesc DEFAULT_MULTIBLEND_STATE_DESC;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Standard depth state configuration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern CDepthStateDesc      STANDARD_DEPTH_STATE_DESC;

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

#endif  //  #ifndef __PUBLIC_STATE_DESCS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
