
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
////        While the variants could be created through the use of templates, this include format reduces the compile time
////        for non-unity builds by reducing the overall size and complexity of the includes.
////
////    Notes:
////
////        notes about the public and virtual functions can be found in array_sorter_header_inc.h
////
////        protected functions:
////
////            enlarge(count, aggressive)      :   allocate or reallocate the array (can only increase the size)
////            acquire(index, aggressive)      :   acquire an index
////            recycle(index)                  :   recycle an index to the pool of unused elements
////            ranking(ranks)                  :   internal helper for the rank() functions
////            linear()                        :   internal helper function for sort() and relink()
////            sorted(parent, lower, upper)    :   construct a perfectly balanced sub-tree
////            sorted()                        :   internal helper function for sort()
////            extend(lower, upper)            :   adds the specified inclusive index range of nodes to the free list
////            extend()                        :   internal helper function for reset(), sort() and acquire()
////            insert(index, key)              :   insert a node into the AVL tree
////            unlink(index)                   :   unlink a node from the AVL tree
////            rotate(index)                   :   perform a compound AVL rebalancing rotate
////            locate()                        :   find any element equal to key
////            locate_first_eq()               :   first element equal to key
////            locate_first_gt()               :   first element greater than key
////            locate_first_ge()               :   first element greater than or equal to key
////            locate_last_eq()                :   last element equal to key
////            locate_last_lt()                :   last element less than key
////            locate_last_le()                :   last element less than or equal to key
////            min_occupied()                  :   lowest index of an occupied element (or -1 if empty)
////            max_occupied()                  :   highest index of an occupied element (or -1 if empty)
////            height(index)                   :   recursively calculate the maximum height of the tree starting at index
////            weight(index)                   :   recursively calculate the weight of the tree starting at index
////            validate(index)                 :   recursively validate the tree starting at index
////            take_obj(take_me)               :   helper for the move constructor and operator
////            clone_obj(clone_me)             :   helper for the copy constructor and operator
////            zero()                          :   set the object to an empty state
////
////            additional detail:
////
////                acquire(index, aggressive):
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
////                ranking(ranks):
////
////                    Constructs the lexical index array for the rank() and rank( ranks ) functions.
////
////                linear():
////
////                    Converts the tree into a sorted looping bi-directional linked list.
////
////                    The function returns the index of the first item in the list.
////
////                sorted(parent, lower, upper):
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
////                    Calls sorted(parent, lower, upper) with standard params. A parent index of -1 indicates that
////                    the sub-tree is the entire tree. The lower index is the 0 and the upper index is one less
////                    than the used node count.
////
////                    This function returns the root index of the new sub-tree.
////
////                extend(lower, upper):
////
////                    Adds the indicated lower to upper inclusive index range to the free list.
////
////                    It is imperative that the nodes being added are not in the node tree or free list.
////
////                extend():
////
////                    Calls extend(lower, upper) with standard params. The lower index is the used node count and
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
#include <new>

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

bool TClass::init(const uint32_t count)
{
    kill();
    if (count && (count <= k_high))
    {
        Node* nodes = new(std::nothrow) Node[count];
        if (nodes != nullptr)
        {   //  allocation succeeded
            m_nodeCount = count;
            m_nodes = nodes;
            extend();
            return(true);
        }
    }
    return(false);
}

void TClass::kill()
{
    if (m_nodes != nullptr)
    {
        delete[] m_nodes;
    }
    zero();
}

void TClass::reset()
{
    if (valid())
    {
        m_rootIndex = -1;
        m_freeIndex = -1;
        m_usedCount = 0;
        m_peakCount = 0;
        extend();
    }
}

bool TClass::reserve(const uint32_t count)
{
    if (valid())
    {
        uint32_t check = (m_nodeCount - m_usedCount);
        if (check < count)
        {
            uint32_t limit = (k_high - m_usedCount);
            if (limit < count)
            {   //  not expandable to accomodate the requested reserve
                return(false);
            }
            uint32_t nodeCount = (m_nodeCount + (count - check));
            return(enlarge(nodeCount, false));
        }
        return(true);
    }
    return(init(count));
}

