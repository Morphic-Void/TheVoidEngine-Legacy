
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   string_table_blob.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	A simple string table for allocation and querying of a small number of strings.
////
////    IMPORTANT NOTE:
////
////        Blob contents may be relocated, so caution must be exercised in taking pointers to strings in the blob.
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

#include "string_table_blob.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    StringTableBlob class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StringTableBlob::init( const uint bytes, const uint count )
{
    m_bytes.init( ( bytes < 1024 ) ? 1024 : bytes );
    StringTableBlobBase::init( ( count < 32 ) ? 32 : count );
    OffsetAndLength item = { m_bytes.appendString( "", 0 ), 0 };
    insert( item );
}

void StringTableBlob::kill()
{
    m_bytes.kill();
    StringTableBlobBase::kill();
}

void StringTableBlob::reset()
{
    m_bytes.reset();
    StringTableBlobBase::reset();
}

uint StringTableBlob::addString( const uint8_t* const string )
{
    if( string )
    {
        uint length = 0;
        while( string[ length ] ) ++length;
        return( addString( string, length ) );
    }
    return( ~uint( 0 ) );   //  the handle ~0 always references the empty string
}

uint StringTableBlob::addString( const uint8_t* const string, const uint length )
{
    ASSERT( ( string != NULL ) || ( length == 0 ) );
    if( string && length )
    {
        m_string = string;
        OffsetAndLength item = { ~uint32_t( 0 ), static_cast< uint32_t >( length ) };
        int index = find( item );
        if( index < 0 )
        {
            index = insert( item );
            if( index >= 0 )
            {
                m_items.getItem( index ).offset = static_cast< uint32_t >( m_bytes.appendString( string, length ) );
            }
        }
        m_string = NULL;
        return( ~uint( index ) );
    }
    return( ~uint( 0 ) );   //  the handle ~0 always references the empty string
}

const uint8_t* StringTableBlob::getString( const uint handle ) const
{
    uint index = ~handle;
    if( index < m_items.getUsed() )
    {
        return( m_bytes.getBuffer() + m_items.getItem( index ).offset );
    }
    return( NULL );
}

const uint8_t* StringTableBlob::getString( const uint handle, uint& length ) const
{
    uint index = ~handle;
    if( index < m_items.getUsed() )
    {
        const OffsetAndLength& item = m_items.getItem( index );
        length = static_cast< uint >( item.length );
        return( m_bytes.getBuffer() + item.offset );
    }
    return( NULL );
}

uint StringTableBlob::getStringLength( const uint handle ) const
{
    uint index = ~handle;
    if( index < m_items.getUsed() )
    {
        return( static_cast< uint >( m_items.getItem( index ).length ) );
    }
    return( 0 );
}

uint StringTableBlob::findString( const uint8_t* const string ) const
{
    if( string )
    {
        uint length = 0;
        while( string[ length ] ) ++length;
        return( findString( string, length ) );
    }
    return( ~uint( 0 ) );   //  the handle ~0 always references the empty string
}

uint StringTableBlob::findString( const uint8_t* const string, const uint length ) const
{
    ASSERT( ( string != NULL ) || ( length == 0 ) );
    if( string && length )
    {
        m_string = string;
        OffsetAndLength item = { ~uint32_t( 0 ), static_cast< uint32_t >( length ) };
        int index = find( item );
        m_string = NULL;
        return( ~static_cast< uint >( index ) );
    }
    return( ~uint( 0 ) );   //  the handle ~0 always references the empty string
}

uint StringTableBlob::getFrozenSize() const
{   //  4 bytes for the count, 8 bytes for each string, N bytes of character data rounded up to a multiple of 4
    return( 4 + ( getCount() << 3 ) + ( ( getBytes() + 3 ) & ~3 ) );
}

FrozenStringTable* StringTableBlob::freeze( uint& size ) const
{
    uint buffer_size = getFrozenSize();
    void* buffer = reinterpret_cast< void* >( new uint8_t[ buffer_size ] );
    size = freeze( buffer, buffer_size );
    return( reinterpret_cast< FrozenStringTable* >( buffer ) );
}

uint StringTableBlob::freeze( void* const buffer, const uint buffer_size ) const
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
        for( int index = first(); index >= 0; index = next( index ) )
        {
            uint length = 0;
            const uint8_t* string = getString( ~uint( index ), length );
            strings[ 0 ] = offset;
            strings[ 1 ] = static_cast< uint32_t >( length );
            strings += 2;
            ++length;
            memcpy_s( reinterpret_cast< void* >( &bytes[ offset ] ), length, reinterpret_cast< const void* >( string ), length );
            offset += length;
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

int StringTableBlob::cmpKey( const void* const key, const int index ) const
{
    const OffsetAndLength& item1 = *reinterpret_cast< const OffsetAndLength* const >( key );
    const OffsetAndLength& item2 = m_items.getItem( index );
    int delta = static_cast< int >( item1.length - item2.length );
    if( delta == 0 )
    {
        const uint8_t* buffer = m_bytes.getBuffer();
        delta = memcmp( ( ( item1.offset == ~uint32_t( 0 ) ) ? m_string : ( buffer + item1.offset ) ), ( buffer + item2.offset ), static_cast< size_t >( item1.length ) );
    }
    return( delta );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



