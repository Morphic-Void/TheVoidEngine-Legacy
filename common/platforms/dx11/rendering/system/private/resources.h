
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   resources.h
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
////    Manifest:
////
////        template    TResourcePool           :   templated resource pool class
////        class       CNamedResources         :   concrete named resource collection class
////        class       CResourceRegistry       :   concrete resource registry class
////        class       CResourceLibrary        :   concrete resource description library class
////        class       CResourceLinears        :   concrete linear resource proxy pool class (used for params, consts, streams, outputs and indices)
////        class       CResourceSRVs           :   concrete SRV resource proxy pool class
////        class       CResourceUAVs           :   concrete UAV resource proxy pool class
////        class       CResourceRTVs           :   concrete RTV resource proxy pool class
////        class       CResourceDSVs           :   concrete DSV resource proxy pool class
////        class       CResourceSamplers       :   concrete sampler resource proxy pool class
////        class       CResourceRasterStates   :   concrete raster state resource proxy pool class
////        class       CResourceDepthStates    :   concrete depth state resource proxy pool class
////        class       CResourceBlendStates    :   concrete blend state resource proxy pool class
////        class       CResourceCommands       :   concrete command resource proxy pool class
////        class       CResourceLinear         :   concrete linear resource proxy class (used for params, consts, streams, outputs and indices)
////        class       CResourceSRV            :   concrete SRV resource proxy class
////        class       CResourceUAV            :   concrete UAV resource proxy class
////        class       CResourceRTV            :   concrete RTV resource proxy class
////        class       CResourceDSV            :   concrete DSV resource proxy class
////        class       CResourceSampler        :   concrete sampler resource proxy class
////        class       CResourceRasterState    :   concrete raster state resource proxy class
////        class       CResourceDepthState     :   concrete depth state resource proxy class
////        class       CResourceBlendState     :   concrete blend state resource proxy class
////        class       CResourceCommand        :   concrete command resource proxy class
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

