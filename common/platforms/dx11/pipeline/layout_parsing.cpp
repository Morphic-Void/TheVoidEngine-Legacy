
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   layout_parsing.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Parsing of .json vertex layout and mapping definitions.
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
////    StreamElementParser : vertex stream element parsing and collection class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool StreamElementParser::init( StringTableBlob& strings, const uint count )
{
    kill();
    uint size = ( ( count < 4 ) ? 4 : count );
    if( StreamElementParserBase::init( strings, size ) )
    {
        m_stride = 0;
        return( true );
    }
    kill();
    return( false );
}

bool StreamElementParser::parseArray( const QueryTree& queryTree, const uint32_t root )
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

bool StreamElementParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: Stream element malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A Stream element named \"%s\" already exists", name );
        return( false );
    }
    StreamElement streamElement;
    streamElement.nameHandle = m_strings->addString( name );
    streamElement.sourceHandle = 0;
    streamElement.dataFormat = 0xffffffff;
    streamElement.dataSize = 0;
    streamElement.dataOffset = 0xffffffff;
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findStreamConfigToken( key ) )
        {
            case( EStreamConfigFormat ):
            {
                int dataFormat = findStreamItemTypeToken( queryTree.GetText( item ) );
                if( dataFormat >= 0 )
                {
                    streamElement.dataFormat = static_cast< uint >( dataFormat );
                    parsed = true;
                }
                break;
            }
            case( EStreamConfigOffset ):
            {
                uint dataOffset = 0;
                if( queryTree.GetNumberAsUint( item, dataOffset ) )
                {
                    streamElement.dataOffset = dataOffset;
                    parsed = true;
                }
                break;
            }
            case( EStreamConfigSource ):
            {
                const char* sourceName = queryTree.GetText( item );
                if( sourceName != NULL )
                {
                    streamElement.sourceHandle = m_strings->addString( sourceName );
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
            LOG( "Problem parsing Stream element parameter \"%s\"", key );
            success = false;
        }
    }
    if( streamElement.dataFormat == 0xffffffff )
    {
        LOG( "Stream element format not specified" );
        success = false;
    }
    else
    {
        streamElement.dataSize = rendering::getEElementFormatSize( static_cast< rendering::EElementFormat >( streamElement.dataFormat ) );
        if( streamElement.dataOffset == 0xffffffff )
        {
            if( streamElement.dataSize & 1 )
            {   //  unaligned
                streamElement.dataOffset = m_stride;
            }
            else if( streamElement.dataSize & 2 )
            {   //  align to 16-bits
                streamElement.dataOffset = ( ( m_stride + 1 ) & ~1 );
            }
            else
            {   //  align to 32-bits
                streamElement.dataOffset = ( ( m_stride + 3 ) & ~3 );
            }
        }
        uint stride = ( streamElement.dataOffset + streamElement.dataSize );
        if( m_stride < stride )
        {
            m_stride = stride;
        }
        int count = size();
        for( int index = 0; index < count; ++index )
        {   //  check that there are no overlaps
            const StreamElement* checkElement = access( index );
            if( streamElement.dataOffset < checkElement->dataOffset )
            {
                if( ( streamElement.dataOffset + streamElement.dataSize ) > checkElement->dataOffset )
                {
                    success = false;
                }
            }
            else
            {
                if( ( checkElement->dataOffset + checkElement->dataSize ) > streamElement.dataOffset )
                {
                    success = false;
                }
            }
            if( !success )
            {
                LOG( "Stream element overlaps existing element" );
            }
        }
    }
    if( !success )
    {
        LOG( "Error: Stream element \"%s\" parsing failed", name );
        return( false );
    }
    insert( streamElement );
    return( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    StreamParser : vertex stream parsing and collection class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool StreamParser::init( StringTableBlob& strings, const uint count )
{
    kill();
    uint size = ( ( count < 4 ) ? 4 : count );
    if( StreamParserBase::init( strings, size ) )
    {
        m_elements.init( size );
        return( true );
    }
    kill();
    return( false );
}

bool StreamParser::parseArray( const QueryTree& queryTree, const uint32_t root )
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

bool StreamParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: Stream malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A Stream named \"%s\" already exists", name );
        return( false );
    }
    StreamLink streamLink;
    streamLink.nameHandle = m_strings->addString( name );
    streamLink.elements = &m_elements.fetch();
    if( !streamLink.elements->init( *m_strings, 4 ) )
    {
        LOG( "Error: Stream \"%s\" initialisation failed", name );
        return( false );
    }
    if( !streamLink.elements->parseArray( queryTree, root ) )
    {
        LOG( "Error: Stream \"%s\" parsing failed", name );
        return( false );
    }
    insert( streamLink );
    return( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    LinkageElementParser : vertex linkage element parsing and collection class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool LinkageElementParser::parseArray( const QueryTree& queryTree, const uint32_t root )
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

bool LinkageElementParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: Linkage element malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A Linkage element named \"%s\" already exists", name );
        return( false );
    }
    LinkageElement linkageElement;
    linkageElement.nameHandle = m_strings->addString( name );
    linkageElement.semanticHandle = 0;
    linkageElement.semanticIndex = 0;
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findLinkageConfigToken( key ) )
        {
            case( ELinkageConfigSemantic ):
            {
                const char* semanticName = queryTree.GetText( item );
                if( semanticName != NULL )
                {   //  need to seperate the semantic name and index (if any)
                    uint semanticLength = 0;
                    while( semanticName[ semanticLength ] ) ++semanticLength;
                    if( semanticLength )
                    {
                        while( semanticLength )
                        {
                            --semanticLength;
                            uint8_t digit = semanticName[ semanticLength ];
                            if( ( digit < '0' ) || ( digit > '9' ) )
                            {
                                ++semanticLength;
                                break;
                            }
                        }
                        if( semanticLength )
                        {
                            linkageElement.semanticHandle = m_strings->addString( semanticName, semanticLength );
                            if( semanticName[ semanticLength ] )
                            {
                                uint semanticIndex = 0;
                                while( semanticName[ semanticLength ] )
                                {
                                    semanticIndex *= 10;
                                    semanticIndex += static_cast< uint >( semanticName[ semanticLength ] - '0' );
                                    semanticLength++;
                                }
                                linkageElement.semanticIndex = semanticIndex;
                            }
                            parsed = true;
                        }
                    }
                }
                break;
            }
            case( ELinkageConfigSemanticIndex ):
            {
                uint semanticIndex = 0;
                if( queryTree.GetNumberAsUint( item, semanticIndex ) )
                {
                    linkageElement.semanticIndex = semanticIndex;
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
            LOG( "Problem parsing Linkage element parameter \"%s\"", key );
            success = false;
        }
    }
    if( linkageElement.semanticHandle == 0 )
    {
        LOG( "Linkage element semantic name was not specified" );
        success = false;
    }
    else
    {
        int count = size();
        for( int index = 0; index < count; ++index )
        {   //  check that there are no duplicate semantic bindings
            const LinkageElement& checkElement = *access( index );
            if( ( linkageElement.semanticHandle == checkElement.semanticHandle ) && ( linkageElement.semanticIndex == checkElement.semanticIndex ) )
            {   //  collision
                LOG( "Linkage element \"%s\" conflicts with element \"%s\" (semantic \"%s\", index %u)", name, m_strings->getString( checkElement.nameHandle ), m_strings->getString( linkageElement.semanticHandle ), linkageElement.semanticIndex );
                success = false;
            }
        }
    }
    if( !success )
    {
        LOG( "Error: Linkage element \"%s\" parsing failed", name );
        return( false );
    }
    insert( linkageElement );
    return( true );
}

