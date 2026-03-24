
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   widget.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Wrapper class for libs/utils/widgets.h
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

#include "widget.h"
#include "libs/system/debug/asserts.h"

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
////    Widget buffer classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetIndices : public widgets::IIB
{
public:
    inline                          WidgetIndices() : m_indices() {};
    inline                          WidgetIndices( const uint count ) : m_indices( count ) {};
    inline                          ~WidgetIndices() { kill(); };
    inline void                     init( const uint count, const bool resize = false ) { m_indices.init( count, resize ); };
    inline void                     kill() { m_indices.kill(); };
    inline void                     reset() { m_indices.reset(); };
    inline uint                     getStride() const { return( m_indices.getStride() ); };
    inline uint                     getCount() const { return( m_indices.getCount() ); };
    inline uint                     getBytes() const { return( m_indices.getBytes() ); };
    inline uint16_t*                getData() { return( m_indices.getBuffer() ); };
    inline const uint16_t*          getData() const { return( m_indices.getBuffer() ); };
protected:
    virtual void                    Set( const uint32_t index );
    IndexStream16                   m_indices;
private:
    inline                          WidgetIndices( const WidgetIndices& ) {};
    inline void                     operator=( const WidgetIndices& ) {};
};

class WidgetVertices : public widgets::IVB
{
public:
    inline                          WidgetVertices() : m_vertices() {};
    inline                          WidgetVertices( const uint count ) : m_vertices( count ) {};
    inline                          ~WidgetVertices() { kill(); };
    inline void                     init( const uint count, const bool resize = false ) { m_vertices.init( count, resize ); };
    inline void                     kill() { m_vertices.kill(); };
    inline void                     reset() { m_vertices.reset(); };
    inline uint                     getStride() const { return( m_vertices.getStride() ); };
    inline uint                     getCount() const { return( m_vertices.getCount() ); };
    inline uint                     getBytes() const { return( m_vertices.getBytes() ); };
    inline SIMPLE_VERTEX*           getData() { return( m_vertices.getBuffer() ); };
    inline const SIMPLE_VERTEX*     getData() const { return( m_vertices.getBuffer() ); };
protected:
    virtual void                    Set( const float px, const float py, const float pz, const float nx, const float ny, const float nz, const uint32_t colour );
    VertexStreamSimple              m_vertices;
private:
    inline                          WidgetVertices( const WidgetVertices& ) {};
    inline void                     operator=( const WidgetVertices& ) {};
};

