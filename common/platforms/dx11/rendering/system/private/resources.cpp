
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   resources.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Private resource classes.
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
////    Named resources class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CNamedResources::init( StringTableBlob& strings, const int count )
{
    kill();
    int size = ( ( count < 4 ) ? 4 : count );
    if( m_sorting.init( *this, size ) )
    {
        m_items = new Item[ size ];
        if( m_items )
        {
            m_strings = &strings;
            m_size = size;
            return( true );
        }
        m_sorting.kill();
    }
    return( false );
}

void CNamedResources::kill()
{
    m_sorting.kill();
    m_strings = NULL;
    if( m_items )
    {
        delete[] m_items;
        m_items = NULL;
    }
    m_size = 0;
}

int CNamedResources::findIndex( const StringItem& name, const uint32_t resourceType ) const
{
    int index = -1;
    if( valid() && name.notEmpty() )
    {
        uint length = 0;
        const uint8_t* string = name.getString( length );
        m_temp.nameHandle = static_cast< uint32_t >( m_strings->findString( string, length ) );
        m_temp.resourceType = resourceType;
        index = m_sorting.find( &m_temp );
    }
    return( index );
}

bool CNamedResources::getName( const int index, StringItem& name ) const
{
    if( valid( index ) )
    {
        uint length = 0;
        const uint8_t* string = m_strings->getString( static_cast< uint >( m_items[ index ].nameHandle ), length );
        name.setString( string, length );
        return( true );
    }
    return( false );
}

const uint32_t CNamedResources::getType( const int index ) const
{
    return( valid( index ) ? m_items[ index ].resourceType : static_cast< uint32_t >( EResourceTypeUnknown ) );
}

const Resource& CNamedResources::getResource( const int index ) const
{
    return( valid( index ) ? m_items[ index ].resource : m_null );
}

bool CNamedResources::getResource( const StringItem& name, const uint32_t resourceType, Resource& resource ) const
{
    int index = findIndex( name, resourceType );
    if( index >= 0 )
    {
        resource.share( m_items[ index ].resource );
        return( true );
    }
    return( false );
}

bool CNamedResources::addResource( const StringItem& name, const uint32_t resourceType, const Resource& resource, const bool allowReplace )
{
    if( valid() )
    {
        int index = findIndex( name, resourceType );
        if( index < 0 )
        {   //  item not found
            uint length = 0;
            const uint8_t* string = name.getString( length );
            m_temp.nameHandle = static_cast< uint32_t >( m_strings->addString( string, length ) );
            m_temp.resourceType = resourceType;
            index = m_sorting.insert( &m_temp );
            if( index >= m_size )
            {   //  need to grow the array
                int size = ( m_size + ( m_size >> 1 ) + 1 );
                size_t bytes = ( m_size * sizeof( Item ) );
                Item* items = new Item[ size ];
                memcpy_s( items, bytes, m_items, bytes );
                memset( m_items, 0, bytes );
                delete[] m_items;
                m_items = items;
                m_size = size;
            }
            Item& item = m_items[ index ];
            item.resource.share( resource );
            item.resourceType = m_temp.resourceType;
            item.nameHandle = m_temp.nameHandle;
            return( true );
        }
        else if( allowReplace )
        {   //  item found
            m_items[ index ].resource.share( resource );
            return( true );
        }
    }
    return( false );
}

const void* CNamedResources::getKey( const int index ) const
{
    return( &m_items[ index ] );
}

int CNamedResources::cmpKey( const void* const key, const int index ) const
{
    const Item& item1 = *reinterpret_cast< const Item* >( key );
    const Item& item2 = m_items[ index ];
    int delta = static_cast< int >( item1.resourceType - item2.resourceType );
    if( delta == 0 )
    {
        delta = static_cast< int >( item1.nameHandle - item2.nameHandle );
    }
    return( delta );
}

