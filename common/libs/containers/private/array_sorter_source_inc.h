
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   array_sorter_source_inc.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	AVL array sorting class.
////
////        This file is expected to be included multiple times.
////
////        This file together with its companion (array_sorter_header_inc.h) declares variants of the CArraySorter class.
////
////        While the variants could be created through the use of templates, this include format reduces the compile and
////        link times for non-unity builds by reducing the overall size and complexity of the includes and generating fewer
////        redundant copies of the compiled code in the object files.
////
////        Advantages of using these classes over the similar STL equivalents include: ideal for things which are naturally
////        in dynamic arrays but where O(log(n)) searching is desirable; very fast O(1) access to array elements; memory locality;
////        use as a non-intrusive or external sorting container; optimised in-place physical reordering of array elements;
////        additional search options; a one stop shop for maps, sets, multimaps and multisets; use with arrays and vectors;
////        low impact on compile and link times; and additional functionality such as calculation of sort indices and ranking.
////
////        Disadvantages include: this not a standard STL container and so is unfamiliar to most programmers; the classes
////        do not have iterators (though iterators could be implemented); for items that are not naturally going to be in an
////        array, the STL equivalents may be a better fit; memory allocation is handled as for an array of objects so adding
////        a single item can be expensive if the array needs to resize.
////
////    Notes:
////
////        notes about the public and virtual functions can be found in array_sort.h
////
////        protected functions:
////
////            enlarge( count, aggressive )    :   allocate or reallocate the array (can only increase the size)
////            acquire( index, aggressive )    :   acquire an index
////            recycle( index )                :   recycle an index to the pool of unused elements
////            ranking( ranks )                :   internal helper for the rank() functions
////            linear()                        :   internal helper function for sort() and relink()
////            sorted( parent, lower, upper )  :   construct a perfectly balanced sub-tree
////            sorted()                        :   internal helper function for sort()
////            extend( lower, upper )          :   adds the specified inclusive index range of nodes to the free list
////            extend()                        :   internal helper function for reset(), sort() and acquire()
////            insert( index, key )            :   insert a node into the AVL tree
////            unlink( index )                 :   unlink a node from the AVL tree
////            rotate( index )                 :   perform a compound AVL rebalancing rotate
////            locate()                        :   find any element equal to key (element == key == element)
////            locate_first_eq()               :   first element equal to key (element == key == element)
////            locate_first_gt()               :   first element greater than key (element > key or key < element)
////            locate_first_ge()               :   first element greater than or equal to key (element >= key or key <= element)
////            locate_last_eq()                :   last element equal to key (element == key == element)
////            locate_last_lt()                :   last element less than key (element < key or key > element)
////            locate_last_le()                :   last element less than or equal to key (element <= key or key >= element)
////            min_occupied()                  :   lowest index of an occupied element (or -1 if empty)
////            max_occupied()                  :   highest index of an occupied element (or -1 if empty)
////            height( index )                 :   recursively calculate the maximum height of the tree starting at index
////            weight( index )                 :   recursively calculate the weight of the tree starting at index
////            validate( index )               :   recursively validate the tree starting at index
////            take_obj( take_me )             :   helper for the move constructor and operator
////            clone_obj( clone_me )           :   helper for the copy constructor and operator
////            zero()                          :   set the object to an empty state
////
////            additional detail:
////
////                acquire( index, aggressive ):
////
////                    If the specified index is negative, the index will be allocated from the free list.
////                    If the free list is empty, the array will be expanded to allow for more items.
////
////                    If the specified index is beyond the current array extents, the array will be expanded
////                    to accomodate it.
////
////                    If the specified index refers to an occupied element, the element will be unlinked
////                    and added to the free list before it is re-allocated.
////
////                ranking( ranks ):
////
////                    Constructs the lexical index array for the rank() and rank( ranks ) functions.
////
////                linear():
////
////                    Converts the tree into a sorted looping bi-directional linked list.
////
////                    The function returns the index of the first item in the list.
////
////                sorted( parent, lower, upper ):
////
////                    Constructs a perfectly balanced sub-tree.
////
////                    The sub-tree will contain the the indicated lower to upper inclusive index range of nodes.
////                    The parent index of the sub-tree root will be parent.
////
////                    Linking the parent node to the root is expected to be the responsibility of the caller.
////
////                    This function returns the root index of the new sub-tree.
////
////                sorted():
////
////                    Calls sorted( parent, lower, upper ) with standard params. The parent index is -1 indicating
////                    that the sub-tree is the entire tree. The lower index is the 0 and the upper index is one
////                    less than the used node count.
////
////                    This function returns the root index of the new sub-tree.
////
////                extend( lower, upper ):
////
////                    Adds the indicated lower to upper inclusive index range to the free list.
////
////                    It is imperative that the nodes being added are not in the node tree or free list.
////
////                extend():
////
////                    Calls extend( lower, upper ) with standard params. The lower index is the used node count and
////                    the upper index is the highest allocated node index.
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

