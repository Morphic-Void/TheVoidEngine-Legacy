
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   shader_parsing.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Parsing of .json shader definitions.
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

#include "shader_parsing.h"
#include "stage_parsing.h"
#include "layout_parsing.h"
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
////    ShaderParser : shader description parsing and collection class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderParser::init( StringTableBlob& strings, const StageParser& stages, const StreamParser& streams, const LinkageParser& linkages, const AssemblyParser& assemblies, const uint count )
{
    kill();
    uint size = ( ( count < 4 ) ? 4 : count );
    if( ShaderParserBase::init( strings, size ) )
    {
        m_stages = &stages;
        m_streams = &streams;
        m_linkages = &linkages;
        m_assemblies = &assemblies;
        return( true );
    }
    kill();
    return( false );
}

bool ShaderParser::parseArray( const QueryTree& queryTree, const uint32_t root )
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

bool ShaderParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: Shader malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A Shader named \"%s\" already exists", name );
        return( false );
    }
    ShaderLink shaderLink;
    shaderLink.nameHandle = m_strings->addString( name );
    shaderLink.chainIndex = -1;
    shaderLink.assemblyIndex = -1;
    shaderLink.streamOutIndex = -1;
    shaderLink.indicesNameHandle = 0;
    shaderLink.indirectNameHandle = 0;
    shaderLink.renderTargetCount = 0;
    for( uint targetIndex = 0; targetIndex < 8; ++targetIndex )
    {
        shaderLink.renderTargetNameHandles[ targetIndex ] = 0;
    }
    shaderLink.depthStencilNameHandle = 0;
    shaderLink.depthStateNameHandle = 0;
    shaderLink.rasterStateNameHandle = 0;
    shaderLink.blendStateNameHandle = 0;
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findShaderConfigToken( key ) )
        {
            case( EShaderConfigStages ):
            {
                const char* stagesName = queryTree.GetText( item );
                if( stagesName != NULL )
                {
                    parsed = true;
                    shaderLink.chainIndex = m_stages->find( stagesName );
                    if( shaderLink.chainIndex < 0 )
                    {
                        LOG( "Referenced stages \"%s\" not found", stagesName );
                        success = false;
                    }
                }
                break;
            }
            case( EShaderConfigAssembly ):
            {
                const char* assemblyName = queryTree.GetText( item );
                if( assemblyName != NULL )
                {
                    parsed = true;
                    shaderLink.assemblyIndex = m_assemblies->find( assemblyName );
                    if( shaderLink.assemblyIndex < 0 )
                    {
                        LOG( "Referenced vertex assembly \"%s\" not found", assemblyName );
                        success = false;
                    }
                }
                break;
            }
            case( EShaderConfigStreamOut ):
            {
                const char* assemblyName = queryTree.GetText( item );
                if( assemblyName != NULL )
                {
                    parsed = true;
                    shaderLink.streamOutIndex = m_assemblies->find( assemblyName );
                    if( shaderLink.streamOutIndex < 0 )
                    {
                        LOG( "Referenced stream-out assembly \"%s\" not found", assemblyName );
                        success = false;
                    }
                }
                break;
            }
            case( EShaderConfigRenderTargets ):
            {
                shaderLink.renderTargetCount = 0;
                for( uint32_t list = queryTree.GetMember( item, uint32_t( 0 ) ); list; list = queryTree.GetNext( list ) )
                {
                    parsed = false;
                    if( shaderLink.renderTargetCount > 7 )
                    {
                        LOG( "More than the maximum of 8 render-targets are specified" );
                        break;
                    }
                    const char* renderTargetName = queryTree.GetText( list );
                    if( renderTargetName == NULL )
                    {
                        break;
                    }
                    shaderLink.renderTargetNameHandles[ shaderLink.renderTargetCount ] = m_strings->addString( renderTargetName );
                    ++shaderLink.renderTargetCount;
                    parsed = true;
                }
                break;
            }
            case( EShaderConfigRenderTarget ):
            {
                const char* renderTargetName = queryTree.GetText( item );
                if( renderTargetName != NULL )
                {
                    if( _stricmp( renderTargetName, "none" ) && _stricmp( renderTargetName, "null" ) && _stricmp( renderTargetName, "false" ) && _stricmp( renderTargetName, "disabled" ) )
                    {   //  name is not an ignore directive
                        shaderLink.renderTargetNameHandles[ 0 ] = m_strings->addString( renderTargetName );
                        shaderLink.renderTargetCount = 1;
                    }
                    parsed = true;
                }
                break;
            }
            case( EShaderConfigDepthStencil ):
            {
                const char* depthStencilName = queryTree.GetText( item );
                if( depthStencilName != NULL )
                {
                    if( _stricmp( depthStencilName, "none" ) && _stricmp( depthStencilName, "null" ) && _stricmp( depthStencilName, "false" ) && _stricmp( depthStencilName, "disabled" ) )
                    {   //  name is not an ignore directive
                        shaderLink.depthStencilNameHandle = m_strings->addString( depthStencilName );
                    }
                    parsed = true;
                }
                break;
            }
            case( EShaderConfigDepthState ):
            {
                const char* depthStateName = queryTree.GetText( item );
                if( depthStateName != NULL )
                {
                    shaderLink.depthStateNameHandle = m_strings->addString( depthStateName );
                    parsed = true;
                }
                break;
            }
            case( EShaderConfigRasterState ):
            {
                const char* rasterStateName = queryTree.GetText( item );
                if( rasterStateName != NULL )
                {
                    shaderLink.rasterStateNameHandle = m_strings->addString( rasterStateName );
                    parsed = true;
                }
                break;
            }
            case( EShaderConfigBlendState ):
            {
                const char* blendStateName = queryTree.GetText( item );
                if( blendStateName != NULL )
                {
                    shaderLink.blendStateNameHandle = m_strings->addString( blendStateName );
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
            LOG( "Problem parsing Shader parameter \"%s\"", key );
            success = false;
        }
    }
    if( success )
    {
        if( shaderLink.chainIndex < 0 )
        {
            LOG( "Shaders must specify a shader stages" );
            success = false;
        }
        else
        {
            const ChainLink& chainLink = *m_stages->access( shaderLink.chainIndex );
            if( chainLink.stageFlags & binary::EShaderStageComputeBit )
            {   //  validate as a compute shader
                validateComputeShader( name, shaderLink );
            }
            else if( shaderLink.assemblyIndex < 0 )
            {   //  an input assembly is required if not a compute shader
                LOG( "This shader requires an input assembly" );
                success = false;
            }
            else
            {
                if( !validateInputAssembly( name, shaderLink, chainLink ) )
                {
                    success = false;
                }
                if( !validateOutputAssembly( name, shaderLink, chainLink ) )
                {
                    success = false;
                }
                if( !validatePixelShader( name, shaderLink, chainLink ) )
                {
                    success = false;
                }
            }
        }
    }
    if( !success )
    {
        LOG( "Error: Shader \"%s\" parsing failed", name );
        return( false );
    }
    insert( shaderLink );
    return( true );
}

