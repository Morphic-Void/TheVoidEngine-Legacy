
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   draw.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Simple bitmap drawing.
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

#include "draw.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin file namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace draw
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Greyscale bitmap class functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmapGrey::set( uint8_t* const bits, const int width, const int height, const int pitch, const int stride )
{
	m_bits = bits;
	m_width = width;
	m_height = height;
	m_stride = ( stride ? stride : 1 );
	m_pitch = ( pitch ? pitch : ( m_stride * width ) );
}

void CBitmapGrey::clear( const uint8_t grey )
{
	if( m_bits )
	{
		uint8_t* bits = m_bits;
		int edge = ( m_pitch - ( m_stride * m_width ) );
		for( int v = m_height; v; --v )
		{
			for( int u = m_width; u; --u )
			{
				*bits = grey;
				bits += m_stride;
			}
			bits += edge;
		}
	}
}

void CBitmapGrey::plot( const uint8_t grey, const int x, const int y )
{
	if( m_bits )
	{
		if( ( static_cast< unsigned int >( x ) < static_cast< unsigned int >( m_width ) ) && ( static_cast< unsigned int >( y ) < static_cast< unsigned int >( m_height ) ) )
		{
			m_bits[ ( x * m_stride ) + ( y * m_pitch ) ] = grey;
		}
	}
}

void CBitmapGrey::draw( const uint8_t grey, const int x1, const int y1, const int x2, const int y2 )
{
	if( m_bits )
	{
		uint8_t* bits = m_bits;
		int major_pos1 = x1;
		int minor_pos1 = y1;
		int major_pos2 = x2;
		int minor_pos2 = y2;
		int major_step = m_stride;
		int minor_step = m_pitch;
		int major_clip = ( m_width - 1 );
		int minor_clip = ( m_height - 1 );
		if( major_pos1 > major_pos2 )
		{	//	flip major
			bits += ( major_clip * major_step );
			major_pos1 = ( major_clip - major_pos1 );
			major_pos2 = ( major_clip - major_pos2 );
			major_step = -major_step;
		}
		int major_span = ( major_pos2 - major_pos1 );
		if( minor_pos1 > minor_pos2 )
		{	//	flip minor
			bits += ( minor_clip * minor_step );
			minor_pos1 = ( minor_clip - minor_pos1 );
			minor_pos2 = ( minor_clip - minor_pos2 );
			minor_step = -minor_step;
		}
		int minor_span = ( minor_pos2 - minor_pos1 );
		if( minor_span > major_span )
		{	//	swap major and minor
			SWAP( minor_pos1, major_pos1 );
			SWAP( minor_pos2, major_pos2 );
			SWAP( minor_step, major_step );
			SWAP( minor_clip, major_clip );
			SWAP( minor_span, major_span );
		}
		if( ( major_pos1 <= major_clip ) && ( major_pos2 >= 0 ) && ( minor_pos1 <= minor_clip ) && ( minor_pos2 >= 0 ) )
		{	//	passes trivial clip test
			if( minor_span == 0 )
			{	//	straight
				if( static_cast< unsigned int >( minor_pos1 ) <= static_cast< unsigned int >( minor_clip ) )
				{	//	minor axis not clipped
					if( major_pos1 < 0 ) major_pos1 = 0;
					if( major_pos2 > major_clip ) major_pos2 = major_clip;
					bits += ( ( minor_pos1 * minor_step ) + ( major_pos1 * major_step ) );
					for( int span = ( major_pos2 - major_pos1 ); span >= 0; --span )
					{	//	line is not entirely clipped
						*bits = grey;
						bits += major_step;
					}
				}
			}
			else if( minor_span == major_span )
			{	//	diagonal
				if( minor_pos1 < 0 )
				{
					major_pos1 -= minor_pos1;
					minor_pos1 = 0;
				}
				if( major_pos1 < 0 )
				{
					minor_pos1 -= major_pos1;
					major_pos1 = 0;
				}
				if( minor_pos2 > minor_clip )
				{
					major_pos2 += ( minor_clip - minor_pos2 );
					minor_pos2 = minor_clip;
				}
				if( major_pos2 > major_clip )
				{
					minor_pos2 += ( major_clip - major_pos2 );
					major_pos2 = major_clip;
				}
				bits += ( ( minor_pos1 * minor_step ) + ( major_pos1 * major_step ) );
				major_step += minor_step;
				major_span = ( major_pos2 - major_pos1 );
				minor_span = ( minor_pos2 - minor_pos1 );
				for( int span = ( ( minor_span < major_span ) ? minor_span : major_span ); span >= 0; --span )
				{	//	line is not entirely clipped
					*bits = grey;
					bits += major_step;
				}
			}
			else
			{	//	bresenham
				++major_span;
				++minor_span;
				major_span <<= 1;
				int step = ( major_span - minor_span );
				minor_span <<= 1;
				if( minor_pos1 < 0 )
				{
					step -= ( major_span * minor_pos1 );
					int clip = ( ( step - major_span + minor_span - 1 ) / minor_span );
					step -= ( minor_span * clip );
					major_pos1 += clip;
					minor_pos1 = 0;
				}
				if( major_pos1 < 0 )
				{
					step += ( minor_span * major_pos1 );
					int clip = ( ( major_span - step ) / major_span );
					step += ( major_span * clip );
					minor_pos1 += clip;
					major_pos1 = 0;
				}
				if( minor_pos2 > minor_clip )
				{
					int clip = ( ( step + ( major_span * ( minor_clip - minor_pos1 ) ) ) / minor_span );
					major_pos2 = ( major_pos1 + clip );
					minor_pos2 = minor_clip;
				}
				if( major_pos2 > major_clip )
				{
					int clip = ( ( ( minor_span * ( major_clip - major_pos1 ) ) - step + major_span - 1 ) / major_span ); 
					minor_pos2 = ( minor_pos1 + clip );
					major_pos2 = major_clip;
				}
				bits += ( ( minor_pos1 * minor_step ) + ( major_pos1 * major_step ) );
				for( int span = ( major_pos2 - major_pos1 ); span >= 0; --span )
				{
					*bits = grey;
					step -= minor_span;
					if( step <= 0 )
					{
						step += major_span;
						bits += minor_step;
					}
					bits += major_step;
				}
			}
		}
	}
}

