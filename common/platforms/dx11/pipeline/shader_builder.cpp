
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   shader_builder.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Processes parsed shaders into binary structures.
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

#include "platforms/dx11/rendering/system/private/resource_types.h"
#include "libs/system/system.h"
#include "libs/compression/m2f.h"
#include "shader_builder.h"

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
////    ShaderBuilder : shader building and collection class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderBuilder::build( const ShaderParsing& parsed, const char* const bin_file )
{
    if( !prepare( parsed ) )
    {
        return( false );
    }
    int shaderCount = parsed.shaders.size();
    for( int shaderIndex = 0; shaderIndex < shaderCount; ++shaderIndex )
    {
        addShader( parsed, shaderIndex );
    }
    optimise();
    finalise();
    bool success = writeBinary( bin_file );
    cleanup();
    return( success );
}

bool ShaderBuilder::prepare( const ShaderParsing& parsed )
{
    uint shaderCount = parsed.stages.getStageCount();
    if( shaderCount == 0 )
    {
        return( false );
    }
    init( shaderCount );
    m_strings.init( 4096 );
    m_binaries.init( 4096 );
    m_blobDescs = new binary::BlobDesc[ shaderCount ];
    memset( m_blobDescs, 0, ( shaderCount * sizeof( binary::BlobDesc ) ) );
    m_resourceItems.init( m_strings, 32 );
    m_resourceIndices.init( 32 );
    m_streamFormats.init( m_strings, 32 );
    m_streamElements.init( 128 );
    m_assemblyLayouts.init( 32 );
    m_assemblyStreams.init( 32 );
    m_assemblyElements.init( 128 );
    m_stagingScatters.init( 32 );
    m_stagingGathers.init( 32 );
    m_stagingIndices.init( 32 );
    m_stageChains.init( m_strings, 32 );
    m_shaderDescs.init( m_strings, 32 );
    m_binaryDescs[ binary::EShaderStageCompute ].init( 32 );
    m_binaryDescs[ binary::EShaderStagePixel ].init( 32 );
    m_binaryDescs[ binary::EShaderStageVertex ].init( 32 );
    m_binaryDescs[ binary::EShaderStageHull ].init( 32 );
    m_binaryDescs[ binary::EShaderStageDomain ].init( 32 );
    m_binaryDescs[ binary::EShaderStageGeometry ].init( 32 );
    m_outputDescs.init( 32 );
    m_inputAssemblyIndices.init( 32 );
    m_outputAssemblyIndices.init( 32 );
    return( true );
}

void ShaderBuilder::cleanup()
{
    kill();
    m_strings.kill();
    m_binaries.kill();
    if( m_blobDescs != NULL )
    {
        delete[] m_blobDescs;
        m_blobDescs = NULL;
    }
    m_resourceItems.kill();
    m_resourceIndices.kill();
    m_streamFormats.kill();
    m_streamElements.kill();
    m_assemblyLayouts.kill();
    m_assemblyStreams.kill();
    m_assemblyElements.kill();
    m_stagingScatters.kill();
    m_stagingGathers.kill();
    m_stagingIndices.kill();
    m_stageChains.kill();
    m_shaderDescs.kill();
    m_binaryDescs[ binary::EShaderStageCompute ].kill();
    m_binaryDescs[ binary::EShaderStagePixel ].kill();
    m_binaryDescs[ binary::EShaderStageVertex ].kill();
    m_binaryDescs[ binary::EShaderStageHull ].kill();
    m_binaryDescs[ binary::EShaderStageDomain ].kill();
    m_binaryDescs[ binary::EShaderStageGeometry ].kill();
    m_outputDescs.kill();
    m_inputAssemblyIndices.kill();
    m_outputAssemblyIndices.kill();
}

void ShaderBuilder::optimise()
{   //  eliminate duplicate data
    {   //  eliminate duplicate StreamElement arrays
        TBlob< binary::StreamFormat >& formatsBlob = m_streamFormats.getBlob();
        uint formatCount = formatsBlob.getUsed();
        for( uint outerIndex = 1; outerIndex < formatCount; ++outerIndex )
        {
            binary::StreamFormat& outerFormat = formatsBlob.getItem( outerIndex );
            for( uint innerIndex = 0; outerIndex < outerIndex; ++innerIndex )
            {   //  check if the StreamElement array referenced by the outer StreamFormat is replicated by a StreamElement array referenced by any previous StreamFormat
                bool match = false;
                binary::StreamFormat& innerFormat = formatsBlob.getItem( innerIndex );
                if( innerFormat.elements.count == outerFormat.elements.count )
                {   //  possible match, now we need to check if the StreamElement array is the same
                    match = true;
                    uint32_t elementCount = innerFormat.elements.count;
                    for( uint32_t elementIndex = 0; elementIndex < elementCount; ++elementIndex )
                    {
                        const binary::StreamElement& outerElement = m_streamElements.getItem( elementIndex + outerFormat.elements.index );
                        const binary::StreamElement& innerElement = m_streamElements.getItem( elementIndex + innerFormat.elements.index );
                        if( ( innerElement.nameHandle != outerElement.nameHandle ) || ( innerElement.format != outerElement.format ) || ( innerElement.size != outerElement.size ) || ( innerElement.offset != outerElement.offset ) )
                        {
                            match = false;
                            break;
                        }
                    }
                }
                if( match )
                {   //  the StreamElement array is a duplicate, so remove the duplicate and remap any higher StreamElement indices
                    m_streamElements.remove( outerFormat.elements.index, outerFormat.elements.count );
                    for( uint shiftIndex = ( outerIndex + 1 ); shiftIndex < formatCount; ++shiftIndex )
                    {
                        binary::StreamFormat& shiftFormat = formatsBlob.getItem( shiftIndex );
                        if( shiftFormat.elements.index > outerFormat.elements.index )
                        {
                            shiftFormat.elements.index -= outerFormat.elements.count;
                        }
                    }
                    outerFormat.elements.index = innerFormat.elements.index;
                    break;
                }
            }
        }
    }
    {   //  eliminate duplicate AssemblyElement arrays
        uint streamCount = m_assemblyStreams.getUsed();
        for( uint outerIndex = 1; outerIndex < streamCount; ++outerIndex )
        {
            binary::AssemblyStream& outerStream = m_assemblyStreams.getItem( outerIndex );
            for( uint innerIndex = 0; outerIndex < outerIndex; ++innerIndex )
            {   //  check if the AssemblyElement array referenced by the outer AssemblyStream is replicated by an AssemblyElement array referenced by any previous AssemblyStream
                bool match = false;
                binary::AssemblyStream& innerStream = m_assemblyStreams.getItem( innerIndex );
                if( innerStream.elements.count == outerStream.elements.count )
                {   //  possible match, now we need to check if the AssemblyElement array is the same
                    match = true;
                    uint32_t elementCount = innerStream.elements.count;
                    for( uint32_t elementIndex = 0; elementIndex < elementCount; ++elementIndex )
                    {
                        const binary::AssemblyElement& outerElement = m_assemblyElements.getItem( elementIndex + outerStream.elements.index );
                        const binary::AssemblyElement& innerElement = m_assemblyElements.getItem( elementIndex + innerStream.elements.index );
                        if( ( innerElement.streamElementIndex != outerElement.streamElementIndex ) || ( innerElement.semanticHandle != outerElement.semanticHandle ) || ( innerElement.semanticIndex != outerElement.semanticIndex ) || ( innerElement.componentIndex != outerElement.componentIndex ) )
                        {
                            match = false;
                            break;
                        }
                    }
                }
                if( match )
                {   //  the AssemblyElement array is a duplicate, so remove the duplicate and remap any higher AssemblyElement indices
                    m_assemblyElements.remove( outerStream.elements.index, outerStream.elements.count );
                    for( uint shiftIndex = ( outerIndex + 1 ); shiftIndex < streamCount; ++shiftIndex )
                    {
                        binary::AssemblyStream& shiftStream = m_assemblyStreams.getItem( shiftIndex );
                        if( shiftStream.elements.index > outerStream.elements.index )
                        {
                            shiftStream.elements.index -= outerStream.elements.count;
                        }
                    }
                    outerStream.elements.index = innerStream.elements.index;
                    break;
                }
            }
        }
    }
    {   //  eliminate duplicate AssemblyStream arrays
        uint layoutCount = m_assemblyLayouts.getUsed();
        for( uint outerIndex = 1; outerIndex < layoutCount; ++outerIndex )
        {
            binary::AssemblyLayout& outerLayout = m_assemblyLayouts.getItem( outerIndex );
            for( uint innerIndex = 0; outerIndex < outerIndex; ++innerIndex )
            {   //  check if the AssemblyStream array referenced by the outer AssemblyLayout is replicated by an AssemblyStream array referenced by any previous AssemblyLayout
                bool match = false;
                binary::AssemblyLayout& innerLayout = m_assemblyLayouts.getItem( innerIndex );
                if( innerLayout.streams.count == outerLayout.streams.count )
                {   //  possible match, now we need to check if the AssemblyStream array is the same
                    match = true;
                    uint32_t streamsCount = innerLayout.streams.count;
                    for( uint32_t streamIndex = 0; streamIndex < streamsCount; ++streamIndex )
                    {
                        const binary::AssemblyStream& outerStream = m_assemblyStreams.getItem( streamIndex + outerLayout.streams.index );
                        const binary::AssemblyStream& innerStream = m_assemblyStreams.getItem( streamIndex + innerLayout.streams.index );
                        if( ( innerStream.streamFormatIndex != outerStream.streamFormatIndex ) || ( innerStream.instanceStepRate != outerStream.instanceStepRate ) || ( innerStream.elements.count != outerStream.elements.count ) || ( innerStream.elements.index != outerStream.elements.index ) )
                        {
                            match = false;
                            break;
                        }
                    }
                }
                if( match )
                {   //  the AssemblyStream array is a duplicate, so remove the duplicate and remap any higher AssemblyElement indices
                    m_assemblyStreams.remove( outerLayout.streams.index, outerLayout.streams.count );
                    for( uint shiftIndex = ( outerIndex + 1 ); shiftIndex < layoutCount; ++shiftIndex )
                    {
                        binary::AssemblyLayout& shiftLayout = m_assemblyLayouts.getItem( shiftIndex );
                        if( shiftLayout.streams.index > outerLayout.streams.index )
                        {
                            shiftLayout.streams.index -= outerLayout.streams.count;
                        }
                    }
                    outerLayout.streams.index = innerLayout.streams.index;
                    break;
                }
            }
        }
    }
    {   //  eliminate duplicate AssemblyLayout items
        uint layoutCount = m_assemblyLayouts.getUsed();
        for( uint outerIndex = 1; outerIndex < layoutCount; ++outerIndex )
        {
            binary::AssemblyLayout& outerLayout = m_assemblyLayouts.getItem( outerIndex );
            for( uint innerIndex = 0; outerIndex < outerIndex; ++innerIndex )
            {   //  check if the outer AssemblyLayout is replicated by any previous AssemblyLayout
                binary::AssemblyLayout& innerLayout = m_assemblyLayouts.getItem( innerIndex );
                if( ( outerLayout.shaderBinaryIndex == innerLayout.shaderBinaryIndex ) && ( outerLayout.rasterisedStreamIndex == innerLayout.rasterisedStreamIndex ) && ( outerLayout.streams.count == innerLayout.streams.count ) && ( outerLayout.streams.index == innerLayout.streams.index ) )
                {   //  we have a duplicate, so remove the duplicate and remap any indices to it or higher
                    m_assemblyLayouts.remove( outerIndex, 1 );
                    for( uint indicesIndex = 0; indicesIndex < 2; ++indicesIndex )
                    {
                        BuildIndices& assemblyIndices = ( indicesIndex ? m_outputAssemblyIndices : m_inputAssemblyIndices );
                        uint* indices = assemblyIndices.getBuffer();
                        uint count = assemblyIndices.getUsed();
                        for( uint index = 0; index < count; ++index )
                        {
                            uint check = indices[ index ];
                            if( check >= outerIndex )
                            {
                                indices[ index ] = ( ( check == outerIndex ) ? innerIndex : ( check - 1 ) );
                            }
                        }
                    }
                    --outerIndex;
                    --layoutCount;
                    break;
                }
            }
        }
    }
    {   //  eliminate duplicate AssemblyIndices items
        TBlob< binary::ShaderDesc >& shaderDescs = m_shaderDescs.getBlob();
        uint shaderDescCount = shaderDescs.getUsed();
        binary::ShaderDesc* shaderDescArray = shaderDescs.getBuffer();
        for( uint assemblyGroupIndex = 0; assemblyGroupIndex < 2; ++assemblyGroupIndex )
        {
            BuildIndices& assemblyIndices = ( assemblyGroupIndex ? m_outputAssemblyIndices : m_inputAssemblyIndices );
            uint assemblyCount = assemblyIndices.getUsed();
            for( uint outerIndex = 1; outerIndex < assemblyCount; ++outerIndex )
            {
                uint outerRef = assemblyIndices.getItem( outerIndex );
                for( uint innerIndex = 0; outerIndex < outerIndex; ++innerIndex )
                {
                    uint innerRef = assemblyIndices.getItem( outerIndex );
                    if( innerRef == outerRef )
                    {   //  found a duplicate reference, so remove the duplicate and remap any indices to it or higher
                        assemblyIndices.remove( outerIndex, 1 );
                        for( uint shaderDescIndex = 0; shaderDescIndex < shaderDescCount; ++shaderDescIndex )
                        {
                            binary::ShaderDesc& shaderDesc = shaderDescArray[ shaderDescIndex ];
                            uint32_t& checkRef = ( assemblyGroupIndex ? shaderDesc.outputIndex : shaderDesc.inputIndex );
                            if( checkRef >= outerRef )
                            {
                                checkRef = ( ( checkRef == outerRef ) ? innerRef : ( checkRef - 1 ) );
                            }
                        }
                        --outerIndex;
                        --assemblyCount;
                        break;
                    }
                }
            }
        }
    }
}

