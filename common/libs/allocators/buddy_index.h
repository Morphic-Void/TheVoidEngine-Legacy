
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   buddy_index.h
////    Author: Ritchie Brannan
////    Date:   07 Aug 16
////
////    Description:
////
////    	Buddy allocation index management classes.
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

#ifndef	__BUDDY_INDEX_INCLUDED__
#define	__BUDDY_INDEX_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"

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
////    CBuddyIndex classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBuddyIndex32
{
public:
    inline                  CBuddyIndex32() { reset(); };
    inline                  ~CBuddyIndex32() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    inline int              getLevelCount() const;
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    inline void             setAllocated( const int index );
    inline void             setDeallocated( const int index );
    inline int              getAvailable() const;
    static inline int       lowBitIndex( const uint32_t value );
    static const int        c_level = 0;
    static const int        c_slots = ( 32 << c_level );
    int                     m_levels;
    uint32_t                m_allocated;
};

class CBuddyIndex64 : public CBuddyIndex32
{
public:
    inline                  CBuddyIndex64() { reset(); };
    inline                  ~CBuddyIndex64() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    inline void             setAvailable( const int index );
    inline void             setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 1;
    static const int        c_slots = ( 2 * 32 );
    uint32_t                m_available0;
    uint32_t                m_allocated[ c_slots >> 5 ];
};

class CBuddyIndex128 : public CBuddyIndex64
{
public:
    inline                  CBuddyIndex128() { reset(); };
    inline                  ~CBuddyIndex128() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 2;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 6 ];
    uint32_t                m_allocated[  c_slots >> 5 ];
};

class CBuddyIndex256 : public CBuddyIndex128
{
public:
    inline                  CBuddyIndex256() { reset(); };
    inline                  ~CBuddyIndex256() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 3;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 6 ];
    uint32_t                m_allocated[  c_slots >> 5 ];
};

class CBuddyIndex512 : public CBuddyIndex256
{
public:
    inline                  CBuddyIndex512() { reset(); };
    inline                  ~CBuddyIndex512() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 4;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 6 ];
    uint32_t                m_allocated[  c_slots >> 5 ];
};

class CBuddyIndex1k : public CBuddyIndex512
{
public:
    inline                  CBuddyIndex1k() { reset(); };
    inline                  ~CBuddyIndex1k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 5;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 6 ];
    uint32_t                m_allocated[  c_slots >> 5 ];
};

class CBuddyIndex2k : public CBuddyIndex1k
{
public:
    inline                  CBuddyIndex2k() { reset(); };
    inline                  ~CBuddyIndex2k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 6;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 6 ];
    uint32_t                m_allocated[  c_slots >> 5 ];
};

