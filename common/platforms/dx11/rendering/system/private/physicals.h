
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   physicals.h
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
////    Manifest:
////
////        template    TPhysicalPool      :   templated physical state pool class
////        class       CPhysicalBuffers        :   concrete physical memory buffer pool class
////        class       CPhysicalAccessors      :   concrete physical memory access pool class
////        class       CPhysicalSamplers       :   concrete physical sampler state pool class
////        class       CPhysicalRasterStates   :   concrete physical raster state pool class
////        class       CPhysicalDepthStates    :   concrete physical depth state pool class
////        class       CPhysicalBlendStates    :   concrete physical blend state pool class
////        class       CPhysicalBuffer         :   concrete physical memory buffer class
////        class       CPhysicalAccess         :   concrete physical memory access class
////        class       CPhysicalSampler        :   concrete physical sampler state class
////        class       CPhysicalRasterState    :   concrete physical raster state class
////        class       CPhysicalDepthState     :   concrete physical depth state class
////        class       CPhysicalBlendState     :   concrete physical blend state class
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

#ifndef	__PRIVATE_PHYSICALS_INCLUDED__
#define	__PRIVATE_PHYSICALS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "physical_types.h"
#include "physical_structs.h"
#include "libs/memory/array_sort.h"
#include "libs/memory/tblob.h"
#include "libs/memory/tslot.h"

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
class CPhysicalBuffer;
class CPhysicalAccess;
class CPhysicalSampler;
class CPhysicalRasterState;
class CPhysicalDepthState;
class CPhysicalBlendState;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Buffer initialisation description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class CBufferInitDesc
//{
//public:
//    inline                  CBufferInitDesc( const EBufferAccess access, const CBufferDesc& desc, const CBufferData* data = NULL ) : m_data( data ), m_desc( desc ), m_access( access ) {};
//    inline                  ~CBufferInitDesc() {};
//    const CBufferDesc&      getDesc() const { return( m_desc ); };
//    const CBufferData*      getData() const { return( m_data ); };
//    EBufferAccess           getAccess() const { return( m_access ); };
//protected:
//    const CBufferData*      m_data;
//    const CBufferDesc&      m_desc;
//    EBufferAccess           m_access;
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Access initialisation description class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class CAccessInitDesc
//{
//public:
//    inline                  CAccessInitDesc( const CAccessDesc& desc ) : m_desc( desc ) {};
//    inline                  ~CAccessInitDesc() {};
//    const CAccessDesc&      getDesc() const { return( m_desc ); };
//protected:
//    const CAccessDesc&      m_desc;
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TPhysicalPoolState physical resource management class template
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename TPhysical, typename TDesc >
class TPhysicalPool : protected IArrayQuery
{
public:
    inline                      TPhysicalPool() : m_factory( NULL ), m_sorting(), m_links(), m_items() {};
    inline                      TPhysicalPool( CSystemFactory& factory, const uint count ) : m_factory( NULL ), m_sorting(), m_links(), m_items() { init( factory, count ); };
    inline                      ~TPhysicalPool() { kill(); };
    inline bool                 valid() const { return( ( this != NULL ) && ( m_factory != NULL ) ); };
    bool                        init( CSystemFactory& factory, const uint count = 32 );
    void                        kill();
    bool                        create( const TDesc& desc, Physical& physical );
    void                        destroy( TPhysical& item );
    CSystemFactory*             getFactory() const { return( m_factory ); };
protected:
    virtual const void*         getKey( const int index ) const;
    virtual int                 cmpKey( const void* const key, const int index ) const;
    virtual void                moveItem( const int source, const int target );
    CSystemFactory*             m_factory;
    ArraySort                   m_sorting;
    TBlob< TPhysical* >         m_links;
    TSlot< TPhysical >          m_items;
private:
    inline                      TPhysicalPool( const TPhysicalPool& ) {};
    inline void                 operator=( const TPhysicalPool& ) const {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TPhysicalPool physical resource management class template public function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename TPhysical, typename TDesc >
bool TPhysicalPool< TPhysical, TDesc >::init( CSystemFactory& factory, const uint count )
{
    if( this != NULL )
    {
        kill();
        uint size = ( ( count < 4 ) ? 4 : count );
        if( m_sorting.init( *this, size ) )
        {
            m_links.init( size );
            m_items.init( size );
            m_factory = &factory;
            return( true );
        }
        kill();
    }
    return( false );
}

template< typename TPhysical, typename TDesc >
void TPhysicalPool< TPhysical, TDesc >::kill()
{
    if( this != NULL )
    {
        m_factory = NULL;
        m_sorting.kill();
        m_links.kill();
        m_items.kill();
    }
}

template< typename TPhysical, typename TDesc >
bool TPhysicalPool< TPhysical, TDesc >::create( const TDesc& desc, Physical& physical )
{
    physical.release();
    if( valid() )
    {
        TPhysical* item = NULL;
        int index = m_sorting.find( &desc );
        if( index >= 0 )
        {
            item = m_links.getItem( static_cast< uint >( index ) );
            item->addRef();
            physical.naked() = static_cast< IShared* >( item );
            return( true );
        }
        item = &m_items.fetch();
        if( item->create( *this, desc ) )
        {
            m_links.setItem( static_cast< uint >( m_sorting.insert( &desc ) ), item );
            m_factory->addRef();
            physical.naked() = static_cast< IShared* >( item );
            return( true );
        }
        m_items.recycle( *item );
    }
    return( false );
}

template< typename TPhysical, typename TDesc >
void TPhysicalPool< TPhysical, TDesc >::destroy( TPhysical& item )
{
    if( valid() )
    {
        int index = m_sorting.find( &item.desc() );
        if( index >= 0 )
        {
            item.destroy();
            m_sorting.remove( index );
            m_links.setItem( static_cast< uint >( index ), NULL );
            m_items.recycle( item );
            m_factory->release();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TPhysicalPool physical resource management class template protected virtual function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename TPhysical, typename TDesc >
const void* TPhysicalPool< TPhysical, TDesc >::getKey( const int index ) const
{
    return( reinterpret_cast< const void* >( &m_links.getItem( index )->desc() ) );
}

template< typename TPhysical, typename TDesc >
int TPhysicalPool< TPhysical, TDesc >::cmpKey( const void* const key, const int index ) const
{
    return( memcmp( key, reinterpret_cast< const void* >( &m_links.getItem( index )->desc() ), sizeof( TDesc ) ) );
}

template< typename TPhysical, typename TDesc >
void TPhysicalPool< TPhysical, TDesc >::moveItem( const int source, const int target )
{
    UNUSED( source );
    UNUSED( target );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Physical resource type specific resource pool classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//using CPhysicalBuffers      = TPhysicalPool< CPhysicalBuffer,       CBufferInitDesc      >;
//using CPhysicalAccessors    = TPhysicalPool< CPhysicalAccess,       CAccessInitDesc      >;
using CPhysicalSamplers     = TPhysicalPool< CPhysicalSampler,      CSamplerDesc         >;
using CPhysicalRasterStates = TPhysicalPool< CPhysicalRasterState,  CRasterStateDesc     >;
using CPhysicalDepthStates  = TPhysicalPool< CPhysicalDepthState,   CDepthStateDesc      >;
using CPhysicalBlendStates  = TPhysicalPool< CPhysicalBlendState,   CMultiBlendStateDesc >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Physical resource type specific resource classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class CPhysicalBuffer : public IPhysicalBuffer
//{
//public:
//    inline                              CPhysicalBuffer() : m_refs( 0 ), m_type( EPhysicalTypeBuffer ), m_pool( NULL ) { reset(); };
//    inline                              ~CPhysicalBuffer() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
//    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
//    bool                                create( CPhysicalBuffers& pool, const CBufferInitDesc& init );
//    void                                destroy();
//    virtual void                        addRef();
//    virtual void                        release();
//    virtual const void*                 pipe() const { return( &m_physical ); };
//    virtual const CBufferDesc&          desc() const { return( m_physical.desc ); };
//protected:
//    uint32_t                            m_refs;
//    uint32_t                            m_type;
//    CPhysicalBuffers*                   m_pool;
//    PhysicalDescBuffer                  m_physical;
//private:
//    inline void                         reset() { m_physical.reset(); };
//    inline                              CPhysicalBuffer( const CPhysicalBuffer& ) {};
//    inline void                         operator=( const CPhysicalBuffer& ) const {};
//};
//
//class CPhysicalAccess : public IPhysicalAccess
//{
//public:
//    inline                              CPhysicalAccess() : m_refs( 0 ), m_type( EPhysicalTypeAccess ), m_pool( NULL ) { reset(); };
//    inline                              ~CPhysicalAccess() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
//    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
//    bool                                create( CPhysicalAccessors& pool, const CAccessInitDesc& init );
//    void                                destroy();
//    virtual void                        addRef();
//    virtual void                        release();
//    virtual const void*                 pipe() const { return( &m_physical ); };
//    virtual const CAccessDesc&          desc() const { return( m_physical.desc ); };
//protected:
//    uint32_t                            m_refs;
//    uint32_t                            m_type;
//    CPhysicalAccessors*                 m_pool;
//    PhysicalDescAccess                  m_physical;
//private:
//    inline void                         reset() { m_physical.reset(); };
//    inline                              CPhysicalAccess( const CPhysicalAccess& ) {};
//    inline void                         operator=( const CPhysicalAccess& ) const {};
//};

class CPhysicalSampler : public IPhysicalSampler
{
public:
    inline                              CPhysicalSampler() : m_refs( 0 ), m_type( EPhysicalTypeSampler ), m_pool( NULL ) { reset(); };
    inline                              ~CPhysicalSampler() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CPhysicalSamplers& pool, const CSamplerDesc& desc );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual const void*                 pipe() const { return( &m_physical ); };
    virtual const CSamplerDesc&         desc() const { return( m_physical.desc ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CPhysicalSamplers*                  m_pool;
    PhysicalDescSampler                 m_physical;
private:
    inline void                         reset() { m_physical.reset(); };
    inline                              CPhysicalSampler( const CPhysicalSampler& ) {};
    inline void                         operator=( const CPhysicalSampler& ) const {};
};

class CPhysicalRasterState : public IPhysicalRasterState
{
public:
    inline                              CPhysicalRasterState() : m_refs( 0 ), m_type( EPhysicalTypeRasterState ), m_pool( NULL ) { reset(); };
    inline                              ~CPhysicalRasterState() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CPhysicalRasterStates& pool, const CRasterStateDesc& desc );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual const void*                 pipe() const { return( &m_physical ); };
    virtual const CRasterStateDesc&     desc() const { return( m_physical.desc ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CPhysicalRasterStates*              m_pool;
    PhysicalDescRasterState             m_physical;
private:
    inline void                         reset() { m_physical.reset(); };
    inline                              CPhysicalRasterState( const CPhysicalRasterState& ) {};
    inline void                         operator=( const CPhysicalRasterState& ) const {};
};

class CPhysicalDepthState : public IPhysicalDepthState
{
public:
    inline                              CPhysicalDepthState() : m_refs( 0 ), m_type( EPhysicalTypeDepthState ), m_pool( NULL ) { reset(); };
    inline                              ~CPhysicalDepthState() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CPhysicalDepthStates& pool, const CDepthStateDesc& desc );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual const void*                 pipe() const { return( &m_physical ); };
    virtual const CDepthStateDesc&      desc() const { return( m_physical.desc ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CPhysicalDepthStates*               m_pool;
    PhysicalDescDepthState              m_physical;
private:
    inline void                         reset() { m_physical.reset(); };
    inline                              CPhysicalDepthState( const CPhysicalDepthState& ) {};
    inline void                         operator=( const CPhysicalDepthState& ) const {};
};

class CPhysicalBlendState : public IPhysicalBlendState
{
public:
    inline                              CPhysicalBlendState() : m_refs( 0 ), m_type( EPhysicalTypeBlendState ), m_pool( NULL ) { reset(); };
    inline                              ~CPhysicalBlendState() { m_refs = 0; m_type = 0; m_pool = NULL; reset(); };
    inline bool                         valid() const { return( ( ( this != NULL ) && ( m_pool != NULL ) && ( m_refs != 0 ) ) ? true : false ); };
    bool                                create( CPhysicalBlendStates& pool, const CMultiBlendStateDesc& desc );
    void                                destroy();
    virtual void                        addRef();
    virtual void                        release();
    virtual const void*                 pipe() const { return( &m_physical ); };
    virtual const CMultiBlendStateDesc& desc() const { return( m_physical.desc ); };
protected:
    uint32_t                            m_refs;
    uint32_t                            m_type;
    CPhysicalBlendStates*               m_pool;
    PhysicalDescBlendState              m_physical;
private:
    inline void                         reset() { m_physical.reset(); };
    inline                              CPhysicalBlendState( const CPhysicalBlendState& ) {};
    inline void                         operator=( const CPhysicalBlendState& ) const {};
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

#endif	//	#ifndef	__PRIVATE_PHYSICALS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
