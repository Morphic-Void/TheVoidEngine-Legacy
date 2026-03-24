
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   main.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        Application entry point.
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

#include "platforms/windows/stdafx.h"
#include "platforms/windows/plugin.h"
#include "platforms/windows/cenginex.h"
#include "libs/system/base/types.h"
#include "libs/memory/ref_blob.h"
#include "main.h"

namespace HilbertTest
{

const uint8_t mortonToHilbertTable[] = {
    48, 33, 27, 34, 47, 78, 28, 77,
    66, 29, 51, 52, 65, 30, 72, 63,
    76, 95, 75, 24, 53, 54, 82, 81,
    18,  3, 17, 80, 61,  4, 62, 15,
     0, 59, 71, 60, 49, 50, 86, 85,
    84, 83,  5, 90, 79, 56,  6, 89,
    32, 23,  1, 94, 11, 12,  2, 93,
    42, 41, 13, 14, 35, 88, 36, 31,
    92, 37, 87, 38, 91, 74,  8, 73,
    46, 45,  9, 10,  7, 20, 64, 19,
    70, 25, 39, 16, 69, 26, 44, 43,
    22, 55, 21, 68, 57, 40, 58, 67,
};
 
const uint8_t hilbertToMortonTable[] = {
    48, 33, 35, 26, 30, 79, 77, 44,
    78, 68, 64, 50, 51, 25, 29, 63,
    27, 87, 86, 74, 72, 52, 53, 89,
    83, 18, 16,  1,  5, 60, 62, 15,
     0, 52, 53, 57, 59, 87, 86, 66,
    61, 95, 91, 81, 80,  2,  6, 76,
    32,  2,  6, 12, 13, 95, 91, 17,
    93, 41, 40, 36, 38, 10, 11, 31,
    14, 79, 77, 92, 88, 33, 35, 82,
    70, 10, 11, 23, 21, 41, 40,  4,
    19, 25, 29, 47, 46, 68, 64, 34,
    45, 60, 62, 71, 67, 18, 16, 49,
};
 
uint32_t transformCurve( const uint32_t in, const uint32_t bits, const uint8_t* const lookupTable )
{
    uint32_t transform = 0;
    uint32_t out = 0;
    for( int32_t i = ( ( bits << 1 ) + bits - 3 ); i >= 0; i -= 3 )
    {
        transform = lookupTable[ ( transform & ~7 ) | ( ( in >> i ) & 7 ) ];
        out = ( ( out << 3 ) | ( transform & 7 ) );
    }
    return( out );
}

unsigned int Morton_3D_Encode_10bit( register unsigned int index1, register unsigned int index2, register unsigned int index3 )
{
	index1 &= 0x000003ff;
	index2 &= 0x000003ff;
	index3 &= 0x000003ff;
	index1 |= ( index1 << 16 );
	index2 |= ( index2 << 16 );
	index3 |= ( index3 << 16 );
	index1 &= 0x030000ff;
	index2 &= 0x030000ff;
	index3 &= 0x030000ff;
	index1 |= ( index1 << 8 );
	index2 |= ( index2 << 8 );
	index3 |= ( index3 << 8 );
	index1 &= 0x0300f00f;
	index2 &= 0x0300f00f;
	index3 &= 0x0300f00f;
	index1 |= ( index1 << 4 );
	index2 |= ( index2 << 4 );
	index3 |= ( index3 << 4 );
	index1 &= 0x030c30c3;
	index2 &= 0x030c30c3;
	index3 &= 0x030c30c3;
	index1 |= ( index1 << 2 );
	index2 |= ( index2 << 2 );
	index3 |= ( index3 << 2 );
	index1 &= 0x09249249;
	index2 &= 0x09249249;
	index3 &= 0x09249249;
	return( index1 | ( index2 << 1 ) | ( index3 << 2 ) );
}

void Morton_3D_Decode_10bit( const unsigned int morton, unsigned int& index1, unsigned int& index2, unsigned int& index3 )
{
	unsigned int value1 = morton;
	unsigned int value2 = ( value1 >> 1 );
	unsigned int value3 = ( value1 >> 2 );
	value1 &= 0x09249249;
	value2 &= 0x09249249;
	value3 &= 0x09249249;
	value1 |= ( value1 >> 2 );
	value2 |= ( value2 >> 2 );
	value3 |= ( value3 >> 2 );
	value1 &= 0x030c30c3;
	value2 &= 0x030c30c3;
	value3 &= 0x030c30c3;
	value1 |= ( value1 >> 4 );
	value2 |= ( value2 >> 4 );
	value3 |= ( value3 >> 4 );
	value1 &= 0x0300f00f;
	value2 &= 0x0300f00f;
	value3 &= 0x0300f00f;
	value1 |= ( value1 >> 8 );
	value2 |= ( value2 >> 8 );
	value3 |= ( value3 >> 8 );
	value1 &= 0x030000ff;
	value2 &= 0x030000ff;
	value3 &= 0x030000ff;
	value1 |= ( value1 >> 16 );
	value2 |= ( value2 >> 16 );
	value3 |= ( value3 >> 16 );
	value1 &= 0x000003ff;
	value2 &= 0x000003ff;
	value3 &= 0x000003ff;
	index1 = value1;
	index2 = value2;
	index3 = value3;
}

unsigned int MortonToHilbert3D( const unsigned int morton, const unsigned int bits )
{
	unsigned int index = morton;
	if( bits > 1 )
	{
		unsigned int block = ( ( bits * 3 ) - 3 );
		unsigned int hcode = ( ( index >> block ) & 7 );
		unsigned int mcode, shift, signs;
		shift = signs = 0;
		while( block )
		{
			block -= 3;
			hcode <<= 2;
			mcode = ( ( 0x20212021 >> hcode ) & 3 );
			shift = ( ( 0x49 >> ( 4 - shift - mcode ) ) & 3 );
			signs = ( ( ( ( signs | ( signs << 3 ) ) >> mcode ) ^ ( 0x53560300 >> hcode ) ) & 7 );
			mcode = ( ( index >> block ) & 7 );
			hcode = mcode;
			hcode = ( ( ( hcode | ( hcode << 3 ) ) >> shift ) & 7 );
			hcode ^= signs;
			index ^= ( ( mcode ^ hcode ) << block );
		}
	}
	index ^= ( ( index >> 1 ) & 0x92492492 );
	index ^= ( ( index & 0x92492492 ) >> 1 );
	return( index );
}

unsigned int HilbertToMorton3D( const unsigned int hilbert, const unsigned int bits )
{
	unsigned int index = hilbert;
	index ^= ( ( index & 0x92492492 ) >> 1 );
	index ^= ( ( index >> 1 ) & 0x92492492 );
	if( bits > 1 )
	{
		unsigned int block = ( ( bits * 3 ) - 3 );
		unsigned int hcode = ( ( index >> block ) & 7 );
		unsigned int mcode, shift, signs;
		shift = signs = 0;
		while( block )
		{
			block -= 3;
			hcode <<= 2;
			mcode = ( ( 0x20212021 >> hcode ) & 3 );
			shift = ( ( 0x49 >> ( 4 - shift + mcode ) ) & 3 );
			signs = ( ( ( ( signs | ( signs << 3 ) ) >> mcode ) ^ ( 0x53560300 >> hcode ) ) & 7 );
			hcode = ( ( index >> block ) & 7 );
			mcode = hcode;
			mcode ^= signs;
			mcode = ( ( ( mcode | ( mcode << 3 ) ) >> shift ) & 7 );
			index ^= ( ( hcode ^ mcode ) << block );
		}
	}
	return( index );
}

inline int absInt( const int i ) { return( ( i < 0 ) ? ( 0 - i ) : i ); };

bool ValidateHilbertFunc()
{
    uint x1, y1, z1;
    Morton_3D_Decode_10bit( HilbertToMorton3D( 0, 4 ), x1, y1, z1 );
    for( uint32_t linear = 1; linear < 0x00001000; ++linear )
    {
        uint x2, y2, z2;
        if( linear != HilbertToMorton3D( MortonToHilbert3D( linear, 4 ), 4 ) )
        {
            return( false );
        }
        Morton_3D_Decode_10bit( HilbertToMorton3D( linear, 4 ), x2, y2, z2 );
        int delta = ( ( absInt( static_cast< int >( x2 - x1 ) ) ) + ( absInt( static_cast< int >( y2 - y1 ) ) ) + ( absInt( static_cast< int >( z2 - z1 ) ) ) );
        if( delta != 1 )
        {
            return( false );
        }
        x1 = x2;
        y1 = y2;
        z1 = z2;
    }
    return( true );
}

bool CompareHilbertFuncs()
{
    for( uint32_t linear = 1; linear < 0x00001000; ++linear )
    {
        if( HilbertToMorton3D( linear, 4 ) != transformCurve( linear, 4, hilbertToMortonTable ) )
        {
            return( false );
        }
    }
    return( true );
}

static int bp = 0;

void calcShifts()
{
    uint32_t shifts_m2h[ 3 ];
    uint32_t shifts_h2m[ 3 ];
    for( uint32_t shift = 0; shift < 3; ++shift )
    {
        uint32_t _shifts_m2h = 0;
        uint32_t _shifts_h2m = 0;
        for( uint32_t hcode = 0; hcode < 8; ++hcode )
        {
            uint32_t block = ( hcode << 2 );
			uint32_t mcode = ( ( 0x20212021 >> block ) & 3 );
			_shifts_m2h += ( ( ( 0x49 >> ( 4 - shift - mcode ) ) & 3 ) << block );
			_shifts_h2m += ( ( ( 0x49 >> ( 4 - shift + mcode ) ) & 3 ) << block );
        }
        shifts_m2h[ shift ] = _shifts_m2h;
        shifts_h2m[ shift ] = _shifts_h2m;
    }
    ++bp;
}

//  shift   mcode   rotate  out == ( shift + mcode ) % 3
//  0       0       7       0
//  0       1       6       1
//  0       2       5       2
//  1       0       6       1
//  1       1       5       2
//  1       2       4       0
//  2       0       5       2
//  2       1       4       0
//  2       2       3       1

//  shift   mcode   rotate  out ==
//  0       0       4       0
//  0       1       5       2
//  0       2       6       1
//  1       0       3       1
//  1       1       4       0
//  1       2       5       2
//  2       0       2       2
//  2       1       3       1
//  2       2       4       0

void calcSigns()
{
    uint32_t sign1_m2h[ 3 ][ 4 ];
    uint32_t sign2_m2h[ 3 ][ 4 ];
    uint32_t sign3_m2h[ 3 ][ 4 ];
    uint32_t sign4_m2h[ 3 ][ 4 ];
    for( uint32_t shift = 0; shift < 3; ++shift )
    {
        for( uint32_t signs = 0; signs < 4; ++signs )
        {
            uint32_t _signs_old = ( ( 0x5630 >> ( signs << 2 ) ) & 0x0007 );
            uint32_t _sign1_m2h = 0;
            uint32_t _sign2_m2h = 0;
            uint32_t _sign3_m2h = 0;
            uint32_t _sign4_m2h = 0;
            for( uint32_t hcode = 0; hcode < 8; ++hcode )
            {
                uint32_t block = ( hcode << 2 );
			    uint32_t mcode = ( ( 0x20212021 >> block ) & 3 );
			    uint32_t _shift_m2h = ( ( 0x48 >> ( 7 - shift - mcode ) ) & 3 );
                uint32_t _signs_now = ( ( ( ( _signs_old | ( _signs_old << 3 ) ) >> mcode ) ^ ( 0x53560300 >> block ) ) & 7 );
                uint32_t _signs_alt = ( ( ( ( _signs_now | ( _signs_now << 3 ) ) << _shift_m2h ) >> 3 ) & 7 );
			    _sign1_m2h += ( _signs_now << block );
			    _sign2_m2h += ( _signs_alt << block );
			    _sign3_m2h += ( ( _signs_now ^ _signs_old ) << block );
			    _sign4_m2h += ( ( _signs_alt ^ _signs_old ) << block );
            }
            sign1_m2h[ shift ][ signs ] = _sign1_m2h;
            sign2_m2h[ shift ][ signs ] = _sign2_m2h;
            sign3_m2h[ shift ][ signs ] = _sign3_m2h;
            sign4_m2h[ shift ][ signs ] = _sign4_m2h;
        }
    }
    ++bp;
}

//  shift
//  0       0x20212021
//  1       0x01020102
//  2       0x12101210
  

void test()
{
    if( !ValidateHilbertFunc() )
    {
        ++bp;
    }
    if( !CompareHilbertFuncs() )
    {
        ++bp;
    }
    calcShifts();
    calcSigns();

    uint32_t shifts[ 3 ] = { 0x00120001, 0x11201112, 0x22012220 };
    //uint32_t shifts[ 3 ] = { 0x00210002, 0x11021110, 0x22102221 };

    for( uint32_t shift = 0; shift < 3; ++shift )
    {
        uint32_t remap = 0;
        for( uint32_t hcode = 0; hcode < 8; ++hcode )
        {
			remap |= ( ( ( 0x48 >> ( 7 - shift - ( ( 0x20212021 >> hcode ) & 3 ) ) ) & 3 ) << ( hcode << 2 ) );
			//remap |= ( ( ( 0x48 >> ( 4 - shift + ( ( 0x20212021 >> hcode ) & 3 ) ) ) & 3 ) << ( hcode << 2 ) );
        }
        shifts[ shift ] = remap;
    }
    uint32_t shifting = ( shifts[ 2 ] | ( shifts[ 0 ] << 2 ) );     // 0x22492224, 0x22942229
    for( uint32_t shift = 0; shift < 3; ++shift )
    {
        shifts[ shift ] = ( ( shifting >> ( 2 - shift ) ) & 0x33333333 );
    }



    uint32_t remaps_m2h[ 12 ] = { 0x54672310, 0x76450132, 0x10236754, 0x32014576, 0x56214730, 0x12650374, 0x30472156, 0x74036512, 0x52436170, 0x34250716, 0x70614352, 0x16072534 };
    uint32_t remaps_h2m[ 12 ] = { 0x45762310, 0x67540132, 0x10237645, 0x32015467, 0x47215630, 0x12740365, 0x30562147, 0x65037412, 0x42537160, 0x35240617, 0x60715342, 0x17062435 };
    remaps_m2h[  0 ] = 0x76543210;
    remaps_m2h[  1 ] = ( remaps_m2h[ 0 ] ^ 0x33333333 );
    remaps_m2h[  2 ] = ( remaps_m2h[ 0 ] ^ 0x66666666 );
    remaps_m2h[  3 ] = ( remaps_m2h[ 0 ] ^ 0x55555555 );
    remaps_m2h[  4 ] = ( ( ( remaps_m2h[ 0 ] & 0x33333333 ) << 1 ) | ( ( remaps_m2h[ 0 ] & 0x44444444 ) >> 2 ) );
    remaps_m2h[  5 ] = ( ( ( remaps_m2h[ 1 ] & 0x33333333 ) << 1 ) | ( ( remaps_m2h[ 1 ] & 0x44444444 ) >> 2 ) );
    remaps_m2h[  6 ] = ( ( ( remaps_m2h[ 2 ] & 0x33333333 ) << 1 ) | ( ( remaps_m2h[ 2 ] & 0x44444444 ) >> 2 ) );
    remaps_m2h[  7 ] = ( ( ( remaps_m2h[ 3 ] & 0x33333333 ) << 1 ) | ( ( remaps_m2h[ 3 ] & 0x44444444 ) >> 2 ) );
    remaps_m2h[  8 ] = ( ( ( remaps_m2h[ 4 ] & 0x33333333 ) << 1 ) | ( ( remaps_m2h[ 4 ] & 0x44444444 ) >> 2 ) );
    remaps_m2h[  9 ] = ( ( ( remaps_m2h[ 5 ] & 0x33333333 ) << 1 ) | ( ( remaps_m2h[ 5 ] & 0x44444444 ) >> 2 ) );
    remaps_m2h[ 10 ] = ( ( ( remaps_m2h[ 6 ] & 0x33333333 ) << 1 ) | ( ( remaps_m2h[ 6 ] & 0x44444444 ) >> 2 ) );
    remaps_m2h[ 11 ] = ( ( ( remaps_m2h[ 7 ] & 0x33333333 ) << 1 ) | ( ( remaps_m2h[ 7 ] & 0x44444444 ) >> 2 ) );
    for( uint32_t a = 0; a < 12; ++a )
    {
        uint32_t m2h = remaps_m2h[ a ];
    	m2h ^= ( ( m2h >> 1 ) & 0x22222222 );
    	m2h ^= ( ( m2h & 0x22222222 ) >> 1 );
        remaps_m2h[ a ] = m2h;
        uint32_t h2m = 0;
        for( uint32_t b = 0; b < 8; ++b )
        {
            h2m |= ( b << ( ( ( m2h >> ( b << 2 ) ) & 7 ) << 2 ) );
        }
        remaps_h2m[ a ] = h2m;
    }
    uint32_t idxs_m2h[ 12 ];
    uint32_t idxs_h2m[ 12 ];
    uint32_t shrs_m2h[ 12 ];
    uint32_t shrs_h2m[ 12 ];
    uint32_t xors_m2h[ 12 ];
    uint32_t xors_h2m[ 12 ];
    {
        uint32_t m2h_lo[ 12 ];
        uint32_t m2h_hi[ 12 ];
        uint32_t h2m_lo[ 12 ];
        uint32_t h2m_hi[ 12 ];
        for( uint32_t a = 0; a < 12; ++a )
        {
            uint32_t _m2h_lo = 0;
            uint32_t _m2h_hi = 0;
            uint32_t _h2m_lo = 0;
            uint32_t _h2m_hi = 0;
            for( uint32_t b = 0; b < 8; ++b )
            {
                uint32_t c = ( ( a << 3 ) + b );
                _m2h_lo |= ( ( mortonToHilbertTable[ c ] & 7 ) << ( b << 2 ) );
                _m2h_hi |= ( ( mortonToHilbertTable[ c ] >> 3 ) << ( b << 2 ) );
                _h2m_lo |= ( ( hilbertToMortonTable[ c ] & 7 ) << ( b << 2 ) );
                _h2m_hi |= ( ( hilbertToMortonTable[ c ] >> 3 ) << ( b << 2 ) );
            }
            m2h_lo[ a ] = _m2h_lo;
            m2h_hi[ a ] = _m2h_hi;
            h2m_lo[ a ] = _h2m_lo;
            h2m_hi[ a ] = _h2m_hi;
        }
        uint32_t m2h_indices[ 12 ];
        uint32_t h2m_indices[ 12 ];
        for( uint32_t a = 0; a < 12; ++a )
        {
            uint32_t _m2h_lo = m2h_lo[ a ];
            uint32_t _h2m_lo = h2m_lo[ a ];
            for( uint32_t b = 0; b < 12; ++b )
            {
                if( _m2h_lo == remaps_m2h[ b ] )
                {
                    m2h_indices[ a ] = b;
                }
                if( _h2m_lo == remaps_h2m[ b ] )
                {
                    h2m_indices[ a ] = b;
                }
            }
        }
        for( uint32_t a = 0; a < 12; ++a )
        {
            uint32_t _m2h_hi = m2h_hi[ a ];
            uint32_t _h2m_hi = h2m_hi[ a ];
            for( uint32_t b = 0; b < 8; ++b )
            {
                _m2h_hi = ( ( _m2h_hi << 4 ) | m2h_indices[ ( _m2h_hi >> 28 ) & 7 ] );
                _h2m_hi = ( ( _h2m_hi << 4 ) | h2m_indices[ ( _h2m_hi >> 28 ) & 7 ] );
            }
            idxs_m2h[ m2h_indices[ a ] ] = _m2h_hi;
            idxs_h2m[ h2m_indices[ a ] ] = _h2m_hi;
            shrs_m2h[ m2h_indices[ a ] ] = ( ( _m2h_hi >> 2 ) & 0x33333333 );
            shrs_h2m[ h2m_indices[ a ] ] = ( ( _h2m_hi >> 2 ) & 0x33333333 );
            xors_m2h[ m2h_indices[ a ] ] = ( _m2h_hi & 0x33333333 );
            xors_h2m[ h2m_indices[ a ] ] = ( _h2m_hi & 0x33333333 );
        }
    }




    uint32_t used = 0;
    for( uint32_t l = 0; l < 8; ++l )
    {
        for( uint32_t k = 0; k < 8; ++k )
        {
            for( uint32_t j = 0; j < 8; ++j )
            {
                uint32_t check = 0;
                for( uint32_t i = 0; i < 8; ++i )
                {
                    check |= ( ( MortonToHilbert3D( ( ( ( ( ( ( l << 3 ) + k ) << 3 ) + j ) << 3 ) + i ), 4 ) & 7 ) << ( i << 2 ) );
                }
                bool found = false;
                for( uint32_t i = 0; i < 12; ++i )
                {
                    if( check == remaps_m2h[ i ] )
                    {
                        found = true;
                        used |= ( uint32_t( 1 ) << i );
                        break;
                    }
                }
                if( !found )
                {
                    ++bp;
                }
            }
        }
    }
    ++bp;


//  shift: 0, 1, 2
//  signs: 0, 3, 5, 6

//  only 12 possibilities of conversion


}


// each row has only 5 possible targets
// need to convert to shifts and nots, need to find if shifts and nots can be absolute or cumulative

//  m2h_hi
//  0x93954346
//  0x79386638
//  0xaa6639b9
//  0x1707a202
//  0xaa667870
//  0xb079b0aa
//  0xb011b024
//  0x34b41155
//  0x919b4a4b
//  0x28201155
//  0x55382438
//  0x87578262

//  m2h_lo
//  0x54672310
//  0x70614352
//  0x12650374
//  0x76450132
//  0x56214730
//  0x16072534
//  0x52436170
//  0x74036512
//  0x10236754
//  0x30472156
//  0x34250716
//  0x32014576

//  h2m_hi
//  0x59933446
//  0x73366889
//  0xb6699aa3
//  0x1770022a
//  0x8aa77660
//  0x900aabb7
//  0x2bb11004
//  0x3114455b
//  0xa44bb991
//  0x05522118
//  0x48855332
//  0x62288775

//
//  h2m_lo
//  0x45762310
//  0x75132046
//  0x15402673
//  0x76451023
//  0x26731540
//  0x46201375
//  0x13754620
//  0x73264015
//  0x23104576
//  0x40157326
//  0x20467513
//  0x10237645


//  m2h_lo
//  0x54672310
//  0x70614352
//  0x12650374
//  0x76450132
//  0x56214730
//  0x16072534
//  0x52436170
//  0x74036512
//  0x10236754
//  0x30472156
//  0x34250716
//  0x32014576

//        0x54672310,         //  0     -   0
//        0x76450132,         //  3     -   1
//        0x10236754,         //  2
//        0x32014576,         //  3
//        0x56214730,         //  4
//        0x12650374,         //  2     -   5
//        0x30472156,         //  6
//        0x74036512,         //  7
//        0x52436170,         //  8
//        0x34250716,         //  9
//        0x70614352,         //  1     -   10
//        0x16072534 };       //  11

}   //  namespace HilbertTest

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    application entry point
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow )
{
    UNUSED( hInstance );
    UNUSED( hPrevInstance );
    UNUSED( lpCmdLine );
    UNUSED( nCmdShow );

    //HilbertTest::test();
    //return( 0 );

    LOG_INFO( "Game started" );
    platform_windows::CEngineX Game( "Locality" );
    WPARAM wndCode = Game.WndCode();
    LOG_INFO( "Game exited" );
    return( static_cast< int >( wndCode ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