#include "array_sorter_header_inc.h"
#include <memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Set up sorting structure configuration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (INCLUDE_AS_16BIT)
#define TClass  CArraySorter16
#define TIndex  int16_t
#define TOther  int8_t
#else
#define TClass  CArraySorter32
#define TIndex  int32_t
#define TOther  int16_t
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CArraySorter class public function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TClass::init( const uint count )
{
    kill();
    if( count && ( count <= k_high ) )
    {
        Node* nodes = new Node[ count ];
        if( nodes != NULL )
        {   //  allocation succeeded
            m_nodeCount = count;
            m_nodes = nodes;
            extend();
            return( true );
        }
    }
    return( false );
}

void TClass::kill()
{
    if( m_nodes != NULL )
    {
        delete[] m_nodes;
    }
    zero();
}

void TClass::reset()
{
    if( valid() )
    {
        m_rootIndex = -1;
        m_freeIndex = -1;
        m_usedCount = 0;
        m_peakCount = 0;
        extend();
    }
}

bool TClass::reserve( const uint count )
{
    if( valid() )
    {
        uint check = ( m_nodeCount - m_usedCount );
        if( check < count )
        {
            uint limit = ( k_high - m_usedCount );
            if( limit < count )
            {   //  not expandable to accomodate the requested reserve
                return( false );
            }
            uint nodeCount = ( m_nodeCount + ( count - check ) );
            return( enlarge( nodeCount, false ) );
        }
        return( true );
    }
    return( init( count ) );
}

bool TClass::shrink_to_fit()
{
    if( valid() )
    {
        int lastIndex = max_occupied();
        if( lastIndex >= 0 )
        {
            uint nodeCount = ( static_cast< uint >( lastIndex ) + 1 );
            Node* nodes = new Node[ nodeCount ];
            if( nodes != NULL )
            {   //  allocation succeeded
                memcpy_s( reinterpret_cast< void* >( nodes ), ( sizeof( Node ) * nodeCount ), reinterpret_cast< const void* >( m_nodes ), ( sizeof( Node ) * nodeCount ) );
                delete[] m_nodes;
                m_nodes = nodes;
                m_nodeCount = nodeCount;
                m_freeIndex = -1;
                uint freeCount = ( m_nodeCount - m_usedCount );
                if( freeCount )
                {   //  free items exist
                    int scan = 0;
                    while( m_nodes[ scan ].flag != k_free ) ++scan;
                    Node& head_node = m_nodes[ scan ];
                    head_node.children[ 0 ] = head_node.children[ 1 ] = -1;
                    head_node.parent = -1;
                    head_node.balance = 0;
                    int head = scan;
                    while( --freeCount )
                    {
                        int tail = scan;
                        ++scan;
                        while( m_nodes[ scan ].flag != k_free ) ++scan;
                        Node& tail_node = m_nodes[ scan ];
                        m_nodes[ head ].children[ 0 ] = m_nodes[ tail ].children[ 1 ] = static_cast< TIndex >( scan );
                        tail_node.children[ 0 ] = static_cast< TIndex >( tail );
                        tail_node.children[ 1 ] = static_cast< TIndex >( head );
                        tail_node.parent = -1;
                        tail_node.balance = 0;
                    }
                    m_freeIndex = head;
                    resize( m_nodeCount );
                }
                return( true );
            }
        }
        else
        {   //  no occupied elements so delete the array
            kill();
            resize( 0 );
            return( true );
        }
    }
    return( false );
}

int TClass::insert( const int index, const bool unique )
{
    if( !valid() )
    {   //  ensure that we are in a safe state if no initialisation has been detected
        zero();
    }
    if( ( static_cast< uint >( index ) < m_nodeCount ) && ( m_nodes[ index ].flag == k_used ) )
    {   //  the index references a currently occupied element
        if( !unlink( index ) )
        {   //  the element could not be unlinked
            return( -1 );
        }
        recycle( index );
    }
    if( !unique || ( locate() < 0 ) )
    {   //  the item being inserted is unique or not required to be unique
        int nodeIndex = acquire( index, true );
        if( nodeIndex >= 0 )
        {   //  index acquisition succeeded
            if( insert( nodeIndex, -1 ) )
            {   //  the node was successfully inserted
                return( nodeIndex );
            }
            recycle( nodeIndex );
        }
    }
    return( -1 );
}