bool ShaderParser::validateInputAssembly( const char* shaderName, ShaderLink& shaderLink, const ChainLink& chainLink ) const
{
    bool valid = true;
    const AssemblyLink& assemblyLink = *m_assemblies->access( shaderLink.assemblyIndex );
    const AssemblyElementParser& assemblyElements = *assemblyLink.elements;
    const uint8_t* assemblyName = m_strings->getString( assemblyLink.nameHandle );
    if( assemblyLink.linkagesConflict )
    {
        LOG( "The assembly \"%s\" is not valid as an input assembly due to semantic conflicts (it is only valid for geometry shader stream-out)", assemblyName );
        assemblyElements.listConflicts();
        valid = false;
    }
    else
    {
        const StageDesc& vertexStageDesc = *m_stages->accessStage( chainLink.stages[ binary::EShaderStageVertex ] );
        const StageParams& inputs = vertexStageDesc.inputs;
        int inputCount = inputs.size();
        for( int inputIndex = 0; inputIndex < inputCount; ++inputIndex )
        {   //  verify the shader against the assembly
            const StageParam& inputDesc = *inputs.access( inputIndex );
            uint semanticNameLength = 0;
            const uint8_t* semanticName = m_strings->getString( inputDesc.semanticHandle, semanticNameLength );
            if( ( semanticName[ 0 ] != 'S' ) || ( semanticName[ 1 ] != 'V' ) || ( semanticName[ 2 ] != '_' ) )
            {   //  the semantic does not represent system provided data
                if( !assemblyElements.containsSemantic( inputDesc.semanticHandle, inputDesc.semanticIndex ) )
                {
                    LOG( "Vertex shader input semantic not found in input assembly \"%s\" (semantic \"%s\", index %u)", assemblyName, semanticName, inputDesc.semanticIndex );
                    valid = false;
                }
            }
        }
        if( valid )
        {
            bool warn = false;
            int assemblyElementCount = assemblyElements.size();
            for( int assemblyElementIndex = 0; assemblyElementIndex < assemblyElementCount; ++assemblyElementIndex )
            {   //  verify the assembly against the shader
                const AssemblyElement& assemblyElement = *assemblyElements.access( assemblyElementIndex );
                const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
                const LinkageElementParser& linkageElements = *linkageLink.elements;
                int linkageElementCount = linkageElements.size();
                for( int linkageElementIndex = 0; linkageElementIndex < linkageElementCount; ++linkageElementIndex )
                {
                    const LinkageElement& linkageElement = *linkageElements.access( linkageElementIndex );
                    uint semanticNameLength = 0;
                    const uint8_t* semanticName = m_strings->getString( linkageElement.semanticHandle, semanticNameLength );
                    if( inputs.find( semanticName, semanticNameLength, linkageElement.semanticIndex ) < 0 )
                    {
                        LOG( "Input assembly \"%s\" semantic not found in vertex shader (semantic \"%s\", index %u)", assemblyName, semanticName, linkageElement.semanticIndex );
                        warn = true;
                    }
                }
            }
            if( warn )
            {
                LOG( "Warning: Shader \"%s\" input assembly and shader chain are compatible but miss-matched", shaderName );
            }
        }
    }
    return( valid );
}

