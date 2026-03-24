
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   noise.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Assorted noise generation functions.
////
////    Comments:
////
////    	The Perlin and Simplex noise generators implemented in this file are
////    	taken from Stefan Gustavson's paper "Simplex noise demystified".
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

#include "noise.h"
#include "consts.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin maths namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace maths
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    consts
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace consts
{
    const float         Gradients2D[ 8 ][ 2 ] = {
                            {  0,  1 }, {  RCPROOT2,  RCPROOT2 },
                            {  1,  0 }, {  RCPROOT2, -RCPROOT2 },
                            {  0, -1 }, { -RCPROOT2, -RCPROOT2 },
                            { -1,  0 }, { -RCPROOT2,  RCPROOT2 } };
    const char          Gradients3D[ 12 ][ 4 ] = {
                            {  1,  1,  0,  0 }, { -1,  1,  0,  0 }, {  1, -1,  0,  0 }, { -1, -1,  0,  0 },
                            {  1,  0,  1,  0 }, { -1,  0,  1,  0 }, {  1,  0, -1,  0 }, { -1,  0, -1,  0 },
                            {  0,  1,  1,  0 }, {  0, -1,  1,  0 }, {  0,  1, -1,  0 }, {  0, -1, -1,  0 } };
    const char          Gradients4D[ 32 ][ 4 ] = {
                            {  0,  1,  1,  1 }, {  0,  1,  1, -1 }, {  0,  1, -1,  1 }, {  0,  1, -1, -1 },
                            {  0, -1,  1,  1 }, {  0, -1,  1, -1 }, {  0, -1, -1,  1 }, {  0, -1, -1, -1 },
                            {  1,  0,  1,  1 }, {  1,  0,  1, -1 }, {  1,  0, -1,  1 }, {  1,  0, -1, -1 },
                            { -1,  0,  1,  1 }, { -1,  0,  1, -1 }, { -1,  0, -1,  1 }, { -1,  0, -1, -1 },
                            {  1,  1,  0,  1 }, {  1,  1,  0, -1 }, {  1, -1,  0,  1 }, {  1, -1,  0, -1 },
                            { -1,  1,  0,  1 }, { -1,  1,  0, -1 }, { -1, -1,  0,  1 }, { -1, -1,  0, -1 },
                            {  1,  1,  1,  0 }, {  1,  1, -1,  0 }, {  1, -1,  1,  0 }, {  1, -1, -1,  0 },
                            { -1,  1,  1,  0 }, { -1,  1, -1,  0 }, { -1, -1,  1,  0 }, { -1, -1, -1,  0 } };
    const uchar         Permute256[ 512 ] = {
                            191, 162, 154,  63,  61, 196, 231,  32, 187,  91, 145,   5, 132, 192, 217, 133,
                             37, 251, 173, 193, 226, 210, 180,  44, 185, 138, 121,  40,  78,  27,  88, 163,
                            214,  97, 155, 139, 129, 143,  76, 137, 106, 170,  13, 127,  23,  58,  74,  67,
                            202,  95, 230, 135, 253,  18, 208,  22, 250, 184,  45, 219, 152,   4, 227,  10,
                            109, 189, 244, 238,  73, 107, 105, 161, 175, 117, 245, 130, 241, 218,  89, 188,
                             92, 232, 228, 119,   3, 174,  15, 200,  96,  28, 164,   9, 136,  75, 125,  49,
                             30,  98,  33, 113, 249,  47,  64, 225, 171, 146,  29,  25, 114, 102, 101,  38,
                            110,  57, 108, 204,  36,  68, 181, 166, 242, 150, 131, 167, 100, 142, 198,  43,
                             71, 116, 118,   1,  12,  86, 103,  69,  80,  39,  51, 235, 169, 153,  94, 216,
                            236, 168,  70, 148, 147, 211,   7,   2, 203, 141, 172,  34, 183, 213,  90, 207,
                            248, 128, 122, 190, 158,   8,  83, 178, 221, 255,  41, 104, 209,  53, 243, 197,
                            240,   0, 123, 234,  66, 144, 194,  59,  99,  24, 233, 165,  79,  65,  31, 254,
                             42,  19,  17, 215,  11,  72, 160, 205,  87, 222,  48, 111, 115, 199,  50,  77,
                             54, 151,  21, 149, 239, 157,  26, 195,  16,  14, 159, 156, 229,  84, 201,  52,
                            252, 134, 237,  82,  62, 220, 206,   6, 212, 186, 179,  20,  93,  35, 112,  81,
                            176,  60, 120, 247,  55,  46, 177,  85, 126, 124,  56, 182, 224, 223, 246, 140, //  wrap
                            191, 162, 154,  63,  61, 196, 231,  32, 187,  91, 145,   5, 132, 192, 217, 133,
                             37, 251, 173, 193, 226, 210, 180,  44, 185, 138, 121,  40,  78,  27,  88, 163,
                            214,  97, 155, 139, 129, 143,  76, 137, 106, 170,  13, 127,  23,  58,  74,  67,
                            202,  95, 230, 135, 253,  18, 208,  22, 250, 184,  45, 219, 152,   4, 227,  10,
                            109, 189, 244, 238,  73, 107, 105, 161, 175, 117, 245, 130, 241, 218,  89, 188,
                             92, 232, 228, 119,   3, 174,  15, 200,  96,  28, 164,   9, 136,  75, 125,  49,
                             30,  98,  33, 113, 249,  47,  64, 225, 171, 146,  29,  25, 114, 102, 101,  38,
                            110,  57, 108, 204,  36,  68, 181, 166, 242, 150, 131, 167, 100, 142, 198,  43,
                             71, 116, 118,   1,  12,  86, 103,  69,  80,  39,  51, 235, 169, 153,  94, 216,
                            236, 168,  70, 148, 147, 211,   7,   2, 203, 141, 172,  34, 183, 213,  90, 207,
                            248, 128, 122, 190, 158,   8,  83, 178, 221, 255,  41, 104, 209,  53, 243, 197,
                            240,   0, 123, 234,  66, 144, 194,  59,  99,  24, 233, 165,  79,  65,  31, 254,
                             42,  19,  17, 215,  11,  72, 160, 205,  87, 222,  48, 111, 115, 199,  50,  77,
                             54, 151,  21, 149, 239, 157,  26, 195,  16,  14, 159, 156, 229,  84, 201,  52,
                            252, 134, 237,  82,  62, 220, 206,   6, 212, 186, 179,  20,  93,  35, 112,  81,
                            176,  60, 120, 247,  55,  46, 177,  85, 126, 124,  56, 182, 224, 223, 246, 140 };
    const uchar         Permute128[ 256 ] = {
                            111,  90,  75,  40,  17, 122,  45,   9,  61,   7,  28, 117,  87,  86, 107, 127,
                             34,   4,  21,  15,  98,  78,  94,  68,  42, 101, 115,  56,  92,  84,  72,  37,
                             71,  65,  22,  88,  62,  31,   0, 118, 113,  85, 116, 124,  29,   6,  93,  95,
                             18,  74,  55,  89, 114,  96, 105,  50,  63, 120,  76,  35,  24,  81,  36, 102,
                             53,  33, 110,  16, 121,  91,  79,  44,  52, 106,  49,  27, 126, 100,  39, 119,
                              2, 123, 112,  64,  32,  41,   5,  12,  59,  51, 125,  73,  10,  99,  67,  47,
                             83,  20, 109,  26,  77,  66,  80,  25, 108,  54,  38,  14, 104,  97, 103,  69,
                             82,   3,  48,  46,  60,  11,  30,  70,  57,  23,  13,   8,  43,   1,  58,  19, //  wrap
                            111,  90,  75,  40,  17, 122,  45,   9,  61,   7,  28, 117,  87,  86, 107, 127,
                             34,   4,  21,  15,  98,  78,  94,  68,  42, 101, 115,  56,  92,  84,  72,  37,
                             71,  65,  22,  88,  62,  31,   0, 118, 113,  85, 116, 124,  29,   6,  93,  95,
                             18,  74,  55,  89, 114,  96, 105,  50,  63, 120,  76,  35,  24,  81,  36, 102,
                             53,  33, 110,  16, 121,  91,  79,  44,  52, 106,  49,  27, 126, 100,  39, 119,
                              2, 123, 112,  64,  32,  41,   5,  12,  59,  51, 125,  73,  10,  99,  67,  47,
                             83,  20, 109,  26,  77,  66,  80,  25, 108,  54,  38,  14, 104,  97, 103,  69,
                             82,   3,  48,  46,  60,  11,  30,  70,  57,  23,  13,   8,  43,   1,  58,  19 };
    const uchar         Permute64[ 128 ] = {
                             36,  61,  39,  15,  29,   1,  27,  63,  35,  31,  28,  38,  34,  56,  18,  41,
                             40,  23,  12,  52,  47,  22,  24,  17,  57,  14,  13,  44,   0,  30,  50,  54,
                             58,  16,  25,   8,  21,  60,   4,   2,  59,   3,   5,  42,   6,  33,   7,  20,
                             43,  10,  62,  48,  55,  45,  37,  19,  26,  49,  32,  53,   9,  51,  11,  46, //  wrap
                             36,  61,  39,  15,  29,   1,  27,  63,  35,  31,  28,  38,  34,  56,  18,  41,
                             40,  23,  12,  52,  47,  22,  24,  17,  57,  14,  13,  44,   0,  30,  50,  54,
                             58,  16,  25,   8,  21,  60,   4,   2,  59,   3,   5,  42,   6,  33,   7,  20,
                             43,  10,  62,  48,  55,  45,  37,  19,  26,  49,  32,  53,   9,  51,  11,  46 };
    const uchar         Permute32[ 64 ] = {
                             25,  16,  22,   9,   8,  26,  30,  31,  17,  27,  24,   5,  23,   1,   4,  28,
                             13,  29,  19,  18,  15,   6,   2,  12,  10,   0,  11,   3,  20,  14,  21,   7, //  wrap
                             25,  16,  22,   9,   8,  26,  30,  31,  17,  27,  24,   5,  23,   1,   4,  28,
                             13,  29,  19,  18,  15,   6,   2,  12,  10,   0,  11,   3,  20,  14,  21,   7 };
    const uchar         Permute16[ 32 ] = {
                             14,   2,   1,   4,   3,   6,  15,  13,   9,   7,  12,   8,  10,  11,   0,   5, //  wrap
                             14,   2,   1,   4,   3,   6,  15,  13,   9,   7,  12,   8,  10,  11,   0,   5 };
    const uchar         Permute8[ 16 ] = {
                              4,   3,   0,   6,   2,   7,   5,   1, //  wrap
                              4,   3,   0,   6,   2,   7,   5,   1 };
    const uchar         Permute[ 512 ] = {
                            151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
                            140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
                            247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
                             57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
                             74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
                             60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
                             65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
                            200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
                             52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
                            207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
                            119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
                            129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
                            218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
                             81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
                            184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
                            222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180, //  wrap
                            151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
                            140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
                            247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
                             57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
                             74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
                             60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
                             65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
                            200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
                             52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
                            207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
                            119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
                            129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
                            218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
                             81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
                            184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
                            222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180 };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    private noise input warp function
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! warp a coordinate
uint WarpCoord( const int c, const uint mask )
{
    const uint u = ( static_cast< uint >( c ) & mask );
	const uint x = ( u & 255 );
	const uint y = ( ( u >> 8 ) & 255 );
	const uint z = ( ( u >> 16 ) & 255 );
	const uint w = ( ( u >> 24 ) & 255 );
	return( static_cast< uint >( consts::Permute[ x + consts::Permute[ y + consts::Permute[ z + consts::Permute[ w ] ] ] ] ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    integer permute function
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! permute an integer
uint Permute( const uint u )
{
	const uint x = ( u & 255 );
	const uint y = ( ( u >> 8 ) & 255 );
	const uint z = ( ( u >> 16 ) & 255 );
	const uint w = ( ( u >> 24 ) & 255 );
	uint i = consts::Permute[ x + consts::Permute[ y + consts::Permute[ z + consts::Permute[ w ] ] ] ];
	uint j = consts::Permute[ y + consts::Permute[ z + consts::Permute[ w + consts::Permute[ x ] ] ] ];
	uint k = consts::Permute[ z + consts::Permute[ w + consts::Permute[ x + consts::Permute[ y ] ] ] ];
	uint l = consts::Permute[ w + consts::Permute[ x + consts::Permute[ y + consts::Permute[ z ] ] ] ];
	return( ( ( ( ( ( l << 8 ) + k ) << 8 ) + j ) << 8 ) + i );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    value noise
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! eased 1D noise
float EasedNoise( const float x )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int jx = ( ix + 1 );
	float fx = ( x - ix );
	fx = ( ( ( ( ( fx * 6 ) - 15 ) * fx ) + 10 ) * fx * fx * fx );
	float n[ 2 ];
	n[ 0 ] = FNoise( ix );
	n[ 1 ] = FNoise( jx );
	n[ 0 ] += ( ( n[ 1 ] - n[ 0 ] ) * fx );
	return( n[ 0 ] );
}

//! eased 2D noise
float EasedNoise( const float x, const float y )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	int jx = ( ix + 1 );
	int jy = ( iy + 1 );
	float fx = ( x - ix );
	float fy = ( y - iy );
	fx = ( ( ( ( ( fx * 6 ) - 15 ) * fx ) + 10 ) * fx * fx * fx );
	fy = ( ( ( ( ( fy * 6 ) - 15 ) * fy ) + 10 ) * fy * fy * fy );
	float n[ 2 ][ 2 ];
	n[ 0 ][ 0 ] = FNoise( ix, iy );
	n[ 1 ][ 0 ] = FNoise( jx, iy );
	n[ 0 ][ 1 ] = FNoise( ix, jy );
	n[ 1 ][ 1 ] = FNoise( jx, jy );
	n[ 0 ][ 0 ] += ( ( n[ 0 ][ 1 ] - n[ 0 ][ 0 ] ) * fy );
	n[ 1 ][ 0 ] += ( ( n[ 1 ][ 1 ] - n[ 1 ][ 0 ] ) * fy );
	n[ 0 ][ 0 ] += ( ( n[ 1 ][ 0 ] - n[ 0 ][ 0 ] ) * fx );
	return( n[ 0 ][ 0 ] );
}

//! eased 3D noise
float EasedNoise( const float x, const float y, const float z )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	int iz = static_cast< int >( z );
	if( z < 0 ) --iz;
	int jx = ( ix + 1 );
	int jy = ( iy + 1 );
	int jz = ( iz + 1 );
	float fx = ( x - ix );
	float fy = ( y - iy );
	float fz = ( z - iz );
	fx = ( ( ( ( ( fx * 6 ) - 15 ) * fx ) + 10 ) * fx * fx * fx );
	fy = ( ( ( ( ( fy * 6 ) - 15 ) * fy ) + 10 ) * fy * fy * fy );
	fz = ( ( ( ( ( fz * 6 ) - 15 ) * fz ) + 10 ) * fz * fz * fz );
	float n[ 2 ][ 2 ][ 2 ];
	n[ 0 ][ 0 ][ 0 ] = FNoise( ix, iy, iz );
	n[ 1 ][ 0 ][ 0 ] = FNoise( jx, iy, iz );
	n[ 0 ][ 1 ][ 0 ] = FNoise( ix, jy, iz );
	n[ 1 ][ 1 ][ 0 ] = FNoise( jx, jy, iz );
	n[ 0 ][ 0 ][ 1 ] = FNoise( ix, iy, jz );
	n[ 1 ][ 0 ][ 1 ] = FNoise( jx, iy, jz );
	n[ 0 ][ 1 ][ 1 ] = FNoise( ix, jy, jz );
	n[ 1 ][ 1 ][ 1 ] = FNoise( jx, jy, jz );
	n[ 0 ][ 0 ][ 0 ] += ( ( n[ 0 ][ 0 ][ 1 ] - n[ 0 ][ 0 ][ 0 ] ) * fz );
	n[ 1 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 0 ][ 1 ] - n[ 1 ][ 0 ][ 0 ] ) * fz );
	n[ 0 ][ 1 ][ 0 ] += ( ( n[ 0 ][ 1 ][ 1 ] - n[ 0 ][ 1 ][ 0 ] ) * fz );
	n[ 1 ][ 1 ][ 0 ] += ( ( n[ 1 ][ 1 ][ 1 ] - n[ 1 ][ 1 ][ 0 ] ) * fz );
	n[ 0 ][ 0 ][ 0 ] += ( ( n[ 0 ][ 1 ][ 0 ] - n[ 0 ][ 0 ][ 0 ] ) * fy );
	n[ 1 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 1 ][ 0 ] - n[ 1 ][ 0 ][ 0 ] ) * fy );
	n[ 0 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 0 ][ 0 ] - n[ 0 ][ 0 ][ 0 ] ) * fx );
	return( n[ 0 ][ 0 ][ 0 ] );
}

