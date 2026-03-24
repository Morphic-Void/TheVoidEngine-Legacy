
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vtext.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Embedded vector text handling.
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

#include "vtext.h"
#include "libs/parsers/text/ascii.h"
#include "libs/system/debug/asserts.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin vtext namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vtext
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    vector text handling
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Text::GetSpacing( unsigned int& h, unsigned int& v ) const
{
	unsigned int default_spacing = Font::GetSpacing( m_scale );
	h = ( ( m_spacing.x < 0 ) ? default_spacing : static_cast< unsigned int >( m_spacing.x ) );
	v = ( ( m_spacing.y < 0 ) ? default_spacing : static_cast< unsigned int >( m_spacing.y ) );
}

void Text::GetStepping( unsigned int& x, unsigned int& y ) const
{
	const Font& font( Font::GetFont( m_font ) );
	unsigned int default_spacing = font.GetSpacing( m_scale );
	x = ( ( ( m_spacing.x < 0 ) ? default_spacing : static_cast< unsigned int >( m_spacing.x ) ) + font.GetWidth( m_scale ) );
	y = ( ( ( m_spacing.y < 0 ) ? default_spacing : static_cast< unsigned int >( m_spacing.y ) ) + font.GetHeight( m_scale, m_descenders ) );
}

void Text::GetFontCharDimensions( unsigned int& w, unsigned int& h ) const
{
	Font::GetFont( m_font ).GetDimensions( w, h, m_scale, m_descenders );
}

void Text::ConvertCharsToClip( const unsigned int chars_wide, const unsigned int chars_high, unsigned int& clip_width, unsigned int& clip_height, const bool spaced ) const
{	//	the 'spaced' parameter indicates whether the text block should have character spacing around it
	const Font& font( Font::GetFont( m_font ) );
	unsigned int default_spacing = font.GetSpacing( m_scale );
	unsigned int space_x = ( ( m_spacing.x < 0 ) ? default_spacing : static_cast< unsigned int >( m_spacing.x ) );
	unsigned int space_y = ( ( m_spacing.y < 0 ) ? default_spacing : static_cast< unsigned int >( m_spacing.y ) );
	clip_width = ( chars_wide * ( space_x + font.GetWidth( m_scale ) ) );
	clip_height = ( chars_high * ( space_y + font.GetHeight( m_scale, m_descenders ) ) );
	if( spaced )
	{
		clip_width += space_x;
		clip_height += space_y;
	}
	else
	{
		clip_width -= space_x;
		clip_height -= space_y;
	}
}

void Text::ConvertClipToChars( const unsigned int clip_width, const unsigned int clip_height, unsigned int& chars_wide, unsigned int& chars_high, const bool spaced ) const
{	//	the 'spaced' parameter indicates whether the text block should have character spacing around it
	const Font& font( Font::GetFont( m_font ) );
	unsigned int default_spacing = font.GetSpacing( m_scale );
	unsigned int space_x = ( ( m_spacing.x < 0 ) ? default_spacing : static_cast< unsigned int >( m_spacing.x ) );
	unsigned int space_y = ( ( m_spacing.y < 0 ) ? default_spacing : static_cast< unsigned int >( m_spacing.y ) );
	chars_wide = clip_width;
	chars_high = clip_height;
	if( spaced )
	{
		chars_wide -= space_x;
		chars_high -= space_y;
	}
	else
	{
		chars_wide += space_x;
		chars_high += space_y;
	}
	chars_wide /= ( space_x + font.GetWidth( m_scale ) );
	chars_high /= ( space_y + font.GetHeight( m_scale, m_descenders ) );
}

void Text::Draw( ICharacterStream& stream, const char* const text, const char* const codes )
{
	if( text )
	{
		Draw( stream, VTEXT_RAW( static_cast< unsigned int >( strlen( reinterpret_cast< const char* const >( text ) ) ), const_cast< char* const >( text ), const_cast< char* const >( codes ) ) );
	}
}