bool TClass::shrink_to_fit()
{
    if (valid())
    {
        int32_t lastIndex = max_occupied();
        if (lastIndex >= 0)
        {
            uint32_t nodeCount = (static_cast<uint32_t>(lastIndex) + 1);
            Node* nodes = new(std::nothrow) Node[nodeCount];
            if (nodes != nullptr)
            {   //  allocation succeeded
                memcpy_s(reinterpret_cast<void*>(nodes), (sizeof(Node) * nodeCount), reinterpret_cast<const void*>(m_nodes), (sizeof(Node) * nodeCount));
                delete[] m_nodes;
                m_nodes = nodes;
                m_nodeCount = nodeCount;
                m_freeIndex = -1;
                uint32_t freeCount = (m_nodeCount - m_usedCount);
                if (freeCount)
                {   //  free items exist
                    int32_t scan = 0;
                    while (m_nodes[scan].flag != k_free) ++scan;
                    Node& node = m_nodes[scan];
                    node.child[0] = node.child[1] = -1;
                    node.parent = -1;
                    node.balance = 0;
                    int32_t head = scan;
                    while (--freeCount)
                    {
                        int32_t tail = scan;
                        ++scan;
                        while (m_nodes[scan].flag != k_free) ++scan;
                        Node& node = m_nodes[scan];
                        m_nodes[head].child[0] = m_nodes[tail].child[1] = static_cast<TIndex>(scan);
                        node.child[0] = static_cast<TIndex>(tail);
                        node.child[1] = static_cast<TIndex>(head);
                        node.parent = -1;
                        node.balance = 0;
                    }
                    m_freeIndex = head;
                    resize(m_nodeCount);
                }
                return(true);
            }
        }
        else
        {   //  no occupied elements so delete the array
            kill();
            resize(0);
            return(true);
        }
    }
    return(false);
}

int32_t TClass::insert(const int32_t index, const bool unique)
{
    if (!valid())
    {   //  ensure that we are in a safe state if no initialisation has been detected
        zero();
    }
    if ((static_cast<uint32_t>(index) < m_nodeCount) && (m_nodes[index].flag == k_used))
    {   //  the index references a currently occupied element
        if (!unlink(index))
        {   //  the element could not be unlinked
            return(-1);
        }
        recycle(index);
    }
    if (!unique || (locate() < 0))
    {   //  the item being inserted is unique or not required to be unique
        int32_t nodeIndex = acquire(index, true);
        if (nodeIndex >= 0)
        {   //  index acquisition succeeded
            if (insert(nodeIndex, -1))
            {   //  the node was successfully inserted
                return(nodeIndex);
            }
            recycle(nodeIndex);
        }
    }
    return(-1);
}

bool TClass::update(const int32_t index)
{   //  resort a single key : required if the indexed data key has been modified
    if (contains(index) && unlink(index))
    {
        return(insert(index, index));
    }
    return(false);
}

bool TClass::remove(const int32_t index)
{
    if (contains(index) && unlink(index))
    {
        recycle(index);
        return(true);
    }
    return(false);
}

//  relink()
//
//      Optimises search performance, though gains are liable to be marginal.
//
//      This function should also be called if the indexed data array is modified.
//
bool TClass::relink()
{   //  resort the array : required if the indexed data array is modified
    if (valid())
    {
        uint32_t count = m_usedCount;
        if (count > 1)
        {
            int32_t list = linear();
            m_rootIndex = -1;
            while (count)
            {
                int32_t index = list;
                Node& node = m_nodes[index];
                list = node.child[1];
                m_nodes[node.child[0]].child[1] = node.child[1];
                m_nodes[node.child[1]].child[0] = node.child[0];
                node.parent = -1;
                node.child[0] = node.child[1] = -1;
                node.balance = 0;
                if (!insert(index, index))
                {
                    return(false);
                }
                --count;
            }
        }
        return(true);
    }
    return(false);
}

