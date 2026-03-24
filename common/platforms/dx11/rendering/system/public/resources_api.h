
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   resources_API.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Public resources API.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Manifest:
////
////        class       ResourceLibrary         :   smart pointer class for handling a resource library
////        class       ResourceRegistry        :   smart pointer class for handling a resource registry
////        class       ResourceStream          :   smart pointer class for configuring and binding stream resources
////        class       ResourceOutput          :   smart pointer class for configuring and binding output resources
////        class       ResourceConsts          :   smart pointer class for configuring and binding buffer resources
////        class       ResourceSRV             :   smart pointer class for configuring and binding SRV resources
////        class       ResourceUAV             :   smart pointer class for configuring and binding UAV resources
////        class       ResourceRTV             :   smart pointer class for configuring and binding RTV resources
////        class       ResourceDSV             :   smart pointer class for configuring and binding DSV resources
////        class       ResourceSampler         :   smart pointer class for configuring and binding sampler state resources
////        class       ResourceRasterState     :   smart pointer class for configuring and binding raster state resources
////        class       ResourceDepthState      :   smart pointer class for configuring and binding depth-stencil state resources
////        class       ResourceBlendState      :   smart pointer class for configuring and binding blend state resources
////        class       ResourceParams          :   smart pointer class for configuring and binding parameter resources
////        class       ResourceIndices         :   smart pointer class for configuring and binding indexing resources
////        class       ResourceCommand         :   smart pointer class for configuring and binding command resources
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

#ifndef	__RESOURCES_API_INCLUDED__
#define	__RESOURCES_API_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../private/resource_interfaces.h"
#include "state_descs.h"
#include "commands.h"

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
////    User resource library class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( ResourceLibrary, IResourceLibrary, SharedPtr )
    inline int                          getSamplerCount() const { return( usable() ? iptr()->getSamplerCount() : 0 ); };
    inline int                          getRasterStateCount() const { return( usable() ? iptr()->getRasterStateCount() : 0 ); };
    inline int                          getDepthStateCount() const { return( usable() ? iptr()->getDepthStateCount() : 0 ); };
    inline int                          getBlendStateCount() const { return( usable() ? iptr()->getBlendStateCount() : 0 ); };
    inline int                          findSamplerIndex( const StringItem& name ) const { return( usable() ? iptr()->findSamplerIndex( name ) : -1 ); };
    inline int                          findRasterStateIndex( const StringItem& name ) const { return( usable() ? iptr()->findRasterStateIndex( name ) : -1 ); };
    inline int                          findDepthStateIndex( const StringItem& name ) const { return( usable() ? iptr()->findDepthStateIndex( name ) : -1 ); };
    inline int                          findBlendStateIndex( const StringItem& name ) const { return( usable() ? iptr()->findBlendStateIndex( name ) : -1 ); };
    inline bool                         getSamplerName( const int index, StringItem& name ) const { return( usable() ? iptr()->getSamplerName( index, name ) : false ); };
    inline bool                         getRasterStateName( const int index, StringItem& name ) const { return( usable() ? iptr()->getRasterStateName( index, name ) : false ); };
    inline bool                         getDepthStateName( const int index, StringItem& name ) const { return( usable() ? iptr()->getDepthStateName( index, name ) : false ); };
    inline bool                         getBlendStateName( const int index, StringItem& name ) const { return( usable() ? iptr()->getBlendStateName( index, name ) : false ); };
    inline const CSamplerDesc*          getSamplerDesc( const int index ) const { return( usable() ? iptr()->getSamplerDesc( index ) : NULL ); };
    inline const CRasterStateDesc*      getRasterStateDesc( const int index ) const { return( usable() ? iptr()->getRasterStateDesc( index ) : NULL ); };
    inline const CDepthStateDesc*       getDepthStateDesc( const int index ) const { return( usable() ? iptr()->getDepthStateDesc( index ) : NULL ); };
    inline const CMultiBlendStateDesc*  getBlendStateDesc( const int index ) const { return( usable() ? iptr()->getBlendStateDesc( index ) : NULL ); };