int LinkageElementParser::findSemantic( const uint semanticHandle, const uint semanticIndex ) const
{
    if( valid() )
    {
        int elementCount = size();
        for( int elementIndex = 0; elementIndex < elementCount; ++elementIndex )
        {
            const LinkageElement& linkageElement = *access( elementIndex );
            if( ( linkageElement.semanticHandle == semanticHandle ) && ( linkageElement.semanticIndex == semanticIndex ) )
            {
                return( elementIndex );
            }
        }
    }
    return( -1 );
}

int LinkageElementParser::findSemantic( const uint8_t* const semanticName, const uint semanticIndex ) const
{
    if( semanticName != NULL )
    {
        uint semanticLength = 0;
        while( semanticName[ semanticLength ] ) ++semanticLength;
        return( findSemantic( semanticName, semanticLength, semanticIndex ) );
    }
    return( -1 );
}

int LinkageElementParser::findSemantic( const uint8_t* const semanticName, const uint semanticLength, const uint semanticIndex ) const
{
    if( valid() && ( semanticName != NULL ) )
    {
        uint semanticHandle = m_strings->findString( semanticName, semanticLength );
        if( semanticHandle )
        {   //  the semantic name has been used
            return( findSemantic( semanticHandle, semanticIndex ) );
        }
    }
    return( -1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    LinkageParser : vertex linkage parsing and collection class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool LinkageParser::init( StringTableBlob& strings, const uint count )
{
    kill();
    uint size = ( ( count < 4 ) ? 4 : count );
    if( LinkageParserBase::init( strings, size ) )
    {
        m_elements.init( size );
        return( true );
    }
    kill();
    return( false );
}

bool LinkageParser::parseArray( const QueryTree& queryTree, const uint32_t root )
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

bool LinkageParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: Linkage malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: A Linkage named \"%s\" already exists", name );
        return( false );
    }
    LinkageLink linkageLink;
    linkageLink.nameHandle = m_strings->addString( name );
    linkageLink.elements = &m_elements.fetch();
    if( !linkageLink.elements->init( *m_strings, 4 ) )
    {
        LOG( "Error: Linkage \"%s\" initialisation failed", name );
        return( false );
    }
    if( !linkageLink.elements->parseArray( queryTree, root ) )
    {
        LOG( "Error: Linkage \"%s\" parsing failed", name );
        return( false );
    }
    insert( linkageLink );
    return( true );
}

