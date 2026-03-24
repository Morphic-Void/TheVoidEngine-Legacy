
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   factory.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Private factory classes.
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

#include "libs/system/debug/asserts.h"
#include "shaders.h"
#include "factory.h"

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
////    System object factory class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CSystemFactory::create( ID3D11Device* device, ID3D11DeviceContext* context, SystemFactory& systemFactory )
{
    ASSERT( ( m_refs == 0 ) && ( m_device == NULL ) && ( m_context == NULL ) && ( device != NULL ) && ( context != NULL ) );
    if( ( m_refs == 0 ) && ( m_device == NULL ) && ( m_context == NULL ) && ( device != NULL ) && ( context != NULL ) )
    {
        m_refs = 1;
        m_device = device;
        m_context = context;
        m_strings.init( 4096, 64 );
        m_resourceLinears.init( *this, 32 );
        m_resourceSRVs.init( *this, 32 );
        m_resourceUAVs.init( *this, 32 );
        m_resourceRTVs.init( *this, 32 );
        m_resourceDSVs.init( *this, 32 );
        m_resourceSamplers.init( *this, 32 );
        m_resourceRasterStates.init( *this, 32 );
        m_resourceDepthStates.init( *this, 32 );
        m_resourceBlendStates.init( *this, 32 );
        m_resourceCommands.init( *this, 32 );
        m_physicalSamplers.init( *this, 32 );
        m_physicalRasterStates.init( *this, 32 );
        m_physicalDepthStates.init( *this, 32 );
        m_physicalBlendStates.init( *this, 32 );
        systemFactory.release();
        systemFactory.naked() = static_cast< IShared* >( this );
        return( true );
    }
    return( false );
}

void CSystemFactory::destroy()
{
    ASSERT( m_refs == 0 );
    m_refs = 0;
    m_reserved = 0;
    m_device = NULL;
    m_context = NULL;
    m_strings.kill();
    m_resourceLinears.kill();
    m_resourceSRVs.kill();
    m_resourceUAVs.kill();
    m_resourceRTVs.kill();
    m_resourceDSVs.kill();
    m_resourceSamplers.kill();
    m_resourceRasterStates.kill();
    m_resourceDepthStates.kill();
    m_resourceBlendStates.kill();
    m_resourceCommands.kill();
    m_physicalSamplers.kill();
    m_physicalRasterStates.kill();
    m_physicalDepthStates.kill();
    m_physicalBlendStates.kill();
}

void CSystemFactory::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CSystemFactory::release()
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

bool CSystemFactory::create( RefBlob& blob, ShaderLibrary& shaderLibrary )
{
    CShaderLibrary* library = new CShaderLibrary;
    if( library->create( *this, blob, shaderLibrary ) )
    {
        return( true );
    }
    delete library;
    return( false );
}

bool CSystemFactory::create( RefBlob& blob, ResourceLibrary& resourceLibrary )
{
    CResourceLibrary* library = new CResourceLibrary;
    if( library->create( *this, blob, resourceLibrary ) )
    {
        return( true );
    }
    delete library;
    return( false );
}

bool CSystemFactory::create( ResourceRegistry& resourceRegistry )
{
    CResourceRegistry* registry = new CResourceRegistry;
    if( registry->create( *this, m_strings, resourceRegistry ) )
    {
        return( true );
    }
    delete registry;
    return( false );
}

bool CSystemFactory::create( ResourceStream& resourceStream )
{
    return( valid() ? m_resourceLinears.create( resourceStream ) : false );
}

bool CSystemFactory::create( ResourceOutput& resourceOutput )
{
    return( valid() ? m_resourceLinears.create( resourceOutput ) : false );
}

bool CSystemFactory::create( ResourceConsts& resourceConsts )
{
    return( valid() ? m_resourceLinears.create( resourceConsts ) : false );
}

bool CSystemFactory::create( ResourceSRV& resourceSRV )
{
    return( valid() ? m_resourceSRVs.create( resourceSRV ) : false );
}

bool CSystemFactory::create( ResourceUAV& resourceUAV )
{
    return( valid() ? m_resourceUAVs.create( resourceUAV ) : false );
}

bool CSystemFactory::create( ResourceRTV& resourceRTV )
{
    return( valid() ? m_resourceRTVs.create( resourceRTV ) : false );
}

bool CSystemFactory::create( ResourceDSV& resourceDSV )
{
    return( valid() ? m_resourceDSVs.create( resourceDSV ) : false );
}

bool CSystemFactory::create( const CSamplerDesc& desc, ResourceSampler& resourceSampler )
{
    if( valid() )
    {
        PhysicalSampler physicalSampler;
        if( m_physicalSamplers.create( desc, physicalSampler ) )
        {
            if( m_resourceSamplers.create( physicalSampler, resourceSampler ) )
            {
                return( true );
            }
        }
    }
    return( false );
}

bool CSystemFactory::create( const CRasterStateDesc& desc, ResourceRasterState& resourceRasterState )
{
    if( valid() )
    {
        PhysicalRasterState physicalRasterState;
        if( m_physicalRasterStates.create( desc, physicalRasterState ) )
        {
            if( m_resourceRasterStates.create( physicalRasterState, resourceRasterState ) )
            {
                return( true );
            }
        }
    }
    return( false );
}

bool CSystemFactory::create( const CDepthStateDesc& desc, ResourceDepthState& resourceDepthState )
{
    if( valid() )
    {
        PhysicalDepthState physicalDepthState;
        if( m_physicalDepthStates.create( desc, physicalDepthState ) )
        {
            if( m_resourceDepthStates.create( physicalDepthState, resourceDepthState ) )
            {
                return( true );
            }
        }
    }
    return( false );
}

bool CSystemFactory::create( const CBlendStateDesc& desc, ResourceBlendState& resourceBlendState )
{
    if( valid() )
    {
        PhysicalBlendState physicalBlendState;
        if( m_physicalBlendStates.create( desc, physicalBlendState ) )
        {
            if( m_resourceBlendStates.create( physicalBlendState, resourceBlendState ) )
            {
                return( true );
            }
        }
    }
    return( false );
}

bool CSystemFactory::create( const CMultiBlendStateDesc& desc, ResourceBlendState& resourceBlendState )
{
    if( valid() )
    {
        PhysicalBlendState physicalBlendState;
        if( m_physicalBlendStates.create( desc, physicalBlendState ) )
        {
            if( m_resourceBlendStates.create( physicalBlendState, resourceBlendState ) )
            {
                return( true );
            }
        }
    }
    return( false );
}

bool CSystemFactory::create( const CBlendStateConfig& config, ResourceBlendState& resourceBlendState )
{
    if( valid() )
    {
        PhysicalBlendState physicalBlendState;
        if( m_physicalBlendStates.create( config, physicalBlendState ) )
        {
            if( m_resourceBlendStates.create( physicalBlendState, resourceBlendState ) )
            {
                return( true );
            }
        }
    }
    return( false );
}

bool CSystemFactory::create( ResourceParams& resourceParams )
{
    return( valid() ? m_resourceLinears.create( resourceParams ) : false );
}

bool CSystemFactory::create( ResourceIndices& resourceIndices )
{
    return( valid() ? m_resourceLinears.create( resourceIndices ) : false );
}

bool CSystemFactory::create( ResourceCommand& resourceCommand )
{
    return( valid() ? m_resourceCommands.create( resourceCommand ) : false );
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