void Text::Draw( ICharacterStream& stream, const VTEXT_LINES& lines )
{
	if( lines.lines )
	{
		SETUP setup;
		const Font& font( Setup( setup ) );
		int min_y = ( ( setup.rect.top < 0 ) ? 0 : setup.rect.top );
		int max_y = ( ( setup.rect.bottom >= static_cast< int >( lines.height ) ) ? ( lines.height - 1 ) : setup.rect.bottom );
		if( m_tabs )
		{	//	tab expansion required
			int draw_y = ( setup.origin.y + ( setup.stepping.y * min_y ) );
			for( int y = min_y; y <= max_y; ++y )
			{
				VTEXT_LINE& line = *lines.lines[ static_cast< unsigned int >( y + lines.start ) % lines.height ];
				int x = 0;
				int draw_x = setup.origin.x;
				for( unsigned int i = 0; i < line.width; ++i )
				{
					if( x > setup.rect.right )
					{
						break;
					}
					char character = line.text[ i ];
					if( character == 0x09 )
					{	//	tab
						int tab = ( m_tabs - ( x % m_tabs ) );
						x += tab;
						draw_x += ( setup.stepping.x * tab );
					}
					else
					{	//	character
						if( x >= setup.rect.left )
						{
							character = stream.Item( character, ( line.codes ? line.codes[ i ] : 0 ) );
							stream.Draw( font.GetGlyph( character, m_descenders ), draw_x, draw_y, m_scale );
						}
						++x;
						draw_x += setup.stepping.x;
					}
				}
				draw_y += setup.stepping.y;
				stream.Item( 0, -1 );	//	end of line
			}
		}
		else
		{	//	tab expansion not required
			int draw_y = ( setup.origin.y + ( setup.stepping.y * min_y ) );
			for( int y = min_y; y <= max_y; ++y )
			{
				VTEXT_LINE& line = *lines.lines[ static_cast< unsigned int >( y + lines.start ) % lines.height ];
				int min_x = ( ( setup.rect.left < 0 ) ? 0 : setup.rect.left );
				int max_x = ( ( setup.rect.right >= static_cast< int >( line.width ) ) ? ( line.width - 1 ) : setup.rect.right );
				int draw_x = ( setup.origin.x + ( setup.stepping.x * min_x ) );
				for( int x = min_x; x <= max_x; ++x )
				{
					char character = line.text[ x ];
					character = stream.Item( character, ( line.codes ? line.codes[ x ] : 0 ) );
					stream.Draw( font.GetGlyph( character, m_descenders ), draw_x, draw_y, m_scale );
					draw_x += setup.stepping.x;
				}
				draw_y += setup.stepping.y;
				stream.Item( 0, -1 );	//	end of line
			}
		}
	}
	stream.Item( -1, -1 );	//	close the character output
}

void Text::Draw( ICharacterStream& stream, const VTEXT_CMAP& cmap )
{
	if( cmap.text )
	{
		SETUP setup;
		const Font& font( Setup( setup ) );
		int min_x = ( ( setup.rect.left < 0 ) ? 0 : setup.rect.left );
		int max_x = ( ( setup.rect.right >= static_cast< int >( cmap.width ) ) ? ( cmap.width - 1 ) : setup.rect.right );
		int min_y = ( ( setup.rect.top < 0 ) ? 0 : setup.rect.top );
		int max_y = ( ( setup.rect.bottom >= static_cast< int >( cmap.height ) ) ? ( cmap.height - 1 ) : setup.rect.bottom );
		int draw_y = ( setup.origin.y + ( setup.stepping.y * min_y ) );
		for( int y = min_y; y <= max_y; ++y )
		{
			int i = ( ( ( static_cast< unsigned int >( y + cmap.start ) % cmap.height ) * cmap.width ) + min_x );
			int draw_x = ( setup.origin.x + ( setup.stepping.x * min_x ) );
			for( int x = min_x; x <= max_x; ++x )
			{
				char character = cmap.text[ i ];
				character = stream.Item( character, ( cmap.codes ? cmap.codes[ i ] : 0 ) );
				stream.Draw( font.GetGlyph( character, m_descenders ), draw_x, draw_y, m_scale );
				draw_x += setup.stepping.x;
				++i;
			}
			draw_y += setup.stepping.y;
			stream.Item( 0, -1 );	//	end of line
		}
	}
	stream.Item( -1, -1 );	//	close the character output
}

