
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   resource_interfaces.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Private resource interfaces.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Manifest:
////
////        INTERFACE   IResourceLibrary        :   interface to the private resource library implementation
////        INTERFACE   IResourceRegistry       :   interface to the private resource registry implementation
////        INTERFACE   IResource               :   interface to the private resource implementation
////        INTERFACE   IResourceLinear         :   interface to the private linear resource implementation
////        INTERFACE   IResourceSRV            :   interface to the private SRV resource implementation
////        INTERFACE   IResourceUAV            :   interface to the private UAV resource implementation
////        INTERFACE   IResourceRTV            :   interface to the private RTV resource implementation
////        INTERFACE   IResourceDSV            :   interface to the private DSV resource implementation
////        INTERFACE   IResourceSampler        :   interface to the private sampler state resource implementation
////        INTERFACE   IResourceRasterState    :   interface to the private raster state resource implementation
////        INTERFACE   IResourceDepthState     :   interface to the private depth-stencil state resource implementation
////        INTERFACE   IResourceBlendState     :   interface to the private blend state resource implementation
////        INTERFACE   IResourceCommand        :   interface to the private command resource implementation
////        class       Resource                :   smart pointer base class for binding resources
////        class       ResourceLinear          :   smart pointer linear alias class for Params, Consts, Stream, Output and Indices
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

#ifndef	__PRIVATE_RESOURCE_INTERFACES_INCLUDED__
#define	__PRIVATE_RESOURCE_INTERFACES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/memory/shared_ptr.h"
#include "libs/strings/string_item.h"

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

class CSamplerDesc;
class CRasterStateDesc;
class CDepthStateDesc;
class CMultiBlendStateDesc;
class ResourceLinear;
class ResourceStream;
class ResourceOutput;
class ResourceConsts;
class ResourceSRV;
class ResourceUAV;
class ResourceRTV;
class ResourceDSV;
class ResourceSampler;
class ResourceRasterState;
class ResourceDepthState;
class ResourceBlendState;
class ResourceParams;
class ResourceIndices;
class ResourceCommand;
struct PrimDesc;
struct DispatchParams;
struct DrawingParams;
struct IndexedParams;
struct InstancingParams;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource library interface
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INTERFACE_BEGIN_INHERIT( IResourceLibrary, IShared )
    virtual int                         getSamplerCount() const = 0;
    virtual int                         getRasterStateCount() const = 0;
    virtual int                         getDepthStateCount() const = 0;
    virtual int                         getBlendStateCount() const = 0;
    virtual int                         findSamplerIndex( const StringItem& name ) const = 0;
    virtual int                         findRasterStateIndex( const StringItem& name ) const = 0;
    virtual int                         findDepthStateIndex( const StringItem& name ) const = 0;
    virtual int                         findBlendStateIndex( const StringItem& name ) const = 0;
    virtual bool                        getSamplerName( const int index, StringItem& name ) const = 0;
    virtual bool                        getRasterStateName( const int index, StringItem& name ) const = 0;
    virtual bool                        getDepthStateName( const int index, StringItem& name ) const = 0;
    virtual bool                        getBlendStateName( const int index, StringItem& name ) const = 0;
    virtual const CSamplerDesc*         getSamplerDesc( const int index ) const = 0;
    virtual const CRasterStateDesc*     getRasterStateDesc( const int index ) const = 0;
    virtual const CDepthStateDesc*      getDepthStateDesc( const int index ) const = 0;
    virtual const CMultiBlendStateDesc* getBlendStateDesc( const int index ) const = 0;
INTERFACE_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource registry interface
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INTERFACE_BEGIN_INHERIT( IResourceRegistry, IShared )
    virtual bool                        add( const StringItem& name, const ResourceStream& resourceStream, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceOutput& resourceOutput, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceConsts& resourceConsts, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceSRV& resourceSRV, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceUAV& resourceUAV, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceRTV& resourceRTV, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceDSV& resourceDSV, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceSampler& resourceSampler, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceRasterState& resourceRasterState, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceDepthState& resourceDepthState, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceBlendState& resourceBlendState, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceIndices& resourceIndices, const bool allowReplace = false ) = 0;
    virtual bool                        add( const StringItem& name, const ResourceCommand& resourceCommand, const bool allowReplace = false ) = 0;
    virtual bool                        find( const StringItem& name, ResourceStream& resourceStream ) = 0;
    virtual bool                        find( const StringItem& name, ResourceOutput& resourceOutput ) = 0;
    virtual bool                        find( const StringItem& name, ResourceConsts& resourceConsts ) = 0;
    virtual bool                        find( const StringItem& name, ResourceSRV& resourceSRV ) = 0;
    virtual bool                        find( const StringItem& name, ResourceUAV& resourceUAV ) = 0;
    virtual bool                        find( const StringItem& name, ResourceRTV& resourceRTV ) = 0;
    virtual bool                        find( const StringItem& name, ResourceDSV& resourceDSV ) = 0;
    virtual bool                        find( const StringItem& name, ResourceSampler& resourceSampler ) = 0;
    virtual bool                        find( const StringItem& name, ResourceRasterState& resourceRasterState ) = 0;
    virtual bool                        find( const StringItem& name, ResourceDepthState& resourceDepthState ) = 0;
    virtual bool                        find( const StringItem& name, ResourceBlendState& resourceBlendState ) = 0;
    virtual bool                        find( const StringItem& name, ResourceIndices& resourceIndices ) = 0;
    virtual bool                        find( const StringItem& name, ResourceCommand& resourceCommand ) = 0;
