
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vector_drawing.cpp
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
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/system/debug/asserts.h"
#include "libs/utils/colours.h"
#include "vector_drawing.h"

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
////    Vector drawing class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CVectorDrawing::create( const CoreSystem& system )
{
    kill();
    if( init( system ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void CVectorDrawing::destroy()
{
    kill();
}

bool CVectorDrawing::init( const CoreSystem& system )
{
//  create the name string for the standard vector drawing resources and bindings
    StringItem name( "vdrawing" );

//  create the shader and bind the default bindings
    if( !system.shaders.create( name, m_shader ) )
    {
        return( false );
    }
    m_shader.bind( system.defaults );

//  create the local bindings and bind them to the shader
    if( !system.shaders.create( "cb_vdrawing", m_constsBinding ) || !system.shaders.create( "draw2d", m_vertexBinding ) || !system.shaders.create( name, m_blendBinding ) )
    {
        return( false );
    }
    m_shader.bind( m_constsBinding );
    m_shader.bind( m_vertexBinding );
    m_shader.bind( m_blendBinding );

//  setup the required blend resources
    if( !system.registry.find( "solid", m_blendSolid ) || !system.registry.find( "premul", m_blendPremul ) )
    {
        return( false );
    }
    m_blendBinding.bind( m_blendPremul );

//  create the instance resource and bind it
    system.factory.create( m_constsResource );
    m_constsBinding.bind( m_constsResource );

//  create the vertex and index resources
    system.factory.create( m_trisVertexResource );
    system.factory.create( m_trisIndexResource );
    system.factory.create( m_linesVertexResource );
    system.factory.create( m_linesIndexResource );

//  initialise a primitive description structure
    PrimDesc primDesc;
    memset( &primDesc, 0, sizeof( primDesc ) );

//  create the tris command
    primDesc.primType = rendering::EPrimTypeTriList;
    system.factory.create( m_trisCommand );
    m_trisCommand.setPrim( primDesc );
    memset( &m_trisParams, 0, sizeof( m_trisParams ) );

//  create the lines command
    primDesc.primType = rendering::EPrimTypeLineList;
    system.factory.create( m_linesCommand );
    m_linesCommand.setPrim( primDesc );
    memset( &m_linesParams, 0, sizeof( m_linesParams ) );

//  create the points command
    primDesc.primType = rendering::EPrimTypePointList;
    system.factory.create( m_pointsCommand );
    m_pointsCommand.setPrim( primDesc );
    memset( &m_pointsParams, 0, sizeof( m_pointsParams ) );

//  initialise the data containers
    memset( &m_constsData, 0, sizeof( m_constsData ) );
    m_trisVertexData.init( 128 );
    m_trisIndexData.init( 384 );
    m_trisVertexLock = m_trisIndexLock = 0;
    m_linesVertexData.init( 256 );
    m_linesIndexData.init( 512 );
    m_linesVertexLock = m_linesIndexLock = 0;

//  general initialise
    m_solid = true;
    m_brightness = 1.0f;
    m_transparency = 0.0f;
    m_vtext.Reset();
    m_vtext.GetStepping( m_stepping.width, m_stepping.height );
    m_vtext.GetSpacing( m_spacing.width, m_spacing.height );
    m_drawMode = EModeDefault;
    m_drawCount = 0;
    memset( &m_drawVertices, 0, sizeof( m_drawVertices ) );
    m_drawIndices[ 0 ] = m_drawIndices[ 1 ] = 0;
    m_offset = 0;
    m_colour = 0;
    m_visible = false;
    m_premul = false;
    m_dirty = false;
    return( true );
}

void CVectorDrawing::kill()
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
        m_trisVertexResource.release();
        m_trisIndexResource.release();
        m_trisVertexBuffer.destroy();
        m_trisIndexBuffer.destroy();
        m_trisVertexData.kill();
        m_trisIndexData.kill();
        m_trisVertexLock = m_trisIndexLock = 0;
        m_linesVertexResource.release();
        m_linesIndexResource.release();
        m_linesVertexBuffer.destroy();
        m_linesIndexBuffer.destroy();
        m_linesVertexData.kill();
        m_linesIndexData.kill();
        m_linesVertexLock = m_linesIndexLock = 0;
        memset( &m_trisParams, 0, sizeof( m_trisParams ) );
        m_trisCommand.release();
        memset( &m_linesParams, 0, sizeof( m_linesParams ) );
        m_linesCommand.release();
        memset( &m_pointsParams, 0, sizeof( m_pointsParams ) );
        m_pointsCommand.release();
        m_solid = true;
        m_brightness = 1.0f;
        m_transparency = 0.0f;
        m_stepping.width = m_stepping.height = 0;
        m_spacing.width = m_spacing.height = 0;
        m_drawMode = EModeDefault;
        m_drawCount = 0;
        memset( &m_drawVertices, 0, sizeof( m_drawVertices ) );
        m_drawIndices[ 0 ] = m_drawIndices[ 1 ] = 0;
        memset( &m_vtext, 0, sizeof( m_vtext ) );
        m_offset = 0;
        m_colour = 0;
        m_visible = false;
        m_premul = false;
        m_dirty = false;
    }
}

void CVectorDrawing::update()
{
    if( valid() )
    {
        if( m_dirty )
        {   //  something has changed, so update

//  update the constants data
            memset( &m_constsData, 0, sizeof( m_constsData ) );
            m_constsData.config.x = m_brightness;
            if( m_solid )
            {
                m_constsData.config.y = 0.0f;
                m_constsData.config.z = ( 1.0f - m_transparency );
            }
            else
            {
                m_constsData.config.y = ( 1.0f - m_transparency );
                m_constsData.config.z = 0.0f;
            }

//  update the buffers and resources
            m_constsBuffer.create( sizeof( m_constsData ), &m_constsData );
            m_constsResource.setBuffer( m_constsBuffer.getBuffer() );
            m_constsResource.setOffset( 0 );
            m_constsResource.setStride( m_constsBuffer.getStride() );
            if( m_trisVertexData.getCount() )
            {
                m_trisVertexBuffer.create( m_trisVertexData.getCount(), m_trisVertexData.getStride(), m_trisVertexData.getBuffer() );
                m_trisVertexResource.setStream( m_trisVertexBuffer.getBuffer() );
                m_trisVertexResource.setOffset( 0 );
                m_trisVertexResource.setStride( m_trisVertexBuffer.getStride() );
            }
            if( m_trisIndexData.getCount() )
            {
                m_trisIndexBuffer.create( m_trisIndexData.getCount(), m_trisIndexData.getStride(), m_trisIndexData.getBuffer() );
                m_trisIndexResource.setIndices( m_trisIndexBuffer.getBuffer() );
                m_trisIndexResource.setOffset( 0 );
                m_trisIndexResource.setStride( m_trisIndexBuffer.getStride() );
            }
            if( m_linesVertexData.getCount() )
            {
                m_linesVertexBuffer.create( m_linesVertexData.getCount(), m_linesVertexData.getStride(), m_linesVertexData.getBuffer() );
                m_linesVertexResource.setStream( m_linesVertexBuffer.getBuffer() );
                m_linesVertexResource.setOffset( 0 );
                m_linesVertexResource.setStride( m_linesVertexBuffer.getStride() );
            }
            if( m_linesIndexData.getCount() )
            {
                m_linesIndexBuffer.create( m_linesIndexData.getCount(), m_linesIndexData.getStride(), m_linesIndexData.getBuffer() );
                m_linesIndexResource.setIndices( m_linesIndexBuffer.getBuffer() );
                m_linesIndexResource.setOffset( 0 );
                m_linesIndexResource.setStride( m_linesIndexBuffer.getStride() );
            }

//  update the commands
            m_trisParams.indexCount = static_cast< UINT >( m_trisIndexData.getCount() );
            m_trisCommand.setDraw( m_trisParams, m_trisIndexResource );
            m_linesParams.indexCount = static_cast< UINT >( m_linesIndexData.getCount() );
            m_linesCommand.setDraw( m_linesParams, m_linesIndexResource );
            m_pointsParams.vertexCount = static_cast< UINT >( m_linesVertexData.getCount() );
            m_pointsCommand.setDraw( m_pointsParams );

//  setup the rendering flags
            m_visible = ( ( m_transparency < 1.0f ) ? true : false );
            m_premul = ( ( m_transparency > 0.0f ) ? true : false );
            m_dirty = false;
            m_drawMode = EModeDefault;
        }
    }
}

void CVectorDrawing::render()
{
    if( valid() )
    {
        if( m_visible )
        {   //  something is not fully transparent
            m_blendBinding.bind( m_premul ? m_blendPremul : m_blendSolid );
            if( m_trisParams.indexCount )
            {
                m_vertexBinding.bind( m_trisVertexResource );
                if( m_shader.update() )
                {
                    if( m_shader.submit() )
                    {
                        m_shader.execute( m_trisCommand );
                    }
                }
            }
            if( ( m_linesParams.indexCount ) || ( m_pointsParams.vertexCount ) )
            {
                m_vertexBinding.bind( m_linesVertexResource );
                if( m_shader.update() )
                {
                    if( m_shader.submit() )
                    {
                        if( m_linesParams.indexCount )
                        {
                            m_shader.execute( m_linesCommand );
                        }
                        if( m_pointsParams.vertexCount )
                        {
                            m_shader.execute( m_pointsCommand );
                        }
                    }
                }
            }
        }
    }
}

bool CVectorDrawing::bind( const Binding& binding )
{
    return( valid() ? m_shader.bind( binding ) : false );
}

bool CVectorDrawing::unbind( const Binding& binding )
{
    return( valid() ? m_shader.unbind( binding ) : false );
}

bool CVectorDrawing::bind( const BindingGroup& bindingGroup )
{
    return( valid() ? m_shader.bind( bindingGroup ) : false );
}

bool CVectorDrawing::unbind( const BindingGroup& bindingGroup )
{
    return( valid() ? m_shader.unbind( bindingGroup ) : false );
}

void CVectorDrawing::closeLoop()
{
    if( valid() )
    {
        if( m_drawMode == EModeLineStrip )
        {
            m_linesIndexData.append( m_drawIndices[ 0 ] );
            m_linesIndexData.append( m_drawIndices[ 1 ] );
            m_dirty = true;
        }
        m_drawMode = EModeDefault;
    }
}

void CVectorDrawing::draw( const short x, const short y, const uint32_t colour )
{
    if( valid() )
    {
        DRAW2D_VERTEX vertex;
        vertex.colour = colour;
        vertex.position.x = static_cast< int16_t >( x );
        vertex.position.y = static_cast< int16_t >( y );
        switch( m_drawMode )
        {
            case( EModeTriFan ):
            {
                if( m_drawCount < 2 )
                {   //  gather vertices ready for the first triangle
                    m_drawVertices[ m_drawCount ] = vertex;
                }
                else
                {
                    if( m_drawCount == 2 )
                    {   //  start the triangle fan
                        m_drawIndices[ 0 ] = static_cast< uint16_t >( m_trisVertexData.getCount() );
                        m_drawIndices[ 1 ] = ( m_drawIndices[ 0 ] + 1 );
                        m_trisVertexData.append( m_drawVertices[ 0 ] );
                        m_trisVertexData.append( m_drawVertices[ 1 ] );
                    }
                    m_trisVertexData.append( vertex );
                    m_trisIndexData.append( m_drawIndices[ 0 ] );
                    m_trisIndexData.append( m_drawIndices[ 1 ] );
                    uint16_t& index = m_drawIndices[ 1 ];
                    index += 1;
                    m_trisIndexData.append( index );
                    m_dirty = true; 
                }
                break;
            }
            case( EModeTriStrip ):
            {
                if( m_drawCount < 2 )
                {   //  gather vertices ready for the first triangle
                    m_drawVertices[ m_drawCount ] = vertex;
                }
                else
                {
                    if( m_drawCount == 2 )
                    {   //  start the triangle strip
                        m_drawIndices[ 0 ] = static_cast< uint16_t >( m_trisVertexData.getCount() );
                        m_drawIndices[ 1 ] = ( m_drawIndices[ 0 ] + 1 );
                        m_trisVertexData.append( m_drawVertices[ 0 ] );
                        m_trisVertexData.append( m_drawVertices[ 1 ] );
                    }
                    m_trisVertexData.append( vertex );
                    m_trisIndexData.append( m_drawIndices[ 0 ] );
                    m_trisIndexData.append( m_drawIndices[ 1 ] );
                    uint16_t& index = m_drawIndices[ m_drawCount & 1 ];
                    index += 2;
                    m_trisIndexData.append( index );
                    m_dirty = true; 
                }
                break;
            }
            case( EModeLineStrip ):
            {
                if( m_drawCount < 1 )
                {
                    m_drawVertices[ 0 ] = vertex;
                }
                else
                {
                    if( m_drawCount == 1 )
                    {   //  start the line strip
                        m_drawIndices[ 0 ] = m_drawIndices[ 1 ] = static_cast< uint16_t >( m_linesVertexData.getCount() );
                        m_linesVertexData.append( m_drawVertices[ 0 ] );
                    }
                    m_linesVertexData.append( vertex );
                    m_linesIndexData.append( m_drawIndices[ 0 ] );
                    ++m_drawIndices[ 0 ];
                    m_linesIndexData.append( m_drawIndices[ 0 ] );
                    m_dirty = true; 
                }
                break;
            }
            default:
            {
                break;
            }
        }
        ++m_drawCount;
    }
}

void CVectorDrawing::drawText( const short x, const short y, const char* const text, const uint32_t colour )
{
    if( valid() )
    {
        m_drawMode = EModeDefault;
        m_vtext.SetOrigin( static_cast< int >( x ), static_cast< int >( y ) );
        m_offset = 0;
        m_colour = colour;
        m_vtext.Draw( *this, text );
        m_dirty = true; 
    }
}

void CVectorDrawing::drawLine( const short x1, const short y1, const short x2, const short y2, const uint32_t colour )
{
    if( valid() )
    {
        m_drawMode = EModeDefault;
        uint16_t index = static_cast< uint16_t >( m_linesVertexData.getCount() );
        DRAW2D_VERTEX vertex;
        vertex.colour = colour;
        vertex.position.x = static_cast< int16_t >( x1 );
        vertex.position.y = static_cast< int16_t >( y1 );
        m_linesVertexData.append( vertex );
        vertex.position.x = static_cast< int16_t >( x2 );
        vertex.position.y = static_cast< int16_t >( y2 );
        m_linesVertexData.append( vertex );
        m_linesIndexData.append( index );
        m_linesIndexData.append( index + 1 );
        m_dirty = true; 
    }
}

void CVectorDrawing::drawLine( const short x1, const short y1, const short x2, const short y2, const uint32_t colour1, const uint32_t colour2 )
{
    if( valid() )
    {
        m_drawMode = EModeDefault;
        uint16_t index = static_cast< uint16_t >( m_linesVertexData.getCount() );
        DRAW2D_VERTEX vertex;
        vertex.colour = colour1;
        vertex.position.x = static_cast< int16_t >( x1 );
        vertex.position.y = static_cast< int16_t >( y1 );
        m_linesVertexData.append( vertex );
        vertex.colour = colour2;
        vertex.position.x = static_cast< int16_t >( x2 );
        vertex.position.y = static_cast< int16_t >( y2 );
        m_linesVertexData.append( vertex );
        m_linesIndexData.append( index );
        m_linesIndexData.append( index + 1 );
        m_dirty = true; 
    }
}

void CVectorDrawing::drawRect( const short left, const short top, const short right, const short bottom, const uint32_t colour )
{
    if( valid() )
    {
        m_drawMode = EModeDefault;
        uint16_t index = static_cast< uint16_t >( m_linesVertexData.getCount() );
        DRAW2D_VERTEX vertex;
        vertex.colour = colour;
        vertex.position.x = static_cast< int16_t >( left );
        vertex.position.y = static_cast< int16_t >( top );
        m_linesVertexData.append( vertex );
        if( ( left == right ) || ( top == bottom ) )
        {   //  point or line
            if( ( left != right ) || ( top != bottom ) )
            {   //  line
                vertex.position.x = static_cast< int16_t >( right );
                vertex.position.y = static_cast< int16_t >( bottom );
                m_linesVertexData.append( vertex );
                m_linesIndexData.append( index );
                m_linesIndexData.append( index + 1 );
            }
        }
        else
        {   //  rectangle
            vertex.position.x = static_cast< int16_t >( right );
            vertex.position.y = static_cast< int16_t >( top );
            m_linesVertexData.append( vertex );
            vertex.position.x = static_cast< int16_t >( right );
            vertex.position.y = static_cast< int16_t >( bottom );
            m_linesVertexData.append( vertex );
            vertex.position.x = static_cast< int16_t >( left );
            vertex.position.y = static_cast< int16_t >( bottom );
            m_linesVertexData.append( vertex );
            m_linesIndexData.append( index );
            m_linesIndexData.append( index + 1 );
            m_linesIndexData.append( index + 1 );
            m_linesIndexData.append( index + 2 );
            m_linesIndexData.append( index + 2 );
            m_linesIndexData.append( index + 3 );
            m_linesIndexData.append( index + 3 );
            m_linesIndexData.append( index );
        }
        m_dirty = true; 
    }
}

void CVectorDrawing::drawQuad( const short left, const short top, const short right, const short bottom, const uint32_t colour )
{
    if( valid() )
    {
        m_drawMode = EModeDefault;
        uint16_t index = static_cast< uint16_t >( m_trisVertexData.getCount() );
        DRAW2D_VERTEX vertex;
        vertex.colour = colour;
        vertex.position.x = static_cast< int16_t >( left );
        vertex.position.y = static_cast< int16_t >( top );
        m_trisVertexData.append( vertex );
        vertex.position.x = static_cast< int16_t >( right + 1 );
        vertex.position.y = static_cast< int16_t >( top );
        m_trisVertexData.append( vertex );
        vertex.position.x = static_cast< int16_t >( left );
        vertex.position.y = static_cast< int16_t >( bottom + 1 );
        m_trisVertexData.append( vertex );
        vertex.position.x = static_cast< int16_t >( right + 1 );
        vertex.position.y = static_cast< int16_t >( bottom + 1 );
        m_trisVertexData.append( vertex );
        m_trisIndexData.append( index );
        m_trisIndexData.append( index + 1 );
        m_trisIndexData.append( index + 2 );
        m_trisIndexData.append( index + 1 );
        m_trisIndexData.append( index + 2 );
        m_trisIndexData.append( index + 3 );
        m_dirty = true; 
    }
}

void CVectorDrawing::setFont( const vtext::VFONT font )
{
    if( valid() )
    {
        m_vtext.SetFont( font );
        m_vtext.GetStepping( m_stepping.width, m_stepping.height );
        m_vtext.GetSpacing( m_spacing.width, m_spacing.height );
    }
}

void CVectorDrawing::setFont( const vtext::VFONT font, const unsigned int scale )
{
    if( valid() )
    {
        m_vtext.SetFont( font, scale );
        m_vtext.GetStepping( m_stepping.width, m_stepping.height );
        m_vtext.GetSpacing( m_spacing.width, m_spacing.height );
    }
}

void CVectorDrawing::setFont( const vtext::VFONT font, const unsigned int scale, const bool descenders )
{
    if( valid() )
    {
        m_vtext.SetFont( font, scale, descenders );
        m_vtext.GetStepping( m_stepping.width, m_stepping.height );
        m_vtext.GetSpacing( m_spacing.width, m_spacing.height );
    }
}
       
char CVectorDrawing::Item( const char character, const char code )
{   //  character translation and effects
    UNUSED( code );
    return( character );
}

void CVectorDrawing::Draw( const vtext::Glyph& glyph, const int x, const int y, const unsigned int scale )
{
    m_offset = static_cast< uint32_t >( m_linesVertexData.getCount() );
    glyph.Draw( *this, x, y, scale );
}

void CVectorDrawing::AddVert( const int x, const int y )
{
    DRAW2D_VERTEX vertex;
    vertex.colour = m_colour;
    vertex.position.x = static_cast< int16_t >( x );
    vertex.position.y = static_cast< int16_t >( y );
    m_linesVertexData.append( vertex );
}

void CVectorDrawing::AddLine( const unsigned int i1, const unsigned int i2 )
{
    m_linesIndexData.append( static_cast< uint16_t >( i1 + m_offset ) );
    m_linesIndexData.append( static_cast< uint16_t >( i2 + m_offset ) );
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

