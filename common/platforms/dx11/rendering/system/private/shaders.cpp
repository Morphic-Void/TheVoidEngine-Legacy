
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   shaders.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Private shader classes.
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
#include "libs/compression/m2f.h"
#include "resources.h"
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
////    Binding class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CBinding::create( CShaderLibrary& library, const uint32_t handle )
{
    ASSERT( m_refs == 0 );
    if( m_refs == 0 )
    {
        m_refs = 1;
        m_handle = handle;
        m_library = &library;
        m_resource.release();
        return( true );
    }
    return( false );
}

void CBinding::destroy()
{
    ASSERT( m_refs == 0 );
    CShaderLibrary* library = m_library;
    m_refs = 0;
    m_handle = 0;
    m_library = NULL;
    m_resource.release();
    if( library != NULL ) library->destroy( *this );
}

void CBinding::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CBinding::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            destroy();
        }
    }
}

bool CBinding::clone( Binding& bindingClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_library->clone( *this, bindingClone ) );
    }
    bindingClone.release();
    return( false );
}

bool CBinding::bind( const Resource& resource )
{
    ASSERT( valid() );
    if( valid() )
    {
        m_resource.share( resource );
        return( resource.valid() );
    }
    return( false );
}

bool CBinding::unbind()
{
    ASSERT( valid() );
    if (valid())
    {
        m_resource.release();
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Binding group class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CBindingGroup::create( CShaderLibrary& library, const uint32_t count, Binding* const bindings )
{
    ASSERT( m_refs == 0 );
    if( m_refs == 0 )
    {
        m_refs = 1;
        m_count = count;
        m_library = &library;
        m_bindings = bindings;
        return( true );
    }
    return( false );
}

void CBindingGroup::destroy()
{
    ASSERT( m_refs == 0 );
    CShaderLibrary* library = m_library;
    m_refs = 0;
    m_count = 0;
    m_library = NULL;
    if( m_bindings != NULL )
    {
        delete[] m_bindings;
        m_bindings = NULL;
    }
    if( library != NULL ) library->destroy( *this );
}

void CBindingGroup::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CBindingGroup::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            destroy();
        }
    }
}

bool CBindingGroup::clone( BindingGroup& bindingGroupClone ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( m_library->clone( *this, bindingGroupClone ) );
    }
    bindingGroupClone.release();
    return( false );
}

bool CBindingGroup::bind( const Binding& binding )
{
    ASSERT( valid() );
    return( valid() ? m_library->bind( m_bindings, binding ) : false );
}

bool CBindingGroup::unbind( const Binding& binding )
{
    ASSERT( valid() );
    return( valid() ? m_library->unbind( m_bindings, binding ) : false );
}