bool ShaderParser::validateOutputAssembly( const char* shaderName, ShaderLink& shaderLink, const ChainLink& chainLink ) const
{
    bool valid = true;
    if( shaderLink.streamOutIndex >= 0 )
    {   //  validate the stream-out assembly
        if( m_assemblies->assembliesConflict( shaderLink.assemblyIndex, shaderLink.streamOutIndex ) )
        {
            LOG( "Shader \"%s\" input and stream-out assemblies resource bindings conflict", shaderName );
            valid = false;
        }
        else
        {
            const AssemblyLink& assemblyLink = *m_assemblies->access( shaderLink.streamOutIndex );
            const AssemblyElementParser& assemblyElements = *assemblyLink.elements;
            const uint8_t* assemblyName = m_strings->getString( assemblyLink.nameHandle );
            int streamOutStage = binary::EShaderStageVertex;
            int streamOutStageBit = binary::EShaderStageVertexBit;
            const char* streamOutStageName = "vertex";
            if( chainLink.stageFlags & binary::EShaderStageGeometryBit )
            {
                streamOutStage = binary::EShaderStageGeometry;
                streamOutStageBit = binary::EShaderStageGeometryBit;
                streamOutStageName = "geometry";
            }
            else if( chainLink.stageFlags & binary::EShaderStageDomainBit )
            {
                streamOutStage = binary::EShaderStageDomain;
                streamOutStageBit = binary::EShaderStageDomainBit;
                streamOutStageName = "domain";
            }
            const StageDesc& outStageDesc = *m_stages->accessStage( chainLink.stages[ streamOutStage ] );
            const StageParams& outputs = outStageDesc.outputs;
            int assemblyElementCount = assemblyElements.size();
            if( assemblyElementCount > 1 )
            {   //  only valid with geometry shader stream-out
                if( streamOutStage != binary::EShaderStageGeometry )
                {
                    LOG( "Multi-stream stream-out is only valid for geometry shaders" );
                    valid = false;
                }
                if( !assemblyLink.singleElementStreams )
                {
                    LOG( "Multi-stream stream-out requires single element streams" );
                    valid = false;
                }
                if( assemblyElementCount > 4 )
                {
                    LOG( "Multi-stream stream-out is limited to 4 streams" );
                    valid = false;
                }
            }
            if( valid )
            {
                int outputCount = outputs.size();
                for( int outputIndex = 0; outputIndex < outputCount; ++outputIndex )
                {
                    const StageParam& outputDesc = *outputs.access( outputIndex );
                    uint semanticNameLength = 0;
                    const uint8_t* semanticName = m_strings->getString( outputDesc.semanticHandle, semanticNameLength );
                    if( ( semanticName[ 0 ] != 'S' ) || ( semanticName[ 1 ] != 'V' ) || ( semanticName[ 2 ] != '_' ) )
                    {   //  the semantic does not represent system provided data
                        uint semanticIndex = ( outputDesc.semanticIndex & 0x0000ffff );
                        int streamIndex = ( ( outputDesc.semanticIndex >> 16 ) & 0x0000ffff );
                        if( streamIndex >= assemblyElementCount )
                        {
                            LOG( "Stream-out %s shader references resource stream index %u which is not defined in the stream-out assembly", streamOutStageName, streamIndex );
                            valid = false;
                        }
                        else
                        {
                            const AssemblyElement& assemblyElement = *assemblyElements.access( streamIndex );
                            const uint8_t* assemblyElementName = m_strings->getString( assemblyElement.nameHandle );
                            const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
                            const LinkageElementParser& linkageElements = *linkageLink.elements;
                            int linkageElementIndex = linkageElements.findSemantic( outputDesc.semanticHandle, semanticIndex );
                            if( linkageElementIndex < 0 )
                            {
                                LOG( "Stream-out %s shader semantic not found in output assembly \"%s\" (resource stream \"%s\", semantic \"%s\", index %u)", streamOutStageName, assemblyName, assemblyElementName, semanticName, outputDesc.semanticIndex );
                                valid = false;
                            }
                            else
                            {
                                const LinkageElement& linkageElement = *linkageElements.access( linkageElementIndex );
                                const StreamLink& streamLink = *m_streams->access( assemblyElement.streamLinkIndex );
                                const StreamElementParser& streamElements = *streamLink.elements;
                                uint elementNameLength = 0;
                                const uint8_t* elementName = m_strings->getString( linkageElement.nameHandle, elementNameLength );
                                int streamElementIndex = streamElements.find( elementName, elementNameLength );
                                const StreamElement& streamElement = *streamElements.access( streamElementIndex );
                                uint dataSize = 0;
                                BYTE dataMask = outputDesc.mask;
                                if( dataMask )
                                {
                                    while( ( dataMask & 1 ) == 0 ) dataMask >>= 1;
                                    while( dataMask != 0 )
                                    {
                                        dataMask >>= 1;
                                        dataSize += 4;
                                    }
                                }
                                if( streamElement.dataSize != dataSize )
                                {
                                    LOG( "Stream-out %s shader data size mismatch : shader output size = %u, target element size = %u\n    output = semantic \"%s\", index %u\n    target = assembly \"%s\", resource stream \"%s\", element \"%s\"",
                                        streamOutStageName, dataSize, streamElement.dataSize, semanticName, outputDesc.semanticIndex, assemblyName, assemblyElementName, elementName );
                                    valid = false;
                                }
                            }
                        }
                    }
                }
                if( valid )
                {   //  verify the assembly against the shader
                    bool warn = false;
                    for( int assemblyElementIndex = 0; assemblyElementIndex < assemblyElementCount; ++assemblyElementIndex )
                    {
                        const AssemblyElement& assemblyElement = *assemblyElements.access( assemblyElementIndex );
                        const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
                        const LinkageElementParser& linkageElements = *linkageLink.elements;
                        uint streamIndex = ( static_cast< uint >( assemblyElementIndex ) & 0x0000ffff );
                        int linkageElementCount = linkageElements.size();
                        for( int linkageElementIndex = 0; linkageElementIndex < linkageElementCount; ++linkageElementIndex )
                        {
                            const LinkageElement& linkageElement = *linkageElements.access( linkageElementIndex );
                            uint semanticIndex = ( linkageElement.semanticIndex & 0x0000ffff );
                            uint semanticNameLength = 0;
                            const uint8_t* semanticName = m_strings->getString( linkageElement.semanticHandle, semanticNameLength );
                            if( outputs.find( semanticName, semanticNameLength, ( semanticIndex | ( streamIndex << 16 ) ) ) < 0 )
                            {
                                LOG( "Output assembly \"%s\" semantic not found in stream-out %s shader (semantic \"%s\", index %u, stream %u)", assemblyName, streamOutStageName, semanticName, semanticIndex, streamIndex );
                                warn = true;
                            }
                        }
                        if( warn )
                        {
                            LOG( "Warning: Shader \"%s\" output assembly and shader chain are compatible but miss-matched", shaderName );
                        }
                    }
                }
            }
            if( !valid )
            {
                LOG( "Shader \"%s\" is incompatible with stream-out assembly \"%s\"", shaderName, assemblyName );
            }
        }
    }
    else if( chainLink.streamOutOnly )
    {
        LOG( "Shader \"%s\" requires a stream-out assembly", shaderName );
        valid = false;
    }
    return( valid );
}

