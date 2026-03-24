
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   renderables.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game renderable classes and structures
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

#ifndef	__RENDERABLES_INCLUDED__
#define	__RENDERABLES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/memory/tmanaged.h"
#include "libs/maths/vec2.h"
#include "platforms/dx11/rendering/system/public/complete.h"
#include "platforms/dx11/rendering/utils/vertices.h"
#include "platforms/dx11/system/systemx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin locality namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace locality
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Import namespaces
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace rendering;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Forward declaration of external classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace constructs
{
class GeometryConstruct;
class LayeringConstruct;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Door rendering pass enumeration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EDoorRenderPass
{
    EDoorNullPass = 0,  //  null render pass
    EDoorBiasFace,      //  door biasing, front face only
    EDoorBiasBack,      //  door biasing, back face only
    EDoorBiasBoth,      //  door biasing, both faces
    EDoorViewBack,      //  door sorting, front face transparent, back face open
    EDoorViewFace,      //  door sorting, front face open, back face transparent
    EDoorSortNone,      //  door sorting, both faces closed
    EDoorSortFace,      //  door sorting, front face open, back face closed
    EDoorSortBack,      //  door sorting, front face closed, back face open
    EDoorSortBoth,      //  door sorting, both faces open double sided
    EDoorSortFrom,      //  door sorting, both faces open
    EDoorMaskFace,      //  door masking, front face open, back face closed
    EDoorMaskBack,      //  door masking, front face closed, back face open
    EDoorMaskBoth,      //  door masking, both faces open
    EDoorFromFace,      //  door masking, viewing through front face (special case for doors potentially penetrating the near clip plane)
    EDoorFromBack,      //  door masking, viewing through back face (special case for doors potentially penetrating the near clip plane)
    EDoorFastFace,      //  door masking, cheap version of EDoorFromFace
    EDoorFastBack,      //  door masking, cheap version of EDoorFromBack
    EDoorEdgeFace,      //  door edge frame, front face
    EDoorEdgeBack,      //  door edge frame, back face
    EDoorEdgeBoth,      //  door edge frame, both faces
    EDoorTrimFace,      //  door trim frame, front face
    EDoorTrimBack,      //  door trim frame, back face
    EDoorTrimBoth       //  door trim frame, both faces
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DoorRenderer door rendering class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DoorRenderer
{
public:
    inline                  DoorRenderer() : m_valid( false ) {};
    inline                  ~DoorRenderer() { kill(); };
    static inline void      setFrame( const float radius, const float width, const float depth, const float sill ) { s_frame_radius = radius; s_frame_width = width; s_frame_depth = depth; s_frame_sill = sill; };
    static inline void      setConfig( const float length, const float inset, const float zbias ) { s_length = length; s_inset = inset; s_zbias = zbias; };
    inline bool             valid() const { return( this && m_valid ); };
    bool                    create( const CoreSystem& system, const maths::vec2* const verts, const uint count );
    void                    destroy();
    void                    render( const ResourceConsts& object, const ResourceConsts& viewer, const ResourceConsts& lintel, const UINT stencilRef, const UINT stencilNew, const EDoorRenderPass renderPass ) const;
protected:
    enum EDrawCalls
    {
        EDrawBiasFace1 = 0,     //  door biasing, front face only: draw single sided front face using m_stencilFlip
        EDrawBiasFace2,         //  door biasing, front face only: draw single sided front z-bias cap using m_stencilBias
        EDrawBiasFace3,         //  door biasing, front face only: draw single sided front face using m_stencilKill and the zero depth vertices
        EDrawBiasBack1,         //  door biasing, back face only: draw single sided back face using m_stencilFlip
        EDrawBiasBack2,         //  door biasing, back face only: draw single sided back z-bias cap using m_stencilBias
        EDrawBiasBack3,         //  door biasing, back face only: draw single sided back face using m_stencilKill and the zero depth vertices
        EDrawBiasBoth1,         //  door biasing, both faces: draw double sided face using m_stencilFlip
        EDrawBiasBoth2,         //  door biasing, both faces: draw single sided z-bias box using m_stencilBias
        EDrawBiasBoth3,         //  door biasing, both faces: draw double sided face using m_stencilKill and the zero depth vertices
        EDrawSortNone1,         //  door sorting, both faces closed: draw single sided front and back faces using m_stencilSort, rendering colour and depth
        EDrawSortFace1,         //  door sorting, front face open, back face closed: draw single sided front face using m_stencilSort, rendering depth only
        EDrawSortFace2,         //  door sorting, front face open, back face closed: draw single sided back face using m_stencilSort, rendering colour and depth
        EDrawSortBack1,         //  door sorting, front face closed, back face open: draw single sided back face using m_stencilSort, rendering depth only
        EDrawSortBack2,         //  door sorting, front face closed, back face open: draw single sided front face using m_stencilSort, rendering colour and depth
        EDrawSortBoth1,         //  door sorting, both faces open: draw single sided front and back faces using m_stencilSort, rendering depth only
        EDrawSortFrom1,         //  door sorting, both faces open: draw single sided front and back faces using m_stencilSort, rendering depth only
        EDrawMaskFace1,         //  door masking, front face open, back face closed: draw single sided front face using m_stencilMask
        EDrawMaskFace2,         //  door masking, front face open, back face closed: draw single sided front face using m_stencilKill and the zero depth vertices
        EDrawMaskBack1,         //  door masking, front face closed, back face open: draw single sided back face using m_stencilMask
        EDrawMaskBack2,         //  door masking, front face closed, back face open: draw single sided back face using m_stencilKill and the zero depth vertices
        EDrawMaskBoth1,         //  door masking, both faces open: draw double sided face using m_stencilMask
        EDrawMaskBoth2,         //  door masking, both faces open: draw double sided face using m_stencilKill and the zero depth vertices
        EDrawFastFace1,         //  door masking, viewing through front face: draw double sided front inset cup using m_stencilClip
        EDrawFromFace1,         //  door masking, viewing through front face: draw double sided front inset box using m_stencilClip
        EDrawFromFace2,         //  door masking, viewing through front face: draw double sided front face using m_stencilFace
        EDrawFromFace3,         //  door masking, viewing through front face: draw double sided front inset cup using m_stencilKill and the zero depth vertices
        EDrawFastBack1,         //  door masking, viewing through back face: draw double sided back inset cup using m_stencilClip
        EDrawFromBack1,         //  door masking, viewing through back face: draw double sided back inset box using m_stencilClip
        EDrawFromBack2,         //  door masking, viewing through back face: draw double sided back face using m_stencilFace
        EDrawFromBack3,         //  door masking, viewing through back face: draw double sided back inset cup using m_stencilKill and the zero depth vertices
        EDrawEdgeFace1,         //  door edge frame, front face
        EDrawEdgeBack1,         //  door edge frame, back face
        EDrawEdgeBoth1,         //  door edge frame, both faces
        EDrawTrimFace1,         //  door trim frame, front face
        EDrawTrimBack1,         //  door trim frame, back face
        EDrawTrimBoth1,         //  door trim frame, both faces
        EDrawCallCount,
        EDrawCallForce32 = 0x7fffffff
    };
    bool                    init( const CoreSystem& system, const maths::vec2* const verts, const uint count );
    void                    kill();
    bool                    initResources( const CoreSystem& system );
    bool                    buildGeometry( const maths::vec2* const verts, const uint count );
    static void             appendFaceIndices( IndexStream16& indices, const bool flip, const uint16_t count, const uint16_t start );
    static void             appendBandIndices( IndexStream16& indices, const bool flip, const uint16_t count, const uint16_t row1, const uint16_t row2, const uint16_t step );
    static void             appendEdgeIndices( IndexStream16& indices, const bool flip, const uint16_t count, const uint16_t row1, const uint16_t row2, const uint16_t step );
    static float            s_frame_radius;
    static float            s_frame_width;
    static float            s_frame_depth;
    static float            s_frame_sill;
    static float            s_length;
    static float            s_inset;
    static float            s_zbias;
    bool                    m_valid;
    CoreSystem              m_system;
    Shader                  m_maskShader;
    Shader                  m_fillShader;
    Shader                  m_trimShader;
    BindingConsts           m_bindObject;
    BindingConsts           m_bindViewer;
    BindingConsts           m_bindLintel;
    BindingDepthState       m_bindDepthState;
    BindingRasterState      m_bindRasterState;
    BindingStream           m_bindMaskVertices;
    BindingStream           m_bindFillVertices;
    BindingStream           m_bindTrimVertices;
    ResourceStream          m_maskVertexResource;
    ResourceStream          m_fillVertexResource;
    ResourceStream          m_trimVertexResource;
    ResourceIndices         m_indexResource;
    CVertices               m_maskVertexBuffer;
    CVertices               m_fillVertexBuffer;
    CVertices               m_trimVertexBuffer;
    CIndices                m_indexBuffer;
    ResourceRasterState     m_rasterFace;       //  single sided rendering of the front face
    ResourceRasterState     m_rasterBack;       //  single sided rendering of the back face
    ResourceRasterState     m_rasterBoth;       //  double sided rendering
    ResourceDepthState      m_stencilClip;      //  inverts stencil bit 0
    ResourceDepthState      m_stencilFlip;      //  if stencil bits 1-7 match reference value: inverts stencil bit 0
    ResourceDepthState      m_stencilFace;      //  if stencil bits 1-7 match reference value: front inverts stencil bit 0 if depth passes, back inverts stencil bit 0 ignoring the depth test
    ResourceDepthState      m_stencilMask;      //  if stencil bits 1-7 match reference value and depth passes: inverts stencil bit 0
    ResourceDepthState      m_stencilSort;      //  if stencil bits 1-7 match reference value and depth passes: renders
    ResourceDepthState      m_stencilBias;      //  if stencil bit 0 is set and depth fails: inverts stencil bit 0
    ResourceDepthState      m_stencilKill;      //  if stencil bit 0 is set: replaces the depth and stencil values, ignores depth test
    ResourceCommand         m_commands[ EDrawCallCount ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ItemRenderer item rendering class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemRenderer
{
public:
    inline                  ItemRenderer() : m_valid( false ) {};
    inline                  ~ItemRenderer() { kill(); };
    inline bool             valid() const { return( this && m_valid ); };
    bool                    create( const CoreSystem& system, const constructs::GeometryConstruct& construct );
    void                    destroy();
    void                    render( const ResourceConsts& object, const ResourceConsts& viewer, const UINT stencilRef, const bool doubleSided, const bool writeStencil ) const;
protected:
    bool                    init( const CoreSystem& system, const constructs::GeometryConstruct& construct );
    void                    kill();
    bool                    initResources( const CoreSystem& system );
    bool                    buildGeometry( const constructs::GeometryConstruct& construct );
    bool                    m_valid;
    CoreSystem              m_system;
    Shader                  m_skyShader;
    Shader                  m_drawShader;
    BindingConsts           m_bindObject;
    BindingConsts           m_bindViewer;
    BindingDepthState       m_bindDepthState;
    BindingRasterState      m_bindRasterState;
    BindingStream           m_bindSkyVertices;
    BindingStream           m_bindDrawVertices;
    ResourceStream          m_skyVertexResource;
    ResourceStream          m_drawVertexResource;
    ResourceIndices         m_indexResource;
    CVertices               m_skyVertexBuffer;
    CVertices               m_drawVertexBuffer;
    CIndices                m_indexBuffer;
    ResourceRasterState     m_rasterFace;       //  single sided rendering of the front face
    ResourceRasterState     m_rasterBack;       //  single sided rendering of the back face
    ResourceRasterState     m_rasterBoth;       //  double sided rendering
    ResourceDepthState      m_stencilMask;      //  renders if depth passes, replaces stencil value
    ResourceDepthState      m_stencilDraw;      //  renders if stencil bits 1-7 match reference value and depth passes
    ResourceCommand         m_skyCommand;
    ResourceCommand         m_drawCommand;
    bool                    m_skyValid;
    bool                    m_drawValid;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    IRenderable interface
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INTERFACE_BEGIN( IRenderable )
    virtual void            render() const = 0;
INTERFACE_END();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Renderable array type
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using Renderables = TBlob< IRenderable* >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Renderable classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RenderableRoom : public IRenderable
{
public:
    inline                  RenderableRoom() : m_renderer( NULL ) {};
    inline                  ~RenderableRoom() { reset(); };
    inline bool             valid() const { return( ( ( this != NULL ) && ( m_renderer != NULL ) && m_rtv.valid() && m_dsv.valid() && m_object.valid() && m_viewer.valid() ) ? true : false ); };
    inline void             setRenderer( const ItemRenderer* const renderer ) { m_renderer = renderer; };
    inline void             setRenderer( const ItemRenderer& renderer ) { setRenderer( &renderer ); };
    inline void             setResources( const ResourceRTV& rtv, const ResourceDSV& dsv, const ResourceConsts& object, const ResourceConsts& viewer ) { m_rtv = rtv, m_dsv = dsv; m_object = object; m_viewer = viewer; };
    inline void             setConfig( const FBOX& viewport, const SRECT& scissoring, const UINT stencilRef, const bool doubleSided, const bool writeStencil ) { m_viewport = viewport; m_scissoring = scissoring; m_stencilRef = stencilRef; m_doubleSided = doubleSided; m_writeStencil = writeStencil; };
    inline void             reset() { m_renderer = NULL; m_rtv.release(); m_dsv.release(); m_object.release(); m_viewer.release(); };
    virtual void            render() const { if( valid() ) { m_rtv.setViewport( m_viewport ); m_dsv.setViewport( m_viewport ); m_rtv.setScissoring( m_scissoring ); m_dsv.setScissoring( m_scissoring ); m_renderer->render( m_object, m_viewer, m_stencilRef, m_doubleSided, m_writeStencil ); } };
protected:
    ResourceRTV             m_rtv;
    ResourceDSV             m_dsv;
    ResourceConsts          m_object;
    ResourceConsts          m_viewer;
    FBOX                    m_viewport;
    SRECT                   m_scissoring;
    UINT                    m_stencilRef;
    bool                    m_doubleSided;
    bool                    m_writeStencil;
    const ItemRenderer*     m_renderer;
};

class RenderableItem : public IRenderable
{
public:
    inline                  RenderableItem() : m_renderer( NULL ) {};
    inline                  ~RenderableItem() { reset(); };
    inline bool             valid() const { return( ( ( this != NULL ) && ( m_renderer != NULL ) && m_object.valid() && m_viewer.valid() ) ? true : false ); };
    inline void             setRenderer( const ItemRenderer* const renderer ) { m_renderer = renderer; };
    inline void             setRenderer( const ItemRenderer& renderer ) { setRenderer( &renderer ); };
    inline void             setResources( const ResourceConsts& object, const ResourceConsts& viewer ) { m_object = object; m_viewer = viewer; };
    inline void             setConfig( const UINT stencilRef, const bool doubleSided ) { m_stencilRef = stencilRef; m_doubleSided = doubleSided; };
    inline void             reset() { m_renderer = NULL; m_object.release(); m_viewer.release(); };
    virtual void            render() const { if( valid() ) m_renderer->render( m_object, m_viewer, m_stencilRef, m_doubleSided, false ); };
protected:
    ResourceConsts          m_object;
    ResourceConsts          m_viewer;
    UINT                    m_stencilRef;
    bool                    m_doubleSided;
    const ItemRenderer*     m_renderer;
};

class RenderableDoor : public IRenderable
{
public:
    inline                  RenderableDoor() : m_renderer( NULL ) {};
    inline                  ~RenderableDoor() { reset(); };
    inline bool             valid() const { return( ( ( this != NULL ) && ( m_renderer != NULL ) && m_object.valid() && m_viewer.valid() ) ? true : false ); };
    inline void             setRenderer( const DoorRenderer* const renderer ) { m_renderer = renderer; };
    inline void             setRenderer( const DoorRenderer& renderer ) { setRenderer( &renderer ); };
    inline void             setResources( const ResourceConsts& object, const ResourceConsts& viewer, const ResourceConsts& lintel ) { m_object = object; m_viewer = viewer; m_lintel = lintel; };
    inline void             setConfig( const UINT stencilRef, const UINT stencilNew, const EDoorRenderPass renderPass ) { m_stencilRef = stencilRef; m_stencilNew = stencilNew; m_renderPass = renderPass; };
    inline void             reset() { m_renderer = NULL; m_object.release(); m_viewer.release(); m_lintel.release(); };
    virtual void            render() const { if( valid() ) m_renderer->render( m_object, m_viewer, m_lintel, m_stencilRef, m_stencilNew, m_renderPass ); };
protected:
    ResourceConsts          m_object;
    ResourceConsts          m_viewer;
    ResourceConsts          m_lintel;
    UINT                    m_stencilRef;
    UINT                    m_stencilNew;
    EDoorRenderPass         m_renderPass;
    const DoorRenderer*     m_renderer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Renderables pool types
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using RoomRenderables = TManagedReset< RenderableRoom >;
using ItemRenderables = TManagedReset< RenderableItem >;
using DoorRenderables = TManagedReset< RenderableDoor >;
using ManagedCBuffers = TManagedDestroy< CConstants >;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end locality namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace locality

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //  #ifndef __RENDERABLES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
