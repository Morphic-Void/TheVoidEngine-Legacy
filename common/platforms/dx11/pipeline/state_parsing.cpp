
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   state_parsing.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Parsing of .json state definitions.
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

#include "state_parsing.h"
#include "parsing_tokens.h"
#include "libs/system/system.h"
#include "libs/parsers/parsers.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin pipeline namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace pipeline
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    SamplerParser sampler state parsing and collection class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool SamplerParser::parseArray( const QueryTree& queryTree, const uint32_t root )
{
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        if( !parse( queryTree, item ) )
        {
            return( false );
        }
    }
    return( true );
}

bool SamplerParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: Sampler malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A Sampler named \"%s\" already exists", name );
        return( false );
    }
    rendering::SAMPLER_DEF samplerDef;
    samplerDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findSamplerConfigToken( key ) )
        {
            case( ESamplerConfigSamplingMode ):
            {
                int samplingMode = findSamplingModeToken( queryTree.GetText( item ) );
                if( samplingMode >= 0 )
                {
                    samplerDef.SamplingMode = static_cast< rendering::ESamplingMode >( samplingMode );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigCmpFunc ):
            {
                int cmpFunc = findCmpFuncToken( queryTree.GetText( item ) );
                if( cmpFunc >= 0 )
                {
                    samplerDef.CmpFunc = static_cast< rendering::ECmpFunc >( cmpFunc );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigLinearMin ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    samplerDef.LinearMin = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigLinearMag ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    samplerDef.LinearMag = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigLinearMip ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    samplerDef.LinearMip = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigWrapU ):
            {
                int wrapMode = findWrapModeToken( queryTree.GetText( item ) );
                if( wrapMode >= 0 )
                {
                    samplerDef.WrapU = static_cast< rendering::EWrapMode >( wrapMode );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigWrapV ):
            {
                int wrapMode = findWrapModeToken( queryTree.GetText( item ) );
                if( wrapMode >= 0 )
                {
                    samplerDef.WrapV = static_cast< rendering::EWrapMode >( wrapMode );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigWrapW ):
            {
                int wrapMode = findWrapModeToken( queryTree.GetText( item ) );
                if( wrapMode >= 0 )
                {
                    samplerDef.WrapW = static_cast< rendering::EWrapMode >( wrapMode );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigAnisotropicMax ):
            {
                uint value = 0;
                if( queryTree.GetNumberAsUint( item, value ) )
                {
                    samplerDef.AnisotropicMax = static_cast< UINT >( value );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigBorder ):
            {
                double values[ 4 ] = { 0 };
                if( queryTree.GetNumber( queryTree.GetMember( item, uint32_t( 0 ) ), values[ 0 ] ) &&
                    queryTree.GetNumber( queryTree.GetMember( item, uint32_t( 1 ) ), values[ 1 ] ) &&
                    queryTree.GetNumber( queryTree.GetMember( item, uint32_t( 2 ) ), values[ 2 ] ) &&
                    queryTree.GetNumber( queryTree.GetMember( item, uint32_t( 3 ) ), values[ 3 ] ) )
                {
                    samplerDef.Border[ 0 ] = static_cast< float >( values[ 0 ] );
                    samplerDef.Border[ 1 ] = static_cast< float >( values[ 1 ] );
                    samplerDef.Border[ 2 ] = static_cast< float >( values[ 2 ] );
                    samplerDef.Border[ 3 ] = static_cast< float >( values[ 3 ] );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigMinLOD ):
            {
                double value = 0.0;
                if( queryTree.GetNumber( item, value ) )
                {
                    samplerDef.MinLOD = static_cast< float >( value );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigMaxLOD ):
            {
                double value = 0.0;
                if( queryTree.GetNumber( item, value ) )
                {
                    samplerDef.MaxLOD = static_cast< float >( value );
                    parsed = true;
                }
                break;
            }
            case( ESamplerConfigBiasLOD ):
            {
                double value = 0.0;
                if( queryTree.GetNumber( item, value ) )
                {
                    samplerDef.BiasLOD = static_cast< float >( value );
                    parsed = true;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing SamplerDef parameter \"%s\"", key );
            success = false;
        }
    }
    if( ( samplerDef.Border[ 0 ] != 1.0f ) || ( samplerDef.Border[ 1 ] != 1.0f ) || ( samplerDef.Border[ 2 ] != 1.0f ) || ( samplerDef.Border[ 3 ] != 1.0f ) )
    {
        samplerDef.EnableBorder = TRUE;
    }
    if( ( samplerDef.MinLOD != 0.0f ) || ( samplerDef.MaxLOD != 0.0f ) || ( samplerDef.BiasLOD != 0.0f ) )
    {
        samplerDef.EnableModifyLOD = TRUE;
    }
    if( !success )
    {
        LOG( "Error: Sampler \"%s\" parsing failed", name );
        return( false );
    }
    rendering::CSamplerDesc desc( samplerDef );
    int index = m_descs.find( desc );
    if( index < 0 )
    {
        index = m_descs.insert( desc );
    }
    StateLink stateLink;
    stateLink.nameHandle = m_strings->addString( name );
    stateLink.descIndex = index;
    insert( stateLink );
    return( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DepthStateParser depth-stencil state parsing and collection class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool DepthStateParser::parseArray( const QueryTree& queryTree, const uint32_t root )
{
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        if( !parse( queryTree, item ) )
        {
            return( false );
        }
    }
    return( true );
}

bool DepthStateParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: DepthState malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A DepthState named \"%s\" already exists", name );
        return( false );
    }
    rendering::DEPTH_STENCIL_STATE_DEF depthStencilStateDef;
    depthStencilStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findDepthStencilConfigToken( key ) )
        {
            case( EDepthStencilConfigDepth ):
            {
                parsed = parseDepth( queryTree, item, depthStencilStateDef.DepthDef );
                break;
            }
            case( EDepthStencilConfigStencil ):
            {
                parsed = parseStencil( queryTree, item, depthStencilStateDef.StencilDef );
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing DepthStencilStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    if( !success )
    {
        LOG( "Error: DepthState \"%s\" parsing failed", name );
        return( false );
    }
    rendering::CDepthStateDesc desc( depthStencilStateDef );
    int index = m_descs.find( desc );
    if( index < 0 )
    {
        index = m_descs.insert( desc );
    }
    StateLink stateLink;
    stateLink.nameHandle = m_strings->addString( name );
    stateLink.descIndex = index;
    insert( stateLink );
    return( true );
}

bool DepthStateParser::parseDepth( const QueryTree& queryTree, const uint32_t root, rendering::DEPTH_STATE_DEF& depthStateDef )
{
    depthStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findDepthConfigToken( key ) )
        {
            case( EDepthConfigReadEnable ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    depthStateDef.EnableReads = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( EDepthConfigWriteEnable ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    depthStateDef.EnableWrites = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( EDepthConfigCmpFunc ):
            {
                int cmpFunc = findCmpFuncToken( queryTree.GetText( item ) );
                if( cmpFunc >= 0 )
                {
                    depthStateDef.DepthCmpFunc = static_cast< rendering::ECmpFunc >( cmpFunc );
                    parsed = true;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing DepthStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    return( success );
}

bool DepthStateParser::parseStencil( const QueryTree& queryTree, const uint32_t root, rendering::STENCIL_STATE_DEF& stencilStateDef )
{
    stencilStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findStencilConfigToken( key ) )
        {
            case( EStencilConfigEnable ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    stencilStateDef.EnableStencil = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( EStencilConfigReadMask ):
            {
                uint value = 0;
                if( queryTree.GetNumberAsUint( item, value ) )
                {
                    stencilStateDef.StencilReadMask = static_cast< UINT >( value );
                    parsed = true;
                }
                break;
            }
            case( EStencilConfigWriteMask ):
            {
                uint value = 0;
                if( queryTree.GetNumberAsUint( item, value ) )
                {
                    stencilStateDef.StencilWriteMask = static_cast< UINT >( value );
                    parsed = true;
                }
                break;
            }
            case( EStencilConfigFrontFace ):
            {
                parsed = parseStencilFace( queryTree, item, stencilStateDef.FrontFace );
                break;
            }
            case( EStencilConfigBackFace ):
            {
                parsed = parseStencilFace( queryTree, item, stencilStateDef.BackFace );
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing StencilStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    return( success );
}

bool DepthStateParser::parseStencilFace( const QueryTree& queryTree, const uint32_t root, rendering::STENCIL_FACE_STATE_DEF& stencilFaceStateDef )
{
    stencilFaceStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findStencilFaceConfigToken( key ) )
        {
            case( EStencilFaceConfigCmpFunc ):
            {
                int cmpFunc = findCmpFuncToken( queryTree.GetText( item ) );
                if( cmpFunc >= 0 )
                {
                    stencilFaceStateDef.CmpFunc = static_cast< rendering::ECmpFunc >( cmpFunc );
                    parsed = true;
                }
                break;
            }
            case( EStencilFaceConfigPassBoth ):
            {
                int stencilOp = findStencilOpToken( queryTree.GetText( item ) );
                if( stencilOp >= 0 )
                {
                    stencilFaceStateDef.PassBoth = static_cast< rendering::EStencilOp >( stencilOp );
                    parsed = true;
                }
                break;
            }
            case( EStencilFaceConfigPassStencil ):
            {
                int stencilOp = findStencilOpToken( queryTree.GetText( item ) );
                if( stencilOp >= 0 )
                {
                    stencilFaceStateDef.PassStencil = static_cast< rendering::EStencilOp >( stencilOp );
                    parsed = true;
                }
                break;
            }
            case( EStencilFaceConfigFailStencil ):
            {
                int stencilOp = findStencilOpToken( queryTree.GetText( item ) );
                if( stencilOp >= 0 )
                {
                    stencilFaceStateDef.FailStencil = static_cast< rendering::EStencilOp >( stencilOp );
                    parsed = true;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing StencilFaceStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    return( success );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    RasterStateParser raster state parsing and collection class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RasterStateParser::parseArray( const QueryTree& queryTree, const uint32_t root )
{
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        if( !parse( queryTree, item ) )
        {
            return( false );
        }
    }
    return( true );
}

bool RasterStateParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: RasterState malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A RasterState named \"%s\" already exists", name );
        return( false );
    }
    rendering::RASTER_STATE_DEF rasterStateDef;
    rasterStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findRasterConfigToken( key ) )
        {
            case( ERasterConfigCulling ):
            {
                int cullMode = findCullModeToken( queryTree.GetText( item ) );
                if( cullMode >= 0 )
                {
                    rasterStateDef.Culling = static_cast< rendering::ECullMode >( cullMode );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigSamplesUAV ):
            {
                uint value = 0;
                if( queryTree.GetNumberAsUint( item, value ) )
                {
                    parsed = true;
                    switch( value )
                    {
                        case( 0 ):
                        {
                            rasterStateDef.SamplesUAV = rendering::ESamplesUnforced;
                            break;
                        }
                        case( 1 ):
                        {
                            rasterStateDef.SamplesUAV = rendering::ESamplesForce1;
                            break;
                        }
                        case( 2 ):
                        {
                            rasterStateDef.SamplesUAV = rendering::ESamplesForce2;
                            break;
                        }
                        case( 4 ):
                        {
                            rasterStateDef.SamplesUAV = rendering::ESamplesForce4;
                            break;
                        }
                        case( 8 ):
                        {
                            rasterStateDef.SamplesUAV = rendering::ESamplesForce8;
                            break;
                        }
                        case( 16 ):
                        {
                            rasterStateDef.SamplesUAV = rendering::ESamplesForce16;
                            break;
                        }
                        default:
                        {
                            parsed = false;
                            break;
                        }
                    }
                }
                break;
            }
            case( ERasterConfigClockwise ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    rasterStateDef.AntiClockwise = ( value ? FALSE : TRUE );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigAntiClockwise ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    rasterStateDef.AntiClockwise = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigEnableScissor ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    rasterStateDef.EnableScissor = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigEnableWireframe ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    rasterStateDef.EnableWireframe = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigEnableDepthClip ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    rasterStateDef.EnableDepthClip = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigEnableMultiSample ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    rasterStateDef.EnableMultiSample = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigEnableAntiAliasedLine ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    rasterStateDef.EnableAntiAliasedLine = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigDepthBias ):
            {
                int value = 0;
                if( queryTree.GetNumberAsInt( item, value ) )
                {
                    rasterStateDef.DepthBias = static_cast< INT >( value );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigSlopeBias ):
            {
                double value = 0.0;
                if( queryTree.GetNumber( item, value ) )
                {
                    rasterStateDef.SlopeBias = static_cast< float >( value );
                    parsed = true;
                }
                break;
            }
            case( ERasterConfigClampBias ):
            {
                double value = 0.0;
                if( queryTree.GetNumber( item, value ) )
                {
                    rasterStateDef.ClampBias = static_cast< float >( value );
                    parsed = true;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing RasterStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    if( ( rasterStateDef.DepthBias != 0 ) || ( rasterStateDef.SlopeBias != 0.0f ) || ( rasterStateDef.ClampBias != 0.0f ) )
    {
        rasterStateDef.EnableBiasing = TRUE;
    }
    if( !success )
    {
        LOG( "Error: RasterState \"%s\" parsing failed", name );
        return( false );
    }
    rendering::CRasterStateDesc desc( rasterStateDef );
    int index = m_descs.find( desc );
    if( index < 0 )
    {
        index = m_descs.insert( desc );
    }
    StateLink stateLink;
    stateLink.nameHandle = m_strings->addString( name );
    stateLink.descIndex = index;
    insert( stateLink );
    return( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    BlendStateParser blend state parsing and collection class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BlendStateParser::parseArray( const QueryTree& queryTree, const uint32_t root )
{
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        if( !parse( queryTree, item ) )
        {
            return( false );
        }
    }
    return( true );
}

bool BlendStateParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: BlendState malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A BlendState named \"%s\" already exists", name );
        return( false );
    }
    rendering::MULTI_BLEND_STATE_DEF blendStateDef;
    blendStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findBlendConfigToken( key ) )
        {
            case( EBlendConfigRGBA ):
            {
                parsed = parseEffect( queryTree, item, blendStateDef.BlendTargets[ 0 ].BlendConfig.Colour );
                if( parsed )
                {
                   blendStateDef.BlendTargets[ 0 ].BlendConfig.Alpha = blendStateDef.BlendTargets[ 0 ].BlendConfig.Colour;
                }
                break;
            }
            case( EBlendConfigColour ):
            {
                parsed = parseEffect( queryTree, item, blendStateDef.BlendTargets[ 0 ].BlendConfig.Colour );
                break;
            }
            case( EBlendConfigAlpha ):
            {
                parsed = parseEffect( queryTree, item, blendStateDef.BlendTargets[ 0 ].BlendConfig.Alpha );
                break;
            }
            case( EBlendConfigLogic ):
            {
                parsed = parseLogic( queryTree, item, blendStateDef.BlendTargets[ 0 ].LogicConfig );
                break;
            }
            case( EBlendConfigMask ):
            {
                uint value = 0;
                if( queryTree.GetNumberAsUint( item, value ) )
                {
                    blendStateDef.BlendTargets[ 0 ].BlendConfig.Mask = static_cast< UINT >( value );
                    parsed = true;
                }
                break;
            }
            case( EBlendConfigTargets ):
            {
                parsed = parseTargets( queryTree, item, blendStateDef );
                break;
            }
            case( EBlendConfigEnableA2C ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    blendStateDef.EnableAlphaToCoverage = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing BlendStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    if( blendStateDef.RenderTargetCount < 0 )
    {
        blendStateDef.RenderTargetCount = 1;
    }
    if( !success )
    {
        LOG( "Error: BlendState \"%s\" parsing failed", name );
        return( false );
    }
    rendering::CMultiBlendStateDesc desc( blendStateDef );
    int index = m_descs.find( desc );
    if( index < 0 )
    {
        index = m_descs.insert( desc );
    }
    StateLink stateLink;
    stateLink.nameHandle = m_strings->addString( name );
    stateLink.descIndex = index;
    insert( stateLink );
    return( true );
}

bool BlendStateParser::parseTargets( const QueryTree& queryTree, const uint32_t root, rendering::MULTI_BLEND_STATE_DEF& blendStateDef )
{
    blendStateDef.RenderTargetCount = 0;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        if( blendStateDef.RenderTargetCount > 7 )
        {
            LOG( "More than 8 render targets defined" );
            return( false );
        }
        if( !parseTarget( queryTree, item, blendStateDef.BlendTargets[ blendStateDef.RenderTargetCount ] ) )
        {
            return( false );
        }
        ++blendStateDef.RenderTargetCount;
    }
    for( uint targetIndex = blendStateDef.RenderTargetCount; targetIndex < 8; ++targetIndex )
    {
        blendStateDef.BlendTargets[ targetIndex ].setDefault();
    }
    return( true );
}

bool BlendStateParser::parseTarget( const QueryTree& queryTree, const uint32_t root, rendering::BLEND_TARGET_STATE_DEF& blendTargetStateDef )
{
    blendTargetStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findBlendTargetToken( key ) )
        {
            case( EBlendTargetConfigRGBA ):
            {
                parsed = parseEffect( queryTree, item, blendTargetStateDef.BlendConfig.Colour );
                if( parsed )
                {
                   blendTargetStateDef.BlendConfig.Alpha = blendTargetStateDef.BlendConfig.Colour;
                }
                break;
            }
            case( EBlendTargetConfigColour ):
            {
                parsed = parseEffect( queryTree, item, blendTargetStateDef.BlendConfig.Colour );
                break;
            }
            case( EBlendTargetConfigAlpha ):
            {
                parsed = parseEffect( queryTree, item, blendTargetStateDef.BlendConfig.Alpha );
                break;
            }
            case( EBlendTargetConfigLogic ):
            {
                parsed = parseLogic( queryTree, item, blendTargetStateDef.LogicConfig );
                break;
            }
            case( EBlendTargetConfigMask ):
            {
                uint value = 0;
                if( queryTree.GetNumberAsUint( item, value ) )
                {
                    blendTargetStateDef.BlendConfig.Mask = static_cast< UINT >( value );
                    parsed = true;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing BlendTargetStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    return( success );
}

bool BlendStateParser::parseEffect( const QueryTree& queryTree, const uint32_t root, rendering::BLEND_EFFECT_STATE_DEF& blendEffectStateDef )
{
    blendEffectStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findBlendEffectToken( key ) )
        {
            case( EBlendEffectConfigEnable ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    blendEffectStateDef.Enable = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( EBlendEffectConfigSourceMul ):
            {
                int blendMul = findBlendMulToken( queryTree.GetText( item ) );
                if( blendMul >= 0 )
                {
                    blendEffectStateDef.SourceMul = static_cast< rendering::EBlendMul >( blendMul );
                    parsed = true;
                }
                break;
            }
            case( EBlendEffectConfigTargetMul ):
            {
                int blendMul = findBlendMulToken( queryTree.GetText( item ) );
                if( blendMul >= 0 )
                {
                    blendEffectStateDef.TargetMul = static_cast< rendering::EBlendMul >( blendMul );
                    parsed = true;
                }
                break;
            }
            case( EBlendEffectConfigCombiner ):
            {
                int combiner = findCombinerToken( queryTree.GetText( item ) );
                if( combiner >= 0 )
                {
                    blendEffectStateDef.Combiner = static_cast< rendering::ECombiner >( combiner );
                    parsed = true;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing BlendEffectStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    return( success );
}

bool BlendStateParser::parseLogic( const QueryTree& queryTree, const uint32_t root, rendering::LOGIC_CONFIG_STATE_DEF& blendLogicStateDef )
{
    blendLogicStateDef.setDefault();
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findBlendLogicToken( key ) )
        {
            case( EBlendLogicConfigEnable ):
            {
                bool value = false;
                if( queryTree.GetBool( item, value ) )
                {
                    blendLogicStateDef.EnableOp = ( value ? TRUE : FALSE );
                    parsed = true;
                }
                break;
            }
            case( EBlendLogicConfigOp ):
            {
                int logicOp = findLogicOpToken( queryTree.GetText( item ) );
                if( logicOp >= 0 )
                {
                    blendLogicStateDef.LogicOp = static_cast< rendering::ELogicOp >( logicOp );
                    parsed = true;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        if( !parsed )
        {
            LOG( "Problem parsing BlendLogicStateDef parameter \"%s\"", key );
            success = false;
        }
    }
    return( success );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end pipeline namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace pipeline

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