void LinkageParser::listConflicts( const int linkage1, const int linkage2 ) const
{
    if( valid() )
    {
        if( ( static_cast< const uint >( linkage1 ) < static_cast< const uint >( size() ) ) && ( static_cast< const uint >( linkage2 ) < static_cast< const uint >( size() ) ) )
        {
            bool compatible = true;
            if( linkage1 == linkage2 )
            {
                LOG( "Linkage \"%s\" automatically conflicts with itself", m_strings->getString( access( linkage1 )->nameHandle ) );
                compatible = false;
            }
            else
            {
                const LinkageLink& link1 = *access( linkage1 );
                const LinkageLink& link2 = *access( linkage2 );
                const LinkageElementParser& elements1 = *link1.elements;
                const LinkageElementParser& elements2 = *link2.elements;
                int count1 = elements1.size();
                int count2 = elements2.size();
                for( int index1 = 0; index1 < count1; ++index1 )
                {
                    const LinkageElement& element1 = *elements1.access( index1 );
                    for( int index2 = 0; index2 < count2; ++index2 )
                    {
                        const LinkageElement& element2 = *elements2.access( index2 );
                        if( ( element1.semanticHandle == element2.semanticHandle ) && ( element1.semanticIndex == element2.semanticIndex ) )
                        {
                            if( compatible )
                            {
                                LOG( "Linkages \"%s\" and \"%s\" have the following conflicts:", m_strings->getString( link1.nameHandle ), m_strings->getString( link2.nameHandle ) );
                                compatible = false;
                            }
                            LOG( "    semantic \"%s\", index %u", m_strings->getString( element1.semanticHandle ), element1.semanticIndex );
                        }
                    }
                }
            }
        }
    }
}

