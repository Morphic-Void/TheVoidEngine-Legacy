
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   widgets.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Widget geometry construction.
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

#include "widgets.h"
#include "libs/maths/vec3.h"
#include "libs/maths/consts.h"
#include "libs/system/debug/asserts.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin widgets namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace widgets
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    grid widget builder class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WIDGET_TYPE Grid::GetType( void ) const
{
	return( WIDGET_VECTOR_GRID );
}

uint32_t Grid::GetPrims( void ) const
{
	return( ( ( m_size - 1 ) << 2 ) + ( m_border ? 6 : 2 ) );
}

uint32_t Grid::GetIndices( void ) const
{
	return( ( ( m_size - 1 ) << 3 ) + ( m_border ? 12 : 4 ) );
}

uint32_t Grid::GetIndices( IIB& buffer ) const
{
	uint32_t n = ( ( ( m_size - 1 ) << 3 ) + 4 );
	for( uint32_t i = 0; i < n; ++i )
	{
		buffer.Set( i );
	}
	if( m_border )
	{
		buffer.Set( n );
		buffer.Set( n + 1 );
		buffer.Set( n + 1 );
		buffer.Set( n + 2 );
		buffer.Set( n + 2 );
		buffer.Set( n + 3 );
		buffer.Set( n + 3 );
		buffer.Set( n );
	}
	return( GetIndices() );
}

uint32_t Grid::GetVertices( void ) const
{
	return( ( ( m_size - 1 ) << 3 ) + ( m_border ? 8 : 4 ) );
}

