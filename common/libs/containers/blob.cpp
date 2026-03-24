
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   blob.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	A growable blob of byte data.
////
////    Notes:
////
////        The append() functions return the offset of the appended data within the blob.
////        The append() and insert() functions may cause the blob buffer to be relocated.
////        The align() function only aligns the buffer relative write location.
////
////    IMPORTANT:
////
////        This class is primarily intended for use with POD data types.
////        Exercise caution when appending or inserting data with complex types.
////        Exercise caution taking pointers to items in the blob due to the potential for relocation of the contents.
////        Alignment of the internal buffer is not guaranteed.
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

#include "blob.h"
#include "libs/system/debug/asserts.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    byte data blob object class functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Blob::align( const size_t alignment )
{
    if( alignment > 1 )
    {
        size_t size = ( ( ( m_used + alignment - 1 ) / alignment ) * alignment );
        if( size > m_used )
        {
            grow( size );
            m_used = size;
        }
    }
}

void Blob::shrink( const size_t bytes )
{
    ASSERT( ( bytes != 0 ) && ( bytes <= m_used ) );
    m_used = ( ( bytes < m_used ) ? ( m_used - bytes ) : 0 );
}

size_t Blob::append( const size_t bytes, const bool clear )
{
    ASSERT( bytes != 0 );
    size_t offset = m_used;
    if( bytes )
    {
        size_t size = ( m_used + bytes );
        grow( size );
        if( clear )
        {
            memset( &m_buffer[ m_used ], 0, bytes );
        }
        m_used = size;
    }
    return( offset );
}

size_t Blob::append( const void* const data, const size_t bytes )
{
    ASSERT( ( bytes != 0 ) && ( data != NULL ) );
    size_t offset = m_used;
    if( bytes )
    {
        size_t size = ( m_used + bytes );
        grow( size );
        uint8_t* memory = &m_buffer[ m_used ];
        if( data == NULL )
        {
            memset( memory, 0, bytes );
        }
        else
        {
            memcpy_s( memory, ( m_size - m_used ), data, bytes );
        }
        m_used = size;
    }
    return( offset );
}

void Blob::insert( const size_t offset, const size_t bytes )
{
    ASSERT( bytes != 0 );
    if( bytes )
    {
        size_t size = ( ( ( m_used > offset ) ? m_used : offset ) + bytes );
        grow( size );
        uint8_t* memory = &m_buffer[ offset ];
        if( m_used > offset )
        {   //  shift for insert
            memmove( &memory[ bytes ], memory, ( m_used - offset ) );
        }
        memset( memory, 0, bytes );
        m_used = size;
    }
}

void Blob::insert( const size_t offset, const void* const data, const size_t bytes )
{
    ASSERT( ( bytes != 0 ) && ( data != NULL ) );
    if( bytes )
    {
        size_t size = ( ( ( m_used > offset ) ? m_used : offset ) + bytes );
        grow( size );
        uint8_t* memory = &m_buffer[ offset ];
        if( m_used > offset )
        {   //  shift for insert
            memmove( &memory[ bytes ], memory, ( m_used - offset ) );
        }
        if( data == NULL )
        {
            memset( memory, 0, bytes );
        }
        else
        {
            memcpy_s( memory, bytes, data, bytes );
        }
        m_used = size;
    }
}

void Blob::remove( const size_t offset, const size_t bytes )
{
    ASSERT( ( bytes != 0 ) && ( offset < m_used ) && ( bytes <= ( m_used - offset ) ) );
    if( bytes )
    {
        if( offset < m_used )
        {
            uint8_t* memory = &m_buffer[ offset ];
            size_t after = ( m_used - offset );
            if( bytes >= after )
            {
                m_used = offset;
                memset( memory, 0, after );
            }
            else
            {
                memmove( memory, &memory[ bytes ], ( after - bytes ) );
                m_used -= bytes;
                memset( &m_buffer[ m_used ], 0, bytes );
            }
        }
    }
}

size_t Blob::appendString( const uint8_t* const string )
{
    size_t length = 0;
    if( string != NULL ) while( string[ length ] ) ++length;
    return( appendString( string, length ) );
}

size_t Blob::appendString( const uint8_t* const string, const size_t length )
{
    size_t offset = m_used;
    grow( m_used + length + 1 );
    uint8_t* memory = &m_buffer[ m_used ];
    if( length )
    {
        if( string == NULL )
        {
            memset( memory, 0, length );
        }
        else
        {
            memcpy_s( memory, ( m_size - m_used ), string, length );
        }
        m_used += length;
    }
    memory[ length ] = 0;
    ++m_used;
    return( offset );
}

void Blob::insertString( const size_t offset, const uint8_t* const string )
{
    size_t length = 0;
    if( string != NULL ) while( string[ length ] ) ++length;
    return( insertString( offset, string, length ) );
}

void Blob::insertString( const size_t offset, const uint8_t* const string, const size_t length )
{
    size_t size = ( ( ( m_used > offset ) ? m_used : offset ) + length + 1 );
    grow( size );
    uint8_t* memory = &m_buffer[ offset ];
    if( m_used > offset )
    {   //  shift for insert
        memmove( &m_buffer[ offset + length + 1 ], memory, ( m_used - offset ) );
    }
    if( length )
    {
        if( string == NULL )
        {
            memset( memory, 0, length );
        }
        else
        {
            memcpy_s( memory, length, string, length );
        }
    }
    memory[ length ] = 0;
    m_used = size;
}

void Blob::grow( const size_t size )
{
    if( m_size < size )
    {
        while( m_size < size ) m_size += ( ( m_size >> 1 ) + 1 );
        uint8_t* data = m_buffer;
        m_buffer = new uint8_t[ m_size ];
        if( data )
        {
            memcpy_s( m_buffer, m_size, data, m_used );
            memset( &m_buffer[ m_used ], 0, ( m_size - m_used ) );
            delete[] data;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