bool TClass::update( const int index )
{   //  resort a single key : required if the indexed data key has been modified
    if( contains( index ) && unlink( index ) )
    {
        return( insert( index, index ) );
    }
    return( false );
}

bool TClass::remove( const int index )
{
    if( contains( index ) && unlink( index ) )
    {
        recycle( index );
        return( true );
    }
    return( false );
}

//  relink()
//
//      Optimises search performance, though gains are liable to be marginal.
//
//      This function should also be called if the indexed data array is modified.
//
bool TClass::relink()
{   //  resort the array : required if the indexed data array is modified
    if( valid() )
    {
        uint count = m_usedCount;
        if( count > 1 )
        {
            int list = linear();
            m_rootIndex = -1;
            while( count )
            {
                int index = list;
                Node& node = m_nodes[ index ];
                list = node.children[ 1 ];
                m_nodes[ node.children[ 0 ] ].children[ 1 ] = node.children[ 1 ];
                m_nodes[ node.children[ 1 ] ].children[ 0 ] = node.children[ 0 ];
                node.parent = -1;
                node.children[ 0 ] = node.children[ 1 ] = -1;
                node.balance = 0;
                if( !insert( index, index ) )
                {
                    return( false );
                }
                --count;
            }
        }
        return( true );
    }
    return( false );
}

bool TClass::sort()
{   //  sort the array contents removing any gaps
    if( valid() )
    {
        uint count = m_usedCount;
        if( count )
        {
            int list = linear();
            for( uint check = count; check; --check )
            {   //  move any items whose target location is unoccupied
                Node& node = m_nodes[ list ];
                int source = list;
                int target = node.parent;
                if( ( source == target ) || ( m_nodes[ target ].flag == k_free ) )
                {   //  target is free
                    move( source, target );
                    node.flag = k_free;
                    m_nodes[ node.children[ 0 ] ].children[ 1 ] = node.children[ 1 ];
                    m_nodes[ node.children[ 1 ] ].children[ 0 ] = node.children[ 0 ];
                    if( static_cast< uint >( source ) < m_usedCount )
                    {
                        check = count;
                    }
                    --count;
                }
                list = node.children[ 1 ];
            }
            while( count )
            {   //  all remaining items must be in cyclic lists, so rotate them
                int cyclic = list;
                int source = -1;
                int target = cyclic;
                do
                {   //  unlink the cycle and set the source index into children[ 1 ]
                    Node& node = m_nodes[ target ];
                    list = node.children[ 1 ];
                    m_nodes[ node.children[ 0 ] ].children[ 1 ] = node.children[ 1 ];
                    m_nodes[ node.children[ 1 ] ].children[ 0 ] = node.children[ 0 ];
                    node.children[ 1 ] = static_cast< TIndex >( source );
                    source = target;
                    target = node.parent;
                    --count;
                } while( target != cyclic );
                move( source, -1 );
                while( ( target = source ) >= 0 )
                {
                    source = m_nodes[ target ].children[ 1 ];
                    move( source, target );
                }
            }
            m_rootIndex = sorted();
            m_freeIndex = -1;
            extend();
        }
        return( true );
    }
    return( false );
}

int* TClass::rank() const
{   //  return an array of the rank of all items (i.e. the indicies that the items would have if the array was sorted)
    int* ranks = NULL;
    if( valid() )
    {
        ranks = new int[ m_nodeCount ];
        ranking( ranks );
    }
    return( ranks );
}

bool TClass::rank( int* const ranks ) const
{
    if( valid() && ( ranks != NULL ) )
    {
        ranking( ranks );
        return( true );
    }
    return( false );
}

int TClass::rank( const int index ) const
{   //  return the rank of the index (i.e. the index that this item would have if the array was sorted)
    int count = -1;
    if( contains( index ) )
    {
        int found = index;
        while( found >= 0 )
        {   //  count the number of times we can step to a previous node
            ++count;
            int check = m_nodes[ found ].children[ 0 ];
            if( check < 0 )
            {
                for( check = found; ( ( ( found = m_nodes[ check ].parent ) >= 0 ) && ( m_nodes[ found ].children[ 1 ] != check ) ); check = found );
            }
            else
            {
                for( found = check; ( ( check = m_nodes[ found ].children[ 1 ] ) >= 0 ); found = check );
            }
        }
    }
    return( count );
}

int TClass::prev( const int index ) const
{
    int found = -1;
    if( contains( index ) )
    {
        int check = m_nodes[ index ].children[ 0 ];
        if( check < 0 )
        {
            for( check = index; ( ( ( found = m_nodes[ check ].parent ) >= 0 ) && ( m_nodes[ found ].children[ 1 ] != check ) ); check = found );
        }
        else
        {
            for( found = check; ( ( check = m_nodes[ found ].children[ 1 ] ) >= 0 ); found = check );
        }
    }
    return( found );
}

