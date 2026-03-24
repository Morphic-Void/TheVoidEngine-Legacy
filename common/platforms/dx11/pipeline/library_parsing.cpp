
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   library_parsing.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Parsing of .json library file definitions and building of the binary library files.
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

#include "library_parsing.h"
#include "platforms/dx11/rendering/system/binary/resources.h"
#include "platforms/dx11/rendering/system/binary/shaders.h"
#include "platforms/dx11/rendering/system/public/state_descs.h"
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
////    ResourceLibraryParser : resource library .json parsing and binary file builder function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ResourceLibraryParser::init()
{
    kill();
    m_strings.init( 4096 );
    if( m_samplers.init( m_strings, 32 ) && m_rasterStates.init( m_strings, 32 ) && m_depthStates.init( m_strings, 32 ) && m_blendStates.init( m_strings, 32 ) )
    {
        m_error = false;
        return( true );
    }
    kill();
    return( false );
}

void ResourceLibraryParser::kill()
{
    m_error = true;
    m_strings.kill();
    m_samplers.kill();
    m_rasterStates.kill();
    m_depthStates.kill();
    m_blendStates.kill();
}

bool ResourceLibraryParser::parse( const char* const json_file )
{
    if( m_error )
    {
        LOG( "ResourceLibraryParser is in an error state and unable to continue parsing new files" );
        return( false );
    }
    QueryTree queryTree;
    {
        size_t length = 0;
        void* buffer = file::loadFile( json_file, length, 4 );
        if( !buffer )
        {
            LOG( "ResourceLibraryParser was unable to load the .json file \"%s\"", json_file );
            return( false );
        }
        if( !queryTree.ParseJSON( buffer, static_cast< uint >( length ) ) )
        {
            delete[] buffer;
            LOG( "ResourceLibraryParser was unable to parse the .json file \"%s\"", json_file );
            return( false );
        }
        delete[] buffer;
    }
    {
        uint32_t root = queryTree.FindItem( "samplers" );
        if( root )
        {
            if( !m_samplers.parseArray( queryTree, root ) )
            {
                LOG( "ResourceLibraryParser failed while parsing samplers from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            }
        }
    }
    {
        uint32_t root = queryTree.FindItem( "rasterStates" );
        if( root )
        {
            if( !m_rasterStates.parseArray( queryTree, root ) )
            {
                LOG( "ResourceLibraryParser failed while parsing raster states from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            };
        }
    }
    {
        uint32_t root = queryTree.FindItem( "depthStates" );
        if( root )
        {
            if( !m_depthStates.parseArray( queryTree, root ) )
            {
                LOG( "ResourceLibraryParser failed while parsing depth states from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            }
        }
    }
    {
        uint32_t root = queryTree.FindItem( "blendStates" );
        if( root )
        {
            if( !m_blendStates.parseArray( queryTree, root ) )
            {
                LOG( "ResourceLibraryParser failed while parsing blend states from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            }
        }
    }
    return( true );
}

bool ResourceLibraryParser::build( const char* const bin_file )
{
    if( m_error )
    {
        LOG( "ResourceLibraryParser is in an error state and unable to build a library binary file" );
        return( false );
    }
    m_samplers.finalise();
    m_rasterStates.finalise();
    m_depthStates.finalise();
    m_blendStates.finalise();
    uint32_t namesSize = m_strings.getFrozenSize();
    uint32_t samplerDescsCount = m_samplers.getDescs().getUsed();
    uint32_t samplerLinksCount = m_samplers.getLinks().getUsed();
    uint32_t rasterStateDescsCount = m_rasterStates.getDescs().getUsed();
    uint32_t rasterStateLinksCount = m_rasterStates.getLinks().getUsed();
    uint32_t depthStateDescsCount = m_depthStates.getDescs().getUsed();
    uint32_t depthStateLinksCount = m_depthStates.getLinks().getUsed();
    uint32_t blendStateDescsCount = m_blendStates.getDescs().getUsed();
    uint32_t blendStateLinksCount = m_blendStates.getLinks().getUsed();
    uint32_t samplerDescsSize = static_cast< uint32_t >( samplerDescsCount * sizeof( rendering::CSamplerDesc ) );
    uint32_t samplerLinksSize = static_cast< uint32_t >( samplerLinksCount * sizeof( binary::LinkDesc ) );
    uint32_t rasterStateDescsSize = static_cast< uint32_t >( rasterStateDescsCount * sizeof( rendering::CRasterStateDesc ) );
    uint32_t rasterStateLinksSize = static_cast< uint32_t >( rasterStateLinksCount * sizeof( binary::LinkDesc ) );
    uint32_t depthStateDescsSize = static_cast< uint32_t >( depthStateDescsCount * sizeof( rendering::CDepthStateDesc ) );
    uint32_t depthStateLinksSize = static_cast< uint32_t >( depthStateLinksCount * sizeof( binary::LinkDesc ) );
    uint32_t blendStateDescsSize = static_cast< uint32_t >( blendStateDescsCount * sizeof( rendering::CMultiBlendStateDesc ) );
    uint32_t blendStateLinksSize = static_cast< uint32_t >( blendStateLinksCount * sizeof( binary::LinkDesc ) );
    uint32_t fileSize = static_cast< uint32_t >( sizeof( binary::ResourceLibrary ) + namesSize + samplerDescsSize + samplerLinksSize + rasterStateDescsSize + rasterStateLinksSize + depthStateDescsSize + depthStateLinksSize + blendStateDescsSize + blendStateLinksSize );
    binary::ResourceBinaryPtr binaryPtr;
    binaryPtr.data = new uint32_t[ fileSize >> 2 ];
    memset( binaryPtr.data, 0, fileSize );
    binary::ResourceLibrary& library = *binaryPtr.library;
    library.header.magic = MAKE4CC( 'i', 's', 'R', 'L' );
    library.header.platform = MAKE4CC( 'D', 'X', '1', '1' );
    library.header.version = 0x00010001;
    library.header.fileSize = fileSize;
    library.index.stringTable.size = namesSize;
    library.index.stringTable.offset = ( sizeof( binary::ResourceLibrary ) >> 2 );
    library.index.samplerDescs.count = samplerDescsCount;
    library.index.samplerDescs.offset = ( library.index.stringTable.offset + ( namesSize >> 2 ) );
    library.index.rasterStateDescs.count = rasterStateDescsCount;
    library.index.rasterStateDescs.offset = ( library.index.samplerDescs.offset + ( samplerDescsSize >> 2 ) );
    library.index.depthStateDescs.count = depthStateDescsCount;
    library.index.depthStateDescs.offset = ( library.index.rasterStateDescs.offset + ( rasterStateDescsSize >> 2 ) );
    library.index.blendStateDescs.count = blendStateDescsCount;
    library.index.blendStateDescs.offset = ( library.index.depthStateDescs.offset + ( depthStateDescsSize >> 2 ) );
    library.index.samplerLinks.count = samplerLinksCount;
    library.index.samplerLinks.offset = ( library.index.blendStateDescs.offset + ( blendStateDescsSize >> 2 ) );
    library.index.rasterStateLinks.count = rasterStateLinksCount;
    library.index.rasterStateLinks.offset = ( library.index.samplerLinks.offset + ( samplerLinksSize >> 2 ) );
    library.index.depthStateLinks.count = depthStateLinksCount;
    library.index.depthStateLinks.offset = ( library.index.rasterStateLinks.offset + ( rasterStateLinksSize >> 2 ) );
    library.index.blendStateLinks.count = blendStateLinksCount;
    library.index.blendStateLinks.offset = ( library.index.depthStateLinks.offset + ( depthStateLinksSize >> 2 ) );
    m_strings.freeze( &binaryPtr.data[ library.index.stringTable.offset ], namesSize );
    memcpy( &binaryPtr.data[ library.index.samplerDescs.offset ], m_samplers.getDescs().getBuffer(), samplerDescsSize );
    memcpy( &binaryPtr.data[ library.index.rasterStateDescs.offset ], m_rasterStates.getDescs().getBuffer(), rasterStateDescsSize );
    memcpy( &binaryPtr.data[ library.index.depthStateDescs.offset ], m_depthStates.getDescs().getBuffer(), depthStateDescsSize );
    memcpy( &binaryPtr.data[ library.index.blendStateDescs.offset ], m_blendStates.getDescs().getBuffer(), blendStateDescsSize );
    const int* nameRanks = m_strings.getIndexRanks();
    {
        binary::LinkDesc* linkDescs = reinterpret_cast< binary::LinkDesc* >( &binaryPtr.data[ library.index.samplerLinks.offset ] );
        for( uint32_t stateIndex = 0; stateIndex < samplerLinksCount; ++stateIndex )
        {
            const StateLink& stateLink = m_samplers.getLinks().getItem( stateIndex );
            binary::LinkDesc& linkDesc = linkDescs[ stateIndex ];
            linkDesc.nameHandle = static_cast< uint32_t >( nameRanks[ m_strings.getIndex( stateLink.nameHandle ) ] );
            linkDesc.itemIndex = static_cast< uint32_t >( stateLink.descIndex );
        }
    }
    {
        binary::LinkDesc* linkDescs = reinterpret_cast< binary::LinkDesc* >( &binaryPtr.data[ library.index.rasterStateLinks.offset ] );
        for( uint32_t stateIndex = 0; stateIndex < rasterStateLinksCount; ++stateIndex )
        {
            const StateLink& stateLink = m_rasterStates.getLinks().getItem( stateIndex );
            binary::LinkDesc& linkDesc = linkDescs[ stateIndex ];
            linkDesc.nameHandle = static_cast< uint32_t >( nameRanks[ m_strings.getIndex( stateLink.nameHandle ) ] );
            linkDesc.itemIndex = static_cast< uint32_t >( stateLink.descIndex );
        }
    }
    {
        binary::LinkDesc* linkDescs = reinterpret_cast< binary::LinkDesc* >( &binaryPtr.data[ library.index.depthStateLinks.offset ] );
        for( uint32_t stateIndex = 0; stateIndex < depthStateLinksCount; ++stateIndex )
        {
            const StateLink& stateLink = m_depthStates.getLinks().getItem( stateIndex );
            binary::LinkDesc& linkDesc = linkDescs[ stateIndex ];
            linkDesc.nameHandle = static_cast< uint32_t >( nameRanks[ m_strings.getIndex( stateLink.nameHandle ) ] );
            linkDesc.itemIndex = static_cast< uint32_t >( stateLink.descIndex );
        }
    }
    {
        binary::LinkDesc* linkDescs = reinterpret_cast< binary::LinkDesc* >( &binaryPtr.data[ library.index.blendStateLinks.offset ] );
        for( uint32_t stateIndex = 0; stateIndex < blendStateLinksCount; ++stateIndex )
        {
            const StateLink& stateLink = m_blendStates.getLinks().getItem( stateIndex );
            binary::LinkDesc& linkDesc = linkDescs[ stateIndex ];
            linkDesc.nameHandle = static_cast< uint32_t >( nameRanks[ m_strings.getIndex( stateLink.nameHandle ) ] );
            linkDesc.itemIndex = static_cast< uint32_t >( stateLink.descIndex );
        }
    }
    delete[] nameRanks;
    bool success = file::saveFile( bin_file, fileSize, binaryPtr.data );
    delete[] binaryPtr.data;
    if( !success )
    {
        LOG( "ResourceLibraryParser failed to save binary library file \"%s\"", bin_file );
        return( false );
    }
    return( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ShaderLibraryParser : shader library .json parsing and binary file builder function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderLibraryParser::init()
{
    prepareParse();
    return( true );
}

void ShaderLibraryParser::kill()
{
    cleanupParse();
}

bool ShaderLibraryParser::parse( const char* const json_file )
{
    QueryTree queryTree;
    {
        size_t length = 0;
        void* buffer = file::loadFile( json_file, length, 4 );
        if( !buffer )
        {
            LOG( "ShaderLibraryParser was unable to load the .json file \"%s\"", json_file );
            return( false );
        }
        if( !queryTree.ParseJSON( buffer, static_cast< uint >( length ) ) )
        {
            delete[] buffer;
            LOG( "ShaderLibraryParser was unable to parse the .json file \"%s\"", json_file );
            return( false );
        }
        delete[] buffer;
    }
    {
        uint32_t root = queryTree.FindItem( "streams" );
        if( root )
        {
            if( !m_parse.streams.parseArray( queryTree, root ) )
            {
                LOG( "ShaderLibraryParser failed while parsing streams from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            }
        }
    }
    {
        uint32_t root = queryTree.FindItem( "linkages" );
        if( root )
        {
            if( !m_parse.linkages.parseArray( queryTree, root ) )
            {
                LOG( "ShaderLibraryParser failed while parsing linkages from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            }
        }
    }
    {
        uint32_t root = queryTree.FindItem( "assemblies" );
        if( root )
        {
            if( !m_parse.assemblies.parseArray( queryTree, root ) )
            {
                LOG( "ShaderLibraryParser failed while parsing assemblies from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            }
        }
    }
    {
        uint32_t root = queryTree.FindItem( "stages" );
        if( root )
        {
            if( !m_parse.stages.parseArray( queryTree, root ) )
            {
                LOG( "ShaderLibraryParser failed while parsing stages from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            }
        }
    }
    {
        uint32_t root = queryTree.FindItem( "shaders" );
        if( root )
        {
            if( !m_parse.shaders.parseArray( queryTree, root ) )
            {
                LOG( "ShaderLibraryParser failed while parsing shaders from .json file \"%s\"", json_file );
                m_error = true;
                return( false );
            }
        }
    }
    return( true );
}

bool ShaderLibraryParser::build( const char* const bin_file )
{
    if( m_error )
    {
        LOG( "ShaderLibraryParser is in an error state and unable to build a library binary file" );
        return( false );
    }
    ShaderBuilder builder;
    return( builder.build( m_parse, bin_file ) );
}

void ShaderLibraryParser::prepareParse()
{
    kill();
    m_parse.strings.init( 4096 );
    m_parse.binaries.init( 4096 * 16 );
    m_parse.resources.init( m_parse.strings, 32 );
    m_parse.streams.init( m_parse.strings, 32 );
    m_parse.linkages.init( m_parse.strings, 32 );
    m_parse.assemblies.init( m_parse.strings, m_parse.streams, m_parse.linkages, 32 );
    m_parse.stages.init( m_parse.strings, m_parse.binaries, m_parse.resources, 32 );
    m_parse.shaders.init( m_parse.strings, m_parse.stages, m_parse.streams, m_parse.linkages, m_parse.assemblies, 32 );
    m_error = false;
}

void ShaderLibraryParser::cleanupParse()
{
    m_error = true;
    m_parse.strings.kill();
    m_parse.binaries.kill();
    m_parse.resources.kill();
    m_parse.streams.kill();
    m_parse.linkages.kill();
    m_parse.assemblies.kill();
    m_parse.stages.kill();
    m_parse.shaders.kill();
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