void CNamedResources::moveItem( const int source, const int target )
{
    Item& sourceItem = ( ( source < 0 ) ? m_temp : m_items[ source ] );
    Item& targetItem = ( ( target < 0 ) ? m_temp : m_items[ target ] );
    targetItem = sourceItem;
    sourceItem.resource.release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource registry class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CResourceRegistry::create( CSystemFactory& factory, StringTableBlob& strings, ResourceRegistry& resourceRegistry )
{
    ASSERT( m_refs == 0 );
    if( m_refs == 0 )
    {
        m_resources.init( strings, 4 );
        m_refs = 1;
        m_factory = &factory;
        factory.addRef();
        resourceRegistry.release();
        resourceRegistry.naked() = static_cast< IShared* >( this );
        return( true );
    }
    return( false );
}

void CResourceRegistry::destroy()
{
    ASSERT( m_refs == 0 );
    m_refs = 0;
    m_reserved = 0;
    if( m_factory )
    {
        m_factory->release();
        m_factory = NULL;
    }
    m_resources.kill();
}

void CResourceRegistry::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceRegistry::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            delete this;
        }
    }
}

bool CResourceRegistry::add( const StringItem& name, const ResourceStream& resourceStream, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeStream, resourceStream, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceOutput& resourceOutput, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeOutput, resourceOutput, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceConsts& resourceConsts, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeConsts, resourceConsts, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceSRV& resourceSRV, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeSRV, resourceSRV, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceUAV& resourceUAV, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeUAV, resourceUAV, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceRTV& resourceRTV, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeRTV, resourceRTV, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceDSV& resourceDSV, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeDSV, resourceDSV, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceSampler& resourceSampler, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeSampler, resourceSampler, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceRasterState& resourceRasterState, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeRasterState, resourceRasterState, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceDepthState& resourceDepthState, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeDepthState, resourceDepthState, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceBlendState& resourceBlendState, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeBlendState, resourceBlendState, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceParams& resourceParams, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeParams, resourceParams, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceIndices& resourceIndices, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeIndices, resourceIndices, allowReplace ) : false );
}

bool CResourceRegistry::add( const StringItem& name, const ResourceCommand& resourceCommand, const bool allowReplace )
{
    return( valid() ? m_resources.addResource( name, EResourceTypeCommand, resourceCommand, allowReplace ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceStream& resourceStream )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeStream, resourceStream ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceOutput& resourceOutput )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeOutput, resourceOutput ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceConsts& resourceConsts )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeConsts, resourceConsts ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceSRV& resourceSRV )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeSRV, resourceSRV ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceUAV& resourceUAV )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeUAV, resourceUAV ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceRTV& resourceRTV )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeRTV, resourceRTV ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceDSV& resourceDSV )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeDSV, resourceDSV ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceSampler& resourceSampler )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeSampler, resourceSampler ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceRasterState& resourceRasterState )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeRasterState, resourceRasterState ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceDepthState& resourceDepthState )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeDepthState, resourceDepthState ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceBlendState& resourceBlendState )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeBlendState, resourceBlendState ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceParams& resourceParams )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeParams, resourceParams ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceIndices& resourceIndices )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeIndices, resourceIndices ) : false );
}