int TClass::next( const int index ) const
{
    int found = -1;
    if( contains( index ) )
    {
        int check = m_nodes[ index ].children[ 1 ];
        if( check < 0 )
        {
            for( check = index; ( ( ( found = m_nodes[ check ].parent ) >= 0 ) && ( m_nodes[ found ].children[ 0 ] != check ) ); check = found );
        }
        else
        {
            for( found = check; ( ( check = m_nodes[ found ].children[ 0 ] ) >= 0 ); found = check );
        }
    }
    return( found );
}

int TClass::root() const
{
    return( ( m_nodes != NULL ) ? m_rootIndex : -1 );
}

int TClass::first() const
{
    int found = -1;
    if( m_nodes != NULL )
    {
        for( int check = m_rootIndex; check >= 0; check = m_nodes[ found ].children[ 0 ] ) found = check;
    }
    return( found );
}

int TClass::last() const
{
    int found = -1;
    if( m_nodes != NULL )
    {
        for( int check = m_rootIndex; check >= 0; check = m_nodes[ found ].children[ 1 ] ) found = check;
    }
    return( found );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CArraySorter class protected function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TClass::enlarge( const uint count, const bool aggressive )
{
    if( count <= k_high )
    {
        uint nodeCount = count;
        if( aggressive )
        {
            nodeCount += ( ( nodeCount >> 1 ) + 1 );
            if( nodeCount < 32 )
            {
                nodeCount = 32;
            }
            else if( nodeCount > k_high )
            {
                nodeCount = k_high;
            }
        }
        if( nodeCount > m_nodeCount )
        {
            Node* nodes = new Node[ nodeCount ];
            if( nodes != NULL )
            {   //  allocation succeeded
                if( m_nodeCount && ( m_nodes != NULL ) )
                {
                    memcpy_s( reinterpret_cast< void* >( nodes ), ( sizeof( Node ) * nodeCount ), reinterpret_cast< const void* >( m_nodes ), ( sizeof( Node ) * m_nodeCount ) );
                    delete[] m_nodes;
                }
                m_nodes = nodes;
                extend( static_cast< int >( m_nodeCount ), static_cast< int >( nodeCount - 1 ) );
                m_nodeCount = nodeCount;
                resize( nodeCount );
                return( true );
            }
        }
    }
    return( false );
}

int TClass::acquire( const int index, const bool aggressive )
{
    int nodeIndex = ( ( index < 0 ) ? ( ( m_freeIndex < 0 ) ? static_cast< int >( m_nodeCount ) : m_freeIndex ) : index );
    uint nodeCount = static_cast< uint >( nodeIndex + 1 );
    if( ( nodeCount <= m_nodeCount ) || enlarge( nodeCount, aggressive ) )
    {   //  the nodeIndex is within the array or the newly enlarged array
        Node& node = m_nodes[ nodeIndex ];
        if( ( node.flag == k_free ) || unlink( nodeIndex ) )
        {   //  the item was already in the free list or was successfully unlinked
            if( m_freeIndex == nodeIndex )
            {
                m_freeIndex = ( ( node.children[ 1 ] == nodeIndex ) ? -1 : node.children[ 1 ] );
            }
            m_nodes[ node.children[ 0 ] ].children[ 1 ] = node.children[ 1 ];
            m_nodes[ node.children[ 1 ] ].children[ 0 ] = node.children[ 0 ];
            node.parent = node.children[ 0 ] = node.children[ 1 ] = -1;
            node.balance = 0;
            node.flag = k_used;
            ++m_usedCount;
            if( m_peakCount < m_usedCount )
            {
                m_peakCount = m_usedCount;
            }
            return( nodeIndex );
        }
    }
    return( -1 );
}

void TClass::recycle( const int index )
{   //  adds the indicated node to the start of the free list
    Node& node = m_nodes[ index ];
    if( m_freeIndex >= 0 )
    {
        node.children[ 0 ] = m_nodes[ m_freeIndex ].children[ 0 ];
        node.children[ 1 ] = static_cast< TIndex >( m_freeIndex );
        m_nodes[ node.children[ 0 ] ].children[ 1 ] = static_cast< TIndex >( index );
        m_nodes[ node.children[ 1 ] ].children[ 0 ] = static_cast< TIndex >( index );
    }
    else
    {
        node.children[ 0 ] = node.children[ 1 ] = static_cast< TIndex >( index );
    }
    m_freeIndex = index;
    node.parent = -1;
    node.balance = 0;
    node.flag = k_free;
    --m_usedCount;
}

void TClass::ranking( int* const ranks ) const
{
    memset( ranks, -1, ( m_nodeCount * sizeof( int ) ) );
    int index = m_rootIndex;
    for( int check = index; check >= 0; check = m_nodes[ index ].children[ 0 ] ) index = check;
    for( int rank = 0; index >= 0; ++rank )
    {   //  step through the nodes from first to last and assign their ranks
        ranks[ index ] = rank;
        int check = m_nodes[ index ].children[ 1 ];
        if( check < 0 )
        {
            for( check = index; ( ( ( index = m_nodes[ check ].parent ) >= 0 ) && ( m_nodes[ index ].children[ 0 ] != check ) ); check = index );
        }
        else
        {
            for( index = check; ( ( check = m_nodes[ index ].children[ 0 ] ) >= 0 ); index = check );
        }
    }
}

int TClass::linear()
{   //  convert the tree to a sorted bi-directional circular list (for use by the sort() and relink() functions)
    int prev, next;
    int root = -1;
    int last = -1;
    for( next = m_rootIndex; next >= 0; next = m_nodes[ last ].children[ 1 ] ) last = next;
    if( last >= 0 )
    {
        int item = last;
        while( item >= 0 )
        {   //  create a sorted list with the links in children[ 1 ]
            Node& node = m_nodes[ item ];
            node.children[ 1 ] = static_cast< TIndex >( root );
            root = item;
            if( ( item = node.children[ 0 ] ) < 0 )
            {
                prev = root;
                while( ( ( item = m_nodes[ prev ].parent ) >= 0 ) && ( m_nodes[ item ].children[ 1 ] != prev ) )
                {
                    prev = item;
                }
            }
            else
            {
                while( ( prev = m_nodes[ item ].children[ 1 ] ) >= 0 )
                {
                    item = prev;
                }
            }
        }
        prev = last;
        int index = 0;
        for( item = root; item >= 0; item = next )
        {   //  make the list bi-directional and set the parent index to be the linear ordered index of the node
            Node& node = m_nodes[ item ];
            node.parent = static_cast< TIndex >( index );
            node.children[ 0 ] = static_cast< TIndex >( prev );
            next = node.children[ 1 ];
            prev = item;
            ++index;
        }
        m_nodes[ last ].children[ 1 ] = static_cast< TIndex >( root );
    }
    return( root );
}

int TClass::sorted( const int parent, const int lower, const int upper )
{   //  create a contiguous balanced tree for sorted data (note: lower and upper represent an inclusive range)
    int index = -1;
    if( lower <= upper )
    {
        index = static_cast< int >( static_cast< uint >( lower + upper ) >> 1 );
        Node& node = m_nodes[ index ];
        node.parent = static_cast< TIndex >( parent );
        node.children[ 0 ] = static_cast< TIndex >( sorted( index, lower, ( index - 1 ) ) );
        node.children[ 1 ] = static_cast< TIndex >( sorted( index, ( index + 1 ), upper ) );
        node.balance = static_cast< TOther >( upper + lower - index - index );
        node.flag = k_used;
    }
    return( index );
}

void TClass::extend( const int lower, const int upper )
{   //  add the inclusive lower to upper index range to the bi-directional looped linked list of free nodex
    for( uint index = lower; static_cast< uint >( index ) <= static_cast< uint >( upper ); ++index )
    {
        Node& node = m_nodes[ index ];
        node.parent = -1;
        node.children[ 0 ] = static_cast< TIndex >(  index - 1 );
        node.children[ 1 ] = static_cast< TIndex >(  index + 1 );
        node.balance = 0;
        node.flag = k_free;
    }
    int freeIndex = m_freeIndex;
    if( freeIndex < 0 )
    {
        m_freeIndex = lower;
        m_nodes[ upper ].children[ 1 ] = static_cast< TIndex >( lower );
        m_nodes[ lower ].children[ 0 ] = static_cast< TIndex >( upper );
    }
    else
    {
        int tailIndex = m_nodes[ freeIndex ].children[ 0 ];
        m_nodes[ upper ].children[ 1 ] = static_cast< TIndex >( freeIndex );
        m_nodes[ lower ].children[ 0 ] = static_cast< TIndex >( tailIndex );
        m_nodes[ tailIndex ].children[ 1 ] = static_cast< TIndex >( lower );
        m_nodes[ freeIndex ].children[ 0 ] = static_cast< TIndex >( upper );
    }
}

bool TClass::insert( const int index, const int key )
{
    if( index >= 0 )
    {
        Node& insertNode = m_nodes[ index ];
        int parent = m_rootIndex;
        int child = index;
        if( parent >= 0 )
        {   //  this is not the first node to be inserted so will need sorting
            for(;;)
            {
                int compare = cmp( key, parent );
                Node& parentNode = m_nodes[ parent ];
                int flip = ( ( compare >> ( ( sizeof( int ) << 3 ) - 1 ) ) + 1 );
                if( parentNode.children[ flip ] < 0 )
                {
                    parentNode.children[ flip ] = static_cast< TIndex >( index );
                    insertNode.parent = static_cast< TIndex >( parent );
                    break;
                }
                parent = parentNode.children[ flip ];
            }
            while( parent >= 0 )
            {
                Node& parentNode = m_nodes[ parent ];
                TOther balance = ( ( parentNode.children[ 0 ] == child ) ? -1 : 1 );
                if( ( parentNode.balance - balance ) == 0 )
                {
                    Node& childNode = m_nodes[ child ];
                    if( ( childNode.balance + balance ) == 0 )
                    {
    	                child = childNode.children[ -( balance >> 1 ) ];
    	                rotate( child );
                    }
                    parentNode.balance += balance;
                    rotate( child );
                }
                else
                {
                    parentNode.balance += balance;
                    child = parent;
                }
                if( m_nodes[ child ].balance == 0 )
                {
                    for( int check = -1; ( check = m_nodes[ child ].parent ) >= 0; child = check );
                }
                parent = m_nodes[ child ].parent;
            }
        }
        m_rootIndex = child;
        return( true );
    }
    return( false );
}

bool TClass::unlink( const int index )
{
    if( index >= 0 )
    {
        Node& unlinkNode = m_nodes[ index ];
        int child = index;
        int flip = ( ( unlinkNode.balance < 0 ) ? 0 : 1 );
        if( unlinkNode.children[ flip ] >= 0 )
        {
            child = unlinkNode.children[ flip ];
            flip ^= 1;
            for( int check = -1; ( ( check = m_nodes[ child ].children[ flip ] ) >= 0 ); child = check );
        }
        flip ^= 1;
        rotate( m_nodes[ child ].children[ flip ] );
        int parent = m_nodes[ child ].parent;
        if( parent >= 0 )
        {   //  not the last node in the tree
            if( child != index )
            {   //  exchange the linkage of the child node and the node being removed
                if( parent == index ) parent = child;
                if( unlinkNode.parent >= 0 )
                {
                    Node& parentNode = m_nodes[ unlinkNode.parent ];
                    parentNode.children[ ( parentNode.children[ 0 ] == index ) ? 0 : 1 ] = static_cast< TIndex >( child );
                }
                if( unlinkNode.children[ 0 ] >= 0 ) m_nodes[ unlinkNode.children[ 0 ] ].parent = static_cast< TIndex >( child );
                if( unlinkNode.children[ 1 ] >= 0 ) m_nodes[ unlinkNode.children[ 1 ] ].parent = static_cast< TIndex >( child );
                Node& childNode = m_nodes[ child ];
                childNode.parent = unlinkNode.parent;
                childNode.children[ 0 ] = unlinkNode.children[ 0 ];
                childNode.children[ 1 ] = unlinkNode.children[ 1 ];
                childNode.balance = unlinkNode.balance;
                unlinkNode.parent = static_cast< TIndex >( parent );
                unlinkNode.children[ 0 ] = unlinkNode.children[ 1 ] = -1;
                unlinkNode.balance = 0;
                Node& parentNode = m_nodes[ parent ];
		        parentNode.children[ ( parentNode.children[ 0 ] == child ) ? 0 : 1 ] = static_cast< TIndex >( index );
                child = index;
            }
            while( parent >= 0 )
            {
                Node& parentNode = m_nodes[ parent ];
                TOther balance = ( ( parentNode.children[ 0 ] == child ) ? 1 : -1 );
                if( ( parentNode.balance - balance ) == 0 )
                {
                    child = parentNode.children[ ( balance + 1 ) >> 1 ];
                    Node& childNode = m_nodes[ child ];
                    if( ( childNode.balance + balance ) == 0 )
                    {
                        child = childNode.children[ -( balance >> 1 ) ];
                        rotate( child );
                    }
                    parentNode.balance += balance;
                    rotate( child );
                }
                else
                {
                    parentNode.balance += balance;
                    child = parent;
                }
                if( m_nodes[ child ].balance != 0 )
                {
                    for( int check = -1; ( check = m_nodes[ child ].parent ) >= 0; child = check );
                }
                parent = m_nodes[ child ].parent;
            }
            Node& parentNode = m_nodes[ unlinkNode.parent ];
            parentNode.children[ ( parentNode.children[ 0 ] == index ) ? 0 : 1 ] = -1;
            unlinkNode.parent = -1;
        }
        else
        {   //  this was the last node in the tree
            child = -1;
        }
        m_rootIndex = child;
        return( true );
    }
    return( false );
}

void TClass::rotate( const int index )
{   //  rotate a node to it's parent's position
    if( index >= 0 )
    {
        Node& rotateNode = m_nodes[ index ];
        int parent = rotateNode.parent;
        if( parent >= 0 )
        {
            Node& parentNode = m_nodes[ parent ];
            int gramps = parentNode.parent;
            if( gramps >= 0 )
            {
                Node& grampsNode = m_nodes[ gramps ];
                grampsNode.children[ ( grampsNode.children[ 0 ] == parent ) ? 0 : 1 ] = static_cast< TIndex >( index );
            }
            rotateNode.parent = static_cast< TIndex >( gramps );
            parentNode.parent = static_cast< TIndex >( index );
            if( parentNode.children[ 0 ] == index )
            {
                int child = rotateNode.children[ 1 ];
                if( child >= 0 ) m_nodes[ child ].parent = static_cast< TIndex >( parent );
                parentNode.children[ 0 ] = rotateNode.children[ 1 ];
                rotateNode.children[ 1 ] = static_cast< TIndex >( parent );
                if( rotateNode.balance < 0 ) parentNode.balance -= rotateNode.balance;
                parentNode.balance += 1;
                if( parentNode.balance > 0 ) rotateNode.balance += parentNode.balance;
                rotateNode.balance += 1;
            }
            else
            {
                int child = rotateNode.children[ 0 ];
                if( child >= 0 ) m_nodes[ child ].parent = static_cast< TIndex >( parent );
                parentNode.children[ 1 ] = rotateNode.children[ 0 ];
                rotateNode.children[ 0 ] = static_cast< TIndex >( parent );
                if( rotateNode.balance > 0 ) parentNode.balance -= rotateNode.balance;
                parentNode.balance -= 1;
                if( parentNode.balance < 0 ) rotateNode.balance += parentNode.balance;
                rotateNode.balance -= 1;
            }
        }
    }
}

int TClass::locate() const
{   //  any element equal to key (element == key == element)
    int found = m_rootIndex;
    while( found >= 0 )
    {
        int compare = cmp( -1, found );
        if( compare == 0 ) break;
        found = m_nodes[ found ].children[ ( compare >> ( ( sizeof( int ) << 3 ) - 1 ) ) + 1 ];
    }
    return( found );
}

int TClass::locate_first_eq() const
{   //  first element equal to key (element == key == element)
    int found = -1;
    int check = m_rootIndex;
    while( check >= 0 )
    {
        int compare = cmp( -1, check );
        if( compare >= 0 )
        {
            if( compare == 0 )
            {
                found = check;
            }
            check = m_nodes[ check ].children[ 0 ];
        }
        else
        {
            check = m_nodes[ check ].children[ 1 ];
        }
    }
    return( found );
}

int TClass::locate_first_gt() const
{   //  first element greater than key (element > key or key < element)
    int found = -1;
    int check = m_rootIndex;
    while( check >= 0 )
    {
        int compare = cmp( -1, check );
        if( compare < 0 )
        {
            found = check;
            check = m_nodes[ check ].children[ 0 ];
        }
        else
        {
            check = m_nodes[ check ].children[ 1 ];
        }
    }
    return( found );
}

int TClass::locate_first_ge() const
{   //  first element greater than or equal to key (element >= key or key <= element)
    int found = -1;
    int check = m_rootIndex;
    while( check >= 0 )
    {
        int compare = cmp( -1, check );
        if( compare <= 0 )
        {
            found = check;
            check = m_nodes[ check ].children[ 0 ];
        }
        else
        {
            check = m_nodes[ check ].children[ 1 ];
        }
    }
    return( found );
}

int TClass::locate_last_eq() const
{   //  last element equal to key (element == key == element)
    int found = -1;
    int check = m_rootIndex;
    while( check >= 0 )
    {
        int compare = cmp( -1, check );
        if( compare <= 0 )
        {
            if( compare == 0 )
            {
                found = check;
            }
            check = m_nodes[ check ].children[ 1 ];
        }
        else
        {
            check = m_nodes[ check ].children[ 0 ];
        }
    }
    return( found );
}

int TClass::locate_last_lt() const
{   //  last element less than key (element < key or key > element)
    int found = -1;
    int check = m_rootIndex;
    while( check >= 0 )
    {
        int compare = cmp( -1, check );
        if( compare > 0 )
        {
            found = check;
            check = m_nodes[ check ].children[ 1 ];
        }
        else
        {
            check = m_nodes[ check ].children[ 0 ];
        }
    }
    return( found );
}

int TClass::locate_last_le() const
{   //  last element less than or equal to key (element <= key or key >= element)
    int found = -1;
    int check = m_rootIndex;
    while( check >= 0 )
    {
        int compare = cmp( -1, check );
        if( compare >= 0 )
        {
            found = check;
            check = m_nodes[ check ].children[ 1 ];
        }
        else
        {
            check = m_nodes[ check ].children[ 0 ];
        }
    }
    return( found );
}

int TClass::min_occupied() const
{
    uint index = 0;
    while( index < m_nodeCount )
    {
        Node& node = m_nodes[ index ];
        if( node.flag == k_used )
        {
            return( static_cast< int >( index ) );
        }
        ++index;
    }
    return( -1 );
}

int TClass::max_occupied() const
{
    uint index = m_nodeCount;
    while( index )
    {
        --index;
        Node& node = m_nodes[ index ];
        if( node.flag == k_used )
        {
            return( static_cast< int >( index ) );
        }
    }
    return( -1 );
}

uint TClass::height( const int index ) const
{
    if( index >= 0 )
    {
        Node& node = m_nodes[ index ];
        int height0 = height( node.children[ 0 ] );
        int height1 = height( node.children[ 1 ] );
        return( ( ( height0 > height1 ) ? height0 : height1 ) + 1 );
    }
    return( 0 );
}

uint TClass::weight( const int index ) const
{
    if( index >= 0 )
    {
        Node& node = m_nodes[ index ];
        return( weight( node.children[ 0 ] ) + weight( node.children[ 1 ] ) + 1 );
    }
    return( 0 );
}

bool TClass::validate( const int index ) const
{
    if( static_cast< uint >( index ) >= m_nodeCount )
    {
        return( false );
    }
    Node& node = m_nodes[ index ];
    if( node.flag != k_used )
    {
        return( false );
    }
    if( ( node.balance < -1 ) || ( node.balance > 1 ) )
    {
        return( false );
    }
    if( static_cast< TOther >( height( node.children[ 1 ] ) - height( node.children[ 0 ] ) ) != node.balance )
    {
        return( false );
    }
    if( node.children[ 0 ] >= 0 )
    {
        if( !validate( node.children[ 0 ] ) )
        {
            return( false );
        }
        if( m_nodes[ node.children[ 0 ] ].parent != index )
        {
            return( false );
        }
    }
    if( node.children[ 1 ] >= 0 )
    {
        if( !validate( node.children[ 1 ] ) )
        {
            return( false );
        }
        if( m_nodes[ node.children[ 1 ] ].parent != index )
        {
            return( false );
        }
    }
    int prevIndex = prev( index );
    if( prevIndex >= 0 )
    {
        if( next( prevIndex ) != index )
        {
            return( false );
        }
        if( cmp( index, prevIndex ) <= 0 )
        {
            return( false );
        }
    }
    int nextIndex = next( index );
    if( nextIndex >= 0 )
    {
        if( prev( nextIndex ) != index )
        {
            return( false );
        }
        if( cmp( index, nextIndex ) >= 0 )
        {
            return( false );
        }
    }
    return( true );
}

void TClass::take_obj( TClass& take_me )
{
    m_rootIndex = take_me.m_rootIndex;
    m_freeIndex = take_me.m_freeIndex;
    m_nodeCount = take_me.m_nodeCount;
    m_usedCount = take_me.m_usedCount;
    m_peakCount = take_me.m_peakCount;
	m_nodes = take_me.m_nodes;
    take_me.zero();
}

void TClass::clone_obj( const TClass& clone_me )
{
    if( clone_me.valid() )
    {
        m_rootIndex = clone_me.m_rootIndex;
        m_freeIndex = clone_me.m_freeIndex;
        m_nodeCount = clone_me.m_nodeCount;
        m_usedCount = clone_me.m_usedCount;
        m_peakCount = clone_me.m_peakCount;
        m_nodes = new Node[ m_nodeCount ];
        size_t size = ( m_nodeCount * sizeof( Node ) );
        memcpy_s( m_nodes, size, clone_me.m_nodes, size );
    }
    else
    {
        zero();
    }
}

void TClass::zero()
{
    m_rootIndex = -1;
    m_freeIndex = -1;
    m_nodeCount = 0;
    m_usedCount = 0;
    m_peakCount = 0;
    m_nodes = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Remove sorting structure configuration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef TClass
#undef TIndex
#undef TOther

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


