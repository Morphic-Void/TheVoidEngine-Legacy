
////    File:   array_sort.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	AVL array sorting class.

#pragma once

#ifndef	__ARRAY_SORT_INCLUDED__
#define	__ARRAY_SORT_INCLUDED__

#include "libs/system/base/types.h"

////    Array query interface
////
////    notes:
////
////        getKey()    :   returns a key.
////        cmpKey()    :   compares a key against the the indexed item.
////        moveItem()  :   moves an item from one indexed location to another.
////
////        key will never be nullptr.
////
////        index will never be negative.
////
////        source and target will never be the same value.
////
////        source or target but not both may have a value of -1 indicating that the location is temporary storage.
////
////        Pointers returned by the getKey() function are not cached, only the most recently returned pointer is required.

INTERFACE_BEGIN(IArrayQuery)
virtual const void*     getKey(const int index) const = 0;
virtual int             cmpKey(const void* const key, const int index) const = 0;
virtual void            moveItem(const int source, const int target) = 0;
INTERFACE_END();

class ArraySort
{
public:
    inline                  ArraySort() : m_rootIndex(-1), m_freeIndex(-1), m_nodeCount(0), m_usedCount(0), m_peakCount(0), m_nodes(nullptr), m_query(nullptr) {};
    inline                  ArraySort(IArrayQuery& query, const uint count = 0) : m_rootIndex(-1), m_freeIndex(-1), m_nodeCount(0), m_usedCount(0), m_peakCount(0), m_nodes(nullptr), m_query(nullptr) { init(query, count); };
    virtual                 ~ArraySort() { kill(); };
    inline bool             valid() const { return(((m_nodes != nullptr) && (m_query != nullptr)) ? true : false); };
    bool                    init(IArrayQuery& query, const uint count = 0);
    void                    kill();
    void                    reset();
    void                    clone(const ArraySort& source);
    inline bool             contains(const int index) const { return((valid() && (static_cast<uint>(index) < static_cast<uint>(m_nodeCount)) && (m_nodes[index].flag == k_used)) ? true : false); };
    int                     insert(const void* const key);
    bool                    update(const int index);
    bool                    remove(const int index);
    bool                    relink();
    bool                    sort();
    int*                    rank() const;
    int                     rank(const int index) const;
    int                     find(const void* const key) const;
    int                     prev(const int index) const;
    int                     next(const int index) const;
    int                     root() const;
    int                     first() const;
    int                     last() const;
    inline int              used() const { return(m_usedCount); };
    inline int              size() const { return(m_peakCount); };
    inline int              height() const { return((m_nodes != nullptr) ? height(m_rootIndex) : 0); };
    inline int              weight() const { return((m_nodes != nullptr) ? weight(m_rootIndex) : 0); };
    inline bool             validate() const { return(((m_nodes != nullptr) && (m_query != nullptr)) ? validate(m_rootIndex) : false); };
protected:
    struct                  Node { int parent; int children[2]; short balance; short flag; };
    int                     allocate();
    void                    recycle(const int index);
    int                     linear();
    int                     sorted(const int parent, const int lower, const int upper);
    inline int              sorted() { return(sorted(-1, 0, (m_usedCount - 1))); };
    void                    extend(const int lower, const int upper);
    inline void             extend() { extend(m_usedCount, (m_nodeCount - 1)); };
    bool                    insert(const int index, const void* const key);
    bool                    unlink(const int index);
    void				    rotate(const int index);
    int					    height(const int index) const;
    int					    weight(const int index) const;
    bool				    validate(const int index) const;
    int                     m_rootIndex;    //  index of the root node
    int                     m_freeIndex;    //  index of the first free node
    int                     m_nodeCount;    //  count of nodes allocated
    int                     m_usedCount;    //  count of nodes occupied
    int                     m_peakCount;    //  peak count of nodes occupied
    Node*                   m_nodes;        //  array of allocated nodes
    IArrayQuery*            m_query;        //  indexable data query object
    static const short      k_used = 0x1248;
    static const short      k_free = 0x4182;
private:
    inline                  ArraySort(const ArraySort&) : m_rootIndex(-1), m_freeIndex(-1), m_nodeCount(0), m_usedCount(0), m_peakCount(0), m_nodes(nullptr), m_query(nullptr) {};
    inline void             operator=(const ArraySort&) {};
};

#endif	//	#ifndef	__ARRAY_SORT_INCLUDED__


