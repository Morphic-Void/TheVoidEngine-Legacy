
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   mediator.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        Mediator tree class.
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

#include "mediator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Mediator class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mediator* Mediator::Create( Mediator*& pool, Mediator*& root, const uint8_t* const name, const unsigned int type, const bool constant, const unsigned int count, uint8_t* const absolute, const uint8_t* const* const redirect, const bool indirect, const unsigned int size )
{
	Mediator* mediator = pool;
	if( mediator )
	{
		Mediator* free = static_cast< Mediator* >( mediator->m_parent );
		mediator->m_parent = 0;
		mediator->m_name = name;
		Mediator* tree = static_cast< Mediator* >( mediator->insert( root ) );
		if( tree )
		{
			pool = free;
			root = tree;
			mediator->m_flags = FLAG_VALID;
			if( constant ) mediator->m_flags |= FLAG_CONST;
			if( indirect ) mediator->m_flags |= FLAG_TABLE;
			mediator->m_count = count;
			mediator->m_type = type;
			mediator->m_size = size;
			mediator->m_pool = &pool;
			mediator->m_root = &root;
			mediator->m_absolute = absolute;
			mediator->m_relative = ( redirect ? *redirect : 0 );
			mediator->m_redirect = redirect;

		}
		else
		{
			mediator->m_parent = free;
			mediator->m_name = 0;
			mediator = 0;
		}
	}
	return( mediator );
}

Mediator* Mediator::Obtain( const uint8_t* const name, const unsigned int type, const bool constant ) const
{
	if( this )
	{
		Mediator* found = static_cast< Mediator* >( Node::find( name ) );
		if( found )
		{
			if( ( found->m_flags & FLAG_VALID ) && ( type == found->m_type ) && ( ( ( found->m_flags & FLAG_CONST ) == 0 ) || constant ) )
			{
				found->m_refs++;
				return( found );
			}
		}
	}
	return( 0 );
}

void Mediator::Delete( void )
{
	if( this )
	{
		*m_root = static_cast< Mediator* >( remove() );
		Null();
		m_parent = *m_pool;
		*m_pool = this;
	}
}

bool Mediator::Rename( const uint8_t* const name )
{
	if( this )
	{
		Mediator* root = static_cast< Mediator* >( Node::rename( name ) );
		if( root )
		{
			*m_root = root;
			return( true );
		}
	}
	return( false );
}

void Mediator::Resort( void )
{
	if( this )
	{
		*m_root = static_cast< Mediator* >( Node::resort() );
	}
}

Mediator* Mediator::InitPool( const unsigned int count )
{
	if( count )
	{
		unsigned int index = ( count - 1 );
		this[ index ].m_parent = 0;
		while( index )
		{
			--index;
			this[ index ].m_parent = &this[ index + 1 ];
		}
		return( this );
	}
	return( 0 );
}

void Mediator::Null( void )
{
	m_flags = 0;
	m_count = 0;
	m_type = 0;
	m_size = 0;
	m_refs = 0;
	m_pool = 0;
	m_root = 0;
	m_absolute = 0;
	m_relative = 0;
	m_redirect = 0;
	m_name = 0;
	m_parent = 0;
	m_children[ 0 ] = 0;
	m_children[ 1 ] = 0;
	m_balance = 0;
}

bool Mediator::AddRef( void )
{
	if( m_flags & FLAG_VALID )
	{
		++m_refs;
		return( true );
	}
	return( false );
}

void Mediator::Abandon( void )
{
	if( m_flags & FLAG_VALID )
	{
		m_flags ^= FLAG_VALID;
		if( !m_refs )
		{
			Delete();
		}
	}
}

void Mediator::Release( void )
{
	if( m_refs )
	{
		--m_refs;
		if( !m_refs && !( m_flags & FLAG_VALID ) )
		{
			Delete();
		}
	}
}

uint8_t* Mediator::Get( const unsigned int type, const bool constant, const unsigned int index ) const
{
	if( ( m_flags & FLAG_VALID ) && ( type == m_type ) && ( index < m_count ) && ( ( ( m_flags & FLAG_CONST ) == 0 ) || constant ) )
	{
		uint8_t* address = ( ( m_flags & FLAG_TABLE ) ? reinterpret_cast< uint8_t** >( m_absolute )[ index ] : ( m_absolute + ( index * m_size ) ) );
		if( m_redirect ) address += ( *m_redirect - m_relative );
		return( address );
	}
	return( 0 );
}

bool Mediator::Set( const unsigned int type, const bool constant, const unsigned int count, uint8_t* const absolute, const uint8_t* const* const redirect, bool indirect )
{
	if( ( m_flags & FLAG_VALID ) && ( type == m_type ) && ( count > 0 ) && ( ( m_flags & FLAG_CONST ) || !constant ) )
	{
		m_flags |= FLAG_TABLE;
		if( !indirect ) m_flags ^= FLAG_TABLE;
		m_count = count;
		m_absolute = absolute;
		m_relative = ( redirect ? *redirect : 0 );
		m_redirect = redirect;
		return( true );
	}
	return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
