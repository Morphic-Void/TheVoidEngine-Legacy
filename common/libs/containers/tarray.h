
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   tarray.h
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

#ifndef	__TARRAY_INCLUDED__
#define	__TARRAY_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/system/debug/asserts.h"
#include <memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    typed data blob object class template
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
template< typename T >
class TArray
{
public:
    inline                  TArray() : m_used( 0 ), m_size( 0 ), m_buffer( NULL ) {};
    inline                  TArray( TArray&& take_me ) { take_obj( take_me ); };
    inline                  TArray( const TArray& clone_me ) { clone_obj( clone_me ); };
    inline                  TArray( const size_t size ) : m_used( 0 ), m_size( 0 ), m_buffer( NULL ) { init( size ); };
    inline                  ~TArray() { kill(); };
    inline TArray&          operator =( TArray&& take_me ) { take( take_me ); return( *this ); };
    inline TArray&          operator =( const TArray& clone_me ) { clone( clone_me ); return( *this ); };
    inline T&               operator []( const size_t index );
    inline const T&         operator []( const size_t index ) const;
    inline bool             isValid() const { return( ( ( m_buffer != NULL ) && ( m_size > 0 ) && ( m_used <= m_size ) ) ? true : false ); };
    void                    init( const size_t size = 32 );
    void                    kill();
    inline void             take( TArray& take_me ) { kill(); take_obj( take_me ); };
    inline void             clone( const TArray& clone_me ) { kill(); clone_obj( clone_me ); };
    inline void             reset() { m_used = 0; };
    T&                      allocate();
    T*                      allocate( const size_t count );
    void                    shrink( const size_t count = 1 );
    size_t                  append( const T* const data, const size_t count = 1 );
    inline size_t           append( const T& data ) { return( append( &data ) ); };
    void                    insert( const size_t index, const T* const data, const size_t count = 1 );
    inline void             insert( const size_t index, const T& data ) { insert( index, &data ); };
    void                    remove( const size_t index, const size_t count );
    void                    setItem( const size_t index, const T& item );
    inline T&               getItem( const size_t index );
    inline const T&         getItem( const size_t index ) const;
    inline size_t           getItemSize() const { return( sizeof( T ) ); };
    inline size_t           getUsed() const { return( m_used ); };
    inline size_t           getSize() const { return( m_size ); };
    inline T*               getBuffer() { return( m_data ); };
    inline const T*         getBuffer() const { return( m_data ); };
protected:
    void                    grow( const size_t size );
    inline void             take_obj( TArray& take_me ) { m_used = take_me.m_used; m_size = take_me.m_size; m_buffer = take_me.m_buffer; take_me.m_used = take_me.m_size = 0; take_me.m_buffer = NULL; };
    void                    clone_obj( const TArray& clone_me );
    size_t                  m_used;
    size_t                  m_size;
    T*                      m_buffer;
};
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    typed data blob object class template functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

template< typename T >
void TArray< T >::init( const size_t size )
{
    kill();
    m_used = 0;
    m_size = size;
    if( m_size ) 
    {
        m_buffer = new T[ m_size ];
    }
}

template< typename T >
void TArray< T >::kill()
{
    m_used = m_size = 0;
    if( m_buffer != NULL )
    {
        delete[] m_buffer;
        m_buffer = NULL;
    }
}

template< typename T >
void TArray< T >::clone_obj( const TArray< T >& clone_me )
{
    if( clone_me.isValid() )
    {
        m_used = clone_me.m_used;
        m_size = clone_me.m_size;
        m_buffer = new T[ m_size ];
        for( size_t index = 0; index < m_size; ++index )
        {
            m_buffer[ index ] = clone_me.m_buffer[ index ];
        }
    }
    else
    {
        m_used = m_size = 0;
        m_buffer = NULL;
    }
}

template< typename T >
T& TArray< T >::allocate()
{
    size_t offset = m_used;
    size_t size = ( m_used + 1 );
    grow( size );
    m_used = size;
    return( m_data[ offset ] );
}