SMART_API_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    User resource registry class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( ResourceRegistry, IResourceRegistry, SharedPtr )
    inline bool                         add( const StringItem& name, const ResourceStream& resourceStream, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceStream, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceOutput& resourceOutput, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceOutput, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceConsts& resourceConsts, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceConsts, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceSRV& resourceSRV, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceSRV, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceUAV& resourceUAV, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceUAV, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceRTV& resourceRTV, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceRTV, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceDSV& resourceDSV, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceDSV, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceSampler& resourceSampler, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceSampler, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceRasterState& resourceRasterState, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceRasterState, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceDepthState& resourceDepthState, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceDepthState, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceBlendState& resourceBlendState, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceBlendState, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceIndices& resourceIndices, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceIndices, allowReplace ) : false ); };
    inline bool                         add( const StringItem& name, const ResourceCommand& resourceCommand, const bool allowReplace = false ) const { return( usable() ? iptr()->add( name, resourceCommand, allowReplace ) : false ); };
    inline bool                         find( const StringItem& name, ResourceStream& resourceStream ) const { return( usable() ? iptr()->find( name, resourceStream ) : false ); };
    inline bool                         find( const StringItem& name, ResourceOutput& resourceOutput ) const { return( usable() ? iptr()->find( name, resourceOutput ) : false ); };
    inline bool                         find( const StringItem& name, ResourceConsts& resourceConsts ) const { return( usable() ? iptr()->find( name, resourceConsts ) : false ); };
    inline bool                         find( const StringItem& name, ResourceSRV& resourceSRV ) const { return( usable() ? iptr()->find( name, resourceSRV ) : false ); };
    inline bool                         find( const StringItem& name, ResourceUAV& resourceUAV ) const { return( usable() ? iptr()->find( name, resourceUAV ) : false ); };
    inline bool                         find( const StringItem& name, ResourceRTV& resourceRTV ) const { return( usable() ? iptr()->find( name, resourceRTV ) : false ); };
    inline bool                         find( const StringItem& name, ResourceDSV& resourceDSV ) const { return( usable() ? iptr()->find( name, resourceDSV ) : false ); };
    inline bool                         find( const StringItem& name, ResourceSampler& resourceSampler ) const { return( usable() ? iptr()->find( name, resourceSampler ) : false ); };
    inline bool                         find( const StringItem& name, ResourceRasterState& resourceRasterState ) const { return( usable() ? iptr()->find( name, resourceRasterState ) : false ); };
    inline bool                         find( const StringItem& name, ResourceDepthState& resourceDepthState ) const { return( usable() ? iptr()->find( name, resourceDepthState ) : false ); };
    inline bool                         find( const StringItem& name, ResourceBlendState& resourceBlendState ) const { return( usable() ? iptr()->find( name, resourceBlendState ) : false ); };
    inline bool                         find( const StringItem& name, ResourceIndices& resourceIndices ) const { return( usable() ? iptr()->find( name, resourceIndices ) : false ); };
    inline bool                         find( const StringItem& name, ResourceCommand& resourceCommand ) const { return( usable() ? iptr()->find( name, resourceCommand ) : false ); };
SMART_API_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource type specific user classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( ResourceStream, IResourceLinear, Resource )
    inline bool                         clone( ResourceStream& resourceClone ) const { return( usable() ? iptr()->clone( *reinterpret_cast< ResourceLinear* >( &resourceClone ) ) : false ); };
    inline void                         setStream( void* const buffer ) const { if( usable() ) iptr()->setBuffer( buffer ); };
    inline void                         setOffset( const UINT offset ) const { if( usable() ) iptr()->setOffset( offset ); };
    inline void                         setStride( const UINT stride ) const { if( usable() ) iptr()->setStride( stride ); };
    inline UINT                         getOffset() const { return( usable() ? iptr()->getOffset() : 0 ); };
    inline UINT                         getStride() const { return( usable() ? iptr()->getStride() : 0 ); };
SMART_API_END();

SMART_API_BEGIN( ResourceOutput, IResourceLinear, Resource )
    inline bool                         clone( ResourceOutput& resourceClone ) const { return( usable() ? iptr()->clone( *reinterpret_cast< ResourceLinear* >( &resourceClone ) ) : false ); };
    inline void                         setOutput( void* const buffer ) const { if( usable() ) iptr()->setBuffer( buffer ); };
    inline void                         setOffset( const UINT offset ) const { if( usable() ) iptr()->setOffset( offset ); };
    inline void                         setStride( const UINT stride ) const { if( usable() ) iptr()->setStride( stride ); };
    inline UINT                         getOffset() const { return( usable() ? iptr()->getOffset() : 0 ); };
    inline UINT                         getStride() const { return( usable() ? iptr()->getStride() : 0 ); };
