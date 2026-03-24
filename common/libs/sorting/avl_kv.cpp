
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   avl_kv.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	AVL key value pair tree handling classes.
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

#include "avl_kv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin sorting namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sorting
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin avl_kv namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace avl_kv
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Node class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Node* Node::find( const uint8_t* key ) const
{
	if( !this ) return( 0 );
	const Node* search = top();
	while( search )
	{
		int compare = cmp( key, search->m_key );
		if( compare == 0 ) break;
		int index = ( ( compare >> ( ( sizeof( int ) << 3 ) - 1 ) ) + 1 );
		search = search->m_children[ index ];
	}
	return( const_cast< Node* >( search ) );
}

Node* Node::first( void ) const
{
	if( !this ) return( 0 );
	const Node* first = this;
	while( first->m_parent ) first = first->m_parent;
	while( first->m_children[ 0 ] ) first = first->m_children[ 0 ];
	return( const_cast< Node* >( first ) );
}

Node* Node::last( void ) const
{
	if( !this ) return( 0 );
	const Node* last = this;
	while( last->m_parent ) last = last->m_parent;
	while( last->m_children[ 1 ] ) last = last->m_children[ 1 ];
	return( const_cast< Node* >( last ) );
}

Node* Node::prev( void ) const
{
	if( !this ) return( 0 );
	const Node* prev = this;
	if( prev->m_children[ 0 ] )
	{
		for( prev = prev->m_children[ 0 ]; prev->m_children[ 1 ]; prev = prev->m_children[ 1 ] );
	}
	else
	{
		for( const Node* child = prev; ( prev = prev->m_parent ) != 0; child = prev ) if( prev->m_children[ 1 ] == child ) break;
	}
	return( const_cast< Node* >( prev ) );
}

Node* Node::next( void ) const
{
	if( !this ) return( 0 );
	const Node* next = this;
	if( next->m_children[ 1 ] )
	{
		for( next = next->m_children[ 1 ]; next->m_children[ 0 ]; next = next->m_children[ 0 ] );
	}
	else
	{
		for( const Node* child = next; ( next = next->m_parent ) != 0; child = next ) if( next->m_children[ 0 ] == child ) break;
	}
	return( const_cast< Node* >( next ) );
}

Node* Node::top( void ) const
{
	if( !this ) return( 0 );
	const Node* top = this;
	while( top->m_parent ) top = top->m_parent;
	return( const_cast< Node* >( top ) );
}

bool Node::loose( void ) const
{
	if( !this ) return( true );
	return( ( m_parent == 0 ) && ( m_children[ 0 ] == 0 ) && ( m_children[ 1 ] == 0 ) );
}

Node* Node::rename( const uint8_t* const key )
{
	if( !this ) return( 0 );
	Node* found = find( key );
	if( found )
	{
		if( found != this ) return( 0 );
		m_key = key;
		return( top() );
	}
	m_key = key;
	return( insert( remove() ) );
}

Node* Node::insert( Node* const root )
{
	if( !this ) return( 0 );
	if( !loose() ) return( 0 );
	if( !root ) return( this );
	if( root->find( m_key ) != 0 ) return( 0 );
	m_balance = 0;
	Node* parent = root->top();
	Node* child = this;
	for(;;)
	{
		int compare = cmp( m_key, parent->m_key );
		if( compare == 0 )
		{
			parent = child = 0;
			break;
		}
		int index = ( ( compare >> ( ( sizeof( int ) << 3 ) - 1 ) ) + 1 );
		if( parent->m_children[ index ] == 0 )
		{
			parent->m_children[ index ] = this;
			m_parent = parent;
			break;
		}
		parent = parent->m_children[ index ];
	}
	while( parent )
	{
		int balance = ( ( parent->m_children[ 0 ] == child ) ? -1 : 1 );
		if( ( parent->m_balance - balance ) == 0 )
		{
			if( ( child->m_balance + balance ) == 0 )
			{
				child = child->m_children[ -( balance >> 1 ) ];
				child->rotate();
			}
			parent->m_balance += balance;
			child->rotate();
		}
		else
		{
			parent->m_balance += balance;
			child = parent;
		}
		if( child->m_balance == 0 ) while( child->m_parent ) child = child->m_parent;
		parent = child->m_parent;
	}
	return( child );
}