bool CBindingGroup::unbind()
{
    ASSERT( valid() );
    if (valid())
    {
        m_library->unbind(m_bindings);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Shader class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CShader::create( CShaderLibrary& library, const uint32_t handle, Binding* const bindings, ShaderStaging* const staging )
{
    ASSERT( m_refs == 0 );
    if( m_refs == 0 )
    {
        m_refs = 1;
        m_handle = handle;
        m_library = &library;
        m_bindings = bindings;
        m_staging = staging;
        return( true );
    }
    return( false );
}

void CShader::destroy()
{
    ASSERT( m_refs == 0 );
    CShaderLibrary* library = m_library;
    m_refs = 0;
    m_handle = 0;
    m_library = NULL;
    if( m_bindings != NULL )
    {
        delete[] m_bindings;
        m_bindings = NULL;
    }
    if( m_staging != NULL )
    {
        delete[] m_staging;
        m_staging = NULL;
    }
    if( library != NULL ) library->destroy( *this );
}

void CShader::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CShader::release()
{
    ASSERT( valid() );
    if( valid() )
    {
        --m_refs;
        if( !m_refs )
        {
            destroy();
        }
    }
}

bool CShader::clone( Shader& shaderClone, const bool empty ) const
{
    ASSERT( valid() );
    if( valid() )
    {
        return( empty ? m_library->clone( *this, shaderClone ) : m_library->create( m_handle, shaderClone ) );
    }
    shaderClone.release();
    return( false );
}

bool CShader::bind( const Binding& binding )
{
    ASSERT( valid() );
    return( valid() ? m_library->bind( m_handle, m_bindings, binding ) : false );
}

bool CShader::unbind( const Binding& binding )
{
    ASSERT( valid() );
    return( valid() ? m_library->unbind( m_handle, m_bindings, binding ) : false );
}

bool CShader::update( const Binding& binding )
{
    ASSERT( valid() );
    return( valid() ? m_library->update( m_handle, m_bindings, *m_staging, binding ) : false );
}

bool CShader::bind( const BindingGroup& bindingGroup )
{
    ASSERT( valid() );
    return( valid() ? m_library->bind( m_handle, m_bindings, bindingGroup ) : false );
}

bool CShader::unbind( const BindingGroup& bindingGroup )
{
    ASSERT( valid() );
    return( valid() ? m_library->unbind( m_handle, m_bindings, bindingGroup ) : false );
}

bool CShader::unbind()
{
    ASSERT( valid() );
    return( valid() ? m_library->unbind( m_handle, m_bindings ) : false );
}

bool CShader::update()
{
    ASSERT( valid() );
    return( valid() ? m_library->update( m_handle, m_bindings, *m_staging ) : false );
}

bool CShader::submit() const
{
    ASSERT( valid() );
    return( valid() ? m_library->submit( m_handle, *m_staging ) : false );
}

bool CShader::execute( const ResourceCommand& command ) const
{
    ASSERT( valid() );
    return( valid() ? m_library->execute( command ) : false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Translation of stream element type to DXGI equivalent
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const DXGI_FORMAT CShaderLibrary::s_EELEMENT2DXGI[] =
{
    DXGI_FORMAT_UNKNOWN,                //  EElement_UNKNOWN        unknown size and type (used for stream-out gaps)
    DXGI_FORMAT_R8G8B8A8_SINT,          //  EElement_8x4_SINT       4 bytes (32-bits):   4 * 8-bit signed integers
    DXGI_FORMAT_R8G8B8A8_UINT,          //  EElement_8x4_UINT       4 bytes (32-bits):   4 * 8-bit unsigned integers
    DXGI_FORMAT_R8G8B8A8_SNORM,         //  EElement_8x4_SNORM      4 bytes (32-bits):   4 * 8-bit signed integers to be scaled into the range -1.0:+1.0
    DXGI_FORMAT_R8G8B8A8_UNORM,         //  EElement_8x4_UNORM      4 bytes (32-bits):   4 * 8-bit unsigned integers to be scaled into the range 0.0:+1.0
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,    //  EElement_8x4_SRGB       4 bytes (32-bits):   4 * 8-bit unsigned integers to be scaled into the range 0.0:+1.0 with SRGB decode to linear
    DXGI_FORMAT_R16G16_SINT,            //  EElement_16x2_SINT      4 bytes (32-bits):   2 * 16-bit signed integers
    DXGI_FORMAT_R16G16_UINT,            //  EElement_16x2_UINT      4 bytes (32-bits):   2 * 16-bit unsigned integers
    DXGI_FORMAT_R16G16_SNORM,           //  EElement_16x2_SNORM     4 bytes (32-bits):   2 * 16-bit signed integers to be scaled into the range -1.0:+1.0
    DXGI_FORMAT_R16G16_UNORM,           //  EElement_16x2_UNORM     4 bytes (32-bits):   2 * 16-bit unsigned integers to be scaled into the range 0.0:+1.0
    DXGI_FORMAT_R16G16_FLOAT,           //  EElement_16x2_FLOAT     4 bytes (32-bits):   2 * 16-bit half-precision floating point values
    DXGI_FORMAT_R16G16B16A16_SINT,      //  EElement_16x4_SINT      8 bytes (64-bits):   4 * 16-bit signed integers
    DXGI_FORMAT_R16G16B16A16_UINT,      //  EElement_16x4_UINT      8 bytes (64-bits):   4 * 16-bit unsigned integers
    DXGI_FORMAT_R16G16B16A16_SNORM,     //  EElement_16x4_SNORM     8 bytes (64-bits):   4 * 16-bit signed integers to be scaled into the range -1.0:+1.0
    DXGI_FORMAT_R16G16B16A16_UNORM,     //  EElement_16x4_UNORM     8 bytes (64-bits):   4 * 16-bit unsigned integers to be scaled into the range 0.0:+1.0
    DXGI_FORMAT_R16G16B16A16_FLOAT,     //  EElement_16x4_FLOAT     8 bytes (64-bits):   4 * 16-bit half-precision floating point values
    DXGI_FORMAT_R32_TYPELESS,           //  EElement_32x1_TYPELESS  4 bytes (32-bits):   1 * 32-bit typeless value
    DXGI_FORMAT_R32_SINT,               //  EElement_32x1_SINT      4 bytes (32-bits):   1 * 32-bit signed integer
    DXGI_FORMAT_R32_UINT,               //  EElement_32x1_UINT      4 bytes (32-bits):   1 * 32-bit unsigned integer
    DXGI_FORMAT_R32_FLOAT,              //  EElement_32x1_FLOAT     4 bytes (32-bits):   1 * 32-bit floating point value
    DXGI_FORMAT_R32G32_TYPELESS,        //  EElement_32x2_TYPELESS  8 bytes (64-bits):   2 * 32-bit typeless values
    DXGI_FORMAT_R32G32_SINT,            //  EElement_32x2_SINT      8 bytes (64-bits):   2 * 32-bit signed integers
    DXGI_FORMAT_R32G32_UINT,            //  EElement_32x2_UINT      8 bytes (64-bits):   2 * 32-bit unsigned integers
    DXGI_FORMAT_R32G32_FLOAT,           //  EElement_32x2_FLOAT     8 bytes (64-bits):   2 * 32-bit floating point values
    DXGI_FORMAT_R32G32B32_TYPELESS,     //  EElement_32x3_TYPELESS  12 bytes (96-bits):  3 * 32-bit typeless values
    DXGI_FORMAT_R32G32B32_SINT,         //  EElement_32x3_SINT      12 bytes (96-bits):  3 * 32-bit signed integers
    DXGI_FORMAT_R32G32B32_UINT,         //  EElement_32x3_UINT      12 bytes (96-bits):  3 * 32-bit unsigned integers
    DXGI_FORMAT_R32G32B32_FLOAT,        //  EElement_32x3_FLOAT     12 bytes (96-bits):  3 * 32-bit floating point values
    DXGI_FORMAT_R32G32B32A32_TYPELESS,  //  EElement_32x4_TYPELESS  16 bytes (128-bits): 4 * 32-bit typeless values
    DXGI_FORMAT_R32G32B32A32_SINT,      //  EElement_32x4_SINT      16 bytes (128-bits): 4 * 32-bit signed integers
    DXGI_FORMAT_R32G32B32A32_UINT,      //  EElement_32x4_UINT      16 bytes (128-bits): 4 * 32-bit unsigned integers
    DXGI_FORMAT_R32G32B32A32_FLOAT      //  EElement_32x4_FLOAT     16 bytes (128-bits): 4 * 32-bit floating point values
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CShaderLibrary class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CShaderLibrary::create( CSystemFactory& factory, RefBlob& blob, ShaderLibrary& shaderLibrary )
{
    ASSERT( m_refs == 0 );
    if( m_refs == 0 )
    {
        void* buffer = blob.getBuffer();
        uint size = blob.getSize();
        ASSERT( ( buffer != NULL ) && ( size > sizeof( binary::ShaderLibrary ) ) );
        if( ( buffer != NULL ) && ( size > sizeof( binary::ShaderLibrary ) ) )
        {
            binary::ShaderLibrary& binaryLibrary = *reinterpret_cast< binary::ShaderLibrary* >( buffer );
            if( ( binaryLibrary.header.magic == s_magic ) && ( binaryLibrary.header.platform == s_platform ) && ( binaryLibrary.header.version <= 0x0001ffff ) && ( binaryLibrary.header.fileSize <= size ) )
            {   //  the data is a shader library file in a compatible format and appears to be complete
                blob.addRef();
                factory.addRef();
                m_blob = &blob;
                m_factory = &factory;
                m_device = factory.getDevice();
                m_context = factory.getContext();
                m_data.binary.library = &binaryLibrary;
                m_data.stringTable = reinterpret_cast< FrozenStringTable* >( m_data.binary.data + binaryLibrary.index.stringTable.offset );
                m_data.shaderBinaries = reinterpret_cast< uint8_t* >( m_data.binary.data + binaryLibrary.index.shaderBinaries.offset );
                m_data.shaderBlobs = reinterpret_cast< binary::BlobDesc* >( m_data.binary.data + binaryLibrary.index.shaderBlobs.offset );
                m_data.resourceItems = reinterpret_cast< binary::ResourceItem* >( m_data.binary.data + binaryLibrary.index.resourceItems.offset );
                m_data.resourceIndices = reinterpret_cast< uint32_t* >( m_data.binary.data + binaryLibrary.index.resourceIndices.offset );
                m_data.streamFormats = reinterpret_cast< binary::StreamFormat* >( m_data.binary.data + binaryLibrary.index.streamFormats.offset );
                m_data.streamElements = reinterpret_cast< binary::StreamElement* >( m_data.binary.data + binaryLibrary.index.streamElements.offset );
                m_data.assemblyLayouts = reinterpret_cast< binary::AssemblyLayout* >( m_data.binary.data + binaryLibrary.index.assemblyLayouts.offset );
                m_data.assemblyStreams = reinterpret_cast< binary::AssemblyStream* >( m_data.binary.data + binaryLibrary.index.assemblyStreams.offset );
                m_data.assemblyElements = reinterpret_cast< binary::AssemblyElement* >( m_data.binary.data + binaryLibrary.index.assemblyElements.offset );
                m_data.stagingScatters = reinterpret_cast< binary::StagingScatter* >( m_data.binary.data + binaryLibrary.index.stagingScatters.offset );
                m_data.stagingGathers = reinterpret_cast< binary::StagingGather* >( m_data.binary.data + binaryLibrary.index.stagingGathers.offset );
                m_data.stagingIndices = reinterpret_cast< uint32_t* >( m_data.binary.data + binaryLibrary.index.stagingIndices.offset );
                m_data.stageChains = reinterpret_cast< binary::StageChain* >( m_data.binary.data + binaryLibrary.index.stageChains.offset );
                m_data.shaderDescs = reinterpret_cast< binary::ShaderDesc* >( m_data.binary.data + binaryLibrary.index.shaderDescs.offset );
                m_data.stringTableSize = binaryLibrary.index.stringTable.size;
                m_data.shaderBinariesSize = binaryLibrary.index.shaderBinaries.size;
                m_data.shaderBlobCount = binaryLibrary.index.shaderBlobs.count;
                m_data.resourceItemCount = binaryLibrary.index.resourceItems.count;
                m_data.resourceIndexCount = binaryLibrary.index.resourceIndices.count;
                m_data.streamFormatCount = binaryLibrary.index.streamFormats.count;
                m_data.streamElementCount = binaryLibrary.index.streamElements.count;
                m_data.assemblyLayoutCount = binaryLibrary.index.assemblyLayouts.count;
                m_data.assemblyStreamCount = binaryLibrary.index.assemblyStreams.count;
                m_data.assemblyElementCount = binaryLibrary.index.assemblyElements.count;
                m_data.stagingScatterCount = binaryLibrary.index.stagingScatters.count;
                m_data.stagingGatherCount = binaryLibrary.index.stagingGathers.count;
                m_data.stagingIndexCount = binaryLibrary.index.stagingIndices.count;
                m_data.stageChainCount = binaryLibrary.index.stageChains.count;
                m_data.shaderDescCount = binaryLibrary.index.shaderDescs.count;
                m_data.interfaceCount = binaryLibrary.setup.interfaceCount;
                m_interfaces = new IUnknown*[ m_data.interfaceCount ];
                memset( m_interfaces, 0, ( m_data.interfaceCount * sizeof( IUnknown* ) ) );
                unmangle();
                if( createInterfaces() )
                {
                    m_refs = 1;
                    shaderLibrary.release();
                    shaderLibrary.naked() = static_cast< IShared* >( this );
                    m_bindingGroups.init( 32 );
                    m_bindings.init( static_cast< uint >( m_data.resourceIndexCount ) );
                    m_shaders.init( static_cast< uint >( m_data.shaderDescCount ) );
                    return( true );
                }
                mangle();
            }
        }
        destroy();
    }
    return( false );
}

void CShaderLibrary::destroy()
{
    ASSERT( m_refs == 0 );
    m_shaders.kill();
    m_bindings.kill();
    m_bindingGroups.kill();
    if( m_interfaces != NULL )
    {
        uint32_t interfaceCount = m_data.interfaceCount;
        for( uint32_t interfaceIndex = 0; interfaceIndex < interfaceCount; ++interfaceIndex )
        {
            IUnknown*& iunknown = m_interfaces[ interfaceIndex ];
            if( iunknown != NULL )
            {
                iunknown->Release();
                iunknown = NULL;
            }
        }
        delete[] m_interfaces;
        m_interfaces = NULL;
    }
	m_device = NULL;
	m_context = NULL;
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
    memset( reinterpret_cast< void* >( &m_data ), 0, sizeof( m_data ) );
    m_reserved = 0;
    m_refs = 0;
}

void CShaderLibrary::zero()
{
    m_refs = 0;
    m_reserved = 0;
    m_blob = NULL;
	m_factory = NULL;
	m_device = NULL;
	m_context = NULL;
    m_interfaces = NULL;
    memset( reinterpret_cast< void* >( &m_data ), 0, sizeof( m_data ) );
}

void CShaderLibrary::addRef()
{
    ASSERT( valid() );
    if( valid() )
    {
        ++m_refs;
    }
}

void CShaderLibrary::release()
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

bool CShaderLibrary::create( const StringItem& name, Shader& shader )
{
    if( valid() )
    {   //  the library is valid
        int shaderIndex = findShader( name );
        if( shaderIndex >= 0 )
        {
            uint32_t handle = ~static_cast< uint32_t >( shaderIndex );
            return( create( handle, shader ) );
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::create( const StringItem& name, BindingStream& bindingStream )
{
    return( create( name, bindingStream, EResourceTypeStream ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingOutput& bindingOutput )
{
    return( create( name, bindingOutput, EResourceTypeOutput ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingConsts& bindingConsts )
{
    return( create( name, bindingConsts, EResourceTypeConsts ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingSRV& bindingSRV )
{
    return( create( name, bindingSRV, EResourceTypeSRV ) );
}
 
bool CShaderLibrary::create( const StringItem& name, BindingUAV& bindingUAV )
{
    return( create( name, bindingUAV, EResourceTypeUAV ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingRTV& bindingRTV )
{
    return( create( name, bindingRTV, EResourceTypeRTV ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingDSV& bindingDSV )
{
    return( create( name, bindingDSV, EResourceTypeDSV ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingSampler& bindingSampler )
{
    return( create( name, bindingSampler, EResourceTypeSampler ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingRasterState& bindingRasterState )
{
    return( create( name, bindingRasterState, EResourceTypeRasterState ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingDepthState& bindingDepthState )
{
    return( create( name, bindingDepthState, EResourceTypeDepthState ) );
}

bool CShaderLibrary::create( const StringItem& name, BindingBlendState& bindingBlendState )
{
    return( create( name, bindingBlendState, EResourceTypeBlendState ) );
}

bool CShaderLibrary::create( BindingGroup& bindingGroup )
{
    bindingGroup.release();
    if( valid() )
    {   //  the library is valid
        uint32_t bindingCount = m_data.resourceItemCount;
        Binding* bindings = new Binding[ bindingCount ];
        CBindingGroup& newBindingGroup = m_bindingGroups.fetch();
        if( newBindingGroup.create( *this, m_data.resourceItemCount, bindings ) )
        {
            bindingGroup.naked() = static_cast< IShared* >( &newBindingGroup );
            addRef();
            return( true );
        }
        delete[] bindings;
        m_bindingGroups.recycle( newBindingGroup );
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::clone( const CBindingGroup& bindingGroup, BindingGroup& bindingGroupClone )
{
    bindingGroupClone.release();
    if( valid() )
    {   //  the library is valid
        uint32_t bindingCount = m_data.resourceItemCount;
        const Binding* bindings = bindingGroup.getBindings();
        Binding* clonedBindings = new Binding[ bindingCount ];
        for( uint32_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex )
        {
            clonedBindings[ bindingIndex ].share( bindings[ bindingIndex ] );
        }
        CBindingGroup& newBindingGroup = m_bindingGroups.fetch();
        if( newBindingGroup.create( *this, m_data.resourceItemCount, clonedBindings ) )
        {
            bindingGroupClone.naked() = static_cast< IShared* >( &newBindingGroup );
            addRef();
            return( true );
        }
        delete[] clonedBindings;
        m_bindingGroups.recycle( newBindingGroup );
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

void CShaderLibrary::destroy( CBindingGroup& bindingGroup )
{
    if( valid() )
    {   //  the library is valid
        m_bindingGroups.recycle( bindingGroup );
        release();
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
}

bool CShaderLibrary::create( const StringItem& name, Binding& binding, const uint32_t resourceType )
{
    binding.release();
    if( valid() )
    {   //  the library is valid
        int resourceItemIndex = findResource( name, resourceType );
        if( resourceItemIndex >= 0 )
        {
            uint32_t handle = ~static_cast< uint32_t >( resourceItemIndex );
            CBinding& newBinding = m_bindings.fetch();
            if( newBinding.create( *this, handle ) )
            {
                binding.naked() = static_cast< IShared* >( &newBinding );
                addRef();
                return( true );
            }
            m_bindings.recycle( newBinding );
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::clone( const CBinding& binding, Binding& bindingClone )
{
    bindingClone.release();
    if( valid() )
    {   //  the library is valid
        uint32_t handle = binding.getHandle();
        CBinding& newBinding = m_bindings.fetch();
        if( newBinding.create( *this, handle ) )
        {
            bindingClone.naked() = static_cast< IShared* >( &newBinding );
            addRef();
            return( true );
        }
        m_bindings.recycle( newBinding );
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

void CShaderLibrary::destroy( CBinding& binding )
{
    if( valid() )
    {   //  the library is valid
        m_bindings.recycle( binding );
        release();
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
}

bool CShaderLibrary::create( const uint32_t shaderHandle, Shader& shader )
{
    shader.release();
    if( valid() )
    {   //  the library is valid
        uint32_t shaderIndex = ~shaderHandle;
        if( shaderIndex < m_data.shaderDescCount )
        {
            const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
            const binary::StageChain& stageChain = m_data.stageChains[ shaderDesc.chainIndex ];
            const uint32_t bindingCount = shaderDesc.resources.count;
            const bool renderable = ( ( stageChain.stageMask & binary::EShaderStagePixelBit ) ? true : false );
            Binding* bindings = new Binding[ bindingCount ];
            ShaderStaging* staging = createStaging( shaderDesc.stagingCount, renderable );
            CShader& newShader = m_shaders.fetch();
            if( newShader.create( *this, shaderHandle, bindings, staging ) )
            {
                shader.naked() = static_cast< IShared* >( &newShader );
                addRef();
                return( true );
            }
            delete[] staging;
            delete[] bindings;
            m_shaders.recycle( newShader );
        }
        else
        {
            ASSERT_FAIL( "Invalid shader handle in call to CShaderLibrary::create()" );
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::clone( const CShader& shader, Shader& shaderClone )
{
    shaderClone.release();
    if( valid() )
    {   //  the library is valid
        uint32_t shaderHandle = shader.getHandle();
        uint32_t shaderIndex = ~shaderHandle;
        if( shaderIndex < m_data.shaderDescCount )
        {
            const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
            const binary::StageChain& stageChain = m_data.stageChains[ shaderDesc.chainIndex ];
            const uint32_t bindingCount = shaderDesc.resources.count;
            const bool renderable = ( ( stageChain.stageMask & binary::EShaderStagePixelBit ) ? true : false );
            const Binding* bindings = shader.getBindings();
            Binding* clonedBindings = new Binding[ bindingCount ];
            const ShaderStaging* staging = shader.getStaging();
            ShaderStaging* clonedStaging = createStaging( shaderDesc.stagingCount, renderable );
            for( uint32_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex )
            {
                clonedBindings[ bindingIndex ].share( bindings[ bindingIndex ] );
            }
            memcpy( clonedStaging->buffers, staging->buffers, ( shaderDesc.stagingCount * sizeof( IUnknown* ) ) );
            memcpy( clonedStaging->offsets, staging->offsets, ( shaderDesc.stagingCount * sizeof( UINT ) ) );
            memcpy( clonedStaging->strides, staging->strides, ( shaderDesc.stagingCount * sizeof( UINT ) ) );
            if( renderable )
            {
                memcpy( clonedStaging->renderable, staging->renderable, sizeof( RenderableStaging ) );
            }
            CShader& newShader = m_shaders.fetch();
            if( newShader.create( *this, shaderHandle, clonedBindings, clonedStaging ) )
            {
                shaderClone.naked() = static_cast< IShared* >( &newShader );
                addRef();
                return( true );
            }
            delete[] clonedStaging;
            delete[] clonedBindings;
            m_shaders.recycle( newShader );
        }
        else
        {
            ASSERT_FAIL( "Invalid shader handle in call to CShaderLibrary::clone()" );
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

void CShaderLibrary::destroy( CShader& shader )
{
    if( valid() )
    {   //  the library is valid
        m_shaders.recycle( shader );
        release();
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
}

bool CShaderLibrary::bind( Binding* const bindings, const Binding& binding )
{   //  bind a binding group binding
    if( valid() )
    {   //  the library is valid
        const uint32_t handle = getHandle( binding );
        if( handle )
        {
            const uint32_t resourceItemIndex = ~handle;
            Binding& bound = bindings[ resourceItemIndex ];
            bound.share( binding );
            return( true );
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::unbind( Binding* const bindings, const Binding& binding )
{   //  unbind a binding group binding
    if( valid() )
    {   //  the library is valid
        const uint32_t handle = getHandle( binding );
        if( handle )
        {
            const uint32_t resourceItemIndex = ~handle;
            Binding& bound = bindings[ resourceItemIndex ];
            if( bound == binding )
            {
                bound.release();
                return( true );
            }
            else
            {
                ASSERT_FAIL( "Attempting to unbind incorrect binding" );
            }
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::unbind( Binding* const bindings )
{   //  unbind BindingGroup bindings
    if( valid() )
    {   //  the library is valid
        const uint32_t bindingCount = m_data.resourceItemCount;
        for( uint32_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex )
        {
            bindings[ bindingIndex ].release();
        }
        return true;
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return false;
}

bool CShaderLibrary::bind( const uint32_t shaderHandle, Binding* const bindings, const Binding& binding )
{   //  bind a Shader binding
    if( valid() )
    {   //  the library is valid
        const uint32_t handle = getHandle( binding );
        if( handle )
        {
            const uint32_t resourceItemIndex = ~handle;
            const int bindingIndex = findInShader( shaderHandle, resourceItemIndex );
            if( bindingIndex >= 0 )
            {
                Binding& bound = bindings[ bindingIndex ];
                bound.share( binding );
                return( true );
            }
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::unbind( const uint32_t shaderHandle, Binding* const bindings, const Binding& binding )
{   //  unbind a Shader binding
    if( valid() )
    {   //  the library is valid
        const uint32_t handle = getHandle( binding );
        if( handle )
        {
            const uint32_t resourceItemIndex = ~handle;
            const int bindingIndex = findInShader( shaderHandle, resourceItemIndex );
            if( bindingIndex >= 0 )
            {
                Binding& bound = bindings[ bindingIndex ];
                if( bound == binding )
                {
                    bound.release();
                    return( true );
                }
                else
                {
                    ASSERT_FAIL( "Attempting to unbind incorrect binding" );
                }
            }
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::bind( const uint32_t shaderHandle, Binding* const bindings, const BindingGroup& bindingGroup )
{   //  bind matching Shader bindings
    if( valid() )
    {   //  the library is valid
        const Binding* groupBindings = getBindings( bindingGroup );
        if( groupBindings )
        {
            uint32_t shaderIndex = ~shaderHandle;
            if( shaderIndex < m_data.shaderDescCount )
            {
                const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
                const uint32_t* const resourceMap = ( m_data.resourceIndices + shaderDesc.resources.index );
                const uint32_t bindingCount = shaderDesc.resources.count;
                for( uint32_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex )
                {
                    const Binding& groupBinding = groupBindings[ resourceMap[ bindingIndex ] ];
                    if( groupBinding.valid() )
                    {
                        Binding& bound = bindings[ bindingIndex ];
                        bound.share( groupBinding );
                    }
                }
                return true;
            }
            else
            {
                ASSERT_FAIL( "Invalid shader handle in call to CShaderLibrary::unbind()" );
            }
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return false;
}

bool CShaderLibrary::unbind( const uint32_t shaderHandle, Binding* const bindings, const BindingGroup& bindingGroup )
{   //  unbind matching Shader bindings
    if( valid() )
    {   //  the library is valid
        const Binding* groupBindings = getBindings( bindingGroup );
        if( groupBindings )
        {
            uint32_t shaderIndex = ~shaderHandle;
            if( shaderIndex < m_data.shaderDescCount )
            {
                const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
                const uint32_t* const resourceMap = ( m_data.resourceIndices + shaderDesc.resources.index );
                const uint32_t bindingCount = shaderDesc.resources.count;
                for( uint32_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex )
                {
                    const Binding& groupBinding = groupBindings[ resourceMap[ bindingIndex ] ];
                    Binding& bound = bindings[ bindingIndex ];
                    if( bound == groupBinding )
                    {
                        bound.release();
                    }
                }
                return true;
            }
            else
            {
                ASSERT_FAIL( "Invalid shader handle in call to CShaderLibrary::unbind()" );
            }
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return false;
}

bool CShaderLibrary::unbind( const uint32_t shaderHandle, Binding* const bindings )
{   //  unbind all Shader bindings
    if( valid() )
    {   //  the library is valid
        uint32_t shaderIndex = ~shaderHandle;
        if( shaderIndex < m_data.shaderDescCount )
        {
            const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
            const uint32_t bindingCount = shaderDesc.resources.count;
            for( uint32_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex )
            {
                bindings[ bindingIndex ].release();
            }
            return true;
        }
        else
        {
            ASSERT_FAIL( "Invalid shader handle in call to CShaderLibrary::unbind()" );
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return false;
}

bool CShaderLibrary::update( const uint32_t shaderHandle, const Binding* const bindings, ShaderStaging& staging, const Binding& binding ) const
{
    if( valid() )
    {   //  the library is valid
        const uint32_t handle = getHandle( binding );
        if( handle )
        {
            const uint32_t resourceItemIndex = ~handle;
            const int bindingIndex = findInShader( shaderHandle, resourceItemIndex );
            if( bindingIndex >= 0 )
            {
                const Binding& bound = bindings[ bindingIndex ];
                if( bound == binding )
                {
                    const void* const desc = getDesc( binding );
                    if( desc )
                    {
                        const uint32_t shaderIndex = ~shaderHandle;
                        const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
                        const binary::StageChain& stageChain = m_data.stageChains[ shaderDesc.chainIndex ];
                        const binary::ResourceItem& resourceItem = m_data.resourceItems[ resourceItemIndex ];
                        switch( resourceItem.resourceType )
                        {
                            case( EResourceTypeConsts ):
                            {
                                const ResourceDescLinear& descLinear = *reinterpret_cast< const ResourceDescLinear* >( desc );
                                void* const buffer = descLinear.unknown;
                                const UINT offset = descLinear.offset;
                                const UINT stride = descLinear.stride;
                                const binary::Scattering& scattering = stageChain.bufferScattering;
                                const uint32_t scatterIndex = ( static_cast< uint32_t >( bindingIndex ) - scattering.source );
                                const binary::StagingScatter& stagingScatter = m_data.stagingScatters[ scattering.target.index + scatterIndex ];
                                const uint32_t* const stagingIndices = ( m_data.stagingIndices + stagingScatter.locations );
                                const uint32_t instanceCount = stagingScatter.instances;
                                for( uint32_t instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex )
                                {
                                    const uint32_t stagingIndex = stagingIndices[ instanceIndex ];
                                    staging.buffers[ stagingIndex ] = buffer;
                                    staging.offsets[ stagingIndex ] = offset;
                                    staging.strides[ stagingIndex ] = stride;
                                }
                                break;
                            }
                            case( EResourceTypeSRV ):
                            {
                                const ResourceDescSRV& descSRV = *reinterpret_cast< const ResourceDescSRV* >( desc );
                                void* const srv = descSRV.unknown;
                                const binary::Scattering& scattering = stageChain.srvScattering;
                                const uint32_t scatterIndex = ( static_cast< uint32_t >( bindingIndex ) - scattering.source );
                                const binary::StagingScatter& stagingScatter = m_data.stagingScatters[ scattering.target.index + scatterIndex ];
                                const uint32_t* const stagingIndices = ( m_data.stagingIndices + stagingScatter.locations );
                                const uint32_t instanceCount = stagingScatter.instances;
                                for( uint32_t instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex )
                                {
                                    const uint32_t stagingIndex = stagingIndices[ instanceIndex ];
                                    staging.buffers[ stagingIndex ] = srv;
                                }
                                break;
                            }
                            case( EResourceTypeUAV ):
                            {
                                const ResourceDescUAV& descUAV = *reinterpret_cast< const ResourceDescUAV* >( desc );
                                void* const uav = descUAV.unknown;
                                const UINT initialCount = descUAV.initialCount;
                                const binary::Scattering& scattering = stageChain.uavScattering;
                                const uint32_t scatterIndex = ( static_cast< uint32_t >( bindingIndex ) - scattering.source );
                                const binary::StagingScatter& stagingScatter = m_data.stagingScatters[ scattering.target.index + scatterIndex ];
                                const uint32_t* const stagingIndices = ( m_data.stagingIndices + stagingScatter.locations );
                                const uint32_t instanceCount = stagingScatter.instances;
                                for( uint32_t instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex )
                                {
                                    const uint32_t stagingIndex = stagingIndices[ instanceIndex ];
                                    staging.buffers[ stagingIndex ] = uav;
                                    staging.offsets[ stagingIndex ] = initialCount;
                                }
                                break;
                            }
                            case( EResourceTypeSampler ):
                            {
                                const ResourceDescSampler& descSampler = *reinterpret_cast< const ResourceDescSampler* >( desc );
                                void* const sampler = descSampler.unknown;
                                const binary::Scattering& scattering = stageChain.samplerScattering;
                                const uint32_t scatterIndex = ( static_cast< uint32_t >( bindingIndex ) - scattering.source );
                                const binary::StagingScatter& stagingScatter = m_data.stagingScatters[ scattering.target.index + scatterIndex ];
                                const uint32_t* const stagingIndices = ( m_data.stagingIndices + stagingScatter.locations );
                                const uint32_t instanceCount = stagingScatter.instances;
                                for( uint32_t instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex )
                                {
                                    const uint32_t stagingIndex = stagingIndices[ instanceIndex ];
                                    staging.buffers[ stagingIndex ] = sampler;
                                }
                                break;
                            }
                            case( EResourceTypeStream ):
                            {
                                const ResourceDescLinear& descLinear = *reinterpret_cast< const ResourceDescLinear* >( desc );
                                const binary::Remapping& remapping = shaderDesc.streams;
                                const uint32_t remapIndex = ( static_cast< uint32_t >( bindingIndex ) - remapping.source );
                                const uint32_t stagingIndex = ( m_data.stagingIndices[ remapping.target.indices.index + remapIndex ] + remapping.target.offset );
                                staging.buffers[ stagingIndex ] = descLinear.unknown;
                                staging.offsets[ stagingIndex ] = descLinear.offset;
                                staging.strides[ stagingIndex ] = descLinear.stride;
                                break;
                            }
                            case( EResourceTypeOutput ):
                            {
                                const ResourceDescLinear& descLinear = *reinterpret_cast< const ResourceDescLinear* >( desc );
                                const binary::Remapping& remapping = shaderDesc.outputs;
                                const uint32_t remapIndex = ( static_cast< uint32_t >( bindingIndex ) - remapping.source );
                                const uint32_t stagingIndex = ( m_data.stagingIndices[ remapping.target.indices.index + remapIndex ] + remapping.target.offset );
                                staging.buffers[ stagingIndex ] = descLinear.unknown;
                                staging.offsets[ stagingIndex ] = descLinear.offset;
                                staging.strides[ stagingIndex ] = descLinear.stride;
                                break;
                            }
                            case( EResourceTypeRTV ):
                            {
                                ASSERT( staging.renderable );
                                if( staging.renderable )
                                {   //  renderable bindings
                                    RenderableStaging& renderableStaging = *staging.renderable;
                                    const binary::ListDesc& remapping = shaderDesc.rtvRemap;
                                    const uint32_t remapIndex = ( static_cast< uint32_t >( bindingIndex ) - shaderDesc.rtvIndex );
                                    const uint32_t stagingIndex = m_data.stagingIndices[ remapping.index + remapIndex ];
                                    const ResourceDescRTV& descRTV = *reinterpret_cast< const ResourceDescRTV* >( desc );
                                    const FBOX& viewport = descRTV.viewport;
                                    const SRECT& scissoring = descRTV.scissoring;
                                    VIEWPORT& d3dViewport = renderableStaging.renderTargets.viewports[ stagingIndex ];
                                    RECT& d3dScissoring = renderableStaging.renderTargets.scissoring[ stagingIndex ];
                                    renderableStaging.renderTargets.buffers[ stagingIndex ] = descRTV.unknown;
                                    d3dViewport.TopLeftX = viewport.min.x;
                                    d3dViewport.TopLeftY = viewport.min.y;
                                    d3dViewport.Width = ( viewport.max.x - viewport.min.x );
                                    d3dViewport.Height = ( viewport.max.y - viewport.min.y );
                                    d3dViewport.MinDepth = viewport.min.z;
                                    d3dViewport.MaxDepth = viewport.max.z;
                                    d3dScissoring.left = scissoring.left;
                                    d3dScissoring.top = scissoring.top;
                                    d3dScissoring.right = scissoring.right;
                                    d3dScissoring.bottom = scissoring.bottom;
                                }
                                break;
                            }
                            case( EResourceTypeDSV ):
                            {
                                ASSERT( staging.renderable );
                                if( staging.renderable )
                                {   //  renderable bindings
                                    RenderableStaging& renderableStaging = *staging.renderable;
                                    const ResourceDescDSV& descDSV = *reinterpret_cast< const ResourceDescDSV* >( desc );
                                    const FBOX& viewport = descDSV.viewport;
                                    const SRECT& scissoring = descDSV.scissoring;
                                    VIEWPORT& d3dViewport = renderableStaging.depthStencil.viewport;
                                    RECT& d3dScissoring = renderableStaging.depthStencil.scissoring;
                                    renderableStaging.depthStencil.buffer = descDSV.unknown;
                                    d3dViewport.TopLeftX = viewport.min.x;
                                    d3dViewport.TopLeftY = viewport.min.y;
                                    d3dViewport.Width = ( viewport.max.x - viewport.min.x );
                                    d3dViewport.Height = ( viewport.max.y - viewport.min.y );
                                    d3dViewport.MinDepth = viewport.min.z;
                                    d3dViewport.MaxDepth = viewport.max.z;
                                    d3dScissoring.left = scissoring.left;
                                    d3dScissoring.top = scissoring.top;
                                    d3dScissoring.right = scissoring.right;
                                    d3dScissoring.bottom = scissoring.bottom;
                                }
                                break;
                            }
                            case( EResourceTypeRasterState ):
                            {
                                ASSERT( staging.renderable );
                                if( staging.renderable )
                                {   //  renderable bindings
                                    RenderableStaging& renderableStaging = *staging.renderable;
                                    const ResourceDescRasterState& descRaster = *reinterpret_cast< const ResourceDescRasterState* >( desc );
                                    renderableStaging.stateStaging.rasterState = descRaster.unknown;
                                }
                                break;
                            }
                            case( EResourceTypeDepthState ):
                            {
                                ASSERT( staging.renderable );
                                if( staging.renderable )
                                {   //  renderable bindings
                                    RenderableStaging& renderableStaging = *staging.renderable;
                                    const ResourceDescDepthState& descDepth = *reinterpret_cast< const ResourceDescDepthState* >( desc );
                                    renderableStaging.stateStaging.depthState = descDepth.unknown;
                                    renderableStaging.stateStaging.stencilRef = descDepth.stencilRef;
                                }
                                break;
                            }
                            case( EResourceTypeBlendState ):
                            {
                                ASSERT( staging.renderable );
                                if( staging.renderable )
                                {   //  renderable bindings
                                    RenderableStaging& renderableStaging = *staging.renderable;
                                    const ResourceDescBlendState& descBlend = *reinterpret_cast< const ResourceDescBlendState* >( desc );
                                    renderableStaging.stateStaging.blendState = descBlend.unknown;
                                    renderableStaging.stateStaging.sampleMask = descBlend.sampleMask;
                                    renderableStaging.stateStaging.blendFactor[ 0 ] = descBlend.blendFactor.x;
                                    renderableStaging.stateStaging.blendFactor[ 1 ] = descBlend.blendFactor.y;
                                    renderableStaging.stateStaging.blendFactor[ 2 ] = descBlend.blendFactor.z;
                                    renderableStaging.stateStaging.blendFactor[ 3 ] = descBlend.blendFactor.w;
                                }
                                break;
                            }
                            default:
                            {   //  miscellaneous non-staging resources
                                break;
                            }
                        }
                        return( true );
                    }
                }
                else
                {
                    ASSERT_FAIL( "Attempting to update incorrect binding" );
                }
            }
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::update( const uint32_t shaderHandle, const Binding* const bindings, ShaderStaging& staging ) const
{   //  update the staging of all shader resources
    bool ret = false;
    if( valid() )
    {   //  the library is valid
        uint32_t shaderIndex = ~shaderHandle;
        if( shaderIndex < m_data.shaderDescCount )
        {
            ret = true;
            const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
            const binary::StageChain& stageChain = m_data.stageChains[ shaderDesc.chainIndex ];
            {   //  transfer buffer data to the staging buffer
                const binary::Scattering& scattering = stageChain.bufferScattering;
                const binary::StagingScatter* const stagingScatters = ( m_data.stagingScatters + scattering.target.index );
                const Binding* const scatterBindings = ( bindings + scattering.source );
                const uint32_t scatterCount = scattering.target.count;
                for( uint32_t scatterIndex = 0; scatterIndex < scatterCount; ++scatterIndex )
                {
                    const void* const desc = getDesc( scatterBindings[ scatterIndex ] );
                    if( desc )
                    {
                        const ResourceDescLinear& descLinear = *reinterpret_cast< const ResourceDescLinear* >( desc );
                        void* const buffer = descLinear.unknown;
                        const UINT offset = descLinear.offset;
                        const UINT stride = descLinear.stride;
                        const binary::StagingScatter& stagingScatter = stagingScatters[ scatterIndex ];
                        const uint32_t* const stagingIndices = ( m_data.stagingIndices + stagingScatter.locations );
                        const uint32_t instanceCount = stagingScatter.instances;
                        for( uint32_t instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex )
                        {
                            const uint32_t stagingIndex = stagingIndices[ instanceIndex ];
                            staging.buffers[ stagingIndex ] = buffer;
                            staging.offsets[ stagingIndex ] = offset;
                            staging.strides[ stagingIndex ] = stride;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            {   //  transfer srv data to the staging buffer
                const binary::Scattering& scattering = stageChain.srvScattering;
                const binary::StagingScatter* const stagingScatters = ( m_data.stagingScatters + scattering.target.index );
                const Binding* const scatterBindings = ( bindings + scattering.source );
                const uint32_t scatterCount = scattering.target.count;
                for( uint32_t scatterIndex = 0; scatterIndex < scatterCount; ++scatterIndex )
                {
                    const void* const desc = getDesc( scatterBindings[ scatterIndex ] );
                    if( desc )
                    {
                        const ResourceDescSRV& descSRV = *reinterpret_cast< const ResourceDescSRV* >( desc );
                        void* const srv = descSRV.unknown;
                        const binary::StagingScatter& stagingScatter = stagingScatters[ scatterIndex ];
                        const uint32_t* const stagingIndices = ( m_data.stagingIndices + stagingScatter.locations );
                        const uint32_t instanceCount = stagingScatter.instances;
                        for( uint32_t instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex )
                        {
                            const uint32_t stagingIndex = stagingIndices[ instanceIndex ];
                            staging.buffers[ stagingIndex ] = srv;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            {   //  transfer uav data to the staging buffer
                const binary::Scattering& scattering = stageChain.uavScattering;
                const binary::StagingScatter* const stagingScatters = ( m_data.stagingScatters + scattering.target.index );
                const Binding* const scatterBindings = ( bindings + scattering.source );
                const uint32_t scatterCount = scattering.target.count;
                for( uint32_t scatterIndex = 0; scatterIndex < scatterCount; ++scatterIndex )
                {
                    const void* const desc = getDesc( scatterBindings[ scatterIndex ] );
                    if( desc )
                    {
                        const ResourceDescUAV& descUAV = *reinterpret_cast< const ResourceDescUAV* >( desc );
                        void* const uav = descUAV.unknown;
                        const UINT initialCount = descUAV.initialCount;
                        const binary::StagingScatter& stagingScatter = stagingScatters[ scatterIndex ];
                        const uint32_t* const stagingIndices = ( m_data.stagingIndices + stagingScatter.locations );
                        const uint32_t instanceCount = stagingScatter.instances;
                        for( uint32_t instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex )
                        {
                            const uint32_t stagingIndex = stagingIndices[ instanceIndex ];
                            staging.buffers[ stagingIndex ] = uav;
                            staging.offsets[ stagingIndex ] = initialCount;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            {   //  transfer sampler data to the staging buffer
                const binary::Scattering& scattering = stageChain.samplerScattering;
                const binary::StagingScatter* const stagingScatters = ( m_data.stagingScatters + scattering.target.index );
                const Binding* const scatterBindings = ( bindings + scattering.source );
                const uint32_t scatterCount = scattering.target.count;
                for( uint32_t scatterIndex = 0; scatterIndex < scatterCount; ++scatterIndex )
                {
                    const void* const desc = getDesc( scatterBindings[ scatterIndex ] );
                    if( desc )
                    {
                        const ResourceDescSampler& descSampler = *reinterpret_cast< const ResourceDescSampler* >( desc );
                        void* const sampler = descSampler.unknown;
                        const binary::StagingScatter& stagingScatter = stagingScatters[ scatterIndex ];
                        const uint32_t* const stagingIndices = ( m_data.stagingIndices + stagingScatter.locations );
                        const uint32_t instanceCount = stagingScatter.instances;
                        for( uint32_t instanceIndex = 0; instanceIndex < instanceCount; ++instanceIndex )
                        {
                            const uint32_t stagingIndex = stagingIndices[ instanceIndex ];
                            staging.buffers[ stagingIndex ] = sampler;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            {   //  input stream bindings
                const binary::Remapping& remapping = shaderDesc.streams;
                const uint32_t* const stagingIndices = ( m_data.stagingIndices + remapping.target.indices.index );
                const Binding* const remapBindings = ( bindings + remapping.source );
                const uint32_t remapCount = remapping.target.indices.count;
                for( uint32_t remapIndex = 0; remapIndex < remapCount; ++remapIndex )
                {
                    const void* const desc = getDesc( remapBindings[ remapIndex ] );
                    if( desc )
                    {
                        const ResourceDescLinear& descLinear = *reinterpret_cast< const ResourceDescLinear* >( desc );
                        const uint32_t stagingIndex = ( stagingIndices[ remapIndex ] + remapping.target.offset );
                        staging.buffers[ stagingIndex ] = descLinear.unknown;
                        staging.offsets[ stagingIndex ] = descLinear.offset;
                        staging.strides[ stagingIndex ] = descLinear.stride;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            {   //  output stream bindings
                const binary::Remapping& remapping = shaderDesc.outputs;
                const uint32_t* const stagingIndices = ( m_data.stagingIndices + remapping.target.indices.index );
                const Binding* const remapBindings = ( bindings + remapping.source );
                const uint32_t remapCount = remapping.target.indices.count;
                for( uint32_t remapIndex = 0; remapIndex < remapCount; ++remapIndex )
                {
                    const void* const desc = getDesc( remapBindings[ remapIndex ] );
                    if( desc )
                    {
                        const ResourceDescLinear& descLinear = *reinterpret_cast< const ResourceDescLinear* >( desc );
                        const uint32_t stagingIndex = ( stagingIndices[ remapIndex ] + remapping.target.offset );
                        staging.buffers[ stagingIndex ] = descLinear.unknown;
                        staging.offsets[ stagingIndex ] = descLinear.offset;
                        staging.strides[ stagingIndex ] = descLinear.stride;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            if( staging.renderable )
            {   //  renderable bindings
                RenderableStaging& renderableStaging = *staging.renderable;
                {   //  render target bindings
                    const uint32_t bindingIndex = shaderDesc.rtvIndex;
                    if( bindingIndex != 0xffffffff )
                    {
                        const binary::ListDesc& remapping = shaderDesc.rtvRemap;
                        const uint32_t* const stagingIndices = ( m_data.stagingIndices + remapping.index );
                        const Binding* const remapBindings = ( bindings + bindingIndex );
                        const uint32_t remapCount = remapping.count;
                        for( uint32_t remapIndex = 0; remapIndex < remapCount; ++remapIndex )
                        {
                            const void* const desc = getDesc( remapBindings[ remapIndex ] );
                            if( desc )
                            {
                                const uint32_t stagingIndex = stagingIndices[ remapIndex ];
                                const ResourceDescRTV& descRTV = *reinterpret_cast< const ResourceDescRTV* >( desc );
                                const FBOX& viewport = descRTV.viewport;
                                const SRECT& scissoring = descRTV.scissoring;
                                VIEWPORT& d3dViewport = renderableStaging.renderTargets.viewports[ stagingIndex ];
                                RECT& d3dScissoring = renderableStaging.renderTargets.scissoring[ stagingIndex ];
                                renderableStaging.renderTargets.buffers[ stagingIndex ] = descRTV.unknown;
                                d3dViewport.TopLeftX = viewport.min.x;
                                d3dViewport.TopLeftY = viewport.min.y;
                                d3dViewport.Width = ( viewport.max.x - viewport.min.x );
                                d3dViewport.Height = ( viewport.max.y - viewport.min.y );
                                d3dViewport.MinDepth = viewport.min.z;
                                d3dViewport.MaxDepth = viewport.max.z;
                                d3dScissoring.left = scissoring.left;
                                d3dScissoring.top = scissoring.top;
                                d3dScissoring.right = scissoring.right;
                                d3dScissoring.bottom = scissoring.bottom;
                            }
                            else
                            {
                                ret = false;
                            }
                        }
                    }
                }
                {   //  depth-stencil binding
                    const uint32_t bindingIndex = shaderDesc.dsvIndex;
                    if( bindingIndex != 0xffffffff )
                    {
                        const void* const desc = getDesc( bindings[ bindingIndex ] );
                        if( desc )
                        {
                            const ResourceDescDSV& descDSV = *reinterpret_cast< const ResourceDescDSV* >( desc );
                            const FBOX& viewport = descDSV.viewport;
                            const SRECT& scissoring = descDSV.scissoring;
                            VIEWPORT& d3dViewport = renderableStaging.depthStencil.viewport;
                            RECT& d3dScissoring = renderableStaging.depthStencil.scissoring;
                            renderableStaging.depthStencil.buffer = descDSV.unknown;
                            d3dViewport.TopLeftX = viewport.min.x;
                            d3dViewport.TopLeftY = viewport.min.y;
                            d3dViewport.Width = ( viewport.max.x - viewport.min.x );
                            d3dViewport.Height = ( viewport.max.y - viewport.min.y );
                            d3dViewport.MinDepth = viewport.min.z;
                            d3dViewport.MaxDepth = viewport.max.z;
                            d3dScissoring.left = scissoring.left;
                            d3dScissoring.top = scissoring.top;
                            d3dScissoring.right = scissoring.right;
                            d3dScissoring.bottom = scissoring.bottom;
                        }
                        else
                        {
                            ret = false;
                        }
                    }
                }
                {   //  raster state binding
                    const uint32_t bindingIndex = shaderDesc.rasterStateIndex;
                    if( bindingIndex != 0xffffffff )
                    {
                        const void* const desc = getDesc( bindings[ bindingIndex ] );
                        if( desc )
                        {
                            const ResourceDescRasterState& descRaster = *reinterpret_cast< const ResourceDescRasterState* >( desc );
                            renderableStaging.stateStaging.rasterState = descRaster.unknown;
                        }
                        else
                        {
                            ret = false;
                        }
                    }
                }
                {   //  depth state binding
                    const uint32_t bindingIndex = shaderDesc.depthStateIndex;
                    if( bindingIndex != 0xffffffff )
                    {
                        const void* const desc = getDesc( bindings[ bindingIndex ] );
                        if( desc )
                        {
                            const ResourceDescDepthState& descDepth = *reinterpret_cast< const ResourceDescDepthState* >( desc );
                            renderableStaging.stateStaging.depthState = descDepth.unknown;
                            renderableStaging.stateStaging.stencilRef = descDepth.stencilRef;
                        }
                        else
                        {
                            ret = false;
                        }
                    }
                }
                {   //  blend state binding
                    const uint32_t bindingIndex = shaderDesc.blendStateIndex;
                    if( bindingIndex != 0xffffffff )
                    {
                        const void* const desc = getDesc( bindings[ bindingIndex ] );
                        if( desc )
                        {
                            const ResourceDescBlendState& descBlend = *reinterpret_cast< const ResourceDescBlendState* >( desc );
                            renderableStaging.stateStaging.blendState = descBlend.unknown;
                            renderableStaging.stateStaging.sampleMask = descBlend.sampleMask;
                            renderableStaging.stateStaging.blendFactor[ 0 ] = descBlend.blendFactor.x;
                            renderableStaging.stateStaging.blendFactor[ 1 ] = descBlend.blendFactor.y;
                            renderableStaging.stateStaging.blendFactor[ 2 ] = descBlend.blendFactor.z;
                            renderableStaging.stateStaging.blendFactor[ 3 ] = descBlend.blendFactor.w;
                        }
                        else
                        {
                            ret = false;
                        }
                    }
                }
            }
            if( !ret )
            {
                ASSERT_FAIL( "Logged failure in call to CShaderLibrary::update()" );
            }
        }
        else
        {
            ASSERT_FAIL( "Invalid shader handle in call to CShaderLibrary::update()" );
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( ret );
}

bool CShaderLibrary::submit( const uint32_t shaderHandle, const ShaderStaging& staging ) const
{
    if( valid() )
    {   //  the library is valid
        const uint32_t shaderIndex = ~shaderHandle;
        if( shaderIndex < m_data.shaderDescCount )
        {
            const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
            const binary::StageChain& stageChain = m_data.stageChains[ shaderDesc.chainIndex ];
            const uint32_t stageMask = stageChain.stageMask;
            m_context->CSSetShader( NULL, NULL, 0 );
            if( stageMask & binary::EShaderStageComputeBit )
            {   //  set compute stage resources
                const uint stageIndex = binary::EShaderStageCompute;
                {   //  set the compute shader
                    m_context->CSSetShader( reinterpret_cast< ID3D11ComputeShader* >( m_interfaces[ stageChain.shaderIndices[ stageIndex ] ] ), NULL, 0 );
                }
                {   //  set compute stage constant buffers
                    const binary::Gathering& gathering = stageChain.bufferGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    //UINT* firstConstant = ( staging.paramStaging.offsets + gathering.source );
                    //UINT* numConstants = ( staging.paramStaging.strides + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->CSSetConstantBuffers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11Buffer* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                        //firstConstant += stagingGather.slotCount;
                        //numConstants += stagingGather.slotCount;
                    }
                }
                {   //  set compute stage srvs
                    const binary::Gathering& gathering = stageChain.uavGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    UINT* initialCounts = ( staging.offsets + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->CSSetUnorderedAccessViews( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11UnorderedAccessView* const* >( buffers ), initialCounts );
                        buffers += stagingGather.slotCount;
                        initialCounts += stagingGather.slotCount;
                    }
                }
                {   //  set compute stage uavs
                    const binary::Gathering& gathering = stageChain.srvGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->CSSetShaderResources( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11ShaderResourceView* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
                {   //  set compute stage samplers
                    const binary::Gathering& gathering = stageChain.samplerGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->CSSetSamplers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11SamplerState* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
            }
            m_context->PSSetShader( NULL, NULL, 0 );
            if( stageMask & binary::EShaderStagePixelBit )
            {   //  set pixel stage resources
                const uint stageIndex = binary::EShaderStagePixel;
                ASSERT( staging.renderable );
                if( staging.renderable )
                {   //  set the pixel shader, render states, targets, uavs and viewports
                    const RenderableStaging& renderable = *staging.renderable;
                    m_context->PSSetShader( reinterpret_cast< ID3D11PixelShader* >( m_interfaces[ stageChain.shaderIndices[ stageIndex ] ] ), NULL, 0 );
                    if( shaderDesc.rtvIndex != 0xffffffff )
                    {   //  set scissoring and viewports from render targets
                        m_context->RSSetViewports( static_cast< UINT >( shaderDesc.rtvRemap.count ), reinterpret_cast< const D3D11_VIEWPORT* >( &renderable.renderTargets.viewports[ 0 ] ) );
                        m_context->RSSetScissorRects( static_cast< UINT >( shaderDesc.rtvRemap.count ), reinterpret_cast< const D3D11_RECT* >( renderable.renderTargets.scissoring ) );
                    }
                    else if( shaderDesc.dsvIndex != 0xffffffff )
                    {   //  set scissoring and viewport from depth-stencil
                        m_context->RSSetViewports( 1, reinterpret_cast< const D3D11_VIEWPORT* >( &renderable.depthStencil.viewport ) );
                        m_context->RSSetScissorRects( 1, reinterpret_cast< const D3D11_RECT* >( &renderable.depthStencil.scissoring ) );
                    }
                    m_context->RSSetState( reinterpret_cast< ID3D11RasterizerState* >( renderable.stateStaging.rasterState ) );
                    m_context->OMSetDepthStencilState( reinterpret_cast< ID3D11DepthStencilState* >( renderable.stateStaging.depthState ), renderable.stateStaging.stencilRef );
                    m_context->OMSetBlendState( reinterpret_cast< ID3D11BlendState* >( renderable.stateStaging.blendState ), renderable.stateStaging.blendFactor, renderable.stateStaging.sampleMask );
                    const binary::Gathering& gathering = stageChain.uavGathering[ stageIndex ];
                    if( gathering.target.count )
                    {   //  uavs need setting
                        const binary::StagingGather& stagingGather = m_data.stagingGathers[ gathering.target.index ];
                        m_context->OMSetRenderTargetsAndUnorderedAccessViews(
                            static_cast< UINT >( shaderDesc.rtvRemap.count ),
                            reinterpret_cast< ID3D11RenderTargetView* const* >( renderable.renderTargets.buffers ),
                            reinterpret_cast< ID3D11DepthStencilView* >( renderable.depthStencil.buffer ),
                            static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ),
                            reinterpret_cast< ID3D11UnorderedAccessView* const* >( staging.buffers + gathering.source ),
                            ( staging.offsets + gathering.source ) );
                    }
                    else
                    {
                        m_context->OMSetRenderTargets(
                            static_cast< UINT >( shaderDesc.rtvRemap.count ),
                            reinterpret_cast< ID3D11RenderTargetView* const* >( renderable.renderTargets.buffers ),
                            reinterpret_cast< ID3D11DepthStencilView* >( renderable.depthStencil.buffer ) );
                    }
                }
                {   //  set pixel stage constant buffers
                    const binary::Gathering& gathering = stageChain.bufferGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    //UINT* firstConstant = ( staging.paramStaging.offsets + gathering.source );
                    //UINT* numConstants = ( staging.paramStaging.strides + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->PSSetConstantBuffers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11Buffer* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                        //firstConstant += stagingGather.slotCount;
                        //numConstants += stagingGather.slotCount;
                    }
                }
                {   //  set pixel stage srvs
                    const binary::Gathering& gathering = stageChain.srvGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->PSSetShaderResources( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11ShaderResourceView* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
                {   //  set pixel stage samplers
                    const binary::Gathering& gathering = stageChain.samplerGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->PSSetSamplers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11SamplerState* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
            }
            m_context->VSSetShader( NULL, NULL, 0 );
            if( stageMask & binary::EShaderStageVertexBit )
            {   //  set vertex stage resources
                const uint stageIndex = binary::EShaderStageVertex;
                {   //  set the vertex shader, input layout and input vertex streams
                    m_context->VSSetShader( reinterpret_cast< ID3D11VertexShader* >( m_interfaces[ stageChain.shaderIndices[ stageIndex ] ] ), NULL, 0 );
                    m_context->IASetInputLayout( reinterpret_cast< ID3D11InputLayout* >( m_interfaces[ shaderDesc.inputIndex ] ) );
                    m_context->IASetVertexBuffers( 0, static_cast< UINT >( shaderDesc.streams.target.indices.count ), reinterpret_cast< ID3D11Buffer* const* >( staging.buffers + shaderDesc.streams.target.offset ), ( staging.strides + shaderDesc.streams.target.offset ), ( staging.offsets + shaderDesc.streams.target.offset ) );
                }
                {   //  set vertex stage constant buffers
                    const binary::Gathering& gathering = stageChain.bufferGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    //UINT* firstConstant = ( staging.paramStaging.offsets + gathering.source );
                    //UINT* numConstants = ( staging.paramStaging.strides + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->VSSetConstantBuffers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11Buffer* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                        //firstConstant += stagingGather.slotCount;
                        //numConstants += stagingGather.slotCount;
                    }
                }
                {   //  set vertex stage srvs
                    const binary::Gathering& gathering = stageChain.srvGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->VSSetShaderResources( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11ShaderResourceView* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
                {   //  set vertex stage samplers
                    const binary::Gathering& gathering = stageChain.samplerGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->VSSetSamplers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11SamplerState* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
            }
            m_context->HSSetShader( NULL, NULL, 0 );
            if( stageMask & binary::EShaderStageHullBit )
            {   //  set hull stage resources
                const uint stageIndex = binary::EShaderStageHull;
                {   //  set the hull shader
                    m_context->HSSetShader( reinterpret_cast< ID3D11HullShader* >( m_interfaces[ stageChain.shaderIndices[ stageIndex ] ] ), NULL, 0 );
                }
                {   //  set hull stage constant buffers
                    const binary::Gathering& gathering = stageChain.bufferGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    //UINT* firstConstant = ( staging.paramStaging.offsets + gathering.source );
                    //UINT* numConstants = ( staging.paramStaging.strides + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->HSSetConstantBuffers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11Buffer* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                        //firstConstant += stagingGather.slotCount;
                        //numConstants += stagingGather.slotCount;
                    }
                }
                {   //  set hull stage srvs
                    const binary::Gathering& gathering = stageChain.srvGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->HSSetShaderResources( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11ShaderResourceView* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
                {   //  set hull stage samplers
                    const binary::Gathering& gathering = stageChain.samplerGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->HSSetSamplers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11SamplerState* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
            }
            m_context->DSSetShader( NULL, NULL, 0 );
            if( stageMask & binary::EShaderStageDomainBit )
            {   //  set domain stage resources
                const uint stageIndex = binary::EShaderStageDomain;
                {   //  set the domain shader
                    m_context->DSSetShader( reinterpret_cast< ID3D11DomainShader* >( m_interfaces[ stageChain.shaderIndices[ stageIndex ] ] ), NULL, 0 );
                }
                {   //  set domain stage constant buffers
                    const binary::Gathering& gathering = stageChain.bufferGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    //UINT* firstConstant = ( staging.paramStaging.offsets + gathering.source );
                    //UINT* numConstants = ( staging.paramStaging.strides + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->DSSetConstantBuffers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11Buffer* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                        //firstConstant += stagingGather.slotCount;
                        //numConstants += stagingGather.slotCount;
                    }
                }
                {   //  set domain stage srvs
                    const binary::Gathering& gathering = stageChain.srvGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->DSSetShaderResources( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11ShaderResourceView* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
                {   //  set domain stage samplers
                    const binary::Gathering& gathering = stageChain.samplerGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->DSSetSamplers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11SamplerState* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
            }
            m_context->GSSetShader( NULL, NULL, 0 );
            if( ( stageMask & binary::EShaderStageGeometryBit ) || ( shaderDesc.outputIndex != 0xffffffff ) )
            {   //  set geometry stage resources
                const uint stageIndex = binary::EShaderStageGeometry;
                {   //  set the geometry shader and any stream-out streams
                    if( shaderDesc.outputIndex != 0xffffffff )
                    {
                        m_context->GSSetShader( reinterpret_cast< ID3D11GeometryShader* >( m_interfaces[ shaderDesc.outputIndex ] ), NULL, 0 );
                        m_context->SOSetTargets( static_cast< UINT >( shaderDesc.outputs.target.indices.count ), reinterpret_cast< ID3D11Buffer* const* >( staging.buffers + shaderDesc.outputs.target.offset ), ( staging.offsets + shaderDesc.outputs.target.offset ) );
                    }
                    else
                    {
                        m_context->GSSetShader( reinterpret_cast< ID3D11GeometryShader* >( m_interfaces[ stageChain.shaderIndices[ stageIndex ] ] ), NULL, 0 );
                    }
                }
                {   //  set geometry stage constant buffers
                    const binary::Gathering& gathering = stageChain.bufferGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    //UINT* firstConstant = ( staging.paramStaging.offsets + gathering.source );
                    //UINT* numConstants = ( staging.paramStaging.strides + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->GSSetConstantBuffers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11Buffer* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                        //firstConstant += stagingGather.slotCount;
                        //numConstants += stagingGather.slotCount;
                    }
                }
                {   //  set geometry stage srvs
                    const binary::Gathering& gathering = stageChain.srvGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->GSSetShaderResources( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11ShaderResourceView* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
                {   //  set geometry stage samplers
                    const binary::Gathering& gathering = stageChain.samplerGathering[ stageIndex ];
                    const binary::StagingGather* const stagingGathers = ( m_data.stagingGathers + gathering.target.index );
                    void** buffers = ( staging.buffers + gathering.source );
                    const uint32_t gatherCount = gathering.target.count;
                    for( uint32_t gatherIndex = 0; gatherIndex < gatherCount; ++gatherIndex )
                    {
                        const binary::StagingGather& stagingGather = stagingGathers[ gatherIndex ];
                        m_context->GSSetSamplers( static_cast< UINT >( stagingGather.firstSlot ), static_cast< UINT >( stagingGather.slotCount ), reinterpret_cast< ID3D11SamplerState* const* >( buffers ) );
                        buffers += stagingGather.slotCount;
                    }
                }
            }
            return( true );
        }
        else
        {
            ASSERT_FAIL( "Invalid shader handle" );
        }
    }
    else
    {
        ASSERT_FAIL( "CShaderLibrary not initialised" );
    }
    return( false );
}

bool CShaderLibrary::execute( const ResourceCommand& resourceCommand ) const
{
    bool ret = false;
    const void* const command_desc = resourceCommand.pipe();
    if( command_desc )
    {
        const ResourceDescCommand& command = *reinterpret_cast< const ResourceDescCommand* >( command_desc );
        switch( command.func )
        {
            case( ECmdFuncNULL ):
            {
                ret = true;
                break;
            }
            case( ECmdFuncAuto ):
            {
                if( setTopology( command.primDesc ) )
                {
                    m_context->DrawAuto();
                    ret = true;
                }
                break;
            }
            case( ECmdFuncDrawing ):
            {
                if( setTopology( command.primDesc ) )
                {
                    m_context->Draw( command.drawing.vertexCount, command.drawing.firstVertexIndex );
                    ret = true;
                }
                break;
            }
            case( ECmdFuncIndexed ):
            {
                if( command.indices.valid() )
                {
                    if( setTopology( command.primDesc ) )
                    {
                        const void* const indices_desc = command.indices.pipe();
                        if( indices_desc )
                        {
                            const ResourceDescLinear& descLinear = *reinterpret_cast< const ResourceDescLinear* >( indices_desc );
                            m_context->IASetIndexBuffer( reinterpret_cast< ID3D11Buffer* >( descLinear.unknown ), ( ( descLinear.stride == 2 ) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT ), descLinear.offset );
                            m_context->DrawIndexed( command.indexed.indexCount, command.indexed.firstIndexIndex, command.indexed.indexOffset );
                            ret = true;
                        }
                    }
                }
                else
                {
                    ASSERT_FAIL( "Execute draw indexed: index buffer is not specified" );
                }
                break;
            }
            case( ECmdFuncInstancedDrawing ):
            {
                if( setTopology( command.primDesc ) )
                {
                    m_context->DrawInstanced( command.drawing.vertexCount, command.instancing.instanceCount, command.drawing.firstVertexIndex, command.instancing.firstInstanceIndex );
                    ret = true;
                }
                break;
            }
            case( ECmdFuncInstancedIndexed ):
            {
                if( command.indices.valid() )
                {
                    if( setTopology( command.primDesc ) )
                    {
                        const void* const indices_desc = command.indices.pipe();
                        if( indices_desc )
                        {
                            const ResourceDescLinear& indices = *reinterpret_cast< const ResourceDescLinear* >( indices_desc );
                            m_context->IASetIndexBuffer( reinterpret_cast< ID3D11Buffer* >( indices.unknown ), ( ( indices.stride == 2 ) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT ), indices.offset );
                            m_context->DrawIndexedInstanced( command.indexed.indexCount, command.instancing.instanceCount, command.indexed.firstIndexIndex, command.indexed.indexOffset, command.instancing.firstInstanceIndex );
                            ret = true;
                        }
                    }
                }
                else
                {
                    ASSERT_FAIL( "Execute draw instanced indexed: index buffer is not specified" );
                }
                break;
            }
            case( ECmdFuncInstancedDrawingIndirect ):
            {
                if( command.indirect.valid() )
                {
                    if( setTopology( command.primDesc ) )
                    {
                        const void* const indirect_desc = command.indirect.pipe();
                        if( indirect_desc )
                        {
                            const ResourceDescLinear& indirect = *reinterpret_cast< const ResourceDescLinear* >( indirect_desc );
                            m_context->DrawInstancedIndirect( reinterpret_cast< ID3D11Buffer* >( indirect.unknown ), indirect.offset );
                            ret = true;
                        }
                    }
                }
                else
                {
                    ASSERT_FAIL( "Execute draw indirect: params buffer is not specified" );
                }
                break;
            }
            case( ECmdFuncInstancedIndexedIndirect ):
            {
                if( command.indirect.valid() )
                {
                    if( command.indices.valid() )
                    {
                        if( setTopology( command.primDesc ) )
                        {
                            const void* const indices_desc = command.indices.pipe();
                            const void* const indirect_desc = command.indirect.pipe();
                            if( indices_desc && indirect_desc )
                            {
                                const ResourceDescLinear& indices = *reinterpret_cast< const ResourceDescLinear* >( indices_desc );
                                const ResourceDescLinear& indirect = *reinterpret_cast< const ResourceDescLinear* >( indirect_desc );
                                m_context->IASetIndexBuffer( reinterpret_cast< ID3D11Buffer* >( indices.unknown ), ( ( indices.stride == 2 ) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT ), indices.offset );
                                m_context->DrawIndexedInstancedIndirect( reinterpret_cast< ID3D11Buffer* >( indirect.unknown ), indirect.offset );
                                ret = true;
                            }
                        }
                    }
                    else
                    {
                        ASSERT_FAIL( "Execute draw indirect indexed: index buffer is not specified" );
                    }
                }
                else
                {
                    ASSERT_FAIL( "Execute draw indirect indexed: params buffer is not specified" );
                }
                break;
            }
            case( ECmdFuncDispatch ):
            {
                m_context->Dispatch( command.dispatch.threadGroupCountX, command.dispatch.threadGroupCountY, command.dispatch.threadGroupCountZ );
                ret = true;
                break;
            }
            case( ECmdFuncDispatchIndirect ):
            {
                if( command.indirect.valid() )
                {
                    const void* const indirect_desc = command.indirect.pipe();
                    if( indirect_desc )
                    {
                        const ResourceDescLinear& indirect = *reinterpret_cast< const ResourceDescLinear* >( indirect_desc );
                        m_context->DispatchIndirect( reinterpret_cast< ID3D11Buffer* >( indirect.unknown ), indirect.offset );
                        ret = true;
                    }
                }
                else
                {
                    ASSERT_FAIL( "Execute dispatch indirect: params buffer is not specified" );
                }
                break;
            }
            default:
            {
                ASSERT_FAIL( "Execute: unknown command" );
                break;
            }
        }
    }
    else
    {
        ASSERT_FAIL( "Call to ResourceCommand::pipe() returned NULL" );
    }
    return( ret );
}

void CShaderLibrary::clearState() const
{   //  set all DirectX bindings to NULL
    UINT zeroArray[ 128 ] = { 0 };
    void* nullArray[ 128 ] = { 0 };
    FLOAT blendFactor[ 4 ] = { 0 };
    m_context->CSSetShader( NULL, NULL, 0 );
    m_context->PSSetShader( NULL, NULL, 0 );
    m_context->VSSetShader( NULL, NULL, 0 );
    m_context->HSSetShader( NULL, NULL, 0 );
    m_context->DSSetShader( NULL, NULL, 0 );
    m_context->GSSetShader( NULL, NULL, 0 );
    m_context->CSSetConstantBuffers( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, reinterpret_cast< ID3D11Buffer* const* >( nullArray ) );
    m_context->PSSetConstantBuffers( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, reinterpret_cast< ID3D11Buffer* const* >( nullArray ) );
    m_context->VSSetConstantBuffers( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, reinterpret_cast< ID3D11Buffer* const* >( nullArray ) );
    m_context->HSSetConstantBuffers( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, reinterpret_cast< ID3D11Buffer* const* >( nullArray ) );
    m_context->DSSetConstantBuffers( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, reinterpret_cast< ID3D11Buffer* const* >( nullArray ) );
    m_context->GSSetConstantBuffers( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, reinterpret_cast< ID3D11Buffer* const* >( nullArray ) );
    m_context->CSSetShaderResources( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, reinterpret_cast< ID3D11ShaderResourceView* const* >( nullArray ) );
    m_context->PSSetShaderResources( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, reinterpret_cast< ID3D11ShaderResourceView* const* >( nullArray ) );
    m_context->VSSetShaderResources( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, reinterpret_cast< ID3D11ShaderResourceView* const* >( nullArray ) );
    m_context->HSSetShaderResources( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, reinterpret_cast< ID3D11ShaderResourceView* const* >( nullArray ) );
    m_context->DSSetShaderResources( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, reinterpret_cast< ID3D11ShaderResourceView* const* >( nullArray ) );
    m_context->GSSetShaderResources( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, reinterpret_cast< ID3D11ShaderResourceView* const* >( nullArray ) );
    m_context->CSSetSamplers( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, reinterpret_cast< ID3D11SamplerState* const* >( nullArray ) );
    m_context->PSSetSamplers( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, reinterpret_cast< ID3D11SamplerState* const* >( nullArray ) );
    m_context->VSSetSamplers( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, reinterpret_cast< ID3D11SamplerState* const* >( nullArray ) );
    m_context->HSSetSamplers( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, reinterpret_cast< ID3D11SamplerState* const* >( nullArray ) );
    m_context->DSSetSamplers( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, reinterpret_cast< ID3D11SamplerState* const* >( nullArray ) );
    m_context->GSSetSamplers( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, reinterpret_cast< ID3D11SamplerState* const* >( nullArray ) );
    m_context->CSSetUnorderedAccessViews( 0, D3D11_1_UAV_SLOT_COUNT, reinterpret_cast< ID3D11UnorderedAccessView* const* >( nullArray ), zeroArray );
    m_context->IASetInputLayout( 0 );
    m_context->IASetIndexBuffer( 0, DXGI_FORMAT_R16_UINT, 0 );
    m_context->IASetVertexBuffers( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, reinterpret_cast< ID3D11Buffer* const* >( nullArray ), zeroArray, zeroArray );
    m_context->SOSetTargets( 4, reinterpret_cast< ID3D11Buffer* const* >( nullArray ), zeroArray );
    m_context->RSSetState( NULL );
    m_context->OMSetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, reinterpret_cast< ID3D11RenderTargetView* const* >( nullArray ), NULL );
    m_context->OMSetDepthStencilState( NULL, 0 );
    m_context->OMSetBlendState( NULL, blendFactor, 0xffffffff );
}

bool CShaderLibrary::createInterfaces()
{
    const binary::ShaderLibrary& shaderLibrary = *m_data.binary.library;
    D3D11_INPUT_ELEMENT_DESC* inputs = new D3D11_INPUT_ELEMENT_DESC[ shaderLibrary.setup.maxStreamElements ];
    D3D11_SO_DECLARATION_ENTRY* outputs = new D3D11_SO_DECLARATION_ENTRY[ shaderLibrary.setup.maxStreamOutElements ];
    bool ret = createInterfaces( inputs, outputs );
    delete[] outputs;
    delete[] inputs;
    return( ret );
}

bool CShaderLibrary::createInterfaces( D3D11_INPUT_ELEMENT_DESC* const inputs, D3D11_SO_DECLARATION_ENTRY* const outputs )
{
    const binary::ShaderLibrary& shaderLibrary = *m_data.binary.library;
    uint32_t interfaceIndex = 0;
    for( uint shaderStage = 0; shaderStage < binary::EShaderStageCount; ++shaderStage )
    {   //  create the shader interfaces
        const binary::ListDesc& listDesc = shaderLibrary.setup.shaderStages[ shaderStage ];
        const binary::BlobDesc* const shaderBlobs = ( m_data.shaderBlobs + listDesc.index );
        const uint32_t shaderCount = listDesc.count;
        for( uint32_t shaderIndex = 0; shaderIndex < shaderCount; ++shaderIndex )
        {
            IUnknown* shaderInterface = createDeviceShader( shaderBlobs[ shaderIndex ], shaderStage );
            if( shaderInterface == NULL )
            {
                return( false );
            }
            m_interfaces[ interfaceIndex ] = shaderInterface;
            ++interfaceIndex;
        }
    }
    {   //  create the stream-out geometry shader interfaces
        const binary::ListDesc& listDesc = shaderLibrary.setup.outputAssemblies;
        const binary::AssemblyLayout* const assemblyLayouts = ( m_data.assemblyLayouts + listDesc.index );
        const uint32_t assemblyLayoutCount = listDesc.count;
        for( uint32_t assemblyLayoutIndex = 0; assemblyLayoutIndex < assemblyLayoutCount; ++assemblyLayoutIndex )
        {
            const binary::AssemblyLayout& assemblyLayout = assemblyLayouts[ assemblyLayoutIndex ];
            uint32_t elementIndex = 0;
            UINT strides[ 4 ];
            const binary::AssemblyStream* const assemblyStreams = ( m_data.assemblyStreams + assemblyLayout.streams.index );
            const uint32_t assemblyStreamCount = assemblyLayout.streams.count;
            for( uint32_t assemblyStreamIndex = 0; assemblyStreamIndex < assemblyStreamCount; ++assemblyStreamIndex )
            {
                const binary::AssemblyStream& assemblyStream = assemblyStreams[ assemblyStreamIndex ];
                const binary::StreamFormat& streamFormat = m_data.streamFormats[ assemblyStream.streamFormatIndex ];
                const binary::StreamElement* const streamElements = ( m_data.streamElements + streamFormat.elements.index );
                uint32_t stride = streamFormat.stride;
                uint32_t offset = 0;
                const binary::AssemblyElement* const assemblyElements = ( m_data.assemblyElements + assemblyStream.elements.index );
                const uint32_t assemblyElementCount = assemblyStream.elements.count;
                for( uint32_t assemblyElementIndex = 0; assemblyElementIndex < assemblyElementCount; ++assemblyElementIndex )
                {
                    const binary::AssemblyElement& assemblyElement = assemblyElements[ assemblyElementIndex ];
                    const binary::StreamElement& streamElement = streamElements[ assemblyElement.streamElementIndex ];
                    if( streamElement.offset != offset )
                    {   //  we need to add a gap element
                        D3D11_SO_DECLARATION_ENTRY& gapElement = outputs[ elementIndex ];
                        memset( &gapElement, 0, sizeof( gapElement ) );
                        gapElement.Stream = static_cast< UINT >( assemblyStreamIndex );
                        gapElement.ComponentCount = static_cast< BYTE >( ( streamElement.offset - offset ) >> 2 );
                        gapElement.OutputSlot = static_cast< BYTE >( assemblyStreamIndex );
                        ++elementIndex;
                    }
                    D3D11_SO_DECLARATION_ENTRY& outElement = outputs[ elementIndex ];
                    memset( &outElement, 0, sizeof( outElement ) );
                    outElement.Stream = static_cast< UINT >( assemblyStreamIndex );
                    outElement.SemanticName = reinterpret_cast< const char* const >( m_data.stringTable->getString( assemblyElement.semanticHandle ) );
                    outElement.SemanticIndex = static_cast< UINT >( assemblyElement.semanticIndex );
                    outElement.StartComponent = static_cast< BYTE >( assemblyElement.componentIndex );
                    outElement.ComponentCount = static_cast< BYTE >( streamElement.size >> 2 );
                    outElement.OutputSlot = static_cast< BYTE >( assemblyStreamIndex );
                    ++elementIndex;
                    offset = ( streamElement.offset + streamElement.size );
                }
                if( offset != stride )
                {   //  we need to add a gap element
                    D3D11_SO_DECLARATION_ENTRY& gapElement = outputs[ elementIndex ];
                    memset( &gapElement, 0, sizeof( gapElement ) );
                    gapElement.Stream = static_cast< UINT >( assemblyStreamIndex );
                    gapElement.ComponentCount = static_cast< BYTE >( ( stride - offset ) >> 2 );
                    gapElement.OutputSlot = static_cast< BYTE >( assemblyStreamIndex );
                    ++elementIndex;
                }
                strides[ assemblyStreamIndex ] = static_cast< UINT >( stride );
            }
            const binary::BlobDesc& shaderBlob = m_data.shaderBlobs[ assemblyLayout.shaderBinaryIndex ];
            ID3D11GeometryShader* shaderInterface = NULL;
            if( m_device->CreateGeometryShaderWithStreamOutput( reinterpret_cast< const void * >( m_data.shaderBinaries + shaderBlob.offset ), static_cast< SIZE_T >( shaderBlob.size ), outputs, static_cast< UINT >( elementIndex ), strides, static_cast< UINT >( assemblyStreamCount ), static_cast< UINT >( assemblyLayout.rasterisedStreamIndex ), NULL, &shaderInterface ) != S_OK )
            {
                ASSERT_FAIL( "CreateGeometryShaderWithStreamOutput() failed" );
                return( false );
            }
            m_interfaces[ interfaceIndex ] = shaderInterface;
            ++interfaceIndex;
        }
    }
    {   //  create the stream input layout interfaces
        const binary::ListDesc& listDesc = shaderLibrary.setup.inputAssemblies;
        const binary::AssemblyLayout* const assemblyLayouts = ( m_data.assemblyLayouts + listDesc.index );
        const uint32_t assemblyLayoutCount = listDesc.count;
        for( uint32_t assemblyLayoutIndex = 0; assemblyLayoutIndex < assemblyLayoutCount; ++assemblyLayoutIndex )
        {
            const binary::AssemblyLayout& assemblyLayout = assemblyLayouts[ assemblyLayoutIndex ];
            uint32_t elementIndex = 0;
            const binary::AssemblyStream* const assemblyStreams = ( m_data.assemblyStreams + assemblyLayout.streams.index );
            const uint32_t assemblyStreamCount = assemblyLayout.streams.count;
            for( uint32_t assemblyStreamIndex = 0; assemblyStreamIndex < assemblyStreamCount; ++assemblyStreamIndex )
            {
                const binary::AssemblyStream& assemblyStream = assemblyStreams[ assemblyStreamIndex ];
                const binary::StreamFormat& streamFormat = m_data.streamFormats[ assemblyStream.streamFormatIndex ];
                const binary::StreamElement* const streamElements = ( m_data.streamElements + streamFormat.elements.index );
                D3D11_INPUT_CLASSIFICATION InputSlotClass = ( assemblyStream.instanceStepRate ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA );
                const binary::AssemblyElement* const assemblyElements = ( m_data.assemblyElements + assemblyStream.elements.index );
                const uint32_t assemblyElementCount = assemblyStream.elements.count;
                for( uint32_t assemblyElementIndex = 0; assemblyElementIndex < assemblyElementCount; ++assemblyElementIndex )
                {
                    const binary::AssemblyElement& assemblyElement = assemblyElements[ assemblyElementIndex ];
                    const binary::StreamElement& streamElement = streamElements[ assemblyElement.streamElementIndex ];
                    D3D11_INPUT_ELEMENT_DESC& inElement = inputs[ elementIndex ];
                    memset( &inElement, 0, sizeof( inElement ) );
                    inElement.SemanticName = reinterpret_cast< const char* const >( m_data.stringTable->getString( assemblyElement.semanticHandle ) );
                    inElement.SemanticIndex = static_cast< UINT >( assemblyElement.semanticIndex );
                    inElement.Format = s_EELEMENT2DXGI[ streamElement.format ];
                    inElement.InputSlot = static_cast< UINT >( assemblyStreamIndex );
                    inElement.AlignedByteOffset = static_cast< UINT >( streamElement.offset );
                    inElement.InputSlotClass = InputSlotClass;
                    inElement.InstanceDataStepRate = static_cast< UINT >( assemblyStream.instanceStepRate );
                    ++elementIndex;
                }
            }
            const binary::BlobDesc& shaderBlob = m_data.shaderBlobs[ assemblyLayout.shaderBinaryIndex ];
            ID3D11InputLayout* shaderInterface = NULL;
            if( m_device->CreateInputLayout( inputs, static_cast< UINT >( elementIndex ), reinterpret_cast< const void * >( m_data.shaderBinaries + shaderBlob.offset ), static_cast< SIZE_T >( shaderBlob.size ), &shaderInterface ) != S_OK )
            {
                ASSERT_FAIL( "CreateInputLayout() failed" );
                return( false );
            }
            m_interfaces[ interfaceIndex ] = shaderInterface;
            ++interfaceIndex;
        }
    }
    return( true );
}

IUnknown* CShaderLibrary::createDeviceShader( const binary::BlobDesc& shaderBlob, const uint shaderStage ) const
{
    IUnknown* shaderInterface = NULL;
    switch( shaderStage )
    {
        case( binary::EShaderStageCompute ):
        {
            if( m_device->CreateComputeShader( reinterpret_cast< const void * >( m_data.shaderBinaries + shaderBlob.offset ), static_cast< SIZE_T >( shaderBlob.size ), NULL, reinterpret_cast< ID3D11ComputeShader** >( &shaderInterface ) ) != S_OK )
            {
                ASSERT_FAIL( "CreateComputeShader() failed" );
                shaderInterface = NULL;
            }
            break;
        }
        case( binary::EShaderStagePixel ):
        {
            if( m_device->CreatePixelShader( reinterpret_cast< const void * >( m_data.shaderBinaries + shaderBlob.offset ), static_cast< SIZE_T >( shaderBlob.size ), NULL, reinterpret_cast< ID3D11PixelShader** >( &shaderInterface ) ) != S_OK )
            {
                ASSERT_FAIL( "CreatePixelShader() failed" );
                shaderInterface = NULL;
            }
            break;
        }
        case( binary::EShaderStageVertex ):
        {
            if( m_device->CreateVertexShader( reinterpret_cast< const void * >( m_data.shaderBinaries + shaderBlob.offset ), static_cast< SIZE_T >( shaderBlob.size ), NULL, reinterpret_cast< ID3D11VertexShader** >( &shaderInterface ) ) != S_OK )
            {
                ASSERT_FAIL( "CreateVertexShader() failed" );
                shaderInterface = NULL;
            }
            break;
        }
        case( binary::EShaderStageHull ):
        {
            if( m_device->CreateHullShader( reinterpret_cast< const void * >( m_data.shaderBinaries + shaderBlob.offset ), static_cast< SIZE_T >( shaderBlob.size ), NULL, reinterpret_cast< ID3D11HullShader** >( &shaderInterface ) ) != S_OK )
            {
                ASSERT_FAIL( "CreateHullShader() failed" );
                shaderInterface = NULL;
            }
            break;
        }
        case( binary::EShaderStageDomain ):
        {
            if( m_device->CreateDomainShader( reinterpret_cast< const void * >( m_data.shaderBinaries + shaderBlob.offset ), static_cast< SIZE_T >( shaderBlob.size ), NULL, reinterpret_cast< ID3D11DomainShader** >( &shaderInterface ) ) != S_OK )
            {
                ASSERT_FAIL( "CreateDomainShader() failed" );
                shaderInterface = NULL;
            }
            break;
        }
        case( binary::EShaderStageGeometry ):
        {
            if( m_device->CreateGeometryShader( reinterpret_cast< const void * >( m_data.shaderBinaries + shaderBlob.offset ), static_cast< SIZE_T >( shaderBlob.size ), NULL, reinterpret_cast< ID3D11GeometryShader** >( &shaderInterface ) ) != S_OK )
            {
                ASSERT_FAIL( "CreateGeometryShader() failed" );
                shaderInterface = NULL;
            }
            break;
        }
        default:
        {
            ASSERT_FAIL( "createDeviceShader() called with unknown shader stage" );
            break;
        }
    }
    return( shaderInterface );
}

ShaderStaging* CShaderLibrary::createStaging( const uint32_t stagingCount, const bool renderable )
{
    size_t stagingSize = ( sizeof( ShaderStaging ) + ( stagingCount * ( sizeof( void* ) + ( sizeof( UINT ) << 1 ) ) ) );
    if( renderable )
    {
        stagingSize += sizeof( RenderableStaging );
    }
    stagingSize = ( ( stagingSize + 7 ) & ~7 );
    ShaderStaging* staging = reinterpret_cast< ShaderStaging* >( new void*[ stagingSize / sizeof( void* ) ] );
    memset( staging, 0, stagingSize );
    staging->buffers = reinterpret_cast< void** >( &staging[ 1 ] );
    staging->offsets = reinterpret_cast< UINT* >( &staging->buffers[ stagingCount ] );
    staging->strides = reinterpret_cast< UINT* >( &staging->offsets[ stagingCount ] );
    staging->renderable = ( renderable ? reinterpret_cast< RenderableStaging* >( &staging->strides[ stagingCount ] ) : NULL );
    return( staging );
}

bool CShaderLibrary::setTopology( const PrimDesc& primDesc ) const
{
    D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    switch( primDesc.primType )
    {
        case( EPrimTypePointList ):
        {
            topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        }
        case( EPrimTypeLineList ):
        {
            topology = ( primDesc.adjacency ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ : D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
            break;
        }
        case( EPrimTypeLineStrip ):
        {
            topology = ( primDesc.adjacency ? D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ : D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
            break;
        }
        case( EPrimTypeTriList ):
        {
            topology = ( primDesc.adjacency ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
            break;
        }
        case( EPrimTypeTriStrip ):
        {
            topology = ( primDesc.adjacency ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
            break;
        }
        case( EPrimTypePatch ):
        {
            if( ( primDesc.cpCount > 0 ) && ( primDesc.cpCount <= 64 ) )
            {
                topology = static_cast< D3D11_PRIMITIVE_TOPOLOGY >( static_cast< UINT >( D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST ) + primDesc.cpCount - 1 );
            }
            break;
        }
        default:
        {
            break;
        }
    }
    if( topology != D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED )
    {
        m_context->IASetPrimitiveTopology( topology );
        return( true );
    }
    ASSERT_FAIL( "Execute draw: primitive topology is invalid" );
    return( false );
}

void CShaderLibrary::mangle()
{
    compression::m2f::Encode( reinterpret_cast< uint8_t* >( &m_data.binary.library[ 1 ] ), static_cast< uint32_t >( m_data.binary.library->header.fileSize - sizeof( binary::ShaderLibrary ) ) );
}

void CShaderLibrary::unmangle()
{
    compression::m2f::Decode( reinterpret_cast< uint8_t* >( &m_data.binary.library[ 1 ] ), static_cast< uint32_t >( m_data.binary.library->header.fileSize - sizeof( binary::ShaderLibrary ) ) );
}

int CShaderLibrary::findShader( const StringItem& name ) const
{
    if( name.notEmpty() )
    {
        int nameIndex = findString( name );
        if( nameIndex >= 0 )
        {
            uint32_t nameHandle = static_cast< uint32_t >( nameIndex );
            int shaderIndex = 0;
            int shaderLimit = static_cast< int >( m_data.shaderDescCount - 1 );
            while( shaderIndex < shaderLimit )
            {
                int shaderCheck = ( ( shaderIndex + shaderLimit ) >> 1 );
                if( nameHandle > m_data.shaderDescs[ shaderCheck ].nameHandle )
                {
                    shaderIndex = ( shaderCheck + 1 );
                }
                else
                {
                    shaderLimit = shaderCheck;
                }
            }
            if( nameHandle == m_data.shaderDescs[ shaderIndex ].nameHandle )
            {   //  found the shader
                return( shaderIndex );
            }
        }
    }
    return( -1 );
}

int CShaderLibrary::findResource( const StringItem& name, const uint32_t resourceType ) const
{
    if( name.notEmpty() )
    {
        int nameIndex = findString( name );
        if( nameIndex >= 0 )
        {
            uint32_t nameHandle = static_cast< uint32_t >( nameIndex );
            int resourceItemIndex = 0;
            int resourceItemLimit = static_cast< int >( m_data.resourceItemCount - 1 );
            while( resourceItemIndex < resourceItemLimit )
            {
                int resourceItemCheck = ( ( resourceItemIndex + resourceItemLimit ) >> 1 );
                const binary::ResourceItem& resourceItem = m_data.resourceItems[ resourceItemCheck ];
                if( ( resourceType > resourceItem.resourceType ) || ( ( resourceType == resourceItem.resourceType ) && ( nameHandle > resourceItem.nameHandle ) ) )
                {
                    resourceItemIndex = ( resourceItemCheck + 1 );
                }
                else
                {
                    resourceItemLimit = resourceItemCheck;
                }
            }
            const binary::ResourceItem& resourceItem = m_data.resourceItems[ resourceItemIndex ];
            if( ( resourceType == resourceItem.resourceType ) && ( nameHandle == resourceItem.nameHandle ) )
            {   //  found the resourceItem
                return( resourceItemIndex );
            }
        }
    }
    return( -1 );
}

int CShaderLibrary::findInShader( const uint32_t shaderHandle, const uint32_t resourceItemIndex ) const
{
    const uint32_t shaderIndex = ~shaderHandle;
    if( shaderIndex < m_data.shaderDescCount )
    {
        const binary::ShaderDesc& shaderDesc = m_data.shaderDescs[ shaderIndex ];
        const uint32_t* const resourceMap = ( m_data.resourceIndices + shaderDesc.resources.index );
        int bindingIndex = 0;
        int bindingLimit = static_cast< int >( shaderDesc.resources.count - 1 );
        while( bindingIndex < bindingLimit )
        {
            int bindingCheck = ( ( bindingIndex + bindingLimit ) >> 1 );
            if( resourceItemIndex > resourceMap[ bindingCheck ] )
            {
                bindingIndex = ( bindingCheck + 1 );
            }
            else
            {
                bindingLimit = bindingCheck;
            }
        }
        if( resourceItemIndex == resourceMap[ bindingIndex ] )
        {   //  found the binding index
            return( bindingIndex );
        }
    }
    else
    {
        ASSERT_FAIL( "Invalid shader handle" );
    }
    return( -1 );
}

const void* CShaderLibrary::getDesc( const Binding& binding ) const
{
    if( binding.valid() )
    {
        const CBinding& concrete = *static_cast< const CBinding* >( binding.naked() );
        const Resource& resource = concrete.getResource();
        if( resource.valid() )
        {
            const void* desc = resource.pipe();
            if( desc )
            {
                return( desc );
            }
            else
            {
                DEBUG_LOG( "%s binding '%s' call to pipe() on the bound resource returned NULL", getEResourceTypeName( static_cast< EResourceType >( m_data.resourceItems[ ~concrete.getHandle() ].resourceType ) ), getName( m_data.resourceItems[ ~concrete.getHandle() ].nameHandle ) );
            }
        }
        else
        {
            DEBUG_LOG( "%s binding '%s' has no resource bound to it", getEResourceTypeName( static_cast< EResourceType >( m_data.resourceItems[ ~concrete.getHandle() ].resourceType ) ), getName( m_data.resourceItems[ ~concrete.getHandle() ].nameHandle ) );
        }
    }
    else
    {
        DEBUG_LOG( "Binding is not specified" );
    }
    return( NULL );
}

uint32_t CShaderLibrary::getHandle( const Binding& binding ) const
{
    if( binding.valid() )
    {   //  the binding object exists
        const CBinding& concrete = *static_cast< const CBinding* >( binding.naked() );
        if( concrete.getLibrary() == this )
        {   //  the binding object was created by this library
            uint32_t handle = concrete.getHandle();
            if( ~handle < m_data.resourceItemCount )
            {
                return( handle );
            }
            ASSERT_FAIL( "Binding object appears to be corrupted" );
        }
        else
        {
            ASSERT_FAIL( "Binding object not from this shader library" );
        }
    }
    return( 0 );
}

const Binding* CShaderLibrary::getBindings( const BindingGroup& bindingGroup ) const
{
    if( bindingGroup.valid() )
    {   //  the binding group exists
        const CBindingGroup& concrete = *static_cast< const CBindingGroup* >( bindingGroup.naked() );
        if( concrete.getLibrary() == this )
        {   //  the binding group was created by this library
            uint32_t count = concrete.getCount();
            if( count == m_data.resourceItemCount )
            {
                const Binding* bindings = concrete.getBindings();
                if( bindings )
                {
                    return( bindings );
                }
            }
            ASSERT_FAIL( "Binding group appears to be corrupted" );
        }
        else
        {
            ASSERT_FAIL( "Binding group not from this shader library" );
        }
    }
    return( NULL );
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

