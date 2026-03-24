
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   tga.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	TGA file support.
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

#include "tga.h"
#include "libs/system/io/file.h"
#include "libs/system/debug/asserts.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin file namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace file
{
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TGA bitmap file header structure and defines
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGA_ALPHA	0x0f
#define TGA_HFLIP	0x10
#define TGA_VFLIP	0x20

struct TGA_HEADER
{
	uint8_t		idsize;				//	size of the id field
	uint8_t		cmap;				//	0 if no cmap, 1 if cmap
	uint8_t		type;				//	uncompressed true-colour == 2, uncompressed grey-scale == 3
	uint8_t		reserved[ 9 ];		//	0
	uint8_t		width[ 2 ];			//	low byte == [ 0 ], high byte == [ 1 ]
	uint8_t		height[ 2 ];		//	low byte == [ 0 ], high byte == [ 1 ]
	uint8_t		bpp;				//	bits per pixel (8, 16, 24 or 32)
	uint8_t		alpha;				//	alpha bits (0 to 15) and image flipping
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TGA file loading function
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool loadTGA( const char* const name, void*& bits, uint16_t& width, uint16_t& height, bool& colour )
{
	ASSERT( name );
	if( name )
	{
		size_t size = 0;
		uint8_t* binary = reinterpret_cast< uint8_t* >( loadFile( name, size ) );
		if( binary )
		{
	        ASSERT( size > sizeof( TGA_HEADER ) );
            if( size > sizeof( TGA_HEADER ) )
            {
		        TGA_HEADER* header = reinterpret_cast< TGA_HEADER* >( binary );
			    if( header->cmap == 0 )
			    {	//	image does not have a colour map
				    uint8_t type = header->type;
				    uint8_t bpp = header->bpp;
				    if( ( ( bpp == 8 ) && ( ( type == 3 ) || ( type == 11 ) ) ) || ( ( ( bpp == 24 ) || ( bpp == 32 ) ) && ( ( type == 2 ) || ( type == 10 ) ) ) )
				    {	//	recognized image type
					    width = ( ( static_cast< uint16_t >( header->width[ 1 ] ) << 8 ) + header->width[ 0 ] );
					    height = ( ( static_cast< uint16_t >( header->height[ 1 ] ) << 8 ) + header->height[ 0 ] );
					    uint32_t texels = ( static_cast< uint32_t >( width ) * height );
					    bpp >>= 3;
					    uint8_t out = 0;
					    if( type & 1 )
					    {
						    colour = false;
						    bits = reinterpret_cast< void* >( new uint8_t[ texels ] );
						    out = 1;
					    }
					    else
					    {
						    colour = true;
						    bits = reinterpret_cast< void* >( new uint8_t[ texels << 2 ] );
						    out = 4;
						    if( bpp == 3 )
						    {
							    uint32_t* const fill = reinterpret_cast< uint32_t* const >( bits );
							    for( uint32_t i = 0; i < texels; ++i ) fill[ i ] = static_cast< uint32_t >( 0 - 1 );
						    }
					    }
					    ASSERT( bits );
					    if( bits )
					    {
					        uint8_t* max = &binary[ size ];
						    uint8_t* src = &binary[ sizeof( TGA_HEADER ) + header->idsize ];
						    uint8_t* trg = reinterpret_cast< uint8_t* >( bits );
						    if( type & 8 )
						    {	//	rle
							    src -= bpp;
							    for( uint16_t v = height; v; --v )
							    {
								    uint8_t rep = 0;
								    uint8_t inc = 0;
								    for( uint16_t u = width; u; --u )
								    {
									    if( rep )
									    {
										    --rep;
										    src += inc;
									    }
									    else
									    {
										    src += bpp;
                                            if( src < max )
                                            {
                                                rep = *src;
										        inc = ( bpp & ( ( rep >> 7 ) - 1 ) );
										        rep &= 127;
										        ++src;
                                            }
                                            if( ( src + ( inc * rep ) + bpp ) > max )
                                            {
                                                rep = 1;
                                                break;
                                            }
									    }
									    for( uint8_t c = 0; c < bpp; ++c ) trg[ c ] = src[ c ];
									    trg += out;
								    }
								    if( rep )
								    {	//	error
									    delete[] reinterpret_cast< uint8_t* >( bits );
									    bits = 0;
									    break;
								    }
							    }
						    }
						    else
                            {	//	raw
                                if( ( src + ( bpp * texels ) ) <= max )
						        {                                
							        for( uint32_t i = 0; i < texels; ++i )
							        {
								        for( uint8_t c = 0; c < bpp; ++c ) trg[ c ] = src[ c ];
								        src += bpp;
								        trg += out;
							        }
						        }
                                else
                                {
                                    delete[] reinterpret_cast< uint8_t* >( bits );
                                    bits = 0;
                                }
                            }
						    if( bits )
						    {
							    if( ( header->alpha & TGA_HFLIP ) == TGA_HFLIP )
							    {	//	flip horizontally
								    uint8_t* a = reinterpret_cast< uint8_t* >( bits );
								    uint8_t* b = &a[ ( width - 1 ) * out ];
								    for( uint16_t u = ( width >> 1 ); u; --u )
								    {
									    for( uint16_t v = height; v; --v )
									    {
										    for( uint8_t c = 0; c < out; ++c )
										    {
											    uint8_t t = a[ c ];
											    a[ c ] = b[ c ];
											    b[ c ] = t;
										    }
										    a += width;
										    b += width;
									    }
									    a -= ( texels - out );
									    b -= ( texels + out );
								    }
							    }
							    if( ( header->alpha & TGA_VFLIP ) != TGA_VFLIP )
							    {	//	flip vertically
								    uint8_t* a = reinterpret_cast< uint8_t* >( bits );
								    uint8_t* b = &a[ ( texels - width ) * out ];
								    for( uint16_t v = ( height >> 1 ); v; --v )
								    {
									    for( uint16_t u = width; u; --u )
									    {
										    for( uint8_t c = 0; c < out; ++c )
										    {
											    uint8_t t = a[ c ];
											    a[ c ] = b[ c ];
											    b[ c ] = t;
										    }
										    a += out;
										    b += out;
									    }
									    b -= ( ( width << 1 ) * out );
								    }
							    }
							    delete[] header;
							    return( true );
						    }
					    }
				    }
			    }
            }
		    delete[] binary;
		}
	}
	bits = 0;
	width = 0;
	height = 0;
	colour = false;
	return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TGA file saving functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool saveTGA_Grey( const char* const name, const uint8_t* const bits, const uint16_t width, const uint16_t height, const int16_t pitch, const int16_t stride )
{
	ASSERT( name && bits && width && height );
	if( name && bits && width && height )
	{
		CTGA_Grey image( bits, width, height, pitch, stride );
		return( saveTGA_Grey( name, image ) );
	}
	return( false );
}

bool saveTGA_ARGB( const char* const name, const ARGB8* const bits, const uint16_t width, const uint16_t height, const int16_t pitch )
{
	ASSERT( name && bits && width && height );
	if( name && bits && width && height )
	{
		CTGA_ARGB image( bits, width, height, pitch );
		return( saveTGA_ARGB( name, image ) );
	}
	return( false );
}

bool saveTGA_Grey( const char* const name, ITGA_Grey& image )
{
	bool success = false;
	ASSERT( name );
	if( name )
	{
		void* data = 0;
		unsigned int size = 0;
		if( createTGA_Grey( data, size, image ) )
		{
			success = saveFile( name, size, data );
			delete[] reinterpret_cast< uint8_t* >( data );
		}
	}
	return( success );
}

bool saveTGA_ARGB( const char* const name, ITGA_ARGB& image )
{
	bool success = false;
	ASSERT( name );
	if( name )
	{
		void* data = 0;
		unsigned int size = 0;
		if( createTGA_ARGB( data, size, image ) )
		{
			success = saveFile( name, size, data );
			delete[] reinterpret_cast< uint8_t* >( data );
		}
	}
	return( success );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    TGA file memory image creation functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool createTGA_Grey( void*& data, unsigned int& size, const uint8_t* const bits, const uint16_t width, const uint16_t height, const int16_t pitch, const int16_t stride )
{
	ASSERT( bits && width && height );
	if( bits && width && height )
	{
		CTGA_Grey image( bits, width, height, pitch, stride );
		return( createTGA_Grey( data, size, image ) );
	}
	size = 0;
	return( false );
}

bool createTGA_ARGB( void*& data, unsigned int& size, const ARGB8* const bits, const uint16_t width, const uint16_t height, const int16_t pitch )
{
	ASSERT( bits && width && height );
	if( bits && width && height )
	{
		CTGA_ARGB image( bits, width, height, pitch );
		return( createTGA_ARGB( data, size, image ) );
	}
	size = 0;
	return( false );
}

bool createTGA_Grey( void*& data, unsigned int& size, ITGA_Grey& image )
{
	uint16_t width = image.getWidth();
	uint16_t height = image.getHeight();
	size = ( sizeof( TGA_HEADER ) + ( sizeof( uint8_t ) * width * height ) );
	data = reinterpret_cast< void* >( new uint8_t[ size ] );
	ASSERT( data );
	if( data )
	{
		TGA_HEADER& header = *reinterpret_cast< TGA_HEADER* >( data );
		header.idsize = 0;
		header.cmap = 0;
		header.type = 3;	//	code for uncompressed grey-scale image data
		header.reserved[ 0 ] = header.reserved[ 1 ] = header.reserved[ 2 ] = 0;
		header.reserved[ 3 ] = header.reserved[ 4 ] = header.reserved[ 5 ] = 0;
		header.reserved[ 6 ] = header.reserved[ 7 ] = header.reserved[ 8 ] = 0;
		header.width[ 0 ] = static_cast< uint8_t >( width );
		header.width[ 1 ] = static_cast< uint8_t >( width >> 8 );
		header.height[ 0 ] = static_cast< uint8_t >( height );
		header.height[ 1 ] = static_cast< uint8_t >( height >> 8 );
		header.bpp = 8;
		header.alpha = TGA_VFLIP;
		uint8_t* bitmap = &reinterpret_cast< uint8_t* >( data )[ sizeof( TGA_HEADER ) ];
		for( uint16_t y = 0; y < height; ++y )
		{
			for( uint16_t x = 0; x < width; ++x )
			{
				*bitmap = image.getTexel( x, y );
				++bitmap;
			}
		}
		return( true );
	}
	size = 0;
	return( false );
}

bool createTGA_ARGB( void*& data, unsigned int& size, ITGA_ARGB& image )
{
	uint16_t width = image.getWidth();
	uint16_t height = image.getHeight();
	size = ( sizeof( TGA_HEADER ) + ( sizeof( uint32_t ) * width * height ) );
	data = reinterpret_cast< void* >( new uint8_t[ size ] );
	ASSERT( data );
	if( data )
	{
		TGA_HEADER& header = *reinterpret_cast< TGA_HEADER* >( data );
		header.idsize = 0;
		header.cmap = 0;
		header.type = 2;	//	code for uncompressed true-colour image data
		header.reserved[ 0 ] = header.reserved[ 1 ] = header.reserved[ 2 ] = 0;
		header.reserved[ 3 ] = header.reserved[ 4 ] = header.reserved[ 5 ] = 0;
		header.reserved[ 6 ] = header.reserved[ 7 ] = header.reserved[ 8 ] = 0;
		header.width[ 0 ] = static_cast< uint8_t >( width );
		header.width[ 1 ] = static_cast< uint8_t >( width >> 8 );
		header.height[ 0 ] = static_cast< uint8_t >( height );
		header.height[ 1 ] = static_cast< uint8_t >( height >> 8 );
		header.bpp = 32;
		header.alpha = ( 8 | TGA_VFLIP );
		ARGB8* bitmap = reinterpret_cast< ARGB8* >( &reinterpret_cast< uint8_t* >( data )[ sizeof( TGA_HEADER ) ] );
		for( uint16_t y = 0; y < height; ++y )
		{
			for( uint16_t x = 0; x < width; ++x )
			{
				*bitmap = image.getTexel( x, y );
				++bitmap;
			}
		}
		return( true );
	}
	size = 0;
	return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Default image handler classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTGA_Grey::set( const uint8_t* const bits, const uint16_t width, const uint16_t height, const int16_t pitch, const int16_t stride )
{
	ASSERT( bits && width && height );
	m_bits = bits;
	m_width = width;
	m_height = height;
	m_pitch = ( pitch ? pitch : static_cast< int16_t >( width ) );
	m_stride = ( stride ? stride : 1 );
}

uint16_t CTGA_Grey::getWidth( void ) const
{
	return( m_width );
}

uint16_t CTGA_Grey::getHeight( void ) const
{
	return( m_height );
}

uint8_t CTGA_Grey::getTexel( const uint16_t x, const uint16_t y ) const
{
	ASSERT( m_bits );
	if( m_bits && ( x < m_width ) && ( y < m_height ) )
	{
		return( m_bits[ ( static_cast< int32_t >( x ) * m_stride ) + ( static_cast< int32_t >( y ) * m_pitch ) ] );
	}
	return( 0 );
}

void CTGA_Grey::clear( void )
{
	m_bits = 0;
	m_width = 0;
	m_height = 0;
	m_pitch = 0;
	m_stride = 0;
}

void CTGA_ARGB::set( const ARGB8* const bits, const uint16_t width, const uint16_t height, const int16_t pitch )
{
	ASSERT( bits && width && height );
	m_bits = bits;
	m_width = width;
	m_height = height;
	m_pitch = ( pitch ? pitch : static_cast< int16_t >( width ) );
}

uint16_t CTGA_ARGB::getWidth( void ) const
{
	return( m_width );
}

uint16_t CTGA_ARGB::getHeight( void ) const
{
	return( m_height );
}

ARGB8 CTGA_ARGB::getTexel( const uint16_t x, const uint16_t y ) const
{
	ASSERT( m_bits );
	if( m_bits && ( x < m_width ) && ( y < m_height ) )
	{
		return( m_bits[ static_cast< int32_t >( x ) + ( static_cast< int32_t >( y ) * m_pitch ) ] );
	}
	return( 0 );
}

void CTGA_ARGB::clear( void )
{
	m_bits = 0;
	m_width = 0;
	m_height = 0;
	m_pitch = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end file namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace file

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