INTERFACE_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource base interface
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INTERFACE_BEGIN_INHERIT( IResource, IShared )
    virtual const void*                 pipe() const = 0;
INTERFACE_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Resource type specific interfaces
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INTERFACE_BEGIN_INHERIT( IResourceLinear, IResource )
    virtual bool                        clone( ResourceLinear& resourceClone ) const = 0;
    virtual void                        setBuffer( void* const buffer ) = 0;
    virtual void                        setOffset( const UINT offset ) = 0;
    virtual void                        setStride( const UINT stride ) = 0;
    virtual UINT                        getOffset() const = 0;
    virtual UINT                        getStride() const = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceSRV, IResource )
    virtual bool                        clone( ResourceSRV& resourceClone ) const = 0;
    virtual void                        setSRV( void* const srv ) = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceUAV, IResource )
    virtual bool                        clone( ResourceUAV& resourceClone ) const = 0;
    virtual void                        setUAV( void* const uav ) = 0;
    virtual void                        setInitialCount( const UINT initialCount ) = 0;
    virtual UINT                        getInitialCount() const = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceRTV, IResource )
    virtual bool                        clone( ResourceRTV& resourceClone ) const = 0;
    virtual void                        setRTV( void* const rtv ) = 0;
    virtual void                        setViewport( const FBOX& viewport ) = 0;
    virtual void                        setScissoring( const SRECT& scissoring ) = 0;
    virtual const FBOX&                 getViewport() const = 0;
    virtual const SRECT&                getScissoring() const = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceDSV, IResource )
    virtual bool                        clone( ResourceDSV& resourceClone ) const = 0;
    virtual void                        setDSV( void* const dsv ) = 0;
    virtual void                        setViewport( const FBOX& viewport ) = 0;
    virtual void                        setScissoring( const SRECT& scissoring ) = 0;
    virtual const FBOX&                 getViewport() const = 0;
    virtual const SRECT&                getScissoring() const = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceSampler, IResource )
    virtual bool                        clone( ResourceSampler& resourceClone ) const = 0;
    virtual const CSamplerDesc&         desc() const = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceRasterState, IResource )
    virtual bool                        clone( ResourceRasterState& resourceClone ) const = 0;
    virtual const CRasterStateDesc&     desc() const = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceDepthState, IResource )
    virtual bool                        clone( ResourceDepthState& resourceClone ) const = 0;
    virtual const CDepthStateDesc&      desc() const = 0;
    virtual void                        setStencilRef( const UINT stencilRef ) = 0;
    virtual UINT                        getStencilRef() const = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceBlendState, IResource )
    virtual bool                        clone( ResourceBlendState& resourceClone ) const = 0;
    virtual const CMultiBlendStateDesc& desc() const = 0;
    virtual void                        setSampleMask( const UINT sampleMask ) = 0;
    virtual void                        setBlendFactor( const FVEC4& blendFactor ) = 0;
    virtual UINT                        getSampleMask() const = 0;
    virtual const FVEC4&                getBlendFactor() const = 0;
INTERFACE_END();

INTERFACE_BEGIN_INHERIT( IResourceCommand, IResource )
    virtual bool                        clone( ResourceCommand& resourceClone ) const = 0;
    virtual void                        setNull() = 0;
    virtual void                        setPrim( const PrimDesc& primDesc ) = 0;
    virtual void                        setDraw() = 0;
    virtual void                        setDraw( const DrawingParams& drawing ) = 0;
    virtual void                        setDraw( const DrawingParams& drawing, const InstancingParams& instancing ) = 0;
    virtual void                        setDraw( const IndexedParams& indexed, const ResourceIndices& indices ) = 0;
    virtual void                        setDraw( const IndexedParams& indexed, const ResourceIndices& indices, const InstancingParams& instancing ) = 0;
    virtual void                        setDraw( const ResourceParams& indirect ) = 0;
    virtual void                        setDraw( const ResourceParams& indirect, const ResourceIndices& indices ) = 0;
    virtual void                        setDispatch( const DispatchParams& dispatch ) = 0;
    virtual void                        setDispatch( const ResourceParams& indirect ) = 0;
INTERFACE_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Smart resource base class
////
////    Note: the pipe() function is for internal engine use only
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( Resource, IResource, SharedPtr )
    inline const void*                  pipe() const { return( usable() ? iptr()->pipe() : nullptr ); };
SMART_API_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Smart resource linear aliasing class
////
////    Note: this aliases ResourceParams, ResourceConsts, ResourceStream, ResourceOutput and ResourceIndices
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SMART_API_BEGIN( ResourceLinear, IResourceLinear, Resource )
    inline bool                         clone( ResourceLinear& resourceClone ) const { return( usable() ? iptr()->clone( resourceClone ) : false ); };
    inline void                         setBuffer( void* const buffer ) const { if( usable() ) iptr()->setBuffer( buffer ); };
    inline void                         setOffset( const UINT offset ) const { if( usable() ) iptr()->setOffset( offset ); };
    inline void                         setStride( const UINT stride ) const { if( usable() ) iptr()->setStride( stride ); };
    inline UINT                         getOffset() const { return( usable() ? iptr()->getOffset() : 0 ); };
    inline UINT                         getStride() const { return( usable() ? iptr()->getStride() : 0 ); };
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

#endif	//	#ifndef	__PRIVATE_RESOURCE_INTERFACES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