bool TClass::sort()
{   //  sort the array contents removing any gaps
    if (valid())
    {
        uint32_t count = m_usedCount;
        if (count)
        {
            int32_t list = linear();
            for (uint32_t check = count; check; --check)
            {   //  move any items whose target location is unoccupied
                Node& node = m_nodes[list];
                int32_t source = list;
                int32_t target = node.parent;
                if ((source == target) || (m_nodes[target].flag == k_free))
                {   //  target is free
                    move(source, target);
                    node.flag = k_free;
                    m_nodes[node.child[0]].child[1] = node.child[1];
                    m_nodes[node.child[1]].child[0] = node.child[0];
                    if (static_cast<uint32_t>(source) < m_usedCount)
                    {
                        check = count;
                    }
                    --count;
                }
                list = node.child[1];
            }
            while (count)
            {   //  all remaining items must be in cyclic lists, so rotate them
                int32_t cyclic = list;
                int32_t source = -1;
                int32_t target = cyclic;
                do
                {   //  unlink the cycle and set the source index into child[1]
                    Node& node = m_nodes[target];
                    list = node.child[1];
                    m_nodes[node.child[0]].child[1] = node.child[1];
                    m_nodes[node.child[1]].child[0] = node.child[0];
                    node.child[1] = static_cast<TIndex>(source);
                    source = target;
                    target = node.parent;
                    --count;
                } while (target != cyclic);
                move(source, -1);
                while ((target = source) >= 0)
                {
                    source = m_nodes[target].child[1];
                    move(source, target);
                }
            }
            m_rootIndex = sorted();
            m_freeIndex = -1;
            extend();
        }
        return(true);
    }
    return(false);
}

int32_t* TClass::rank() const
{   //  return an array of the rank of all items (i.e. the indicies that the items would have if the array was sorted)
    int32_t* ranks = nullptr;
    if (valid())
    {
        ranks = new(std::nothrow) int32_t[m_nodeCount];
        ranking(ranks);
    }
    return(ranks);
}

bool TClass::rank(int32_t* const ranks) const
{
    if (valid() && (ranks != nullptr))
    {
        ranking(ranks);
        return(true);
    }
    return(false);
}

int32_t TClass::rank(const int32_t index) const
{   //  return the rank of the index (i.e. the index that this item would have if the array was sorted)
    int32_t count = -1;
    if (contains(index))
    {
        int32_t found = index;
        while (found >= 0)
        {   //  count the number of times we can step to a previous node
            ++count;
            int32_t check = m_nodes[found].child[0];
            if (check < 0)
            {
                for (check = found; (((found = m_nodes[check].parent) >= 0) && (m_nodes[found].child[1] != check)); check = found);
            }
            else
            {
                for (found = check; ((check = m_nodes[found].child[1]) >= 0); found = check);
            }
        }
    }
    return(count);
}

int32_t TClass::prev(const int32_t index) const
{
    int32_t found = -1;
    if (contains(index))
    {
        int32_t check = m_nodes[index].child[0];
        if (check < 0)
        {
            for (check = index; (((found = m_nodes[check].parent) >= 0) && (m_nodes[found].child[1] != check)); check = found);
        }
        else
        {
            for (found = check; ((check = m_nodes[found].child[1]) >= 0); found = check);
        }
    }
    return(found);
}

int32_t TClass::next(const int32_t index) const
{
    int32_t found = -1;
    if (contains(index))
    {
        int32_t check = m_nodes[index].child[1];
        if (check < 0)
        {
            for (check = index; (((found = m_nodes[check].parent) >= 0) && (m_nodes[found].child[0] != check)); check = found);
        }
        else
        {
            for (found = check; ((check = m_nodes[found].child[0]) >= 0); found = check);
        }
    }
    return(found);
}

int32_t TClass::root() const
{
    return((m_nodes != nullptr) ? m_rootIndex : -1);
}

int32_t TClass::first() const
{
    int32_t found = -1;
    if (m_nodes != nullptr)
    {
        for (int32_t check = m_rootIndex; check >= 0; check = m_nodes[found].child[0]) found = check;
    }
    return(found);
}