class WidgetNVertices : public widgets::IVB
{
public:
    inline                          WidgetNVertices() : m_vertices() {};
    inline                          WidgetNVertices( const uint count ) : m_vertices( count ) {};
    inline                          ~WidgetNVertices() { kill(); };
    inline void                     init( const uint count, const bool resize = false ) { m_vertices.init( count, resize ); };
    inline void                     kill() { m_vertices.kill(); };
    inline void                     reset() { m_vertices.reset(); };
    inline uint                     getStride() const { return( m_vertices.getStride() ); };
    inline uint                     getCount() const { return( m_vertices.getCount() ); };
    inline uint                     getBytes() const { return( m_vertices.getBytes() ); };
    inline SIMPLE_LIT_VERTEX*       getData() { return( m_vertices.getBuffer() ); };
    inline const SIMPLE_LIT_VERTEX* getData() const { return( m_vertices.getBuffer() ); };
protected:
    virtual void                    Set( const float px, const float py, const float pz, const float nx, const float ny, const float nz, const uint32_t colour );
    VertexStreamSimpleLit           m_vertices;
private:
    inline                          WidgetNVertices( const WidgetNVertices& ) {};
    inline void                     operator=( const WidgetNVertices& ) {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Widget buffer classes function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WidgetIndices::Set( const uint32_t index )
{
    m_indices.append( static_cast< uint16_t >( index ) );
}

void WidgetVertices::Set( const float px, const float py, const float pz, const float nx, const float ny, const float nz, const uint32_t colour )
{
    UNUSED( nx );
    UNUSED( ny );
    UNUSED( nz );
    SIMPLE_VERTEX vertex;
    vertex.position.x = px;
    vertex.position.y = py;
    vertex.position.z = pz;
    vertex.position.w = 1.0f;
    vertex.colour = colour;
    m_vertices.append( vertex );
}

void WidgetNVertices::Set( const float px, const float py, const float pz, const float nx, const float ny, const float nz, const uint32_t colour )
{
    SIMPLE_LIT_VERTEX vertex;
    vertex.position.x = px;
    vertex.position.y = py;
    vertex.position.z = pz;
    vertex.position.w = 1.0f;
    vertex.normal.x = nx;
    vertex.normal.y = ny;
    vertex.normal.z = nz;
    vertex.normal.w = 0.0f;
    vertex.colour = colour;
    m_vertices.append( vertex );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Widget class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Widget::create( const CoreSystem& system, const widgets::IWidget& widget, const bool use_normals )
{
    kill();
    if( init( system, widget, use_normals ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void Widget::destroy()
{
    kill();
}

bool Widget::init( const CoreSystem& system, const widgets::IWidget& widget, const bool use_normals )
{
    PrimDesc primDesc;
    IndexedParams indexed;
    if( use_normals )
    {
        StringItem name( "simple_lit" );
        if( !system.shaders.create( name, m_shader ) || !system.shaders.create( name, m_vertexBinding ) )
        {
            return( false );
        }
        WidgetNVertices vertices( widget.GetVertices() );
        widget.GetVertices( vertices );
        m_vertexBuffer.create( vertices.getCount(), vertices.getStride(), vertices.getData() );
    }
    else
    {
        StringItem name( "simple" );
        if( !system.shaders.create( name, m_shader ) || !system.shaders.create( name, m_vertexBinding ) )
        {
            return( false );
        }
        WidgetVertices vertices( widget.GetVertices() );
        widget.GetVertices( vertices );
        m_vertexBuffer.create( vertices.getCount(), vertices.getStride(), vertices.getData() );
    }
    m_shader.bind( system.defaults );
    if( !system.factory.create( m_vertexResource ) || !system.factory.create( m_indexResource ) || !system.factory.create( m_command ) )
    {
        return( false );
    }
    if( !m_shader.bind( m_vertexBinding ) || !m_vertexBinding.bind( m_vertexResource ) )
    {
        return( false );
    }
    m_vertexResource.setStream( reinterpret_cast< void* >( m_vertexBuffer.getBuffer() ) );
    m_vertexResource.setOffset( 0 );
    m_vertexResource.setStride( m_vertexBuffer.getStride() );
    WidgetIndices indices( widget.GetIndices() );
    widget.GetIndices( indices );
    m_indexBuffer.create( indices.getCount(), indices.getStride(), indices.getData() );
    m_indexResource.setIndices( reinterpret_cast< void* >( m_indexBuffer.getBuffer() ) );
    m_indexResource.setOffset( 0 );
    m_indexResource.setStride( m_indexBuffer.getStride() );
    memset( &primDesc, 0, sizeof( primDesc ) );
    primDesc.primType = ( widget.IsFilled() ? EPrimTypeTriList : EPrimTypeLineList );
    m_command.setPrim( primDesc );
    memset( &indexed, 0, sizeof( indexed ) );
    indexed.indexCount = static_cast< UINT >( indices.getCount() );
    m_command.setDraw( indexed, m_indexResource );
    return( true );
}

void Widget::kill()
{
    if( this )
    {
        m_shader.release();
        m_vertexBinding.release();
        m_vertexResource.release();
        m_vertexBuffer.destroy();
        m_indexResource.release();
        m_indexBuffer.destroy();
        m_command.release();
    }
}

void Widget::render()
{
    if( valid() )
    {
        if( m_shader.update() )
        {
            if( m_shader.submit() )
            {
                m_shader.execute( m_command );
            }
        }
    }
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