#ifndef	__PRIVATE_RESOURCES_INCLUDED__
#define	__PRIVATE_RESOURCES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/memory/tslot.h"
#include "libs/memory/ref_blob.h"
#include "libs/memory/array_sort.h"
#include "libs/strings/string_table_blob.h"
#include "resource_types.h"
#include "resource_structs.h"
#include "resource_binary.h"
#include "physicals_api.h"

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
////    Forward declarations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSystemFactory;
class CResourceLinear;
class CResourceSRV;
class CResourceUAV;
class CResourceRTV;
class CResourceDSV;
class CResourceSampler;
class CResourceRasterState;
class CResourceDepthState;
class CResourceBlendState;
class CResourceCommand;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TResourcePool resource management class template
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename TResource >
class TResourcePool
{
public:
    inline                              TResourcePool() : m_factory( NULL ), m_items() {};
    inline                              TResourcePool( CSystemFactory& factory, const uint count ) : m_factory( NULL ), m_items() { init( factory, count ); };
    inline                              ~TResourcePool() { kill(); };
    inline bool                         valid() const { return( ( this != NULL ) && ( m_factory != NULL ) ); };
    bool                                init( CSystemFactory& factory, const uint count = 32 );
    void                                kill();
    bool                                create( Resource& resource );
    bool                                create( Physical& physical, Resource& resource );
    void                                destroy( TResource& item );
    bool                                clone( const TResource& resource, Resource& resourceClone );
    CSystemFactory*                     getFactory() const { return( m_factory ); };
protected:
    CSystemFactory*                     m_factory;
    TSlot< TResource >                  m_items;
private:
    inline                              TResourcePool( const TResourcePool& ) {};
    inline void                         operator=( const TResourcePool& ) const {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TResourcePool resource management class template public function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename TResource >
bool TResourcePool< TResource >::init( CSystemFactory& factory, const uint count )
{
    if( this != NULL )
    {
        kill();
        uint size = ( ( count < 4 ) ? 4 : count );
        m_items.init( size );
        m_factory = &factory;
        return( true );
    }
    return( false );
}

template< typename TResource >
void TResourcePool< TResource >::kill()
{
    if( this != NULL )
    {
        m_factory = NULL;
        m_items.kill();
    }
}

template< typename TResource >
bool TResourcePool< TResource >::create( Resource& resource )
{
    resource.release();
    if( valid() )
    {
        TResource* item = &m_items.fetch();
        if( item->create( *this ) )
        {
            m_factory->addRef();
            resource.naked() = static_cast< IShared* >( item );
            return( true );
        }
        m_items.recycle( *item );
    }
    return( false );
}

template< typename TResource >
bool TResourcePool< TResource >::create( Physical& physical, Resource& resource )
{
    resource.release();
    if( valid() )
    {
        TResource* item = &m_items.fetch();
        if( item->create( *this, physical ) )
        {
            m_factory->addRef();
            resource.naked() = static_cast< IShared* >( item );
            return( true );
        }
        m_items.recycle( *item );
    }
    return( false );
}

template< typename TResource >
void TResourcePool< TResource >::destroy( TResource& item )
{
    if( valid() && m_items.contains( item ) )
    {
        item.destroy();
        m_items.recycle( item );
        m_factory->release();
    }
}

template< typename TResource >
bool TResourcePool< TResource >::clone( const TResource& resource, Resource& resourceClone )
{
    resourceClone.release();
    if( valid() )
    {
        TResource* item = &m_items.fetch();
        if( item->create( *reinterpret_cast< const TResource* >( &resource ) ) )
        {
            m_factory->addRef();
            resourceClone.naked() = static_cast< IShared* >( item );
            return( true );
        }
        m_items.recycle( *item );
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Named resources class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CNamedResources : public IArrayQuery
{
public:
    inline                              CNamedResources() : m_sorting(), m_strings( NULL ), m_items( NULL ), m_null(), m_size( 0 ) {};
    inline                              CNamedResources( StringTableBlob& strings, const int count = 32 ) : m_sorting(), m_strings( NULL ), m_items( NULL ), m_null(), m_size( 0 ) { init( strings, count ); };
    inline                              ~CNamedResources() { kill(); };
    inline bool                         valid() const { return( ( m_strings && m_items ) ? true : false ); };
    inline bool                         valid( const int index ) const { return( ( static_cast< uint >( index ) < static_cast< uint >( getCount() ) ) ? true : false ); };
    bool                                init( StringTableBlob& strings, const int count = 32 );
    void                                kill();
    int                                 findIndex( const StringItem& name, const uint32_t resourceType ) const;
    inline int                          getCount() const { return( valid() ? m_sorting.used() : 0 ); };
    bool                                getName( const int index, StringItem& name ) const;
    const uint32_t                      getType( const int index ) const;
    const Resource&                     getResource( const int index ) const;
    bool                                getResource( const StringItem& name, const uint32_t resourceType, Resource& resource ) const;
    bool                                addResource( const StringItem& name, const uint32_t resourceType, const Resource& resource, const bool allowReplace = false );
protected:
    struct                              Item { Resource resource; uint32_t resourceType; uint32_t nameHandle; };
    virtual const void*                 getKey( const int index ) const;
    virtual int                         cmpKey( const void* const key, const int index ) const;
    virtual void                        moveItem( const int source, const int target );
    ArraySort                           m_sorting;
    StringTableBlob*                    m_strings;
    Item*                               m_items;
    mutable Item                        m_temp;
    Resource                            m_null;
    int                                 m_size;
private:
    inline                              CNamedResources( const CNamedResources& ) {};
    inline void                         operator=( const CNamedResources& ) {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource registry class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CResourceRegistry : public IResourceRegistry
{
public:
    inline                              CResourceRegistry() : m_refs( 0 ), m_reserved( 0 ), m_factory( NULL ) {};
    inline                              ~CResourceRegistry() { m_refs = 0; m_reserved = 0; destroy(); };
    inline bool                         valid() const { return( m_refs ? true : false ); };
    bool                                create( CSystemFactory& factory, StringTableBlob& strings, ResourceRegistry& resourceRegistry );
    void                                destroy();
    inline CSystemFactory*              getFactory() const { return( m_factory ); };
public:
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        add( const StringItem& name, const ResourceStream& resourceStream, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceOutput& resourceOutput, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceConsts& resourceConsts, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceSRV& resourceSRV, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceUAV& resourceUAV, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceRTV& resourceRTV, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceDSV& resourceDSV, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceSampler& resourceSampler, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceRasterState& resourceRasterState, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceDepthState& resourceDepthState, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceBlendState& resourceBlendState, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceParams& resourceParams, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceIndices& resourceIndices, const bool allowReplace = false );
    virtual bool                        add( const StringItem& name, const ResourceCommand& resourceCommand, const bool allowReplace = false );
    virtual bool                        find( const StringItem& name, ResourceStream& resourceStream );
    virtual bool                        find( const StringItem& name, ResourceOutput& resourceOutput );
    virtual bool                        find( const StringItem& name, ResourceConsts& resourceConsts );
    virtual bool                        find( const StringItem& name, ResourceSRV& resourceSRV );
    virtual bool                        find( const StringItem& name, ResourceUAV& resourceUAV );
    virtual bool                        find( const StringItem& name, ResourceRTV& resourceRTV );
    virtual bool                        find( const StringItem& name, ResourceDSV& resourceDSV );
    virtual bool                        find( const StringItem& name, ResourceSampler& resourceSampler );
    virtual bool                        find( const StringItem& name, ResourceRasterState& resourceRasterState );
    virtual bool                        find( const StringItem& name, ResourceDepthState& resourceDepthState );
    virtual bool                        find( const StringItem& name, ResourceBlendState& resourceBlendState );
    virtual bool                        find( const StringItem& name, ResourceParams& resourceParams );
    virtual bool                        find( const StringItem& name, ResourceIndices& resourceIndices );
    virtual bool                        find( const StringItem& name, ResourceCommand& resourceCommand );
protected:
    uint32_t                            m_refs;
    uint32_t                            m_reserved;
    CSystemFactory*                     m_factory;
    CNamedResources                     m_resources;
private:
    inline                              CResourceRegistry( const CResourceRegistry& ) {};
    inline void                         operator=( const CResourceRegistry& ) const {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource library class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CResourceLibrary : public IResourceLibrary
{
public:
    inline                              CResourceLibrary() : m_refs( 0 ), m_reserved( 0 ) { zero(); };
    inline                              ~CResourceLibrary() { m_refs = 0; m_reserved = 0; destroy(); };
    inline bool                         valid() const { return( m_refs ? true : false ); };
    bool                                create( CSystemFactory& factory, RefBlob& blob, ResourceLibrary& resourceLibrary );
    void                                destroy();
    inline CSystemFactory*              getFactory() const { return( m_factory ); };
public:
    virtual void                        addRef();
    virtual void                        release();
    virtual int                         getSamplerCount() const;
    virtual int                         getRasterStateCount() const;
    virtual int                         getDepthStateCount() const;
    virtual int                         getBlendStateCount() const;
    virtual int                         findSamplerIndex( const StringItem& name ) const;
    virtual int                         findRasterStateIndex( const StringItem& name ) const;
    virtual int                         findDepthStateIndex( const StringItem& name ) const;
    virtual int                         findBlendStateIndex( const StringItem& name ) const;
    virtual bool                        getSamplerName( const int index, StringItem& name ) const;
    virtual bool                        getRasterStateName( const int index, StringItem& name ) const;
    virtual bool                        getDepthStateName( const int index, StringItem& name ) const;
    virtual bool                        getBlendStateName( const int index, StringItem& name ) const;
    virtual const CSamplerDesc*         getSamplerDesc( const int index ) const;
    virtual const CRasterStateDesc*     getRasterStateDesc( const int index ) const;
    virtual const CDepthStateDesc*      getDepthStateDesc( const int index ) const;
    virtual const CMultiBlendStateDesc* getBlendStateDesc( const int index ) const;
protected:
    int                                 find( const uint32_t linkCount, const binary::LinkDesc* const linkDescs, const StringItem& name ) const;
    uint32_t                            m_refs;
    uint32_t                            m_reserved;
    RefBlob*                            m_blob;
    CSystemFactory*                     m_factory;
    ResourceLibraryData                 m_data;
    static const uint32_t               s_magic = MAKE4CC( 'i', 's', 'R', 'L' );
    static const uint32_t               s_platform = MAKE4CC( 'D', 'X', '1', '1' );
private:
    void                                zero();
    inline                              CResourceLibrary( const CResourceLibrary& ) {};
    inline void                         operator=( const CResourceLibrary& ) const {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource type specific resource pool classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CResourceLinears      = TResourcePool< CResourceLinear >;
using CResourceSRVs         = TResourcePool< CResourceSRV >;
using CResourceUAVs         = TResourcePool< CResourceUAV >;
using CResourceRTVs         = TResourcePool< CResourceRTV >;
using CResourceDSVs         = TResourcePool< CResourceDSV >;
using CResourceSamplers     = TResourcePool< CResourceSampler >;
using CResourceRasterStates = TResourcePool< CResourceRasterState >;
using CResourceDepthStates  = TResourcePool< CResourceDepthState >;
using CResourceBlendStates  = TResourcePool< CResourceBlendState >;
using CResourceCommands     = TResourcePool< CResourceCommand >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource type specific resource classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CResourceLinear : public IResourceLinear
{
public:
    inline                              CResourceLinear() : m_refs( 0 ), m_type( EResourceTypeLinear ), m_pool( NULL ) { reset(); };
    inline                              ~CResourceLinear() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceLinears& pool );
    bool                                create( const CResourceLinear& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceLinear& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual void                        setBuffer( void* const buffer );
    virtual void                        setOffset( const UINT offset ) { m_resource.offset = offset; };
    virtual void                        setStride( const UINT stride ) { m_resource.stride = stride; };
    virtual UINT                        getOffset() const { return( m_resource.offset ); };
    virtual UINT                        getStride() const { return( m_resource.stride ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceLinears*                   m_pool;
    ResourceDescLinear                  m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceLinear( const CResourceLinear& ) {};
    inline void                         operator=( const CResourceLinear& ) const {};
};

class CResourceSRV : public IResourceSRV
{
public:
    inline                              CResourceSRV() : m_refs( 0 ), m_type( EResourceTypeSRV ), m_pool( NULL ) { reset(); };
    inline                              ~CResourceSRV() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceSRVs& pool );
    bool                                create( const CResourceSRV& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceSRV& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual void                        setSRV( void* const srv );
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceSRVs*                      m_pool;
    ResourceDescSRV                     m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceSRV( const CResourceSRV& ) {};
    inline void                         operator=( const CResourceSRV& ) const {};
};

class CResourceUAV : public IResourceUAV
{
public:
    inline                              CResourceUAV() : m_refs( 0 ), m_type( EResourceTypeUAV ), m_pool( NULL ) { reset(); };
    inline                              ~CResourceUAV() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceUAVs& pool );
    bool                                create( const CResourceUAV& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceUAV& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual void                        setUAV( void* const uav );
    virtual void                        setInitialCount( const UINT initialCount ) { m_resource.initialCount = initialCount; };
    virtual UINT                        getInitialCount() const { return( m_resource.initialCount ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceUAVs*                      m_pool;
    ResourceDescUAV                     m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceUAV( const CResourceUAV& ) {};
    inline void                         operator=( const CResourceUAV& ) const {};
};

class CResourceRTV : public IResourceRTV
{
public:
    inline                              CResourceRTV() : m_refs( 0 ), m_type( EResourceTypeRTV ), m_pool( NULL ) { reset(); };
    inline                              ~CResourceRTV() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceRTVs& pool );
    bool                                create( const CResourceRTV& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceRTV& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual void                        setRTV( void* const rtv );
    virtual void                        setViewport( const FBOX& viewport ) { m_resource.viewport = viewport; };
    virtual void                        setScissoring( const SRECT& scissoring ) { m_resource.scissoring = scissoring; };
    virtual const FBOX&                 getViewport() const { return( m_resource.viewport ); };
    virtual const SRECT&                getScissoring() const { return( m_resource.scissoring ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceRTVs*                      m_pool;
    ResourceDescRTV                     m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceRTV( const CResourceRTV& ) {};
    inline void                         operator=( const CResourceRTV& ) const {};
};

class CResourceDSV : public IResourceDSV
{
public:
    inline                              CResourceDSV() : m_refs( 0 ), m_type( EResourceTypeDSV ), m_pool( NULL ) { reset(); };
    inline                              ~CResourceDSV() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceDSVs& pool );
    bool                                create( const CResourceDSV& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceDSV& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual void                        setDSV( void* const dsv );
    virtual void                        setViewport( const FBOX& viewport ) { m_resource.viewport = viewport; };
    virtual void                        setScissoring( const SRECT& scissoring ) { m_resource.scissoring = scissoring; };
    virtual const FBOX&                 getViewport() const { return( m_resource.viewport ); };
    virtual const SRECT&                getScissoring() const { return( m_resource.scissoring ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceDSVs*                      m_pool;
    ResourceDescDSV                     m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceDSV( const CResourceDSV& ) {};
    inline void                         operator=( const CResourceDSV& ) const {};
};

class CResourceSampler : public IResourceSampler
{
public:
    inline                              CResourceSampler() : m_refs( 0 ), m_type( EResourceTypeSampler ), m_pool( NULL ), m_physical() { reset(); };
    inline                              ~CResourceSampler() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceSamplers& pool );
    bool                                create( CResourceSamplers& pool, Physical& physical );
    bool                                create( const CResourceSampler& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceSampler& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual const CSamplerDesc&         desc() const { return( *m_physical.desc() ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceSamplers*                  m_pool;
    PhysicalSampler                     m_physical;
    ResourceDescSampler                 m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceSampler( const CResourceSampler& ) {};
    inline void                         operator=( const CResourceSampler& ) const {};
};

class CResourceRasterState : public IResourceRasterState
{
public:
    inline                              CResourceRasterState() : m_refs( 0 ), m_type( EResourceTypeRasterState ), m_pool( NULL ), m_physical() { reset(); };
    inline                              ~CResourceRasterState() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceRasterStates& pool );
    bool                                create( CResourceRasterStates& pool, Physical& physical );
    bool                                create( const CResourceRasterState& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceRasterState& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual const CRasterStateDesc&     desc() const { return( *m_physical.desc() ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceRasterStates*              m_pool;
    PhysicalRasterState                 m_physical;
    ResourceDescRasterState             m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceRasterState( const CResourceRasterState& ) {};
    inline void                         operator=( const CResourceRasterState& ) const {};
};

class CResourceDepthState : public IResourceDepthState
{
public:
    inline                              CResourceDepthState() : m_refs( 0 ), m_type( EResourceTypeDepthState ), m_pool( NULL ), m_physical() { reset(); };
    inline                              ~CResourceDepthState() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceDepthStates& pool );
    bool                                create( CResourceDepthStates& pool, Physical& physical );
    bool                                create( const CResourceDepthState& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceDepthState& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual const CDepthStateDesc&      desc() const { return( *m_physical.desc() ); };
    virtual void                        setStencilRef( const UINT stencilRef ) { m_resource.stencilRef = stencilRef; };
    virtual UINT                        getStencilRef() const { return( m_resource.stencilRef ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceDepthStates*               m_pool;
    PhysicalDepthState                  m_physical;
    ResourceDescDepthState              m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceDepthState( const CResourceDepthState& ) {};
    inline void                         operator=( const CResourceDepthState& ) const {};
};

class CResourceBlendState : public IResourceBlendState
{
public:
    inline                              CResourceBlendState() : m_refs( 0 ), m_type( EResourceTypeBlendState ), m_pool( NULL ), m_physical() { reset(); };
    inline                              ~CResourceBlendState() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceBlendStates& pool );
    bool                                create( CResourceBlendStates& pool, Physical& physical );
    bool                                create( const CResourceBlendState& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceBlendState& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual const CMultiBlendStateDesc& desc() const { return( *m_physical.desc() ); };
    virtual void                        setSampleMask( const UINT sampleMask ) { m_resource.sampleMask = sampleMask; };
    virtual void                        setBlendFactor( const FVEC4& blendFactor ) { m_resource.blendFactor = blendFactor; };
    virtual UINT                        getSampleMask() const { return( m_resource.sampleMask ); };
    virtual const FVEC4&                getBlendFactor() const { return( m_resource.blendFactor ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceBlendStates*               m_pool;
    PhysicalBlendState                  m_physical;
    ResourceDescBlendState              m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceBlendState( const CResourceBlendState& ) {};
    inline void                         operator=( const CResourceBlendState& ) const {};
};

class CResourceCommand : public IResourceCommand
{
public:
    inline                              CResourceCommand() : m_refs( 0 ), m_type( EResourceTypeCommand ), m_pool( NULL ) { reset(); };
    inline                              ~CResourceCommand() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CResourceCommands& pool );
    bool                                create( const CResourceCommand& other );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual bool                        clone( ResourceCommand& resourceClone ) const;
    virtual const void*                 pipe() const { return( &m_resource ); };
    virtual void                        setNull();
    virtual void                        setPrim( const PrimDesc& primDesc );
    virtual void                        setDraw();
    virtual void                        setDraw( const DrawingParams& drawing );
    virtual void                        setDraw( const DrawingParams& drawing, const InstancingParams& instancing );
    virtual void                        setDraw( const IndexedParams& indexed, const ResourceIndices& indices );
    virtual void                        setDraw( const IndexedParams& indexed, const ResourceIndices& indices, const InstancingParams& instancing );
    virtual void                        setDraw( const ResourceParams& indirect );
    virtual void                        setDraw( const ResourceParams& indirect, const ResourceIndices& indices );
    virtual void                        setDispatch( const DispatchParams& dispatch );
    virtual void                        setDispatch( const ResourceParams& indirect );
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CResourceCommands*                  m_pool;
    ResourceDescCommand                 m_resource;
private:
    inline void                         reset() { m_resource.reset(); };
    inline                              CResourceCommand( const CResourceCommand& ) {};
    inline void                         operator=( const CResourceCommand& ) const {};
};

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
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__PUBLIC_RESOURCES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