int32_t TClass::last() const
{
    int32_t found = -1;
    if (m_nodes != nullptr)
    {
        for (int32_t check = m_rootIndex; check >= 0; check = m_nodes[found].child[1]) found = check;
    }
    return(found);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CArraySorter class protected function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TClass::enlarge(const uint32_t count, const bool aggressive)
{
    if (count <= k_high)
    {
        uint32_t nodeCount = count;
        if (aggressive)
        {
            nodeCount += ((nodeCount >> 1) + 1);
            if (nodeCount < 32)
            {
                nodeCount = 32;
            }
            else if (nodeCount > k_high)
            {
                nodeCount = k_high;
            }
        }
        if (nodeCount > m_nodeCount)
        {
            Node* nodes = new(std::nothrow) Node[nodeCount];
            if (nodes != nullptr)
            {   //  allocation succeeded
                if (m_nodeCount && (m_nodes != nullptr))
                {
                    memcpy_s(reinterpret_cast<void*>(nodes), (sizeof(Node) * nodeCount), reinterpret_cast<const void*>(m_nodes), (sizeof(Node) * m_nodeCount));
                    delete[] m_nodes;
                }
                m_nodes = nodes;
                extend(static_cast<int32_t>(m_nodeCount), static_cast<int32_t>(nodeCount - 1));
                m_nodeCount = nodeCount;
                resize(nodeCount);
                return(true);
            }
        }
    }
    return(false);
}

int32_t TClass::acquire(const int32_t index, const bool aggressive)
{
    int32_t nodeIndex = ((index < 0) ? ((m_freeIndex < 0) ? static_cast<int32_t>(m_nodeCount) : m_freeIndex) : index);
    uint32_t nodeCount = static_cast<uint32_t>(nodeIndex + 1);
    if ((nodeCount <= m_nodeCount) || enlarge(nodeCount, aggressive))
    {   //  the nodeIndex is within the array or the newly enlarged array
        Node& node = m_nodes[nodeIndex];
        if ((node.flag == k_free) || unlink(nodeIndex))
        {   //  the item was already in the free list or was successfully unlinked
            if (m_freeIndex == nodeIndex)
            {
                m_freeIndex = ((node.child[1] == nodeIndex) ? -1 : node.child[1]);
            }
            m_nodes[node.child[0]].child[1] = node.child[1];
            m_nodes[node.child[1]].child[0] = node.child[0];
            node.parent = node.child[0] = node.child[1] = -1;
            node.balance = 0;
            node.flag = k_used;
            ++m_usedCount;
            if (m_peakCount < m_usedCount)
            {
                m_peakCount = m_usedCount;
            }
            return(nodeIndex);
        }
    }
    return(-1);
}

void TClass::recycle(const int32_t index)
{   //  adds the indicated node to the start of the free list
    Node& node = m_nodes[index];
    if (m_freeIndex >= 0)
    {
        node.child[0] = m_nodes[m_freeIndex].child[0];
        node.child[1] = static_cast<TIndex>(m_freeIndex);
        m_nodes[node.child[0]].child[1] = static_cast<TIndex>(index);
        m_nodes[node.child[1]].child[0] = static_cast<TIndex>(index);
    }
    else
    {
        node.child[0] = node.child[1] = static_cast<TIndex>(index);
    }
    m_freeIndex = index;
    node.parent = -1;
    node.balance = 0;
    node.flag = k_free;
    --m_usedCount;
}

void TClass::ranking(int32_t* const ranks) const
{
    memset(ranks, -1, (m_nodeCount * sizeof(int32_t)));
    int32_t index = m_rootIndex;
    for (int32_t check = index; check >= 0; check = m_nodes[index].child[0]) index = check;
    for (int32_t rank = 0; index >= 0; ++rank)
    {   //  step through the nodes from first to last and assign their ranks
        ranks[index] = rank;
        int32_t check = m_nodes[index].child[1];
        if (check < 0)
        {
            for (check = index; (((index = m_nodes[check].parent) >= 0) && (m_nodes[index].child[0] != check)); check = index);
        }
        else
        {
            for (index = check; ((check = m_nodes[index].child[0]) >= 0); index = check);
        }
    }
}

int32_t TClass::linear()
{   //  convert the tree to a sorted bi-directional circular list (for use by the sort() and relink() functions)
    int32_t prev, next;
    int32_t root = -1;
    int32_t last = -1;
    for (next = m_rootIndex; next >= 0; next = m_nodes[last].child[1]) last = next;
    if (last >= 0)
    {
        int32_t item = last;
        while (item >= 0)
        {   //  create a sorted list with the links in child[1]
            Node& node = m_nodes[item];
            node.child[1] = static_cast<TIndex>(root);
            root = item;
            if ((item = node.child[0]) < 0)
            {
                prev = root;
                while (((item = m_nodes[prev].parent) >= 0) && (m_nodes[item].child[1] != prev))
                {
                    prev = item;
                }
            }
            else
            {
                while ((prev = m_nodes[item].child[1]) >= 0)
                {
                    item = prev;
                }
            }
        }
        prev = last;
        int32_t index = 0;
        for (item = root; item >= 0; item = next)
        {   //  make the list bi-directional and set the parent index to be the linear ordered index of the node
            Node& node = m_nodes[item];
            node.parent = static_cast<TIndex>(index);
            node.child[0] = static_cast<TIndex>(prev);
            next = node.child[1];
            prev = item;
            ++index;
        }
        m_nodes[last].child[1] = static_cast<TIndex>(root);
    }
    return(root);
}

int32_t TClass::sorted(const int32_t parent, const int32_t lower, const int32_t upper)
{   //  create a contiguous balanced tree for sorted data (note: lower and upper represent an inclusive range)
    int32_t index = -1;
    if (lower <= upper)
    {
        index = static_cast<int32_t>(static_cast<uint32_t>(lower + upper) >> 1);
        Node& node = m_nodes[index];
        node.parent = static_cast<TIndex>(parent);
        node.child[0] = static_cast<TIndex>(sorted(index, lower, (index - 1)));
        node.child[1] = static_cast<TIndex>(sorted(index, (index + 1), upper));
        node.balance = static_cast<TOther>(upper + lower - index - index);
        node.flag = k_used;
    }
    return(index);
}

void TClass::extend(const int32_t lower, const int32_t upper)
{   //  add the inclusive lower to upper index range to the bi-directional looped linked list of free nodes
    if (lower >= 0)
    {
        for (int32_t index = lower; index <= upper; ++index)
        {
            Node& node = m_nodes[index];
            node.parent = -1;
            node.child[0] = static_cast<TIndex>(index - 1);
            node.child[1] = static_cast<TIndex>(index + 1);
            node.balance = 0;
            node.flag = k_free;
        }
        int32_t freeIndex = m_freeIndex;
        if (freeIndex < 0)
        {
            m_freeIndex = lower;
            m_nodes[upper].child[1] = static_cast<TIndex>(lower);
            m_nodes[lower].child[0] = static_cast<TIndex>(upper);
        }
        else
        {
            int32_t tailIndex = m_nodes[freeIndex].child[0];
            m_nodes[upper].child[1] = static_cast<TIndex>(freeIndex);
            m_nodes[lower].child[0] = static_cast<TIndex>(tailIndex);
            m_nodes[tailIndex].child[1] = static_cast<TIndex>(lower);
            m_nodes[freeIndex].child[0] = static_cast<TIndex>(upper);
        }
    }
}

bool TClass::insert(const int32_t index, const int32_t key)
{
    if (index >= 0)
    {
        Node& insertNode = m_nodes[index];
        int32_t parent = m_rootIndex;
        int32_t child = index;
        if (parent >= 0)
        {   //  this is not the first node to be inserted so will need sorting
            for (;;)
            {
                int32_t compare = cmp(key, parent);
                Node& parentNode = m_nodes[parent];
                int32_t flip = ((compare >> ((sizeof(int32_t) << 3) - 1)) + 1);
                if (parentNode.child[flip] < 0)
                {
                    parentNode.child[flip] = static_cast<TIndex>(index);
                    insertNode.parent = static_cast<TIndex>(parent);
                    break;
                }
                parent = parentNode.child[flip];
            }
            while (parent >= 0)
            {
                Node& parentNode = m_nodes[parent];
                TOther balance = ((parentNode.child[0] == child) ? -1 : 1);
                if ((parentNode.balance - balance) == 0)
                {
                    Node& childNode = m_nodes[child];
                    if ((childNode.balance + balance) == 0)
                    {
                        child = childNode.child[-(balance >> 1)];
                        rotate(child);
                    }
                    parentNode.balance += balance;
                    rotate(child);
                }
                else
                {
                    parentNode.balance += balance;
                    child = parent;
                }
                if (m_nodes[child].balance == 0)
                {
                    for (int32_t check = -1; (check = m_nodes[child].parent) >= 0; child = check);
                }
                parent = m_nodes[child].parent;
            }
        }
        m_rootIndex = child;
        return(true);
    }
    return(false);
}

bool TClass::unlink(const int32_t index)
{
    if (index >= 0)
    {
        Node& unlinkNode = m_nodes[index];
        int32_t child = index;
        int32_t flip = ((unlinkNode.balance < 0) ? 0 : 1);
        if (unlinkNode.child[flip] >= 0)
        {
            child = unlinkNode.child[flip];
            flip ^= 1;
            for (int32_t check = -1; ((check = m_nodes[child].child[flip]) >= 0); child = check);
        }
        flip ^= 1;
        rotate(m_nodes[child].child[flip]);
        int32_t parent = m_nodes[child].parent;
        if (parent >= 0)
        {   //  not the last node in the tree
            if (child != index)
            {   //  exchange the linkage of the child node and the node being removed
                if (parent == index) parent = child;
                if (unlinkNode.parent >= 0)
                {
                    Node& parentNode = m_nodes[unlinkNode.parent];
                    parentNode.child[(parentNode.child[0] == index) ? 0 : 1] = static_cast<TIndex>(child);
                }
                if (unlinkNode.child[0] >= 0) m_nodes[unlinkNode.child[0]].parent = static_cast<TIndex>(child);
                if (unlinkNode.child[1] >= 0) m_nodes[unlinkNode.child[1]].parent = static_cast<TIndex>(child);
                Node& childNode = m_nodes[child];
                childNode.parent = unlinkNode.parent;
                childNode.child[0] = unlinkNode.child[0];
                childNode.child[1] = unlinkNode.child[1];
                childNode.balance = unlinkNode.balance;
                unlinkNode.parent = static_cast<TIndex>(parent);
                unlinkNode.child[0] = unlinkNode.child[1] = -1;
                unlinkNode.balance = 0;
                Node& parentNode = m_nodes[parent];
                parentNode.child[(parentNode.child[0] == child) ? 0 : 1] = static_cast<TIndex>(index);
                child = index;
            }
            while (parent >= 0)
            {
                Node& parentNode = m_nodes[parent];
                TOther balance = ((parentNode.child[0] == child) ? 1 : -1);
                if ((parentNode.balance - balance) == 0)
                {
                    child = parentNode.child[(balance + 1) >> 1];
                    Node& childNode = m_nodes[child];
                    if ((childNode.balance + balance) == 0)
                    {
                        child = childNode.child[-(balance >> 1)];
                        rotate(child);
                    }
                    parentNode.balance += balance;
                    rotate(child);
                }
                else
                {
                    parentNode.balance += balance;
                    child = parent;
                }
                if (m_nodes[child].balance != 0)
                {
                    for (int32_t check = -1; (check = m_nodes[child].parent) >= 0; child = check);
                }
                parent = m_nodes[child].parent;
            }
            Node& parentNode = m_nodes[unlinkNode.parent];
            parentNode.child[(parentNode.child[0] == index) ? 0 : 1] = -1;
            unlinkNode.parent = -1;
        }
        else
        {   //  this was the last node in the tree
            child = -1;
        }
        m_rootIndex = child;
        return(true);
    }
    return(false);
}

void TClass::rotate(const int32_t index)
{
    if (index >= 0)
    {
        Node& rotateNode = m_nodes[index];
        int32_t parent = rotateNode.parent;
        if (parent >= 0)
        {
            Node& parentNode = m_nodes[parent];
            int32_t gramps = parentNode.parent;
            if (gramps >= 0)
            {
                Node& grampsNode = m_nodes[gramps];
                grampsNode.child[(grampsNode.child[0] == parent) ? 0 : 1] = static_cast<TIndex>(index);
            }
            rotateNode.parent = static_cast<TIndex>(gramps);
            parentNode.parent = static_cast<TIndex>(index);
            if (parentNode.child[0] == index)
            {
                int32_t child = rotateNode.child[1];
                if (child >= 0) m_nodes[child].parent = static_cast<TIndex>(parent);
                parentNode.child[0] = rotateNode.child[1];
                rotateNode.child[1] = static_cast<TIndex>(parent);
                if (rotateNode.balance < 0) parentNode.balance -= rotateNode.balance;
                parentNode.balance += 1;
                if (parentNode.balance > 0) rotateNode.balance += parentNode.balance;
                rotateNode.balance += 1;
            }
            else
            {
                int32_t child = rotateNode.child[0];
                if (child >= 0) m_nodes[child].parent = static_cast<TIndex>(parent);
                parentNode.child[1] = rotateNode.child[0];
                rotateNode.child[0] = static_cast<TIndex>(parent);
                if (rotateNode.balance > 0) parentNode.balance -= rotateNode.balance;
                parentNode.balance -= 1;
                if (parentNode.balance < 0) rotateNode.balance += parentNode.balance;
                rotateNode.balance -= 1;
            }
        }
    }
}

int32_t TClass::locate() const
{   //  any element equal to key (element == key == element)
    int32_t found = m_rootIndex;
    while (found >= 0)
    {
        int32_t compare = cmp(-1, found);
        if (compare == 0) break;
        found = m_nodes[found].child[(compare >> ((sizeof(int32_t) << 3) - 1)) + 1];
    }
    return(found);
}

int32_t TClass::locate_first_eq() const
{   //  first element equal to key (element == key == element)
    int32_t found = -1;
    int32_t check = m_rootIndex;
    while (check >= 0)
    {
        int32_t compare = cmp(-1, check);
        if (compare >= 0)
        {
            if (compare == 0)
            {
                found = check;
            }
            check = m_nodes[check].child[0];
        }
        else
        {
            check = m_nodes[check].child[1];
        }
    }
    return(found);
}

int32_t TClass::locate_first_gt() const
{   //  first element greater than key (element > key or key < element)
    int32_t found = -1;
    int32_t check = m_rootIndex;
    while (check >= 0)
    {
        int32_t compare = cmp(-1, check);
        if (compare < 0)
        {
            found = check;
            check = m_nodes[check].child[0];
        }
        else
        {
            check = m_nodes[check].child[1];
        }
    }
    return(found);
}

int32_t TClass::locate_first_ge() const
{   //  first element greater than or equal to key (element >= key or key <= element)
    int32_t found = -1;
    int32_t check = m_rootIndex;
    while (check >= 0)
    {
        int32_t compare = cmp(-1, check);
        if (compare <= 0)
        {
            found = check;
            check = m_nodes[check].child[0];
        }
        else
        {
            check = m_nodes[check].child[1];
        }
    }
    return(found);
}

int32_t TClass::locate_last_eq() const
{   //  last element equal to key (element == key == element)
    int32_t found = -1;
    int32_t check = m_rootIndex;
    while (check >= 0)
    {
        int32_t compare = cmp(-1, check);
        if (compare <= 0)
        {
            if (compare == 0)
            {
                found = check;
            }
            check = m_nodes[check].child[1];
        }
        else
        {
            check = m_nodes[check].child[0];
        }
    }
    return(found);
}

int32_t TClass::locate_last_lt() const
{   //  last element less than key (element < key or key > element)
    int32_t found = -1;
    int32_t check = m_rootIndex;
    while (check >= 0)
    {
        int32_t compare = cmp(-1, check);
        if (compare > 0)
        {
            found = check;
            check = m_nodes[check].child[1];
        }
        else
        {
            check = m_nodes[check].child[0];
        }
    }
    return(found);
}

int32_t TClass::locate_last_le() const
{   //  last element less than or equal to key (element <= key or key >= element)
    int32_t found = -1;
    int32_t check = m_rootIndex;
    while (check >= 0)
    {
        int32_t compare = cmp(-1, check);
        if (compare >= 0)
        {
            found = check;
            check = m_nodes[check].child[1];
        }
        else
        {
            check = m_nodes[check].child[0];
        }
    }
    return(found);
}

int32_t TClass::min_occupied() const
{
    uint32_t index = 0;
    while (index < m_nodeCount)
    {
        Node& node = m_nodes[index];
        if (node.flag == k_used)
        {
            return(static_cast<int32_t>(index));
        }
        ++index;
    }
    return(-1);
}

int32_t TClass::max_occupied() const
{
    uint32_t index = m_nodeCount;
    while (index)
    {
        --index;
        Node& node = m_nodes[index];
        if (node.flag == k_used)
        {
            return(static_cast<int32_t>(index));
        }
    }
    return(-1);
}

uint32_t TClass::height(const int32_t index) const
{
    if (index >= 0)
    {
        Node& node = m_nodes[index];
        int32_t height0 = height(node.child[0]);
        int32_t height1 = height(node.child[1]);
        return(((height0 > height1) ? height0 : height1) + 1);
    }
    return(0);
}

uint32_t TClass::weight(const int32_t index) const
{
    if (index >= 0)
    {
        Node& node = m_nodes[index];
        return(weight(node.child[0]) + weight(node.child[1]) + 1);
    }
    return(0);
}

bool TClass::validate(const int32_t index) const
{
    if (static_cast<uint32_t>(index) >= m_nodeCount)
    {
        return(false);
    }
    Node& node = m_nodes[index];
    if (node.flag != k_used)
    {
        return(false);
    }
    if ((node.balance < -1) || (node.balance > 1))
    {
        return(false);
    }
    if (static_cast<TOther>(height(node.child[1]) - height(node.child[0])) != node.balance)
    {
        return(false);
    }
    if (node.child[0] >= 0)
    {
        if (!validate(node.child[0]))
        {
            return(false);
        }
        if (m_nodes[node.child[0]].parent != index)
        {
            return(false);
        }
    }
    if (node.child[1] >= 0)
    {
        if (!validate(node.child[1]))
        {
            return(false);
        }
        if (m_nodes[node.child[1]].parent != index)
        {
            return(false);
        }
    }
    int32_t prevIndex = prev(index);
    if (prevIndex >= 0)
    {
        if (next(prevIndex) != index)
        {
            return(false);
        }
        if (cmp(index, prevIndex) <= 0)
        {
            return(false);
        }
    }
    int32_t nextIndex = next(index);
    if (nextIndex >= 0)
    {
        if (prev(nextIndex) != index)
        {
            return(false);
        }
        if (cmp(index, nextIndex) >= 0)
        {
            return(false);
        }
    }
    return(true);
}

void TClass::take_obj(TClass& take_me)
{
    m_rootIndex = take_me.m_rootIndex;
    m_freeIndex = take_me.m_freeIndex;
    m_nodeCount = take_me.m_nodeCount;
    m_usedCount = take_me.m_usedCount;
    m_peakCount = take_me.m_peakCount;
    m_nodes = take_me.m_nodes;
    take_me.zero();
}

void TClass::clone_obj(const TClass& clone_me)
{
    if (clone_me.valid())
    {
        m_rootIndex = clone_me.m_rootIndex;
        m_freeIndex = clone_me.m_freeIndex;
        m_nodeCount = clone_me.m_nodeCount;
        m_usedCount = clone_me.m_usedCount;
        m_peakCount = clone_me.m_peakCount;
        m_nodes = new(std::nothrow) Node[m_nodeCount];
        size_t size = (m_nodeCount * sizeof(Node));
        memcpy_s(m_nodes, size, clone_me.m_nodes, size);
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
    m_nodes = nullptr;
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
