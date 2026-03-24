
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   buddy_index.cpp
////    Author: Ritchie Brannan
////    Date:   07 Aug 16
////
////    Description:
////
////    	Buddy allocation index management classes.
////
////    Notes:
////
////        There are 3 public allocation checking functions:
////
////            isAllocated( index, level ):
////
////                returns true if the index references an allocation made at the specified level
////                returns false if the index is not valid for allocation at the specified level
////                returns false if the slot is allocated for use by a higher level
////
////            isAllocation( index ):
////
////                returns true if the index references an allocation
////                returns false if the index does not reference an allocation
////
////            findAllocation( index ):
////
////                returns the level at which the index is a part of an allocation
////                or -1 if the index references an unallocated slot
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  need assorted allocation checks (return level if allocated or 0 if unallocated)
//      allocated and deallocatable at any level - returns level or -1
//      - isAllocation()
//      within allocation block at any level - returns level or -1
//      - findAllocation()
//      allocated only at specified level (not allocated or available at a higher level)
//      - isAllocated()
//      - check level above for either of the 2 slots being allocated


//  need assorted global stats
//      current allocation count
//      peak allocation count
//      current allocated size
//      peak allocated size

//  need assorted level specific stats
//      current allocation count
//      peak allocation count

//  need method of extracting stats per level (other than casting)
//      have accessible structure that can be returned


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "buddy_index.h"
#include "libs/system/debug/asserts.h"
#include <memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin memory namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace memory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes reset() function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void   CBuddyIndex32::reset() { m_allocated = 0; m_levels = ( c_level + 1 ); };
void   CBuddyIndex64::reset() { m_available0 = 0; m_allocated[ 0 ] = m_allocated[ 1 ] = 0; m_levels = ( c_level + 1 ); };
void  CBuddyIndex128::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void  CBuddyIndex256::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void  CBuddyIndex512::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void   CBuddyIndex1k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void   CBuddyIndex2k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void   CBuddyIndex4k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void   CBuddyIndex8k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void  CBuddyIndex16k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void  CBuddyIndex32k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void  CBuddyIndex64k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void CBuddyIndex128k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void CBuddyIndex256k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void CBuddyIndex512k::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void   CBuddyIndex1m::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void   CBuddyIndex2m::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void   CBuddyIndex4m::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void   CBuddyIndex8m::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void  CBuddyIndex16m::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void  CBuddyIndex32m::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };
void  CBuddyIndex64m::reset() { memset( this, 0, sizeof( *this ) ); m_levels = ( c_level + 1 ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex32 class inline lowBitIndex() function body
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CBuddyIndex32::lowBitIndex( const uint32_t value )
{
    uint32_t count = ( value ? ( value ^ ( value - 1 ) ) : 0 );
    count = ( ( count & 0x49249249 ) + ( ( count >> 1 ) & 0x49249249 ) + ( ( count >> 2 ) & 0x49249249 ) );
    count = ( ( count + ( count >> 3 ) ) & 0xc71c71c7 );
    return( ( count % 63 ) - 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex32 class inline function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CBuddyIndex32::getAvailable() const { return( lowBitIndex( ~m_allocated ) ); };
void CBuddyIndex32::setAllocated( const int index ) { m_allocated |= ( uint32_t( 1 ) << index ); };
void CBuddyIndex32::setDeallocated( const int index ) { m_allocated &= ~( uint32_t( 1 ) << index ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex64 class inline function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBuddyIndex64::setAvailable( const int index ) { m_available0 |= ( uint32_t( 1 ) << ( index >> 1 ) ); };
void CBuddyIndex64::setUnavailable( const int index ) { m_available0 &= ~( uint32_t( 1 ) << ( index >> 1 ) ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes inline isAllocated() function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool   CBuddyIndex32::isAllocated( const int index ) const { return( ( ( m_allocated               >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex64::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex128::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex256::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex512::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex1k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex2k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex4k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex8k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex16k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex32k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex64k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool CBuddyIndex128k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool CBuddyIndex256k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool CBuddyIndex512k::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex1m::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex2m::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex4m::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool   CBuddyIndex8m::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex16m::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex32m::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };
bool  CBuddyIndex64m::isAllocated( const int index ) const { return( ( ( m_allocated[ index >> 5 ] >> ( index & 31 ) ) & 1 ) ? true : false ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes inline doAllocation() function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int   CBuddyIndex32::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex64::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex128::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex256::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex512::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex1k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex2k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex4k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex8k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex16k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex32k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex64k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int CBuddyIndex128k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int CBuddyIndex256k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int CBuddyIndex512k::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex1m::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex2m::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex4m::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int   CBuddyIndex8m::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex16m::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex32m::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };
int  CBuddyIndex64m::doAllocation() { int index = getAvailable(); if( index >= 0 ) { setAllocated( index ); } return( index ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes doDeallocation() function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool   CBuddyIndex32::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( false ); };
bool   CBuddyIndex64::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex32::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex128::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex64::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex256::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :  CBuddyIndex128::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex512::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :  CBuddyIndex256::doDeallocation( index >> 1 ) ); };
bool   CBuddyIndex1k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :  CBuddyIndex512::doDeallocation( index >> 1 ) ); };
bool   CBuddyIndex2k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex1k::doDeallocation( index >> 1 ) ); };
bool   CBuddyIndex4k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex2k::doDeallocation( index >> 1 ) ); };
bool   CBuddyIndex8k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex4k::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex16k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex8k::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex32k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :  CBuddyIndex16k::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex64k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :  CBuddyIndex32k::doDeallocation( index >> 1 ) ); };
bool CBuddyIndex128k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :  CBuddyIndex64k::doDeallocation( index >> 1 ) ); };
bool CBuddyIndex256k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false : CBuddyIndex128k::doDeallocation( index >> 1 ) ); };
bool CBuddyIndex512k::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false : CBuddyIndex256k::doDeallocation( index >> 1 ) ); };
bool   CBuddyIndex1m::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false : CBuddyIndex512k::doDeallocation( index >> 1 ) ); };
bool   CBuddyIndex2m::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex1m::doDeallocation( index >> 1 ) ); };
bool   CBuddyIndex4m::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex2m::doDeallocation( index >> 1 ) ); };
bool   CBuddyIndex8m::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex4m::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex16m::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :   CBuddyIndex8m::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex32m::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :  CBuddyIndex16m::doDeallocation( index >> 1 ) ); };
bool  CBuddyIndex64m::doDeallocation( const int index ) { if( isAllocated( index ) ) { setDeallocated( index ); return( true ); } return( ( index & 1 ) ? false :  CBuddyIndex32m::doDeallocation( index >> 1 ) ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes inline getAvailablePair() function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int   CBuddyIndex64::getAvailablePair() const { int index =   CBuddyIndex32::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex128::getAvailablePair() const { int index =   CBuddyIndex64::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex256::getAvailablePair() const { int index =  CBuddyIndex128::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex512::getAvailablePair() const { int index =  CBuddyIndex256::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int   CBuddyIndex1k::getAvailablePair() const { int index =  CBuddyIndex512::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int   CBuddyIndex2k::getAvailablePair() const { int index =   CBuddyIndex1k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int   CBuddyIndex4k::getAvailablePair() const { int index =   CBuddyIndex2k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int   CBuddyIndex8k::getAvailablePair() const { int index =   CBuddyIndex4k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex16k::getAvailablePair() const { int index =   CBuddyIndex8k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex32k::getAvailablePair() const { int index =  CBuddyIndex16k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex64k::getAvailablePair() const { int index =  CBuddyIndex32k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int CBuddyIndex128k::getAvailablePair() const { int index =  CBuddyIndex64k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int CBuddyIndex256k::getAvailablePair() const { int index = CBuddyIndex128k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int CBuddyIndex512k::getAvailablePair() const { int index = CBuddyIndex256k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int   CBuddyIndex1m::getAvailablePair() const { int index = CBuddyIndex512k::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int   CBuddyIndex2m::getAvailablePair() const { int index =   CBuddyIndex1m::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int   CBuddyIndex4m::getAvailablePair() const { int index =   CBuddyIndex2m::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int   CBuddyIndex8m::getAvailablePair() const { int index =   CBuddyIndex4m::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex16m::getAvailablePair() const { int index =   CBuddyIndex8m::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex32m::getAvailablePair() const { int index =  CBuddyIndex16m::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };
int  CBuddyIndex64m::getAvailablePair() const { int index =  CBuddyIndex32m::getAvailable(); return( ( index < 0 ) ? index : ( index << 1 ) ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes inline setAvailablePair() function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void   CBuddyIndex64::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex32::setAllocated( index >> 1 ); };
void  CBuddyIndex128::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex64::setAllocated( index >> 1 ); };
void  CBuddyIndex256::setAvailablePair( const int index ) { setAvailable( index );  CBuddyIndex128::setAllocated( index >> 1 ); };
void  CBuddyIndex512::setAvailablePair( const int index ) { setAvailable( index );  CBuddyIndex256::setAllocated( index >> 1 ); };
void   CBuddyIndex1k::setAvailablePair( const int index ) { setAvailable( index );  CBuddyIndex512::setAllocated( index >> 1 ); };
void   CBuddyIndex2k::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex1k::setAllocated( index >> 1 ); };
void   CBuddyIndex4k::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex2k::setAllocated( index >> 1 ); };
void   CBuddyIndex8k::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex4k::setAllocated( index >> 1 ); };
void  CBuddyIndex16k::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex8k::setAllocated( index >> 1 ); };
void  CBuddyIndex32k::setAvailablePair( const int index ) { setAvailable( index );  CBuddyIndex16k::setAllocated( index >> 1 ); };
void  CBuddyIndex64k::setAvailablePair( const int index ) { setAvailable( index );  CBuddyIndex32k::setAllocated( index >> 1 ); };
void CBuddyIndex128k::setAvailablePair( const int index ) { setAvailable( index );  CBuddyIndex64k::setAllocated( index >> 1 ); };
void CBuddyIndex256k::setAvailablePair( const int index ) { setAvailable( index ); CBuddyIndex128k::setAllocated( index >> 1 ); };
void CBuddyIndex512k::setAvailablePair( const int index ) { setAvailable( index ); CBuddyIndex256k::setAllocated( index >> 1 ); };
void   CBuddyIndex1m::setAvailablePair( const int index ) { setAvailable( index ); CBuddyIndex512k::setAllocated( index >> 1 ); };
void   CBuddyIndex2m::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex1m::setAllocated( index >> 1 ); };
void   CBuddyIndex4m::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex2m::setAllocated( index >> 1 ); };
void   CBuddyIndex8m::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex4m::setAllocated( index >> 1 ); };
void  CBuddyIndex16m::setAvailablePair( const int index ) { setAvailable( index );   CBuddyIndex8m::setAllocated( index >> 1 ); };
void  CBuddyIndex32m::setAvailablePair( const int index ) { setAvailable( index );  CBuddyIndex16m::setAllocated( index >> 1 ); };
void  CBuddyIndex64m::setAvailablePair( const int index ) { setAvailable( index );  CBuddyIndex32m::setAllocated( index >> 1 ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes inline setUnavailablePair() function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void   CBuddyIndex64::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex32::setDeallocated( index >> 1 ); };
void  CBuddyIndex128::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex64::setDeallocated( index >> 1 ); };
void  CBuddyIndex256::setUnavailablePair( const int index ) { setUnavailable( index );  CBuddyIndex128::setDeallocated( index >> 1 ); };
void  CBuddyIndex512::setUnavailablePair( const int index ) { setUnavailable( index );  CBuddyIndex256::setDeallocated( index >> 1 ); };
void   CBuddyIndex1k::setUnavailablePair( const int index ) { setUnavailable( index );  CBuddyIndex512::setDeallocated( index >> 1 ); };
void   CBuddyIndex2k::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex1k::setDeallocated( index >> 1 ); };
void   CBuddyIndex4k::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex2k::setDeallocated( index >> 1 ); };
void   CBuddyIndex8k::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex4k::setDeallocated( index >> 1 ); };
void  CBuddyIndex16k::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex8k::setDeallocated( index >> 1 ); };
void  CBuddyIndex32k::setUnavailablePair( const int index ) { setUnavailable( index );  CBuddyIndex16k::setDeallocated( index >> 1 ); };
void  CBuddyIndex64k::setUnavailablePair( const int index ) { setUnavailable( index );  CBuddyIndex32k::setDeallocated( index >> 1 ); };
void CBuddyIndex128k::setUnavailablePair( const int index ) { setUnavailable( index );  CBuddyIndex64k::setDeallocated( index >> 1 ); };
void CBuddyIndex256k::setUnavailablePair( const int index ) { setUnavailable( index ); CBuddyIndex128k::setDeallocated( index >> 1 ); };
void CBuddyIndex512k::setUnavailablePair( const int index ) { setUnavailable( index ); CBuddyIndex256k::setDeallocated( index >> 1 ); };
void   CBuddyIndex1m::setUnavailablePair( const int index ) { setUnavailable( index ); CBuddyIndex512k::setDeallocated( index >> 1 ); };
void   CBuddyIndex2m::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex1m::setDeallocated( index >> 1 ); };
void   CBuddyIndex4m::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex2m::setDeallocated( index >> 1 ); };
void   CBuddyIndex8m::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex4m::setDeallocated( index >> 1 ); };
void  CBuddyIndex16m::setUnavailablePair( const int index ) { setUnavailable( index );   CBuddyIndex8m::setDeallocated( index >> 1 ); };
void  CBuddyIndex32m::setUnavailablePair( const int index ) { setUnavailable( index );  CBuddyIndex16m::setDeallocated( index >> 1 ); };
void  CBuddyIndex64m::setUnavailablePair( const int index ) { setUnavailable( index );  CBuddyIndex32m::setDeallocated( index >> 1 ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes allocate( slots ) function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int   CBuddyIndex32::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() : -1 ); };
int   CBuddyIndex64::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex32::allocate( slots ) ); };
int  CBuddyIndex128::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex64::allocate( slots ) ); };
int  CBuddyIndex256::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :  CBuddyIndex128::allocate( slots ) ); };
int  CBuddyIndex512::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :  CBuddyIndex256::allocate( slots ) ); };
int   CBuddyIndex1k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :  CBuddyIndex512::allocate( slots ) ); };
int   CBuddyIndex2k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex1k::allocate( slots ) ); };
int   CBuddyIndex4k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex2k::allocate( slots ) ); };
int   CBuddyIndex8k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex4k::allocate( slots ) ); };
int  CBuddyIndex16k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex8k::allocate( slots ) ); };
int  CBuddyIndex32k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :  CBuddyIndex16k::allocate( slots ) ); };
int  CBuddyIndex64k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :  CBuddyIndex32k::allocate( slots ) ); };
int CBuddyIndex128k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :  CBuddyIndex64k::allocate( slots ) ); };
int CBuddyIndex256k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() : CBuddyIndex128k::allocate( slots ) ); };
int CBuddyIndex512k::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() : CBuddyIndex256k::allocate( slots ) ); };
int   CBuddyIndex1m::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() : CBuddyIndex512k::allocate( slots ) ); };
int   CBuddyIndex2m::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex1m::allocate( slots ) ); };
int   CBuddyIndex4m::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex2m::allocate( slots ) ); };
int   CBuddyIndex8m::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex4m::allocate( slots ) ); };
int  CBuddyIndex16m::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :   CBuddyIndex8m::allocate( slots ) ); };
int  CBuddyIndex32m::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :  CBuddyIndex16m::allocate( slots ) ); };
int  CBuddyIndex64m::allocate( const uint slots ) { return( ( slots <= ( uint( 1 ) << ( m_levels - c_level - 1 ) ) ) ? doAllocation() :  CBuddyIndex32m::allocate( slots ) ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes deallocate() function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool   CBuddyIndex32::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex64::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex128::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex256::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex512::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex1k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex2k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex4k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex8k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex16k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex32k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex64k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool CBuddyIndex128k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool CBuddyIndex256k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool CBuddyIndex512k::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex1m::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex2m::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex4m::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool   CBuddyIndex8m::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex16m::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex32m::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };
bool  CBuddyIndex64m::deallocate( const int index ) { return( isValidIndex( index ) ? doDeallocation( index ) : false ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes setAllocated() function bodies
////
////    note: these functions are identical for all classes apart from CBuddyIndex32 which has no parent class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBuddyIndex64::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex128::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex256::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex512::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex1k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex2k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex4k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex8k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex16k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex32k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex64k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex128k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex256k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex512k::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex1m::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex2m::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex4m::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex8m::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex16m::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex32m::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

void CBuddyIndex64m::setAllocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated |= ( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated == 3 )
    {   //  the pair is fully allocated, so mark as unavailable
        setUnavailable( index );
    }
    else
    {   //  the pair is partially allocated, so mark as available and allocate from parent
        setAvailablePair( index );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes setDeallocated() function bodies
////
////    note: these functions are identical for all classes apart from CBuddyIndex32 which has no parent class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBuddyIndex64::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}


void CBuddyIndex128::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex256::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex512::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex1k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}



void CBuddyIndex2k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex4k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex8k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex16k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex32k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex64k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex128k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex256k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex512k::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex1m::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex2m::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex4m::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex8m::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex16m::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex32m::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

void CBuddyIndex64m::setDeallocated( const int index )
{
    int i = ( index >> 5 );
    uint32_t allocated = m_allocated[ i ];
    allocated &= ~( uint32_t( 1 ) << ( index & 31 ) );
    m_allocated[ i ] = allocated;
    allocated = ( ( allocated >> ( index & 30 ) ) & 3 );
    if( allocated != 0 )
    {   //  the pair is partially allocated, so mark as available
        setAvailable( index );
    }
    else
    {   //  the pair is fully deallocated, so mark as unavailable and deallocate from the parent
        setUnavailablePair( index );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes getAvailable() function bodies
////
////    note:
////
////        These functions are identical within each of the following groups:
////
////            { CBuddyIndex128,  CBuddyIndex256,  CBuddyIndex512,  CBuddyIndex1k,  CBuddyIndex2k  }
////            { CBuddyIndex4k,   CBuddyIndex8k,   CBuddyIndex16k,  CBuddyIndex32k, CBuddyIndex64k }
////            { CBuddyIndex128k, CBuddyIndex256k, CBuddyIndex512k, CBuddyIndex1m,  CBuddyIndex2m  }
////            { CBuddyIndex4m,   CBuddyIndex8m,   CBuddyIndex16m,  CBuddyIndex32m, CBuddyIndex64m }
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CBuddyIndex64::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex128::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex256::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex512::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex1k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex2k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex4k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex8k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex16k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex32k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex64k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex128k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex256k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex512k::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex1m::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex2m::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex4m::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available4[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex8m::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available4[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex16m::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available4[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex32m::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available4[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

int CBuddyIndex64m::getAvailable() const
{
    int index = -1;
    if( m_available0 )
    {
        index = lowBitIndex( m_available0 );
        index = ( ( index << 5 ) + lowBitIndex( m_available1[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available2[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available3[ index ] ) );
        index = ( ( index << 5 ) + lowBitIndex( m_available4[ index ] ) );
        index <<= 1;
        if( isAllocated( index ) ) ++index;
        return( index );
    }
    return( getAvailablePair() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes setAvailable() function bodies
////
////    note:
////
////        These functions are identical within each of the following groups:
////
////            { CBuddyIndex128,  CBuddyIndex256,  CBuddyIndex512,  CBuddyIndex1k,  CBuddyIndex2k  }
////            { CBuddyIndex4k,   CBuddyIndex8k,   CBuddyIndex16k,  CBuddyIndex32k, CBuddyIndex64k }
////            { CBuddyIndex128k, CBuddyIndex256k, CBuddyIndex512k, CBuddyIndex1m,  CBuddyIndex2m  }
////            { CBuddyIndex4m,   CBuddyIndex8m,   CBuddyIndex16m,  CBuddyIndex32m, CBuddyIndex64m }
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBuddyIndex128::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available1[ j ];
    m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        m_available0 |= ( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex256::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available1[ j ];
    m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        m_available0 |= ( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex512::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available1[ j ];
    m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        m_available0 |= ( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex1k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available1[ j ];
    m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        m_available0 |= ( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex2k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available1[ j ];
    m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        m_available0 |= ( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex4k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available2[ j ];
    m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available1[ i ];
        m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            m_available0 |= ( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex8k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available2[ j ];
    m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available1[ i ];
        m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            m_available0 |= ( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex16k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available2[ j ];
    m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available1[ i ];
        m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            m_available0 |= ( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex32k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available2[ j ];
    m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available1[ i ];
        m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            m_available0 |= ( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex64k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available2[ j ];
    m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available1[ i ];
        m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            m_available0 |= ( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex128k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available3[ j ];
    m_available3[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available2[ i ];
        m_available2[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available1[ j ];
            m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                m_available0 |= ( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex256k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available3[ j ];
    m_available3[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available2[ i ];
        m_available2[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available1[ j ];
            m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                m_available0 |= ( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex512k::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available3[ j ];
    m_available3[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available2[ i ];
        m_available2[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available1[ j ];
            m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                m_available0 |= ( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex1m::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available3[ j ];
    m_available3[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available2[ i ];
        m_available2[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available1[ j ];
            m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                m_available0 |= ( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex2m::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available3[ j ];
    m_available3[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available2[ i ];
        m_available2[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available1[ j ];
            m_available1[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                m_available0 |= ( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex4m::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available4[ j ];
    m_available4[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available3[ i ];
        m_available3[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available2[ j ];
            m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                i >>= 10;
                available = m_available1[ i ];
                m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
                if( available == 0 )
                {   //  the block is becoming available
                    m_available0 |= ( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

void CBuddyIndex8m::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available4[ j ];
    m_available4[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available3[ i ];
        m_available3[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available2[ j ];
            m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                i >>= 10;
                available = m_available1[ i ];
                m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
                if( available == 0 )
                {   //  the block is becoming available
                    m_available0 |= ( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

void CBuddyIndex16m::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available4[ j ];
    m_available4[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available3[ i ];
        m_available3[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available2[ j ];
            m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                i >>= 10;
                available = m_available1[ i ];
                m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
                if( available == 0 )
                {   //  the block is becoming available
                    m_available0 |= ( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

void CBuddyIndex32m::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available4[ j ];
    m_available4[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available3[ i ];
        m_available3[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available2[ j ];
            m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                i >>= 10;
                available = m_available1[ i ];
                m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
                if( available == 0 )
                {   //  the block is becoming available
                    m_available0 |= ( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

void CBuddyIndex64m::setAvailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = m_available4[ j ];
    m_available4[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
    if( available == 0 )
    {   //  the block is becoming available
        i >>= 10;
        available = m_available3[ i ];
        m_available3[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
        if( available == 0 )
        {   //  the block is becoming available
            j >>= 10;
            available = m_available2[ j ];
            m_available2[ j ] = ( available | ( uint32_t( 1 ) << ( i & 31 ) ) );
            if( available == 0 )
            {   //  the block is becoming available
                i >>= 10;
                available = m_available1[ i ];
                m_available1[ i ] = ( available | ( uint32_t( 1 ) << ( j & 31 ) ) );
                if( available == 0 )
                {   //  the block is becoming available
                    m_available0 |= ( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes setUnavailable() function bodies
////
////    note:
////
////        These functions are identical within each of the following groups:
////
////            { CBuddyIndex128,  CBuddyIndex256,  CBuddyIndex512,  CBuddyIndex1k,  CBuddyIndex2k  }
////            { CBuddyIndex4k,   CBuddyIndex8k,   CBuddyIndex16k,  CBuddyIndex32k, CBuddyIndex64k }
////            { CBuddyIndex128k, CBuddyIndex256k, CBuddyIndex512k, CBuddyIndex1m,  CBuddyIndex2m  }
////            { CBuddyIndex4m,   CBuddyIndex8m,   CBuddyIndex16m,  CBuddyIndex32m, CBuddyIndex64m }
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBuddyIndex128::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available1[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        m_available0 &= ~( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex256::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available1[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        m_available0 &= ~( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex512::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available1[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        m_available0 &= ~( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex1k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available1[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        m_available0 &= ~( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex2k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available1[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        m_available0 &= ~( uint32_t( 1 ) << j );
    }
}

void CBuddyIndex4k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available2[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available1[ i ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            m_available0 &= ~( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex8k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available2[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available1[ i ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            m_available0 &= ~( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex16k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available2[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available1[ i ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            m_available0 &= ~( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex32k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available2[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available1[ i ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            m_available0 &= ~( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex64k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available2[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available1[ i ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            m_available0 &= ~( uint32_t( 1 ) << i );
        }
    }
}

void CBuddyIndex128k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available3[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available3[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available2[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available2[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available1[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                m_available0 &= ~( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex256k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available3[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available3[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available2[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available2[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available1[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                m_available0 &= ~( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex512k::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available3[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available3[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available2[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available2[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available1[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                m_available0 &= ~( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex1m::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available3[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available3[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available2[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available2[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available1[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                m_available0 &= ~( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex2m::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available3[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available3[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available2[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available2[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available1[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available1[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                m_available0 &= ~( uint32_t( 1 ) << j );
            }
        }
    }
}

void CBuddyIndex4m::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available4[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available4[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available3[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available3[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available2[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                i >>= 10;
                available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
                m_available1[ i ] = available;
                if( available == 0 )
                {   //  the block is becoming unavailable
                    m_available0 &= ~( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

void CBuddyIndex8m::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available4[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available4[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available3[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available3[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available2[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                i >>= 10;
                available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
                m_available1[ i ] = available;
                if( available == 0 )
                {   //  the block is becoming unavailable
                    m_available0 &= ~( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

void CBuddyIndex16m::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available4[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available4[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available3[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available3[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available2[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                i >>= 10;
                available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
                m_available1[ i ] = available;
                if( available == 0 )
                {   //  the block is becoming unavailable
                    m_available0 &= ~( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

void CBuddyIndex32m::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available4[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available4[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available3[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available3[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available2[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                i >>= 10;
                available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
                m_available1[ i ] = available;
                if( available == 0 )
                {   //  the block is becoming unavailable
                    m_available0 &= ~( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

void CBuddyIndex64m::setUnavailable( const int index )
{
    int i = ( index >> 1 );
    int j = ( i >> 5 );
    uint32_t available = ( m_available4[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
    m_available4[ j ] = available;
    if( available == 0 )
    {   //  the block is becoming unavailable
        i >>= 10;
        available = ( m_available3[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
        m_available3[ j ] = available;
        if( available == 0 )
        {   //  the block is becoming unavailable
            j >>= 10;
            available = ( m_available2[ j ] & ~( uint32_t( 1 ) << ( i & 31 ) ) );
            m_available2[ j ] = available;
            if( available == 0 )
            {   //  the block is becoming unavailable
                i >>= 10;
                available = ( m_available1[ i ] & ~( uint32_t( 1 ) << ( j & 31 ) ) );
                m_available1[ i ] = available;
                if( available == 0 )
                {   //  the block is becoming unavailable
                    m_available0 &= ~( uint32_t( 1 ) << i );
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end memory namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace memory

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