Node* Node::remove( void )
{
	if( !this ) return( 0 );
	Node* child = this;
	int index = ( ( m_balance < 0 ) ? 0 : 1 );
	if( child->m_children[ index ] )
	{
		child = m_children[ index ];
		index ^= 1;
		while( child->m_children[ index ] ) child = child->m_children[ index ];
	}
	index ^= 1;
	if( child->m_children[ index ] ) child->m_children[ index ]->rotate();
	Node* parent = child->m_parent;
	if( !parent ) return( 0 );
	if( child != this )
	{   //  exchange the linkage of the child node and the node being removed
        if( parent == this ) parent = child;
		if( m_parent ) m_parent->m_children[ ( m_parent->m_children[ 0 ] == this ) ? 0 : 1 ] = child;
		if( m_children[ 0 ] ) m_children[ 0 ]->m_parent = child;
		if( m_children[ 1 ] ) m_children[ 1 ]->m_parent = child;
		child->m_parent = m_parent;
		child->m_children[ 0 ] = m_children[ 0 ];
		child->m_children[ 1 ] = m_children[ 1 ];
		child->m_balance = m_balance;
		m_parent = parent;
		m_children[ 0 ] = m_children[ 1 ] = 0;
		m_balance = 0;
		parent->m_children[ ( parent->m_children[ 0 ] == child ) ? 0 : 1 ] = this;
		child = this;
	}
	while( parent )
	{
		int balance = ( ( parent->m_children[ 0 ] == child ) ? 1 : -1 );
		if( ( parent->m_balance - balance ) == 0 )
		{
			child = parent->m_children[ ( balance + 1 ) >> 1 ];
			if( ( child->m_balance + balance ) == 0 )
			{
				child = child->m_children[ -( balance >> 1 ) ];
				child->rotate();
			}
			parent->m_balance += balance;
			child->rotate();
		}
		else
		{
			parent->m_balance += balance;
			child = parent;
		}
		if( child->m_balance != 0 ) while( child->m_parent ) child = child->m_parent;
		parent = child->m_parent;
	}
    m_parent->m_children[ ( m_parent->m_children[ 0 ] == this ) ? 0 : 1 ] = 0;
    m_parent = 0;
	return( child );
}

Node* Node::resort( void )
{
	if( !this ) return( 0 );
	Node* list = last();
	for( Node* prev = list->prev(); prev; prev = list->prev() )
	{
		prev->m_children[ 1 ] = list;
		list = prev;
	}
	Node* root = 0;
	while( list )
	{
		Node* next = list->m_children[ 1 ];
        list->m_parent = list->m_children[ 0 ] = list->m_children[ 1 ] = 0;
	    list->m_balance = 0;
		root = list->insert( root );
		list = next;
	}
	return( root );
}

void Node::rotate( void )
{
	if( this )
	{
		Node* parent = m_parent;
		if( parent )
		{
			if( parent->m_parent ) parent->m_parent->m_children[ ( parent->m_parent->m_children[ 0 ] == parent ) ? 0 : 1 ] = this;
			this->m_parent = parent->m_parent;
			parent->m_parent = this;
			if( parent->m_children[ 0 ] == this )
			{
				if( this->m_children[ 1 ] ) this->m_children[ 1 ]->m_parent = parent;
				parent->m_children[ 0 ] = this->m_children[ 1 ];
				this->m_children[ 1 ] = parent;
				if( this->m_balance < 0 ) parent->m_balance -= this->m_balance;
				parent->m_balance += 1;
				if( parent->m_balance > 0 ) this->m_balance += parent->m_balance;
				this->m_balance += 1;
			}
			else
			{
				if( this->m_children[ 0 ] ) this->m_children[ 0 ]->m_parent = parent;
				parent->m_children[ 1 ] = this->m_children[ 0 ];
				this->m_children[ 0 ] = parent;
				if( this->m_balance > 0 ) parent->m_balance -= this->m_balance;
				parent->m_balance -= 1;
				if( parent->m_balance < 0 ) this->m_balance += parent->m_balance;
				this->m_balance -= 1;
			}
		}
	}
}