void Text::Draw( ICharacterStream& stream, const VTEXT_RAW& raw )
{
	if( raw.text )
	{
		SETUP setup;
		const Font& font( Setup( setup ) );
		unsigned int bytes, read;
		unsigned int i = 0;
		int y = -static_cast< int >( raw.start );
		int draw_y = setup.origin.y;
		while( ( i < raw.size ) && ( y <= setup.rect.bottom ) )
		{
			ascii::ParseLine( &raw.text[ i ], ( raw.size - i ), bytes, read );
			if( y >= setup.rect.top )
			{
				unsigned int j = i;
				int x = 0;
				int draw_x = setup.origin.x;
				for( unsigned int count = bytes; count && ( x <= setup.rect.right ); --count )
				{
					char character = raw.text[ j ];
					if( m_tabs && ( character == 0x09 ) )
					{	//	tab
						int tab = ( m_tabs - ( x % m_tabs ) );
						x += tab;
						draw_x += ( setup.stepping.x * tab );
					}
					else
					{	//	character
						if( x >= setup.rect.left )
						{
							character = stream.Item( character, ( raw.codes ? raw.codes[ j ] : 0 ) );
							stream.Draw( font.GetGlyph( character, m_descenders ), draw_x, draw_y, m_scale );
						}
						++x;
						draw_x += setup.stepping.x;
					}
					++j;
				}
			}
			++y;
			draw_y += setup.stepping.y;
			i += read;
			stream.Item( 0, -1 );	//	end of line
		}
	}
	stream.Item( -1, -1 );	//	close the character output
}