class CBuddyIndex4k : public CBuddyIndex2k
{
public:
    inline                  CBuddyIndex4k() { reset(); };
    inline                  ~CBuddyIndex4k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 7;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 11 ];
    uint32_t                m_available2[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex8k : public CBuddyIndex4k
{
public:
    inline                  CBuddyIndex8k() { reset(); };
    inline                  ~CBuddyIndex8k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 8;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 11 ];
    uint32_t                m_available2[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex16k : public CBuddyIndex8k
{
public:
    inline                  CBuddyIndex16k() { reset(); };
    inline                  ~CBuddyIndex16k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 9;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 11 ];
    uint32_t                m_available2[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex32k : public CBuddyIndex16k
{
public:
    inline                  CBuddyIndex32k() { reset(); };
    inline                  ~CBuddyIndex32k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 10;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 11 ];
    uint32_t                m_available2[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex64k : public CBuddyIndex32k
{
public:
    inline                  CBuddyIndex64k() { reset(); };
    inline                  ~CBuddyIndex64k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 11;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 11 ];
    uint32_t                m_available2[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex128k : public CBuddyIndex64k
{
public:
    inline                  CBuddyIndex128k() { reset(); };
    inline                  ~CBuddyIndex128k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 12;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 16 ];
    uint32_t                m_available2[ c_slots >> 11 ];
    uint32_t                m_available3[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex256k : public CBuddyIndex128k
{
public:
    inline                  CBuddyIndex256k() { reset(); };
    inline                  ~CBuddyIndex256k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 13;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 16 ];
    uint32_t                m_available2[ c_slots >> 11 ];
    uint32_t                m_available3[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex512k : public CBuddyIndex256k
{
public:
    inline                  CBuddyIndex512k() { reset(); };
    inline                  ~CBuddyIndex512k() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 14;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 16 ];
    uint32_t                m_available2[ c_slots >> 11 ];
    uint32_t                m_available3[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex1m : public CBuddyIndex512k
{
public:
    inline                  CBuddyIndex1m() { reset(); };
    inline                  ~CBuddyIndex1m() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 15;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 16 ];
    uint32_t                m_available2[ c_slots >> 11 ];
    uint32_t                m_available3[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex2m : public CBuddyIndex1m
{
public:
    inline                  CBuddyIndex2m() { reset(); };
    inline                  ~CBuddyIndex2m() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 16;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 16 ];
    uint32_t                m_available2[ c_slots >> 11 ];
    uint32_t                m_available3[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex4m : public CBuddyIndex2m
{
public:
    inline                  CBuddyIndex4m() { reset(); };
    inline                  ~CBuddyIndex4m() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 17;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 21 ];
    uint32_t                m_available2[ c_slots >> 16 ];
    uint32_t                m_available3[ c_slots >> 11 ];
    uint32_t                m_available4[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex8m : public CBuddyIndex4m
{
public:
    inline                  CBuddyIndex8m() { reset(); };
    inline                  ~CBuddyIndex8m() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 18;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 21 ];
    uint32_t                m_available2[ c_slots >> 16 ];
    uint32_t                m_available3[ c_slots >> 11 ];
    uint32_t                m_available4[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex16m : public CBuddyIndex8m
{
public:
    inline                  CBuddyIndex16m() { reset(); };
    inline                  ~CBuddyIndex16m() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 19;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 21 ];
    uint32_t                m_available2[ c_slots >> 16 ];
    uint32_t                m_available3[ c_slots >> 11 ];
    uint32_t                m_available4[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex32m : public CBuddyIndex16m
{
public:
    inline                  CBuddyIndex32m() { reset(); };
    inline                  ~CBuddyIndex32m() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 20;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 21 ];
    uint32_t                m_available2[ c_slots >> 16 ];
    uint32_t                m_available3[ c_slots >> 11 ];
    uint32_t                m_available4[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

class CBuddyIndex64m : public CBuddyIndex32m
{
public:
    inline                  CBuddyIndex64m() { reset(); };
    inline                  ~CBuddyIndex64m() { reset(); };
    static inline bool      isValidIndex( const int index );
    static inline int       getSlotCount();
    void                    reset();
    int                     allocate( const uint slots );
    bool                    deallocate( const int index );
protected:
    inline int              doAllocation();
    bool                    doDeallocation( const int index );
    inline bool             isAllocated( const int index ) const;
    void                    setAllocated( const int index );
    void                    setDeallocated( const int index );
    int                     getAvailable() const;
    void                    setAvailable( const int index );
    void                    setUnavailable( const int index );
    inline int              getAvailablePair() const;
    inline void             setAvailablePair( const int index );
    inline void             setUnavailablePair( const int index );
    static const int        c_level = 21;
    static const int        c_slots = ( 32 << c_level );
    uint32_t                m_available0;
    uint32_t                m_available1[ c_slots >> 21 ];
    uint32_t                m_available2[ c_slots >> 16 ];
    uint32_t                m_available3[ c_slots >> 11 ];
    uint32_t                m_available4[ c_slots >>  6 ];
    uint32_t                m_allocated[  c_slots >>  5 ];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes inline getLevelCount() level count function body
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CBuddyIndex32::getLevelCount() const { return( m_levels ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes inline isValidIndex() index checking function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool   CBuddyIndex32::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex64::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex128::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex256::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex512::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex1k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex2k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex4k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex8k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex16k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex32k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex64k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool CBuddyIndex128k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool CBuddyIndex256k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool CBuddyIndex512k::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex1m::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex2m::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex4m::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool   CBuddyIndex8m::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex16m::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex32m::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };
bool  CBuddyIndex64m::isValidIndex( const int index ) { return( ( static_cast< uint >( index ) < static_cast< uint >( c_slots ) ) ? true : false ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CBuddyIndex classes inline getSlotCount() slot count function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int   CBuddyIndex32::getSlotCount() { return( c_slots ); };
int   CBuddyIndex64::getSlotCount() { return( c_slots ); };
int  CBuddyIndex128::getSlotCount() { return( c_slots ); };
int  CBuddyIndex256::getSlotCount() { return( c_slots ); };
int  CBuddyIndex512::getSlotCount() { return( c_slots ); };
int   CBuddyIndex1k::getSlotCount() { return( c_slots ); };
int   CBuddyIndex2k::getSlotCount() { return( c_slots ); };
int   CBuddyIndex4k::getSlotCount() { return( c_slots ); };
int   CBuddyIndex8k::getSlotCount() { return( c_slots ); };
int  CBuddyIndex16k::getSlotCount() { return( c_slots ); };
int  CBuddyIndex32k::getSlotCount() { return( c_slots ); };
int  CBuddyIndex64k::getSlotCount() { return( c_slots ); };
int CBuddyIndex128k::getSlotCount() { return( c_slots ); };
int CBuddyIndex256k::getSlotCount() { return( c_slots ); };
int CBuddyIndex512k::getSlotCount() { return( c_slots ); };
int   CBuddyIndex1m::getSlotCount() { return( c_slots ); };
int   CBuddyIndex2m::getSlotCount() { return( c_slots ); };
int   CBuddyIndex4m::getSlotCount() { return( c_slots ); };
int   CBuddyIndex8m::getSlotCount() { return( c_slots ); };
int  CBuddyIndex16m::getSlotCount() { return( c_slots ); };
int  CBuddyIndex32m::getSlotCount() { return( c_slots ); };
int  CBuddyIndex64m::getSlotCount() { return( c_slots ); };

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
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__BUDDY_INDEX_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