void CBitmapGrey::rect( const uint8_t grey, const int x, const int y, const int width, const int height )
{
	if( m_bits && width && height )
	{
		int u, v, w, h;
		if( width < 0 )
		{
			u = ( x + width );
			w = x;
		}
		else
		{
			u = x;
			w = ( x + width );
		}
		if( u < 0 ) u = 0;
		if( w > m_width ) w = m_width;
		w -= u;
		if( height < 0 )
		{
			v = ( y + height );
			h = ( y - 1 );
		}
		else
		{
			v = y;
			h = ( y + height - 1 );
		}
		if( v < 0 ) v = 0;
		if( h > m_height ) h = m_height;
		h -= v;
		if( ( w > 0 ) && ( h > 0 ) )
		{
			uint8_t* bits = &m_bits[ ( u * m_stride ) + ( v * m_pitch ) ];
			if( w == 1 )
			{
				for( v = h; v; --v )
				{
					*bits = grey;
					bits += m_pitch;
				}
			}
			else
			{
				for( u = w; u; --u )
				{
					*bits = grey;
					bits += m_stride;
				}
				if( h > 1 )
				{
					bits += ( m_pitch - ( m_stride * w ) );
					if( h > 2 )
					{
						int side = ( ( w - 1 ) * m_stride );
						for( v = ( h - 2 ); v; --v )
						{
							bits[ 0 ] = grey;
							bits[ side ] = grey;
							bits += m_pitch;
						}
					}
					for( u = w; u; --u )
					{
						*bits = grey;
						bits += m_stride;
					}
				}
			}
		}
	}
}

void CBitmapGrey::fill( const uint8_t grey, const int x, const int y, const int width, const int height )
{
	if( m_bits && width && height )
	{
		int u, v, w, h;
		if( width < 0 )
		{
			u = ( x + width );
			w = x;
		}
		else
		{
			u = x;
			w = ( x + width );
		}
		if( u < 0 ) u = 0;
		if( w > m_width ) w = m_width;
		w -= u;
		if( height < 0 )
		{
			v = ( y + height );
			h = ( y - 1 );
		}
		else
		{
			v = y;
			h = ( y + height - 1 );
		}
		if( v < 0 ) v = 0;
		if( h > m_height ) h = m_height;
		h -= v;
		if( ( w > 0 ) && ( h > 0 ) )
		{
			uint8_t* bits = &m_bits[ ( u * m_stride ) + ( v * m_pitch ) ];
			int edge = ( m_pitch - ( m_stride * w ) );
			for( v = h; v; --v )
			{
				for( u = w; u; --u )
				{
					*bits = grey;
					bits += m_stride;
				}
				bits += edge;
			}
		}
	}
}

bool CBitmapGrey::sample( const int x, const int y, uint8_t& grey ) const
{
	if( m_bits )
	{
		if( ( static_cast< unsigned int >( x ) < static_cast< unsigned int >( m_width ) ) && ( static_cast< unsigned int >( y ) < static_cast< unsigned int >( m_height ) ) )
		{
			grey = m_bits[ ( x * m_stride ) + ( y * m_pitch ) ];
			return( true );
		}
	}
	return( false );
}