SMART_API_END();

SMART_API_BEGIN( ResourceConsts, IResourceLinear, Resource )
    inline bool                         clone( ResourceConsts& resourceClone ) const { return( usable() ? iptr()->clone( *reinterpret_cast< ResourceLinear* >( &resourceClone ) ) : false ); };
    inline void                         setBuffer( void* const buffer ) const { if( usable() ) iptr()->setBuffer( buffer ); };
    inline void                         setOffset( const UINT offset ) const { if( usable() ) iptr()->setOffset( offset ); };
    inline void                         setStride( const UINT stride ) const { if( usable() ) iptr()->setStride( stride ); };
    inline UINT                         getOffset() const { return( usable() ? iptr()->getOffset() : 0 ); };
    inline UINT                         getStride() const { return( usable() ? iptr()->getStride() : 0 ); };
SMART_API_END();

SMART_API_BEGIN( ResourceSRV, IResourceSRV, Resource )
    inline bool                         clone( ResourceSRV& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline void                         setSRV( void* const srv ) const { if( usable() ) iptr()->setSRV( srv ); };
SMART_API_END();

SMART_API_BEGIN( ResourceUAV, IResourceUAV, Resource )
    inline bool                         clone( ResourceUAV& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline void                         setUAV( void* const uav ) const { if( usable() ) iptr()->setUAV( uav ); };
    inline void                         setInitialCount( const UINT initialCount ) const { if( usable() ) iptr()->setInitialCount( initialCount ); };
    inline UINT                         getInitialCount() const { return( usable() ? iptr()->getInitialCount() : 0 ); };
SMART_API_END();

SMART_API_BEGIN( ResourceRTV, IResourceRTV, Resource )
    inline bool                         clone( ResourceRTV& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline void                         setRTV( void* const rtv ) const { if( usable() ) iptr()->setRTV( rtv ); };
    inline void                         setViewport( const FBOX& viewport ) const { if( usable() ) iptr()->setViewport( viewport ); };
    inline void                         setScissoring( const SRECT& scissoring ) const { if( usable() ) iptr()->setScissoring( scissoring ); };
    inline const FBOX*                  getViewport() const { return( usable() ? &iptr()->getViewport() : NULL ); };
    inline const SRECT*                 getScissoring() const { return( usable() ? &iptr()->getScissoring() : NULL ); };
SMART_API_END();

SMART_API_BEGIN( ResourceDSV, IResourceDSV, Resource )
    inline bool                         clone( ResourceDSV& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline void                         setDSV( void* const dsv ) const { if( usable() ) iptr()->setDSV( dsv ); };
    inline void                         setViewport( const FBOX& viewport ) const { if( usable() ) iptr()->setViewport( viewport ); };
    inline void                         setScissoring( const SRECT& scissoring ) const { if( usable() ) iptr()->setScissoring( scissoring ); };
    inline const FBOX*                  getViewport() const { return( usable() ? &iptr()->getViewport() : NULL ); };
    inline const SRECT*                 getScissoring() const { return( usable() ? &iptr()->getScissoring() : NULL ); };
SMART_API_END();

SMART_API_BEGIN( ResourceSampler, IResourceSampler, Resource )
    inline bool                         clone( ResourceSampler& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline const CSamplerDesc*          desc() const { return( usable() ? &iptr()->desc() : NULL ); };
SMART_API_END();

SMART_API_BEGIN( ResourceRasterState, IResourceRasterState, Resource )
    inline bool                         clone( ResourceRasterState& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline const CRasterStateDesc*      desc() const { return( usable() ? &iptr()->desc() : NULL ); };
SMART_API_END();

SMART_API_BEGIN( ResourceDepthState, IResourceDepthState, Resource )
    inline bool                         clone( ResourceDepthState& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline const CDepthStateDesc*       desc() const { return( usable() ? &iptr()->desc() : NULL ); };
    inline void                         setStencilRef( const UINT stencilRef ) const { if( usable() ) iptr()->setStencilRef( stencilRef ); };
    inline UINT                         getStencilRef() const { return( usable() ? iptr()->getStencilRef() : 0 ); };
SMART_API_END();

SMART_API_BEGIN( ResourceBlendState, IResourceBlendState, Resource )
    inline bool                         clone( ResourceBlendState& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline const CMultiBlendStateDesc*  desc() const { return( usable() ? &iptr()->desc() : NULL ); };
    inline void                         setSampleMask( const UINT sampleMask ) const { if( usable() ) iptr()->setSampleMask( sampleMask ); };
    inline void                         setBlendFactor( const FVEC4& blendFactor ) const { if( usable() ) iptr()->setBlendFactor( blendFactor ); };
    inline UINT                         getSampleMask() const { return( usable() ? iptr()->getSampleMask() : 0xffffffff ); };
    inline const FVEC4*                 getBlendFactor() const { return( usable() ? &iptr()->getBlendFactor() : NULL ); };
SMART_API_END();

SMART_API_BEGIN( ResourceParams, IResourceLinear, Resource )
    inline bool                         clone( ResourceParams& resourceClone ) const { return( usable() ? iptr()->clone( *reinterpret_cast< ResourceLinear* >( &resourceClone ) ) : false ); };
    inline void                         setParams( void* const buffer ) const { if( usable() ) iptr()->setBuffer( buffer ); };
    inline void                         setOffset( const UINT offset ) const { if( usable() ) iptr()->setOffset( offset ); };
    inline void                         setStride( const UINT stride ) const { if( usable() ) iptr()->setStride( stride ); };
    inline UINT                         getOffset() const { return( usable() ? iptr()->getOffset() : 0 ); };
    inline UINT                         getStride() const { return( usable() ? iptr()->getStride() : 0 ); };
SMART_API_END();

SMART_API_BEGIN( ResourceIndices, IResourceLinear, Resource )
    inline bool                         clone( ResourceIndices& resourceClone ) const { return( usable() ? iptr()->clone( *reinterpret_cast< ResourceLinear* >( &resourceClone ) ) : false ); };
    inline void                         setIndices( void* const buffer ) const { if( usable() ) iptr()->setBuffer( buffer ); };
    inline void                         setOffset( const UINT offset ) const { if( usable() ) iptr()->setOffset( offset ); };
    inline void                         setStride( const UINT stride ) const { if( usable() ) iptr()->setStride( stride ); };
    inline UINT                         getOffset() const { return( usable() ? iptr()->getOffset() : 0 ); };
    inline UINT                         getStride() const { return( usable() ? iptr()->getStride() : 0 ); };
SMART_API_END();

SMART_API_BEGIN( ResourceCommand, IResourceCommand, Resource )
    inline bool                         clone( ResourceCommand& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline void                         setNull() const { if( usable() ) iptr()->setNull(); };
    inline void                         setPrim( const PrimDesc& primDesc ) const { if( usable() ) iptr()->setPrim( primDesc ); };
    inline void                         setDraw() const { if( usable() ) iptr()->setDraw(); };
    inline void                         setDraw( const DrawingParams& drawing ) const { if( usable() ) iptr()->setDraw( drawing ); };
    inline void                         setDraw( const DrawingParams& drawing, const InstancingParams& instancing ) const { if( usable() ) iptr()->setDraw( drawing, instancing ); };
    inline void                         setDraw( const IndexedParams& indexed, const ResourceIndices& indices ) const { if( usable() ) iptr()->setDraw( indexed, indices ); };
    inline void                         setDraw( const IndexedParams& indexed, const ResourceIndices& indices, const InstancingParams& instancing ) const { if( usable() ) iptr()->setDraw( indexed, indices, instancing ); };
    inline void                         setDraw( const ResourceParams& indirect ) const { if( usable() ) iptr()->setDraw( indirect ); };
    inline void                         setDraw( const ResourceParams& indirect, const ResourceIndices& indices ) const { if( usable() ) iptr()->setDraw( indirect, indices ); };
    inline void                         setDispatch( const DispatchParams& dispatch ) const { if( usable() ) iptr()->setDispatch( dispatch ); };
    inline void                         setDispatch( const ResourceParams& indirect ) const { if( usable() ) iptr()->setDispatch( indirect ); };
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

#endif	//	#ifndef	__RESOURCES_API_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
