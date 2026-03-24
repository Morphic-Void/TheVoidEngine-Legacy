
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   shaders.h
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
////    include guard begin
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef	__PRIVATE_SHADERS_INCLUDED__
#define	__PRIVATE_SHADERS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    platform includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <dxgi.h>
#include <d3d11.h>
#include <XInput.h>
#include <DirectXColors.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../public/shaders_api.h"
#include "shader_structs.h"
#include "shader_binary.h"
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
////    forward declarations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSystemFactory;
class CShaderLibrary;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Binding class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBinding : public IBinding
{
public:
    inline                          CBinding() : m_refs( 0 ), m_handle( 0 ), m_library( NULL ), m_resource() {};
    inline                          ~CBinding() { destroy(); };
    inline bool                     valid() const { return( m_refs ? true : false ); };
    bool                            create( CShaderLibrary& shaderLibrary, const uint32_t handle );
    void                            destroy();
    inline uint32_t                 getHandle() const { return( m_handle ); };
    inline CShaderLibrary*          getLibrary() const { return( m_library ); };
    inline const Resource&          getResource() const { return( m_resource ); };
public:
    virtual void                    addRef();
    virtual void                    release();
    virtual bool                    clone( Binding& bindingClone ) const;
    virtual bool                    bind( const Resource& resource );
    virtual bool                    unbind();
protected:
    uint32_t                        m_refs;
    uint32_t                        m_handle;
    CShaderLibrary*                 m_library;
    Resource                        m_resource;
private:
    inline                          CBinding( const CBinding& ) {};
    inline void                     operator=( const CBinding& ) const {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Binding pool class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CBindings = TSlot< CBinding >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Binding group class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBindingGroup : public IBindingGroup
{
public:
    inline                          CBindingGroup() : m_refs( 0 ), m_count( 0 ), m_library( NULL ), m_bindings( NULL ) {};
    inline                          ~CBindingGroup() { destroy(); };
    inline bool                     valid() const { return( m_refs ? true : false ); };
    bool                            create( CShaderLibrary& shaderLibrary, const uint32_t count, Binding* const bindings );
    void                            destroy();
    inline uint32_t                 getCount() const { return( m_count ); };
    inline CShaderLibrary*          getLibrary() const { return( m_library ); };
    inline const Binding*           getBindings() const { return( m_bindings ); };
public:
    virtual void                    addRef();
    virtual void                    release();
    virtual bool                    clone( BindingGroup& bindingGroupClone ) const;
    virtual bool                    bind( const Binding& binding );
    virtual bool                    unbind( const Binding& binding );
    virtual bool                    unbind();
protected:
    uint32_t                        m_refs;
    uint32_t                        m_count;
    CShaderLibrary*                 m_library;
    Binding*                        m_bindings;
private:
    inline                          CBindingGroup( const CBindingGroup& ) {};
    inline void                     operator=( const CBindingGroup& ) const {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Binding group pool class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CBindingGroups = TSlot< CBindingGroup >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Shader class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CShader : public IShader
{
public:
    inline                          CShader() : m_refs( 0 ), m_handle( 0 ), m_library( NULL ), m_bindings( NULL ), m_staging( NULL ) {};
    inline                          ~CShader() { destroy(); };
    inline bool                     valid() const { return( ( m_refs && m_library ) ? true : false ); };
    bool                            create( CShaderLibrary& library, const uint32_t handle, Binding* const bindings, ShaderStaging* const staging );
    void                            destroy();
    inline uint32_t                 getHandle() const { return( m_handle ); };
    inline CShaderLibrary*          getLibrary() const { return( m_library ); };
    inline const Binding*           getBindings() const { return( m_bindings ); };
    inline const ShaderStaging*     getStaging() const { return( m_staging ); };
public:
    virtual void                    addRef();
    virtual void                    release();
    virtual bool                    clone( Shader& shaderClone, const bool empty = false ) const;
    virtual bool                    bind( const Binding& binding );
    virtual bool                    unbind( const Binding& binding );
    virtual bool                    update( const Binding& binding );
    virtual bool                    bind( const BindingGroup& bindingGroup );
    virtual bool                    unbind( const BindingGroup& bindingGroup );
    virtual bool                    unbind();
    virtual bool                    update();
    virtual bool                    submit() const;
    virtual bool                    execute( const ResourceCommand& command ) const;
protected:
    uint32_t                        m_refs;
    uint32_t                        m_handle;
    CShaderLibrary*                 m_library;
    Binding*                        m_bindings;
    ShaderStaging*                  m_staging;
private:
    inline                          CShader( const CShader& ) {};
    inline void                     operator=( const CShader& ) const {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Shader pool class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CShaders = TSlot< CShader >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Shader library class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CShaderLibrary : public IShaderLibrary
{
public:
    inline                          CShaderLibrary() { zero(); };
    inline                          ~CShaderLibrary() { destroy(); };
    inline bool                     valid() const { return( m_refs ? true : false ); };
    bool                            create( CSystemFactory& factory, RefBlob& blob, ShaderLibrary& shaderLibrary );
    void                            destroy();
public:
    virtual void                    addRef();
    virtual void                    release();
    virtual bool                    create( const StringItem& name, Shader& shader );
    virtual bool                    create( const StringItem& name, BindingStream& bindingStream );
    virtual bool                    create( const StringItem& name, BindingOutput& bindingOutput );
    virtual bool                    create( const StringItem& name, BindingConsts& bindingConsts );
    virtual bool                    create( const StringItem& name, BindingSRV& bindingSRV ); 
    virtual bool                    create( const StringItem& name, BindingUAV& bindingUAV );
    virtual bool                    create( const StringItem& name, BindingRTV& bindingRTV );
    virtual bool                    create( const StringItem& name, BindingDSV& bindingDSV );
    virtual bool                    create( const StringItem& name, BindingSampler& bindingSampler );
    virtual bool                    create( const StringItem& name, BindingRasterState& bindingRasterState );
    virtual bool                    create( const StringItem& name, BindingDepthState& bindingDepthState );
    virtual bool                    create( const StringItem& name, BindingBlendState& bindingBlendState );
    virtual bool                    create( BindingGroup& bindingGroup );
public:
    bool                            clone( const CBindingGroup& bindingGroup, BindingGroup& bindingGroupClone );
    void                            destroy( CBindingGroup& bindingGroup );
    bool                            create( const StringItem& name, Binding& binding, const uint32_t resourceType );
    bool                            clone( const CBinding& binding, Binding& bindingClone );
    void                            destroy( CBinding& binding );
    bool                            create( const uint32_t shaderHandle, Shader& shader );
    bool                            clone( const CShader& shader, Shader& shaderClone );
    void                            destroy( CShader& shader );
    bool                            bind( Binding* const bindings, const Binding& binding );
    bool                            unbind( Binding* const bindings, const Binding& binding );
    bool                            unbind( Binding* const bindings );
    bool                            bind( const uint32_t shaderHandle, Binding* const bindings, const Binding& binding );
    bool                            unbind( const uint32_t shaderHandle, Binding* const bindings, const Binding& binding );
    bool                            bind( const uint32_t shaderHandle, Binding* const bindings, const BindingGroup& bindingGroup );
    bool                            unbind( const uint32_t shaderHandle, Binding* const bindings, const BindingGroup& bindingGroup );
    bool                            unbind( const uint32_t shaderHandle, Binding* const bindings );
    bool                            update( const uint32_t shaderHandle, const Binding* const bindings, ShaderStaging& staging, const Binding& binding ) const;
    bool                            update( const uint32_t shaderHandle, const Binding* const bindings, ShaderStaging& staging ) const;
    bool                            submit( const uint32_t shaderHandle, const ShaderStaging& staging ) const;
    bool                            execute( const ResourceCommand& command ) const;
    void                            clearState() const;
protected:
    bool                            createInterfaces();
    bool                            createInterfaces( D3D11_INPUT_ELEMENT_DESC* const inputs, D3D11_SO_DECLARATION_ENTRY* const outputs );
    IUnknown*                       createDeviceShader( const binary::BlobDesc& shaderBlob, const uint shaderStage ) const;
    ShaderStaging*                  createStaging( const uint32_t stagingCount, const bool renderable );
    bool                            setTopology( const PrimDesc& primDesc ) const;
    void                            mangle();
    void                            unmangle();
    inline const uint8_t*           getName( const uint32_t nameHandle ) const { return( m_data.stringTable->getString( nameHandle ) ); };
    inline const uint8_t*           getName( const uint32_t nameHandle, uint& nameLength ) const { return( m_data.stringTable->getString( nameHandle, nameLength ) ); };
    inline int                      findString( const StringItem& item ) const { uint length = 0; const uint8_t* string = item.getString( length ); return( m_data.stringTable->findString( string, length ) ); };
    int                             findShader( const StringItem& name ) const;
    int                             findResource( const StringItem& name, const uint32_t resourceType ) const;
    int                             findInShader( const uint32_t shaderHandle, const uint32_t resourceItemIndex ) const;
    const void*                     getDesc( const Binding& binding ) const;
    uint32_t                        getHandle( const Binding& binding ) const;
    const Binding*                  getBindings( const BindingGroup& bindingGroup ) const;
protected:
    uint32_t                        m_refs;
    uint32_t                        m_reserved;
    RefBlob*                        m_blob;
    CSystemFactory*                 m_factory;
	ID3D11Device*                   m_device;
	ID3D11DeviceContext*            m_context;
    IUnknown**                      m_interfaces;                           //  interface pointer array for resources created by the library (Shaders, InputLayouts and StreamOutputs)
    CBindingGroups                  m_bindingGroups;
    CBindings                       m_bindings;
    CShaders                        m_shaders;
    ShaderLibraryData               m_data;
    static const uint32_t           s_magic = MAKE4CC( 'i', 's', 'S', 'L' );
    static const uint32_t           s_platform = MAKE4CC( 'D', 'X', '1', '1' );
    static const DXGI_FORMAT        s_EELEMENT2DXGI[];
private:
    void                            zero();
    inline                          CShaderLibrary( const CShaderLibrary& ) {};
    inline void                     operator=( const CShaderLibrary& ) const {};
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

#endif  //  #ifndef __PRIVATE_SHADERS_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
