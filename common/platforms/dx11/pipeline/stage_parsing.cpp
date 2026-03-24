
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   stage_parsing.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Parsing of .json shader stages and binary shader files.
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
#include "platforms/dx11/rendering/system/private/resource_types.h"
#include "libs/system/system.h"
#include "libs/parsers/parsers.h"
#include "parsing_tokens.h"
#include "stage_parsing.h"

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
////    StageParser : shader chain parsing and collection class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool StageParser::init( StringTableBlob& strings, Blob& binaries, BoundResources& resources, const uint count )
{
    kill();
    m_files.init( 4096 );
    m_stageDescs.init( count << 1 );
    if( ChainLinks::init( strings, count ) && m_stageLinks.init( m_files, ( count << 1 ) ) )
    {
        m_binaries = &binaries;
        m_resources = &resources;
        return( true );
    }
    kill();
    return( false );
}


const StageDesc* StageParser::accessStage( const int index ) const
{
    if( valid() )
    {
        const StageLink* stageLink = m_stageLinks.access( index );
        if( stageLink != NULL )
        {
            return( stageLink->stageDesc );
        }
    }
    return( NULL );
}

bool StageParser::parseArray( const QueryTree& queryTree, const uint32_t root )
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

bool StageParser::parse( const QueryTree& queryTree, const uint32_t root )
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
    ChainLink chainLink;
    chainLink.nameHandle = m_strings->addString( name );
    chainLink.streamOutOnly = false;
    chainLink.streamOutStage = -1;
    chainLink.stageFlags = 0;
    for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
    {
        chainLink.stages[ stageIndex ] = -1;
    }
    m_streamOutOnly = false;
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findStagesConfigToken( key ) )
        {
            case( EStagesConfigCompute ):
            {
                const binary::EShaderStage stage = binary::EShaderStageCompute;
                chainLink.stages[ stage ] = addStage( queryTree.GetText( item ) );
                if( chainLink.stages[ stage ] >= 0 )
                {
                    chainLink.stageFlags |= ( 1 << stage );
                    parsed = true;
                }
                break;
            }
            case( EStagesConfigPixel ):
            {
                const binary::EShaderStage stage = binary::EShaderStagePixel;
                chainLink.stages[ stage ] = addStage( queryTree.GetText( item ) );
                if( chainLink.stages[ stage ] >= 0 )
                {
                    chainLink.stageFlags |= ( 1 << stage );
                    parsed = true;
                }
                break;
            }
            case( EStagesConfigVertex ):
            {
                const binary::EShaderStage stage = binary::EShaderStageVertex;
                chainLink.stages[ stage ] = addStage( queryTree.GetText( item ) );
                if( chainLink.stages[ stage ] >= 0 )
                {
                    chainLink.stageFlags |= ( 1 << stage );
                    parsed = true;
                }
                break;
            }
            case( EStagesConfigHull ):
            {
                const binary::EShaderStage stage = binary::EShaderStageHull;
                chainLink.stages[ stage ] = addStage( queryTree.GetText( item ) );
                if( chainLink.stages[ stage ] >= 0 )
                {
                    chainLink.stageFlags |= ( 1 << stage );
                    parsed = true;
                }
                break;
            }
            case( EStagesConfigDomain ):
            {
                const binary::EShaderStage stage = binary::EShaderStageDomain;
                chainLink.stages[ stage ] = addStage( queryTree.GetText( item ) );
                if( chainLink.stages[ stage ] >= 0 )
                {
                    chainLink.stageFlags |= ( 1 << stage );
                    parsed = true;
                }
                break;
            }
            case( EStagesConfigGeometry ):
            {
                const binary::EShaderStage stage = binary::EShaderStageGeometry;
                chainLink.stages[ stage ] = addStage( queryTree.GetText( item ) );
                if( chainLink.stages[ stage ] >= 0 )
                {
                    chainLink.stageFlags |= ( 1 << stage );
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
            LOG( "Problem parsing Stages parameter \"%s\"", key );
            success = false;
        }
    }
    if( success )
    {
        chainLink.streamOutOnly = m_streamOutOnly;
        switch( chainLink.stageFlags )
        {   //  validate the shader chain linkage
        case( 0 ):
            {
                LOG( "No valid shader stages were specified" );
                success = false;
                break;
            }
        case( binary::EShaderStageHullBit ):
        case( binary::EShaderStageDomainBit ):
        case( binary::EShaderStageGeometryBit ):
        case( binary::EShaderStagePixelBit ):
        case( binary::EShaderStageDomainBit | binary::EShaderStageGeometryBit ):
        case( binary::EShaderStageHullBit | binary::EShaderStageGeometryBit ):
        case( binary::EShaderStageHullBit | binary::EShaderStageDomainBit ):
        case( binary::EShaderStageHullBit | binary::EShaderStageDomainBit | binary::EShaderStageGeometryBit ):
        case( binary::EShaderStagePixelBit | binary::EShaderStageGeometryBit ):
        case( binary::EShaderStagePixelBit | binary::EShaderStageDomainBit ):
        case( binary::EShaderStagePixelBit | binary::EShaderStageDomainBit | binary::EShaderStageGeometryBit ):
        case( binary::EShaderStagePixelBit | binary::EShaderStageHullBit ):
        case( binary::EShaderStagePixelBit | binary::EShaderStageHullBit | binary::EShaderStageGeometryBit ):
        case( binary::EShaderStagePixelBit | binary::EShaderStageHullBit | binary::EShaderStageDomainBit ):
        case( binary::EShaderStagePixelBit | binary::EShaderStageHullBit | binary::EShaderStageDomainBit | binary::EShaderStageGeometryBit ):
            {
                LOG( "Either a compute or vertex shader stage is required" );
                success = false;
                break;
            }
        case( binary::EShaderStageVertexBit | binary::EShaderStageHullBit ):
        case( binary::EShaderStageVertexBit | binary::EShaderStageDomainBit ):
        case( binary::EShaderStageVertexBit | binary::EShaderStageHullBit | binary::EShaderStageGeometryBit ):
        case( binary::EShaderStageVertexBit | binary::EShaderStageDomainBit | binary::EShaderStageGeometryBit ):
        case( binary::EShaderStageVertexBit | binary::EShaderStagePixelBit | binary::EShaderStageDomainBit ):
        case( binary::EShaderStageVertexBit | binary::EShaderStagePixelBit | binary::EShaderStageGeometryBit | binary::EShaderStageDomainBit ):
        case( binary::EShaderStageVertexBit | binary::EShaderStagePixelBit | binary::EShaderStageHullBit ):
        case( binary::EShaderStageVertexBit | binary::EShaderStagePixelBit | binary::EShaderStageGeometryBit | binary::EShaderStageHullBit ):
            {
                LOG( "Hull and Domain shader stages must be paired" );
                success = false;
                break;
            }
        case( binary::EShaderStageComputeBit ):
            {   //  compute shader
                break;
            }
        case( binary::EShaderStageVertexBit ):
            {   //  vertex shader stream-out
                chainLink.streamOutOnly = true;
                chainLink.streamOutStage = binary::EShaderStageVertex;
                break;
            }
        case( binary::EShaderStageVertexBit | binary::EShaderStageHullBit | binary::EShaderStageDomainBit ):
            {   //  domain shader stream-out
                chainLink.streamOutOnly = true;
                chainLink.streamOutStage = binary::EShaderStageDomain;
                if( !validate( chainLink.stages[ binary::EShaderStageVertex ], chainLink.stages[ binary::EShaderStageHull ] ) )
                {
                    LOG( "Vertex->Hull stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageHull ], chainLink.stages[ binary::EShaderStageDomain ] ) )
                {
                    LOG( "Hull->Domain stage linkage is invalid" );
                    success = false;
                }
                break;
            }
        case( binary::EShaderStageVertexBit | binary::EShaderStageGeometryBit ):
            {   //  geometry shader stream-out
                chainLink.streamOutOnly = true;
                chainLink.streamOutStage = binary::EShaderStageGeometry;
                if( !validate( chainLink.stages[ binary::EShaderStageVertex ], chainLink.stages[ binary::EShaderStageGeometry ] ) )
                {
                    LOG( "Vertex->Geometry stage linkage is invalid" );
                    success = false;
                }
                break;
            }
        case( binary::EShaderStageVertexBit | binary::EShaderStageHullBit | binary::EShaderStageDomainBit | binary::EShaderStageGeometryBit ):
            {   //  geometry shader stream-out
                chainLink.streamOutOnly = true;
                chainLink.streamOutStage = binary::EShaderStageGeometry;
                if( !validate( chainLink.stages[ binary::EShaderStageVertex ], chainLink.stages[ binary::EShaderStageHull ] ) )
                {
                    LOG( "Vertex->Hull stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageHull ], chainLink.stages[ binary::EShaderStageDomain ] ) )
                {
                    LOG( "Hull->Domain stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageDomain ], chainLink.stages[ binary::EShaderStageGeometry ] ) )
                {
                    LOG( "Domain->Geometry stage linkage is invalid" );
                    success = false;
                }
                break;
            }
        case( binary::EShaderStageVertexBit | binary::EShaderStagePixelBit ):
            {
                chainLink.streamOutStage = binary::EShaderStageVertex;
                if( !validate( chainLink.stages[ binary::EShaderStageVertex ], chainLink.stages[ binary::EShaderStagePixel ] ) )
                {
                    LOG( "Vertex->Pixel stage linkage is invalid" );
                    success = false;
                }
                break;
            }
        case( binary::EShaderStageVertexBit | binary::EShaderStageGeometryBit | binary::EShaderStagePixelBit ):
            {
                chainLink.streamOutStage = binary::EShaderStageGeometry;
                if( !validate( chainLink.stages[ binary::EShaderStageVertex ], chainLink.stages[ binary::EShaderStageGeometry ] ) )
                {
                    LOG( "Vertex->Geometry stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageGeometry ], chainLink.stages[ binary::EShaderStagePixel ] ) )
                {
                    LOG( "Geometry->Pixel stage linkage is invalid" );
                    success = false;
                }
                break;
            }
        case( binary::EShaderStageVertexBit | binary::EShaderStageHullBit | binary::EShaderStageDomainBit | binary::EShaderStagePixelBit ):
            {
                chainLink.streamOutStage = binary::EShaderStageDomain;
                if( !validate( chainLink.stages[ binary::EShaderStageVertex ], chainLink.stages[ binary::EShaderStageHull ] ) )
                {
                    LOG( "Vertex->Hull stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageHull ], chainLink.stages[ binary::EShaderStageDomain ] ) )
                {
                    LOG( "Hull->Domain stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageDomain ], chainLink.stages[ binary::EShaderStagePixel ] ) )
                {
                    LOG( "Domain->Pixel stage linkage is invalid" );
                    success = false;
                }
                break;
            }
        case( binary::EShaderStageVertexBit | binary::EShaderStageHullBit | binary::EShaderStageDomainBit | binary::EShaderStageGeometryBit | binary::EShaderStagePixelBit ):
            {
                chainLink.streamOutStage = binary::EShaderStageGeometry;
                if( !validate( chainLink.stages[ binary::EShaderStageVertex ], chainLink.stages[ binary::EShaderStageHull ] ) )
                {
                    LOG( "Vertex->Hull stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageHull ], chainLink.stages[ binary::EShaderStageDomain ] ) )
                {
                    LOG( "Hull->Domain stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageDomain ], chainLink.stages[ binary::EShaderStageGeometry ] ) )
                {
                    LOG( "Domain->Geometry stage linkage is invalid" );
                    success = false;
                }
                else if( !validate( chainLink.stages[ binary::EShaderStageGeometry ], chainLink.stages[ binary::EShaderStagePixel ] ) )
                {
                    LOG( "Geometry->Pixel stage linkage is invalid" );
                    success = false;
                }
                break;
            }
        default:
            {
                LOG( "Compute shaders cannot be specified in combination with any other stages" );
                success = false;
                break;
            }
        }
    }
    if( !success )
    {
        LOG( "Error: Shader \"%s\" parsing failed", name );
        return( false );
    }
    insert( chainLink );
    return( true );
}

void StageParser::cleanUp()
{
    m_binaries = NULL;
    m_resources = NULL;
    m_files.kill();
    m_stageDescs.kill();
    m_stageLinks.kill();
}

int StageParser::addStage( const char* const file_name )
{
    int shaderHandle = -1;
    if( file_name != NULL )
    {
        StageLink stageLink;
        memset( &stageLink, 0, sizeof( stageLink ) );
        stageLink.nameHandle = m_files.addString( file_name );
        shaderHandle = m_stageLinks.find( file_name );
        if( shaderHandle < 0 )
        {   //  this is a new shader file
            size_t binarySize = 0;
            void* binaryData = file::loadFile( file_name, binarySize );
            if( binaryData )
            {   //  the file loaded, now check if it is identical to any of our existing binaries
                ID3DBlob* strippedBlob = NULL;
                if( D3DStripShader( binaryData, binarySize, ( D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_TEST_BLOBS | D3DCOMPILER_STRIP_PRIVATE_DATA ), &strippedBlob ) == S_OK )
                {
                    size_t strippedSize = strippedBlob->GetBufferSize();
                    void* strippedData = strippedBlob->GetBufferPointer();
                    shaderHandle = findBinary( strippedData, strippedSize );
                    if( shaderHandle < 0 )
                    {
                        stageLink.stageDesc = &m_stageDescs.fetch();
                        stageLink.stageDesc->binaryBlob.size = static_cast< uint32_t >( strippedSize );
                        stageLink.stageDesc->binaryBlob.offset = static_cast< uint32_t >( m_binaries->append( strippedData, static_cast< uint >( stageLink.stageDesc->binaryBlob.size ) ) );
                        ID3D11ShaderReflection* reflection = NULL;
                        if( D3D11Reflect( binaryData, binarySize, &reflection ) == S_OK )
                        {
                            if( reflect( reflection, *stageLink.stageDesc ) )
                            {
                                shaderHandle = m_stageLinks.insert( stageLink );
                            }
                            else
                            {
                                LOG( "Failed to reflect binary shader file \"%s\"", file_name );
                            }
                            reflection->Release();
                        }
                        else
                        {
                            LOG( "D3D11Reflect() failed processing binary shader file \"%s\"", file_name );
                        }
                    }
                    strippedBlob->Release();
                    strippedBlob = NULL;
                }
                else
                {
                    LOG( "D3DStripShader() failed processing binary shader file \"%s\"", file_name );
                }
                delete[] binaryData;
            }
            else
            {
                LOG( "Failed to load binary shader file \"%s\"", file_name );
            }
        }
    }
    else
    {
        LOG( "Unable to process shader with no file name" );
    }
    return( shaderHandle );
}

int StageParser::findBinary( const void* const binaryData, const size_t binarySize ) const
{
    uint32_t blobSize = static_cast< uint32_t >( binarySize );
    const uint8_t* buffer = m_binaries->getBuffer();
    int stageCount = m_stageLinks.size();
    for( int stageIndex = 0; stageIndex < stageCount; ++stageIndex )
    {
        const StageLink& stageLink = *m_stageLinks.access( stageIndex );
        if( stageLink.stageDesc->binaryBlob.size == blobSize )
        {   //  binaries are the same size
            if( memcmp( binaryData, ( buffer + stageLink.stageDesc->binaryBlob.offset ), binarySize ) == 0 )
            {   //  binaries are identical
                return( stageIndex );
            }
        }
    }
    return( -1 );
}

bool StageParser::reflect( ID3D11ShaderReflection* const reflection, StageDesc& stageDesc )
{
    stageDesc.inputs.init( *m_strings, 32 );
    stageDesc.outputs.init( *m_strings, 32 );
    stageDesc.bindings.init( *m_resources, 32 );
    D3D11_SHADER_DESC reflectionDesc;
    if( reflection->GetDesc( &reflectionDesc ) != S_OK )
    {
        LOG( "ID3D11ShaderReflection::GetDesc() failed" );
        return( false );
    }
    UINT count, index;
    count = reflectionDesc.InputParameters;
    for( index = 0; index < count; ++index )
    {
        D3D11_SIGNATURE_PARAMETER_DESC d3dParamDesc;
        if( reflection->GetInputParameterDesc( index, &d3dParamDesc ) != S_OK )
        {
            LOG( "ID3D11ShaderReflection::GetInputParameterDesc( %u ) failed", index );
            return( false );
        }
        StageParam param;
        translate( d3dParamDesc, param );
        if( stageDesc.inputs.insert( param ) < 0 )
        {
            LOG( "Duplicate input semantic error (semantic = \"%s\", index = %u)", d3dParamDesc.SemanticName, d3dParamDesc.SemanticIndex );
            return( false );
        }
    }
    count = reflectionDesc.OutputParameters;
    for( index = 0; index < count; ++index )
    {
        D3D11_SIGNATURE_PARAMETER_DESC d3dParamDesc;
        if( reflection->GetOutputParameterDesc( index, &d3dParamDesc ) != S_OK )
        {
            LOG( "ID3D11ShaderReflection::GetOutputParameterDesc( %u ) failed", index );
            return( false );
        }
        StageParam param;
        translate( d3dParamDesc, param );
        if( stageDesc.outputs.insert( param ) < 0 )
        {
            LOG( "Duplicate output semantic error (semantic = \"%s\", index = %u)", d3dParamDesc.SemanticName, d3dParamDesc.SemanticIndex );
            return( false );
        }
        if( d3dParamDesc.Stream > 0 )
        {
            m_streamOutOnly = true;
        }
    }
    count = reflectionDesc.BoundResources;
    for( index = 0; index < count; ++index )
    {
        D3D11_BINDING_LINK d3dBindingLink;
        if( reflection->GetResourceBindingDesc( index, &d3dBindingLink.bindingDesc ) != S_OK )
        {
            LOG( "ID3D11ShaderReflection::GetResourceBindingDesc( %u ) failed", index );
            return( false );
        }
        ID3D11ShaderReflectionConstantBuffer* cbuffer = reflection->GetConstantBufferByName( d3dBindingLink.bindingDesc.Name );
        if( ( cbuffer == NULL ) || ( cbuffer->GetDesc( &d3dBindingLink.bufferDesc ) != S_OK ) )
        {
            memset( &d3dBindingLink.bufferDesc, 0, sizeof( d3dBindingLink.bufferDesc ) );
        }
        StageBinding binding;
        if( !translate( d3dBindingLink, binding ) )
        {
            return( false );
        }
        if( stageDesc.bindings.insert( binding ) < 0 )
        {
            LOG( "Duplicate binding link error (resource \"%s\")", d3dBindingLink.bindingDesc.Name );
            return( false );
        }
    }
    stageDesc.inputs.sort();
    stageDesc.outputs.sort();
    stageDesc.bindings.sort();
    return( true );
}

void StageParser::translate( const D3D11_SIGNATURE_PARAMETER_DESC& d3dParamDesc, StageParam& param )
{
    memset( &param, 0, sizeof( param ) );
    param.semanticHandle = m_strings->addString( d3dParamDesc.SemanticName );
    param.setSemanticIndex( d3dParamDesc.SemanticIndex );
    param.setStreamIndex( d3dParamDesc.Stream );
    param.registerIndex =  d3dParamDesc.Register;
    param.mask = d3dParamDesc.Mask;
    param.readWriteMask = d3dParamDesc.ReadWriteMask;
    param.minPrecision = d3dParamDesc.MinPrecision;
    param.componentType = d3dParamDesc.ComponentType;
    param.d3dName = d3dParamDesc.SystemValueType;
}

bool StageParser::translate( const D3D11_BINDING_LINK& d3dBindingLink, StageBinding& binding )
{
    BoundResource resource;
    memset( &binding, 0, sizeof( binding ) );
    memset( &resource, 0, sizeof( resource ) );
    resource.nameHandle = m_strings->addString( d3dBindingLink.bindingDesc.Name );
    resource.bindCount = d3dBindingLink.bindingDesc.BindCount;
    resource.inputType = d3dBindingLink.bindingDesc.Type;
    resource.inputFlags = d3dBindingLink.bindingDesc.uFlags;
    resource.cbufferType = d3dBindingLink.bufferDesc.Type;
    resource.cbufferFlags = d3dBindingLink.bufferDesc.uFlags;
    resource.srvDimension = d3dBindingLink.bindingDesc.Dimension;
    resource.returnType = d3dBindingLink.bindingDesc.ReturnType;
    resource.size = d3dBindingLink.bufferDesc.Size;
    switch( d3dBindingLink.bindingDesc.Type )
    {
        case( D3D_SIT_SAMPLER ):
        {
            resource.resourceType = rendering::EResourceTypeSampler;
            if( d3dBindingLink.bindingDesc.BindPoint >= MAX_SS )
            {
                ASSERT_FAIL( "ERROR: Sampler \"%s\" binds to slot index %u (maximum slot index is %u)", d3dBindingLink.bindingDesc.Name, d3dBindingLink.bindingDesc.BindPoint, ( MAX_SS - 1 ) );
                return( false );
            }
            break;
        }
        case( D3D_SIT_CBUFFER ):
        {
            resource.resourceType = rendering::EResourceTypeConsts;
            if( d3dBindingLink.bindingDesc.BindPoint >= MAX_CB )
            {
                ASSERT_FAIL( "ERROR: Constant buffer \"%s\" binds to slot index %u (maximum slot index is %u)", d3dBindingLink.bindingDesc.Name, d3dBindingLink.bindingDesc.BindPoint, ( MAX_CB - 1 ) );
                return( false );
            }
            break;
        }
        case( D3D_SIT_TBUFFER ):
        case( D3D_SIT_TEXTURE ):
        case( D3D_SIT_STRUCTURED ):
        case( D3D_SIT_BYTEADDRESS ):
        {
            resource.resourceType = rendering::EResourceTypeSRV;
            if( d3dBindingLink.bindingDesc.NumSamples != 0xffffffff )
            {
                resource.sampleCount = d3dBindingLink.bindingDesc.NumSamples;
            }
            if( d3dBindingLink.bindingDesc.BindPoint >= MAX_SRV )
            {
                ASSERT_FAIL( "ERROR: SRV resource \"%s\" binds to slot index %u (maximum slot index is %u)", d3dBindingLink.bindingDesc.Name, d3dBindingLink.bindingDesc.BindPoint, ( MAX_SRV - 1 ) );
                return( false );
            }
            break;
        }
        case( D3D_SIT_UAV_RWTYPED ):
        case( D3D_SIT_UAV_RWBYTEADDRESS ):
        case( D3D_SIT_UAV_RWSTRUCTURED ):
        case( D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER ):
        case( D3D_SIT_UAV_APPEND_STRUCTURED ):
        case( D3D_SIT_UAV_CONSUME_STRUCTURED ):
        {
            resource.resourceType = rendering::EResourceTypeUAV;
            if( d3dBindingLink.bindingDesc.BindPoint >= MAX_UAV )
            {
                ASSERT_FAIL( "ERROR: UAV resource \"%s\" binds to slot index %u (maximum slot index is %u)", d3dBindingLink.bindingDesc.Name, d3dBindingLink.bindingDesc.BindPoint, ( MAX_UAV - 1 ) );
                return( false );
            }
            break;
        }
        default:
        {
            LOG( "Unknown resource type error (resource \"%s\")", d3dBindingLink.bindingDesc.Name );
            return( false );
        }
    }
    int resourceIndex = m_resources->find( d3dBindingLink.bindingDesc.Name, resource.resourceType );
    if( resourceIndex < 0 )
    {   //  resource does not yet exist
        resourceIndex = m_resources->insert( resource );
    }
    else
    {   //  bindingDesc exists, check that it is exactly the same
        BoundResource& resourceRef = m_resources->getBlob().getItem( resourceIndex );
        if( memcmp( &resource, &resourceRef, sizeof( resource ) ) != 0 )
        {
            LOG( "Bound resource description missmatch (resource \"%s\")", d3dBindingLink.bindingDesc.Name );
            return( false );
        }
    }
    binding.resourceIndex = resourceIndex;
    binding.bindingPoint = static_cast< uint >( d3dBindingLink.bindingDesc.BindPoint );
    binding.resourceType = resource.resourceType;
    return( true );
}

bool StageParser::validate( const int sourceStage, const int targetStage ) const
{   //  validate shader stage linking
    const StageLink* sourceLink = m_stageLinks.access( sourceStage );
    const StageLink* targetLink = m_stageLinks.access( targetStage );
    if( ( sourceLink == NULL ) || ( targetLink == NULL ) )
    {
        LOG( "Error accessing shader reflection for link validation" );
        return( false );
    }
    const StageParams& outputs = sourceLink->stageDesc->outputs;
    const StageParams& inputs = targetLink->stageDesc->inputs;
    int outputCount = outputs.size();
    for( int outputIndex = 0; outputIndex < outputCount; ++outputIndex )
    {   //  compare all the outputs with the inputs of the subsequent stage
        const StageParam& outputParam = *outputs.access( outputIndex );
        uint semanticLength = 0;
        const uint8_t* semanticName = m_strings->getString( outputParam.semanticHandle, semanticLength );
        if( ( semanticName[ 0 ] != 'S' ) || ( semanticName[ 1 ] != 'V' ) || ( semanticName[ 2 ] != '_' ) )
        {   //  the parameter is not system specified
            int inputIndex = inputs.find( semanticName, semanticLength, outputParam.semanticIndex );
            if( inputIndex < 0 )
            {
                LOG( "Shader linkage failed for semantic name \"%s\" index %u", semanticName, outputParam.semanticIndex );
                return( false );
            }
            const StageParam& inputParam = *inputs.access( inputIndex );
            if( inputParam.d3dName != outputParam.d3dName )
            {
                LOG( "Shader linkage failed due to D3D_NAME miss-match for semantic name \"%s\" index %u", semanticName, outputParam.semanticIndex );
                return( false );
            }
            if( inputParam.componentType != outputParam.componentType )
            {
                LOG( "Shader linkage failed due to D3D_REGISTER_COMPONENT_TYPE miss-match for semantic name \"%s\" index %u", semanticName, outputParam.semanticIndex );
                return( false );
            }
            if( inputParam.minPrecision != outputParam.minPrecision )
            {
                LOG( "Shader linkage failed due to D3D_MIN_PRECISION miss-match for semantic name \"%s\" index %u", semanticName, outputParam.semanticIndex );
                return( false );
            }
            if( inputParam.readWriteMask & outputParam.readWriteMask )
            {
                LOG( "Shader linkage failed due to ReadWriteMask component miss-match for semantic name \"%s\" index %u", semanticName, outputParam.semanticIndex );
                return( false );
            }
            if( inputParam.mask != outputParam.mask )
            {
                LOG( "Shader linkage failed due to Mask component miss-match for semantic name \"%s\" index %u", semanticName, outputParam.semanticIndex );
                return( false );
            }
        }
    }
    int inputCount = inputs.size();
    for( int inputIndex = 0; inputIndex < inputCount; ++inputIndex )
    {   //  compare all the inputs with the outputs of the preceeding stage
        const StageParam& inputParam = *inputs.access( inputIndex );
        uint semanticLength = 0;
        const uint8_t* semanticName = m_strings->getString( inputParam.semanticHandle, semanticLength );
        if( ( toupper( semanticName[ 0 ] ) != 'S' ) || ( toupper( semanticName[ 1 ] ) != 'V' ) || ( semanticName[ 2 ] != '_' ) )
        {   //  the parameter is not system specified
            int outputIndex = outputs.find( semanticName, semanticLength, inputParam.semanticIndex );
            if( outputIndex < 0 )
            {
                LOG( "Shader linkage failed for semantic name \"%s\" index %u", semanticName, inputParam.semanticIndex );
                return( false );
            }
            const StageParam& outputParam = *outputs.access( outputIndex );
            if( inputParam.d3dName != outputParam.d3dName )
            {
                LOG( "Shader linkage failed due to D3D_NAME miss-match for semantic name \"%s\" index %u", semanticName, inputParam.semanticIndex );
                return( false );
            }
            if( inputParam.componentType != outputParam.componentType )
            {
                LOG( "Shader linkage failed due to D3D_REGISTER_COMPONENT_TYPE miss-match for semantic name \"%s\" index %u", semanticName, inputParam.semanticIndex );
                return( false );
            }
            if( inputParam.minPrecision != outputParam.minPrecision )
            {
                LOG( "Shader linkage failed due to D3D_MIN_PRECISION miss-match for semantic name \"%s\" index %u", semanticName, inputParam.semanticIndex );
                return( false );
            }
            if( inputParam.readWriteMask & outputParam.readWriteMask )
            {
                LOG( "Shader linkage failed due to ReadWriteMask component miss-match for semantic name \"%s\" index %u", semanticName, inputParam.semanticIndex );
                return( false );
            }
            if( inputParam.mask != outputParam.mask )
            {
                LOG( "Shader linkage failed due to Mask component miss-match for semantic name \"%s\" index %u", semanticName, inputParam.semanticIndex );
                return( false );
            }
        }
    }
    return( true );
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