int Node::height( void ) const
{
	if( this )
	{
		int height0 = m_children[ 0 ]->height();
		int height1 = m_children[ 1 ]->height();
		return( ( ( height0 > height1 ) ? height0 : height1 ) + 1 );
	}
	return( 0 );
}

int Node::weight( void ) const
{
	if( this )
	{
		return( m_children[ 0 ]->weight() + m_children[ 1 ]->weight() + 1 );
	}
	return( 0 );
}

bool Node::validate( void ) const
{
	if( !this )
    {
        return( false );
    }
	if( ( m_balance < -1 ) || ( m_balance > 1 ) )
    {
        return( false );
    }
	if( ( m_children[ 1 ]->height() - m_children[ 0 ]->height() ) != m_balance )
    {
        return( false );
    }
	if( m_children[ 0 ] )
    {
	    if( !m_children[ 0 ]->validate() )
        {
            return( false );
        }
        if( m_children[ 0 ]->m_parent != this )
        {
            return( false );
        }
    }
	if( m_children[ 1 ] )
    {
	    if( !m_children[ 1 ]->validate() )
        {
            return( false );
        }
        if( m_children[ 1 ]->m_parent != this )
        {
            return( false );
        }
    }
	if( const Node* node = prev() )
	{
		if( node->next() != this )
        {
            return( false );
        }
		if( cmp( node->m_key, m_key ) >= 0 )
        {
            return( false );
        }
	}
	if( const Node* node = next() )
	{
		if( node->prev() != this )
        {
            return( false );
        }
		if( cmp( m_key, node->m_key ) >= 0 )
        {
            return( false );
        }
	}
	return( true );
}

int Node::cmp( const uint8_t* const key1, const uint8_t* const key2 )
{
	int char1 = 0;
	int char2 = 0;
    int index = 0;
	do
	{
		char1 = static_cast< int >( key1[ index ] );
		char2 = static_cast< int >( key2[ index ] );
		char1 -= char2;
		++index;
	}
	while( ( char1 == 0 ) && ( char2 != 0 ) );
    return( char1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Tree class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Tree::contains( const Node* const node ) const
{
	if( this && node )
	{
		return( node->top() == m_root );
	}
	return( false );
}

bool Tree::rename( Node* const node, const uint8_t* const key )
{
	if( this && node && key )
	{
		if( node->top() == m_root )
		{
			Node* root = node->rename( key );
			if( root )
			{
				m_root = root;
				return( true );
			}
		}
	}
	return( false );
}

bool Tree::insert( Node* const node )
{
	if( this && node )
	{	//	node exists
		Node* root = node->insert( m_root );
		if( root )
		{
			m_root = root;
			return( true );
		}
	}
	return( false );
}

bool Tree::remove( Node* const node )
{
	if( this && node )
	{
		if( m_root == node->top() )
		{
			m_root = node->remove();
			return( true );
		}
	}
	return( false );
}

void Tree::resort( void )
{
	if( this )
	{
		m_root = m_root->resort();
	}
}

Node* Tree::find( const uint8_t* const key ) const
{
	return( ( this && key ) ? m_root->find( key ) : 0 );
}

Node* Tree::root( void ) const
{
	return( this ? m_root : 0 );
}

Node* Tree::first( void ) const
{
	return( this ? m_root->first() : 0 );
}

Node* Tree::last( void ) const
{
	return( this ? m_root->last() : 0 );
}

int Tree::height( void ) const
{
	return( this ? m_root->height() : 0 );
}

int Tree::weight( void ) const
{
	return( this ? m_root->weight() : 0 );
}

bool Tree::validate( void ) const
{
	return( this ? m_root->validate() : true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end avl_kv namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace avl_kv

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end sorting namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace sorting

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

