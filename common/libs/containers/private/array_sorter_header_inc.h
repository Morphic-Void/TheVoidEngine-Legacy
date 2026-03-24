
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   array_sorter_header_inc.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	AVL array sorting class.
////
////        This file is expected to be included multiple times.
////
////        This file together with its companion (array_sorter_source_inc.h) declares variants of the CArraySorter class.
////
////        While the variants could be created through the use of templates, this include format reduces the compile and
////        link times for non-unity builds by reducing the overall size and complexity of the includes and generating fewer
////        redundant copies of the compiled code in the object files.
////
////        The functionality of these classes is very similar to that of std::map, std::multimap, std::set and std::multiset.
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
////        virtual functions:
////
////            cmp( source, target )
////
////                Compare the keys of 2 indexed items and return the relationship.
////                The integer relationship pseudo-operation is ( item[ source ].key - item[ target ].key ).
////                During calls to the insert() and find() functions, the source index will be -1 to indicate
////                that the source for the comparison is the key of the item being sought or inserted.
////                This function is also called during calls to relink() and update().
////
////            move( source, target )
////
////                Moves an item from one indexed location to another.
////                The source and target indices will never be the same value.
////                Either index but not both may be -1 indicating that the location is temporary storage.
////                This function is only called during calls to sort().
////
////            resize( count )
////
////                The array sorter is being resized to accomodate 'count' elements.
////                This function is may be called during calls to shrink_to_fit(), reserve() and insert() functions.
////                This function is not called during construction, destruction, assignment or initialisation.
////
////        public functions:
////
////            take( take_me )         :   move the contents of the specified object to this object
////            clone( clone_me )       :   clone the specified object
////            valid()                 :   true if initialised
////            init( count )           :   initialise and allocate space for an initial array with 'count' elements
////            kill()                  :   deallocate internal storage and set internal parameters to defaults
////            reset()                 :   replace existing indexing with an empty array
////            reserve( count )        :   ensure that at least 'count' elements are available (can be used as an alternative to init())
////            shrink_to_fit()         :   reallocates the array end after the last allocated element index
////            contains( index )       :   true if the index is within the array and is occupied
////            insert( index, unique ) :   insert a new item and return its index (or -1 on failure)
////            update( index )         :   resort an indexed item whose lexicographic key has changed
////            remove( index )         :   remove an indexed item from the array
////            relink()                :   optimise the sorting of the tree
////            sort()                  :   physically reorder the array into lexical order and remove any indexing gaps
////            rank()                  :   allocate and fill an array with the lexical indices of all items in the array
////            rank( ranks )           :   fill an array with the lexical indices of all items in the array
////            rank( index )           :   return the lexical index of the indexed item
////            find()                  :   find the index of any item in the array matching the key (or -1 if not found)
////            find_first_eq()         :   find the index of the first item in the array matching the key (or -1 if not found)
////            find_first_gt()         :   find the index of the first item in the array greater than the key (or -1 if not found)
////            find_first_ge()         :   find the index of the first item in the array greater than or matching the key (or -1 if not found)
////            find_last_eq()          :   find the index of the last item in the array matching the key (or -1 if not found)
////            find_last_lt()          :   find the index of the last item in the array less than the key (or -1 if not found)
////            find_last_le()          :   find the index of the last item in the array less than or matching the key (or -1 if not found)
////            lower_bound()           :   find the index of the first item in the array not less than the specified item (or -1 if not found)
////                                    :   this is equivalent to find_first_ge()
////            upper_bound()           :   find the index of the first item in the array greater than the specified item (or -1 if not found)
////                                    :   this is equivalent to find_first_gt()
////            prev( index )           :   the index of the previous item in lexical order
////            next( index )           :   the index of the next item in lexical order
////            root()                  :   the index of the tree root item
////            first()                 :   the index of the first item in lexical order
////            last()                  :   the index of the last item in lexical order
////            size()                  :   the current count of managed array elements
////            used()                  :   the current count of occupied array elements
////            peak()                  :   the peak count of occupied array elements (managed array element count)
////            height()                :   the maximum height of the binary tree
////            weight()                :   the number of nodes in the binary tree
////            validate()              :   true if the tree exists and has integrity
////            element_limit()         :   returns the maximum number of elements supported by the class
////
////            additional detail:
////
////                insert( index, unique ):
////
////                    Will initialise the array if it is not already initialised.
////
////                    Will fail if unique is true and the item being inserted already exists lexicographically.
////
////                    If the specified index is negative, the index will be allocated from the free list. If the
////                    free list is empty, the array will be expanded to create more free indices.
////
////                    If the specified index is beyond the current array extents, the array will be expanded
////                    to accomodate it.
////
////                    If the specified index refers to an occupied element, the element will be unlinked
////                    and added to the free list before it is re-inserted. If the re-insert fails, the
////                    element will be removed from the tree.
////
////                rank(), rank( ranks ) and rank( index ):
////
////                    Unoccupied elements and invalid indices will have a lexical index of -1.
////
////                    The ranks buffer must have space for size() entries.
////
////                weight():
////
////                    Should return the same value as the used() function if the tree is well formed.
////
////        notes about the protected functions can be found in array_sort_source_inc.cpp
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