template< typename T >
T* TArray< T >::allocate( const size_t count )
{
    ASSERT( count != 0 );
    size_t offset = m_used;
    if( count )
    {
        size_t size = ( m_used + count );
        grow( size );
        m_used = size;
    }
    return( &m_data[ offset ] );
}

template< typename T >
void TArray< T >::shrink( const size_t count )
{
    ASSERT( count <= m_used );
    m_used = ( ( m_used <= count ) ? 0 : ( m_used - count ) );
};

*/

//template< typename T >
//size_t TArray< T >::append( const T* const data, const size_t count )
//{
//    ASSERT( count != 0 );
//    size_t offset = m_used;
//    if( count )
//    {
//        size_t size = ( m_used + count );
//        grow( size );
//        if( data )
//        {
//            if( count > 1 )
//            {
//                memcpy_s( reinterpret_cast< void* >( &m_data[ m_used ] ), ( ( m_size - m_used ) * sizeof( T ) ), data, ( count * sizeof( T ) ) );
//            }
//            else
//            {
//                m_data[ m_used ] = *data;
//            }
//        }
//        m_used = size;
//    }
//    return( offset );
//}
//
//template< typename T >
//void TBlob< T >::insert( const size_t index, const T* const data, const size_t count )
//{
//    ASSERT( count != 0 );
//    if( count )
//    {
//        uint size = ( ( ( m_used > index ) ? m_used : index ) + count );
//        grow( size );
//        if( m_used > index )
//        {   //  shift for insert
//            memmove( reinterpret_cast< void* >( &m_data[ index + count ] ), reinterpret_cast< void* >( &m_data[ index ] ), ( ( m_used - index ) * sizeof( T ) ) );
//        }
//        if( data )
//        {
//            if( count > 1 )
//            {
//                memcpy_s( reinterpret_cast< void* >( &m_data[ index ] ), ( count * sizeof( T ) ), data, ( count * sizeof( T ) ) );
//            }
//            else
//            {
//                m_data[ index ] = *data;
//            }
//        }
//        m_used = size;
//    }
//}
//
//template< typename T >
//void TBlob< T >::remove( const uint size_t, const size_t count )
//{
//    ASSERT( ( count != 0 ) && ( index < m_used ) && ( count <= ( m_used - index ) ) );
//    if( count )
//    {
//        if( index < m_used )
//        {
//            if( count >= ( m_used - index ) )
//            {
//                m_used = index;
//            }
//            else
//            {
//                memmove( reinterpret_cast< void* >( &m_data[ index ] ), reinterpret_cast< void* >( &m_data[ index + count ] ), ( ( m_used - ( index + count ) ) * sizeof( T ) ) );
//                m_used -= count;
//            }
//        }
//    }
//}
//
//template< typename T >
//void TBlob< T >::setItem( const size_t index, const T& item )
//{
//    if( index >= m_used )
//    {
//        m_used = ( index + 1 );
//        grow( m_used );
//    }
//    m_data[ index ] = item;
//}
//
//template< typename T >
//T& TBlob< T >::getItem( const size_t index )
//{
//    ASSERT( index < m_used );
//    return( m_data[ index ] );
//}
//
//template< typename T >
//const T& TBlob< T >::getItem( const size_t index ) const
//{
//    ASSERT( index < m_used );
//    return( m_data[ index ] );
//}
//
//template< typename T >
//void TBlob< T >::grow( const size_t size )
//{
//    if( m_size < size )
//    {
//        while( m_size < size )
//        {
//            m_size += ( ( m_size >> 1 ) + 1 );
//        }
//        T* data = m_data;
//        m_data = new T[ m_size ];
//        if( data )
//        {
//            memcpy_s( reinterpret_cast< void* >( m_data ), ( m_size * sizeof( T ) ), reinterpret_cast< const void* >( data ), ( m_used * sizeof( T ) ) );
//            delete[] data;
//        }
//    }
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__TARRAY_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

