
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   buffer.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	A simple static memory append buffer.
////
////    Notes:
////
////        The append() and insert() functions return a pointer to the data within the buffer or NULL on failure.
////        The align() function only aligns the buffer relative write location.
////
////    IMPORTANT:
////
////        This class is primarily intended for use with POD data types.
////        Exercise caution when appending or inserting data with complex types.
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

#include "buffer.h"
#include "libs/system/debug/asserts.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    byte data append buffer object class functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Buffer::align( const size_t alignment )
{
    if( alignment > 1 )
    {
        size_t size = ( ( ( m_used + alignment - 1 ) / alignment ) * alignment );
        if( size > m_size )
        {
            return( false );
        }
        if( size > m_used )
        {
            memset( &m_buffer[ m_used ], 0, ( size - m_used ) );
            m_used = size;
        }
    }
    return( true );
}

void Buffer::shrink( const size_t bytes )
{
    ASSERT( ( bytes != 0 ) && ( bytes <= m_used ) );
    m_used = ( ( bytes < m_used ) ? ( m_used - bytes ) : 0 );
}

uint8_t* Buffer::append( const size_t bytes, const bool clear )
{
    ASSERT( bytes != 0 );
    if( bytes <= ( m_size - m_used ) )
    {
        uint8_t* memory = &m_buffer[ m_used ];
        if( bytes )
        {
            if( clear )
            {
                memset( memory, 0, bytes );
            }
            m_used += bytes;
        }
        return( memory );
    }
    return( NULL );
}

uint8_t* Buffer::append( const void* const data, const size_t bytes )
{
    ASSERT( ( bytes != 0 ) && ( data != NULL ) );
    if( bytes <= ( m_size - m_used ) )
    {
        uint8_t* memory = &m_buffer[ m_used ];
        if( bytes )
        {
            if( data == NULL )
            {
                memset( memory, 0, bytes );
            }
            else
            {
                memcpy_s( memory, ( m_size - m_used ), data, bytes );
            }
            m_used += bytes;
        }
        return( memory );
    }
    return( NULL );
}

uint8_t* Buffer::insert( const size_t offset, const size_t bytes )
{
    ASSERT( bytes != 0 );
    if( bytes )
    {
        size_t size = ( ( ( m_used > offset ) ? m_used : offset ) + bytes );
        ASSERT( size <= m_size );
        if( size <= m_size )
        {
            uint8_t* memory = &m_buffer[ offset ];
            if( m_used > offset )
            {   //  shift for insert
                memmove( &memory[ bytes ], memory, ( m_used - offset ) );
            }
            memset( memory, 0, bytes );
            m_used = size;
            return( memory );
        }
    }
    return( NULL );
}

uint8_t* Buffer::insert( const size_t offset, const void* const data, const size_t bytes )
{
    ASSERT( ( bytes != 0 ) && ( data != NULL ) );
    if( bytes )
    {
        size_t size = ( ( ( m_used > offset ) ? m_used : offset ) + bytes );
        ASSERT( size <= m_size );
        if( size <= m_size )
        {
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
            return( memory );
        }
    }
    return( NULL );
}

void Buffer::remove( const size_t offset, const size_t bytes )
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

uint8_t* Buffer::appendString( const uint8_t* const string )
{
    size_t length = 0;
    if( string != NULL ) while( string[ length ] ) ++length;
    return( appendString( string, length ) );
}

uint8_t* Buffer::appendString( const uint8_t* const string, const size_t length )
{
    if( length < ( m_size - m_used ) )
    {
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
        return( memory );
    }
    return( NULL );
}

uint8_t* Buffer::insertString( const size_t offset, const uint8_t* const string )
{
    size_t length = 0;
    if( string != NULL ) while( string[ length ] ) ++length;
    return( insertString( offset, string, length ) );
}

uint8_t* Buffer::insertString( const size_t offset, const uint8_t* const string, const size_t length )
{
    size_t size = ( ( ( m_used > offset ) ? m_used : offset ) + length + 1 );
    ASSERT( size <= m_size );
    if( size <= m_size )
    {
        uint8_t* memory = &m_buffer[ offset ];
        if( m_used > offset )
        {   //  shift for insert
            memmove( &memory[ length + 1 ], memory, ( m_used - offset ) );
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
        return( memory );
    }
    return( NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