//! eased 4D noise
float EasedNoise( const float x, const float y, const float z, const float w )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	int iz = static_cast< int >( z );
	if( z < 0 ) --iz;
	int iw = static_cast< int >( w );
	if( w < 0 ) --iw;
	int jx = ( ix + 1 );
	int jy = ( iy + 1 );
	int jz = ( iz + 1 );
	int jw = ( iw + 1 );
	float fx = ( x - ix );
	float fy = ( y - iy );
	float fz = ( z - iz );
	float fw = ( w - iw );
	fx = ( ( ( ( ( fx * 6 ) - 15 ) * fx ) + 10 ) * fx * fx * fx );
	fy = ( ( ( ( ( fy * 6 ) - 15 ) * fy ) + 10 ) * fy * fy * fy );
	fz = ( ( ( ( ( fz * 6 ) - 15 ) * fz ) + 10 ) * fz * fz * fz );
	fw = ( ( ( ( ( fw * 6 ) - 15 ) * fw ) + 10 ) * fw * fw * fw );
	float n[ 2 ][ 2 ][ 2 ][ 2 ];
	n[ 0 ][ 0 ][ 0 ][ 0 ] = FNoise( ix, iy, iz, iw );
	n[ 1 ][ 0 ][ 0 ][ 0 ] = FNoise( jx, iy, iz, iw );
	n[ 0 ][ 1 ][ 0 ][ 0 ] = FNoise( ix, jy, iz, iw );
	n[ 1 ][ 1 ][ 0 ][ 0 ] = FNoise( jx, jy, iz, iw );
	n[ 0 ][ 0 ][ 1 ][ 0 ] = FNoise( ix, iy, jz, iw );
	n[ 1 ][ 0 ][ 1 ][ 0 ] = FNoise( jx, iy, jz, iw );
	n[ 0 ][ 1 ][ 1 ][ 0 ] = FNoise( ix, jy, jz, iw );
	n[ 1 ][ 1 ][ 1 ][ 0 ] = FNoise( jx, jy, jz, iw );
	n[ 0 ][ 0 ][ 0 ][ 1 ] = FNoise( ix, iy, iz, jw );
	n[ 1 ][ 0 ][ 0 ][ 1 ] = FNoise( jx, iy, iz, jw );
	n[ 0 ][ 1 ][ 0 ][ 1 ] = FNoise( ix, jy, iz, jw );
	n[ 1 ][ 1 ][ 0 ][ 1 ] = FNoise( jx, jy, iz, jw );
	n[ 0 ][ 0 ][ 1 ][ 1 ] = FNoise( ix, iy, jz, jw );
	n[ 1 ][ 0 ][ 1 ][ 1 ] = FNoise( jx, iy, jz, jw );
	n[ 0 ][ 1 ][ 1 ][ 1 ] = FNoise( ix, jy, jz, jw );
	n[ 1 ][ 1 ][ 1 ][ 1 ] = FNoise( jx, jy, jz, jw );
	n[ 0 ][ 0 ][ 0 ][ 0 ] += ( ( n[ 0 ][ 0 ][ 0 ][ 1 ] - n[ 0 ][ 0 ][ 0 ][ 0 ] ) * fw );
	n[ 1 ][ 0 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 0 ][ 0 ][ 1 ] - n[ 1 ][ 0 ][ 0 ][ 0 ] ) * fw );
	n[ 0 ][ 1 ][ 0 ][ 0 ] += ( ( n[ 0 ][ 1 ][ 0 ][ 1 ] - n[ 0 ][ 1 ][ 0 ][ 0 ] ) * fw );
	n[ 1 ][ 1 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 1 ][ 0 ][ 1 ] - n[ 1 ][ 1 ][ 0 ][ 0 ] ) * fw );
	n[ 0 ][ 0 ][ 1 ][ 0 ] += ( ( n[ 0 ][ 0 ][ 1 ][ 1 ] - n[ 0 ][ 0 ][ 1 ][ 0 ] ) * fw );
	n[ 1 ][ 0 ][ 1 ][ 0 ] += ( ( n[ 1 ][ 0 ][ 1 ][ 1 ] - n[ 1 ][ 0 ][ 1 ][ 0 ] ) * fw );
	n[ 0 ][ 1 ][ 1 ][ 0 ] += ( ( n[ 0 ][ 1 ][ 1 ][ 1 ] - n[ 0 ][ 1 ][ 1 ][ 0 ] ) * fw );
	n[ 1 ][ 1 ][ 1 ][ 0 ] += ( ( n[ 1 ][ 1 ][ 1 ][ 1 ] - n[ 1 ][ 1 ][ 1 ][ 0 ] ) * fw );
	n[ 0 ][ 0 ][ 0 ][ 0 ] += ( ( n[ 0 ][ 0 ][ 1 ][ 0 ] - n[ 0 ][ 0 ][ 0 ][ 0 ] ) * fz );
	n[ 1 ][ 0 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 0 ][ 1 ][ 0 ] - n[ 1 ][ 0 ][ 0 ][ 0 ] ) * fz );
	n[ 0 ][ 1 ][ 0 ][ 0 ] += ( ( n[ 0 ][ 1 ][ 1 ][ 0 ] - n[ 0 ][ 1 ][ 0 ][ 0 ] ) * fz );
	n[ 1 ][ 1 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 1 ][ 1 ][ 0 ] - n[ 1 ][ 1 ][ 0 ][ 0 ] ) * fz );
	n[ 0 ][ 0 ][ 0 ][ 0 ] += ( ( n[ 0 ][ 1 ][ 0 ][ 0 ] - n[ 0 ][ 0 ][ 0 ][ 0 ] ) * fy );
	n[ 1 ][ 0 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 1 ][ 0 ][ 0 ] - n[ 1 ][ 0 ][ 0 ][ 0 ] ) * fy );
	n[ 0 ][ 0 ][ 0 ][ 0 ] += ( ( n[ 1 ][ 0 ][ 0 ][ 0 ] - n[ 0 ][ 0 ][ 0 ][ 0 ] ) * fx );
	return( n[ 0 ][ 0 ][ 0 ][ 0 ] );
}

