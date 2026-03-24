
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   shaders_api.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Public shaders API.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Manifest:
////
////        class       BindingStream       :   smart pointer class to bind a ResourceStream to a Shader or BindingGroup
////        class       BindingOutput       :   smart pointer class to bind a ResourceOutput to a Shader or BindingGroup
////        class       BindingConsts       :   smart pointer class to bind a ResourceConsts to a Shader or BindingGroup
////        class       BindingSRV          :   smart pointer class to bind a ResourceSRV to a Shader or BindingGroup
////        class       BindingUAV          :   smart pointer class to bind a ResourceUAV to a Shader or BindingGroup
////        class       BindingRTV          :   smart pointer class to bind a ResourceRTV to a Shader or BindingGroup
////        class       BindingDSV          :   smart pointer class to bind a ResourceDSV to a Shader or BindingGroup
////        class       BindingSampler      :   smart pointer class to bind a ResourceSampler to a Shader or BindingGroup
////        class       BindingRasterState  :   smart pointer class to bind a ResourceRasterState to a Shader or BindingGroup
////        class       BindingDepthState   :   smart pointer class to bind a ResourceDepthState to a Shader or BindingGroup
////        class       BindingBlendState   :   smart pointer class to bind a ResourceBlendState to a Shader or BindingGroup
////        class       BindingGroup        :   smart pointer class to manage multiple bindings
////        class       Shader              :   smart pointer class to manage a shader
////        class       ShaderLibrary       :   smart pointer class to manage a shader library
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Notes:
////
////        Shader::clone( shader, empty )
////            creates a copy of this shader
////            if empty is true, then the copy will have no resources bound to it
////
////        Shader::bind( binding )
////            binds a specific resource to the shader
////
////        Shader::unbind( binding )
////            unbinds a specific resource from the shader
////
////        Shader::update( binding )
////            updates the staging of a specific resource bound to the shader
////
////        Shader::bind( bindingGroup )
////            binds any resources from the binding group which match the shader
////
////        Shader::unbind( bindingGroup )
////            unbinds any resources from the binding group which are currently bound to the shader
////
////        Shader::unbind()
////            unbinds all resources from the shader
////
////        Shader::update()
////            updates the staging of all resources bound to the shader
////            will fail if any resource is not bound or not ready
////
////        Shader::submit()
////            submits the current staging data to the hardware/SDK
////
////        Shader::execute( command )
////            executes a command
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

