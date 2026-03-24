
////    File:   array_sort.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	AVL array sorting class.

#include "array_sort.h"
#include <memory.h>

//! ArraySort class public function bodies

bool ArraySort::init(IArrayQuery& query, const uint count)
{
    kill();
    if (static_cast<int>(count) >= 0)
    {
        int nodeCount = ((count < 4) ? 4 : static_cast<int>(count));
        Node* nodes = new Node[nodeCount];
        if (nodes != nullptr)
        {   //  allocation succeeded
            m_nodeCount = nodeCount;
            m_nodes = nodes;
            m_query = &query;
            reset();
            return(true);
        }
    }
    return(false);
}

void ArraySort::kill()
{
    m_rootIndex = -1;
    m_freeIndex = -1;
    m_nodeCount = 0;
    m_usedCount = 0;
    m_peakCount = 0;
    if (m_nodes != nullptr)
    {
        delete[] m_nodes;
        m_nodes = nullptr;
    }
    m_query = nullptr;
}

void ArraySort::reset()
{
    if (valid())
    {
        m_rootIndex = -1;
        m_freeIndex = 0;
        m_usedCount = 0;
        m_peakCount = 0;
        extend();
    }
}

void ArraySort::clone(const ArraySort& source)
{
    kill();
    m_rootIndex = source.m_rootIndex;
    m_freeIndex = source.m_freeIndex;
    m_nodeCount = source.m_nodeCount;
    m_usedCount = source.m_usedCount;
    m_peakCount = source.m_peakCount;
    m_nodes = new Node[m_nodeCount];
    memcpy(m_nodes, source.m_nodes, (m_nodeCount * sizeof(Node)));
    m_query = source.m_query;
}

int ArraySort::insert(const void* key)
{
    if (valid() && (key != nullptr))
    {
        int index = find(key);
        if (index < 0)
        {   //  no matching key found so we should be safe to insert this one
            index = allocate();
            if (index >= 0)
            {   //  a new node was successfully allocated
                if (insert(index, key))
                {   //  the node was successfully inserted
                    return(index);
                }
            }
        }
    }
    return(-1);
}

bool ArraySort::update(const int index)
{   //  resort a single key : required if the indexed data key has been modified
    if (contains(index))
    {
        const void* key = m_query->getKey(index);
        if (key != nullptr)
        {
            if (unlink(index))
            {
                return(insert(index, key));
            }
        }
    }
    return(false);
}