void ShaderBuilder::finalise()
{
    {   //  linearise the sortable arrays and remap indices appropriately for the new order (resourceItems, streamFormats, stageChains, shaderDescs)
        {
            const int* resourceItemRanks = m_resourceItems.rank();
            uint resourceIndexCount = m_resourceIndices.getUsed();
            uint32_t* resourceIndexArray = m_resourceIndices.getBuffer();
            for( uint resourceIndexIndex = 0; resourceIndexIndex < resourceIndexCount; ++resourceIndexIndex )
            {
                resourceIndexArray[ resourceIndexIndex ] = static_cast< uint32_t >( resourceItemRanks[ resourceIndexArray[ resourceIndexIndex ] ] );
            }
            delete[] resourceItemRanks;
        }
        m_resourceItems.sort();
        {
            const int* streamFormatRanks = m_streamFormats.rank();
            uint assemblyStreamCount = m_assemblyStreams.getUsed();
            binary::AssemblyStream* assemblyStreamArray = m_assemblyStreams.getBuffer();
            for( uint assemblyStreamIndex = 0; assemblyStreamIndex < assemblyStreamCount; ++assemblyStreamIndex )
            {
                binary::AssemblyStream& assemblyStream = assemblyStreamArray[ assemblyStreamIndex ];
                assemblyStream.streamFormatIndex = static_cast< uint32_t >( streamFormatRanks[ assemblyStream.streamFormatIndex ] );
            }
            TBlob< binary::ResourceItem >& resourceItems = m_resourceItems.getBlob();
            uint resourceItemCount = resourceItems.getUsed();
            for( uint resourceItemIndex = 0; resourceItemIndex < resourceItemCount; ++resourceItemIndex )
            {
                binary::ResourceItem& resourceItem = resourceItems.getItem( resourceItemIndex );
                if( ( resourceItem.resourceType == rendering::EResourceTypeStream ) || ( resourceItem.resourceType == rendering::EResourceTypeOutput ) )
                {
                    resourceItem.desc.streamFormatIndex = static_cast< uint32_t >( streamFormatRanks[ resourceItem.desc.streamFormatIndex ] );
                }
            }
            delete[] streamFormatRanks;
        }
        m_streamFormats.sort();
        {
            const int* stageChainRanks = m_stageChains.rank();
            TBlob< binary::ShaderDesc >& shaderDescs = m_shaderDescs.getBlob();
            uint shaderDescCount = shaderDescs.getUsed();
            binary::ShaderDesc* shaderDescArray = shaderDescs.getBuffer();
            for( uint shaderDescIndex = 0; shaderDescIndex < shaderDescCount; ++shaderDescIndex )
            {
                binary::ShaderDesc& shaderDesc = shaderDescArray[ shaderDescIndex ];
                shaderDesc.chainIndex = static_cast< uint32_t >( stageChainRanks[ shaderDesc.chainIndex ] );
            }
            delete[] stageChainRanks;
        }
        m_stageChains.sort();
        m_shaderDescs.sort();
    }
    {   //  remap interface indices to final unified index into the internal interface array (shader stages, input assemblies, output assemblies)
        uint32_t stageOffsets[ binary::EShaderStageCount ];
        stageOffsets[ binary::EShaderStageCompute ] = 0;
        stageOffsets[ binary::EShaderStagePixel ] = static_cast< uint32_t >( stageOffsets[ binary::EShaderStageCompute ] + m_binaryDescs[ binary::EShaderStageCompute ].size() );
        stageOffsets[ binary::EShaderStageVertex ] = static_cast< uint32_t >( stageOffsets[ binary::EShaderStagePixel ] + m_binaryDescs[ binary::EShaderStagePixel ].size() );
        stageOffsets[ binary::EShaderStageHull ] = static_cast< uint32_t >( stageOffsets[ binary::EShaderStageVertex ] + m_binaryDescs[ binary::EShaderStageVertex ].size() );
        stageOffsets[ binary::EShaderStageDomain ] = static_cast< uint32_t >( stageOffsets[ binary::EShaderStageHull ] + m_binaryDescs[ binary::EShaderStageHull ].size() );
        stageOffsets[ binary::EShaderStageGeometry ] = static_cast< uint32_t >( stageOffsets[ binary::EShaderStageDomain ] + m_binaryDescs[ binary::EShaderStageDomain ].size() );
        uint32_t outputAssembliesOffset = static_cast< uint32_t >( stageOffsets[ binary::EShaderStageGeometry ] + m_binaryDescs[ binary::EShaderStageGeometry ].size() );
        uint32_t inputAssembliesOffset = static_cast< uint32_t >( outputAssembliesOffset + m_outputAssemblyIndices.getUsed() );
        {   //  remap the StageChain shader stage indices
            TBlob< binary::StageChain >& stageChains = m_stageChains.getBlob();
            uint stageChainCount = stageChains.getUsed();
            binary::StageChain* stageChainArray = stageChains.getBuffer();
            for( uint stageChainIndex = 0; stageChainIndex < stageChainCount; ++stageChainIndex )
            {
                binary::StageChain& stageChain = stageChainArray[ stageChainIndex ];
                for( uint stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                {
                    if( stageChain.shaderIndices[ stageIndex ] != 0xffffffff )
                    {
                        stageChain.shaderIndices[ stageIndex ] += stageOffsets[ stageIndex ];
                    }
                }
            }
        }
        {   //  remap the ShaderDesc assembly indices
            TBlob< binary::ShaderDesc >& shaderDescs = m_shaderDescs.getBlob();
            uint shaderDescCount = shaderDescs.getUsed();
            binary::ShaderDesc* shaderDescArray = shaderDescs.getBuffer();
            for( uint shaderDescIndex = 0; shaderDescIndex < shaderDescCount; ++shaderDescIndex )
            {
                binary::ShaderDesc& shaderDesc = shaderDescArray[ shaderDescIndex ];
                if( shaderDesc.inputIndex != 0xffffffff )
                {
                    shaderDesc.inputIndex += inputAssembliesOffset;
                }
                if( shaderDesc.outputIndex != 0xffffffff )
                {
                    shaderDesc.outputIndex += outputAssembliesOffset;
                }
            }
        }
        {   //  remap the Assembly shader references to the appropriate shader binary indices
            uint assemblyCount = m_assemblyLayouts.getUsed();
            for( uint assemblyIndex = 0; assemblyIndex < assemblyCount; ++assemblyIndex )
            {
                binary::AssemblyLayout& assemblyLayout = m_assemblyLayouts.getItem( assemblyIndex );
                assemblyLayout.shaderBinaryIndex = ( ( assemblyLayout.shaderBinaryIndex & 0x0000ffff ) + stageOffsets[ ( assemblyLayout.shaderBinaryIndex >> 16 ) & 0x0000ffff ] );
            }
        }
    }
    {   //  remap all string handles to their sorted index ready for use with the frozen string table
        const int* indexRanks = m_strings.getIndexRanks();
        {
            TBlob< binary::StreamFormat >& blob = m_streamFormats.getBlob();
            uint count = blob.getUsed();
            for( uint index = 0; index < count; ++index )
            {
                binary::StreamFormat& item = blob.getItem( index );
                item.nameHandle = static_cast< uint32_t >( indexRanks[ m_strings.getIndex( item.nameHandle ) ] );
            }
        }
        {
            uint count = m_streamElements.getUsed();
            for( uint index = 0; index < count; ++index )
            {
                binary::StreamElement& item = m_streamElements.getItem( index );
                item.nameHandle = static_cast< uint32_t >( indexRanks[ m_strings.getIndex( item.nameHandle ) ] );
            }
        }
        {
            uint count = m_assemblyElements.getUsed();
            for( uint index = 0; index < count; ++index )
            {
                binary::AssemblyElement& item = m_assemblyElements.getItem( index );
                item.semanticHandle = static_cast< uint32_t >( indexRanks[ m_strings.getIndex( item.semanticHandle ) ] );
            }
        }
        {
            TBlob< binary::ResourceItem >& blob = m_resourceItems.getBlob();
            uint count = blob.getUsed();
            for( uint index = 0; index < count; ++index )
            {
                binary::ResourceItem& item = blob.getItem( index );
                item.nameHandle = static_cast< uint32_t >( indexRanks[ m_strings.getIndex( item.nameHandle ) ] );
            }
        }
        {
            TBlob< binary::StageChain >& blob = m_stageChains.getBlob();
            uint count = blob.getUsed();
            for( uint index = 0; index < count; ++index )
            {
                binary::StageChain& item = blob.getItem( index );
                item.nameHandle = static_cast< uint32_t >( indexRanks[ m_strings.getIndex( item.nameHandle ) ] );
            }
        }
        {
            TBlob< binary::ShaderDesc >& blob = m_shaderDescs.getBlob();
            uint count = blob.getUsed();
            for( uint index = 0; index < count; ++index )
            {
                binary::ShaderDesc& item = blob.getItem( index );
                item.nameHandle = static_cast< uint32_t >( indexRanks[ m_strings.getIndex( item.nameHandle ) ] );
            }
        }
        delete[] indexRanks;
    }
    {   //  find the maximum staging element count for the run-time setup
        uint32_t maxStagingCount = 0;
        const TBlob< binary::ShaderDesc >& shaderDescs = m_shaderDescs.getBlob();
        uint shaderDescCount = shaderDescs.getUsed();
        for( uint32_t shaderDescIndex = 0; shaderDescIndex < shaderDescCount; ++shaderDescIndex )
        {
            uint32_t stagingCount = shaderDescs.getItem( shaderDescIndex ).stagingCount;
            if( maxStagingCount < stagingCount )
            {
                maxStagingCount = stagingCount;
            }
        }
    }
    {   //  find the maximum input layout element count for the run-time setup
        uint32_t maxElementCount = 0;
        BuildIndices& assemblyIndices = m_inputAssemblyIndices;
        uint assemblyCount = assemblyIndices.getUsed();
        for( uint assemblyIndex = 0; assemblyIndex < assemblyCount; ++assemblyIndex )
        {
            const binary::AssemblyLayout& assemblyLayout = m_assemblyLayouts.getItem( assemblyIndices.getItem( assemblyIndex ) );
            uint32_t elementCount = 0;
            uint32_t assemblyStreamCount = assemblyLayout.streams.count;
            for( uint32_t assemblyStreamIndex = 0; assemblyStreamIndex < assemblyStreamCount; ++assemblyStreamIndex )
            {
                const binary::AssemblyStream& assemblyStream = m_assemblyStreams.getItem( assemblyLayout.streams.index + assemblyStreamIndex );
                elementCount += assemblyStream.elements.count;
            }
            if( maxElementCount < elementCount )
            {
                maxElementCount = elementCount;
            }
        }
        m_maxStreamElements = maxElementCount;
    }
    {   //  find the maximum stream-out element count for the run-time setup (including gap elements)
        uint32_t maxElementCount = 0;
        BuildIndices& assemblyIndices = m_outputAssemblyIndices;
        uint assemblyCount = assemblyIndices.getUsed();
        for( uint assemblyIndex = 0; assemblyIndex < assemblyCount; ++assemblyIndex )
        {
            const binary::AssemblyLayout& assemblyLayout = m_assemblyLayouts.getItem( assemblyIndices.getItem( assemblyIndex ) );
            uint32_t elementCount = 0;
            uint32_t assemblyStreamCount = assemblyLayout.streams.count;
            for( uint32_t assemblyStreamIndex = 0; assemblyStreamIndex < assemblyStreamCount; ++assemblyStreamIndex )
            {
                const binary::AssemblyStream& assemblyStream = m_assemblyStreams.getItem( assemblyLayout.streams.index + assemblyStreamIndex );
                const binary::StreamFormat& streamFormat = *m_streamFormats.access( static_cast< int >( assemblyStream.streamFormatIndex ) );
                uint32_t stride = streamFormat.stride;
                uint32_t offset = 0;
                uint32_t assemblyElementCount = assemblyStream.elements.count;
                for( uint32_t assemblyElementIndex = 0; assemblyElementIndex < assemblyElementCount; ++assemblyElementIndex )
                {
                    const binary::AssemblyElement& assemblyElement = m_assemblyElements.getItem( assemblyStream.elements.index + assemblyElementIndex );
                    const binary::StreamElement& streamElement = m_streamElements.getItem( static_cast< uint >( streamFormat.elements.index + assemblyElement.streamElementIndex ) );
                    if( streamElement.offset != offset )
                    {   //  need to add a gap element
                        ++elementCount;
                    }
                    ++elementCount;
                    offset = ( streamElement.offset + streamElement.size );
                }
                if( stride != offset )
                {   //  need to add a final gap element
                    ++elementCount;
                }
            }
            if( maxElementCount < elementCount )
            {
                maxElementCount = elementCount;
            }
        }
        m_maxStreamOutElements = maxElementCount;
    }
}

bool ShaderBuilder::writeBinary( const char* const bin_file )
{
    uint32_t computeStageCount = static_cast< uint32_t >( m_binaryDescs[ binary::EShaderStageCompute ].size() );
    uint32_t pixelStageCount = static_cast< uint32_t >( m_binaryDescs[ binary::EShaderStagePixel ].size() );
    uint32_t vertexStageCount = static_cast< uint32_t >( m_binaryDescs[ binary::EShaderStageVertex ].size() );
    uint32_t hullStageCount = static_cast< uint32_t >( m_binaryDescs[ binary::EShaderStageHull ].size() );
    uint32_t domainStageCount = static_cast< uint32_t >( m_binaryDescs[ binary::EShaderStageDomain ].size() );
    uint32_t geometryStageCount = static_cast< uint32_t >( m_binaryDescs[ binary::EShaderStageGeometry ].size() );
    uint32_t outputStageCount = static_cast< uint32_t >( m_binaryDescs[ binary::EShaderStageCount ].size() );
    uint32_t shaderBlobsCount = ( computeStageCount + pixelStageCount + vertexStageCount + hullStageCount + domainStageCount + geometryStageCount + outputStageCount );
    uint32_t inputAssembliesCount = static_cast< uint32_t >( m_inputAssemblyIndices.getUsed() );
    uint32_t outputAssembliesCount = static_cast< uint32_t >( m_outputAssemblyIndices.getUsed() );
    uint32_t resourceItemsCount = static_cast< uint32_t >( m_resourceItems.size() );
    uint32_t resourceIndicesCount = static_cast< uint32_t >( m_resourceIndices.getUsed() );
    uint32_t streamFormatsCount = static_cast< uint32_t >( m_streamFormats.size() );
    uint32_t streamElementsCount = static_cast< uint32_t >( m_streamElements.getUsed() );
    uint32_t assemblyLayoutsCount = static_cast< uint32_t >( m_assemblyLayouts.getUsed() );
    uint32_t assemblyStreamsCount = static_cast< uint32_t >( m_assemblyStreams.getUsed() );
    uint32_t assemblyElementsCount = static_cast< uint32_t >( m_assemblyElements.getUsed() );
    uint32_t stagingScattersCount = static_cast< uint32_t >( m_stagingScatters.getUsed() );
    uint32_t stagingGathersCount = static_cast< uint32_t >( m_stagingGathers.getUsed() );
    uint32_t stagingIndicesCount = static_cast< uint32_t >( m_stagingIndices.getUsed() );
    uint32_t stageChainsCount = static_cast< uint32_t >( m_stageChains.size() );
    uint32_t shaderDescsCount = static_cast< uint32_t >( m_shaderDescs.size() );
    uint32_t libraryHeaderSize = static_cast< uint32_t >( sizeof( binary::ShaderLibrary ) );
    uint32_t stringTableSize = static_cast< uint32_t >( m_strings.getFrozenSize() );
    uint32_t shaderBinariesSize = ( ( static_cast< uint32_t >( m_binaries.getUsed() ) + 3 ) & ~3 );
    uint32_t shaderBlobsSize = static_cast< uint32_t >( shaderBlobsCount * sizeof( binary::BlobDesc ) );
    uint32_t resourceItemsSize = static_cast< uint32_t >( resourceItemsCount * sizeof( binary::ResourceItem ) );
    uint32_t resourceIndicesSize = static_cast< uint32_t >( resourceIndicesCount * sizeof( uint32_t ) );
    uint32_t streamFormatsSize = static_cast< uint32_t >( streamFormatsCount * sizeof( binary::StreamFormat ) );
    uint32_t streamElementsSize = static_cast< uint32_t >( streamElementsCount * sizeof( binary::StreamElement ) );
    uint32_t assemblyLayoutsSize = static_cast< uint32_t >( assemblyLayoutsCount * sizeof( binary::AssemblyLayout ) );
    uint32_t assemblyStreamsSize = static_cast< uint32_t >( assemblyStreamsCount * sizeof( binary::AssemblyStream ) );
    uint32_t assemblyElementsSize = static_cast< uint32_t >( assemblyElementsCount * sizeof( binary::AssemblyElement ) );
    uint32_t stagingScattersSize = static_cast< uint32_t >( stagingScattersCount * sizeof( binary::StagingScatter ) );
    uint32_t stagingGathersSize = static_cast< uint32_t >( stagingGathersCount * sizeof( binary::StagingGather ) );
    uint32_t stagingIndicesSize = static_cast< uint32_t >( stagingIndicesCount * sizeof( uint32_t ) );
    uint32_t stageChainsSize = static_cast< uint32_t >( stageChainsCount * sizeof( binary::StageChain ) );
    uint32_t shaderDescsSize = static_cast< uint32_t >( shaderDescsCount * sizeof( binary::ShaderDesc ) );
    uint32_t fileSize = (
        libraryHeaderSize + stringTableSize +
        shaderBinariesSize + shaderBlobsSize +
        resourceItemsSize + resourceIndicesSize +
        streamFormatsSize + streamElementsSize +
        assemblyLayoutsSize + assemblyStreamsSize + assemblyElementsSize +
        stagingScattersSize + stagingGathersSize + stagingIndicesSize +
        stageChainsSize + shaderDescsSize );
    binary::ShaderBinaryPtr binaryPtr;
    binaryPtr.data = new uint32_t[ fileSize >> 2 ];
    memset( binaryPtr.data, 0, fileSize );
    binary::ShaderLibrary& library = *binaryPtr.library;
    library.header.magic = MAKE4CC( 'i', 's', 'S', 'L' );
    library.header.platform = MAKE4CC( 'D', 'X', '1', '1' );
    library.header.version = 0x00010001;
    library.header.fileSize = fileSize;
    library.index.stringTable.size = stringTableSize;
    library.index.stringTable.offset = ( libraryHeaderSize >> 2 );
    library.index.shaderBinaries.size = shaderBinariesSize;
    library.index.shaderBinaries.offset = ( library.index.stringTable.offset + ( stringTableSize >> 2 ) );
    library.index.shaderBlobs.count = shaderBlobsCount;
    library.index.shaderBlobs.offset = ( library.index.shaderBinaries.offset + ( shaderBinariesSize >> 2 ) );
    library.index.resourceItems.count = resourceItemsCount;
    library.index.resourceItems.offset = ( library.index.shaderBlobs.offset + ( shaderBlobsSize >> 2 ) );
    library.index.resourceIndices.count = resourceIndicesCount;
    library.index.resourceIndices.offset = ( library.index.resourceItems.offset + ( resourceItemsSize >> 2 ) );
    library.index.streamFormats.count = streamFormatsCount;
    library.index.streamFormats.offset = ( library.index.resourceIndices.offset + ( resourceIndicesSize >> 2 ) );
    library.index.streamElements.count = streamElementsCount;
    library.index.streamElements.offset = ( library.index.streamFormats.offset + ( streamFormatsSize >> 2 ) );
    library.index.assemblyLayouts.count = assemblyLayoutsCount;
    library.index.assemblyLayouts.offset = ( library.index.streamElements.offset + ( streamElementsSize >> 2 ) );
    library.index.assemblyStreams.count = assemblyStreamsCount;
    library.index.assemblyStreams.offset = ( library.index.assemblyLayouts.offset + ( assemblyLayoutsSize >> 2 ) );
    library.index.assemblyElements.count = assemblyElementsCount;
    library.index.assemblyElements.offset = ( library.index.assemblyStreams.offset + ( assemblyStreamsSize >> 2 ) );
    library.index.stagingScatters.count = stagingScattersCount;
    library.index.stagingScatters.offset = ( library.index.assemblyElements.offset + ( assemblyElementsSize >> 2 ) );
    library.index.stagingGathers.count = stagingGathersCount;
    library.index.stagingGathers.offset = ( library.index.stagingScatters.offset + ( stagingScattersSize >> 2 ) );
    library.index.stagingIndices.count = stagingIndicesCount;
    library.index.stagingIndices.offset = ( library.index.stagingGathers.offset + ( stagingGathersSize >> 2 ) );
    library.index.stageChains.count = stageChainsCount;
    library.index.stageChains.offset = ( library.index.stagingIndices.offset + ( stagingIndicesSize >> 2 ) );
    library.index.shaderDescs.count = shaderDescsCount;
    library.index.shaderDescs.offset = ( library.index.stageChains.offset + ( stageChainsSize >> 2 ) );
    library.setup.interfaceCount = ( shaderBlobsCount + inputAssembliesCount + outputAssembliesCount );
    library.setup.maxStagingCount = m_maxStagingCount;
    library.setup.maxStreamElements = m_maxStreamElements;
    library.setup.maxStreamOutElements = m_maxStreamOutElements;
    library.setup.shaderStages[ binary::EShaderStageCompute ].count = computeStageCount;
    library.setup.shaderStages[ binary::EShaderStagePixel ].count = pixelStageCount;
    library.setup.shaderStages[ binary::EShaderStageVertex ].count = vertexStageCount;
    library.setup.shaderStages[ binary::EShaderStageHull ].count = hullStageCount;
    library.setup.shaderStages[ binary::EShaderStageDomain ].count = domainStageCount;
    library.setup.shaderStages[ binary::EShaderStageGeometry ].count = geometryStageCount;
    library.setup.shaderStages[ binary::EShaderStageCompute ].index = 0;
    library.setup.shaderStages[ binary::EShaderStagePixel ].index = ( library.setup.shaderStages[ binary::EShaderStageCompute ].index + library.setup.shaderStages[ binary::EShaderStageCompute ].count );
    library.setup.shaderStages[ binary::EShaderStageVertex ].index = ( library.setup.shaderStages[ binary::EShaderStagePixel ].index + library.setup.shaderStages[ binary::EShaderStagePixel ].count );
    library.setup.shaderStages[ binary::EShaderStageHull ].index = ( library.setup.shaderStages[ binary::EShaderStageVertex ].index + library.setup.shaderStages[ binary::EShaderStageVertex ].count );
    library.setup.shaderStages[ binary::EShaderStageDomain ].index = ( library.setup.shaderStages[ binary::EShaderStageHull ].index + library.setup.shaderStages[ binary::EShaderStageHull ].count );
    library.setup.shaderStages[ binary::EShaderStageGeometry ].index = ( library.setup.shaderStages[ binary::EShaderStageDomain ].index + library.setup.shaderStages[ binary::EShaderStageDomain ].count );
    library.setup.inputAssemblies.count = inputAssembliesCount;
    library.setup.outputAssemblies.count = outputAssembliesCount;
    library.setup.inputAssemblies.index = 0;
    library.setup.outputAssemblies.index = ( library.setup.inputAssemblies.index + library.setup.inputAssemblies.count );
    m_strings.freeze( &binaryPtr.data[ library.index.stringTable.offset ], stringTableSize );
    memcpy( &binaryPtr.data[ library.index.shaderBinaries.offset ], m_binaries.getBuffer(), m_binaries.getUsed() );
    {
        binary::BlobDesc* shaderBlobs = reinterpret_cast< binary::BlobDesc* >( &binaryPtr.data[ library.index.shaderBlobs.offset ] );
        for( uint stageIndex = 0; stageIndex <= binary::EShaderStageCount; ++stageIndex )
        {
            const TBlob< binary::BlobDesc >& binaryDescs = m_binaryDescs[ stageIndex ].getBlob();
            uint count = binaryDescs.getUsed();
            memcpy( shaderBlobs, binaryDescs.getBuffer(), ( count * sizeof( binary::BlobDesc ) ) );
            shaderBlobs += count;
        }
    }
    memcpy( &binaryPtr.data[ library.index.resourceItems.offset ], m_resourceItems.getBlob().getBuffer(), ( m_resourceItems.size() * sizeof( binary::ResourceItem ) ) );
    memcpy( &binaryPtr.data[ library.index.resourceIndices.offset ], m_resourceIndices.getBuffer(), ( m_resourceIndices.getUsed() * sizeof( uint32_t ) ) );
    memcpy( &binaryPtr.data[ library.index.streamFormats.offset ], m_streamFormats.getBlob().getBuffer(), ( m_streamFormats.size() * sizeof( binary::StreamFormat ) ) );
    memcpy( &binaryPtr.data[ library.index.streamElements.offset ], m_streamElements.getBuffer(), ( m_streamElements.getUsed() * sizeof( binary::StreamElement ) ) );
    {
        binary::AssemblyLayout* assemblyLayouts = reinterpret_cast< binary::AssemblyLayout* >( &binaryPtr.data[ library.index.assemblyLayouts.offset ] );
        for( uint assemblyGroupIndex = 0; assemblyGroupIndex < 2; ++assemblyGroupIndex )
        {
            BuildIndices& assemblyIndices = ( assemblyGroupIndex ? m_outputAssemblyIndices : m_inputAssemblyIndices );
            uint assemblyCount = assemblyIndices.getUsed();
            for( uint assemblyIndex = 0; assemblyIndex < assemblyCount; ++assemblyIndex )
            {
                assemblyLayouts[ 0 ] = m_assemblyLayouts.getItem( assemblyIndices.getItem( assemblyIndex ) );
                ++assemblyLayouts;
            }
        }
    }
    memcpy( &binaryPtr.data[ library.index.assemblyStreams.offset ], m_assemblyStreams.getBuffer(), ( m_assemblyStreams.getUsed() * sizeof( binary::AssemblyStream ) ) );
    memcpy( &binaryPtr.data[ library.index.assemblyElements.offset ], m_assemblyElements.getBuffer(), ( m_assemblyElements.getUsed() * sizeof( binary::AssemblyElement ) ) );
    memcpy( &binaryPtr.data[ library.index.stagingScatters.offset ], m_stagingScatters.getBuffer(), ( m_stagingScatters.getUsed() * sizeof( binary::StagingScatter ) ) );
    memcpy( &binaryPtr.data[ library.index.stagingGathers.offset ], m_stagingGathers.getBuffer(), ( m_stagingGathers.getUsed() * sizeof( binary::StagingGather ) ) );
    memcpy( &binaryPtr.data[ library.index.stagingIndices.offset ], m_stagingIndices.getBuffer(), ( m_stagingIndices.getUsed() * sizeof( uint32_t ) ) );
    memcpy( &binaryPtr.data[ library.index.stageChains.offset ], m_stageChains.getBlob().getBuffer(), ( m_stageChains.size() * sizeof( binary::StageChain ) ) );
    memcpy( &binaryPtr.data[ library.index.shaderDescs.offset ], m_shaderDescs.getBlob().getBuffer(), ( m_shaderDescs.size() * sizeof( binary::ShaderDesc ) ) );
    mangle( &binaryPtr.library[ 1 ], ( fileSize - libraryHeaderSize ) );
    bool success = file::saveFile( bin_file, fileSize, binaryPtr.data );
    delete[] binaryPtr.data;
    if( !success )
    {
        LOG( "ShaderBuilder failed to save binary library file \"%s\"", bin_file );
        return( false );
    }
    return( true );
}

void ShaderBuilder::addShader( const ShaderParsing& parsed, const int shaderIndex )
{
    const ShaderLink& shaderLink = *parsed.shaders.access( shaderIndex );
    uint shaderNameLength = 0;
    const uint8_t* shaderName = parsed.strings.getString( shaderLink.nameHandle, shaderNameLength );
    const ChainInfo* stageChainInfo = addChain( parsed, shaderLink.chainIndex );
    ResourceMapping inputsResourceMap( m_strings, m_resourceItems, 32 );
    ResourceMapping outputsResourceMap( m_strings, m_resourceItems, 32 );
    ResourceMapping generalResourceMap( m_strings, m_resourceItems, 32 );
    ResourceMapping targetsResourceMap( m_strings, m_resourceItems, 8 );
    binary::ShaderDesc shaderDesc;
    memset( &shaderDesc, 0, sizeof( shaderDesc ) );
    shaderDesc.nameHandle = static_cast< uint32_t >( m_strings.addString( shaderName, shaderNameLength ) );
    shaderDesc.inputIndex = ( ( shaderLink.assemblyIndex < 0 ) ? 0xffffffff : addInputAssembly( parsed, shaderLink.assemblyIndex, shaderLink.chainIndex, inputsResourceMap ) );
    shaderDesc.outputIndex = ( ( shaderLink.streamOutIndex < 0 ) ? 0xffffffff : addOutputAssembly( parsed, shaderLink.streamOutIndex, shaderLink.chainIndex, outputsResourceMap ) );
    shaderDesc.chainIndex = stageChainInfo->chainIndex;
    int rasterSource = ( shaderLink.rasterStateNameHandle  ? generalResourceMap.insert( addResource( parsed, shaderLink.rasterStateNameHandle,  rendering::EResourceTypeRasterState ) ) : -1 );
    int depthSource =  ( shaderLink.depthStateNameHandle   ? generalResourceMap.insert( addResource( parsed, shaderLink.depthStateNameHandle,   rendering::EResourceTypeDepthState  ) ) : -1 );
    int blendSource =  ( shaderLink.blendStateNameHandle   ? generalResourceMap.insert( addResource( parsed, shaderLink.blendStateNameHandle,   rendering::EResourceTypeBlendState  ) ) : -1 );
    int dsvSource =    ( shaderLink.depthStencilNameHandle ? generalResourceMap.insert( addResource( parsed, shaderLink.depthStencilNameHandle, rendering::EResourceTypeDSV         ) ) : -1 );
    uint32_t renderTargetCount = static_cast< uint32_t >( shaderLink.renderTargetCount );
    for( uint32_t renderTargetIndex = 0; renderTargetIndex < renderTargetCount; ++renderTargetIndex )
    {   //  add the render targets
        targetsResourceMap.insert( addResource( parsed, shaderLink.renderTargetNameHandles[ renderTargetIndex ], rendering::EResourceTypeRTV ) );
    }
    uint32_t resourceIndex = static_cast< uint32_t >( stageChainInfo->resourceMap.size() );
    uint32_t stagingIndex = stageChainInfo->stagingCount;
    {   //  link the general resources and sort the general resource map
        const int* resourceRanks = generalResourceMap.rank();
        shaderDesc.rasterStateIndex = 0xffffffff;
        if( rasterSource >= 0 )
        {   //  add the raster state
            shaderDesc.rasterStateIndex = ( resourceIndex + static_cast< uint32_t >( resourceRanks[ rasterSource ] ) );
        }
        shaderDesc.depthStateIndex = 0xffffffff;
        if( depthSource >= 0 )
        {   //  add the depth state
            shaderDesc.depthStateIndex = ( resourceIndex + static_cast< uint32_t >( resourceRanks[ depthSource ] ) );
        }
        shaderDesc.blendStateIndex = 0xffffffff;
        if( blendSource >= 0 )
        {   //  add the blend state
            shaderDesc.blendStateIndex = ( resourceIndex + static_cast< uint32_t >( resourceRanks[ blendSource ] ) );
        }
        shaderDesc.dsvIndex = 0xffffffff;
        if( dsvSource >= 0 )
        {   //  add the depth-stencil
            shaderDesc.dsvIndex = ( resourceIndex + static_cast< uint32_t >( resourceRanks[ dsvSource ] ) );
        }
        delete[] resourceRanks;
        generalResourceMap.sort();
        resourceIndex += static_cast< uint32_t >( generalResourceMap.size() );
    }
    shaderDesc.rtvIndex = 0xffffffff;
    if( renderTargetCount )
    {   //  link the render targets and sort the targets resource map
        const int* resourceRanks = targetsResourceMap.rank();
        uint indexBase = m_stagingIndices.getUsed();
        shaderDesc.rtvIndex = static_cast< uint32_t >( resourceIndex );
        shaderDesc.rtvRemap.count = renderTargetCount;
        shaderDesc.rtvRemap.index = static_cast< uint32_t >( indexBase );
        m_stagingIndices.append( NULL, static_cast< uint >( renderTargetCount ) );
        for( uint32_t renderTargetIndex = 0; renderTargetIndex < renderTargetCount; ++renderTargetIndex )
        {
            m_stagingIndices.getItem( static_cast< uint >( resourceRanks[ renderTargetIndex ] ) + indexBase ) = renderTargetIndex;
        }
        delete[] resourceRanks;
        targetsResourceMap.sort();
        resourceIndex += static_cast< uint32_t >( targetsResourceMap.size() );
    }
    uint32_t inputResourceCount = static_cast< uint32_t >( inputsResourceMap.size() );
    if( inputResourceCount )
    {   //  link the input streams and sort the inputs resource map
        const int* resourceRanks = inputsResourceMap.rank();
        uint indexBase = m_stagingIndices.getUsed();
        shaderDesc.streams.source = static_cast< uint32_t >( resourceIndex );
        shaderDesc.streams.target.offset = stagingIndex;
        shaderDesc.streams.target.indices.count = inputResourceCount;
        shaderDesc.streams.target.indices.index = static_cast< uint32_t >( indexBase );
        m_stagingIndices.append( NULL, static_cast< uint >( inputResourceCount ) );
        for( uint32_t inputResourceIndex = 0; inputResourceIndex < inputResourceCount; ++inputResourceIndex )
        {
            m_stagingIndices.getItem( static_cast< uint >( resourceRanks[ inputResourceIndex ] ) + indexBase ) = inputResourceIndex;
        }
        stagingIndex += inputResourceCount;
        delete[] resourceRanks;
        inputsResourceMap.sort();
        resourceIndex += static_cast< uint32_t >( inputsResourceMap.size() );
    }
    uint32_t outputResourceCount = static_cast< uint32_t >( outputsResourceMap.size() );
    if( outputResourceCount )
    {   //  link the output streams and sort the outputs resource map
        const int* resourceRanks = outputsResourceMap.rank();
        uint indexBase = m_stagingIndices.getUsed();
        shaderDesc.outputs.source = static_cast< uint32_t >( resourceIndex );
        shaderDesc.outputs.target.offset = stagingIndex;
        shaderDesc.outputs.target.indices.count = outputResourceCount;
        shaderDesc.outputs.target.indices.index = static_cast< uint32_t >( indexBase );
        m_stagingIndices.append( NULL, static_cast< uint >( outputResourceCount ) );
        for( uint32_t outputResourceIndex = 0; outputResourceIndex < outputResourceCount; ++outputResourceIndex )
        {
            m_stagingIndices.getItem( static_cast< uint >( resourceRanks[ outputResourceIndex ] ) + indexBase ) = outputResourceIndex;
        }
        stagingIndex += outputResourceCount;
        delete[] resourceRanks;
        outputsResourceMap.sort();
        resourceIndex += static_cast< uint32_t >( outputsResourceMap.size() );
    }
    shaderDesc.stagingCount = stagingIndex;
    shaderDesc.resources.count = static_cast< uint32_t >( resourceIndex );
    shaderDesc.resources.index = static_cast< uint32_t >( m_resourceIndices.getUsed() );
    {   //  output the shader resource table
        int resourceCount = stageChainInfo->resourceMap.size();
        for( int resourceIndex = 0; resourceIndex < resourceCount; ++resourceIndex )
        {
            m_resourceIndices.append( *stageChainInfo->resourceMap.access( resourceIndex ) );
        }
    }
    {   //  output the general resource table
        int resourceCount = generalResourceMap.size();
        for( int resourceIndex = 0; resourceIndex < resourceCount; ++resourceIndex )
        {
            m_resourceIndices.append( *generalResourceMap.access( resourceIndex ) );
        }
    }
    {   //  output the targets resource table
        int resourceCount = targetsResourceMap.size();
        for( int resourceIndex = 0; resourceIndex < resourceCount; ++resourceIndex )
        {
            m_resourceIndices.append( *targetsResourceMap.access( resourceIndex ) );
        }
    }
    {   //  output the inputs resource table
        int resourceCount = inputsResourceMap.size();
        for( int resourceIndex = 0; resourceIndex < resourceCount; ++resourceIndex )
        {
            m_resourceIndices.append( *inputsResourceMap.access( resourceIndex ) );
        }
    }
    {   //  output the outputs resource table
        int resourceCount = outputsResourceMap.size();
        for( int resourceIndex = 0; resourceIndex < resourceCount; ++resourceIndex )
        {
            m_resourceIndices.append( *outputsResourceMap.access( resourceIndex ) );
        }
    }
    m_shaderDescs.insert( shaderDesc );
}

const ChainInfo* ShaderBuilder::addChain( const ShaderParsing& parsed, const int chainIndex )
{
    StageChainLink stageChainLink = { chainIndex, NULL };
    int linkIndex = find( stageChainLink );
    if( linkIndex < 0 )
    {   //  this shader has not been added yet
        const ChainLink& chainLink = *parsed.stages.access( chainIndex );
        uint chainNameLength = 0;
        const uint8_t* chainName = parsed.strings.getString( chainLink.nameHandle, chainNameLength );
        ChainInfo& chainInfo = m_chains.fetch();
        chainInfo.chainIndex = 0;
        chainInfo.stagingCount = 0;
        chainInfo.streamOutStage = 0xffffffff;
        chainInfo.resourceMap.init( m_strings, m_resourceItems, 32 );
        binary::StageChain stageChain;
        memset( &stageChain, 0, sizeof( stageChain ) );
        stageChain.nameHandle = static_cast< uint32_t >( m_strings.addString( chainName, chainNameLength ) );
        memset( m_cbMap, -1, sizeof( m_cbMap ) );
        memset( m_srvMap, -1, sizeof( m_srvMap ) );
        memset( m_uavMap, -1, sizeof( m_uavMap ) );
        memset( m_ssMap, -1, sizeof( m_ssMap ) );
        int cbResourceCount = 0;
        int srvResourceCount = 0;
        int uavResourceCount = 0;
        int ssResourceCount = 0;
        stageChain.stageMask = 0;
        for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
        {   //  add shader binaries and resources, map resource indices into the map areas
            stageChain.shaderIndices[ stageIndex ] = 0xffffffff;
            int stageLinkIndex = chainLink.stages[ stageIndex ];
            if( ( stageLinkIndex >= 0 ) && ( ( !chainLink.streamOutOnly ) || ( stageIndex != binary::EShaderStageGeometry ) ) )
            {
                stageChain.stageMask |= ( uint32_t( 1 ) << stageIndex );
                const StageDesc& stageDesc = *parsed.stages.accessStage( stageLinkIndex );
                binary::BlobDesc& blobDesc = m_blobDescs[ stageLinkIndex ];
                if( blobDesc.size == 0 )
                {   //  shader stage has not yet been added to the binaries list
                    blobDesc.size = stageDesc.binaryBlob.size;
                    blobDesc.offset = m_binaries.append( ( parsed.binaries.getBuffer() + stageDesc.binaryBlob.offset ), stageDesc.binaryBlob.size );
                }
                int blobIndex = m_binaryDescs[ stageIndex ].find( blobDesc );
                if( blobIndex < 0 )
                {
                    blobIndex = m_binaryDescs[ stageIndex ].insert( blobDesc );
                }
                stageChain.shaderIndices[ stageIndex ] = static_cast< uint32_t >( blobIndex );
                const TBlob< StageBinding >& shaderBindings = stageDesc.bindings.getBlob();
                uint shaderBindingCount = shaderBindings.getUsed();
                chainInfo.stagingCount += shaderBindingCount;
                for( uint shaderBindingIndex = 0; shaderBindingIndex < shaderBindingCount; ++shaderBindingIndex )
                {
                    const StageBinding& shaderBinding = shaderBindings.getItem( shaderBindingIndex );
                    const BoundResource& boundResource = *parsed.resources.access( shaderBinding.resourceIndex );
                    uint resourceNameLength = 0;
                    const uint8_t* resourceName = parsed.strings.getString( boundResource.nameHandle, resourceNameLength );
                    int resourceIndex = chainInfo.resourceMap.find( resourceName, resourceNameLength, boundResource.resourceType );
                    if( resourceIndex < 0 )
                    {   //  this resource is not already in our resource list
                        int resourceItemIndex = m_resourceItems.find( resourceName, resourceNameLength, boundResource.resourceType );
                        if( resourceItemIndex < 0 )
                        {   //  this resource has not been referenced before, so add it to the main resource collection
                            binary::ResourceItem resourceItem;
                            resourceItem.nameHandle = static_cast< uint32_t >( m_strings.addString( resourceName, resourceNameLength ) );
                            resourceItem.resourceType = static_cast< uint32_t >( boundResource.resourceType );
                            resourceItem.desc.resourceValidation.packed = 0;
                            resourceItem.desc.resourceValidation.setInputType( boundResource.inputType );
                            resourceItem.desc.resourceValidation.setInputFlags( boundResource.inputFlags );
                            resourceItem.desc.resourceValidation.setCBufferType( boundResource.cbufferType );
                            resourceItem.desc.resourceValidation.setCBufferFlags( boundResource.cbufferFlags );
                            resourceItem.desc.resourceValidation.setSRVDimension( boundResource.srvDimension );
                            resourceItem.desc.resourceValidation.setReturnType( boundResource.returnType );
                            resourceItem.desc.resourceValidation.setSampleCount( boundResource.sampleCount );
                            resourceItem.stride = static_cast< uint16_t >( boundResource.size );
                            resourceItem.slots = static_cast< uint16_t >( boundResource.bindCount );
                            resourceItemIndex = m_resourceItems.insert( resourceItem );
                        }
                        resourceIndex = chainInfo.resourceMap.insert( resourceItemIndex );
                        switch( boundResource.resourceType )
                        {
                            case( rendering::EResourceTypeConsts ):
                            {
                                ++cbResourceCount;
                                break;
                            }
                            case( rendering::EResourceTypeSRV ):
                            {
                                ++srvResourceCount;
                                break;
                            }
                            case( rendering::EResourceTypeUAV ):
                            {
                                ++uavResourceCount;
                                break;
                            }
                            default:
                            {   //  binary::EResourceTypeSampler
                                ++ssResourceCount;
                                return( false );
                            }
                        }
                    }
                    switch( boundResource.resourceType )
                    {
                        case( rendering::EResourceTypeConsts ):
                        {
                            m_cbMap[ stageIndex ][ shaderBinding.bindingPoint ].resourceIndex = resourceIndex;
                            break;
                        }
                        case( rendering::EResourceTypeSRV ):
                        {
                            m_srvMap[ stageIndex ][ shaderBinding.bindingPoint ].resourceIndex = resourceIndex;
                            break;
                        }
                        case( rendering::EResourceTypeUAV ):
                        {
                            m_uavMap[ stageIndex ][ shaderBinding.bindingPoint ].resourceIndex = resourceIndex;
                            break;
                        }
                        default:
                        {   //  binary::EResourceTypeSampler
                            m_ssMap[ stageIndex ][ shaderBinding.bindingPoint ].resourceIndex = resourceIndex;
                            return( false );
                        }
                    }
                }
            }
        }
        {   //  setup gathering
            binary::StagingGather stagingGather;
            int stagingIndex = 0;
            const int* resourceRanks = chainInfo.resourceMap.rank();
            {
                const int MAX_RESOURCE = MAX_CB;
                for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                {   //  remap the constant buffer indices to their sorted indices, set their staging buffer indices and construct the StagingGather data
                    ResourceStaging* stagingMap = m_cbMap[ stageIndex ];
                    binary::Gathering& gathering = stageChain.bufferGathering[ stageIndex ];
                    int gatherIndex = -1;
                    for( int index = 0; index < MAX_RESOURCE; ++index )
                    {
                        ResourceStaging& stagingItem = stagingMap[ index ];
                        if( stagingItem.resourceIndex >= 0 )
                        {
                            stagingItem.resourceIndex = resourceRanks[ stagingItem.resourceIndex ];
                            stagingItem.stagingIndex = stagingIndex;
                            ++stagingIndex;
                            if( gatherIndex < 0 )
                            {
                                gatherIndex = index;
                            }
                        }
                        else if( gatherIndex >= 0 )
                        {   //  construct the gather
                            if( gathering.target.count == 0 )
                            {   //  this is the first gather so we need to initialise the source and target indices
                                gathering.source = static_cast< uint32_t >( stagingMap[ gatherIndex ].stagingIndex );
                                gathering.target.index = static_cast< uint32_t >( m_stagingGathers.getUsed() );
                            }
                            ++gathering.target.count;
                            stagingGather.firstSlot = static_cast< uint32_t >( gatherIndex );
                            stagingGather.slotCount = static_cast< uint32_t >( index - gatherIndex );
                            m_stagingGathers.append( stagingGather );
                            gatherIndex = -1;
                        }
                    }
                    if( gatherIndex >= 0 )
                    {   //  construct the gather
                        if( gathering.target.count == 0 )
                        {   //  this is the first gather so we need to initialise the source and target indices
                            gathering.source = static_cast< uint32_t >( stagingMap[ gatherIndex ].stagingIndex );
                            gathering.target.index = static_cast< uint32_t >( m_stagingGathers.getUsed() );
                        }
                        ++gathering.target.count;
                        stagingGather.firstSlot = static_cast< uint32_t >( gatherIndex );
                        stagingGather.slotCount = static_cast< uint32_t >( MAX_RESOURCE - gatherIndex );
                        m_stagingGathers.append( stagingGather );
                        gatherIndex = -1;
                    }
                }
            }
            {
                const int MAX_RESOURCE = MAX_SRV;
                for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                {   //  remap the srv indices to their sorted indices, set their staging buffer indices and construct the StagingGather data
                    ResourceStaging* stagingMap = m_srvMap[ stageIndex ];
                    binary::Gathering& gathering = stageChain.srvGathering[ stageIndex ];
                    int gatherIndex = -1;
                    for( int index = 0; index < MAX_RESOURCE; ++index )
                    {
                        ResourceStaging& stagingItem = stagingMap[ index ];
                        if( stagingItem.resourceIndex >= 0 )
                        {
                            stagingItem.resourceIndex = resourceRanks[ stagingItem.resourceIndex ];
                            stagingItem.stagingIndex = stagingIndex;
                            ++stagingIndex;
                            if( gatherIndex < 0 )
                            {
                                gatherIndex = index;
                            }
                        }
                        else if( gatherIndex >= 0 )
                        {   //  construct the gather
                            if( gathering.target.count == 0 )
                            {   //  this is the first gather so we need to initialise the source and target indices
                                gathering.source = static_cast< uint32_t >( stagingMap[ gatherIndex ].stagingIndex );
                                gathering.target.index = static_cast< uint32_t >( m_stagingGathers.getUsed() );
                            }
                            ++gathering.target.count;
                            stagingGather.firstSlot = static_cast< uint32_t >( gatherIndex );
                            stagingGather.slotCount = static_cast< uint32_t >( index - gatherIndex );
                            m_stagingGathers.append( stagingGather );
                            gatherIndex = -1;
                        }
                    }
                    if( gatherIndex >= 0 )
                    {   //  construct the gather
                        if( gathering.target.count == 0 )
                        {   //  this is the first gather so we need to initialise the source and target indices
                            gathering.source = static_cast< uint32_t >( stagingMap[ gatherIndex ].stagingIndex );
                            gathering.target.index = static_cast< uint32_t >( m_stagingGathers.getUsed() );
                        }
                        ++gathering.target.count;
                        stagingGather.firstSlot = static_cast< uint32_t >( gatherIndex );
                        stagingGather.slotCount = static_cast< uint32_t >( MAX_RESOURCE - gatherIndex );
                        m_stagingGathers.append( stagingGather );
                        gatherIndex = -1;
                    }
                }
            }
            {
                const int MAX_RESOURCE = MAX_UAV;
                for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                {   //  remap the uav indices to their sorted indices, set their staging buffer indices and construct the StagingGather data
                    ResourceStaging* stagingMap = m_uavMap[ stageIndex ];
                    binary::Gathering& gathering = stageChain.uavGathering[ stageIndex ];
                    int gatherIndex = -1;
                    for( int index = 0; index < MAX_RESOURCE; ++index )
                    {
                        ResourceStaging& stagingItem = stagingMap[ index ];
                        if( stagingItem.resourceIndex >= 0 )
                        {
                            stagingItem.resourceIndex = resourceRanks[ stagingItem.resourceIndex ];
                            stagingItem.stagingIndex = stagingIndex;
                            ++stagingIndex;
                            if( gatherIndex < 0 )
                            {
                                gatherIndex = index;
                            }
                        }
                        else if( gatherIndex >= 0 )
                        {   //  construct the gather
                            if( gathering.target.count == 0 )
                            {   //  this is the first gather so we need to initialise the source and target indices
                                gathering.source = static_cast< uint32_t >( stagingMap[ gatherIndex ].stagingIndex );
                                gathering.target.index = static_cast< uint32_t >( m_stagingGathers.getUsed() );
                            }
                            ++gathering.target.count;
                            stagingGather.firstSlot = static_cast< uint32_t >( gatherIndex );
                            stagingGather.slotCount = static_cast< uint32_t >( index - gatherIndex );
                            m_stagingGathers.append( stagingGather );
                            gatherIndex = -1;
                        }
                    }
                    if( gatherIndex >= 0 )
                    {   //  construct the gather
                        if( gathering.target.count == 0 )
                        {   //  this is the first gather so we need to initialise the source and target indices
                            gathering.source = static_cast< uint32_t >( stagingMap[ gatherIndex ].stagingIndex );
                            gathering.target.index = static_cast< uint32_t >( m_stagingGathers.getUsed() );
                        }
                        ++gathering.target.count;
                        stagingGather.firstSlot = static_cast< uint32_t >( gatherIndex );
                        stagingGather.slotCount = static_cast< uint32_t >( MAX_RESOURCE - gatherIndex );
                        m_stagingGathers.append( stagingGather );
                        gatherIndex = -1;
                    }
                }
            }
            {
                const int MAX_RESOURCE = MAX_SS;
                for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                {   //  remap the sampler indices to their sorted indices, set their staging buffer indices and construct the StagingGather data
                    ResourceStaging* stagingMap = m_ssMap[ stageIndex ];
                    binary::Gathering& gathering = stageChain.samplerGathering[ stageIndex ];
                    int gatherIndex = -1;
                    for( int index = 0; index < MAX_RESOURCE; ++index )
                    {
                        ResourceStaging& stagingItem = stagingMap[ index ];
                        if( stagingItem.resourceIndex >= 0 )
                        {
                            stagingItem.resourceIndex = resourceRanks[ stagingItem.resourceIndex ];
                            stagingItem.stagingIndex = stagingIndex;
                            ++stagingIndex;
                            if( gatherIndex < 0 )
                            {
                                gatherIndex = index;
                            }
                        }
                        else if( gatherIndex >= 0 )
                        {   //  construct the gather
                            if( gathering.target.count == 0 )
                            {   //  this is the first gather so we need to initialise the source and target indices
                                gathering.source = static_cast< uint32_t >( stagingMap[ gatherIndex ].stagingIndex );
                                gathering.target.index = static_cast< uint32_t >( m_stagingGathers.getUsed() );
                            }
                            ++gathering.target.count;
                            stagingGather.firstSlot = static_cast< uint32_t >( gatherIndex );
                            stagingGather.slotCount = static_cast< uint32_t >( index - gatherIndex );
                            m_stagingGathers.append( stagingGather );
                            gatherIndex = -1;
                        }
                    }
                    if( gatherIndex >= 0 )
                    {   //  construct the gather
                        if( gathering.target.count == 0 )
                        {   //  this is the first gather so we need to initialise the source and target indices
                            gathering.source = static_cast< uint32_t >( stagingMap[ gatherIndex ].stagingIndex );
                            gathering.target.index = static_cast< uint32_t >( m_stagingGathers.getUsed() );
                        }
                        ++gathering.target.count;
                        stagingGather.firstSlot = static_cast< uint32_t >( gatherIndex );
                        stagingGather.slotCount = static_cast< uint32_t >( MAX_RESOURCE - gatherIndex );
                        m_stagingGathers.append( stagingGather );
                        gatherIndex = -1;
                    }
                }
            }
            delete[] resourceRanks;
        }
        chainInfo.resourceMap.sort();
        {   //  setup scattering
            binary::StagingScatter stagingScatter;
            int resourceIndex = 0;
            int resourceCount = 0;
            {
                const int MAX_RESOURCE = MAX_CB;
                binary::Scattering& scattering = stageChain.bufferScattering;
                for( resourceCount += cbResourceCount; resourceIndex < resourceCount; ++resourceIndex )
                {
                    stagingScatter.instances = 0;
                    stagingScatter.locations = 0;
                    for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                    {
                        ResourceStaging* stagingMap = m_cbMap[ stageIndex ];
                        for( int index = 0; index < MAX_RESOURCE; ++index )
                        {
                            ResourceStaging& stagingItem = stagingMap[ index ];
                            if( stagingItem.resourceIndex == resourceIndex )
                            {
                                if( stagingScatter.instances == 0 )
                                {   //  this is the first scatter index so we need to initialise the indexing index
                                    stagingScatter.locations = static_cast< uint32_t >( m_stagingIndices.getUsed() );
                                }
                                ++stagingScatter.instances;
                                m_stagingIndices.append( static_cast< uint32_t >( stagingItem.stagingIndex ) );
                            }
                        }
                    }
                    if( stagingScatter.instances > 0 )
                    {
                        if( scattering.target.count == 0 )
                        {   //  this is the first scattering so we need to initialise the source and target indices
                            scattering.source = static_cast< uint32_t >( resourceIndex );
                            scattering.target.index = static_cast< uint32_t >( m_stagingScatters.getUsed() );
                        }
                        ++scattering.target.count;
                        m_stagingScatters.append( stagingScatter );
                    }
                }
            }
            {
                const int MAX_RESOURCE = MAX_SRV;
                binary::Scattering& scattering = stageChain.srvScattering;
                for( resourceCount += srvResourceCount; resourceIndex < resourceCount; ++resourceIndex )
                {
                    stagingScatter.instances = 0;
                    stagingScatter.locations = 0;
                    for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                    {
                        ResourceStaging* stagingMap = m_srvMap[ stageIndex ];
                        for( int index = 0; index < MAX_RESOURCE; ++index )
                        {
                            ResourceStaging& stagingItem = stagingMap[ index ];
                            if( stagingItem.resourceIndex == resourceIndex )
                            {
                                if( stagingScatter.instances == 0 )
                                {   //  this is the first scatter index so we need to initialise the indexing index
                                    stagingScatter.locations = static_cast< uint32_t >( m_stagingIndices.getUsed() );
                                }
                                ++stagingScatter.instances;
                                m_stagingIndices.append( static_cast< uint32_t >( stagingItem.stagingIndex ) );
                            }
                        }
                    }
                    if( stagingScatter.instances > 0 )
                    {
                        if( scattering.target.count == 0 )
                        {   //  this is the first scattering so we need to initialise the source and target indices
                            scattering.source = static_cast< uint32_t >( resourceIndex );
                            scattering.target.index = static_cast< uint32_t >( m_stagingScatters.getUsed() );
                        }
                        ++scattering.target.count;
                        m_stagingScatters.append( stagingScatter );
                    }
                }
            }
            {
                const int MAX_RESOURCE = MAX_UAV;
                binary::Scattering& scattering = stageChain.uavScattering;
                for( resourceCount += uavResourceCount; resourceIndex < resourceCount; ++resourceIndex )
                {
                    stagingScatter.instances = 0;
                    stagingScatter.locations = 0;
                    for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                    {
                        ResourceStaging* stagingMap = m_uavMap[ stageIndex ];
                        for( int index = 0; index < MAX_RESOURCE; ++index )
                        {
                            ResourceStaging& stagingItem = stagingMap[ index ];
                            if( stagingItem.resourceIndex == resourceIndex )
                            {
                                if( stagingScatter.instances == 0 )
                                {   //  this is the first scatter index so we need to initialise the indexing index
                                    stagingScatter.locations = static_cast< uint32_t >( m_stagingIndices.getUsed() );
                                }
                                ++stagingScatter.instances;
                                m_stagingIndices.append( static_cast< uint32_t >( stagingItem.stagingIndex ) );
                            }
                        }
                    }
                    if( stagingScatter.instances > 0 )
                    {
                        if( scattering.target.count == 0 )
                        {   //  this is the first scattering so we need to initialise the source and target indices
                            scattering.source = static_cast< uint32_t >( resourceIndex );
                            scattering.target.index = static_cast< uint32_t >( m_stagingScatters.getUsed() );
                        }
                        ++scattering.target.count;
                        m_stagingScatters.append( stagingScatter );
                    }
                }
            }
            {
                const int MAX_RESOURCE = MAX_SS;
                binary::Scattering& scattering = stageChain.samplerScattering;
                for( resourceCount += ssResourceCount; resourceIndex < resourceCount; ++resourceIndex )
                {
                    stagingScatter.instances = 0;
                    stagingScatter.locations = 0;
                    for( int stageIndex = 0; stageIndex < binary::EShaderStageCount; ++stageIndex )
                    {
                        ResourceStaging* stagingMap = m_ssMap[ stageIndex ];
                        for( int index = 0; index < MAX_RESOURCE; ++index )
                        {
                            ResourceStaging& stagingItem = stagingMap[ index ];
                            if( stagingItem.resourceIndex == resourceIndex )
                            {
                                if( stagingScatter.instances == 0 )
                                {   //  this is the first scatter index so we need to initialise the indexing index
                                    stagingScatter.locations = static_cast< uint32_t >( m_stagingIndices.getUsed() );
                                }
                                ++stagingScatter.instances;
                                m_stagingIndices.append( static_cast< uint32_t >( stagingItem.stagingIndex ) );
                            }
                        }
                    }
                    if( stagingScatter.instances > 0 )
                    {
                        if( scattering.target.count == 0 )
                        {   //  this is the first scattering so we need to initialise the source and target indices
                            scattering.source = static_cast< uint32_t >( resourceIndex );
                            scattering.target.index = static_cast< uint32_t >( m_stagingScatters.getUsed() );
                        }
                        ++scattering.target.count;
                        m_stagingScatters.append( stagingScatter );
                    }
                }
            }
        }
        chainInfo.chainIndex = static_cast< uint32_t >( m_stageChains.insert( stageChain ) );
        stageChainLink.chainInfo = &chainInfo;
        linkIndex = insert( stageChainLink );
    }
    return( m_items.getItem( linkIndex ).chainInfo );
}

int ShaderBuilder::addResource( const ShaderParsing& parsed, const uint resourceNameHandle, const uint resourceType )
{
    uint resourceNameLength = 0;
    const uint8_t* resourceName = parsed.strings.getString( resourceNameHandle, resourceNameLength );
    int resourceItemIndex = m_resourceItems.find( resourceName, resourceNameLength, resourceType );
    if( resourceItemIndex < 0 )
    {   //  the resource does not exist yet so create it
        binary::ResourceItem resourceItem;
        resourceItem.nameHandle = static_cast< uint32_t >( m_strings.addString( resourceName, resourceNameLength ) );
        resourceItem.resourceType = static_cast< uint32_t >( resourceType );
        resourceItem.desc.resourceValidation.packed = 0;
        resourceItem.stride = 0;
        resourceItem.slots = 0;
        resourceItemIndex = m_resourceItems.insert( resourceItem );
    }
    return( resourceItemIndex );
}

int ShaderBuilder::addStreamFormat( const ShaderParsing& parsed, const int streamLinkIndex )
{
    const StreamLink& streamLink = *parsed.streams.access( streamLinkIndex );
    uint streamNameLength = 0;
    const uint8_t* streamName = parsed.strings.getString( streamLink.nameHandle, streamNameLength );
    int streamFormatIndex = m_streamFormats.find( streamName, streamNameLength );
    if( streamFormatIndex < 0 )
    {   //  this StreamFormat has not been created yet
        binary::StreamFormat streamFormat;
        const StreamElementParser& streamElementParser = *streamLink.elements;
        streamFormat.nameHandle = static_cast< uint32_t >( m_strings.addString( streamName, streamNameLength ) );
        streamFormat.stride = static_cast< uint32_t >( streamElementParser.getStride() );
        streamFormat.elements.count = static_cast< uint32_t >( streamElementParser.size() );
        streamFormat.elements.index = static_cast< uint32_t >( m_streamElements.getUsed() );
        uint elementCount = streamFormat.elements.count;
        binary::StreamElement* streamElements = m_streamElements.fetch( elementCount );
        for( uint sourceElementIndex = 0; sourceElementIndex < elementCount; ++sourceElementIndex )
        {   //  add the elements
            const StreamElement& sourceStreamElement = *streamElementParser.access( sourceElementIndex );
            uint elementOffset = sourceStreamElement.dataOffset;
            uint targetElementIndex = 0;
            for( uint offsetElementIndex = 0; offsetElementIndex < elementCount; ++offsetElementIndex )
            {
                if( streamElementParser.access( offsetElementIndex )->dataOffset < elementOffset )
                {
                    ++targetElementIndex;
                }
            }
            binary::StreamElement& targetStreamElement = streamElements[ targetElementIndex ];
            uint elementNameLength = 0;
            const uint8_t* elementName = parsed.strings.getString( sourceStreamElement.nameHandle, elementNameLength );
            targetStreamElement.nameHandle = static_cast< uint32_t >( m_strings.addString( elementName, elementNameLength ) );
            targetStreamElement.format = static_cast< uint32_t >( sourceStreamElement.dataFormat );
            targetStreamElement.size = static_cast< uint32_t >( sourceStreamElement.dataSize );
            targetStreamElement.offset = static_cast< uint32_t >( sourceStreamElement.dataOffset );
        }
        streamFormatIndex = m_streamFormats.insert( streamFormat );
    }
    return( streamFormatIndex );
}

uint32_t ShaderBuilder::addInputAssembly( const ShaderParsing& parsed, const int assemblyIndex, const int chainIndex, ResourceMapping& resourceMapping )
{
    const AssemblyLink& assemblyLink = *parsed.assemblies.access( assemblyIndex );
    const AssemblyElementParser& assemblyElements = *assemblyLink.elements;
    int streamCount = assemblyElements.size();
    const ChainLink& chainLink = *parsed.stages.access( chainIndex );
    int stageLinkIndex = chainLink.stages[ binary::EShaderStageVertex ];
    const StageDesc& stageDesc = *parsed.stages.accessStage( stageLinkIndex );
    const StageParams& params = stageDesc.inputs;
    binary::BlobDesc& blobDesc = m_blobDescs[ stageLinkIndex ];
    if( blobDesc.size == 0 )
    {   //  shader stage has not yet been added to the binaries list
        blobDesc.size = stageDesc.binaryBlob.size;
        blobDesc.offset = m_binaries.append( ( parsed.binaries.getBuffer() + stageDesc.binaryBlob.offset ), stageDesc.binaryBlob.size );
    }
    int blobIndex = m_binaryDescs[ binary::EShaderStageVertex ].find( blobDesc );
    if( blobIndex < 0 )
    {
        blobIndex = m_binaryDescs[ binary::EShaderStageVertex ].insert( blobDesc );
    }
    binary::AssemblyLayout assemblyLayout;
    assemblyLayout.shaderBinaryIndex = ( ( static_cast< uint32_t >( blobIndex ) & 0x0000ffff ) | ( ( uint32_t( binary::EShaderStageVertex ) & 0x0000ffff ) << 16 ) );
    assemblyLayout.rasterisedStreamIndex = 0;
    assemblyLayout.streams.index = static_cast< uint32_t >( m_assemblyStreams.getUsed() );
    assemblyLayout.streams.count = static_cast< uint32_t >( streamCount );
    for( int streamIndex = 0; streamIndex < streamCount; ++streamIndex )
    {
        const AssemblyElement& assemblyElement = *assemblyElements.getStreamElement( static_cast< uint >( streamIndex ) );
        int resourceItemIndex = addResource( parsed, assemblyElement.nameHandle, rendering::EResourceTypeStream );
        binary::ResourceItem& resourceItem = m_resourceItems.getBlob().getItem( static_cast< uint >( resourceItemIndex ) );
        int streamFormatIndex = addStreamFormat( parsed, assemblyElement.streamLinkIndex );
        const binary::StreamFormat& streamFormat = *m_streamFormats.access( streamFormatIndex );
        resourceItem.desc.streamFormatIndex = static_cast< uint32_t >( streamFormatIndex );
        resourceItem.stride = static_cast< uint16_t >( streamFormat.stride );
        resourceMapping.insert( static_cast<uint32_t >( resourceItemIndex ) );
        binary::AssemblyStream assemblyStream;
        assemblyStream.streamFormatIndex = resourceItem.desc.streamFormatIndex;
        assemblyStream.instanceStepRate = static_cast< uint32_t >( assemblyElement.instanceRate );
        assemblyStream.elements.index = static_cast< uint32_t >( m_assemblyElements.getUsed() );
        assemblyStream.elements.count = 0;
        const LinkageLink& linkageLink = *parsed.linkages.access( assemblyElement.linkageLinkIndex );
        const LinkageElementParser& linkageElements = *linkageLink.elements;
        uint32_t streamElementCount = streamFormat.elements.count;
        for( uint32_t streamElementIndex = 0; streamElementIndex < streamElementCount; ++streamElementIndex )
        {   //  run through the stream elements (which have been sorted by offset) and find their matching linkage element (if any)
            const binary::StreamElement& streamElement = m_streamElements.getItem( streamElementIndex + streamFormat.elements.index );
            uint streamElementNameLength = 0;
            const uint8_t* streamElementName = m_strings.getString( streamElement.nameHandle, streamElementNameLength );
            int linkageElementIndex = linkageElements.find( streamElementName, streamElementNameLength );
            if( linkageElementIndex >= 0 )
            {   //  this stream element has a linkage element, check whether the shader binds to it
                const LinkageElement& linkageElement = *linkageElements.access( linkageElementIndex );
                uint semanticNameLength = 0;
                const uint8_t* semanticName = parsed.strings.getString( linkageElement.semanticHandle, semanticNameLength );
                int paramIndex = params.find( semanticName, semanticNameLength, linkageElement.semanticIndex );
                if( paramIndex >= 0 )
                {   //  the shader references this semantic, so add the element to the stream
                    binary::AssemblyElement assemblyElement;
                    assemblyElement.streamElementIndex = static_cast< uint32_t >( streamElementIndex );
                    assemblyElement.semanticHandle = static_cast< uint32_t >( m_strings.addString( semanticName, semanticNameLength ) );
                    assemblyElement.semanticIndex = static_cast< uint32_t >( linkageElement.semanticIndex );
                    assemblyElement.componentIndex = 0;
                    m_assemblyElements.append( assemblyElement );
                    ++assemblyStream.elements.count;
                }
            }
        }
        m_assemblyStreams.append( assemblyStream );
    }
    return( static_cast< uint32_t >( m_inputAssemblyIndices.append( static_cast< uint32_t >( m_assemblyLayouts.append( assemblyLayout ) ) ) ) );
}

uint32_t ShaderBuilder::addOutputAssembly( const ShaderParsing& parsed, const int assemblyIndex, const int chainIndex, ResourceMapping& resourceMapping )
{
    const AssemblyLink& assemblyLink = *parsed.assemblies.access( assemblyIndex );
    const AssemblyElementParser& assemblyElements = *assemblyLink.elements;
    int streamCount = assemblyElements.size();
    const ChainLink& chainLink = *parsed.stages.access( chainIndex );
    int stageLinkIndex = chainLink.stages[ chainLink.streamOutStage ];
    const StageDesc& stageDesc = *parsed.stages.accessStage( stageLinkIndex );
    const StageParams& params = stageDesc.outputs;
    binary::BlobDesc& blobDesc = m_blobDescs[ stageLinkIndex ];
    if( blobDesc.size == 0 )
    {   //  shader stage has not yet been added to the binaries list
        blobDesc.size = stageDesc.binaryBlob.size;
        blobDesc.offset = m_binaries.append( ( parsed.binaries.getBuffer() + stageDesc.binaryBlob.offset ), stageDesc.binaryBlob.size );
    }
    int blobIndex = m_outputDescs.find( blobDesc );
    if( blobIndex < 0 )
    {
        blobIndex = m_outputDescs.insert( blobDesc );
    }
    binary::AssemblyLayout assemblyLayout;
    assemblyLayout.shaderBinaryIndex = ( ( static_cast< uint32_t >( blobIndex ) & 0x0000ffff ) | ( ( static_cast< uint32_t >( chainLink.streamOutStage ) & 0x0000ffff ) << 16 ) );
    assemblyLayout.rasterisedStreamIndex = ( ( chainLink.stageFlags & binary::EShaderStagePixelBit ) ? 0 : 0xffffffff );
    assemblyLayout.streams.index = static_cast< uint32_t >( m_assemblyStreams.getUsed() );
    assemblyLayout.streams.count = static_cast< uint32_t >( streamCount );
    for( int streamIndex = 0; streamIndex < streamCount; ++streamIndex )
    {
        const AssemblyElement& assemblyElement = *assemblyElements.getStreamElement( static_cast< uint >( streamIndex ) );
        int resourceItemIndex = addResource( parsed, assemblyElement.nameHandle, rendering::EResourceTypeOutput );
        binary::ResourceItem& resourceItem = m_resourceItems.getBlob().getItem( static_cast< uint >( resourceItemIndex ) );
        int streamFormatIndex = addStreamFormat( parsed, assemblyElement.streamLinkIndex );
        const binary::StreamFormat& streamFormat = *m_streamFormats.access( streamFormatIndex );
        resourceItem.desc.streamFormatIndex = static_cast< uint32_t >( streamFormatIndex );
        resourceItem.stride = static_cast< uint16_t >( streamFormat.stride );
        resourceMapping.insert( static_cast<uint32_t >( resourceItemIndex ) );
        binary::AssemblyStream assemblyStream;
        assemblyStream.streamFormatIndex = resourceItem.desc.streamFormatIndex;
        assemblyStream.instanceStepRate = 0;
        assemblyStream.elements.index = static_cast< uint32_t >( m_assemblyElements.getUsed() );
        assemblyStream.elements.count = 0;
        const LinkageLink& linkageLink = *parsed.linkages.access( assemblyElement.linkageLinkIndex );
        const LinkageElementParser& linkageElements = *linkageLink.elements;
        uint32_t streamElementCount = streamFormat.elements.count;
        for( uint32_t streamElementIndex = 0; streamElementIndex < streamElementCount; ++streamElementIndex )
        {   //  run through the stream elements (which have been sorted by offset) and find their matching linkage element (if any)
            const binary::StreamElement& streamElement = m_streamElements.getItem( streamElementIndex + streamFormat.elements.index );
            uint streamElementNameLength = 0;
            const uint8_t* streamElementName = m_strings.getString( streamElement.nameHandle, streamElementNameLength );
            int linkageElementIndex = linkageElements.find( streamElementName, streamElementNameLength );
            if( linkageElementIndex >= 0 )
            {   //  this stream element has a linkage element, check whether the shader binds to it
                const LinkageElement& linkageElement = *linkageElements.access( linkageElementIndex );
                uint semanticNameLength = 0;
                const uint8_t* semanticName = parsed.strings.getString( linkageElement.semanticHandle, semanticNameLength );
                int paramIndex = params.find( semanticName, semanticNameLength, ( ( linkageElement.semanticIndex & 0x0000ffff ) | ( ( static_cast< uint >( streamIndex ) & 0x0000ffff ) << 16 ) ) );
                if( paramIndex < 0 )
                {   //  the stream specific parameter could not be found, so check for the non-specific parameter
                    paramIndex = params.find( semanticName, semanticNameLength, linkageElement.semanticIndex );
                }
                if( paramIndex >= 0 )
                {   //  the shader references this semantic, so add the element to the stream
                    const StageParam& param = *params.access( paramIndex );
                    uint32_t componentIndex = 0;
                    BYTE dataMask = param.mask;
                    if( dataMask )
                    {
                        while( ( dataMask & 1 ) == 0 )
                        {
                            ++componentIndex;
                            dataMask >>= 1;
                        }
                    }
                    binary::AssemblyElement assemblyElement;
                    assemblyElement.streamElementIndex = static_cast< uint32_t >( streamElementIndex );
                    assemblyElement.semanticHandle = static_cast< uint32_t >( m_strings.addString( semanticName, semanticNameLength ) );
                    assemblyElement.semanticIndex = static_cast< uint32_t >( linkageElement.semanticIndex );
                    assemblyElement.componentIndex = componentIndex;
                    m_assemblyElements.append( assemblyElement );
                    ++assemblyStream.elements.count;
                }
            }
        }
        m_assemblyStreams.append( assemblyStream );
    }
    return( static_cast< uint32_t >( m_outputAssemblyIndices.append( static_cast< uint32_t >( m_assemblyLayouts.append( assemblyLayout ) ) ) ) );
}

int ShaderBuilder::cmpKey( const void* const key, const int index ) const
{
    const StageChainLink& key1 = *reinterpret_cast< const StageChainLink* >( key );
    const StageChainLink& key2 = m_items.getItem( index );
    return( key1.shaderIndex - key2.shaderIndex );
}

void ShaderBuilder::mangle( void* const data, const uint32_t size ) const
{
    compression::m2f::Encode( reinterpret_cast< uint8_t* const >( data ), size );
}

void ShaderBuilder::unmangle( void* const data, const uint32_t size ) const
{
    compression::m2f::Decode( reinterpret_cast< uint8_t* const >( data ), size );
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

