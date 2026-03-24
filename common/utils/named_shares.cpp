
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   named_shares.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Named shared pointer registry class.
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

#include "libs/system/debug/asserts.h"
#include "named_shares.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    NamedShares named shared pointer registry class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool NamedShares::init( StringTableBlob& strings, const int count )
{
    kill();
    int size = ( ( count < 4 ) ? 4 : count );
    if( m_sorting.init( *this, size ) )
    {
        m_items = new Item[ size ];
        if( m_items )
        {
            m_strings = &strings;
            m_size = size;
            return( true );
        }
        m_sorting.kill();
    }
    return( false );
}

void NamedShares::kill()
{
    m_sorting.kill();
    m_strings = NULL;
    if( m_items )
    {
        delete[] m_items;
        m_items = NULL;
    }
    m_size = 0;
}

int NamedShares::findIndex( const StringItem& name ) const
{
    int index = -1;
    if( valid() && name.notEmpty() )
    {
        uint length = 0;
        const uint8_t* string = name.getString( length );
        m_temp.nameHandle = static_cast< uintptr_t >( m_strings->findString( string, length ) );
        index = m_sorting.find( &m_temp );
    }
    return( index );
}

bool NamedShares::getName( const int index, StringItem& name ) const
{
    if( valid( index ) )
    {
        uint length = 0;
        const uint8_t* string = m_strings->getString( static_cast< uint >( m_items[ index ].nameHandle ), length );
        name.setString( string, length );
        return( true );
    }
    return( false );
}

const SharedPtr& NamedShares::getItem( const int index ) const
{
    return( valid( index ) ? m_items[ index ].sharedPtr : m_null );
}

bool NamedShares::addItem( const StringItem& name, const SharedPtr& sharedPtr, const bool allowReplace )
{
    if( valid() )
    {
        int index = findIndex( name );
        if( index < 0 )
        {   //  item not found
            uint length = 0;
            const uint8_t* string = name.getString( length );
            m_temp.nameHandle = static_cast< uintptr_t >( m_strings->addString( string, length ) );
            index = m_sorting.insert( &m_temp );
            if( index >= m_size )
            {   //  need to grow the array
                int size = ( m_size + ( m_size >> 1 ) + 1 );
                size_t bytes = ( m_size * sizeof( Item ) );
                Item* items = new Item[ size ];
                memcpy_s( items, bytes, m_items, bytes );
                memset( m_items, 0, bytes );
                delete[] m_items;
                m_items = items;
                m_size = size;
            }
            Item& item = m_items[ index ];
            item.sharedPtr.share( sharedPtr );
            item.nameHandle = m_temp.nameHandle;
            return( true );
        }
        else if( allowReplace )
        {   //  item found
            m_items[ index ].sharedPtr.share( sharedPtr );
            return( true );
        }
    }
    return( false );
}

bool NamedShares::replace( const int index, const SharedPtr& sharedPtr )
{
    if( valid( index ) )
    {
        m_items[ index ].sharedPtr.share( sharedPtr );
        return( true );
    }
    return( false );
}

const void* NamedShares::getKey( const int index ) const
{
    return( &m_items[ index ] );
}

int NamedShares::cmpKey( const void* const key, const int index ) const
{
    const Item& item1 = *reinterpret_cast< const Item* >( key );
    const Item& item2 = m_items[ index ];
    return( static_cast< int >( item1.nameHandle- item2.nameHandle ) );
}

void NamedShares::moveItem( const int source, const int target )
{
    Item& sourceItem = ( ( source < 0 ) ? m_temp : m_items[ source ] );
    Item& targetItem = ( ( target < 0 ) ? m_temp : m_items[ target ] );
    targetItem = sourceItem;
    sourceItem.sharedPtr.release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