void CBitmapGrey::zero( void )
{
	m_bits = 0;
	m_width = 0;
	m_height = 0;
	m_stride = 0;
	m_pitch = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Colour bitmap class functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmapARGB::set( ARGB8* const bits, const int width, const int height, const int pitch, const int stride )
{
	m_bits = bits;
	m_width = width;
	m_height = height;
	m_stride = ( stride ? stride : 1 );
	m_pitch = ( pitch ? pitch : ( m_stride * width ) );
}

void CBitmapARGB::clear( const ARGB8 argb )
{
	if( m_bits )
	{
		ARGB8* bits = m_bits;
		int edge = ( m_pitch - ( m_stride * m_width ) );
		for( int v = m_height; v; --v )
		{
			for( int u = m_width; u; --u )
			{
				*bits = argb;
				bits += m_stride;
			}
			bits += edge;
		}
	}
}

void CBitmapARGB::plot( const ARGB8 argb, const int x, const int y )
{
	if( m_bits )
	{
		if( ( static_cast< unsigned int >( x ) < static_cast< unsigned int >( m_width ) ) && ( static_cast< unsigned int >( y ) < static_cast< unsigned int >( m_height ) ) )
		{
			m_bits[ ( x * m_stride ) + ( y * m_pitch ) ] = argb;
		}
	}
}

void CBitmapARGB::draw( const ARGB8 argb, const int x1, const int y1, const int x2, const int y2 )
{
	if( m_bits )
	{
		ARGB8* bits = m_bits;
		int major_pos1 = x1;
		int minor_pos1 = y1;
		int major_pos2 = x2;
		int minor_pos2 = y2;
		int major_step = m_stride;
		int minor_step = m_pitch;
		int major_clip = ( m_width - 1 );
		int minor_clip = ( m_height - 1 );
		if( major_pos1 > major_pos2 )
		{	//	flip major
			bits += ( major_clip * major_step );
			major_pos1 = ( major_clip - major_pos1 );
			major_pos2 = ( major_clip - major_pos2 );
			major_step = -major_step;
		}
		int major_span = ( major_pos2 - major_pos1 );
		if( minor_pos1 > minor_pos2 )
		{	//	flip minor
			bits += ( minor_clip * minor_step );
			minor_pos1 = ( minor_clip - minor_pos1 );
			minor_pos2 = ( minor_clip - minor_pos2 );
			minor_step = -minor_step;
		}
		int minor_span = ( minor_pos2 - minor_pos1 );
		if( minor_span > major_span )
		{	//	swap major and minor
			SWAP( minor_pos1, major_pos1 );
			SWAP( minor_pos2, major_pos2 );
			SWAP( minor_step, major_step );
			SWAP( minor_clip, major_clip );
			SWAP( minor_span, major_span );
		}
		if( ( major_pos1 <= major_clip ) && ( major_pos2 >= 0 ) && ( minor_pos1 <= minor_clip ) && ( minor_pos2 >= 0 ) )
		{	//	passes trivial clip test
			if( minor_span == 0 )
			{	//	straight
				if( static_cast< unsigned int >( minor_pos1 ) <= static_cast< unsigned int >( minor_clip ) )
				{	//	minor axis not clipped
					if( major_pos1 < 0 ) major_pos1 = 0;
					if( major_pos2 > major_clip ) major_pos2 = major_clip;
					bits += ( ( minor_pos1 * minor_step ) + ( major_pos1 * major_step ) );
					for( int span = ( major_pos2 - major_pos1 ); span >= 0; --span )
					{	//	line is not entirely clipped
						*bits = argb;
						bits += major_step;
					}
				}
			}
			else if( minor_span == major_span )
			{	//	diagonal
				if( minor_pos1 < 0 )
				{
					major_pos1 -= minor_pos1;
					minor_pos1 = 0;
				}
				if( major_pos1 < 0 )
				{
					minor_pos1 -= major_pos1;
					major_pos1 = 0;
				}
				if( minor_pos2 > minor_clip )
				{
					major_pos2 += ( minor_clip - minor_pos2 );
					minor_pos2 = minor_clip;
				}
				if( major_pos2 > major_clip )
				{
					minor_pos2 += ( major_clip - major_pos2 );
					major_pos2 = major_clip;
				}
				bits += ( ( minor_pos1 * minor_step ) + ( major_pos1 * major_step ) );
				major_step += minor_step;
				major_span = ( major_pos2 - major_pos1 );
				minor_span = ( minor_pos2 - minor_pos1 );
				for( int span = ( ( minor_span < major_span ) ? minor_span : major_span ); span >= 0; --span )
				{	//	line is not entirely clipped
					*bits = argb;
					bits += major_step;
				}
			}
			else
			{	//	bresenham
				++major_span;
				++minor_span;
				major_span <<= 1;
				int step = ( major_span - minor_span );
				minor_span <<= 1;
				if( minor_pos1 < 0 )
				{
					step -= ( major_span * minor_pos1 );
					int clip = ( ( step - major_span + minor_span - 1 ) / minor_span );
					step -= ( minor_span * clip );
					major_pos1 += clip;
					minor_pos1 = 0;
				}
				if( major_pos1 < 0 )
				{
					step += ( minor_span * major_pos1 );
					int clip = ( ( major_span - step ) / major_span );
					step += ( major_span * clip );
					minor_pos1 += clip;
					major_pos1 = 0;
				}
				if( minor_pos2 > minor_clip )
				{
					int clip = ( ( step + ( major_span * ( minor_clip - minor_pos1 ) ) ) / minor_span );
					major_pos2 = ( major_pos1 + clip );
					minor_pos2 = minor_clip;
				}
				if( major_pos2 > major_clip )
				{
					int clip = ( ( ( minor_span * ( major_clip - major_pos1 ) ) - step + major_span - 1 ) / major_span ); 
					minor_pos2 = ( minor_pos1 + clip );
					major_pos2 = major_clip;
				}
				bits += ( ( minor_pos1 * minor_step ) + ( major_pos1 * major_step ) );
				for( int span = ( major_pos2 - major_pos1 ); span >= 0; --span )
				{
					*bits = argb;
					step -= minor_span;
					if( step <= 0 )
					{
						step += major_span;
						bits += minor_step;
					}
					bits += major_step;
				}
			}
		}
	}
}

void CBitmapARGB::rect( const ARGB8 argb, const int x, const int y, const int width, const int height )
{
	if( m_bits && width && height )
	{
		int u, v, w, h;
		if( width < 0 )
		{
			u = ( x + width );
			w = x;
		}
		else
		{
			u = x;
			w = ( x + width );
		}
		if( u < 0 ) u = 0;
		if( w > m_width ) w = m_width;
		w -= u;
		if( height < 0 )
		{
			v = ( y + height );
			h = ( y - 1 );
		}
		else
		{
			v = y;
			h = ( y + height - 1 );
		}
		if( v < 0 ) v = 0;
		if( h > m_height ) h = m_height;
		h -= v;
		if( ( w > 0 ) && ( h > 0 ) )
		{
			ARGB8* bits = &m_bits[ ( u * m_stride ) + ( v * m_pitch ) ];
			if( w == 1 )
			{
				for( v = h; v; --v )
				{
					*bits = argb;
					bits += m_pitch;
				}
			}
			else
			{
				for( u = w; u; --u )
				{
					*bits = argb;
					bits += m_stride;
				}
				if( h > 1 )
				{
					bits += ( m_pitch - ( m_stride * w ) );
					if( h > 2 )
					{
						int side = ( ( w - 1 ) * m_stride );
						for( v = ( h - 2 ); v; --v )
						{
							bits[ 0 ] = argb;
							bits[ side ] = argb;
							bits += m_pitch;
						}
					}
					for( u = w; u; --u )
					{
						*bits = argb;
						bits += m_stride;
					}
				}
			}
		}
	}
}

void CBitmapARGB::fill( const ARGB8 argb, const int x, const int y, const int width, const int height )
{
	if( m_bits && width && height )
	{
		int u, v, w, h;
		if( width < 0 )
		{
			u = ( x + width );
			w = x;
		}
		else
		{
			u = x;
			w = ( x + width );
		}
		if( u < 0 ) u = 0;
		if( w > m_width ) w = m_width;
		w -= u;
		if( height < 0 )
		{
			v = ( y + height );
			h = ( y - 1 );
		}
		else
		{
			v = y;
			h = ( y + height - 1 );
		}
		if( v < 0 ) v = 0;
		if( h > m_height ) h = m_height;
		h -= v;
		if( ( w > 0 ) && ( h > 0 ) )
		{
			ARGB8* bits = &m_bits[ ( u * m_stride ) + ( v * m_pitch ) ];
			int edge = ( m_pitch - ( m_stride * w ) );
			for( v = h; v; --v )
			{
				for( u = w; u; --u )
				{
					*bits = argb;
					bits += m_stride;
				}
				bits += edge;
			}
		}
	}
}

bool CBitmapARGB::sample( const int x, const int y, ARGB8& argb ) const
{
	if( m_bits )
	{
		if( ( static_cast< unsigned int >( x ) < static_cast< unsigned int >( m_width ) ) && ( static_cast< unsigned int >( y ) < static_cast< unsigned int >( m_height ) ) )
		{
			argb = m_bits[ ( x * m_stride ) + ( y * m_pitch ) ];
			return( true );
		}
	}
	return( false );
}

void CBitmapARGB::zero( void )
{
	m_bits = 0;
	m_width = 0;
	m_height = 0;
	m_stride = 0;
	m_pitch = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end draw namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace draw

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
