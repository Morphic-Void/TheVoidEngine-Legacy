
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   registry.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	REGISTRY tree node class.
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

#ifndef	__REGISTRY_INCLUDED__
#define	__REGISTRY_INCLUDED__

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "auto_id.h"
#include "libs/system/base/types.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Forward declarations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Mediator;
class Registry;
template< typename T > class IShared;
template< typename T > class IUsable;
template< typename T > class IOwnShared;
template< typename T > class IOwnUsable;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    IMediator interface
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IMediator
{
public:
	virtual bool			IsValid( void ) const = 0;
	virtual bool			IsConst( void ) const = 0;
	virtual const uint8_t*	GetName( void ) const = 0;
	virtual unsigned int	GetType( void ) const = 0;
	virtual unsigned int	GetCount( void ) const = 0;
protected:
	inline					IMediator() {};
	virtual					~IMediator() {};
	virtual	bool			AddRef( void ) = 0;
	virtual	void			Abandon( void ) = 0;
	virtual	void			Release( void ) = 0;
	virtual uint8_t*		Get( const unsigned int type, const bool constant, const unsigned int index ) const = 0;
	virtual bool			Set( const unsigned int type, const bool constant, const unsigned int count, uint8_t* const absolute, const uint8_t* const* const redirect, const bool indirect ) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Templated accessor interfaces
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename T >
class IShared : public IMediator
{
public:
	inline void				Release( void ) { IMediator::Release(); };
	inline const T*			Get( const unsigned int index = 0 ) const;
	inline IShared< T >*	AddShared( void );
protected:
	inline					IShared() {};
	virtual					~IShared() {};
};

template< typename T >
class IUsable : public IMediator
{
public:
	inline void				Release( void ) { IMediator::Release(); };
	inline T*				Get( const unsigned int index = 0 ) const;
	inline IShared< T >*	AddShared( void );
	inline IUsable< T >*	AddUsable( void );
protected:
	inline					IUsable() {};
	virtual					~IUsable() {};
};

template< typename T >
class IOwnShared : public IMediator
{
public:
	inline void				Abandon( void ) { IMediator::Abandon(); };
	inline bool				Set( const T& shared, const unsigned int count = 1 );
	inline bool				Set( const T*& shared, const unsigned int count = 1 );
	inline bool				Set( const T& shared, const void* const& redirect, const unsigned int count = 1 );
	inline bool				Set( const T*& shared, const void* const& redirect, const unsigned int count = 1 );
	inline IShared< T >*	AddShared( void );
protected:
	inline					IOwnShared() {};
	virtual					~IOwnShared() {};
};

template< typename T >
class IOwnUsable : public IMediator
{
public:
	inline void				Abandon( void ) { IMediator::Abandon(); };
	inline bool				Set( T& usable, const unsigned int count = 1 );
	inline bool				Set( T*& usable, const unsigned int count = 1 );
	inline bool				Set( T& usable, const void* const& redirect, const unsigned int count = 1 );
	inline bool				Set( T*& usable, const void* const& redirect, const unsigned int count = 1 );
	inline IShared< T >*	AddShared( void );
	inline IUsable< T >*	AddUsable( void );
protected:
	inline					IOwnUsable() {};
	virtual					~IOwnUsable() {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Registry class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Registry
{
public:
	inline					Registry() : m_root( 0 ), m_pool( 0 ) {};
	inline					Registry( Registry& registry ) : m_root( 0 ), m_pool( 0 ) { SetPool( registry ); };
	inline					Registry( Mediator*& pool ) : m_root( 0 ), m_pool( 0 ) { SetPool( pool ); };
	inline					~Registry() { Delete(); };
	bool					SetPool( Registry& registry );
	bool					SetPool( Mediator*& pool );
	void					Optimize( void );
	template< typename T >
	IShared< T >*			ObtainShared( const uint8_t* const name ) const;
	template< typename T >
	IUsable< T >*			ObtainUsable( const uint8_t* const name ) const;
	template< typename T >
	inline IOwnShared< T >*	RegisterShared( const uint8_t* const name, const T& shared, const unsigned int count = 1 );
	template< typename T >
	inline IOwnShared< T >*	RegisterShared( const uint8_t* const name, const T*& shared, const unsigned int count = 1 );
	template< typename T >
	inline IOwnShared< T >*	RegisterShared( const uint8_t* const name, const T& shared, const void* const& redirect, const unsigned int count = 1 );
	template< typename T >
	inline IOwnShared< T >*	RegisterShared( const uint8_t* const name, const T*& shared, const void* const& redirect, const unsigned int count = 1 );
	template< typename T >
	inline IOwnUsable< T >*	RegisterUsable( const uint8_t* const name, const T& usable, const unsigned int count = 1 );
	template< typename T >
	inline IOwnUsable< T >*	RegisterUsable( const uint8_t* const name, const T*& usable, const unsigned int count = 1 );
	template< typename T >
	inline IOwnUsable< T >*	RegisterUsable( const uint8_t* const name, const T& usable, const void* const& redirect, const unsigned int count = 1 );
	template< typename T >
	inline IOwnUsable< T >*	RegisterUsable( const uint8_t* const name, const T*& usable, const void* const& redirect, const unsigned int count = 1 );
protected:
	Mediator*				Create( const uint8_t* const name, const unsigned int type, const bool constant, const unsigned int count, uint8_t* const absolute, const uint8_t* const* const redirect, const bool indirect, const unsigned int size );
	Mediator*				Obtain( const uint8_t* const name, const unsigned int type, const bool constant ) const;
	void					Delete( void );
	Mediator*				m_root;
	Mediator**				m_pool;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Templated accessor interfaces inline functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename T >
const T* IShared< T >::Get( const unsigned int index ) const
{
	return( reinterpret_cast< const T* >( IMediator::Get( AUTO_ID< T >(), true, index ) ) );
}

template< typename T >
IShared< T >* IShared< T >::AddShared( void )
{
	return( IMediator::AddRef() ? reinterpret_cast< IShared< T >* >( this ) : 0 );
}

template< typename T >
T* IUsable< T >::Get( const unsigned int index ) const
{
	return( reinterpret_cast< T* >( IMediator::Get( AUTO_ID< T >(), false, index ) ) );
}

template< typename T >
IShared< T >* IUsable< T >::AddShared( void )
{
	return( IMediator::AddRef() ? reinterpret_cast< IShared< T >* >( this ) : 0 );
}

template< typename T >
IUsable< T >* IUsable< T >::AddUsable( void )
{
	return( IMediator::AddRef() ? reinterpret_cast< IUsable< T >* >( this ) : 0 );
}

template< typename T >
bool IOwnShared< T >::Set( const T& shared, const unsigned int count )
{
	return( IMediator::Set( AUTO_ID< T >(), true, count,
		reinterpret_cast< uint8_t* const >( const_cast< T* const >( &shared ) ), 0, false ) );
}

template< typename T >
bool IOwnShared< T >::Set( const T*& shared, const unsigned int count )
{
	return( IMediator::Set( AUTO_ID< T >(), true, count,
		reinterpret_cast< uint8_t* const >( const_cast< T** const >( &shared ) ), 0, true ) );
}

template< typename T >
bool IOwnShared< T >::Set( const T& shared, const void* const& redirect, const unsigned int count )
{
	return( IMediator::Set( AUTO_ID< T >(), true, count,
		reinterpret_cast< uint8_t* const >( const_cast< T* const >( &shared ) ),
		reinterpret_cast< const uint8_t* const* const>( &redirect ), false ) );
}

template< typename T >
bool IOwnShared< T >::Set( const T*& shared, const void* const& redirect, const unsigned int count )
{
	return( IMediator::Set( AUTO_ID< T >(), true, count,
		reinterpret_cast< uint8_t* const >( const_cast< T** const >( &shared ) ),
		reinterpret_cast< const uint8_t* const* const>( &redirect ), true ) );
}

template< typename T >
IShared< T >* IOwnShared< T >::AddShared( void )
{
	return( IMediator::AddRef() ? reinterpret_cast< IShared< T >* >( this ) : 0 );
}

template< typename T >
bool IOwnUsable< T >::Set( T& usable, const unsigned int count )
{
	return( IMediator::Set( AUTO_ID< T >(), false, count,
		reinterpret_cast< uint8_t* const >( &usable ), 0, false ) );
}

template< typename T >
bool IOwnUsable< T >::Set( T*& usable, const unsigned int count )
{
	return( IMediator::Set( AUTO_ID< T >(), false, count,
		reinterpret_cast< uint8_t* const >( &usable ), 0, true ) );
}

template< typename T >
bool IOwnUsable< T >::Set( T& usable, const void* const& redirect, const unsigned int count )
{
	return( IMediator::Set( AUTO_ID< T >(), false, count,
		reinterpret_cast< uint8_t* const >( &usable ),
		reinterpret_cast< const uint8_t* const* const>( &redirect ), false ) );
}

template< typename T >
bool IOwnUsable< T >::Set( T*& usable, const void* const& redirect, const unsigned int count )
{
	return( IMediator::Set( AUTO_ID< T >(), false, count,
		reinterpret_cast< uint8_t* const >( &usable ),
		reinterpret_cast< const uint8_t* const* const>( &redirect ), true ) );
}

template< typename T >
IShared< T >* IOwnUsable< T >::AddShared( void )
{
	return( IMediator::AddRef() ? reinterpret_cast< IShared< T >* >( this ) : 0 );
}

template< typename T >
IUsable< T >* IOwnUsable< T >::AddUsable( void )
{
	return( IMediator::AddRef() ? reinterpret_cast< IUsable< T >* >( this ) : 0 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////    Registry class inline functions
//////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename T >
IShared< T >* Registry::ObtainShared( const uint8_t* const name ) const
{
	return( reinterpret_cast< IShared< T >* >( Obtain( name, AUTO_ID< T >(), true ) ) );
}

template< typename T >
IUsable< T >* Registry::ObtainUsable( const uint8_t* const name ) const
{
	return( reinterpret_cast< IUsable< T >* >( Obtain( name, AUTO_ID< T >(), false ) ) );
}

template< typename T >
IOwnShared< T >* Registry::RegisterShared( const uint8_t* const name, const T& shared, const unsigned int count )
{
	return( reinterpret_cast< IOwnShared< T >* >( Create( name, AUTO_ID< T >(), true, count,
		reinterpret_cast< uint8_t* const >( const_cast< T* const >( &shared ) ), 0, false, sizeof( T ) ) ) );
}

template< typename T >
IOwnShared< T >* Registry::RegisterShared( const uint8_t* const name, const T*& shared, const unsigned int count )
{
	return( reinterpret_cast< IOwnShared< T >* >( Create( name, AUTO_ID< T >(), true, count,
		reinterpret_cast< uint8_t* const >( const_cast< T** const >( &shared ) ), 0, true, sizeof( T ) ) ) );
}

template< typename T >
IOwnShared< T >* Registry::RegisterShared( const uint8_t* const name, const T& shared, const void* const& redirect, const unsigned int count )
{
	return( reinterpret_cast< IOwnShared< T >* >( Create( name, AUTO_ID< T >(), true, count,
		reinterpret_cast< uint8_t* const >( const_cast< T* const >( &shared ) ),
		reinterpret_cast< const uint8_t* const* const>( &redirect ), false, sizeof( T ) ) ) );
}

template< typename T >
IOwnShared< T >* Registry::RegisterShared( const uint8_t* const name, const T*& shared, const void* const& redirect, const unsigned int count )
{
	return( reinterpret_cast< IOwnShared< T >* >( Create( name, AUTO_ID< T >(), true, count,
		reinterpret_cast< uint8_t* const >( const_cast< T** const >( &shared ) ),
		reinterpret_cast< const uint8_t* const* const>( &redirect ), true, sizeof( T ) ) ) );
}

template< typename T >
IOwnUsable< T >* Registry::RegisterUsable( const uint8_t* const name, const T& usable, const unsigned int count )
{
	return( reinterpret_cast< IOwnUsable< T >* >( Create( name, AUTO_ID< T >(), false, count,
		reinterpret_cast< uint8_t* const >( &usable ), 0, false, sizeof( T ) ) ) );
}

template< typename T >
IOwnUsable< T >* Registry::RegisterUsable( const uint8_t* const name, const T*& usable, const unsigned int count )
{
	return( reinterpret_cast< IOwnUsable< T >* >( Create( name, AUTO_ID< T >(), false, count,
		reinterpret_cast< uint8_t* const >( &usable ), 0, true, sizeof( T ) ) ) );
}

template< typename T >
IOwnUsable< T >* Registry::RegisterUsable( const uint8_t* const name, const T& usable, const void* const& redirect, const unsigned int count )
{
	return( reinterpret_cast< IOwnUsable< T >* >( Create( name, AUTO_ID< T >(), false, count,
		reinterpret_cast< uint8_t* const >( &usable ),
		reinterpret_cast< const uint8_t* const* const>( &redirect ), false, sizeof( T ) ) ) );
}

template< typename T >
IOwnUsable< T >* Registry::RegisterUsable( const uint8_t* const name, const T*& usable, const void* const& redirect, const unsigned int count )
{
	return( reinterpret_cast< IOwnUsable< T >* >( Create( name, AUTO_ID< T >(), false, count,
		reinterpret_cast< uint8_t* const >( &usable ),
		reinterpret_cast< const uint8_t* const* const>( &redirect ), true, sizeof( T ) ) ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__REGISTRY_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