bool CResourceRegistry::find( const StringItem& name, ResourceCommand& resourceCommand )
{
    return( valid() ? m_resources.getResource( name, EResourceTypeCommand, resourceCommand ) : false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource library class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CResourceLibrary::create( CSystemFactory& factory, RefBlob& blob, ResourceLibrary& resourceLibrary )
{
    ASSERT( m_refs == 0 );
    if( m_refs == 0 )
    {
        void* buffer = blob.getBuffer();
        uint size = blob.getSize();
        ASSERT( ( buffer != NULL ) && ( size > sizeof( binary::ResourceLibrary ) ) );
        if( ( buffer != NULL ) && ( size > sizeof( binary::ResourceLibrary ) ) )
        {
            binary::ResourceLibrary& binaryLibrary = *reinterpret_cast< binary::ResourceLibrary* >( buffer );
            if( ( binaryLibrary.header.magic == s_magic ) && ( binaryLibrary.header.platform == s_platform ) && ( binaryLibrary.header.version <= 0x0001ffff ) && ( binaryLibrary.header.fileSize <= size ) )
            {   //  the data is a resources library file in a compatible format and appears to be complete
                blob.addRef();
                factory.addRef();
                m_blob = &blob;
                m_factory = &factory;
                m_data.binary.library = &binaryLibrary;
                m_data.stringTable = reinterpret_cast< FrozenStringTable* >( m_data.binary.data + binaryLibrary.index.stringTable.offset );
                m_data.samplerDescs = reinterpret_cast< CSamplerDesc* >( m_data.binary.data + binaryLibrary.index.samplerDescs.offset );
                m_data.rasterStateDescs = reinterpret_cast< CRasterStateDesc* >( m_data.binary.data + binaryLibrary.index.rasterStateDescs.offset );
                m_data.depthStateDescs = reinterpret_cast< CDepthStateDesc* >( m_data.binary.data + binaryLibrary.index.depthStateDescs.offset );
                m_data.blendStateDescs = reinterpret_cast< CMultiBlendStateDesc* >( m_data.binary.data + binaryLibrary.index.blendStateDescs.offset );
                m_data.samplerLinks = reinterpret_cast< binary::LinkDesc* >( m_data.binary.data + binaryLibrary.index.samplerLinks.offset );
                m_data.rasterStateLinks = reinterpret_cast< binary::LinkDesc* >( m_data.binary.data + binaryLibrary.index.rasterStateLinks.offset );
                m_data.depthStateLinks = reinterpret_cast< binary::LinkDesc* >( m_data.binary.data + binaryLibrary.index.depthStateLinks.offset );
                m_data.blendStateLinks = reinterpret_cast< binary::LinkDesc* >( m_data.binary.data + binaryLibrary.index.blendStateLinks.offset );
                m_data.stringTableSize = binaryLibrary.index.stringTable.size;
                m_data.samplerDescCount = binaryLibrary.index.samplerDescs.count;
                m_data.rasterStateDescCount = binaryLibrary.index.rasterStateDescs.count;
                m_data.depthStateDescCount = binaryLibrary.index.depthStateDescs.count;
                m_data.blendStateDescCount = binaryLibrary.index.blendStateDescs.count;
                m_data.samplerLinkCount = binaryLibrary.index.samplerLinks.count;
                m_data.rasterStateLinkCount = binaryLibrary.index.rasterStateLinks.count;
                m_data.depthStateLinkCount = binaryLibrary.index.depthStateLinks.count;
                m_data.blendStateLinkCount = binaryLibrary.index.blendStateLinks.count;
                m_refs = 1;
                resourceLibrary.release();
                resourceLibrary.naked() = static_cast< IShared* >( this );
                return( true );
            }
        }
        destroy();
    }
    return( false );
}

void CResourceLibrary::destroy()
{
    ASSERT( m_refs == 0 );
    if( m_factory != NULL )
    {
        m_factory->release();
        m_factory = NULL;
    }
    if( m_blob != NULL )
    {
        m_blob->release();
        m_blob = NULL;
    }
    zero();
}

void CResourceLibrary::zero()
{
    m_refs = 0;
    m_reserved = 0;
    m_blob = NULL;
    m_factory = NULL;
    memset( &m_data, 0, sizeof( m_data ) );
}

int CResourceLibrary::find( const uint32_t linkCount, const binary::LinkDesc* const linkDescs, const StringItem& name ) const
{
    if( linkCount )
    {
        if( name.notEmpty() )
        {
            uint length = 0;
            const uint8_t* string = name.getString( length );
            int stringIndex = m_data.stringTable->findString( string, length );
            if( stringIndex >= 0 )
            {   //  the library contains an item matching the given name, now check if it is in the specified link array
                uint32_t nameHandle = static_cast< uint32_t >( stringIndex );
                int index = 0;
                int limit = static_cast< int >( linkCount - 1 );
                while( index < limit )
                {
                    int check = ( ( index + limit ) >> 1 );
                    if( nameHandle > linkDescs[ check ].nameHandle )
                    {
                        index = ( check + 1 );
                    }
                    else
                    {
                        limit = check;
                    }
                }
                if( nameHandle == linkDescs[ index ].nameHandle )
                {   //  found the state
                    return( index );
                }
            }
        }
    }
    return( -1 );
}

void CResourceLibrary::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceLibrary::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            delete this;
        }
    }
}