//! smooth 1D noise - higher quality but slower than eased noise
float SmoothNoise( const float x )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	float fx = ( x - ix );
	float n[ 4 ];
	for( int nx = 3; nx >= 0; --nx )
	{
		n[ nx ] = FNoise( ix + nx );
	}
	float w1, w2, w3, w4;
	w2 = ( fx * fx );
	w1 = ( w2 * fx );
	w3 = ( ( w1 + fx ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	n[ 0 ] = (
		( n[ 0 ] * w1 ) +
		( n[ 1 ] * w2 ) +
		( n[ 2 ] * w3 ) +
		( n[ 3 ] * w4 ) );
	return( n[ 0 ] / 1.25f );
}

//! smooth 2D noise - higher quality but slower than eased noise
float SmoothNoise( const float x, const float y )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	float fx = ( x - ix );
	float fy = ( y - iy );
	float n[ 4 ][ 4 ];
	for( int nx = 3; nx >= 0; --nx )
	{
		for( int ny = 3; ny >= 0; --ny )
		{
			n[ nx ][ ny ] = FNoise( ( ix + nx ), ( iy + ny ) );
		}
	}
	float w1, w2, w3, w4;
	w2 = ( fy * fy );
	w1 = ( w2 * fy );
	w3 = ( ( w1 + fy ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	for( int nx = 3; nx >= 0; --nx )
	{
		n[ nx ][ 0 ] = (
			( n[ nx ][ 0 ] * w1 ) +
			( n[ nx ][ 1 ] * w2 ) +
			( n[ nx ][ 2 ] * w3 ) +
			( n[ nx ][ 3 ] * w4 ) );
	}
	w2 = ( fx * fx );
	w1 = ( w2 * fx );
	w3 = ( ( w1 + fx ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	n[ 0 ][ 0 ] = (
		( n[ 0 ][ 0 ] * w1 ) +
		( n[ 1 ][ 0 ] * w2 ) +
		( n[ 2 ][ 0 ] * w3 ) +
		( n[ 3 ][ 0 ] * w4 ) );
	return( n[ 0 ][ 0 ] / ( 1.25f * 1.25f ) );
}

//! smooth 3D noise - higher quality but slower than eased noise
float SmoothNoise( const float x, const float y, const float z )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	int iz = static_cast< int >( z );
	if( z < 0 ) --iz;
	float fx = ( x - ix );
	float fy = ( y - iy );
	float fz = ( z - iz );
	float n[ 4 ][ 4 ][ 4 ];
	for( int nx = 3; nx >= 0; --nx )
	{
		for( int ny = 3; ny >= 0; --ny )
		{
			for( int nz = 3; nx >= 0; --nz )
			{
				n[ nx ][ ny ][ nz ] = FNoise( ( ix + nx ), ( iy + ny ), ( iz + nz ) );
			}
		}
	}
	float w1, w2, w3, w4;
	w2 = ( fz * fz );
	w1 = ( w2 * fz );
	w3 = ( ( w1 + fz ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	for( int nx = 3; nx >= 0; --nx )
	{
		for( int ny = 3; ny >= 0; --ny )
		{
			n[ nx ][ ny ][ 0 ] = (
				( n[ nx ][ ny ][ 0 ] * w1 ) +
				( n[ nx ][ ny ][ 1 ] * w2 ) +
				( n[ nx ][ ny ][ 2 ] * w3 ) +
				( n[ nx ][ ny ][ 3 ] * w4 ) );
		}
	}
	w2 = ( fy * fy );
	w1 = ( w2 * fy );
	w3 = ( ( w1 + fy ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	for( int nx = 3; nx >= 0; --nx )
	{
		n[ nx ][ 0 ][ 0 ] = (
			( n[ nx ][ 0 ][ 0 ] * w1 ) +
			( n[ nx ][ 1 ][ 0 ] * w2 ) +
			( n[ nx ][ 2 ][ 0 ] * w3 ) +
			( n[ nx ][ 3 ][ 0 ] * w4 ) );
	}
	w2 = ( fx * fx );
	w1 = ( w2 * fx );
	w3 = ( ( w1 + fx ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	n[ 0 ][ 0 ][ 0 ] = (
		( n[ 0 ][ 0 ][ 0 ] * w1 ) +
		( n[ 1 ][ 0 ][ 0 ] * w2 ) +
		( n[ 2 ][ 0 ][ 0 ] * w3 ) +
		( n[ 3 ][ 0 ][ 0 ] * w4 ) );
	return( n[ 0 ][ 0 ][ 0 ] / ( 1.25f * 1.25f * 1.25f ) );
}

//! smooth 4D noise - higher quality but slower than eased noise
float SmoothNoise( const float x, const float y, const float z, const float w )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	int iz = static_cast< int >( z );
	if( z < 0 ) --iz;
	int iw = static_cast< int >( w );
	if( w < 0 ) --iw;
	float fx = ( x - ix );
	float fy = ( y - iy );
	float fz = ( z - iz );
	float fw = ( w - iw );
	float n[ 4 ][ 4 ][ 4 ][ 4 ];
	for( int nx = 3; nx >= 0; --nx )
	{
		for( int ny = 3; ny >= 0; --ny )
		{
			for( int nz = 3; nx >= 0; --nz )
			{
				for( int nw = 3; nw >= 0; --nw )
				{
					n[ nx ][ ny ][ nz ][ nw ] = FNoise( ( ix + nx ), ( iy + ny ), ( iz + nz ), ( iw + nw ) );
				}
			}
		}
	}
	float w1, w2, w3, w4;
	w2 = ( fw * fw );
	w1 = ( w2 * fw );
	w3 = ( ( w1 + fw ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	for( int nx = 3; nx >= 0; --nx )
	{
		for( int ny = 3; ny >= 0; --ny )
		{
			for( int nz = 3; nx >= 0; --nz )
			{
				n[ nx ][ ny ][ nz ][ 0 ] = (
					( n[ nx ][ ny ][ nz ][ 0 ] * w1 ) +
					( n[ nx ][ ny ][ nz ][ 1 ] * w2 ) +
					( n[ nx ][ ny ][ nz ][ 2 ] * w3 ) +
					( n[ nx ][ ny ][ nz ][ 3 ] * w4 ) );
			}
		}
	}
	w2 = ( fz * fz );
	w1 = ( w2 * fz );
	w3 = ( ( w1 + fz ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	for( int nx = 3; nx >= 0; --nx )
	{
		for( int ny = 3; ny >= 0; --ny )
		{
			n[ nx ][ ny ][ 0 ][ 0 ] = (
				( n[ nx ][ ny ][ 0 ][ 0 ] * w1 ) +
				( n[ nx ][ ny ][ 1 ][ 0 ] * w2 ) +
				( n[ nx ][ ny ][ 2 ][ 0 ] * w3 ) +
				( n[ nx ][ ny ][ 3 ][ 0 ] * w4 ) );
		}
	}
	w2 = ( fy * fy );
	w1 = ( w2 * fy );
	w3 = ( ( w1 + fy ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	for( int nx = 3; nx >= 0; --nx )
	{
		n[ nx ][ 0 ][ 0 ][ 0 ] = (
			( n[ nx ][ 0 ][ 0 ][ 0 ] * w1 ) +
			( n[ nx ][ 1 ][ 0 ][ 0 ] * w2 ) +
			( n[ nx ][ 2 ][ 0 ][ 0 ] * w3 ) +
			( n[ nx ][ 3 ][ 0 ][ 0 ] * w4 ) );
	}
	w2 = ( fx * fx );
	w1 = ( w2 * fx );
	w3 = ( ( w1 + fx ) * 0.5f );
	w4 = ( ( w1 - w2 ) * 0.5f );
	w1 = ( w2 - w3 );
	w2 = ( 1.0f + ( w4 * 3.0f ) - w2 );
	w3 = ( w3 - ( w4 * 4.0f ) );
	n[ 0 ][ 0 ][ 0 ][ 0 ] = (
		( n[ 0 ][ 0 ][ 0 ][ 0 ] * w1 ) +
		( n[ 1 ][ 0 ][ 0 ][ 0 ] * w2 ) +
		( n[ 2 ][ 0 ][ 0 ][ 0 ] * w3 ) +
		( n[ 3 ][ 0 ][ 0 ][ 0 ] * w4 ) );
	return( n[ 0 ][ 0 ][ 0 ][ 0 ] / ( 1.25f * 1.25f * 1.25f * 1.25f ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    layered value noise generator
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! layered eased 1D noise
float EasedNoiseSum( const int layers, const float decay, const float x )
{
	float nx = x;
	float nv = EasedNoise( nx );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		nv = ( ( nv * nd ) + EasedNoise( nx ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered eased 2D noise
float EasedNoiseSum( const int layers, const float decay, const float x, const float y )
{
	float nx = x;
	float ny = y;
	float nv = EasedNoise( nx, ny );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nv = ( ( nv * nd ) + EasedNoise( nx, ny ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered eased 3D noise
float EasedNoiseSum( const int layers, const float decay, const float x, const float y, const float z )
{
	float nx = x;
	float ny = y;
	float nz = z;
	float nv = EasedNoise( nx, ny, nz );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nz *= 0.5f;
		nv = ( ( nv * nd ) + EasedNoise( nx, ny, nz ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered eased 4D noise
float EasedNoiseSum( const int layers, const float decay, const float x, const float y, const float z, const float w )
{
	float nx = x;
	float ny = y;
	float nz = z;
	float nw = w;
	float nv = EasedNoise( nx, ny, nz, nw );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nz *= 0.5f;
		nw *= 0.5f;
		nv = ( ( nv * nd ) + EasedNoise( nx, ny, nz, nw ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered smooth 1D noise - higher quality but slower than eased noise
float SmoothNoiseSum( const int layers, const float decay, const float x )
{
	float nx = x;
	float nv = SmoothNoise( nx );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		nv = ( ( nv * nd ) + SmoothNoise( nx ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered smooth 2D noise - higher quality but slower than eased noise
float SmoothNoiseSum( const int layers, const float decay, const float x, const float y )
{
	float nx = x;
	float ny = y;
	float nv = SmoothNoise( nx, ny );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nv = ( ( nv * nd ) + SmoothNoise( nx, ny ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered smooth 3D noise - higher quality but slower than eased noise
float SmoothNoiseSum( const int layers, const float decay, const float x, const float y, const float z )
{
	float nx = x;
	float ny = y;
	float nz = z;
	float nv = SmoothNoise( nx, ny, nz );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nz *= 0.5f;
		nv = ( ( nv * nd ) + SmoothNoise( nx, ny, nz ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered smooth 4D noise - higher quality but slower than eased noise
float SmoothNoiseSum( const int layers, const float decay, const float x, const float y, const float z, const float w )
{
	float nx = x;
	float ny = y;
	float nz = z;
	float nw = w;
	float nv = SmoothNoise( nx, ny, nz, nw );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nz *= 0.5f;
		nw *= 0.5f;
		nv = ( ( nv * nd ) + SmoothNoise( nx, ny, nz, nw ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Perlin noise functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! 2D Perlin noise
float PerlinNoise( const float x, const float y, const uint mask )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	float fx = ( x - ix );
	float fy = ( y - iy );
	float gx = ( fx - 1 );
	float gy = ( fy - 1 );
    uint ux = WarpCoord( ix, mask );
    uint uy = WarpCoord( iy, mask );
    uint nx = WarpCoord( ( ix + 1 ), mask );
    uint ny = WarpCoord( ( iy + 1 ), mask );
	int i00 = ( consts::Permute[ ux + consts::Permute[ uy ] ] & 7 );
	int i01 = ( consts::Permute[ ux + consts::Permute[ ny ] ] & 7 );
	int i10 = ( consts::Permute[ nx + consts::Permute[ uy ] ] & 7 );
	int i11 = ( consts::Permute[ nx + consts::Permute[ ny ] ] & 7 );
	float f00 = ( ( consts::Gradients2D[ i00 ][ 0 ] * fx ) + ( consts::Gradients2D[ i00 ][ 1 ] * fy ) );
	float f10 = ( ( consts::Gradients2D[ i10 ][ 0 ] * gx ) + ( consts::Gradients2D[ i10 ][ 1 ] * fy ) );
	float f01 = ( ( consts::Gradients2D[ i01 ][ 0 ] * fx ) + ( consts::Gradients2D[ i01 ][ 1 ] * gy ) );
	float f11 = ( ( consts::Gradients2D[ i11 ][ 0 ] * gx ) + ( consts::Gradients2D[ i11 ][ 1 ] * gy ) );
	fx = ( ( ( ( ( fx * 6 ) - 15 ) * fx ) + 10 ) * fx * fx * fx );
	fy = ( ( ( ( ( fy * 6 ) - 15 ) * fy ) + 10 ) * fy * fy * fy );
	f00 = ( ( ( f10 - f00 ) * fx ) + f00 );
	f01 = ( ( ( f11 - f01 ) * fx ) + f01 );
	f00 = ( ( ( f01 - f00 ) * fy ) + f00 );
	return( f00 * maths::consts::ROOT2 );
}

//! 3D Perlin noise
float PerlinNoise( const float x, const float y, const float z, const uint mask )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	int iz = static_cast< int >( z );
	if( z < 0 ) --iz;
	float fx = ( x - ix );
	float fy = ( y - iy );
	float fz = ( z - iz );
	float gx = ( fx - 1 );
	float gy = ( fy - 1 );
	float gz = ( fz - 1 );
    uint ux = WarpCoord( ix, mask );
    uint uy = WarpCoord( iy, mask );
    uint uz = WarpCoord( iz, mask );
    uint nx = WarpCoord( ( ix + 1 ), mask );
    uint ny = WarpCoord( ( iy + 1 ), mask );
    uint nz = WarpCoord( ( iz + 1 ), mask );
	int i000 = ( consts::Permute[ ux + consts::Permute[ uy + consts::Permute[ uz ] ] ] % 12 );
	int i001 = ( consts::Permute[ ux + consts::Permute[ uy + consts::Permute[ nz ] ] ] % 12 );
	int i010 = ( consts::Permute[ ux + consts::Permute[ ny + consts::Permute[ uz ] ] ] % 12 );
	int i011 = ( consts::Permute[ ux + consts::Permute[ ny + consts::Permute[ nz ] ] ] % 12 );
	int i100 = ( consts::Permute[ nx + consts::Permute[ uy + consts::Permute[ uz ] ] ] % 12 );
	int i101 = ( consts::Permute[ nx + consts::Permute[ uy + consts::Permute[ nz ] ] ] % 12 );
	int i110 = ( consts::Permute[ nx + consts::Permute[ ny + consts::Permute[ uz ] ] ] % 12 );
	int i111 = ( consts::Permute[ nx + consts::Permute[ ny + consts::Permute[ nz ] ] ] % 12 );
	float f000 = ( ( consts::Gradients3D[ i000 ][ 0 ] * fx ) + ( consts::Gradients3D[ i000 ][ 1 ] * fy ) + ( consts::Gradients3D[ i000 ][ 2 ] * fz ) );
	float f100 = ( ( consts::Gradients3D[ i100 ][ 0 ] * gx ) + ( consts::Gradients3D[ i100 ][ 1 ] * fy ) + ( consts::Gradients3D[ i100 ][ 2 ] * fz ) );
	float f010 = ( ( consts::Gradients3D[ i010 ][ 0 ] * fx ) + ( consts::Gradients3D[ i010 ][ 1 ] * gy ) + ( consts::Gradients3D[ i010 ][ 2 ] * fz ) );
	float f110 = ( ( consts::Gradients3D[ i110 ][ 0 ] * gx ) + ( consts::Gradients3D[ i110 ][ 1 ] * gy ) + ( consts::Gradients3D[ i110 ][ 2 ] * fz ) );
	float f001 = ( ( consts::Gradients3D[ i001 ][ 0 ] * fx ) + ( consts::Gradients3D[ i001 ][ 1 ] * fy ) + ( consts::Gradients3D[ i001 ][ 2 ] * gz ) );
	float f101 = ( ( consts::Gradients3D[ i101 ][ 0 ] * gx ) + ( consts::Gradients3D[ i101 ][ 1 ] * fy ) + ( consts::Gradients3D[ i101 ][ 2 ] * gz ) );
	float f011 = ( ( consts::Gradients3D[ i011 ][ 0 ] * fx ) + ( consts::Gradients3D[ i011 ][ 1 ] * gy ) + ( consts::Gradients3D[ i011 ][ 2 ] * gz ) );
	float f111 = ( ( consts::Gradients3D[ i111 ][ 0 ] * gx ) + ( consts::Gradients3D[ i111 ][ 1 ] * gy ) + ( consts::Gradients3D[ i111 ][ 2 ] * gz ) );
	fx = ( ( ( ( ( fx * 6 ) - 15 ) * fx ) + 10 ) * fx * fx * fx );
	fy = ( ( ( ( ( fy * 6 ) - 15 ) * fy ) + 10 ) * fy * fy * fy );
	fz = ( ( ( ( ( fz * 6 ) - 15 ) * fz ) + 10 ) * fz * fz * fz );
	f000 = ( ( ( f100 - f000 ) * fx ) + f000 );
	f001 = ( ( ( f101 - f001 ) * fx ) + f001 );
	f010 = ( ( ( f110 - f010 ) * fx ) + f010 );
	f011 = ( ( ( f111 - f011 ) * fx ) + f011 );
	f000 = ( ( ( f010 - f000 ) * fy ) + f000 );
	f001 = ( ( ( f011 - f001 ) * fy ) + f001 );
	f000 = ( ( ( f001 - f000 ) * fz ) + f000 );
	return( f000 );
}

//! 4D Perlin noise
float PerlinNoise( const float x, const float y, const float z, const float w, const uint mask )
{
	int ix = static_cast< int >( x );
	if( x < 0 ) --ix;
	int iy = static_cast< int >( y );
	if( y < 0 ) --iy;
	int iz = static_cast< int >( z );
	if( z < 0 ) --iz;
	int iw = static_cast< int >( w );
	if( w < 0 ) --iw;
	float fx = ( x - ix );
	float fy = ( y - iy );
	float fz = ( z - iz );
	float fw = ( w - iw );
	float gx = ( fx - 1 );
	float gy = ( fy - 1 );
	float gz = ( fz - 1 );
	float gw = ( fw - 1 );
    uint ux = WarpCoord( ix, mask );
    uint uy = WarpCoord( iy, mask );
    uint uz = WarpCoord( iz, mask );
    uint uw = WarpCoord( iw, mask );
    uint nx = WarpCoord( ( ix + 1 ), mask );
    uint ny = WarpCoord( ( iy + 1 ), mask );
    uint nz = WarpCoord( ( iz + 1 ), mask );
    uint nw = WarpCoord( ( iw + 1 ), mask );
	int i0000 = ( consts::Permute[ ux + consts::Permute[ uy + consts::Permute[ uz + consts::Permute[ uw ] ] ] ] & 31 );
	int i0001 = ( consts::Permute[ ux + consts::Permute[ uy + consts::Permute[ uz + consts::Permute[ nw ] ] ] ] & 31 );
	int i0010 = ( consts::Permute[ ux + consts::Permute[ uy + consts::Permute[ nz + consts::Permute[ uw ] ] ] ] & 31 );
	int i0011 = ( consts::Permute[ ux + consts::Permute[ uy + consts::Permute[ nz + consts::Permute[ nw ] ] ] ] & 31 );
	int i0100 = ( consts::Permute[ ux + consts::Permute[ ny + consts::Permute[ uz + consts::Permute[ uw ] ] ] ] & 31 );
	int i0101 = ( consts::Permute[ ux + consts::Permute[ ny + consts::Permute[ uz + consts::Permute[ nw ] ] ] ] & 31 );
	int i0110 = ( consts::Permute[ ux + consts::Permute[ ny + consts::Permute[ nz + consts::Permute[ uw ] ] ] ] & 31 );
	int i0111 = ( consts::Permute[ ux + consts::Permute[ ny + consts::Permute[ nz + consts::Permute[ nw ] ] ] ] & 31 );
	int i1000 = ( consts::Permute[ nx + consts::Permute[ uy + consts::Permute[ uz + consts::Permute[ uw ] ] ] ] & 31 );
	int i1001 = ( consts::Permute[ nx + consts::Permute[ uy + consts::Permute[ uz + consts::Permute[ nw ] ] ] ] & 31 );
	int i1010 = ( consts::Permute[ nx + consts::Permute[ uy + consts::Permute[ nz + consts::Permute[ uw ] ] ] ] & 31 );
	int i1011 = ( consts::Permute[ nx + consts::Permute[ uy + consts::Permute[ nz + consts::Permute[ nw ] ] ] ] & 31 );
	int i1100 = ( consts::Permute[ nx + consts::Permute[ ny + consts::Permute[ uz + consts::Permute[ uw ] ] ] ] & 31 );
	int i1101 = ( consts::Permute[ nx + consts::Permute[ ny + consts::Permute[ uz + consts::Permute[ nw ] ] ] ] & 31 );
	int i1110 = ( consts::Permute[ nx + consts::Permute[ ny + consts::Permute[ nz + consts::Permute[ uw ] ] ] ] & 31 );
	int i1111 = ( consts::Permute[ nx + consts::Permute[ ny + consts::Permute[ nz + consts::Permute[ nw ] ] ] ] & 31 );
	float f0000 = ( ( consts::Gradients4D[ i0000 ][ 0 ] * fx ) + ( consts::Gradients4D[ i0000 ][ 1 ] * fy ) + ( consts::Gradients4D[ i0000 ][ 2 ] * fz ) + ( consts::Gradients4D[ i0000 ][ 2 ] * fw ) );
	float f1000 = ( ( consts::Gradients4D[ i1000 ][ 0 ] * gx ) + ( consts::Gradients4D[ i1000 ][ 1 ] * fy ) + ( consts::Gradients4D[ i1000 ][ 2 ] * fz ) + ( consts::Gradients4D[ i1000 ][ 2 ] * fw ) );
	float f0100 = ( ( consts::Gradients4D[ i0100 ][ 0 ] * fx ) + ( consts::Gradients4D[ i0100 ][ 1 ] * gy ) + ( consts::Gradients4D[ i0100 ][ 2 ] * fz ) + ( consts::Gradients4D[ i0100 ][ 2 ] * fw ) );
	float f1100 = ( ( consts::Gradients4D[ i1100 ][ 0 ] * gx ) + ( consts::Gradients4D[ i1100 ][ 1 ] * gy ) + ( consts::Gradients4D[ i1100 ][ 2 ] * fz ) + ( consts::Gradients4D[ i1100 ][ 2 ] * fw ) );
	float f0010 = ( ( consts::Gradients4D[ i0010 ][ 0 ] * fx ) + ( consts::Gradients4D[ i0010 ][ 1 ] * fy ) + ( consts::Gradients4D[ i0010 ][ 2 ] * gz ) + ( consts::Gradients4D[ i0010 ][ 2 ] * fw ) );
	float f1010 = ( ( consts::Gradients4D[ i1010 ][ 0 ] * gx ) + ( consts::Gradients4D[ i1010 ][ 1 ] * fy ) + ( consts::Gradients4D[ i1010 ][ 2 ] * gz ) + ( consts::Gradients4D[ i1010 ][ 2 ] * fw ) );
	float f0110 = ( ( consts::Gradients4D[ i0110 ][ 0 ] * fx ) + ( consts::Gradients4D[ i0110 ][ 1 ] * gy ) + ( consts::Gradients4D[ i0110 ][ 2 ] * gz ) + ( consts::Gradients4D[ i0110 ][ 2 ] * fw ) );
	float f1110 = ( ( consts::Gradients4D[ i1110 ][ 0 ] * gx ) + ( consts::Gradients4D[ i1110 ][ 1 ] * gy ) + ( consts::Gradients4D[ i1110 ][ 2 ] * gz ) + ( consts::Gradients4D[ i1110 ][ 2 ] * fw ) );
	float f0001 = ( ( consts::Gradients4D[ i0001 ][ 0 ] * fx ) + ( consts::Gradients4D[ i0001 ][ 1 ] * fy ) + ( consts::Gradients4D[ i0001 ][ 2 ] * fz ) + ( consts::Gradients4D[ i0001 ][ 2 ] * gw ) );
	float f1001 = ( ( consts::Gradients4D[ i1001 ][ 0 ] * gx ) + ( consts::Gradients4D[ i1001 ][ 1 ] * fy ) + ( consts::Gradients4D[ i1001 ][ 2 ] * fz ) + ( consts::Gradients4D[ i1001 ][ 2 ] * gw ) );
	float f0101 = ( ( consts::Gradients4D[ i0101 ][ 0 ] * fx ) + ( consts::Gradients4D[ i0101 ][ 1 ] * gy ) + ( consts::Gradients4D[ i0101 ][ 2 ] * fz ) + ( consts::Gradients4D[ i0101 ][ 2 ] * gw ) );
	float f1101 = ( ( consts::Gradients4D[ i1101 ][ 0 ] * gx ) + ( consts::Gradients4D[ i1101 ][ 1 ] * gy ) + ( consts::Gradients4D[ i1101 ][ 2 ] * fz ) + ( consts::Gradients4D[ i1101 ][ 2 ] * gw ) );
	float f0011 = ( ( consts::Gradients4D[ i0011 ][ 0 ] * fx ) + ( consts::Gradients4D[ i0011 ][ 1 ] * fy ) + ( consts::Gradients4D[ i0011 ][ 2 ] * gz ) + ( consts::Gradients4D[ i0011 ][ 2 ] * gw ) );
	float f1011 = ( ( consts::Gradients4D[ i1011 ][ 0 ] * gx ) + ( consts::Gradients4D[ i1011 ][ 1 ] * fy ) + ( consts::Gradients4D[ i1011 ][ 2 ] * gz ) + ( consts::Gradients4D[ i1011 ][ 2 ] * gw ) );
	float f0111 = ( ( consts::Gradients4D[ i0111 ][ 0 ] * fx ) + ( consts::Gradients4D[ i0111 ][ 1 ] * gy ) + ( consts::Gradients4D[ i0111 ][ 2 ] * gz ) + ( consts::Gradients4D[ i0111 ][ 2 ] * gw ) );
	float f1111 = ( ( consts::Gradients4D[ i1111 ][ 0 ] * gx ) + ( consts::Gradients4D[ i1111 ][ 1 ] * gy ) + ( consts::Gradients4D[ i1111 ][ 2 ] * gz ) + ( consts::Gradients4D[ i1111 ][ 2 ] * gw ) );
	fx = ( ( ( ( ( fx * 6 ) - 15 ) * fx ) + 10 ) * fx * fx * fx );
	fy = ( ( ( ( ( fy * 6 ) - 15 ) * fy ) + 10 ) * fy * fy * fy );
	fz = ( ( ( ( ( fz * 6 ) - 15 ) * fz ) + 10 ) * fz * fz * fz );
	fw = ( ( ( ( ( fw * 6 ) - 15 ) * fw ) + 10 ) * fw * fw * fw );
	f0000 = ( ( ( f1000 - f0000 ) * fx ) + f0000 );
	f0001 = ( ( ( f1001 - f0001 ) * fx ) + f0001 );
	f0010 = ( ( ( f1010 - f0010 ) * fx ) + f0010 );
	f0011 = ( ( ( f1011 - f0011 ) * fx ) + f0011 );
	f0100 = ( ( ( f1100 - f0100 ) * fx ) + f0100 );
	f0101 = ( ( ( f1101 - f0101 ) * fx ) + f0101 );
	f0110 = ( ( ( f1110 - f0110 ) * fx ) + f0110 );
	f0111 = ( ( ( f1111 - f0111 ) * fx ) + f0111 );
	f0000 = ( ( ( f0100 - f0000 ) * fy ) + f0000 );
	f0001 = ( ( ( f0101 - f0001 ) * fy ) + f0001 );
	f0010 = ( ( ( f0110 - f0010 ) * fy ) + f0010 );
	f0011 = ( ( ( f0111 - f0011 ) * fy ) + f0011 );
	f0000 = ( ( ( f0010 - f0000 ) * fz ) + f0000 );
	f0001 = ( ( ( f0011 - f0001 ) * fz ) + f0001 );
	f0000 = ( ( ( f0001 - f0000 ) * fw ) + f0000 );
	return( f0000 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    layered Perlin noise generator
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! layered 2D Perlin noise
float PerlinNoiseSum( const int layers, const float decay, const float x, const float y )
{
	float nx = x;
	float ny = y;
	float nv = PerlinNoise( nx, ny );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nv = ( ( nv * nd ) + PerlinNoise( nx, ny ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered 3D Perlin noise
float PerlinNoiseSum( const int layers, const float decay, const float x, const float y, const float z )
{
	float nx = x;
	float ny = y;
	float nz = z;
	float nv = PerlinNoise( nx, ny, nz );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nz *= 0.5f;
		nv = ( ( nv * nd ) + PerlinNoise( nx, ny, nz ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered 4D Perlin noise
float PerlinNoiseSum( const int layers, const float decay, const float x, const float y, const float z, const float w )
{
	float nx = x;
	float ny = y;
	float nz = z;
	float nw = w;
	float nv = PerlinNoise( nx, ny, nz, nw );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nz *= 0.5f;
		nw *= 0.5f;
		nv = ( ( nv * nd ) + PerlinNoise( nx, ny, nz, nw ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Simplex noise functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! 2D Simplex noise
float SimplexNoise( const float x, const float y )
{
	static const float f2 = ( ( sqrtf( 3.0f ) - 1.0f ) * 0.5f );
	static const float g2 = ( ( 3.0f - sqrtf( 3.0f ) ) / 6.0f );
	float s = ( ( ( x + y ) * f2 ) + x );
	int ix = static_cast< int >( s );
	if( s < 0 ) --ix;
	s += ( y - x );
	int iy = static_cast< int >( s );
	if( s < 0 ) --iy;
	s = ( ( ix + iy ) * g2 );
	float x0 = ( x + s - ix );
	float y0 = ( y + s - iy );
	float x1 = ( x0 + g2 );
	float y1 = ( y0 + g2 );
	float x2 = ( x1 + g2 - 1.0f );
	float y2 = ( y1 + g2 - 1.0f );
	ix &= 255;
	iy &= 255;
	int i0 = ( consts::Permute[ ix + consts::Permute[ iy ] ] % 12 );
	int i2 = ( consts::Permute[ ix + 1 + consts::Permute[ iy + 1 ] ] % 12 );
	int i1;
	if( x0 > y0 )
	{
		x1 -= 1.0f;
		i1 = ( consts::Permute[ ix + 1 + consts::Permute[ iy ] ] % 12 );
	}
	else
	{
		y1 -= 1.0f;
		i1 = ( consts::Permute[ ix + consts::Permute[ iy + 1 ] ] % 12 );
	}
	float n = 0;
	float t;
	if( ( t = ( 0.5f - ( x0 * x0 ) - ( y0 * y0 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients3D[ i0 ][ 0 ] * x0 ) +
			( consts::Gradients3D[ i0 ][ 1 ] * y0 ) ) );
	}
	if( ( t = ( 0.5f - ( x1 * x1 ) - ( y1 * y1 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients3D[ i1 ][ 0 ] * x1 ) +
			( consts::Gradients3D[ i1 ][ 1 ] * y1 ) ) );
	}
	if( ( t = ( 0.5f - ( x2 * x2 ) - ( y2 * y2 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients3D[ i2 ][ 0 ] * x2 ) +
			( consts::Gradients3D[ i2 ][ 1 ] * y2 ) ) );
	}
	return( n * 70 );
}

//! 3D Simplex noise
float SimplexNoise( const float x, const float y, const float z )
{
	static const float f3 = ( 1.0f / 3.0f );
	static const float g3 = ( 1.0f / 6.0f );
	float s = ( ( ( x + y + z ) * f3 ) + x );
	int ix = static_cast< int >( s );
	if( s < 0 ) --ix;
	s += ( y - x );
	int iy = static_cast< int >( s );
	if( s < 0 ) --iy;
	s += ( z - y );
	int iz = static_cast< int >( s );
	if( s < 0 ) --iz;
	s = ( ( ix + iy + iz ) * g3 );
	float x0 = ( x + s - ix );
	float y0 = ( y + s - iy );
	float z0 = ( z + s - iz );
	float x1 = ( x0 + g3 );
	float y1 = ( y0 + g3 );
	float z1 = ( z0 + g3 );
	float x2 = ( x1 + g3 );
	float y2 = ( y1 + g3 );
	float z2 = ( z1 + g3 );
	float x3 = ( x2 + g3 - 1.0f );
	float y3 = ( y2 + g3 - 1.0f );
	float z3 = ( z2 + g3 - 1.0f );
	ix &= 255;
	iy &= 255;
	iz &= 255;
	int i0 = ( consts::Permute[ ix + consts::Permute[ iy + consts::Permute[ iz ] ] ] % 12 );
	int i3 = ( consts::Permute[ ix + 1 + consts::Permute[ iy + 1 + consts::Permute[ iz + 1 ] ] ] % 12 );
	int i1, i2;
	int order = 0;
	if( x0 > y0 ) order += 1;
	if( y0 > z0 ) order += 2;
	if( z0 > x0 ) order += 4;
	switch( order )
	{
	case( 4 ):
	case( 5 ):
		{	//	z highest
			z1 -= 1.0f;
			i1 = ( consts::Permute[ ix + consts::Permute[ iy + consts::Permute[ iz + 1 ] ] ] % 12 );
			break;
		}
	case( 2 ):
	case( 6 ):
		{	//	y highest
			y1 -= 1.0f;
			i1 = ( consts::Permute[ ix + consts::Permute[ iy + 1 + consts::Permute[ iz ] ] ] % 12 );
			break;
		}
	default:
		{	//	x highest
			x1 -= 1.0f;
			i1 = ( consts::Permute[ ix + 1 + consts::Permute[ iy + consts::Permute[ iz ] ] ] % 12 );
			break;
		}
	}
	switch( order )
	{
	case( 4 ):
	case( 6 ):
		{	//	x lowest
			y2 -= 1.0f;
			z2 -= 1.0f;
			i2 = ( consts::Permute[ ix + consts::Permute[ iy + 1 + consts::Permute[ iz + 1 ] ] ] % 12 );
			break;
		}
	case( 1 ):
	case( 5 ):
		{	//	y lowest
			z2 -= 1.0f;
			x2 -= 1.0f;
			i2 = ( consts::Permute[ ix + 1 + consts::Permute[ iy + consts::Permute[ iz + 1 ] ] ] % 12 );
			break;
		}
	default:
		{	//	z lowest
			x2 -= 1.0f;
			y2 -= 1.0f;
			i2 = ( consts::Permute[ ix + 1 + consts::Permute[ iy + 1 + consts::Permute[ iz ] ] ] % 12 );
			break;
		}
	}
	float n = 0;
	float t;
	if( ( t = ( 0.6f - ( x0 * x0 ) - ( y0 * y0 ) - ( z0 * z0 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients3D[ i0 ][ 0 ] * x0 ) +
			( consts::Gradients3D[ i0 ][ 1 ] * y0 ) +
			( consts::Gradients3D[ i0 ][ 2 ] * z0 ) ) );
	}
	if( ( t = ( 0.6f - ( x1 * x1 ) - ( y1 * y1 ) - ( z1 * z1 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients3D[ i1 ][ 0 ] * x1 ) +
			( consts::Gradients3D[ i1 ][ 1 ] * y1 ) +
			( consts::Gradients3D[ i1 ][ 2 ] * z1 ) ) );
	}
	if( ( t = ( 0.6f - ( x2 * x2 ) - ( y2 * y2 ) - ( z2 * z2 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients3D[ i2 ][ 0 ] * x2 ) +
			( consts::Gradients3D[ i2 ][ 1 ] * y2 ) +
			( consts::Gradients3D[ i2 ][ 2 ] * z2 ) ) );
	}
	if( ( t = ( 0.6f - ( x3 * x3 ) - ( y3 * y3 ) - ( z3 * z3 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients3D[ i3 ][ 0 ] * x3 ) +
			( consts::Gradients3D[ i3 ][ 1 ] * y3 ) +
			( consts::Gradients3D[ i3 ][ 2 ] * z3 ) ) );
	}
	return( n * 32 );
}

//! 4D Simplex noise
float SimplexNoise( const float x, const float y, const float z, const float w )
{
	static const uchar Simplex[ 64 ][ 4 ] = {
		{ 3, 0, 1, 2 }, { 3, 0, 2, 1 }, { 3, 3, 3, 3 }, { 3, 1, 2, 0 },
		{ 3, 1, 0, 2 }, { 3, 3, 3, 3 }, { 3, 2, 0, 1 }, { 3, 2, 1, 0 },
		{ 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 0, 1, 2, 3 },
		{ 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 0, 2, 1, 3 },
		{ 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 },
		{ 0, 1, 3, 2 }, { 3, 3, 3, 3 }, { 0, 2, 3, 1 }, { 3, 3, 3, 3 },
		{ 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 },
		{ 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 1, 2, 3, 0 }, { 1, 2, 0, 3 },
		{ 0, 3, 1, 2 }, { 0, 3, 2, 1 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 },
		{ 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 },
		{ 3, 3, 3, 3 }, { 1, 3, 2, 0 }, { 3, 3, 3, 3 }, { 1, 0, 2, 3 },
		{ 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 },
		{ 1, 3, 0, 2 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 },
		{ 1, 0, 3, 2 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 }, { 3, 3, 3, 3 },
		{ 2, 3, 0, 1 }, { 2, 3, 1, 0 }, { 3, 3, 3, 3 }, { 2, 0, 1, 3 },
		{ 2, 0, 3, 1 }, { 3, 3, 3, 3 }, { 2, 1, 3, 0 }, { 2, 1, 0, 3 } };
	static const int Deltas[ 6 ] = { 1, 1, 1, 0, 0, 0 };
	static const float f4 = ( ( sqrtf( 5.0f ) - 1.0f ) * 0.25f );
	static const float g4 = ( ( 5.0f - sqrtf( 5.0f ) ) / 20.0f );
	float s = ( ( ( x + y + z + w ) * f4 ) + x );
	int ix = static_cast< int >( s );
	if( s < 0 ) --ix;
	s += ( y - x );
	int iy = static_cast< int >( s );
	if( s < 0 ) --iy;
	s += ( z - y );
	int iz = static_cast< int >( s );
	if( s < 0 ) --iz;
	s += ( w - z );
	int iw = static_cast< int >( s );
	if( s < 0 ) --iw;
	s = ( ( ix + iy + iz + iw ) * g4 );
	float x0 = ( x + s - ix );
	float y0 = ( y + s - iy );
	float z0 = ( z + s - iz );
	float w0 = ( w + s - iw );
	float x1 = ( x0 + g4 );
	float y1 = ( y0 + g4 );
	float z1 = ( z0 + g4 );
	float w1 = ( w0 + g4 );
	float x2 = ( x1 + g4 );
	float y2 = ( y1 + g4 );
	float z2 = ( z1 + g4 );
	float w2 = ( w1 + g4 );
	float x3 = ( x2 + g4 );
	float y3 = ( y2 + g4 );
	float z3 = ( z2 + g4 );
	float w3 = ( w2 + g4 );
	float x4 = ( x3 + g4 - 1.0f );
	float y4 = ( y3 + g4 - 1.0f );
	float z4 = ( z3 + g4 - 1.0f );
	float w4 = ( w3 + g4 - 1.0f );
	int order = 0;
	if( x0 > y0 ) order += 32;
	if( x0 > z0 ) order += 16;
	if( x0 > w0 ) order += 8;
	if( y0 > z0 ) order += 4;
	if( y0 > w0 ) order += 2;
	if( z0 > w0 ) order += 1;
	int dx = Simplex[ order ][ 0 ];
	int dy = Simplex[ order ][ 1 ];
	int dz = Simplex[ order ][ 2 ];
	int dw = Simplex[ order ][ 3 ];
	x1 -= Deltas[ dx + 0 ];
	x2 -= Deltas[ dx + 1 ];
	x3 -= Deltas[ dx + 2 ];
	y1 -= Deltas[ dy + 0 ];
	y2 -= Deltas[ dy + 1 ];
	y3 -= Deltas[ dy + 2 ];
	z1 -= Deltas[ dz + 0 ];
	z2 -= Deltas[ dz + 1 ];
	z3 -= Deltas[ dz + 2 ];
	w1 -= Deltas[ dw + 0 ];
	w2 -= Deltas[ dw + 1 ];
	w3 -= Deltas[ dw + 2 ];
	ix &= 255;
	iy &= 255;
	iz &= 255;
	iw &= 255;
	int i0 = ( consts::Permute[ ix + consts::Permute[ iy + consts::Permute[ iz + consts::Permute[ iw ] ] ] ] & 31 );
	int i1 = ( consts::Permute[ ix + Deltas[ dx + 0 ] + consts::Permute[ iy + Deltas[ dy + 0 ] + consts::Permute[ iz + Deltas[ dz + 0 ] + consts::Permute[ iw + Deltas[ dw + 0 ] ] ] ] ] & 31 );
	int i2 = ( consts::Permute[ ix + Deltas[ dx + 1 ] + consts::Permute[ iy + Deltas[ dy + 1 ] + consts::Permute[ iz + Deltas[ dz + 1 ] + consts::Permute[ iw + Deltas[ dw + 1 ] ] ] ] ] & 31 );
	int i3 = ( consts::Permute[ ix + Deltas[ dx + 2 ] + consts::Permute[ iy + Deltas[ dy + 2 ] + consts::Permute[ iz + Deltas[ dz + 2 ] + consts::Permute[ iw + Deltas[ dw + 2 ] ] ] ] ] & 31 );
	int i4 = ( consts::Permute[ ix + 1 + consts::Permute[ iy + 1 + consts::Permute[ iz + 1 + consts::Permute[ iw + 1 ] ] ] ] & 31 );
	float n = 0;
	float t;
	if( ( t = ( 0.6f - ( x0 * x0 ) - ( y0 * y0 ) - ( z0 * z0 ) - ( w0 * w0 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients4D[ i0 ][ 0 ] * x0 ) +
			( consts::Gradients4D[ i0 ][ 1 ] * y0 ) +
			( consts::Gradients4D[ i0 ][ 2 ] * z0 ) +
			( consts::Gradients4D[ i0 ][ 3 ] * w0 ) ) );
	}
	if( ( t = ( 0.6f - ( x1 * x1 ) - ( y1 * y1 ) - ( z1 * z1 ) - ( w1 * w1 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients4D[ i1 ][ 0 ] * x1 ) +
			( consts::Gradients4D[ i1 ][ 1 ] * y1 ) +
			( consts::Gradients4D[ i1 ][ 2 ] * z1 ) +
			( consts::Gradients4D[ i1 ][ 3 ] * w1 ) ) );
	}
	if( ( t = ( 0.6f - ( x2 * x2 ) - ( y2 * y2 ) - ( z2 * z2 ) - ( w2 * w2 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients4D[ i2 ][ 0 ] * x2 ) +
			( consts::Gradients4D[ i2 ][ 1 ] * y2 ) +
			( consts::Gradients4D[ i2 ][ 2 ] * z2 ) +
			( consts::Gradients4D[ i2 ][ 3 ] * w2 ) ) );
	}
	if( ( t = ( 0.6f - ( x3 * x3 ) - ( y3 * y3 ) - ( z3 * z3 ) - ( w3 * w3 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients4D[ i3 ][ 0 ] * x3 ) +
			( consts::Gradients4D[ i3 ][ 1 ] * y3 ) +
			( consts::Gradients4D[ i3 ][ 2 ] * z3 ) +
			( consts::Gradients4D[ i3 ][ 3 ] * w3 ) ) );
	}
	if( ( t = ( 0.6f - ( x4 * x4 ) - ( y4 * y4 ) - ( z4 * z4 ) - ( w4 * w4 ) ) ) > 0 )
	{
		t *= t;
		t *= t;
		n += ( t * (
			( consts::Gradients4D[ i4 ][ 0 ] * x4 ) +
			( consts::Gradients4D[ i4 ][ 1 ] * y4 ) +
			( consts::Gradients4D[ i4 ][ 2 ] * z4 ) +
			( consts::Gradients4D[ i4 ][ 3 ] * w4 ) ) );
	}
	return( n * 27 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    layered Simplex noise generator
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! layered 2D Simplex noise
float SimplexNoiseSum( const int layers, const float decay, const float x, const float y )
{
	float nx = x;
	float ny = y;
	float nv = SimplexNoise( nx, ny );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nv = ( ( nv * nd ) + SimplexNoise( nx, ny ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered 3D Simplex noise
float SimplexNoiseSum( const int layers, const float decay, const float x, const float y, const float z )
{
	float nx = x;
	float ny = y;
	float nz = z;
	float nv = SimplexNoise( nx, ny, nz );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nz *= 0.5f;
		nv = ( ( nv * nd ) + SimplexNoise( nx, ny, nz ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

//! layered 4D Simplex noise
float SimplexNoiseSum( const int layers, const float decay, const float x, const float y, const float z, const float w )
{
	float nx = x;
	float ny = y;
	float nz = z;
	float nw = w;
	float nv = SimplexNoise( nx, ny, nz, nw );
	float ns = 1.0f;
	float nd = ( ( decay <= 0 ) ? 1 : ( 1.0f - decay ) );
	for( int nl = ( ( layers < 2 ) ? 0 : ( layers - 1 ) ); nl; --nl )
	{
		nx *= 0.5f;
		ny *= 0.5f;
		nz *= 0.5f;
		nw *= 0.5f;
		nv = ( ( nv * nd ) + SimplexNoise( nx, ny, nz, nw ) );
		ns = ( ( ns * nd ) + 1.0f );
	}
	return( nv / ns );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end maths namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace maths

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