const Font& Text::Setup( SETUP& setup ) const
{
	const Font& font( Font::GetFont( m_font ) );
	int default_spacing = static_cast< int >( font.GetSpacing( m_scale ) );
	SPOINT spacing;
	spacing.x = ( ( m_spacing.x < 0 ) ? default_spacing : m_spacing.x );
	spacing.y = ( ( m_spacing.y < 0 ) ? default_spacing : m_spacing.y );
	setup.rect.left = 0;
	setup.rect.top = 0;
	setup.rect.right = 65535;
	setup.rect.bottom = 65535;
	font.GetOrigin( setup.origin.x, setup.origin.y, m_scale );
	setup.origin.x += m_origin.x;
	setup.origin.y += m_origin.y;
	setup.stepping.x = ( spacing.x + font.GetWidth( m_scale ) );
	setup.stepping.y = ( spacing.y + font.GetHeight( m_scale, m_descenders ) );
	if( m_clip )
	{
		setup.rect.left = ( m_rect.left - m_origin.x + spacing.x );
		setup.rect.top = ( m_rect.top - m_origin.y + spacing.y );
		setup.rect.right = ( m_rect.right - m_origin.x - 1 );
		setup.rect.bottom = ( m_rect.bottom - m_origin.y - 1 );
		if( setup.rect.left < 0 )
		{
			setup.rect.left += ( 1 - setup.stepping.x );
		}
		if( setup.rect.top < 0 )
		{
			setup.rect.top += ( 1 - setup.stepping.y );
		}
		if( setup.rect.right < 0 )
		{
			setup.rect.right  += ( 1 - setup.stepping.x );
		}
		if( setup.rect.bottom < 0 )
		{
			setup.rect.bottom += ( 1 - setup.stepping.y );
		}
		setup.rect.left /= setup.stepping.x;
		setup.rect.top /= setup.stepping.y;
		setup.rect.right /= setup.stepping.x;
		setup.rect.bottom /= setup.stepping.y;
	}
	return( font );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end vtext namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace vtext

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Vector text sample images
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef	VTEXT_IMAGES

#include "draw.h"
#include "tga.h"
#include "libs/system/io/file.h"

class WriteVectorTextImageFiles : private vtext::ICharacterStream, vtext::IDirectPointStream, vtext::IDirectLineStream
{
public:
	inline				WriteVectorTextImageFiles() { Run(); };
	inline				~WriteVectorTextImageFiles() {};
private:
	void				Run( void );
	virtual char		Item( const char character, const char code );
	virtual void		Draw( const vtext::Glyph& glyph, const int x, const int y, const unsigned int scale );
	virtual void		AddPoint( const int x, const int y );
	virtual void		AddLine( const int x1, const int y1, const int x2, const int y2 );
	draw::CBitmapARGB	m_bitmap;
	ARGB8				m_drawColour;
	ARGB8				m_plotColour;
};

WriteVectorTextImageFiles writeVectorTextImageFiles;

void WriteVectorTextImageFiles::Run( void )
{
	size_t size;
	char* text = reinterpret_cast< char* >( file::LoadFile( "input/text.txt", size, true ) );
	if( text )
	{
		const int width = 1024;
		const int height = 1024;
		ARGB8* memory = new ARGB8[ width * height ];
		if( memory )
		{
			m_bitmap.Set( memory, width, height );
			m_bitmap.Clear( MAKE_XRGB8( 0, 0, 0 ) );
			const unsigned int hspace = 2;
			const unsigned int vspace = 2;
			SRECT clip;
			clip.left = hspace;
			clip.top = vspace;
			clip.right = ( width - ( hspace << 1 ) );
			clip.bottom = ( height - ( vspace << 1 ) );
			vtext::Text vtext( vtext::VFONT_MEDIUM );
            vtext.SetScale( 2 );
			vtext.SetOrigin( hspace, vspace );
			vtext.SetSpacing( hspace, vspace );
			vtext.SetClipping( clip );
			vtext.Draw( static_cast< vtext::ICharacterStream& >( *this ), vtext::VTEXT_RAW( static_cast< unsigned int >( size ), text ) );
			file::SaveTGA_ARGB( "output/vtext.tga", memory, width, height );
			delete[] memory;
		}
		delete[] text;
	}
}

char WriteVectorTextImageFiles::Item( const char character, const char code )
{
    UNUSED( code );
	if( ( ( character >= 'a' ) && ( character <= 'z' ) ) || ( ( character >= 'A' ) && ( character <= 'Z' ) ) )
	{
		m_plotColour = MAKE_XRGB8( 191, 191, 191 );
		m_drawColour = MAKE_XRGB8( 191, 191, 191 );
	}
	else if( ( character >= '0' ) && ( character <= '9' ) )
	{
		m_plotColour = MAKE_XRGB8( 127, 127, 0 );
		m_drawColour = MAKE_XRGB8( 127, 127, 0 );
	}
	else
	{
		m_plotColour = MAKE_XRGB8( 0, 127, 127 );
		m_drawColour = MAKE_XRGB8( 0, 127, 127 );
	}
	return( character );
}

void WriteVectorTextImageFiles::Draw( const vtext::Glyph& glyph, const int x, const int y, const unsigned int scale )
{
	glyph.Draw( static_cast< vtext::IDirectLineStream& >( *this ), x, y, scale );
	glyph.Draw( static_cast< vtext::IDirectPointStream& >( *this ), x, y, scale );
}

void WriteVectorTextImageFiles::AddPoint( const int x, const int y )
{
	m_bitmap.Plot( m_plotColour, x, y );
}

void WriteVectorTextImageFiles::AddLine( const int x1, const int y1, const int x2, const int y2 )
{
	m_bitmap.Draw( m_drawColour, x1, y1, x2, y2 );
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