int CResourceLibrary::getSamplerCount() const
{
    return( valid() ? static_cast< int >( m_data.samplerLinkCount ) : 0 );
}

int CResourceLibrary::getRasterStateCount() const
{
    return( valid() ? static_cast< int >( m_data.rasterStateLinkCount ) : 0 );
}

int CResourceLibrary::getDepthStateCount() const
{
    return( valid() ? static_cast< int >( m_data.depthStateLinkCount ) : 0 );
}

int CResourceLibrary::getBlendStateCount() const
{
    return( valid() ? static_cast< int >( m_data.blendStateLinkCount ) : 0 );
}

int CResourceLibrary::findSamplerIndex( const StringItem& name ) const
{
    return( valid() ? find( m_data.samplerLinkCount, m_data.samplerLinks, name ): -1 );
}

int CResourceLibrary::findRasterStateIndex( const StringItem& name ) const
{
    return( valid() ? find( m_data.rasterStateLinkCount, m_data.rasterStateLinks, name ): -1 );
}

int CResourceLibrary::findDepthStateIndex( const StringItem& name ) const
{
    return( valid() ? find( m_data.depthStateLinkCount, m_data.depthStateLinks, name ): -1 );
}

int CResourceLibrary::findBlendStateIndex( const StringItem& name ) const
{
    return( valid() ? find( m_data.blendStateLinkCount, m_data.blendStateLinks, name ): -1 );
}

bool CResourceLibrary::getSamplerName( const int index, StringItem& name ) const
{
    if( valid() && ( static_cast< uint32_t >( index ) < m_data.samplerLinkCount ) )
    {
        uint length = 0;
        const uint8_t* string = m_data.stringTable->getString( m_data.samplerLinks[ index ].nameHandle, length );
        name.setString( string, length );
        return( true );
    }
    return( false );
}

bool CResourceLibrary::getRasterStateName( const int index, StringItem& name ) const
{
    if( valid() && ( static_cast< uint32_t >( index ) < m_data.rasterStateLinkCount ) )
    {
        uint length = 0;
        const uint8_t* string = m_data.stringTable->getString( m_data.rasterStateLinks[ index ].nameHandle, length );
        name.setString( string, length );
        return( true );
    }
    return( false );
}

bool CResourceLibrary::getDepthStateName( const int index, StringItem& name ) const
{
    if( valid() && ( static_cast< uint32_t >( index ) < m_data.depthStateLinkCount ) )
    {
        uint length = 0;
        const uint8_t* string = m_data.stringTable->getString( m_data.depthStateLinks[ index ].nameHandle, length );
        name.setString( string, length );
        return( true );
    }
    return( false );
}

bool CResourceLibrary::getBlendStateName( const int index, StringItem& name ) const
{
    if( valid() && ( static_cast< uint32_t >( index ) < m_data.blendStateLinkCount ) )
    {
        uint length = 0;
        const uint8_t* string = m_data.stringTable->getString( m_data.blendStateLinks[ index ].nameHandle, length );
        name.setString( string, length );
        return( true );
    }
    return( false );
}

const CSamplerDesc* CResourceLibrary::getSamplerDesc( const int index ) const
{
    return( ( valid() && ( static_cast< uint32_t >( index ) < m_data.samplerLinkCount ) ) ? &m_data.samplerDescs[ m_data.samplerLinks[ index ].itemIndex ] : NULL );
}

const CRasterStateDesc* CResourceLibrary::getRasterStateDesc( const int index ) const
{
    return( ( valid() && ( static_cast< uint32_t >( index ) < m_data.rasterStateLinkCount ) ) ? &m_data.rasterStateDescs[ m_data.rasterStateLinks[ index ].itemIndex ] : NULL );
}

const CDepthStateDesc* CResourceLibrary::getDepthStateDesc( const int index ) const
{
    return( ( valid() && ( static_cast< uint32_t >( index ) < m_data.depthStateLinkCount ) ) ? &m_data.depthStateDescs[ m_data.depthStateLinks[ index ].itemIndex ] : NULL );
}

