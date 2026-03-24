
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vector_display.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Vector display class.
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

#ifndef __VECTOR_DISPLAY_INCLUDED__
#define __VECTOR_DISPLAY_INCLUDED__

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
#include "../utils/standard_palette.h"
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
////    Vector display text effect codes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EVTextEffect
{
    EVTextGreen             = EColourGreen,
    EVTextCyan              = EColourCyan,
    EVTextLime              = EColourLime,
    EVTextPaleGreen         = EColourPaleGreen,
    EVTextEmerald           = EColourEmerald,
    EVTextTeal              = EColourTeal,
    EVTextDarkBrown         = EColourDarkBrown,
    EVTextGoldenBrown       = EColourGoldenBrown,
    EVTextMustard           = EColourMustard,
    EVTextOlive             = EColourOlive,
    EVTextLemon             = EColourLemon,
    EVTextPaleStraw         = EColourPaleStraw,
    EVTextPaleYellow        = EColourPaleYellow,
    EVTextDarkRed           = EColourDarkRed,
    EVTextRichPink          = EColourRichPink,
    EVTextFuscia            = EColourFuscia,
    EVTextCerise            = EColourCerise,
    EVTextPastelPink        = EColourPastelPink,
    EVTextViolet            = EColourViolet,
    EVTextImperialPurple    = EColourImperialPurple,
    EVTextLightPurple       = EColourLightPurple,
    EVTextPurple            = EColourPurple,
    EVTextPastelPurple      = EColourPastelPurple,
    EVTextDarkBlue          = EColourDarkBlue,
    EVTextAzure             = EColourAzure,
    EVTextRichBlue          = EColourRichBlue,
    EVTextSteelBlue         = EColourSteelBlue,
    EVTextAqua              = EColourAqua,
    EVTextPaleBlue          = EColourPaleBlue,
    EVTextPastelBlue        = EColourPastelBlue,
    EVTextGrey              = EColourGrey,
    EVTextWhite             = EColourWhite,
    EVTextNull              = 0x000000ff,
    EVTextBright            = 0x00000020,
    EVTextBlink             = 0x00000040,
    EVTextFlash             = 0x00000080,
    EVTextHidden            = ( EVTextBlink | EVTextFlash ),
    EVTextForce32           = 0x7fffffff,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Vector display class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CVectorDisplay : protected vtext::ICharacterStream, vtext::IIndexedLineStream
{
public:
    struct CONFIG { DIMS2D bufferChars; DIMS2D windowChars; uint charScale; uint charFont; bool descenders; };
public:
    inline                          CVectorDisplay() : m_shader(), m_data( NULL ) {};
    inline                          CVectorDisplay( const CoreSystem& system, const CONFIG& config ) : m_shader(), m_data( NULL ) { create( system, config ); };
    inline                          ~CVectorDisplay() { destroy(); };
    inline bool                     valid() const { return( m_shader.valid() ); };
    bool                            create( const CoreSystem& system, const CONFIG& config );
    void                            destroy();
    void                            update( const float time );
    void                            render();
    bool                            bind( const Binding& binding );
    bool                            unbind( const Binding& binding );
    bool                            bind( const BindingGroup& bindingGroup );
    bool                            unbind( const BindingGroup& bindingGroup );
    void                            putc( const uint x, const uint y, const char character, const uint effect );
    void                            print( const uint x, const uint y, const char* const text, const uint effect );
    void                            setFont( const vtext::VFONT font );
    void                            setFont( const vtext::VFONT font, const unsigned int scale );
    void                            setFont( const vtext::VFONT font, const unsigned int scale, const bool descenders );
    inline void                     enableDepthFade( const bool enable = true ) { m_depthFade = enable; };
    inline void                     disableDepthFade( const bool disable = true ) { m_depthFade = !disable; };
    inline void                     setDepthFade( const float fadeNear, const float fadeFar ) { m_depthFadeNear = fadeNear; m_depthFadeFar = fadeFar; };
    inline void                     setBorderEffect( const uint effect ) { m_borderEffect = effect; };
    inline void                     setTransparency( const float transparency ) { m_transparency = transparency; };
    inline void                     setBrightness( const float brightness ) { m_brightness = brightness; };
    inline void                     setForegroundBrightness( const float brightness ) { m_foreBrightness = brightness; };
    inline void                     setBackgroundBrightness( const float brightness ) { m_backBrightness = brightness; };
    inline void                     setBackgroundTransparency( const float transparency ) { m_backTransparency = transparency; };
    inline void                     setBackgroundColour( const uint effect ) { m_backColour = effect; };
    inline const DIMS2D&            getBufferChars() const { return( m_config.bufferChars ); };
    inline const DIMS2D&            getWindowChars() const { return( m_config.windowChars ); };
    inline int                      getWindowWidth() const { return( static_cast< int >( m_dimensions.width ) ); };
    inline int                      getWindowHeight() const { return( static_cast< int >( m_dimensions.height ) ); };
    inline int                      getSteppingWidth() const { return( static_cast< int >( m_stepping.width ) ); };
    inline int                      getSteppingHeight() const { return( static_cast< int >( m_stepping.height ) ); };
    inline int                      getSpacingWidth() const { return( static_cast< int >( m_spacing.width ) ); };
    inline int                      getSpacingHeight() const { return( static_cast< int >( m_spacing.height ) ); };
    inline void                     setStartRow( const uint row ) { m_startRow = ( ( row > m_startMax ) ? m_startMax : row ); };
    inline uint                     getStartRow() const { return( m_startRow ); };
    inline int                      getStartRowMax() const { return( m_startMax ); };
    inline void                     setHScroll( const int h ) { m_scrollPos.x = ( ( h < 0 ) ? 0 : ( ( h > m_scrollMax.x ) ? m_scrollMax.x : h ) ); };
    inline int                      getHScroll() const { return( m_scrollPos.x ); };
    inline int                      getHScrollMax() const { return( m_scrollMax.x ); };
    inline void                     setVScroll( const int v ) { m_scrollPos.y = ( ( v < 0 ) ? 0 : ( ( v > m_scrollMax.y ) ? m_scrollMax.y : v ) ); };
    inline int                      getVScroll() const { return( m_scrollPos.y ); };
    inline int                      getVScrollMax() const { return( m_scrollMax.y ); };
    static void                     calcMaxWindowChars( const uint charScale, const uint charFont, const bool descenders, const DIMS2D& display, DIMS2D& windowChars );
protected:
    bool                            init( const CoreSystem& system, const CONFIG& config );
    void                            kill();
    Shader                          m_shader;
    BindingConsts                   m_constsBinding;
    BindingStream                   m_vertexBinding;
    BindingBlendState               m_blendBinding;
    ResourceBlendState              m_blendSolid;
    ResourceBlendState              m_blendPremul;
    ResourceConsts                  m_constsResource;
    CConstants                      m_constsBuffer;
    hlsl::VDisplay                  m_constsData;
    ResourceStream                  m_vertexResource;
    CVertices                       m_vertexBuffer;
    VertexStreamDraw2D              m_vertexData;
    ResourceIndices                 m_indexResource;
    CIndices                        m_indexBuffer;
    IndexStream16                   m_indexData;
    DrawingParams                   m_backParams;
    ResourceCommand                 m_backCommand;
    IndexedParams                   m_linesParams;
    ResourceCommand                 m_linesCommand;
    DrawingParams                   m_pointsParams;
    ResourceCommand                 m_pointsCommand;
    IndexedParams                   m_borderLinesParams;
    ResourceCommand                 m_borderLinesCommand;
    DrawingParams                   m_borderPointsParams;
    ResourceCommand                 m_borderPointsCommand;
    CONFIG                          m_config;
    bool                            m_depthFade;
    float                           m_depthFadeNear;
    float                           m_depthFadeFar;
    uint                            m_borderEffect;
    float                           m_transparency;
    float                           m_brightness;
    float                           m_foreBrightness;
    float                           m_backBrightness;
    float                           m_backTransparency;
    uint                            m_backColour;
    DIMS2D                          m_dimensions;
    DIMS2D                          m_stepping;
    DIMS2D                          m_spacing;
    SRECT                           m_rect;
    SPOINT                          m_scrollMax;
    SPOINT                          m_scrollPos;
    uint                            m_startMax;
    uint                            m_startRow;
    uint                            m_wrap;
    char*                           m_data;
    vtext::Text                     m_vtext;
    vtext::VTEXT_CMAP               m_vcmap;
    uint32_t                        m_offset;
    uint32_t                        m_colour;
    bool                            m_skip;
    char                            m_code;
    float                           m_time;
    uint                            m_ticks;
    bool                            m_visible;
    bool                            m_fadeAll;
    bool                            m_fadeBack;
    bool                            m_drawBack;
    bool                            m_drawBorder;
protected:
    void                            SetFont();
    void                            SetEffect( const uint effect );
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

#endif  //  #ifndef __VECTOR_DISPLAY_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