bool LinkageParser::linkagesConflict( const int linkage1, const int linkage2 ) const
{
    if( valid() )
    {
        if( ( static_cast< const uint >( linkage1 ) < static_cast< const uint >( size() ) ) && ( static_cast< const uint >( linkage2 ) < static_cast< const uint >( size() ) ) )
        {
            if( linkage1 == linkage2 )
            {
                return( true );
            }
            const LinkageLink& link1 = *access( linkage1 );
            const LinkageLink& link2 = *access( linkage2 );
            const LinkageElementParser& elements1 = *link1.elements;
            const LinkageElementParser& elements2 = *link2.elements;
            int count1 = elements1.size();
            int count2 = elements2.size();
            for( int index1 = 0; index1 < count1; ++index1 )
            {
                const LinkageElement& element1 = *elements1.access( index1 );
                for( int index2 = 0; index2 < count2; ++index2 )
                {
                    const LinkageElement& element2 = *elements2.access( index2 );
                    if( ( element1.semanticHandle == element2.semanticHandle ) && ( element1.semanticIndex == element2.semanticIndex ) )
                    {
                        return( true );
                    }
                }
            }
        }
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    AssemblyElementParser : vertex assembly stream and linkage binding parsing and collection class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool AssemblyElementParser::init( StringTableBlob& strings, const StreamParser& streams, const LinkageParser& linkages,const uint count )
{
    kill();
    uint size = ( ( count < 4 ) ? 4 : count );
    if( AssemblyElementParserBase::init( strings, size ) )
    {
        m_streams = &streams;
        m_linkages = &linkages;
        return( true );
    }
    kill();
    return( false );
}

bool AssemblyElementParser::parseArray( const QueryTree& queryTree, const uint32_t root )
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

bool AssemblyElementParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: Assembly element malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: An Assembly element named \"%s\" already exists", name );
        return( false );
    }
    AssemblyElement assemblyElement;
    assemblyElement.nameHandle = m_strings->addString( name );
    assemblyElement.streamIndex = size();
    assemblyElement.instanceRate = 0;
    assemblyElement.streamLinkIndex = 0xffffffff;
    assemblyElement.linkageLinkIndex = 0xffffffff;
    const char* streamName = NULL;
    const char* linkageName = NULL;
    bool success = true;
    for( uint32_t item = queryTree.GetMember( root, uint32_t( 0 ) ); item; item = queryTree.GetNext( item ) )
    {
        bool parsed = false;
        const char* key = queryTree.GetKey( item );
        switch( findAssemblyConfigToken( key ) )
        {
            case( EAssemblyConfigFrequency ):
            {
                uint value = 0;
                if( queryTree.GetNumberAsUint( item, value ) )
                {
                    assemblyElement.instanceRate = value;
                    parsed = true;
                }
                break;
            }
            case( EAssemblyConfigStream ):
            {
                streamName = queryTree.GetText( item );
                if( streamName != NULL )
                {
                    assemblyElement.streamLinkIndex = m_streams->find( streamName );
                    if( assemblyElement.streamLinkIndex < 0 )
                    {
                        LOG( "Assembly element stream \"%s\" not found", streamName );
                    }
                    else
                    {
                        parsed = true;
                    }
                }
                break;
            }
            case( EAssemblyConfigLinkage ):
            {
                linkageName = queryTree.GetText( item );
                if( linkageName != NULL )
                {
                    assemblyElement.linkageLinkIndex = m_linkages->find( linkageName );
                    if( assemblyElement.linkageLinkIndex < 0 )
                    {
                        LOG( "Assembly element linkage \"%s\" not found", linkageName );
                    }
                    else
                    {
                        parsed = true;
                    }
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
            LOG( "Problem parsing Assembly element parameter \"%s\"", key );
            success = false;
        }
    }
    if( assemblyElement.streamLinkIndex == 0xffffffff )
    {
        LOG( "No valid stream was specified" );
        success = false;
    }
    if( assemblyElement.linkageLinkIndex == 0xffffffff )
    {
        LOG( "No valid linkage was specified" );
        success = false;
    }
    if( !success )
    {
        LOG( "Error: Assembly element \"%s\" parsing failed", name );
        return( false );
    }
    const StreamLink& streamLink = *m_streams->access( assemblyElement.streamLinkIndex );
    const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
    const StreamElementParser& streamElements = *streamLink.elements;
    const LinkageElementParser& linkageElements = *linkageLink.elements;
    int linkageCount = linkageElements.size();
    for( int linkageIndex = 0; linkageIndex < linkageCount; ++linkageIndex )
    {   //  check that the stream and linkage are compatible
        const LinkageElement& linkageElement = *linkageElements.access( linkageIndex );
        const uint8_t* const elementName = m_strings->getString( linkageElement.nameHandle );
        int elementIndex = streamElements.find( elementName );
        if( elementIndex < 0 )
        {
            LOG( "Stream element \"%\" referenced by Linkage \"%\" not found in Stream \"%\"", elementName, linkageName, streamName );
            success = false;
        }
    }
    if( !success )
    {
        LOG( "Error: Assembly linkage \"%s\" is incompatible with stream \"%s\"", linkageName, streamName );
        return( false );
    }
    insert( assemblyElement );
    return( true );
}

void AssemblyElementParser::listConflicts() const
{
    if( valid() )
    {
        int elementCount = size();
        for( int outerIndex = 1; outerIndex < elementCount; ++outerIndex )
        {   //  check that the stream and linkage have not already been referenced
            const AssemblyElement& outerElement = *access( outerIndex );
            for( int innerIndex = 0; innerIndex < outerIndex; ++innerIndex )
            {   //  check that the stream and linkage have not already been referenced
                const AssemblyElement& innerElement = *access( innerIndex );
                m_linkages->listConflicts( innerElement.linkageLinkIndex, outerElement.linkageLinkIndex );
            }
        }
    }
}

bool AssemblyElementParser::linkagesConflict() const
{
    if( valid() )
    {
        int elementCount = size();
        for( int outerIndex = 1; outerIndex < elementCount; ++outerIndex )
        {   //  check that the stream and linkage have not already been referenced
            const AssemblyElement& outerElement = *access( outerIndex );
            for( int innerIndex = 0; innerIndex < outerIndex; ++innerIndex )
            {   //  check that the stream and linkage have not already been referenced
                const AssemblyElement& innerElement = *access( innerIndex );
                if( m_linkages->linkagesConflict( innerElement.linkageLinkIndex, outerElement.linkageLinkIndex ) )
                {
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool AssemblyElementParser::singleElementStreams() const
{
    if( valid() )
    {
        int elementCount = size();
        for( int elementIndex = 0; elementIndex < elementCount; ++elementIndex )
        {
            const AssemblyElement& assemblyElement = *access( elementIndex );
            const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
            const LinkageElementParser& linkageElements = *linkageLink.elements;
            if( linkageElements.size() != 1 )
            {
                return( false );
            }
        }
        return( true );
    }
    return( false );
}

bool AssemblyElementParser::streamOutCompatibleTypes() const
{
    if( valid() )
    {
        int elementCount = size();
        for( int elementIndex = 0; elementIndex < elementCount; ++elementIndex )
        {
            const AssemblyElement& assemblyElement = *access( elementIndex );
            const StreamLink& streamLink = *m_streams->access( assemblyElement.streamLinkIndex );
            const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
            const StreamElementParser& streamElements = *streamLink.elements;
            const LinkageElementParser& linkageElements = *linkageLink.elements;
            int linkageElementCount = linkageElements.size();
            for( int linkageElementIndex = 0; linkageElementIndex < linkageElementCount; ++linkageElementIndex )
            {
                const LinkageElement& linkageElement = *linkageElements.access( linkageElementIndex );
                const uint8_t* const elementName = m_strings->getString( linkageElement.nameHandle );
                int streamElementIndex = streamElements.find( elementName );
                if( streamElementIndex < 0 )
                {
                    return( false );
                }
                const StreamElement& streamElement = *streamElements.access( streamElementIndex );
                if( streamElement.dataFormat < rendering::EElement_32x1_TYPELESS )
                {
                    return( false );
                }
            }
        }
        return( true );
    }
    return( false );
}

bool AssemblyElementParser::containsSemantic( const uint semanticHandle, const uint semanticIndex ) const
{
    if( valid() )
    {
        int elementCount = size();
        for( int elementIndex = 0; elementIndex < elementCount; ++elementIndex )
        {
            const AssemblyElement& assemblyElement = *access( elementIndex );
            const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
            if( linkageLink.elements->findSemantic( semanticHandle, semanticIndex ) >= 0 )
            {
                return( true );
            }
        }
    }
    return( false );
}

bool AssemblyElementParser::containsSemantic( const uint8_t* const semanticName, const uint semanticIndex ) const
{
    if( semanticName != NULL )
    {
        uint semanticLength = 0;
        while( semanticName[ semanticLength ] ) ++semanticLength;
        return( containsSemantic( semanticName, semanticLength, semanticIndex ) );
    }
    return( false );
}

bool AssemblyElementParser::containsSemantic( const uint8_t* const semanticName, const uint semanticLength, const uint semanticIndex ) const
{
    if( valid() && ( semanticName != NULL ) )
    {
        int elementCount = size();
        for( int elementIndex = 0; elementIndex < elementCount; ++elementIndex )
        {
            const AssemblyElement& assemblyElement = *access( elementIndex );
            const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
            if( linkageLink.elements->findSemantic( semanticName, semanticLength, semanticIndex ) >= 0 )
            {
                return( true );
            }
        }
    }
    return( false );
}

bool AssemblyElementParser::matchingDataSize( const uint dataSize, const int elementIndex, const uint semanticHandle, const uint semanticIndex ) const
{
    if( valid() )
    {
        if( static_cast< uint >( elementIndex ) < static_cast< uint >( size() ) )
        {
            const AssemblyElement& assemblyElement = *access( elementIndex );
            const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
            const LinkageElementParser& linkageElements = *linkageLink.elements;
            int linkageElementIndex = linkageElements.findSemantic( semanticHandle, semanticIndex );
            if( linkageElementIndex >= 0 )
            {
                const LinkageElement& linkageElement = *linkageElements.access( linkageElementIndex );
                const StreamLink& streamLink = *m_streams->access( assemblyElement.streamLinkIndex );
                const StreamElementParser& streamElements = *streamLink.elements;
                const uint8_t* const elementName = m_strings->getString( linkageElement.nameHandle );
                int streamElementIndex = streamElements.find( elementName );
                if( streamElementIndex >= 0 )
                {
                    const StreamElement& streamElement = *streamElements.access( streamElementIndex );
                    if( streamElement.dataSize == dataSize )
                    {
                        return( true );
                    }
                }
            }
        }
    }
    return( false );
}

bool AssemblyElementParser::matchingDataSize( const uint dataSize, const int elementIndex, const uint8_t* const semanticName, const uint semanticIndex ) const
{
    if( semanticName != NULL )
    {
        uint semanticLength = 0;
        while( semanticName[ semanticLength ] ) ++semanticLength;
        return( matchingDataSize( dataSize, elementIndex, semanticName, semanticLength, semanticIndex ) );
    }
    return( false );
}

bool AssemblyElementParser::matchingDataSize( const uint dataSize, const int elementIndex, const uint8_t* const semanticName, const uint semanticLength, const uint semanticIndex ) const
{
    if( valid() && ( semanticName != NULL ) )
    {
        if( static_cast< uint >( elementIndex ) < static_cast< uint >( size() ) )
        {
            const AssemblyElement& assemblyElement = *access( elementIndex );
            const LinkageLink& linkageLink = *m_linkages->access( assemblyElement.linkageLinkIndex );
            const LinkageElementParser& linkageElements = *linkageLink.elements;
            int linkageElementIndex = linkageElements.findSemantic( semanticName, semanticLength, semanticIndex );
            if( linkageElementIndex >= 0 )
            {
                const LinkageElement& linkageElement = *linkageElements.access( linkageElementIndex );
                const StreamLink& streamLink = *m_streams->access( assemblyElement.streamLinkIndex );
                const StreamElementParser& streamElements = *streamLink.elements;
                const uint8_t* const elementName = m_strings->getString( linkageElement.nameHandle );
                int streamElementIndex = streamElements.find( elementName );
                if( streamElementIndex >= 0 )
                {
                    const StreamElement& streamElement = *streamElements.access( streamElementIndex );
                    if( streamElement.dataSize == dataSize )
                    {
                        return( true );
                    }
                }
            }
        }
    }
    return( false );
}

const AssemblyElement* AssemblyElementParser::getStreamElement( const uint streamIndex ) const
{
    if( valid() )
    {
        int elementCount = size();
        for( int elementIndex = 0; elementIndex < elementCount; ++elementIndex )
        {
            const AssemblyElement& assemblyElement = *access( elementIndex );
            if( assemblyElement.streamIndex == streamIndex )
            {
                return( &assemblyElement );
            }
        }
    }
    return( NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    AssemblyParser : vertex assembly parsing and collection class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool AssemblyParser::init( StringTableBlob& strings, const StreamParser& streams, const LinkageParser& linkages, const uint count )
{
    kill();
    uint size = ( ( count < 4 ) ? 4 : count );
    if( AssemblyParserBase::init( strings, size ) )
    {
        m_elements.init( size );
        m_streams = &streams;
        m_linkages = &linkages;
        return( true );
    }
    kill();
    return( false );
}

bool AssemblyParser::parseArray( const QueryTree& queryTree, const uint32_t root )
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

bool AssemblyParser::parse( const QueryTree& queryTree, const uint32_t root )
{
    if( !valid() )
    {
        return( false );
    }
    const char* name = queryTree.GetKey( root );
    if( name == NULL )
    {
        LOG( "Error: Assembly malformed" );
        return( false );
    }
    if( find( name ) >= 0 )
    {
        LOG( "Error: An Assembly named \"%s\" already exists", name );
        return( false );
    }
    AssemblyLink assemblyLink;
    assemblyLink.nameHandle = m_strings->addString( name );
    assemblyLink.elements = &m_elements.fetch();
    if( !assemblyLink.elements->init( *m_strings, *m_streams, *m_linkages, 4 ) )
    {
        LOG( "Error: Assembly \"%s\" initialisation failed", name );
        return( false );
    }
    if( !assemblyLink.elements->parseArray( queryTree, root ) )
    {
        LOG( "Error: Assembly \"%s\" parsing failed", name );
        return( false );
    }
    int assemblyLinkCount = size();
    int elementCount = assemblyLink.elements->size();
    for( int elementIndex = 0; elementIndex < elementCount; ++elementIndex )
    {   //  check for resource name collisions with any previously defined assemblies (resource name cannot refer to more than 1 stream format)
        const AssemblyElement& assemblyElement = *assemblyLink.elements->access( elementIndex );
        for( int checkAssemblyLinkIndex = 0; checkAssemblyLinkIndex < assemblyLinkCount; ++checkAssemblyLinkIndex )
        {
            const AssemblyLink& checkAssemblyLink = *access( checkAssemblyLinkIndex );
            int checkElementCount = checkAssemblyLink.elements->size();
            for( int checkElementIndex = 0; checkElementIndex < checkElementCount; ++checkElementIndex )
            {
                const AssemblyElement& checkAssemblyElement = *checkAssemblyLink.elements->access( checkElementIndex );
                if( ( assemblyElement.nameHandle == checkAssemblyElement.nameHandle ) && ( assemblyElement.streamLinkIndex != checkAssemblyElement.streamLinkIndex ) )
                {
                    LOG( "Error: Assembly \"%s\" is invalid because the resource \"%s\" stream conflicts with a previous binding definition", name, m_strings->getString( assemblyElement.nameHandle ) );
                    return( false );
                }
            }
        }
    }
    assemblyLink.linkagesConflict = assemblyLink.elements->linkagesConflict();
    assemblyLink.singleElementStreams = assemblyLink.elements->singleElementStreams();
    assemblyLink.streamOutCompatibleTypes = assemblyLink.elements->streamOutCompatibleTypes();
    if( assemblyLink.linkagesConflict && ( !assemblyLink.singleElementStreams || ( assemblyLink.elements->size() > 4 ) ) )
    {
        LOG( "Assembly \"%s\" is invalid as an input due to semantic conflicts between the streams", name );
        if( !assemblyLink.singleElementStreams )
        {
            LOG( "Assembly \"%s\" is invalid as a geometry stage output because it references multiple streams of which at least one has more than 1 element", name );
        }
        if( assemblyLink.elements->size() > 4 )
        {
            LOG( "Assembly \"%s\" is invalid as a geometry stage output because it binds more than 4 streams", name );
        }
        assemblyLink.elements->listConflicts();
        LOG( "Error: Assembly \"%s\" is invalid as both an input and an output", name );
        return( false );
    }
    insert( assemblyLink );
    return( true );
}

bool AssemblyParser::assembliesConflict( const int assembly1, const int assembly2 ) const
{
    if( valid() )
    {
        if( ( static_cast< const uint >( assembly1 ) < static_cast< const uint >( size() ) ) && ( static_cast< const uint >( assembly2 ) < static_cast< const uint >( size() ) ) )
        {
            if( assembly1 == assembly2 )
            {
                return( true );
            }
            const AssemblyLink& link1 = *access( assembly1 );
            const AssemblyLink& link2 = *access( assembly2 );
            const AssemblyElementParser& elements1 = *link1.elements;
            const AssemblyElementParser& elements2 = *link2.elements;
            int count1 = elements1.size();
            int count2 = elements2.size();
            for( int index1 = 0; index1 < count1; ++index1 )
            {
                const AssemblyElement& element1 = *elements1.access( index1 );
                for( int index2 = 0; index2 < count2; ++index2 )
                {
                    const AssemblyElement& element2 = *elements2.access( index2 );
                    if( element1.nameHandle == element2.nameHandle )
                    {
                        return( true );
                    }
                }
            }
        }
    }
    return( false );
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

