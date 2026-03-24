
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   tblob.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	A growable blob of typed data.
////
////    IMPORTANT NOTES:
////
////        This template is intended for use with POD data types.
////        Exercise caution when used with complex classes.
////
////        Blob contents may be relocated, so caution must be exercised in taking pointers to items in the blob.
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

#ifndef	__TBLOB_INCLUDED__
#define	__TBLOB_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/system/debug/asserts.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    typed data blob object class template
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename T >
class TBlob
{
public:
    inline                  TBlob() : m_size( 0 ), m_used( 0 ), m_data( NULL ) {};
    inline                  TBlob( const uint size ) : m_size( 0 ), m_used( 0 ), m_data( NULL ) { init( size ); };
    inline                  ~TBlob() { kill(); };
    void                    init( const uint size = 32 );
    void                    kill();
    inline void             reset() { m_used = 0; };
    void                    clone( const TBlob< T >& source );
    T&                      fetch();
    T*                      fetch( const uint count );
    void                    shrink( const uint count = 1 );
    uint                    append( const T* const data, const uint count = 1 );
    inline uint             append( const T& data ) { return( append( &data ) ); };
    void                    insert( const uint index, const T* const data, const uint count = 1 );
    inline void             insert( const uint index, const T& data ) { insert( index, &data ); };
    void                    remove( const uint index, const uint count );
    void                    setItem( const uint index, const T& item );
    inline T&               getItem( const uint index );
    inline const T&         getItem( const uint index ) const;
    inline uint             getItemSize() const { return( static_cast< uint >( sizeof( T ) ) ); };
    inline uint             getSize() const { return( m_size ); };
    inline uint             getUsed() const { return( m_used ); };
    inline T*               getBuffer() { return( m_data ); };
    inline const T*         getBuffer() const { return( m_data ); };
protected:
    void                    grow( const uint size );
    uint                    m_size;
    uint                    m_used;
    T*                      m_data;
private:
    inline                  TBlob( const TBlob& ) {};
    inline void             operator=( const TBlob& ) {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    typed data blob object class template functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename T >
void TBlob< T >::init( const uint size )
{
    kill();
    m_size = size;
    m_used = 0;
    m_data = ( size ? new T[ m_size ] : NULL );
}

template< typename T >
void TBlob< T >::kill()
{
    m_size = 0;
    m_used = 0;
    if( m_data != NULL )
    {
        delete[] m_data;
        m_data = NULL;
    }
}

template< typename T >
void TBlob< T >::clone( const TBlob< T >& source )
{
    kill();
    m_size = source.m_size;
    m_used = source.m_used;
    if( m_size != 0 )
    {
        m_data = new T[ m_size ];
        memcpy( m_data, source.m_data, ( m_size * sizeof( T ) ) );
    }
}

template< typename T >
T& TBlob< T >::fetch()
{
    uint offset = m_used;
    uint size = ( m_used + 1 );
    grow( size );
    memset( reinterpret_cast< void* >( &m_data[ m_used ] ), 0, sizeof( T ) );
    m_used = size;
    return( m_data[ offset ] );
}

template< typename T >
T* TBlob< T >::fetch( const uint count )
{
    ASSERT( count != 0 );
    uint offset = m_used;
    if( count )
    {
        uint size = ( m_used + count );
        grow( size );
        m_used = size;
    }
    return( &m_data[ offset ] );
}

template< typename T >
void TBlob< T >::shrink( const uint count )
{
    ASSERT( count <= m_used );
    m_used = ( ( m_used <= count ) ? 0 : ( m_used - count ) );
};

template< typename T >
uint TBlob< T >::append( const T* const data, const uint count )
{
    ASSERT( count != 0 );
    uint offset = m_used;
    if( count )
    {
        uint size = ( m_used + count );
        grow( size );
        if( data )
        {
            if( count > 1 )
            {
                memcpy_s( reinterpret_cast< void* >( &m_data[ m_used ] ), ( ( m_size - m_used ) * sizeof( T ) ), data, ( count * sizeof( T ) ) );
            }
            else
            {
                m_data[ m_used ] = *data;
            }
        }
        m_used = size;
    }
    return( offset );
}

template< typename T >
void TBlob< T >::insert( const uint index, const T* const data, const uint count )
{
    ASSERT( count != 0 );
    if( count )
    {
        uint size = ( ( ( m_used > index ) ? m_used : index ) + count );
        grow( size );
        if( m_used > index )
        {   //  shift for insert
            memmove( reinterpret_cast< void* >( &m_data[ index + count ] ), reinterpret_cast< void* >( &m_data[ index ] ), ( ( m_used - index ) * sizeof( T ) ) );
        }
        if( data )
        {
            if( count > 1 )
            {
                memcpy_s( reinterpret_cast< void* >( &m_data[ index ] ), ( count * sizeof( T ) ), data, ( count * sizeof( T ) ) );
            }
            else
            {
                m_data[ index ] = *data;
            }
        }
        m_used = size;
    }
}

template< typename T >
void TBlob< T >::remove( const uint index, const uint count )
{
    ASSERT( ( count != 0 ) && ( index < m_used ) && ( count <= ( m_used - index ) ) );
    if( count )
    {
        if( index < m_used )
        {
            if( count >= ( m_used - index ) )
            {
                m_used = index;
            }
            else
            {
                memmove( reinterpret_cast< void* >( &m_data[ index ] ), reinterpret_cast< void* >( &m_data[ index + count ] ), ( ( m_used - ( index + count ) ) * sizeof( T ) ) );
                m_used -= count;
            }
        }
    }
}

template< typename T >
void TBlob< T >::setItem( const uint index, const T& item )
{
    if( index >= m_used )
    {
        m_used = ( index + 1 );
        grow( m_used );
    }
    m_data[ index ] = item;
}

template< typename T >
T& TBlob< T >::getItem( const uint index )
{
    ASSERT( index < m_used );
    return( m_data[ index ] );
}

template< typename T >
const T& TBlob< T >::getItem( const uint index ) const
{
    ASSERT( index < m_used );
    return( m_data[ index ] );
}

template< typename T >
void TBlob< T >::grow( const uint size )
{
    if( m_size < size )
    {
        while( m_size < size )
        {
            m_size += ( ( m_size >> 1 ) + 1 );
        }
        T* data = m_data;
        m_data = new T[ m_size ];
        if( data )
        {
            memcpy_s( reinterpret_cast< void* >( m_data ), ( m_size * sizeof( T ) ), reinterpret_cast< const void* >( data ), ( m_used * sizeof( T ) ) );
            delete[] data;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__TBLOB_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

