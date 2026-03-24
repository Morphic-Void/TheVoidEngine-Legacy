
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   colour_tools.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Colour manipulation functions.
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

#include "libs/maths/consts.h"
#include "libs/system/debug/asserts.h"
#include "colour_tools.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    colour float conversion functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARGB8 convertFloatToARGB8( const float r, const float g, const float b, const float a )
{
	static const float scale = ( ( 1.0f - maths::consts::FLOAT_EPSILON ) * 256.0f );
	ARGB8 colour = 0;
	if( b > 0 ) colour |= ( ( b > 1 ) ? 255 : static_cast< int >( floorf( b * scale ) ) );
	if( g > 0 ) colour |= ( ( ( g > 1 ) ? 255 : static_cast< int >( floorf( g * scale ) ) ) << 8 );
	if( r > 0 ) colour |= ( ( ( r > 1 ) ? 255 : static_cast< int >( floorf( r * scale ) ) ) << 16 );
	if( a > 0 ) colour |= ( ( ( a > 1 ) ? 255 : static_cast< int >( floorf( a * scale ) ) ) << 24 );
	return( colour );
}

void convertARGB8toFloat( const ARGB8 colour, float& r, float& g, float& b, float& a )
{
	b = ( ( colour & 255 ) * ( 1.0f / 255.0f ) );
	g = ( ( ( colour >> 8 ) & 255 ) * ( 1.0f / 255.0f ) );
	r = ( ( ( colour >> 16 ) & 255 ) * ( 1.0f / 255.0f ) );
	a = ( ( ( colour >> 24 ) & 255 ) * ( 1.0f / 255.0f ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    colour manipulation functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void decomposeRGB( const float r, const float g, const float b, float& min, float& mid, float& max, float& hue )
{
	if( r >= g )
	{	//	r >= g
		if( g >= b )
		{	//	r >= g >= b, r->rg, 0/6->1/6
			min = b;
			mid = g;
			max = r;
			hue = ( ( ( g - b ) / ( ( ( r - b ) * 6.0f ) + maths::consts::FLOAT_MIN_NORM ) ) + ( 0.0f / 3.0f ) );
		}
		else if( r >= b )
		{	//	r >= b >= g, rb->r, 5/6->6/6
			min = g;
			mid = b;
			max = r;
			hue = ( ( ( g - b ) / ( ( ( r - g ) * 6.0f ) + maths::consts::FLOAT_MIN_NORM ) ) + ( 3.0f / 3.0f ) );
		}
		else
		{	//	b >= r >= g, b->br, 4/6->5/6
			min = g;
			mid = r;
			max = b;
			hue = ( ( ( r - g ) / ( ( ( b - g ) * 6.0f ) + maths::consts::FLOAT_MIN_NORM ) ) + ( 2.0f / 3.0f ) );
		}
	}
	else
	{	//	g > r
		if( r >= b )
		{	//	g >= r >= b, rg->g, 1/6->2/6
			min = b;
			mid = r;
			max = g;
			hue = ( ( ( b - r ) / ( ( ( g - b ) * 6.0f ) + maths::consts::FLOAT_MIN_NORM ) ) + ( 1.0f / 3.0f ) );
		}
		else if( g >= b )
		{	//	g >= b >= r, gb->b, 3/6->4/6
			min = r;
			mid = b;
			max = g;
			hue = ( ( ( r - g ) / ( ( ( b - r ) * 6.0f ) + maths::consts::FLOAT_MIN_NORM ) ) + ( 2.0f / 3.0f ) );
		}
		else
		{	//	b >= g >= r, g->gb, 2/6->3/6
			min = r;
			mid = g;
			max = b;
			hue = ( ( ( b - r ) / ( ( ( g - r ) * 6.0f ) + maths::consts::FLOAT_MIN_NORM ) ) + ( 1.0f / 3.0f ) );
		}
	}
}

void convertHueToRGB( const float hue, float& r, float& g, float& b )
{
	if( ( hue < 0.0f ) || ( hue > 1.0f ) )
	{
		r = g = b = 1.0f;
	}
	else
	{
		float h = ( hue * 6.0f );
		if( h < 3.0f )
		{
			if( h < 1.0f )
			{
				r = 1.0f;
				g = ( 0.0f + h );
				b = 0.0f;
			}
			else if( h < 2.0f )
			{
				r = ( 2.0f - h );
				g = 1.0f;
				b = 0.0f;
			}
			else
			{
				r = 0.0f;
				g = 1.0f;
				b = ( h - 2.0f );
			}
		}
		else
		{
			if( h < 4.0f )
			{
				r = 0.0f;
				g = ( 4.0f - h );
				b = 1.0f;
			}
			else if( h < 5.0f )
			{
				r = ( h - 4.0f );
				g = 0.0f;
				b = 1.0f;
			}
			else
			{
				r = 1.0f;
				g = 0.0f;
				b = ( 6.0f - h );
			}
		}
	}
}

void convertRGBToHSL( const float r, const float g, const float b, float& h, float& s, float& l )
{
	float min, mid, max;
	decomposeRGB( r, g, b, min, mid, max, h );
	s = ( ( max - min ) / ( fabsf( 1.0f - fabsf( max + min - 1.0f ) ) + maths::consts::FLOAT_MIN_NORM ) );
	l = ( ( max + min ) * 0.5f );
}

void convertHSLToRGB( const float h, const float s, const float l, float& r, float& g, float& b )
{
	const float mul = ( s * ( 1.0f - fabsf( l + l - 1.0f ) ) );
	const float add = ( ( l + l - mul ) * 0.5f );
	convertHueToRGB( h, r, g, b );
	r = ( ( r * mul ) + add );
	g = ( ( g * mul ) + add );
	b = ( ( b * mul ) + add );
}

void convertRGBToHSV( const float r, const float g, const float b, float& h, float& s, float& v )
{
	float min, mid, max;
	decomposeRGB( r, g, b, min, mid, max, h );
	s = ( ( max - min ) / ( fabsf( max ) + maths::consts::FLOAT_MIN_NORM ) );
	v = max;
}

void convertHSVToRGB( const float h, const float s, const float v, float& r, float& g, float& b )
{
	const float mul = ( s * fabsf( v ) );
	const float add = ( v - mul );
	convertHueToRGB( h, r, g, b );
	r = ( ( r * mul ) + add );
	g = ( ( g * mul ) + add );
	b = ( ( b * mul ) + add );
}

void convertSLToSV( const float hsl_s, const float hsl_l, float& hsv_s, float& hsv_v )
{
	hsv_s = ( hsl_s * ( 1.0f - fabsf( hsl_l + hsl_l - 1.0f ) ) );
	hsv_v = ( ( hsl_l + hsl_l + hsv_s ) * 0.5f );
	hsv_s /= ( fabsf( hsv_v ) + maths::consts::FLOAT_MIN_NORM );
}

void convertSVToSL( const float hsv_s, const float hsv_v, float& hsl_s, float& hsl_l )
{
	hsl_s = ( hsv_s * fabsf( hsv_v ) );
	hsl_l = ( ( hsv_v + hsv_v - hsl_s ) * 0.5f );
	hsl_s /= ( fabsf( 1.0f - fabsf( hsl_l + hsl_l - hsl_s - 1.0f ) ) + maths::consts::FLOAT_MIN_NORM );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    light level modification
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! standard Reinhard tonemapping operator
float reinhardTonemap( const float c )
{	//	needs output gamma correction
	ASSERT( c >= 0.0f );
	return( ( c + c ) / ( c + 1.0f ) );
}

//! filmic tonemapping operator (John Hable's mapping)
float filmicTonemap( const float c )
{	//	assumes gamma 2.2 output
	float t = ( c - 0.004f );
	return( ( t > maths::consts::FLOAT_EPSILON ) ? ( ( ( 6.2f * t ) + 0.5f ) * t ) / ( ( ( ( 6.2f * t ) + 1.7f ) * t ) + 0.06f ) : 0.0f );
}

//!	Gamma space to linear space conversion
float gammaToLinear( const float c, const float gamma )
{
	ASSERT( ( c >= 0.0f ) && ( fabsf( gamma ) >= maths::consts::FLOAT_MIN_RCP ) );
	return( powf( c, gamma ) );
}

//! Linear space to gamma space conversion
float linearToGamma( const float c, const float gamma )
{
	ASSERT( ( c >= 0.0f ) && ( fabsf( gamma ) >= maths::consts::FLOAT_MIN_RCP ) );
	return( gammaToLinear( c, ( 1.0f / gamma ) ) );
}

//!	Gamma 2.0 space to linear space conversion
float gamma2ToLinear( const float c )
{
	return( c * c );
}

//! Linear space to gamma 2.0 space conversion
float linearToGamma2( const float c )
{
	ASSERT( c >= 0.0f );
	return( sqrtf( c ) );
}

//! Linear space luminance calculation
float linearLuminance( const float r, const float g, const float b )
{	//	same as sRGB
	return( ( r * 0.2126f ) + ( g * 0.7152f ) + ( b * 0.0722f ) );
}

//! Standard video luminance calculation
float videoLuminance( const float r, const float g, const float b )
{	//	same as NTSC luminance in gamma 2.2 space
	return( ( r * 0.299f ) + ( g * 0.587f ) + ( b * 0.114f ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

