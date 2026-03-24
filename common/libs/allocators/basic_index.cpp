
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   basic_index.cpp
////    Author: Ritchie Brannan
////    Date:   07 Aug 16
////
////    Description:
////
////    	Array allocation index management classes.
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

#include "basic_index.h"
#include "libs/system/debug/asserts.h"
#include <memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    inline support functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline int getBitIndex( const uint32_t value )
{
    int index = ( ( ( 0 - ( value >> 16 ) ) >> 16 ) & 16 );
	index += ( ( ( 0 - ( value >> ( index + 8 ) ) ) >> 8 ) & 8 );
	index += ( ( ( 0 - ( value >> ( index + 4 ) ) ) >> 4 ) & 4 );
	index += ( value >> ( index + 2 ) );
	return( index + ( value >> index ) - 1 );
}

static inline int lowBitIndex( const uint32_t value )
{
    return( getBitIndex( value & ( 0 - value ) ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CIndexAlloc1k class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CIndexAlloc1k::allocIndex()
{
    if( m_freeCount )
    {
        uint32_t slots0 = ~m_slots0;
        uint32_t bflag0 = ( slots0 & ( 0 - slots0 ) );
        int index1 = getBitIndex( bflag0 );
        uint32_t slots1 = ~m_slots1[ index1 ];
        uint32_t bflag1 = ( slots1 & ( 0 - slots1 ) );
        int index = ( getBitIndex( bflag1 ) + ( index1 << 5 ) );
        slots1 ^= bflag1;
        m_slots1[ index1 ] = ~slots1;
        if( slots1 == 0 )
        {   //  block has become fully allocated
            slots0 ^= bflag0;
            m_slots0 = ~slots0;
        }
        --m_freeCount;
        ++m_usedCount;
        if( m_peakCount < m_usedCount )
        {
            m_peakCount = m_usedCount;
        }
        return( index );
    }
    return( -1 );
}

bool CIndexAlloc1k::allocIndex( const int index )
{
    if( static_cast< uint32_t >( index ) < m_slotCount )
    {
        int index1 = ( index >> 5 );
        uint32_t bflag1 = ( uint32_t( 1 ) << ( index & 31 ) );
        uint32_t slots1 = m_slots1[ index1 ];
        if( ~slots1 & bflag1 )
        {   //  is not allocated
            slots1 ^= bflag1;
            m_slots1[ index1 ] = slots1;
            if( ~slots1 == 0 )
            {   //  block has become fully allocated
                m_slots0 ^= ( uint32_t( 1 ) << ( index1 >> 5 ) );
            }
            --m_freeCount;
            ++m_usedCount;
            if( m_peakCount < m_usedCount )
            {
                m_peakCount = m_usedCount;
            }
            return( true );
        }
    }
    return( false );
}

bool CIndexAlloc1k::freeIndex( const int index )
{
    if( static_cast< uint32_t >( index ) < m_slotCount )
    {
        int index1 = ( index >> 5 );
        uint32_t bflag1 = ( uint32_t( 1 ) << ( index & 31 ) );
        uint32_t slots1 = m_slots1[ index1 ];
        if( slots1 & bflag1 )
        {   //  is allocated
            if( ~slots1 == 0 )
            {   //  block was completely allocated
                m_slots0 ^= ( uint32_t( 1 ) << ( index1 >> 5 ) );
            }
            slots1 ^= bflag1;
            m_slots1[ index1 ] = slots1;
            ++m_freeCount;
            --m_usedCount;
            return( true );
        }
    }
    return( false );
}

void CIndexAlloc1k::freeAll()
{
    memset( this, 0, sizeof( CIndexAlloc1k ) );
    m_slotCount = m_freeCount = ( 32 * 32 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CIndexAlloc32k class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CIndexAlloc32k::allocIndex()
{
    if( m_freeCount )
    {
        uint32_t slots0 = ~m_slots0;
        uint32_t bflag0 = ( slots0 & ( 0 - slots0 ) );
        int index1 = getBitIndex( bflag0 );
        uint32_t slots1 = ~m_slots1[ index1 ];
        uint32_t bflag1 = ( slots1 & ( 0 - slots1 ) );
        int index2 = ( getBitIndex( bflag1 ) + ( index1 << 5 ) );
        uint32_t slots2 = ~m_slots2[ index2 ];
        uint32_t bflag2 = ( slots2 & ( 0 - slots2 ) );
        int index = ( getBitIndex( bflag2 ) + ( index2 << 5 ) );
        slots2 ^= bflag2;
        m_slots2[ index2 ] = ~slots2;
        if( slots2 == 0 )
        {   //  block has become fully allocated
            slots1 ^= bflag1;
            m_slots1[ index1 ] = ~slots1;
            if( slots1 == 0 )
            {   //  block has become fully allocated
                slots0 ^= bflag0;
                m_slots0 = ~slots0;
            }
        }
        --m_freeCount;
        ++m_usedCount;
        if( m_peakCount < m_usedCount )
        {
            m_peakCount = m_usedCount;
        }
        return( index );
    }
    return( -1 );
}

bool CIndexAlloc32k::allocIndex( const int index )
{
    if( static_cast< uint32_t >( index ) < m_slotCount )
    {
        int index2 = ( index >> 5 );
        uint32_t bflag2 = ( uint32_t( 1 ) << ( index & 31 ) );
        uint32_t slots2 = m_slots2[ index2 ];
        if( ~slots2 & bflag2 )
        {   //  is not allocated
            slots2 ^= bflag2;
            m_slots2[ index2 ] = slots2;
            if( ~slots2 == 0 )
            {   //  block has become fully allocated
                int index1 = ( index2 >> 5 );
                uint32_t bflag1 = ( uint32_t( 1 ) << ( index2 & 31 ) );
                uint32_t slots1 = m_slots1[ index1 ];
                slots1 ^= bflag1;
                m_slots1[ index1 ] = slots1;
                if( ~slots1 == 0 )
                {   //  block has become fully allocated
                    m_slots0 ^= ( uint32_t( 1 ) << ( index1 >> 5 ) );
                }
            }
            --m_freeCount;
            ++m_usedCount;
            if( m_peakCount < m_usedCount )
            {
                m_peakCount = m_usedCount;
            }
            return( true );
        }
    }
    return( false );
}

bool CIndexAlloc32k::freeIndex( const int index )
{
    if( static_cast< uint32_t >( index ) < m_slotCount )
    {
        int index2 = ( index >> 5 );
        uint32_t bflag2 = ( uint32_t( 1 ) << ( index & 31 ) );
        uint32_t slots2 = m_slots2[ index2 ];
        if( slots2 & bflag2 )
        {   //  is allocated
            if( ~slots2 == 0 )
            {   //  block was completely allocated
                int index1 = ( index2 >> 5 );
                uint32_t bflag1 = ( uint32_t( 1 ) << ( index2 & 31 ) );
                uint32_t slots1 = m_slots1[ index1 ];
                if( ~slots1 == 0 )
                {   //  block was completely allocated
                    m_slots0 ^= ( uint32_t( 1 ) << ( index1 >> 5 ) );
                }
                slots1 ^= bflag1;
                m_slots1[ index1 ] = slots1;
            }
            slots2 ^= bflag2;
            m_slots2[ index2 ] = slots2;
            ++m_freeCount;
            --m_usedCount;
            return( true );
        }
    }
    return( false );
}

void CIndexAlloc32k::freeAll()
{
    memset( this, 0, sizeof( CIndexAlloc32k ) );
    m_slotCount = m_freeCount = ( 32 * 32 * 32 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CIndexAlloc1024k class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CIndexAlloc1024k::allocIndex()
{
    if( m_freeCount )
    {
        uint32_t slots0 = ~m_slots0;
        uint32_t bflag0 = ( slots0 & ( 0 - slots0 ) );
        int index1 = getBitIndex( bflag0 );
        uint32_t slots1 = ~m_slots1[ index1 ];
        uint32_t bflag1 = ( slots1 & ( 0 - slots1 ) );
        int index2 = ( getBitIndex( bflag1 ) + ( index1 << 5 ) );
        uint32_t slots2 = ~m_slots2[ index2 ];
        uint32_t bflag2 = ( slots2 & ( 0 - slots2 ) );
        int index3 = ( getBitIndex( bflag2 ) + ( index2 << 5 ) );
        uint32_t slots3 = ~m_slots3[ index3 ];
        uint32_t bflag3 = ( slots3 & ( 0 - slots3 ) );
        int index = ( getBitIndex( bflag3 ) + ( index3 << 5 ) );
        slots3 ^= bflag3;
        m_slots3[ index3 ] = ~slots3;
        if( slots3 == 0 )
        {   //  block has become fully allocated
            slots2 ^= bflag2;
            m_slots2[ index2 ] = ~slots2;
            if( slots2 == 0 )
            {   //  block has become fully allocated
                slots1 ^= bflag1;
                m_slots1[ index1 ] = ~slots1;
                if( slots1 == 0 )
                {   //  block has become fully allocated
                    slots0 ^= bflag0;
                    m_slots0 = ~slots0;
                }
            }
        }
        --m_freeCount;
        ++m_usedCount;
        if( m_peakCount < m_usedCount )
        {
            m_peakCount = m_usedCount;
        }
        return( index );
    }
    return( -1 );
}

bool CIndexAlloc1024k::allocIndex( const int index )
{
    if( static_cast< uint32_t >( index ) < m_slotCount )
    {
        int index3 = ( index >> 5 );
        uint32_t bflag3 = ( uint32_t( 1 ) << ( index & 31 ) );
        uint32_t slots3 = m_slots3[ index3 ];
        if( ~slots3 & bflag3 )
        {   //  is not allocated
            slots3 ^= bflag3;
            m_slots3[ index3 ] = slots3;
            if( ~slots3 == 0 )
            {   //  block has become fully allocated
                int index2 = ( index3 >> 5 );
                uint32_t bflag2 = ( uint32_t( 1 ) << ( index3 & 31 ) );
                uint32_t slots2 = m_slots2[ index2 ];
                slots2 ^= bflag2;
                m_slots2[ index2 ] = slots2;
                if( ~slots2 == 0 )
                {   //  block has become fully allocated
                    int index1 = ( index2 >> 5 );
                    uint32_t bflag1 = ( uint32_t( 1 ) << ( index2 & 31 ) );
                    uint32_t slots1 = m_slots1[ index1 ];
                    slots1 ^= bflag1;
                    m_slots1[ index1 ] = slots1;
                    if( ~slots1 == 0 )
                    {   //  block has become fully allocated
                        m_slots0 ^= ( uint32_t( 1 ) << ( index1 >> 5 ) );
                    }
                }
            }
            --m_freeCount;
            ++m_usedCount;
            if( m_peakCount < m_usedCount )
            {
                m_peakCount = m_usedCount;
            }
            return( true );
        }
    }
    return( false );
}

bool CIndexAlloc1024k::freeIndex( const int index )
{
    if( static_cast< uint32_t >( index ) < m_slotCount )
    {
        int index3 = ( index >> 5 );
        uint32_t bflag3 = ( uint32_t( 1 ) << ( index & 31 ) );
        uint32_t slots3 = m_slots3[ index3 ];
        if( slots3 & bflag3 )
        {   //  is allocated
            if( ~slots3 == 0 )
            {   //  block was completely allocated
                int index2 = ( index3 >> 5 );
                uint32_t bflag2 = ( uint32_t( 1 ) << ( index3 & 31 ) );
                uint32_t slots2 = m_slots2[ index2 ];
                if( ~slots2 == 0 )
                {   //  block was completely allocated
                    int index1 = ( index2 >> 5 );
                    uint32_t bflag1 = ( uint32_t( 1 ) << ( index2 & 31 ) );
                    uint32_t slots1 = m_slots1[ index1 ];
                    if( ~slots1 == 0 )
                    {   //  block was completely allocated
                        m_slots0 ^= ( uint32_t( 1 ) << ( index1 >> 5 ) );
                    }
                    slots1 ^= bflag1;
                    m_slots1[ index1 ] = slots1;
                }
                slots2 ^= bflag2;
                m_slots2[ index2 ] = slots2;
            }
            m_slots3[ index3 ] = ( slots3 ^ bflag3 );
            ++m_freeCount;
            --m_usedCount;
            return( true );
        }
    }
    return( false );
}

void CIndexAlloc1024k::freeAll()
{
    memset( this, 0, sizeof( CIndexAlloc1024k ) );
    m_slotCount = m_freeCount = ( 32 * 32 * 32 * 32 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