bool ArraySort::remove(const int index)
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
bool ArraySort::relink()
{   //  resort the array : required if the indexed data array is modified
    if (valid())
    {
        int count = m_usedCount;
        if (count > 1)
        {
            int list = linear();
            m_rootIndex = -1;
            while (count)
            {
                int index = list;
                Node& node = m_nodes[index];
                list = node.children[1];
                m_nodes[node.children[0]].children[1] = node.children[1];
                m_nodes[node.children[1]].children[0] = node.children[0];
                node.parent = -1;
                node.children[0] = node.children[1] = -1;
                node.balance = 0;
                if (!insert(index, m_query->getKey(index)))
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

bool ArraySort::sort()
{   //  sort the array contents removing any gaps
    if (valid())
    {
        int count = m_usedCount;
        if (count)
        {
            int list = linear();
            for (int scan = count; scan; --scan)
            {   //  move any items whose target location is unoccupied
                Node& node = m_nodes[list];
                int source = list;
                int target = node.parent;
                if ((source == target) || (m_nodes[target].flag == k_free))
                {   //  target is free
                    m_query->moveItem(source, target);
                    node.flag = k_free;
                    m_nodes[node.children[0]].children[1] = node.children[1];
                    m_nodes[node.children[1]].children[0] = node.children[0];
                    if (source < m_usedCount)
                    {
                        scan = count;
                    }
                    --count;
                }
                list = node.children[1];
            }
            while (count)
            {   //  all remaining items must be in cyclic lists, so rotate them
                int cyclic = list;
                int source = -1;
                int target = cyclic;
                do
                {   //  unlink the cycle and set the source index into children[ 1 ]
                    Node& node = m_nodes[target];
                    list = node.children[1];
                    m_nodes[node.children[0]].children[1] = node.children[1];
                    m_nodes[node.children[1]].children[0] = node.children[0];
                    node.children[1] = source;
                    source = target;
                    target = node.parent;
                    --count;
                } while (target != cyclic);
                m_query->moveItem(source, -1);
                while ((target = source) >= 0)
                {
                    source = m_nodes[target].children[1];
                    m_query->moveItem(source, target);
                }
            }
            m_rootIndex = sorted();
            extend();
        }
        return(true);
    }
    return(false);
}

int* ArraySort::rank() const
{   //  return an array of the rank of all items (i.e. the indicies that the items would have if the array was sorted)
    int* ranks = nullptr;
    if (valid())
    {
        ranks = new int[m_nodeCount];
        memset(ranks, -1, (m_nodeCount * sizeof(int)));
        int index = m_rootIndex;
        for (int check = index; check >= 0; check = m_nodes[index].children[0]) index = check;
        for (int rank = 0; index >= 0; ++rank)
        {   //  step through the nodes from first to last and assign their ranks
            ranks[index] = rank;
            int check = m_nodes[index].children[1];
            if (check < 0)
            {
                for (check = index; (((index = m_nodes[check].parent) >= 0) && (m_nodes[index].children[0] != check)); check = index);
            }
            else
            {
                for (index = check; ((check = m_nodes[index].children[0]) >= 0); index = check);
            }
        }
    }
    return(ranks);
}

int ArraySort::rank(const int index) const
{   //  return the rank of the index (i.e. the index that this item would have if the array was sorted)
    int count = -1;
    if (contains(index))
    {
        int found = index;
        while (found >= 0)
        {   //  count the number of times we can step to a previous node
            ++count;
            int check = m_nodes[found].children[0];
            if (check < 0)
            {
                for (check = found; (((found = m_nodes[check].parent) >= 0) && (m_nodes[found].children[1] != check)); check = found);
            }
            else
            {
                for (found = check; ((check = m_nodes[found].children[1]) >= 0); found = check);
            }
        }
    }
    return(count);
}

int ArraySort::find(const void* const key) const
{
    int found = -1;
    if (valid() && (key != nullptr))
    {
        found = m_rootIndex;
        while (found >= 0)
        {
            int compare = m_query->cmpKey(key, found);
            if (compare == 0) break;
            found = m_nodes[found].children[(compare >> ((sizeof(int) << 3) - 1)) + 1];
        }
    }
    return(found);
}

int ArraySort::prev(const int index) const
{
    int found = -1;
    if (contains(index))
    {
        int check = m_nodes[index].children[0];
        if (check < 0)
        {
            for (check = index; (((found = m_nodes[check].parent) >= 0) && (m_nodes[found].children[1] != check)); check = found);
        }
        else
        {
            for (found = check; ((check = m_nodes[found].children[1]) >= 0); found = check);
        }
    }
    return(found);
}

int ArraySort::next(const int index) const
{
    int found = -1;
    if (contains(index))
    {
        int check = m_nodes[index].children[1];
        if (check < 0)
        {
            for (check = index; (((found = m_nodes[check].parent) >= 0) && (m_nodes[found].children[0] != check)); check = found);
        }
        else
        {
            for (found = check; ((check = m_nodes[found].children[0]) >= 0); found = check);
        }
    }
    return(found);
}

int ArraySort::root() const
{
    return((m_nodes != nullptr) ? m_rootIndex : -1);
}

int ArraySort::first() const
{
    int found = -1;
    if (m_nodes != nullptr)
    {
        for (int check = m_rootIndex; check >= 0; check = m_nodes[found].children[0]) found = check;
    }
    return(found);
}

int ArraySort::last() const
{
    int found = -1;
    if (m_nodes != nullptr)
    {
        for (int check = m_rootIndex; check >= 0; check = m_nodes[found].children[1]) found = check;
    }
    return(found);
}

//! ArraySort class protected function bodies

int ArraySort::allocate()
{
    int index = m_freeIndex;
    if (index < 0)
    {   //  need to resize the node array
        index = m_usedCount;
        int nodeCount = (m_nodeCount + (m_nodeCount >> 1) + 1);
        Node* nodes = new Node[nodeCount];
        if (nodes == nullptr)
        {   //  allocation failed
            return(-1);
        }
        memcpy_s(reinterpret_cast<void*>(nodes), (sizeof(Node) * nodeCount), reinterpret_cast<const void*>(m_nodes), (sizeof(Node) * m_nodeCount));
        delete[] m_nodes;
        m_nodes = nodes;
        m_nodeCount = nodeCount;
        extend();
    }
    Node& node = m_nodes[index];
    m_freeIndex = ((node.children[1] == index) ? -1 : node.children[1]);
    m_nodes[node.children[0]].children[1] = node.children[1];
    m_nodes[node.children[1]].children[0] = node.children[0];
    node.parent = node.children[0] = node.children[1] = -1;
    node.balance = 0;
    node.flag = k_used;
    ++m_usedCount;
    if (m_peakCount < m_usedCount)
    {
        m_peakCount = m_usedCount;
    }
    return(index);
}

void ArraySort::recycle(const int index)
{   //  adds the indicated node to the start of the free list
    Node& node = m_nodes[index];
    if (m_freeIndex >= 0)
    {
        node.children[0] = m_nodes[m_freeIndex].children[0];
        node.children[1] = m_freeIndex;
        m_nodes[node.children[0]].children[1] = index;
        m_nodes[node.children[1]].children[0] = index;
    }
    else
    {
        node.children[0] = node.children[1] = index;
    }
    m_freeIndex = index;
    node.parent = -1;
    node.balance = 0;
    node.flag = k_free;
    --m_usedCount;
}

int ArraySort::linear()
{   //  convert the tree to a sorted bi-directional circular list (for use by the sort() and relink() functions)
    int prev, next;
    int root = -1;
    int last = -1;
    for (next = m_rootIndex; next >= 0; next = m_nodes[last].children[1]) last = next;
    if (last >= 0)
    {
        int item = last;
        while (item >= 0)
        {   //  create a sorted list with the links in children[ 1 ]
            Node& node = m_nodes[item];
            node.children[1] = root;
            root = item;
            if ((item = node.children[0]) < 0)
            {
                prev = root;
                while (((item = m_nodes[prev].parent) >= 0) && (m_nodes[item].children[1] != prev))
                {
                    prev = item;
                }
            }
            else
            {
                while ((prev = m_nodes[item].children[1]) >= 0)
                {
                    item = prev;
                }
            }
        }
        prev = last;
        int index = 0;
        for (item = root; item >= 0; item = next)
        {   //  make the list bi-directional and set the parent index to be the linear ordered index of the node
            Node& node = m_nodes[item];
            node.parent = index;
            node.children[0] = prev;
            next = node.children[1];
            prev = item;
            ++index;
        }
        m_nodes[last].children[1] = root;
    }
    return(root);
}

int ArraySort::sorted(const int parent, const int lower, const int upper)
{   //  create a contiguous balanced tree for sorted data (note: lower and upper represent an inclusive range)
    int index = -1;
    if (lower <= upper)
    {
        index = ((lower + upper) >> 1);
        Node& node = m_nodes[index];
        node.parent = parent;
        node.children[0] = sorted(index, lower, (index - 1));
        node.children[1] = sorted(index, (index + 1), upper);
        node.balance = static_cast<short>(upper + lower - index - index);
        node.flag = k_used;
    }
    return(index);
}

void ArraySort::extend(const int lower, const int upper)
{   //  create a bi-directional looping linked list of free nodes (note: lower and upper represent an inclusive range)
    for (int index = lower; index <= upper; ++index)
    {
        Node& node = m_nodes[index];
        node.parent = -1;
        node.children[0] = (index - 1);
        node.children[1] = (index + 1);
        node.balance = 0;
        node.flag = k_free;
    }
    m_nodes[lower].children[0] = upper;
    m_nodes[upper].children[1] = lower;
}

bool ArraySort::insert(const int index, const void* const key)
{
    if (index >= 0)
    {
        Node& insertNode = m_nodes[index];
        int parent = m_rootIndex;
        int child = index;
        if (parent >= 0)
        {   //  this is not the first node to be inserted so will need sorting
            for (;;)
            {
                int compare = m_query->cmpKey(key, parent);
                if (compare == 0)
                {   //  this should only happen if the data array has been altered without taking account of the sorting or collisions (critical error)
                    return(false);
                }
                Node& parentNode = m_nodes[parent];
                int flip = ((compare >> ((sizeof(int) << 3) - 1)) + 1);
                if (parentNode.children[flip] < 0)
                {
                    parentNode.children[flip] = index;
                    insertNode.parent = parent;
                    break;
                }
                parent = parentNode.children[flip];
            }
            while (parent >= 0)
            {
                Node& parentNode = m_nodes[parent];
                short balance = ((parentNode.children[0] == child) ? -1 : 1);
                if ((parentNode.balance - balance) == 0)
                {
                    Node& childNode = m_nodes[child];
                    if ((childNode.balance + balance) == 0)
                    {
                        child = childNode.children[-(balance >> 1)];
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
                    for (int check = -1; (check = m_nodes[child].parent) >= 0; child = check);
                }
                parent = m_nodes[child].parent;
            }
        }
        m_rootIndex = child;
        return(true);
    }
    return(false);
}

bool ArraySort::unlink(const int index)
{
    if (index >= 0)
    {
        Node& unlinkNode = m_nodes[index];
        int child = index;
        int flip = ((unlinkNode.balance < 0) ? 0 : 1);
        if (unlinkNode.children[flip] >= 0)
        {
            child = unlinkNode.children[flip];
            flip ^= 1;
            for (int check = -1; ((check = m_nodes[child].children[flip]) >= 0); child = check);
        }
        flip ^= 1;
        rotate(m_nodes[child].children[flip]);
        int parent = m_nodes[child].parent;
        if (parent >= 0)
        {   //  not the last node in the tree
            if (child != index)
            {   //  exchange the linkage of the child node and the node being removed
                if (parent == index) parent = child;
                if (unlinkNode.parent >= 0)
                {
                    Node& parentNode = m_nodes[unlinkNode.parent];
                    parentNode.children[(parentNode.children[0] == index) ? 0 : 1] = child;
                }
                if (unlinkNode.children[0] >= 0) m_nodes[unlinkNode.children[0]].parent = child;
                if (unlinkNode.children[1] >= 0) m_nodes[unlinkNode.children[1]].parent = child;
                Node& childNode = m_nodes[child];
                childNode.parent = unlinkNode.parent;
                childNode.children[0] = unlinkNode.children[0];
                childNode.children[1] = unlinkNode.children[1];
                childNode.balance = unlinkNode.balance;
                unlinkNode.parent = parent;
                unlinkNode.children[0] = unlinkNode.children[1] = -1;
                unlinkNode.balance = 0;
                Node& parentNode = m_nodes[parent];
                parentNode.children[(parentNode.children[0] == child) ? 0 : 1] = index;
                child = index;
            }
            while (parent >= 0)
            {
                Node& parentNode = m_nodes[parent];
                short balance = ((parentNode.children[0] == child) ? 1 : -1);
                if ((parentNode.balance - balance) == 0)
                {
                    child = parentNode.children[(balance + 1) >> 1];
                    Node& childNode = m_nodes[child];
                    if ((childNode.balance + balance) == 0)
                    {
                        child = childNode.children[-(balance >> 1)];
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
                    for (int check = -1; (check = m_nodes[child].parent) >= 0; child = check);
                }
                parent = m_nodes[child].parent;
            }
            Node& parentNode = m_nodes[unlinkNode.parent];
            parentNode.children[(parentNode.children[0] == index) ? 0 : 1] = -1;
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

void ArraySort::rotate(const int index)
{
    if (index >= 0)
    {
        Node& rotateNode = m_nodes[index];
        int parent = rotateNode.parent;
        if (parent >= 0)
        {
            Node& parentNode = m_nodes[parent];
            int gramps = parentNode.parent;
            if (gramps >= 0)
            {
                Node& grampsNode = m_nodes[gramps];
                grampsNode.children[(grampsNode.children[0] == parent) ? 0 : 1] = index;
            }
            rotateNode.parent = gramps;
            parentNode.parent = index;
            if (parentNode.children[0] == index)
            {
                int child = rotateNode.children[1];
                if (child >= 0) m_nodes[child].parent = parent;
                parentNode.children[0] = rotateNode.children[1];
                rotateNode.children[1] = parent;
                if (rotateNode.balance < 0) parentNode.balance -= rotateNode.balance;
                parentNode.balance += 1;
                if (parentNode.balance > 0) rotateNode.balance += parentNode.balance;
                rotateNode.balance += 1;
            }
            else
            {
                int child = rotateNode.children[0];
                if (child >= 0) m_nodes[child].parent = parent;
                parentNode.children[1] = rotateNode.children[0];
                rotateNode.children[0] = parent;
                if (rotateNode.balance > 0) parentNode.balance -= rotateNode.balance;
                parentNode.balance -= 1;
                if (parentNode.balance < 0) rotateNode.balance += parentNode.balance;
                rotateNode.balance -= 1;
            }
        }
    }
}

int ArraySort::height(const int index) const
{
    if (index >= 0)
    {
        Node& node = m_nodes[index];
        int height0 = height(node.children[0]);
        int height1 = height(node.children[1]);
        return(((height0 > height1) ? height0 : height1) + 1);
    }
    return(0);
}

int ArraySort::weight(const int index) const
{
    if (index >= 0)
    {
        Node& node = m_nodes[index];
        return(weight(node.children[0]) + weight(node.children[1]) + 1);
    }
    return(0);
}

bool ArraySort::validate(const int index) const
{
    if (static_cast<uint>(index) >= static_cast<uint>(m_nodeCount))
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
    if ((height(node.children[1]) - height(node.children[0])) != node.balance)
    {
        return(false);
    }
    if (node.children[0] >= 0)
    {
        if (!validate(node.children[0]))
        {
            return(false);
        }
        if (m_nodes[node.children[0]].parent != index)
        {
            return(false);
        }
    }
    if (node.children[1] >= 0)
    {
        if (!validate(node.children[1]))
        {
            return(false);
        }
        if (m_nodes[node.children[1]].parent != index)
        {
            return(false);
        }
    }
    const void* key = m_query->getKey(index);
    if (key == nullptr)
    {
        return(false);
    }
    int prevIndex = prev(index);
    if (prevIndex >= 0)
    {
        if (next(prevIndex) != index)
        {
            return(false);
        }
        if (m_query->cmpKey(key, prevIndex) <= 0)
        {
            return(false);
        }
    }
    int nextIndex = next(index);
    if (nextIndex >= 0)
    {
        if (prev(nextIndex) != index)
        {
            return(false);
        }
        if (m_query->cmpKey(key, nextIndex) >= 0)
        {
            return(false);
        }
    }
    return(true);
}