void ShaderParser::validateComputeShader( const char* shaderName, ShaderLink& shaderLink ) const
{
    bool warn = false;
    if( shaderLink.assemblyIndex >= 0 )
    {
        LOG( "Compute shader: vertex assembly ignored" );
        shaderLink.assemblyIndex = -1;
        warn = true;
    }
    if( shaderLink.streamOutIndex >= 0 )
    {
        LOG( "Compute shader: stream-out assembly ignored" );
        shaderLink.streamOutIndex = -1;
        warn = true;
    }
    if( shaderLink.indicesNameHandle )
    {
        LOG( "Compute shader: index buffer ignored" );
        shaderLink.indicesNameHandle = 0;
        warn = true;
    }
    if( shaderLink.renderTargetCount > 0 )
    {
        LOG( "Compute shader: render-targets ignored" );
        shaderLink.renderTargetCount = 0;
        for( uint targetIndex = 0; targetIndex < 8; ++targetIndex )
        {
            shaderLink.renderTargetNameHandles[ targetIndex ] = 0;
        }
        warn = true;
    }
    if( shaderLink.depthStencilNameHandle )
    {
        LOG( "Compute shader: depth-stencil ignored" );
        shaderLink.depthStencilNameHandle = 0;
        warn = true;
    }
    if( shaderLink.depthStateNameHandle )
    {
        LOG( "Compute shader: depth state ignored" );
        shaderLink.depthStateNameHandle = 0;
        warn = true;
    }
    if( shaderLink.rasterStateNameHandle )
    {
        LOG( "Compute shader: raster state ignored" );
        shaderLink.rasterStateNameHandle = 0;
        warn = true;
    }
    if( shaderLink.blendStateNameHandle )
    {
        LOG( "Compute shader: blend state ignored" );
        shaderLink.blendStateNameHandle = 0;
        warn = true;
    }
    if( warn )
    {
        LOG( "Shader \"%s\" has unused parameters", shaderName );
    }
}