const CMultiBlendStateDesc* CResourceLibrary::getBlendStateDesc( const int index ) const
{
    return( ( valid() && ( static_cast< uint32_t >( index ) < m_data.blendStateLinkCount ) ) ? &m_data.blendStateDescs[ m_data.blendStateLinks[ index ].itemIndex ] : NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource type specific resource class generic function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CResourceLinear::create( CResourceLinears& pool)
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        return( true );
    }
    return( false );
}

bool CResourceSRV::create( CResourceSRVs& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        return( true );
    }
    return( false );
}

bool CResourceUAV::create( CResourceUAVs& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        return( true );
    }
    return( false );
}

bool CResourceRTV::create( CResourceRTVs& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        return( true );
    }
    return( false );
}

bool CResourceDSV::create( CResourceDSVs& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        return( true );
    }
    return( false );
}

bool CResourceSampler::create( CResourceSamplers& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        m_physical.release();
        return( true );
    }
    return( false );
}

bool CResourceRasterState::create( CResourceRasterStates& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        m_physical.release();
        return( true );
    }
    return( false );
}

bool CResourceDepthState::create( CResourceDepthStates& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        m_physical.release();
        return( true );
    }
    return( false );
}

bool CResourceBlendState::create( CResourceBlendStates& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        m_physical.release();
        return( true );
    }
    return( false );
}

bool CResourceCommand::create( CResourceCommands& pool )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        return( true );
    }
    return( false );
}


bool CResourceSampler::create( CResourceSamplers& pool, Physical& physical )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        m_physical.take( *reinterpret_cast< PhysicalSampler* >( &physical ) );
        m_resource.unknown = reinterpret_cast< const PhysicalDescSampler* >( m_physical.pipe() )->unknown;
        return( true );
    }
    return( false );
}

bool CResourceRasterState::create( CResourceRasterStates& pool, Physical& physical )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        m_physical.take( *reinterpret_cast< PhysicalRasterState* >( &physical ) );
        m_resource.unknown = reinterpret_cast< const PhysicalDescRasterState* >( m_physical.pipe() )->unknown;
        return( true );
    }
    return( false );
}

bool CResourceDepthState::create( CResourceDepthStates& pool, Physical& physical )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        m_physical.take( *reinterpret_cast< PhysicalDepthState* >( &physical ) );
        m_resource.unknown = reinterpret_cast< const PhysicalDescDepthState* >( m_physical.pipe() )->unknown;
        return( true );
    }
    return( false );
}

bool CResourceBlendState::create( CResourceBlendStates& pool, Physical& physical )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = &pool;
        m_physical.take( *reinterpret_cast< PhysicalBlendState* >( &physical ) );
        m_resource.unknown = reinterpret_cast< const PhysicalDescBlendState* >( m_physical.pipe() )->unknown;
        return( true );
    }
    return( false );
}

bool CResourceLinear::create( const CResourceLinear& other)
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceSRV::create( const CResourceSRV& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceUAV::create( const CResourceUAV& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceRTV::create( const CResourceRTV& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceDSV::create( const CResourceDSV& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceSampler::create( const CResourceSampler& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_physical.share( other.m_physical );
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceRasterState::create( const CResourceRasterState& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_physical.share( other.m_physical );
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceDepthState::create( const CResourceDepthState& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_physical.share( other.m_physical );
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceBlendState::create( const CResourceBlendState& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_physical.share( other.m_physical );
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

bool CResourceCommand::create( const CResourceCommand& other )
{
    ASSERT( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool == NULL ) && ( m_refs == 0 ) )
    {
        reset();
        m_refs = 1;
        m_pool = other.m_pool;
        m_resource = other.m_resource;
        return( true );
    }
    return( false );
}

void CResourceLinear::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        reset();
    }
}

void CResourceSRV::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        reset();
    }
}

void CResourceUAV::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        reset();
    }
}

void CResourceRTV::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        reset();
    }
}

