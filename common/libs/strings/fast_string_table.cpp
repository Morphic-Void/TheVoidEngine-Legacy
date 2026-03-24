
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   fast_string_table.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	A general string table for allocation and querying of a large number of strings.
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

#include "fast_string_table.h"
#include <new.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    FastStringTable class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FastStringTable::init( const uint size )
{
    NBuffer::init( size );
    uint count = ( ( size >> 4 ) + 1 );
    m_handles.init( count );
    m_tree.kill();
    addString( "", 0 );
}

void FastStringTable::kill()
{
    NBuffer::kill();
    m_handles.kill();
    m_tree.kill();
}

void FastStringTable::reset()
{
    NBuffer::reset();
    m_handles.reset();
    m_tree.kill();
}

FastStringTable::Handle FastStringTable::addString( const uint8_t* const string )
{
    uint length = 0;
    if( string ) while( string[ length ] ) ++length;
    return( addString( string, length ) );
}

FastStringTable::Handle FastStringTable::addString( const uint8_t* const string, const uint length )
{
    sorting::avl_blob::Node* node = m_tree.find( string, length );
    if( node == NULL )
    {
        node = &m_handles.fetch();
        new( node ) sorting::avl_blob::Node( NBuffer::appendString( string, length ), length );
        m_tree.insert( node );
    }
    return( encodeHandle( node ) );
}

FastStringTable::Handle FastStringTable::getStringHandle( const int index ) const
{
    const sorting::avl_blob::Node* scan = m_tree.first();
    for( int count = index; count; --count )
    {
        scan = scan->next();
    }
    return( encodeHandle( scan ) );
}

const uint8_t* FastStringTable::getString( const Handle handle ) const
{
    const sorting::avl_blob::Node* node = decodeHandle( handle );
#ifdef _DEBUG
    if( m_tree.contains( node ) )
#else
    if( node )
#endif
    {
        return( node->blob().data );
    }
    return( NULL );
}

const uint8_t* FastStringTable::getString( const Handle handle, uint& length ) const
{
    const sorting::avl_blob::Node* node = decodeHandle( handle );
#ifdef _DEBUG
    if( m_tree.contains( node ) )
#else
    if( node )
#endif
    {
        const sorting::avl_blob::Blob& blob = node->blob();
        length = static_cast< uint >( blob.size );
        return( blob.data );
    }
    length = 0;
    return( NULL );
}

uint FastStringTable::getStringLength( const Handle handle ) const
{
    const sorting::avl_blob::Node* node = decodeHandle( handle );
#ifdef _DEBUG
    if( m_tree.contains( node ) )
#else
    if( node )
#endif
    {
        return( static_cast< uint >( node->blob().size ) );
    }
    return( 0 );
}

int FastStringTable::getStringIndex( const Handle handle ) const
{
    const sorting::avl_blob::Node* node = decodeHandle( handle );
    const sorting::avl_blob::Node* scan = m_tree.first();
    for( int index = 0; scan; ++index )
    {
        if( scan == node )
        {
            return( index );
        }
        scan = scan->next();
    }
    return( -1 );
}

const uint8_t* FastStringTable::getString( const int index ) const
{
    return( getString( getStringHandle( index ) ) );
}

const uint8_t* FastStringTable::getString( const int index, uint& length ) const
{
    return( getString( getStringHandle( index ), length ) );
}

uint FastStringTable::getStringLength( const int index ) const
{
    return( getStringLength( getStringHandle( index ) ) );
}

FastStringTable::Handle FastStringTable::findString( const uint8_t* const string ) const
{
    uint length = 0;
    if( string ) while( string[ length ] ) ++length;
    return( findString( string, length ) );
}

FastStringTable::Handle FastStringTable::findString( const uint8_t* const string, const uint length ) const
{
    return( encodeHandle( m_tree.find( string, length ) ) );
}

uint FastStringTable::getFrozenSize() const
{   //  4 bytes for the count, 8 bytes for each string, N bytes of character data rounded up to a multiple of 4
    return( 4 + ( getCount() << 3 ) + ( ( getBytes() + 3 ) & ~3 ) );
}

FrozenStringTable* FastStringTable::freeze() const
{
    uint size = 0;
    return( reinterpret_cast< FrozenStringTable* >( freeze( size ) ) );
}

void* FastStringTable::freeze( uint& size ) const
{
    uint buffer_size = getFrozenSize();
    void* buffer = reinterpret_cast< void* >( new uint8_t[ buffer_size ] );
    size = freeze( buffer, buffer_size );
    return( buffer );
}

uint FastStringTable::freeze( void* const buffer, const uint buffer_size ) const
{
    uint size = getFrozenSize();
    if( buffer_size >= size )
    {
        uint count = getCount();
        uint32_t* strings = reinterpret_cast< uint32_t* >( buffer );
        uint8_t* bytes = reinterpret_cast< uint8_t* >( &strings[ ( count << 1 ) + 1 ] );
        strings[ 0 ] = count;
        ++strings;
        uint32_t offset = 0;
        const sorting::avl_blob::Node* node = m_tree.first();
        for( uint index = 0; index < count; ++index )
        {
            const sorting::avl_blob::Blob& blob = node->blob();
            uint32_t length = static_cast< uint32_t >( blob.size );
            strings[ 0 ] = offset;
            strings[ 1 ] = length;
            strings += 2;
            ++length;
            memcpy_s( reinterpret_cast< void* >( &bytes[ offset ] ), length, reinterpret_cast< const void* >( blob.data ), length );
            offset += length;
            node = node->next();
        }
        while( offset < size )
        {
            bytes[ offset ] = 0;
            ++offset;
        }
        return( size );
    }
    return( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