bool ShaderParser::validatePixelShader( const char* shaderName, ShaderLink& shaderLink, const ChainLink& chainLink ) const
{
    bool valid = true;
    bool warn = false;
    if( chainLink.stageFlags & binary::EShaderStagePixelBit )
    {
        const StageDesc& pixelStageDesc = *m_stages->accessStage( chainLink.stages[ binary::EShaderStagePixel ] );
        if( shaderLink.depthStateNameHandle == 0 )
        {
            LOG( "Depth state required and not-specified, using default" );
            shaderLink.depthStateNameHandle = m_strings->addString( "default" );
            warn = true;
        }
        if( shaderLink.rasterStateNameHandle == 0 )
        {
            LOG( "Raster state required and not-specified, using default" );
            shaderLink.rasterStateNameHandle = m_strings->addString( "default" );
            warn = true;
        }
        if( shaderLink.blendStateNameHandle == 0 )
        {
            LOG( "Blend state required and not-specified, using default" );
            shaderLink.blendStateNameHandle = m_strings->addString( "default" );
            warn = true;
        }
        uint usedRenderTargets = 0;
        uint usedDepthTargets = 0;
        const StageParams& outputs = pixelStageDesc.outputs;
        uint svTargetSemanticHandle = m_strings->findString( "SV_Target" );
        uint svDepthSemanticHandle = m_strings->findString( "SV_Depth" );
        int outputCount = outputs.size();
        for( int outputIndex = 0; outputIndex < outputCount; ++outputIndex )
        {   //  validate render-targets and depth-stencil
            const StageParam& outputParam = *outputs.access( outputIndex );
            uint targetCount = ( static_cast< uint >( outputParam.semanticIndex ) + 1 );
            if( ( svTargetSemanticHandle == outputParam.semanticHandle ) && ( usedRenderTargets < targetCount ) )
            {
                usedRenderTargets = targetCount;
            }
            if( ( svDepthSemanticHandle == outputParam.semanticHandle ) && ( usedDepthTargets < targetCount ) )
            {
                usedDepthTargets = targetCount;
            }
        }
        if( shaderLink.renderTargetCount < usedRenderTargets )
        {
            if( usedRenderTargets == 1 )
            {
                LOG( "Render-target required and not-specified, using default" );
                shaderLink.renderTargetCount = 1;
                shaderLink.renderTargetNameHandles[ 0 ] = m_strings->addString( "default" );
                warn = true;
            }
            else
            {
                LOG( "Shader requires %u render-targets, this shader only specifies %u targets", usedRenderTargets, shaderLink.renderTargetCount );
                valid = false;
                warn = true;
            }
        }
        if( shaderLink.depthStencilNameHandle == 0 )
        {
            if( usedDepthTargets > 0 )
            {
                LOG( "Shader explicitly writes to depth, a depth-stencil must be specified" );
                valid = false;
                warn = true;
            }
        }
        if( warn )
        {
            LOG( "Warning: Shader \"%s\" has missing parameters", shaderName );
        }
    }
    else
    {
        if( shaderLink.renderTargetCount > 0 )
        {
            LOG( "No pixel shader: render-targets ignored" );
            shaderLink.renderTargetCount = 0;
            for( uint targetIndex = 0; targetIndex < 8; ++targetIndex )
            {
                shaderLink.renderTargetNameHandles[ targetIndex ] = 0;
            }
            warn = true;
        }
        if( shaderLink.depthStencilNameHandle )
        {
            LOG( "No pixel shader: depth-stencil ignored" );
            shaderLink.depthStencilNameHandle = 0;
        }
        if( shaderLink.depthStateNameHandle )
        {
            LOG( "No pixel shader: depth state ignored" );
            shaderLink.depthStateNameHandle = 0;
            warn = true;
        }
        if( shaderLink.rasterStateNameHandle )
        {
            LOG( "No pixel shader: raster state ignored" );
            shaderLink.rasterStateNameHandle = 0;
            warn = true;
        }
        if( shaderLink.blendStateNameHandle )
        {
            LOG( "No pixel shader: blend state ignored" );
            shaderLink.blendStateNameHandle = 0;
            warn = true;
        }
        if( warn )
        {
            LOG( "Shader \"%s\" has unused parameters", shaderName );
        }
    }
    return( valid );
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