uint32_t Grid::GetVertices( IVB& buffer ) const
{
	float ep = static_cast< float >( m_size );
	float en = -ep;
	float gp = 1.0f;
	float gn = -gp;
	for( uint32_t i = ( m_size - 1 ); i; --i )
	{
		buffer.Set( en, gn, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( ep, gn, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( gp, en, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( gp, ep, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( ep, gp, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( en, gp, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( gn, ep, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( gn, en, 0, 0, 0, -1, m_grid_colour );
		gp += 1.0f;
		gn -= 1.0f;
	}
	buffer.Set( en, 0.0f, 0.0f, 0, 0, -1, m_axis_colour );
	buffer.Set( ep, 0.0f, 0.0f, 0, 0, -1, m_axis_colour );
	buffer.Set( 0.0f, en, 0.0f, 0, 0, -1, m_axis_colour );
	buffer.Set( 0.0f, ep, 0.0f, 0, 0, -1, m_axis_colour );
	if( m_border )
	{
		buffer.Set( en, en, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( ep, en, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( ep, ep, 0, 0, 0, -1, m_grid_colour );
		buffer.Set( en, ep, 0, 0, 0, -1, m_grid_colour );
	}
	return( GetVertices() );
}

void Grid::Set( const uint32_t size, const bool border, const uint32_t grid_colour, const uint32_t axis_colour )
{
	m_size = ( ( size < 1 ) ? 1 : ( ( size > 64 ) ? 64 : size ) );
	m_border = border;
	m_grid_colour = grid_colour;
	m_axis_colour = axis_colour;
}

void Grid::Clear( void )
{
	m_size = 10;
	m_border = true;
	m_grid_colour = MAKE_XRGB8( 63, 63, 63 );
	m_axis_colour = MAKE_XRGB8( 31, 31, 31 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    star widget builder class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WIDGET_TYPE Star::GetType( void ) const
{
	return( WIDGET_VECTOR_STAR );
}

uint32_t Star::GetPrims( void ) const
{
	return( 13 );
}

uint32_t Star::GetIndices( void ) const
{
	return( 26 );
}

uint32_t Star::GetIndices( IIB& buffer ) const
{
	for( uint32_t i = 0; i < 26; ++i )
	{
		buffer.Set( i );
	}
	return( GetIndices() );
}

uint32_t Star::GetVertices( void ) const
{
	return( 26 );
}

uint32_t Star::GetVertices( IVB& buffer ) const
{
	static const float v[ 13 ][ 3 ] = {
		{                     1.0f,                     0.0f,                     0.0f },
		{                     0.0f,                     1.0f,                     0.0f },
		{                     0.0f,                     0.0f,                     1.0f },
		{  maths::consts::RCPROOT2,  maths::consts::RCPROOT2,                     0.0f },
		{  maths::consts::RCPROOT2, -maths::consts::RCPROOT2,                     0.0f },
		{                     0.0f,  maths::consts::RCPROOT2,  maths::consts::RCPROOT2 },
		{                     0.0f,  maths::consts::RCPROOT2, -maths::consts::RCPROOT2 },
		{  maths::consts::RCPROOT2,                     0.0f,  maths::consts::RCPROOT2 },
		{ -maths::consts::RCPROOT2,                     0.0f,  maths::consts::RCPROOT2 },
		{  maths::consts::RCPROOT3,  maths::consts::RCPROOT3,  maths::consts::RCPROOT3 },
		{ -maths::consts::RCPROOT3,  maths::consts::RCPROOT3,  maths::consts::RCPROOT3 },
		{  maths::consts::RCPROOT3, -maths::consts::RCPROOT3,  maths::consts::RCPROOT3 },
		{ -maths::consts::RCPROOT3, -maths::consts::RCPROOT3,  maths::consts::RCPROOT3 } };
	for( uint32_t i = 0; i < 13; ++i )
	{
		buffer.Set(  v[ i ][ 0 ],  v[ i ][ 1 ],  v[ i ][ 2 ],  v[ i ][ 0 ],  v[ i ][ 1 ],  v[ i ][ 2 ], m_colour );
		buffer.Set( -v[ i ][ 0 ], -v[ i ][ 1 ], -v[ i ][ 2 ], -v[ i ][ 0 ], -v[ i ][ 1 ], -v[ i ][ 2 ], m_colour );
	}
	return( GetVertices() );
}

void Star::Set( const uint32_t colour )
{
	m_colour = colour;
}

void Star::Clear( void )
{
	m_colour = MAKE_XRGB8( 255, 191, 63 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    cube widget builder class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WIDGET_TYPE Cube::GetType( void ) const
{
	return( m_fill ? ( m_alt ? WIDGET_FILLED_CUBE2 : WIDGET_FILLED_CUBE1 ) : ( m_alt ? WIDGET_VECTOR_CUBE2 : WIDGET_VECTOR_CUBE1 ) );
}

uint32_t Cube::GetPrims( void ) const
{
	return( 12 );
}

uint32_t Cube::GetIndices( void ) const
{
	return( m_fill ? 36 : 24 );
}

uint32_t Cube::GetIndices( IIB& buffer ) const
{
	if( m_fill )
	{
		if( m_alt )
		{	//	24 vertices
			static const uint8_t o[ 6 ] = { 0, 1, 2, 2, 1, 3 };
			for( uint32_t i = 0; i < 24; i += 4 )
			{
				for( uint32_t j = 0; j < 6; ++j )
				{
					buffer.Set( i + o[ j ] );
				}
			}
		}
		else
		{	//	8 vertices
			static const uint8_t o[ 36 ] = {
				7, 5, 6, 6, 5, 4, 7, 6, 3, 3, 6, 2,
				7, 3, 5, 5, 3, 1, 0, 1, 2, 2, 1, 3,
				0, 4, 1, 1, 4, 5, 0, 2, 4, 4, 2, 6 };
			for( uint32_t i = 0; i < 36; ++i )
			{
				buffer.Set( o[ i ] );
			}
		}
	}
	else
	{
		if( m_alt )
		{	//	24 vertices
			static const uint8_t o[ 24 ] = {
				 0, 1, 0, 2, 6, 4, 6, 7,
				12,13,12,14,11, 9,11,10,
				17,16,17,19,23,22,23,21 };
			for( uint32_t i = 0; i < 24; ++i )
			{
				buffer.Set( o[ i ] );
			}
		}
		else
		{	//	8 vertices
			static const uint8_t o[ 24 ] = {
				0, 1, 0, 2, 0, 4, 3, 2,
				3, 1, 3, 7, 5, 4, 5, 7,
				5, 1, 6, 7, 6, 4, 6, 2 };
			for( uint32_t i = 0; i < 24; ++i )
			{
				buffer.Set( o[ i ] );
			}
		}
	}
	return( GetIndices() );
}

uint32_t Cube::GetVertices( void ) const
{
	return( m_alt ? 24 : 8 );
}

uint32_t Cube::GetVertices( IVB& buffer ) const
{
	if( m_alt )
	{
		static const char v[ 6 ][ 4 ][ 3 ] = {
			{ {  1,  1,  1 }, {  1, -1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } },
			{ {  1,  1,  1 }, {  1,  1, -1 }, { -1,  1,  1 }, { -1,  1, -1 } },
			{ {  1,  1,  1 }, { -1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } },
			{ { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1, -1 }, { -1,  1,  1 } },
			{ { -1, -1, -1 }, {  1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } },
			{ { -1, -1, -1 }, { -1,  1, -1 }, {  1, -1, -1 }, {  1,  1, -1 } } };
		static const char n[ 6 ][ 3 ] = {
			{  1,  0,  0 }, {  0,  1,  0 }, {  0,  0,  1 }, { -1,  0,  0 }, {  0, -1,  0 }, {  0,  0, -1 } };
		for( uint32_t i = 0; i < 6; ++i )
		{
			for( uint32_t j = 0; j < 4; ++j )
			{
				buffer.Set(
					v[ i ][ j ][ 0 ], v[ i ][ j ][ 1 ], v[ i ][ j ][ 2 ],
					n[ i ][ 0 ], n[ i ][ 1 ], n[ i ][ 2 ], m_colour );
			}
		}
	}
	else
	{
		for( uint32_t i = 0; i < 8; ++i )
		{
			float x = ( ( ( i >> 1 ) & 2 ) - 1.0f );
			float y = ( ( i & 2 ) - 1.0f );
			float z = ( ( ( i << 1 ) & 2 ) - 1.0f );
			buffer.Set( x, y, z, x, y, z, m_colour );
		}
	}
	return( GetVertices() );
}

void Cube::SetFill( const bool fill )
{
	m_fill = fill;
}

void Cube::Set( const uint32_t colour, const bool alt, const bool fill )
{
	m_fill = fill;
	m_colour = colour;
	m_alt = alt;
}

void Cube::Clear( void )
{
	m_fill = true;
	m_colour = MAKE_XRGB8( 127, 127, 0 );
	m_alt = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    sphere widget builder class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WIDGET_TYPE Sphere::GetType( void ) const
{
	return( m_fill ? ( m_alt ? WIDGET_FILLED_SPHERE2 : WIDGET_FILLED_SPHERE1 ) : ( m_alt ? WIDGET_VECTOR_SPHERE2 : WIDGET_VECTOR_SPHERE1 ) );
}

uint32_t Sphere::GetPrims( void ) const
{
	uint32_t d = m_density;
	uint32_t dd = ( d * d );
	if( m_fill )
	{
		return( ( m_alt ? ( dd + dd - d ) : dd ) << 3 );
	}
	else
	{
		return( ( m_alt ? ( ( dd << 2 ) - d ) : ( dd + dd + dd ) ) << 2 );
	}
}

uint32_t Sphere::GetIndices( void ) const
{
	uint32_t d = m_density;
	uint32_t dd = ( d * d );
	if( m_fill )
	{
		return( ( m_alt ? ( dd + dd - d ) : dd ) * 24 );
	}
	else
	{
		return( ( m_alt ? ( ( dd << 2 ) - d ) : ( dd + dd + dd ) ) << 3 );
	}
}

uint32_t Sphere::GetIndices( IIB& buffer ) const
{
	uint32_t d = m_density;
	uint32_t d2 = ( d + d );
	uint32_t d4 = ( d2 + d2 );
	if( m_fill )
	{
		uint32_t tl, tr, bl, br;
		tl = tr = bl = br = 0;
		if( m_alt )
		{
			bl = d4;
			for( uint32_t i = d4; i; --i )
			{	//	top cap
				++br;
				buffer.Set( tl );
				buffer.Set( bl );
				buffer.Set( br );
				bl = br;
			}
			for( uint32_t i = ( d2 - 2 ); i; --i )
			{
				tl += d4;
				bl += d4;
				for( uint32_t j = d4; j; --j )
				{	//	center rows
					++tr;
					++br;
					buffer.Set( tl );
					buffer.Set( bl );
					buffer.Set( br );
					buffer.Set( br );
					buffer.Set( tr );
					buffer.Set( tl );
					tl = tr;
					bl = br;
				}
			}
			tl += d4;
			++br;
			for( uint32_t i = d4; i; --i )
			{	//	bottom cap
				++tr;
				buffer.Set( br );
				buffer.Set( tr );
				buffer.Set( tl );
				tl = tr;
			}
		}
		else
		{
			bl = 4;
			for( uint32_t i = 4; i; --i )
			{	//	top cap
				++br;
				buffer.Set( tl );
				buffer.Set( bl );
				buffer.Set( br );
				bl = br;
			}
			for( uint32_t i = 4; i < d4; i += 4 )
			{	//	top hemisphere fill
				tl += i;
				bl += ( i + 4 );
				uint32_t n = ( ( i >> 2 ) - 1 );
				for( uint32_t j = 4; j; --j )
				{
					++tr;
					++br;
					buffer.Set( tl );
					buffer.Set( bl );
					buffer.Set( tr );
					buffer.Set( tr );
					buffer.Set( bl );
					buffer.Set( br );
					tl = tr;
					bl = br;
					++br;
					buffer.Set( tl );
					buffer.Set( bl );
					buffer.Set( br );
					bl = br;
					for( uint32_t k = n; k; --k )
					{
						++tr;
						++br;
						buffer.Set( tl );
						buffer.Set( bl );
						buffer.Set( tr );
						buffer.Set( tr );
						buffer.Set( bl );
						buffer.Set( br );
						tl = tr;
						bl = br;
					}
				}
			}
			for( uint32_t i = ( d4 - 4 ); i; i -= 4 )
			{	//	bottom hemisphere fill
				bl += i;
				tl += ( i + 4 );
				uint32_t n = ( ( i >> 2 ) - 1 );
				for( uint32_t j = 4; j; --j )
				{
					++br;
					++tr;
					buffer.Set( bl );
					buffer.Set( br );
					buffer.Set( tl );
					buffer.Set( tl );
					buffer.Set( br );
					buffer.Set( tr );
					bl = br;
					tl = tr;
					++tr;
					buffer.Set( bl );
					buffer.Set( tr );
					buffer.Set( tl );
					tl = tr;
					for( uint32_t k = n; k; --k )
					{
						++br;
						++tr;
						buffer.Set( bl );
						buffer.Set( br );
						buffer.Set( tl );
						buffer.Set( tl );
						buffer.Set( br );
						buffer.Set( tr );
						bl = br;
						tl = tr;
					}
				}
			}
			tl += 4;
			++br;
			for( uint32_t i = 4; i; --i )
			{	//	bottom cap
				++tr;
				buffer.Set( br );
				buffer.Set( tr );
				buffer.Set( tl );
				tl = tr;
			}
		}
	}
	else
	{
		uint32_t a = 0;
		uint32_t b = 1;
		if( m_alt )
		{
			uint32_t dh = ( d2 - 1 );
			uint32_t dv = ( d2 - 2 );
			for( uint32_t i = dh; i; --i )
			{	//	horizontal lines
				a += d4;
				for( uint32_t j = d4; j; --j )
				{
					buffer.Set( a );
					buffer.Set( b );
					a = b;
					++b;
				}
			}
			a = 0;
			b = 1;
			for( uint32_t i = d4; i; --i )
			{	//	top cap
				buffer.Set( a );
				buffer.Set( b );
				++b;
			}
			for( uint32_t i = 1; i <= d4; ++i )
			{	//	vertical lines
				uint32_t k = i;
				for( uint32_t j = dv; j; --j )
				{
					buffer.Set( k );
					k += d4;
					buffer.Set( k );
				}
			}
			a = ( ( ( d4 * d4 ) >> 1 ) - d4 + 1 );
			b = ( a - d4 );
			for( uint32_t i = d4; i; --i )
			{	//	bottom cap
				buffer.Set( a );
				buffer.Set( b );
				++b;
			}
		}
		else
		{
			for( uint32_t i = 4; i <= d4; i += 4 )
			{	//	top hemisphere horizontal lines
				a += i;
				for( uint32_t j = i; j; --j )
				{
					buffer.Set( a );
					buffer.Set( b );
					a = b;
					++b;
				}
			}
			for( uint32_t i = ( d4 - 4 ); i; i -= 4 )
			{	//	bottom hemisphere horizontal lines
				a += i;
				for( uint32_t j = i; j; --j )
				{
					buffer.Set( a );
					buffer.Set( b );
					a = b;
					++b;
				}
			}
			a = 0;
			b = 1;
			for( uint32_t i = 4; i; --i )
			{	//	top cap
				buffer.Set( a );
				buffer.Set( b );
				++b;
			}
			a = 1;
			for( uint32_t i = 1; i < d; ++i )
			{	//	top hemisphere zig-zag lines
				uint32_t c = ( b + ( i << 2 ) + 3 );
				for( uint32_t j = 4; j; --j )
				{
					buffer.Set( c );
					buffer.Set( a );
					c = b;
					++b;
					for( uint32_t k = i; k; --k )
					{
						buffer.Set( a );
						buffer.Set( c );
						buffer.Set( a );
						buffer.Set( b );
						++a;
						c = b;
						++b;
					}
				}
			}
			for( uint32_t i = ( d - 1 ); i; --i )
			{	//	bottom hemisphere zig-zag lines
				uint32_t c = ( a + ( i << 2 ) + 3 );
				for( uint32_t j = 4; j; --j )
				{
					buffer.Set( c );
					buffer.Set( b );
					c = a;
					++a;
					for( uint32_t k = i; k; --k )
					{
						buffer.Set( b );
						buffer.Set( c );
						buffer.Set( b );
						buffer.Set( a );
						++b;
						c = a;
						++a;
					}
				}
			}
			for( uint32_t i = 4; i; --i )
			{	//	bottom cap
				buffer.Set( b );
				buffer.Set( a );
				++a;
			}
		}
	}
	return( GetIndices() );
}

uint32_t Sphere::GetVertices( void ) const
{
	uint32_t dd = ( m_density * m_density );
	return( ( ( m_alt ? ( dd + dd - m_density ) : dd ) << 2 ) + 2 );
}

uint32_t Sphere::GetVertices( IVB& buffer ) const
{
	uint32_t d4 = ( m_density << 2 );
	buffer.Set( 0, 1, 0, 0, 1, 0, m_colour );
	if( m_alt )
	{
		float ac = ( maths::consts::PI2 / d4 );
		float rv = 0;
		for( uint32_t i = m_density; i; --i )
		{	//	top hemisphere verts
			rv += ac;
			float sv = sinf( rv );
			float cv = cosf( rv );
			float ru = 0;
			for( uint32_t j = d4; j; --j )
			{
				float su = ( sinf( ru ) * sv );
				float cu = ( cosf( ru ) * sv );
				ru += ac;
				buffer.Set( su, cv, cu, su, cv, cu, m_colour );
			}
		}
		for( uint32_t i = ( m_density - 1 ); i; --i )
		{	//	bottom hemisphere verts
			rv += ac;
			float sv = sinf( rv );
			float cv = cosf( rv );
			float ru = 0;
			for( uint32_t j = d4; j; --j )
			{
				float su = ( sinf( ru ) * sv );
				float cu = ( cosf( ru ) * sv );
				ru += ac;
				buffer.Set( su, cv, cu, su, cv, cu, m_colour );
			}
		}
	}
	else
	{
		float av = ( maths::consts::PI2 / d4 );
		float rv = 0;
		for( uint32_t i = 4; i <= d4; i += 4 )
		{	//	top hemisphere verts
			rv += av;
			float sv = sinf( rv );
			float cv = cosf( rv );
			float au = ( maths::consts::PI2 / i );
			float ru = 0;
			for( uint32_t j = i; j; --j )
			{	//	top hemisphere row
				float su = ( sinf( ru ) * sv );
				float cu = ( cosf( ru ) * sv );
				ru += au;
				buffer.Set( su, cv, cu, su, cv, cu, m_colour );
			}
		}
		for( uint32_t i = ( d4 - 4 ); i; i -= 4 )
		{	//	bottom hemisphere verts
			rv += av;
			float sv = sinf( rv );
			float cv = cosf( rv );
			float au = ( maths::consts::PI2 / i );
			float ru = 0;
			for( uint32_t j = i; j; --j )
			{	//	bottom hemisphere row
				float su = ( sinf( ru ) * sv );
				float cu = ( cosf( ru ) * sv );
				ru += au;
				buffer.Set( su, cv, cu, su, cv, cu, m_colour );
			}
		}
	}
	buffer.Set( 0, -1, 0, 0, -1, 0, m_colour );
	return( GetVertices() );
}

void Sphere::SetFill( const bool fill )
{
	m_fill = fill;
}

void Sphere::Set( const uint32_t density, const uint32_t colour, const bool alt, const bool fill )
{
	m_fill = fill;
	m_density = ( ( density < 1 ) ? 1 : ( ( density > 32 ) ? 32 : density ) );
	m_colour = colour;
	m_alt = alt;
}

void Sphere::Clear( void )
{
	m_fill = true;
	m_density = 4;
	m_colour = MAKE_XRGB8( 0, 127, 127 );
	m_alt = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    capsule widget builder class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WIDGET_TYPE Capsule::GetType( void ) const
{
	return( m_fill ? ( m_alt ? WIDGET_FILLED_CAPSULE2 : WIDGET_FILLED_CAPSULE1 ) : ( m_alt ? WIDGET_VECTOR_CAPSULE2 : WIDGET_VECTOR_CAPSULE1 ) );
}

uint32_t Capsule::GetPrims( void ) const
{
	uint32_t d = m_density;
	uint32_t dd = ( d * d );
	if( m_fill )
	{
		return( ( m_alt ? ( dd + dd ) : ( dd + d ) ) << 3 );
	}
	else
	{
		return( ( m_alt ? ( ( dd << 2 ) + d ) : ( ( ( dd + d ) << 1 ) + dd ) ) << 2 );
	}
}

uint32_t Capsule::GetIndices( void ) const
{
	uint32_t d = m_density;
	uint32_t dd = ( d * d );
	if( m_fill )
	{
		return( ( m_alt ? ( dd + dd ) : ( dd + d ) ) * 24 );
	}
	else
	{
		return( ( m_alt ? ( ( dd << 2 ) + d ) : ( ( ( dd + d ) << 1 ) + dd ) ) << 3 );
	}
}

uint32_t Capsule::GetIndices( IIB& buffer ) const
{
	uint32_t d = m_density;
	uint32_t d2 = ( d + d );
	uint32_t d4 = ( d2 + d2 );
	uint32_t dd = ( d * d );
	if( m_fill )
	{
		uint32_t tl, tr, bl, br;
		tl = tr = bl = br = 0;
		if( m_alt )
		{
			bl = d4;
			for( uint32_t i = d4; i; --i )
			{	//	top cap
				++br;
				buffer.Set( tl );
				buffer.Set( bl );
				buffer.Set( br );
				bl = br;
			}
			for( uint32_t i = ( d2 - 1 ); i; --i )
			{
				tl += d4;
				bl += d4;
				for( uint32_t j = d4; j; --j )
				{	//	center rows
					++tr;
					++br;
					buffer.Set( tl );
					buffer.Set( bl );
					buffer.Set( br );
					buffer.Set( br );
					buffer.Set( tr );
					buffer.Set( tl );
					tl = tr;
					bl = br;
				}
			}
			tl += d4;
			++br;
			for( uint32_t i = d4; i; --i )
			{	//	bottom cap
				++tr;
				buffer.Set( br );
				buffer.Set( tr );
				buffer.Set( tl );
				tl = tr;
			}
		}
		else
		{
			bl = 4;
			for( uint32_t i = 4; i; --i )
			{	//	top cap
				++br;
				buffer.Set( tl );
				buffer.Set( bl );
				buffer.Set( br );
				bl = br;
			}
			for( uint32_t i = 4; i < d4; i += 4 )
			{	//	top hemisphere fill
				tl += i;
				bl += ( i + 4 );
				uint32_t n = ( ( i >> 2 ) - 1 );
				for( uint32_t j = 4; j; --j )
				{
					++tr;
					++br;
					buffer.Set( tl );
					buffer.Set( bl );
					buffer.Set( tr );
					buffer.Set( tr );
					buffer.Set( bl );
					buffer.Set( br );
					tl = tr;
					bl = br;
					++br;
					buffer.Set( tl );
					buffer.Set( bl );
					buffer.Set( br );
					bl = br;
					for( uint32_t k = n; k; --k )
					{
						++tr;
						++br;
						buffer.Set( tl );
						buffer.Set( bl );
						buffer.Set( tr );
						buffer.Set( tr );
						buffer.Set( bl );
						buffer.Set( br );
						tl = tr;
						bl = br;
					}
				}
			}
			tl += d4;
			bl += d4;
			for( uint32_t i = d4; i; --i )
			{	//	cylinder fill
				++tr;
				++br;
				buffer.Set( tl );
				buffer.Set( bl );
				buffer.Set( tr );
				buffer.Set( tr );
				buffer.Set( bl );
				buffer.Set( br );
				tl = tr;
				bl = br;
			}
			for( uint32_t i = ( d4 - 4 ); i; i -= 4 )
			{	//	bottom hemisphere fill
				bl += i;
				tl += ( i + 4 );
				uint32_t n = ( ( i >> 2 ) - 1 );
				for( uint32_t j = 4; j; --j )
				{
					++br;
					++tr;
					buffer.Set( bl );
					buffer.Set( br );
					buffer.Set( tl );
					buffer.Set( tl );
					buffer.Set( br );
					buffer.Set( tr );
					bl = br;
					tl = tr;
					++tr;
					buffer.Set( bl );
					buffer.Set( tr );
					buffer.Set( tl );
					tl = tr;
					for( uint32_t k = n; k; --k )
					{
						++br;
						++tr;
						buffer.Set( bl );
						buffer.Set( br );
						buffer.Set( tl );
						buffer.Set( tl );
						buffer.Set( br );
						buffer.Set( tr );
						bl = br;
						tl = tr;
					}
				}
			}
			tl += 4;
			++br;
			for( uint32_t i = 4; i; --i )
			{	//	bottom cap
				++tr;
				buffer.Set( br );
				buffer.Set( tr );
				buffer.Set( tl );
				tl = tr;
			}
		}
	}
	else
	{
		uint32_t a = 0;
		uint32_t b = 1;
		if( m_alt )
		{
			uint32_t dh = d2;
			uint32_t dv = ( d2 - 1 );
			for( uint32_t i = dh; i; --i )
			{	//	horizontal lines
				a += d4;
				for( uint32_t j = d4; j; --j )
				{
					buffer.Set( a );
					buffer.Set( b );
					a = b;
					++b;
				}
			}
			a = 0;
			b = 1;
			for( uint32_t i = d4; i; --i )
			{	//	top cap
				buffer.Set( a );
				buffer.Set( b );
				++b;
			}
			for( uint32_t i = 1; i <= d4; ++i )
			{	//	vertical lines
				uint32_t k = i;
				for( uint32_t j = dv; j; --j )
				{
					buffer.Set( k );
					k += d4;
					buffer.Set( k );
				}
			}
			a = ( ( dd << 3 ) + 1 );
			b = ( a - d4 );
			for( uint32_t i = d4; i; --i )
			{	//	bottom cap
				buffer.Set( a );
				buffer.Set( b );
				++b;
			}
		}
		else
		{
			for( uint32_t i = 4; i <= d4; i += 4 )
			{	//	top hemisphere horizontal lines
				a += i;
				for( uint32_t j = i; j; --j )
				{
					buffer.Set( a );
					buffer.Set( b );
					a = b;
					++b;
				}
			}
			for( uint32_t i = d4; i; i -= 4 )
			{	//	bottom hemisphere horizontal lines
				a += i;
				for( uint32_t j = i; j; --j )
				{
					buffer.Set( a );
					buffer.Set( b );
					a = b;
					++b;
				}
			}
			a = 0;
			b = 1;
			for( uint32_t i = 4; i; --i )
			{	//	top cap
				buffer.Set( a );
				buffer.Set( b );
				++b;
			}
			a = 1;
			for( uint32_t i = 1; i < d; ++i )
			{	//	top hemisphere zig-zag lines
				uint32_t c = ( b + ( i << 2 ) + 3 );
				for( uint32_t j = 4; j; --j )
				{
					buffer.Set( c );
					buffer.Set( a );
					c = b;
					++b;
					for( uint32_t k = i; k; --k )
					{
						buffer.Set( a );
						buffer.Set( c );
						buffer.Set( a );
						buffer.Set( b );
						++a;
						c = b;
						++b;
					}
				}
			}
			for( uint32_t i = d4; i; --i )
			{	//	cylinder vertical lines
				buffer.Set( a );
				buffer.Set( b );
				++a;
				++b;
			}
			for( uint32_t i = ( d - 1 ); i; --i )
			{	//	bottom hemisphere zig-zag lines
				uint32_t c = ( a + ( i << 2 ) + 3 );
				for( uint32_t j = 4; j; --j )
				{
					buffer.Set( c );
					buffer.Set( b );
					c = a;
					++a;
					for( uint32_t k = i; k; --k )
					{
						buffer.Set( b );
						buffer.Set( c );
						buffer.Set( b );
						buffer.Set( a );
						++b;
						c = a;
						++a;
					}
				}
			}
			for( uint32_t i = 4; i; --i )
			{	//	bottom cap
				buffer.Set( b );
				buffer.Set( a );
				++a;
			}
		}
	}
	return( GetIndices() );
}

uint32_t Capsule::GetVertices( void ) const
{
	uint32_t dd = ( m_density * m_density );
	return( ( ( m_alt ? ( dd + dd ) : ( dd + m_density ) ) << 2 ) + 2 );
}

uint32_t Capsule::GetVertices( IVB& buffer ) const
{
	uint32_t d4 = ( m_density << 2 );
	float hh = ( m_height * 0.5f );
	buffer.Set( 0, ( hh + 1 ), 0, 0, 1, 0, m_colour );
	if( m_alt )
	{
		float ac = ( maths::consts::PI2 / d4 );
		float rv = 0;
		for( uint32_t i = m_density; i; --i )
		{	//	top hemisphere verts
			rv += ac;
			float sv = sinf( rv );
			float cv = cosf( rv );
			float ru = 0;
			for( uint32_t j = d4; j; --j )
			{
				float su = ( sinf( ru ) * sv );
				float cu = ( cosf( ru ) * sv );
				ru += ac;
				buffer.Set( su, ( cv + hh ), cu, su, cv, cu, m_colour );
			}
		}
		for( uint32_t i = m_density; i; --i )
		{	//	bottom hemisphere verts
			float sv = sinf( rv );
			float cv = cosf( rv );
			rv += ac;
			float ru = 0;
			for( uint32_t j = d4; j; --j )
			{
				float su = ( sinf( ru ) * sv );
				float cu = ( cosf( ru ) * sv );
				ru += ac;
				buffer.Set( su, ( cv - hh ), cu, su, cv, cu, m_colour );
			}
		}
	}
	else
	{
		float av = ( maths::consts::PI2 / d4 );
		float rv = 0;
		for( uint32_t i = 4; i <= d4; i += 4 )
		{	//	top hemisphere verts
			float au = ( maths::consts::PI2 / i );
			rv += av;
			float sv = sinf( rv );
			float cv = cosf( rv );
			float ru = 0;
			for( uint32_t j = i; j; --j )
			{
				float su = ( sinf( ru ) * sv );
				float cu = ( cosf( ru ) * sv );
				ru += au;
				buffer.Set( su, ( cv + hh ), cu, su, cv, cu, m_colour );
			}
		}
		for( uint32_t i = d4; i; i -= 4 )
		{	//	bottom hemisphere verts
			float au = ( maths::consts::PI2 / i );
			float sv = sinf( rv );
			float cv = cosf( rv );
			rv += av;
			float ru = 0;
			for( uint32_t j = i; j; --j )
			{
				float su = ( sinf( ru ) * sv );
				float cu = ( cosf( ru ) * sv );
				ru += au;
				buffer.Set( su, ( cv - hh ), cu, su, cv, cu, m_colour );
			}
		}
	}
	buffer.Set( 0, -( hh + 1 ), 0, 0, -1, 0, m_colour );
	return( GetVertices() );
}

void Capsule::SetFill( const bool fill )
{
	m_fill = fill;
}

void Capsule::Set( const uint32_t density, const float height, const uint32_t colour, const bool alt, const bool fill )
{
	m_fill = fill;
	m_density = ( ( density < 1 ) ? 1 : ( ( density > 32 ) ? 32 : density ) );
	m_height = height;
	m_colour = colour;
	m_alt = alt;
}

void Capsule::Clear( void )
{
	m_fill = true;
	m_density = 4;
	m_height = 1;
	m_colour = MAKE_XRGB8( 63, 127, 191 );
	m_alt = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    cylinder widget builder class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WIDGET_TYPE Cylinder::GetType( void ) const
{
	return( m_fill ? ( m_alt ? WIDGET_FILLED_CYLINDER2 : WIDGET_FILLED_CYLINDER1 ) : ( m_alt ? WIDGET_VECTOR_CYLINDER2 : WIDGET_VECTOR_CYLINDER1 ) );
}

uint32_t Cylinder::GetPrims( void ) const
{
	return( m_fill ? ( ( m_density << 4 ) - 4 ) : ( m_density * 12 ) );
}

uint32_t Cylinder::GetIndices( void ) const
{
	return( m_fill ? ( ( m_density * 48 ) - 12 ) : ( m_density * 24 ) );
}

uint32_t Cylinder::GetIndices( IIB& buffer ) const
{
	uint32_t d4 = ( m_density << 2 );
	uint32_t d8 = ( d4 + d4 );
	if( m_fill )
	{
		uint32_t tl = ( d8 - 2 );
		uint32_t bl = ( d8 - 1 );
		uint32_t tr = ( 0 );
		uint32_t br = ( 1 );
		for( uint32_t i = d4; i; --i )
		{
			buffer.Set( tl );
			buffer.Set( bl );
			buffer.Set( br );
			buffer.Set( br );
			buffer.Set( tr );
			buffer.Set( tl );
			tl = tr;
			bl = br;
			tr += 2;
			br += 2;
		}
		tl = 0;
		bl = 1;
		if( m_alt )
		{
			tl += d8;
			bl += d8;
		}
		tr = ( tl + 2 );
		br = ( bl + 2 );
		for( uint32_t i = ( d4 - 2 ); i; --i )
		{
			tr += 2;
			buffer.Set( tl );
			buffer.Set( tr - 2 );
			buffer.Set( tr );
		}
		for( uint32_t i = ( d4 - 2 ); i; --i )
		{
			br += 2;
			buffer.Set( bl );
			buffer.Set( br );
			buffer.Set( br - 2 );
		}
	}
	else
	{
		for( uint32_t i = 0; i < d8; i += 2 )
		{
			buffer.Set( i );
			buffer.Set( i + 1 );
		}
		uint32_t l, r;
		l = ( d8 - 2 );
		r = 0;
		if( m_alt )
		{
			l += d8;
			r += d8;
		}
		for( uint32_t i = d4; i; --i )
		{
			buffer.Set( l );
			buffer.Set( r );
			l = r;
			r += 2;
		}
		l = ( d8 - 1 );
		r = 1;
		if( m_alt )
		{
			l += d8;
			r += d8;
		}
		for( uint32_t i = d4; i; --i )
		{
			buffer.Set( l );
			buffer.Set( r );
			l = r;
			r += 2;
		}
	}
	return( GetIndices() );
}

uint32_t Cylinder::GetVertices( void ) const
{
	return( m_alt ? ( m_density << 4 ) : ( m_density << 3 ) );
}

uint32_t Cylinder::GetVertices( IVB& buffer ) const
{
	uint32_t d4 = ( m_density << 2 );
	float hh = ( m_height * 0.5f );
	float au = ( maths::consts::PI2 / d4 );
	float ru = 0.0f;
	if( m_alt )
	{
		for( uint32_t i = d4; i; --i )
		{
			float su = sinf( ru );
			float cu = cosf( ru );
			ru += au;
			buffer.Set( su, hh, cu, su, 0, cu, m_colour );
			buffer.Set( su, -hh, cu, su, 0, cu, m_colour );
		}
		ru = 0.0f;
		for( uint32_t i = d4; i; --i )
		{
			float su = sinf( ru );
			float cu = cosf( ru );
			ru += au;
			buffer.Set( su, hh, cu, 0, 1, 0, m_colour );
			buffer.Set( su, -hh, cu, 0, -1, 0, m_colour );
		}
	}
	else
	{
		for( uint32_t i = d4; i; --i )
		{
			float su = sinf( ru );
			float cu = cosf( ru );
			float sv = ( su * maths::consts::RCPROOT2 );
			float cv = ( cu * maths::consts::RCPROOT2 );
			ru += au;
			buffer.Set( su, hh, cu, sv, maths::consts::RCPROOT2, cv, m_colour );
			buffer.Set( su, -hh, cu, sv, -maths::consts::RCPROOT2, cv, m_colour );
		}
	}
	return( GetVertices() );
}

void Cylinder::SetFill( const bool fill )
{
	m_fill = fill;
}

void Cylinder::Set( const uint32_t density, const float height, const uint32_t colour, const bool alt, const bool fill )
{
	m_fill = fill;
	m_density = ( ( density < 1 ) ? 1 : ( ( density > 32 ) ? 32 : density ) );
	m_height = height;
	m_colour = colour;
	m_alt = alt;
}

void Cylinder::Clear( void )
{
	m_fill = true;
	m_density = 4;
	m_height = 2;
	m_colour = MAKE_XRGB8( 191, 127, 63 );
	m_alt = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    cone widget builder class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WIDGET_TYPE Cone::GetType( void ) const
{
	return( m_fill ? ( m_alt ? WIDGET_FILLED_CONE2 : WIDGET_FILLED_CONE1 ) : ( m_alt ? WIDGET_VECTOR_CONE2 : WIDGET_VECTOR_CONE1 ) );
}

uint32_t Cone::GetPrims( void ) const
{
	return( m_fill ? ( ( m_density << 3 ) - 2 ) : ( m_density << 3 ) );
}

uint32_t Cone::GetIndices( void ) const
{
	return( m_fill ? ( ( m_density * 24 ) - 6 ) : ( m_density << 4 ) );
}

uint32_t Cone::GetIndices( IIB& buffer ) const
{
	uint32_t d4 = ( m_density << 2 );
	uint32_t d8 = ( d4 + d4 );
    uint32_t j = 1;
	if( m_fill )
	{
		if( m_alt )
        {
		    for( uint32_t i = d8; i; i -= 2 )
            {
			    buffer.Set( 0 );
			    buffer.Set( i - 1 );
			    buffer.Set( j );
                j = ( i - 1 );
            }
		    for( uint32_t i = ( d8 - 4 ); i; i -= 2 )
            {
			    buffer.Set( d8 );
			    buffer.Set( i + 2 );
			    buffer.Set( i );
            }
        }
        else
        {
		    for( uint32_t i = d4; i; --i )
            {
			    buffer.Set( 0 );
			    buffer.Set( i );
			    buffer.Set( j );
                j = i;
            }
		    for( uint32_t i = ( d4 - 2 ); i; --i )
            {
			    buffer.Set( d4 );
			    buffer.Set( i + 1 );
			    buffer.Set( i );
            }
        }
	}
	else
	{
		if( m_alt )
        {
            ++j;
		    for( uint32_t i = d8; i; i -= 2 )
            {
			    buffer.Set( 0 );
			    buffer.Set( i - 1 );
			    buffer.Set( i );
			    buffer.Set( j );
                j = i;
            }
        }
        else
        {
        	for( uint32_t i = d4; i; --i )
            {
			    buffer.Set( 0 );
			    buffer.Set( i );
			    buffer.Set( i );
			    buffer.Set( j );
                j = i;
            }
        }
	}
	return( GetIndices() );
}

uint32_t Cone::GetVertices( void ) const
{
	return( ( m_density << ( m_alt ? 3 : 2 ) ) + 1 );
}

uint32_t Cone::GetVertices( IVB& buffer ) const
{
	uint32_t d4 = ( m_density << 2 );
	float au = ( maths::consts::PI2 / d4 );
	float ru = 0.0f;
    float dv = sqrtf( 1.0f / ( ( m_height * m_height ) + 1.0f ) );
    float du = ( m_height * dv );
	if( m_alt )
	{
		buffer.Set( 0, m_height, 0, 0, dv, 0, m_colour );
		for( uint32_t i = d4; i; --i )
		{
			float su = sinf( ru );
			float cu = cosf( ru );
			ru += au;
			buffer.Set( su, 0, cu, ( su * du ), dv, ( cu * du ), m_colour );
			buffer.Set( su, 0, cu, 0, -1, 0, m_colour );
		}
	}
	else
	{
		buffer.Set( 0, m_height, 0, 0, dv, 0, m_colour );
		for( uint32_t i = d4; i; --i )
		{
			float su = sinf( ru );
			float cu = cosf( ru );
			ru += au;
			buffer.Set( su, 0, cu, ( su * du ), dv, ( cu * du ), m_colour );
		}
	}
	return( GetVertices() );
}

void Cone::SetFill( const bool fill )
{
	m_fill = fill;
}

void Cone::Set( const uint32_t density, const float height, const uint32_t colour, const bool alt, const bool fill )
{
	m_fill = fill;
	m_density = ( ( density < 1 ) ? 1 : ( ( density > 32 ) ? 32 : density ) );
	m_height = height;
	m_colour = colour;
	m_alt = alt;
}

void Cone::Clear( void )
{
	m_fill = true;
	m_density = 4;
	m_height = 2;
	m_colour = MAKE_XRGB8( 127, 63, 191 );
	m_alt = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end widgets namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace widgets

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
