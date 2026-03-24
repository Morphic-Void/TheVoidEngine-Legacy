
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vector_drawing.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Vector drawing class.
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

#ifndef __VECTOR_DRAWING_INCLUDED__
#define __VECTOR_DRAWING_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "platforms/dx11/system/systemx.h"
#include "../system/public/complete.h"
#include "../utils/vertices.h"
#include "libs/utils/vtext.h"

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
////    Vector drawing class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CVectorDrawing : protected vtext::ICharacterStream, vtext::IIndexedLineStream
{
public:
    inline                          CVectorDrawing() : m_shader(), m_dirty( false ) {};
    inline                          CVectorDrawing( const CoreSystem& system ) : m_shader(), m_dirty( false ) { create( system ); };
    inline                          ~CVectorDrawing() { destroy(); };
    inline bool                     valid() const { return( m_shader.valid() ); };
    inline bool                     dirty() const { return( m_dirty ); };
    bool                            create( const CoreSystem& system );
    void                            destroy();
    void                            update();
    void                            render();
    bool                            bind( const Binding& binding );
    bool                            unbind( const Binding& binding );
    bool                            bind( const BindingGroup& bindingGroup );
    bool                            unbind( const BindingGroup& bindingGroup );
    inline void                     clear() { clearTris(); clearLines(); };
    inline void                     clearTris() { m_trisVertexData.reset( m_trisVertexLock ); m_trisIndexData.reset( m_trisIndexLock ); m_dirty = true; };
    inline void                     clearLines() { m_linesVertexData.reset( m_linesVertexLock ); m_linesIndexData.reset( m_linesIndexLock ); m_dirty = true; };
    inline void                     lockTris() { m_trisVertexLock = m_trisVertexData.getCount(); m_trisIndexLock = m_trisIndexData.getCount(); };
    inline void                     lockLines() { m_linesVertexLock = m_linesVertexData.getCount(); m_linesIndexLock = m_linesIndexData.getCount(); };
    inline void                     unlockTris() { m_trisVertexLock = m_trisIndexLock = 0; };
    inline void                     unlockLines() { m_linesVertexLock = m_linesIndexLock = 0; };
    inline void                     beginFan() { m_drawMode = EModeTriFan; m_drawCount = 0; };
    inline void                     beginStrip() { m_drawMode = EModeTriStrip; m_drawCount = 0; };
    inline void                     beginLines() { m_drawMode = EModeLineStrip; m_drawCount = 0; };
    void                            closeLoop();
    void                            draw( const short x, const short y, const uint32_t colour );
    void                            drawText( const short x, const short y, const char* const text, const uint32_t colour );
    void                            drawLine( const short x1, const short y1, const short x2, const short y2, const uint32_t colour );
    void                            drawLine( const short x1, const short y1, const short x2, const short y2, const uint32_t colour1, const uint32_t colour2 );
    void                            drawRect( const short left, const short top, const short right, const short bottom, const uint32_t colour );
    void                            drawQuad( const short left, const short top, const short right, const short bottom, const uint32_t colour );
    void                            setFont( const vtext::VFONT font );
    void                            setFont( const vtext::VFONT font, const unsigned int scale );
    void                            setFont( const vtext::VFONT font, const unsigned int scale, const bool descenders );
    inline void                     setSolid( const bool solid = true ) { m_solid = solid; };
    inline void                     setTransparent( const bool transparent = true ) { m_solid = !transparent; };
    inline void                     setBrightness( const float brightness ) { m_brightness = brightness; };
    inline void                     setTransparency( const float transparency ) { m_transparency = transparency; };
    inline int                      getSteppingWidth() const { return( static_cast< int >( m_stepping.width ) ); };
    inline int                      getSteppingHeight() const { return( static_cast< int >( m_stepping.height ) ); };
    inline int                      getSpacingWidth() const { return( static_cast< int >( m_spacing.width ) ); };
    inline int                      getSpacingHeight() const { return( static_cast< int >( m_spacing.height ) ); };
protected:
    enum EDrawMode { EModeDefault = 0, EModeTriFan, EModeTriStrip, EModeLineStrip, EModeCount, EModeForce32 = 0x7fffffff };
    bool                            init( const CoreSystem& system );
    void                            kill();
    Shader                          m_shader;
    BindingConsts                   m_constsBinding;
    BindingStream                   m_vertexBinding;
    BindingBlendState               m_blendBinding;
    ResourceBlendState              m_blendSolid;
    ResourceBlendState              m_blendPremul;
    ResourceConsts                  m_constsResource;
    CConstants                      m_constsBuffer;
    hlsl::VDrawing                  m_constsData;
    ResourceStream                  m_trisVertexResource;
    ResourceIndices                 m_trisIndexResource;
    CVertices                       m_trisVertexBuffer;
    CIndices                        m_trisIndexBuffer;
    VertexStreamDraw2D              m_trisVertexData;
    IndexStream16                   m_trisIndexData;
    uint                            m_trisVertexLock;
    uint                            m_trisIndexLock;
    ResourceStream                  m_linesVertexResource;
    ResourceIndices                 m_linesIndexResource;
    CVertices                       m_linesVertexBuffer;
    CIndices                        m_linesIndexBuffer;
    VertexStreamDraw2D              m_linesVertexData;
    IndexStream16                   m_linesIndexData;
    uint                            m_linesVertexLock;
    uint                            m_linesIndexLock;
    IndexedParams                   m_trisParams;
    ResourceCommand                 m_trisCommand;
    IndexedParams                   m_linesParams;
    ResourceCommand                 m_linesCommand;
    DrawingParams                   m_pointsParams;
    ResourceCommand                 m_pointsCommand;
    bool                            m_solid;
    float                           m_brightness;
    float                           m_transparency;
    DIMS2D                          m_stepping;
    DIMS2D                          m_spacing;
    uint                            m_drawMode;
    uint                            m_drawCount;
    DRAW2D_VERTEX                   m_drawVertices[ 2 ];
    uint16_t                        m_drawIndices[ 2 ];
    vtext::Text                     m_vtext;
    uint32_t                        m_offset;
    uint32_t                        m_colour;
    bool                            m_visible;
    bool                            m_premul;
    bool                            m_dirty;
protected:
    virtual char                    Item( const char character, const char code );
    virtual void                    Draw( const vtext::Glyph& glyph, const int x, const int y, const unsigned int scale );
    virtual void                    AddVert( const int x, const int y );
    virtual void                    AddLine( const unsigned int i1, const unsigned int i2 );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end rendering namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};  //  namespace rendering

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //  #ifndef __VECTOR_DRAWING_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