void CResourceDSV::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        reset();
    }
}

void CResourceSampler::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        m_physical.release();
        reset();
    }
}

void CResourceRasterState::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        m_physical.release();
        reset();
    }
}

void CResourceDepthState::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        m_physical.release();
        reset();
    }
}

void CResourceBlendState::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        m_physical.release();
        reset();
    }
}

void CResourceCommand::destroy()
{
    ASSERT( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) );
    if( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs == 0 ) )
    {
        m_pool = NULL;
        reset();
    }
}

void CResourceLinear::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceSRV::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceUAV::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceRTV::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceDSV::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceSampler::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceRasterState::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceDepthState::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceBlendState::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceCommand::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CResourceLinear::release()
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

void CResourceSRV::release()
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

void CResourceUAV::release()
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

void CResourceRTV::release()
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

void CResourceDSV::release()
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

void CResourceSampler::release()
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

void CResourceRasterState::release()
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

void CResourceDepthState::release()
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

void CResourceBlendState::release()
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

void CResourceCommand::release()
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

bool CResourceLinear::clone( ResourceLinear& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceSRV::clone( ResourceSRV& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceUAV::clone( ResourceUAV& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceRTV::clone( ResourceRTV& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceDSV::clone( ResourceDSV& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceSampler::clone( ResourceSampler& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceRasterState::clone( ResourceRasterState& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceDepthState::clone( ResourceDepthState& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceBlendState::clone( ResourceBlendState& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

bool CResourceCommand::clone( ResourceCommand& resourceClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_pool->clone( *this, resourceClone ) );
    }
    resourceClone.release();
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource type specific resource class specific function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CResourceLinear::setBuffer( void* const buffer )
{
    m_resource.unknown = buffer;
}

void CResourceSRV::setSRV( void* const srv )
{
    m_resource.unknown = srv;
}

void CResourceUAV::setUAV( void* const uav )
{
    m_resource.unknown = uav;
}

void CResourceRTV::setRTV( void* const rtv )
{
    m_resource.unknown = rtv;
}

void CResourceDSV::setDSV( void* const dsv )
{
    m_resource.unknown = dsv;
}

void CResourceCommand::setNull()
{
    m_resource.indices.release();
    m_resource.indirect.release();
    reset();
}

void CResourceCommand::setPrim( const PrimDesc& primDesc )
{
    m_resource.primDesc = primDesc;
}

void CResourceCommand::setDraw()
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncAuto;
}

void CResourceCommand::setDraw( const DrawingParams& drawing )
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncDrawing;
    m_resource.drawing = drawing;
}

void CResourceCommand::setDraw( const DrawingParams& drawing, const InstancingParams& instancing )
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncInstancedDrawing;
    m_resource.drawing = drawing;
    m_resource.instancing = instancing;
}

void CResourceCommand::setDraw( const IndexedParams& indexed, const ResourceIndices& indices )
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncIndexed;
    m_resource.indexed = indexed;
    m_resource.indices.share( indices );
}

void CResourceCommand::setDraw( const IndexedParams& indexed, const ResourceIndices& indices, const InstancingParams& instancing )
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncInstancedIndexed;
    m_resource.indexed = indexed;
    m_resource.instancing = instancing;
    m_resource.indices.share( indices );
}

void CResourceCommand::setDraw( const ResourceParams& indirect )
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncInstancedDrawingIndirect;
    m_resource.indirect.share( indirect );
}

void CResourceCommand::setDraw( const ResourceParams& indirect, const ResourceIndices& indices )
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncInstancedIndexedIndirect;
    m_resource.indirect.share( indirect );
    m_resource.indices.share( indices );
}

void CResourceCommand::setDispatch( const DispatchParams& dispatch )
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncDispatch;
    m_resource.dispatch = dispatch;
}

void CResourceCommand::setDispatch( const ResourceParams& indirect )
{
    m_resource.indices.release();
    m_resource.indirect.release();
    m_resource.func = ECmdFuncDispatchIndirect;
    m_resource.indirect.share( indirect );
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