#include "libs/system/base/types.h"

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
////    CArraySorter class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TClass
{
public:
	inline                  TClass() : m_rootIndex( -1 ), m_freeIndex( -1 ), m_nodeCount( 0 ), m_usedCount( 0 ), m_peakCount( 0 ), m_nodes( NULL ) {};
	inline                  TClass( const uint count ) : m_rootIndex( -1 ), m_freeIndex( -1 ), m_nodeCount( 0 ), m_usedCount( 0 ), m_peakCount( 0 ), m_nodes( NULL ) { init( count ); };
    inline                  TClass( TClass&& take_me ) { take_obj( take_me ); };
    inline                  TClass( const TClass& clone_me ) { clone_obj( clone_me ); };
	virtual                 ~TClass() { kill(); };
    inline TClass&          operator =( TClass&& take_me );
    inline TClass&          operator =( const TClass& clone_me );
    inline void             take( TClass& take_me );
    inline void             clone( const TClass& clone_me );
    inline bool             valid() const;
    bool                    init( const uint count = 32 );
    void                    kill();
    void                    reset();
    bool                    reserve( const uint count );
    bool                    shrink_to_fit();
    inline bool             contains( const int index ) const;
	int                     insert( const int index = -1, const bool unique = true );
	bool                    update( const int index );
	bool                    remove( const int index );
	bool                    relink();
	bool                    sort();
    int*                    rank() const;
    bool                    rank( int* const ranks ) const;
    int                     rank( const int index ) const;
	inline int              find() const;
	inline int              find_first_eq() const;
	inline int              find_first_gt() const;
	inline int              find_first_ge() const;
	inline int              find_last_eq() const;
	inline int              find_last_lt() const;
	inline int              find_last_le() const;
	inline int              lower_bound() const;
	inline int              upper_bound() const;
	int                     prev( const int index ) const;
	int                     next( const int index ) const;
	int                     root() const;
	int                     first() const;
	int                     last() const;
    inline uint             used() const;
    inline uint             size() const;
    inline uint             peak() const;
	inline uint             height() const;
	inline uint             weight() const;
	inline bool             validate() const;
    static inline uint      element_limit();
protected:
    virtual int             cmp( const int source, const int target ) const = 0;
    virtual void            move( const int source, const int target ) = 0;
    virtual void            resize( const uint count ) = 0;
protected:
    struct                  Node { TIndex parent; TIndex children[ 2 ]; TOther balance; TOther flag; };
    bool                    enlarge( const uint count, const bool aggressive );
    int                     acquire( const int index, const bool aggressive );
    void                    recycle( const int index );
    void                    ranking( int* const ranks ) const;
    int                     linear();
    int                     sorted( const int parent, const int lower, const int upper );
    inline int              sorted();
    void                    extend( const int lower, const int upper );
    inline void             extend();
    bool                    insert( const int index, const int key );
    bool                    unlink( const int index );
    void				    rotate( const int index );
    int                     locate() const;             //  any equal to
    int                     locate_first_eq() const;    //  first equal to
    int                     locate_first_gt() const;    //  first greater than
    int                     locate_first_ge() const;    //  first greater than or equal to
    int                     locate_last_eq() const;     //  last equal to
    int                     locate_last_lt() const;     //  last less than
    int                     locate_last_le() const;     //  last less than or equal to
    int                     min_occupied() const;       //  lowest index occupied
    int                     max_occupied() const;       //  highest index occupied
	uint                    height( const int index ) const;
	uint                    weight( const int index ) const;
	bool				    validate( const int index ) const;
    void                    take_obj( TClass& take_me );
    void                    clone_obj( const TClass& clone_me );
    void                    zero();
    int                     m_rootIndex;                //  index of the root node
    int                     m_freeIndex;                //  index of the first free node
    uint                    m_nodeCount;                //  count of nodes allocated
    uint                    m_usedCount;                //  count of nodes occupied
    uint                    m_peakCount;                //  peak count of nodes occupied
	Node*                   m_nodes;                    //  array of allocated nodes
    static const TOther     k_free = ( -1 / 3 );        //  0x55555555
    static const TOther     k_used = ( k_free << 1 );   //  0xAAAAAAAA
    static const uint       k_high = ( uint( 1 ) << ( ( sizeof( TIndex ) << 3 ) - 1 ) );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    inline function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TClass& TClass::operator =( TClass&& take_me ) { take( take_me ); return( *this ); };
TClass& TClass::operator =( const TClass& clone_me ) { clone( clone_me ); return( *this ); };
void TClass::take( TClass& take_me ) { kill(); take_obj( take_me ); };
void TClass::clone( const TClass& clone_me ) { kill(); clone_obj( clone_me ); };
bool TClass::valid() const { return( ( m_nodes != NULL ) ? true : false ); };
bool TClass::contains( const int index ) const { return( ( valid() && ( static_cast< uint >( index ) < m_nodeCount ) && ( m_nodes[ index ].flag == k_used ) ) ? true : false ); };
int TClass::find() const { return( valid() ? locate() : -1 ); };
int TClass::find_first_eq() const { return( valid() ? locate_first_eq() : -1 ); };
int TClass::find_first_gt() const { return( valid() ? locate_first_gt() : -1 ); };
int TClass::find_first_ge() const { return( valid() ? locate_first_ge() : -1 ); };
int TClass::find_last_eq() const { return( valid() ? locate_last_eq() : -1 ); };
int TClass::find_last_lt() const { return( valid() ? locate_last_lt() : -1 ); };
int TClass::find_last_le() const { return( valid() ? locate_last_le() : -1 ); };
int TClass::lower_bound() const { return( find_first_ge() ); };
int TClass::upper_bound() const { return( find_first_gt() ); };
uint TClass::size() const { return( m_nodeCount ); };
uint TClass::used() const { return( m_usedCount ); };
uint TClass::peak() const { return( m_peakCount ); };
uint TClass::height() const { return( valid() ? height( m_rootIndex ) : 0 ); };
uint TClass::weight() const { return( valid() ? weight( m_rootIndex ) : 0 ); };
bool TClass::validate() const { return( valid() ? validate( m_rootIndex ) : false ); };
uint TClass::element_limit() { return( k_high ); };
int TClass::sorted() { return( sorted( -1, 0, static_cast< int >( m_usedCount - 1 ) ) ); };
void TClass::extend() { extend( static_cast< int >( m_usedCount ), static_cast< int >( m_nodeCount - 1 ) ); };

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

