
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   frozen_string_table.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	A string table for use in binary files.
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

#include "frozen_string_table.h"
#include <memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    FrozenStringTable class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const uint8_t* FrozenStringTable::getString( const int index ) const
{
    if( static_cast< uint32_t >( index ) < m_data.count )
    {
        return( reinterpret_cast< const uint8_t* const >( &m_data.strings[ m_data.count ] ) + m_data.strings[ index ].offset );
    }
    return( NULL );
}

const uint8_t* FrozenStringTable::getString( const int index, uint& length ) const
{
    if( static_cast< uint32_t >( index ) < m_data.count )
    {
        const String& string = m_data.strings[ index ];
        length = string.length;
        return( reinterpret_cast< const uint8_t* const >( &m_data.strings[ m_data.count ] ) + string.offset );
    }
    length = 0;
    return( NULL );
}

uint FrozenStringTable::getStringLength( const int index ) const
{
    if( static_cast< uint32_t >( index ) < m_data.count )
    {
        return( m_data.strings[ index ].length );
    }
    return( NULL );
}

int FrozenStringTable::findString( const uint8_t* const string ) const
{
    if( string )
    {
        uint length = 0;
        while( string[ length ] ) ++length;
        return( findString( string, length ) );
    }
    return( 0 );    //  the first string index always references the empty string
}

int FrozenStringTable::findString( const uint8_t* const string, const uint length ) const
{
    if( string )
    {
        uint32_t lower = 0;
	    uint32_t upper = ( m_data.count - 1 );
	    while( lower < upper )
	    {
	    	uint32_t check = ( ( lower + upper ) >> 1 );
            if( cmp( string, length, check ) > 0 )
            {
	    		lower = ( check + 1 );
            }
            else
            {
	    		upper = check;
            }
	    }
	    return( cmp( string, length, lower ) ? -1 : static_cast< int >( lower ) );
    }
    return( 0 );    //  the first string index always references the empty string
}

int FrozenStringTable::cmpString( const uint8_t* const string, const uint index ) const
{
    if( string )
    {
        uint length = 0;
        while( string[ length ] ) ++length;
        return( cmp( string, length, index ) );
    }
    return( -1 );
}

int FrozenStringTable::cmpString( const uint8_t* const string, const uint length, const uint index ) const
{
    if( string )
    {
        return( cmp( string, length, index ) );
    }
    return( -1 );
}

uint FrozenStringTable::getSize() const
{
    const String& item = m_data.strings[ m_data.count - 1 ];
    return( static_cast< uint >( ( item.offset + item.length + 4 ) & ~3 ) );
}

int FrozenStringTable::cmp( const uint8_t* const string, const uint length, const uint index ) const
{
    const String& item = m_data.strings[ index ];
    int delta = static_cast< int >( length - item.length );
    if( delta == 0 )
    {
        const uint8_t* const compare = ( reinterpret_cast< const uint8_t* const >( &m_data.strings[ m_data.count ] ) + item.offset );
        delta = memcmp( reinterpret_cast< const void* const >( string ), reinterpret_cast< const void* const >( compare ), length );
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



