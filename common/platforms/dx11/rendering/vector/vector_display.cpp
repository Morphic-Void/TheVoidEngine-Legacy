
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vector_display.cpp
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
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/system/debug/asserts.h"
#include "libs/utils/colours.h"
#include "vector_display.h"

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
////    Vector display class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CVectorDisplay::create( const CoreSystem& system, const CONFIG& config )
{
    kill();
    if( init( system, config ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void CVectorDisplay::destroy()
{
    kill();
}

bool CVectorDisplay::init( const CoreSystem& system, const CONFIG& config )
{
//  validate the configuration
    if( ( config.bufferChars.width < config.windowChars.width ) || ( config.bufferChars.height < config.windowChars.height ) || ( config.charFont >= vtext::VFONT_COUNT ) || ( config.charScale == 0 ) )
    {
        return( false );
    }

//  create the name string for the standard vector display resources and bindings
    StringItem name( "vdisplay" );

//  create the shader and bind the default bindings
    if( !system.shaders.create( name, m_shader ) )
    {
        return( false );
    }
    m_shader.bind( system.defaults );

//  create the local bindings and bind them to the shader
    if( !system.shaders.create( "cb_vdisplay", m_constsBinding ) )
    {
        return( false );
    }
    if( !system.shaders.create( "draw2d", m_vertexBinding ) )
    {
        return( false );
    }
    if( !system.shaders.create( name, m_blendBinding ) )
    {
        return( false );
    }
    m_shader.bind( m_constsBinding );
    m_shader.bind( m_vertexBinding );
    m_shader.bind( m_blendBinding );

//  setup the required blend resources
    if( !system.registry.find( "solid", m_blendSolid ) )
    {
        return( false );
    }
    if( !system.registry.find( "premul", m_blendPremul ) )
    {
        return( false );
    }
    m_blendBinding.bind( m_blendPremul );

//  create the instance resource and bind it
    system.factory.create( m_constsResource );
    m_constsBinding.bind( m_constsResource );

//  create the vertex binding and resource and bind it
    system.factory.create( m_vertexResource );
    m_vertexBinding.bind( m_vertexResource );

//  create the index buffer resource
    system.factory.create( m_indexResource );

//  initialise a primitive description structure
    PrimDesc primDesc;
    memset( &primDesc, 0, sizeof( primDesc ) );

//  create the background command
    primDesc.primType = rendering::EPrimTypeTriStrip;
    system.factory.create( m_backCommand );
    m_backCommand.setPrim( primDesc );
    memset( &m_backParams, 0, sizeof( m_backParams ) );
    m_backParams.firstVertexIndex = 0;
    m_backParams.vertexCount = 4;
    m_backCommand.setDraw( m_backParams );

//  create the text lines command
    primDesc.primType = rendering::EPrimTypeLineList;
    system.factory.create( m_linesCommand );
    m_linesCommand.setPrim( primDesc );
    memset( &m_linesParams, 0, sizeof( m_linesParams ) );
    m_linesParams.firstIndexIndex = 8;

//  create the text points command
    primDesc.primType = rendering::EPrimTypePointList;
    system.factory.create( m_pointsCommand );
    m_pointsCommand.setPrim( primDesc );
    memset( &m_pointsParams, 0, sizeof( m_pointsParams ) );
    m_pointsParams.firstVertexIndex = 8;

//  create the outline commands
    primDesc.primType = rendering::EPrimTypeLineList;
    system.factory.create( m_borderLinesCommand );
    m_borderLinesCommand.setPrim( primDesc );
    memset( &m_borderLinesParams, 0, sizeof( m_borderLinesParams ) );
    m_borderLinesParams.indexCount = 8;
    m_borderLinesCommand.setDraw( m_borderLinesParams, m_indexResource );
    primDesc.primType = rendering::EPrimTypePointList;
    system.factory.create( m_borderPointsCommand );
    m_borderPointsCommand.setPrim( primDesc );
    memset( &m_borderPointsParams, 0, sizeof( m_borderPointsParams ) );
    m_borderPointsParams.firstVertexIndex = 4;
    m_borderPointsParams.vertexCount = 4;
    m_borderPointsCommand.setDraw( m_borderPointsParams );

//  initialise the data containers
    memset( &m_constsData, 0, sizeof( m_constsData ) );
    m_vertexData.init( 256 );
    m_indexData.init( 512 );

//  general initialise
    m_config = config;
    m_depthFade = false;
    m_depthFadeNear = 100.0f;
    m_depthFadeFar = 200.0f;
    m_borderEffect = EVTextGreen;
    m_transparency = 0.0f;
    m_brightness = 1.0f;
    m_foreBrightness = 1.0f;
    m_backBrightness = 0.001f;
    m_backTransparency = 0.15f;
    m_backColour = EVTextGreen;
    m_wrap = ( m_config.bufferChars.width * m_config.bufferChars.height );
    m_data = new char[ m_wrap << 1 ];
    memset( m_data, 0, ( m_wrap << 1 ) );
    SetFont();
    m_scrollPos.x = 0;
    m_scrollPos.y = 0;
    m_startMax = ( m_config.bufferChars.height - 1 );
    m_startRow = 0;
    m_vtext.SetOrigin( m_spacing.width, m_spacing.height );
    m_vcmap.start = 0;
    m_vcmap.width = m_config.bufferChars.width;
    m_vcmap.height = m_config.bufferChars.height;
    m_vcmap.text = m_data;
    m_vcmap.codes = &m_data[ m_wrap ];
    m_offset = 0;
    m_colour = 0xff000000;
    m_skip = true;
    m_code = 0;
    m_visible = false;
    m_fadeAll = false;
    m_fadeBack = false;
    m_drawBack = false;
    m_drawBorder = false;
    return( true );
}

void CVectorDisplay::kill()
{
    if( this )
    {
        m_shader.release();
        m_constsBinding.release();
        m_vertexBinding.release();
        m_blendBinding.release();
        m_blendSolid.release();
        m_blendPremul.release();
        m_constsResource.release();
        m_constsBuffer.destroy();
        memset( &m_constsData, 0, sizeof( m_constsData ) );
        m_vertexResource.release();
        m_vertexBuffer.destroy();
        m_vertexData.kill();
        m_indexResource.release();
        m_indexBuffer.destroy();
        m_indexData.kill();
        memset( &m_backParams, 0, sizeof( m_backParams ) );
        m_backCommand.release();
        memset( &m_linesParams, 0, sizeof( m_linesParams ) );
        m_linesCommand.release();
        memset( &m_pointsParams, 0, sizeof( m_pointsParams ) );
        m_pointsCommand.release();
        memset( &m_borderLinesParams, 0, sizeof( m_borderLinesParams ) );
        m_borderLinesCommand.release();
        memset( &m_borderPointsParams, 0, sizeof( m_borderPointsParams ) );
        m_borderPointsCommand.release();
        memset( &m_config, 0, sizeof( m_config ) );
        m_depthFade = false;
        m_depthFadeNear = 100.0f;
        m_depthFadeFar = 200.0f;
        m_borderEffect = EVTextNull;
        m_transparency = 0.0f;
        m_brightness = 1.0f;
        m_foreBrightness = 1.0f;
        m_backBrightness = 0.0f;
        m_backTransparency = 1.0f;
        m_backColour = EVTextNull;
        m_dimensions.width = m_dimensions.height = 0;
        m_stepping.width = m_stepping.height = 0;
        m_spacing.width = m_spacing.height = 0;
        m_rect.left = m_rect.top = m_rect.right = m_rect.bottom = 0;
        m_scrollMax.x = m_scrollMax.y = m_scrollPos.x = m_scrollPos.y = 0;
        m_startMax = m_startRow = 0;
        m_wrap = 0;
        if( m_data != NULL )
        {
            delete[] m_data;
            m_data = NULL;
        }
        memset( &m_vtext, 0, sizeof( m_vtext ) );
        memset( &m_vcmap, 0, sizeof( m_vcmap ) );
        m_offset = 0;
        m_colour = 0;
        m_skip = false;
        m_code = 0;
        m_time = 0.0f;
        m_ticks = 0;
        m_visible = false;
        m_fadeAll = false;
        m_fadeBack = false;
        m_drawBack = false;
        m_drawBorder = false;
    }
}

void CVectorDisplay::update( const float time )
{
    if( valid() )
    {

//  update timing
        float total = ( m_time + time );
        float ticks = floorf( total );
        m_time = ( total - ticks );
        m_ticks += static_cast< uint >( ticks );

//  reset the vertex and index data containers
        m_vertexData.reset();
        m_indexData.reset();

//  add the background quad vertices
        m_colour = ( g_standardPalette[ m_backColour & 31 ] & 0x00ffffff );
        AddVert( m_rect.left, m_rect.top );
        AddVert( ( m_rect.right - 1 ), m_rect.top );
        AddVert( m_rect.left, ( m_rect.bottom - 1 ) );
        AddVert( ( m_rect.right - 1 ), ( m_rect.bottom - 1 ) );

//  add the border rect vertices and indices
        SetEffect( m_borderEffect );
        m_drawBorder = !m_skip;
        m_offset = static_cast< uint32_t >( m_vertexData.getCount() );
        AddVert( m_rect.left, m_rect.top );
        AddVert( ( m_rect.right - 1 ), m_rect.top );
        AddVert( m_rect.left, ( m_rect.bottom - 1 ) );
        AddVert( ( m_rect.right - 1 ), ( m_rect.bottom - 1 ) );
        AddLine( 0, 1 );
        AddLine( 1, 3 );
        AddLine( 3, 2 );
        AddLine( 2, 0 );

//  output the text vertices and indices
        m_offset = static_cast< uint32_t >( m_vertexData.getCount() );
        m_colour = 0xff000000;
        m_skip = true;
        m_code = 0;
        m_vcmap.start = m_startRow;
        m_vtext.SetOrigin( ( m_scrollPos.x + m_spacing.width ), ( m_scrollPos.y + m_spacing.height ) );
        m_vtext.Draw( *this, m_vcmap );

//  update the constants data
        memset( &m_constsData, 0, sizeof( m_constsData ) );
        m_constsData.ranges.x = ( ( m_dimensions.width - 1 ) * 0.5f );
        m_constsData.ranges.y = ( ( m_dimensions.height - 1 ) * 0.5f );
        if( m_depthFade )
        {
            m_constsData.ranges.z = ( -1.0f / ( m_depthFadeFar - m_depthFadeNear ) );
            m_constsData.ranges.w = ( m_depthFadeFar / ( m_depthFadeFar - m_depthFadeNear ) );
        }
        else
        {
            m_constsData.ranges.z = 0.0f;
            m_constsData.ranges.w = 1.0f;
        }
        m_constsData.alphas.z = ( 1.0f - m_transparency );
        m_constsData.alphas.x = ( ( 1.0f - m_backTransparency ) * m_constsData.alphas.z );
        m_constsData.alphas.w = ( m_foreBrightness * m_brightness * m_constsData.alphas.z );
        m_constsData.alphas.y = ( m_backBrightness * m_brightness * m_constsData.alphas.x );

//  update the buffers
        m_constsBuffer.create( sizeof( m_constsData ), &m_constsData );
        m_vertexBuffer.create( m_vertexData.getCount(), m_vertexData.getStride(), m_vertexData.getBuffer() );
        m_indexBuffer.create( m_indexData.getCount(), m_indexData.getStride(), m_indexData.getBuffer() );

//  update the resources
        m_constsResource.setBuffer( m_constsBuffer.getBuffer() );
        m_constsResource.setOffset( 0 );
        m_constsResource.setStride( m_constsBuffer.getStride() );
        m_vertexResource.setStream( m_vertexBuffer.getBuffer() );
        m_vertexResource.setOffset( 0 );
        m_vertexResource.setStride( m_vertexBuffer.getStride() );
        m_indexResource.setIndices( m_indexBuffer.getBuffer() );
        m_indexResource.setOffset( 0 );
        m_indexResource.setStride( m_indexBuffer.getStride() );

//  update the commands
        m_linesParams.indexCount = static_cast< UINT >( m_indexData.getCount() - m_linesParams.firstIndexIndex );
        m_linesCommand.setDraw( m_linesParams, m_indexResource );
        m_pointsParams.vertexCount = static_cast< UINT >( m_vertexData.getCount() - m_pointsParams.firstVertexIndex );
        m_pointsCommand.setDraw( m_pointsParams );

//  setup the rendering flags
        m_visible = ( ( m_transparency < 1.0f ) ? true : false );
        m_fadeAll = ( ( m_depthFade || ( m_transparency > 0.0f ) ) ? true : false );
        m_fadeBack = false;
        m_drawBack = false;
        if( m_backTransparency < 1.0f )
        {
            if( m_backTransparency > 0.0f )
            {
                if( m_fadeAll )
                {
                    m_drawBack = true;
                }
                else
                {
                    m_fadeBack = true;
                }
            }
            else
            {
                m_drawBack = true;
            }
        }
    }
}

void CVectorDisplay::render()
{
    if( valid() )
    {
        if( m_visible )
        {   //  something is not fully transparent
            if( m_fadeAll )
            {   //  everything is transparent
                m_blendBinding.bind( m_blendPremul );
            }
            else
            {
                if( m_fadeBack )
                {   //  the background is transparent
                    m_blendBinding.bind( m_blendPremul );
                    if( m_shader.update() )
                    {
                        if( m_shader.submit() )
                        {
                            m_shader.execute( m_backCommand );
                        }
                    }
                }
                m_blendBinding.bind( m_blendSolid );
            }
            if( m_shader.update() )
            {
                if( m_shader.submit() )
                {
                    if( m_drawBack )
                    {
                        m_shader.execute( m_backCommand );
                    }
                    if( m_linesParams.indexCount )
                    {
                        m_shader.execute( m_linesCommand );
                    }
                    if( m_pointsParams.vertexCount )
                    {
                        m_shader.execute( m_pointsCommand );
                    }
                    if( m_drawBorder )
                    {
                        m_shader.execute( m_borderLinesCommand );
                        m_shader.execute( m_borderPointsCommand );
                    }
                }
            }
        }
    }
}

bool CVectorDisplay::bind( const Binding& binding )
{
    return( valid() ? m_shader.bind( binding ) : false );
}

bool CVectorDisplay::unbind( const Binding& binding )
{
    return( valid() ? m_shader.unbind( binding ) : false );
}

bool CVectorDisplay::bind( const BindingGroup& bindingGroup )
{
    return( valid() ? m_shader.bind( bindingGroup ) : false );
}

bool CVectorDisplay::unbind( const BindingGroup& bindingGroup )
{
    return( valid() ? m_shader.unbind( bindingGroup ) : false );
}

void CVectorDisplay::putc( const uint x, const uint y, const char character, const uint effect )
{
    if( valid() )
    {
        char code = ( static_cast< char >( effect ) + 1 );
        uint offset = ( ( x % m_vcmap.width ) + ( ( y % m_vcmap.height ) * m_vcmap.width ) );
        m_vcmap.text[ offset ] = character;
        m_vcmap.codes[ offset ] = code;
    }
}

void CVectorDisplay::print( const uint x, const uint y, const char* const text, const uint effect )
{
    if( valid() )
    {
        char code = ( static_cast< char >( effect ) + 1 );
        uint length = 0;
        while( text[ length ] ) ++length;
        if( length )
        {
            uint offset = ( ( x + ( y * m_vcmap.width ) ) % m_wrap );
            uint index = ( m_wrap - offset );
            if( index > length )
            {
                index = length;
            }
            memcpy( &m_vcmap.text[ offset ], text, index );
            memset( &m_vcmap.codes[ offset ], code, index );
            length -= index;
            if( length )
            {
                for( uint skip = ( m_wrap << 1 ); length >= skip; length -= m_wrap ) index += m_wrap;
                if( length > m_wrap )
                {
                    memcpy( m_vcmap.text, &text[ index ], m_wrap );
                    memset( m_vcmap.codes, code, m_wrap );
                    index += m_wrap;
                    length -= m_wrap;
                }
                memcpy( m_vcmap.text, &text[ index ], length );
                memset( m_vcmap.codes, code, length );
            }
        }
    }
}

void CVectorDisplay::setFont( const vtext::VFONT font )
{
    if( valid() )
    {
        if( font < vtext::VFONT_COUNT )
        {
            m_config.charFont = font;
            SetFont();
        }
    }
}

void CVectorDisplay::setFont( const vtext::VFONT font, const unsigned int scale )
{
    if( valid() )
    {
        if( font < vtext::VFONT_COUNT )
        {
            m_config.charFont = font;
            m_config.charScale = ( ( scale == 0 ) ? 1 : scale );
            SetFont();
        }
    }
}

void CVectorDisplay::setFont( const vtext::VFONT font, const unsigned int scale, const bool descenders )
{
    if( valid() )
    {
        if( font < vtext::VFONT_COUNT )
        {
            m_config.charFont = font;
            m_config.charScale = ( ( scale == 0 ) ? 1 : scale );
            m_config.descenders = descenders;
            SetFont();
        }
    }
}

void CVectorDisplay::SetFont()
{
    m_vtext.Reset();
    m_vtext.SetFont( static_cast< vtext::VFONT >( m_config.charFont ), m_config.charScale, m_config.descenders );
    m_vtext.ConvertCharsToClip( m_config.windowChars.width, m_config.windowChars.height, m_dimensions.width, m_dimensions.height, true );
    m_vtext.GetStepping( m_stepping.width, m_stepping.height );
    m_vtext.GetSpacing( m_spacing.width, m_spacing.height );
    m_rect.left = 0;
    m_rect.top = 0;
    m_rect.right = m_dimensions.width;
    m_rect.bottom = m_dimensions.height;
    m_vtext.SetClipping( m_rect );
    m_scrollMax.x = ( m_stepping.width * ( m_config.bufferChars.width - m_config.windowChars.width ) );
    m_scrollMax.y = ( m_stepping.height * ( m_config.bufferChars.height - m_config.windowChars.height ) );
}

void CVectorDisplay::SetEffect( const uint effect )
{
    m_skip = false;
    m_colour = g_standardPalette[ effect & 31 ];
    if( effect & EVTextFlash )
    {
        if( effect & EVTextBlink )
        {
            m_skip = true;
        }
        if( m_ticks & 0x00000100 )
        {
            m_colour = ( ( ( m_colour >> 3 ) & 0x001f1f1f ) | 0xff000000 );
        }
    }
    else
    {
        if( effect & EVTextBlink )
        {
            if( m_ticks & 0x00000200 )
            {
                m_skip = true;
            }
        }
        if( ( effect & EVTextBright ) == 0 )
        {
            m_colour = ( ( ( m_colour >> 1 ) & 0x007f7f7f ) | 0xff000000 );
        }
    }
}

char CVectorDisplay::Item( const char character, const char code )
{   //  character translation and effects
    if( m_code != code )
    {   //  code change
        m_code = code;
        uint effect = ( static_cast< uint >( code - 1 ) & 255 );
        SetEffect( effect );
    }
    return( character );
}

void CVectorDisplay::Draw( const vtext::Glyph& glyph, const int x, const int y, const unsigned int scale )
{
    if( !m_skip )
    {
        m_offset = static_cast< uint32_t >( m_vertexData.getCount() );
        glyph.Draw( *this, x, y, scale );
    }
}

void CVectorDisplay::AddVert( const int x, const int y )
{
    DRAW2D_VERTEX vertex;
    vertex.colour = m_colour;
    vertex.position.x = static_cast< int16_t >( x );
    vertex.position.y = static_cast< int16_t >( y );
    m_vertexData.append( vertex );
}

void CVectorDisplay::AddLine( const unsigned int i1, const unsigned int i2 )
{
    m_indexData.append( static_cast< uint16_t >( i1 + m_offset ) );
    m_indexData.append( static_cast< uint16_t >( i2 + m_offset ) );
}

void CVectorDisplay::calcMaxWindowChars( const uint charScale, const uint charFont, const bool descenders, const DIMS2D& display, DIMS2D& windowChars )
{
    const vtext::Font& theFont = vtext::Font::GetFont( static_cast< vtext::VFONT >( charFont ) );
    DIMS2D stepping;
    theFont.GetStepping( stepping.width, stepping.height, charScale, descenders );
    uint spacing = theFont.GetSpacing( charScale );
    spacing += 2;
    windowChars.width = ( ( display.width >= spacing ) ? ( ( display.width - spacing ) / stepping.width ) : 0 );
    windowChars.height = ( ( display.height >= spacing ) ? ( ( display.height - spacing ) / stepping.height ) : 0 );
}

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
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