#ifndef	__SHADERS_API_INCLUDED__
#define	__SHADERS_API_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../private/shader_interfaces.h"
#include "resources_api.h"

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
////    Resource type specific user resource binding classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( BindingStream, IBinding, Binding )
    inline bool             clone( BindingStream& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceStream& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingOutput, IBinding, Binding )
    inline bool             clone( BindingOutput& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceOutput& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingConsts, IBinding, Binding )
    inline bool             clone( BindingConsts& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceConsts& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingSRV, IBinding, Binding )
    inline bool             clone( BindingSRV& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceSRV& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingUAV, IBinding, Binding )
    inline bool             bind( BindingUAV& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceUAV& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingRTV, IBinding, Binding )
    inline bool             clone( BindingRTV& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceRTV& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingDSV, IBinding, Binding )
    inline bool             clone( BindingDSV& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceDSV& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingSampler, IBinding, Binding )
    inline bool             clone( BindingSampler& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceSampler& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingRasterState, IBinding, Binding )
    inline bool             clone( BindingRasterState& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceRasterState& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return(Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingDepthState, IBinding, Binding )
    inline bool             clone( BindingDepthState& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceDepthState& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return( Binding::unbind()); };
SMART_API_END();

SMART_API_BEGIN( BindingBlendState, IBinding, Binding )
    inline bool             clone( BindingBlendState& bindingClone ) const { return( Binding::clone( bindingClone ) ); };
    inline bool             bind( const ResourceBlendState& resource ) const { return( Binding::bind( resource ) ); };
    inline bool             unbind() const { return( Binding::unbind()); };
SMART_API_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    User binding group class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( BindingGroup, IBindingGroup, SharedPtr )
    inline bool         clone( BindingGroup& bindingGroupClone ) const { return( usable() ? iptr()->clone( bindingGroupClone ) : false ); };
    inline bool         bind( const Binding& binding ) const { return( usable() ? iptr()->bind( binding ) : false ); };
    inline bool         unbind( const Binding& binding ) const { return( usable() ? iptr()->unbind( binding ) : false ); };
    inline bool         unbind() const { return( usable() ? iptr()->unbind() : false); };
SMART_API_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    User shader class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( Shader, IShader, SharedPtr )
    inline bool         clone( Shader& shaderClone, const bool empty = false ) const { return( usable() ? iptr()->clone( shaderClone, empty ) : false ); };
    inline bool         bind( const Binding& binding ) const { return( usable() ? iptr()->bind( binding ) : false ); };
    inline bool         unbind( const Binding& binding ) const { return( usable() ? iptr()->unbind( binding ) : false ); };
    inline bool         update( const Binding& binding ) const { return( usable() ? iptr()->update( binding ) : false ); };
    inline bool         bind( const BindingGroup& bindingGroup ) const { return( usable() ? iptr()->bind( bindingGroup ) : false); };
    inline bool         unbind( const BindingGroup& bindingGroup ) const { return( usable() ? iptr()->unbind( bindingGroup ) : false); };
    inline bool         unbind() const { return( usable() ? iptr()->unbind() : false ); };
    inline bool         update() const { return( usable() ? iptr()->update() : false ); };
    inline bool         submit() const { return( usable() ? iptr()->submit() : false ); };
    inline bool         execute( const ResourceCommand& command ) const { return( usable() ? iptr()->execute( command ) : false ); };
SMART_API_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    User shader library class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( ShaderLibrary, IShaderLibrary, SharedPtr )
    inline bool     create( const StringItem& name, Shader& shader ) const { return( usable() ? iptr()->create( name, shader ) : false ); };
    inline bool     create( const StringItem& name, BindingStream& bindingStream ) const { return( usable() ? iptr()->create( name, bindingStream ) : false ); };
    inline bool     create( const StringItem& name, BindingOutput& bindingOutput ) const { return( usable() ? iptr()->create( name, bindingOutput ) : false ); };
    inline bool     create( const StringItem& name, BindingConsts& bindingConsts ) const { return( usable() ? iptr()->create( name, bindingConsts ) : false ); };
    inline bool     create( const StringItem& name, BindingSRV& bindingSRV ) const { return( usable() ? iptr()->create( name, bindingSRV ) : false ); };
    inline bool     create( const StringItem& name, BindingUAV& bindingUAV ) const { return( usable() ? iptr()->create( name, bindingUAV ) : false ); };
    inline bool     create( const StringItem& name, BindingRTV& bindingRTV ) const { return( usable() ? iptr()->create( name, bindingRTV ) : false ); };
    inline bool     create( const StringItem& name, BindingDSV& bindingDSV ) const { return( usable() ? iptr()->create( name, bindingDSV ) : false ); };
    inline bool     create( const StringItem& name, BindingSampler& bindingSampler ) const { return( usable() ? iptr()->create( name, bindingSampler ) : false ); };
    inline bool     create( const StringItem& name, BindingRasterState& bindingRasterState ) const { return( usable() ? iptr()->create( name, bindingRasterState ) : false ); };
    inline bool     create( const StringItem& name, BindingDepthState& bindingDepthState ) const { return( usable() ? iptr()->create( name, bindingDepthState ) : false ); };
    inline bool     create( const StringItem& name, BindingBlendState& bindingBlendState ) const { return( usable() ? iptr()->create( name, bindingBlendState ) : false ); };
    inline bool     create( BindingGroup& bindingGroup ) const { return( usable() ? iptr()->create( bindingGroup ) : false ); };
SMART_API_END();

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

#endif	//	#ifndef	__SHADERS_API_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
