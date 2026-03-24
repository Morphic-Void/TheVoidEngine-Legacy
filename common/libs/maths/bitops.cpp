
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   bitops.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	utility functions.
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

#include "bitops.h"

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
////    bit reversing
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Reverse8Bit( const unsigned int value )
{
	unsigned int reg1 = value;
	unsigned int reg2 = reg1;
	reg1 <<= 4;
	reg1 &= 0x000000f0;
	reg2 &= 0x000000f0;
	reg2 >>= 4;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 2;
	reg1 &= 0x000000cc;
	reg2 &= 0x000000cc;
	reg2 >>= 2;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 1;
	reg1 &= 0x000000aa;
	reg2 &= 0x000000aa;
	reg2 >>= 1;
	return( reg1 | reg2 );
}

unsigned int Reverse16Bit( const unsigned int value )
{
	unsigned int reg1 = value;
	unsigned int reg2 = reg1;
	reg1 <<= 8;
	reg1 &= 0x0000ff00;
	reg2 &= 0x0000ff00;
	reg2 >>= 8;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 4;
	reg1 &= 0x0000f0f0;
	reg2 &= 0x0000f0f0;
	reg2 >>= 4;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 2;
	reg1 &= 0x0000cccc;
	reg2 &= 0x0000cccc;
	reg2 >>= 2;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 1;
	reg1 &= 0x0000aaaa;
	reg2 &= 0x0000aaaa;
	reg2 >>= 1;
	return( reg1 | reg2 );
}

unsigned int Reverse32Bit( const unsigned int value )
{
	unsigned int reg1 = value;
	unsigned int reg2 = reg1;
	reg1 <<= 16;
	reg2 >>= 16;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 8;
	reg1 &= 0xff00ff00;
	reg2 &= 0xff00ff00;
	reg2 >>= 8;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 4;
	reg1 &= 0xf0f0f0f0;
	reg2 &= 0xf0f0f0f0;
	reg2 >>= 4;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 2;
	reg1 &= 0xcccccccc;
	reg2 &= 0xcccccccc;
	reg2 >>= 2;
	reg1 |= reg2;
	reg2 = reg1;
	reg1 <<= 1;
	reg1 &= 0xaaaaaaaa;
	reg2 &= 0xaaaaaaaa;
	reg2 >>= 1;
	return( reg1 | reg2 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    bit indices
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BitIndex( const unsigned int value )
{
	int index = ( ( ( 0 - ( value >> 16 ) ) >> 16 ) & 16 );
	index += ( ( ( 0 - ( value >> ( index + 8 ) ) ) >> 8 ) & 8 );
	index += ( ( ( 0 - ( value >> ( index + 4 ) ) ) >> 4 ) & 4 );
	index += ( value >> ( index + 2 ) );
	return( index + ( value >> index ) - 1 );
}

int LoBitIndex( const unsigned int value )
{
	return( BitIndex( value & ( 0 - value ) ) );
}

int HiBitIndex( const unsigned int value )
{
	int index = ( ( ( 0 - ( value >> 16 ) ) >> 16 ) & 16 );
	index += ( ( ( 0 - ( value >> ( index + 8 ) ) ) >> 8 ) & 8 );
	index += ( ( ( 0 - ( value >> ( index + 4 ) ) ) >> 4 ) & 4 );
	index += ( ( ( 0 - ( value >> ( index + 2 ) ) ) >> 2 ) & 2 );
	index += ( value >> ( index + 1 ) );
	return( index + ( value >> index ) - 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    morton codes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Morton_2D_Encode_5bit( unsigned int index1, unsigned int index2 )
{
	index1 &= 0x0000001f;
	index2 &= 0x0000001f;
	index1 *= 0x01041041;
	index2 *= 0x01041041;
	index1 &= 0x10204081;
	index2 &= 0x10204081;
	index1 *= 0x00108421;
	index2 *= 0x00108421;
	index1 &= 0x15500000;
	index2 &= 0x15500000;
	return( ( index1 >> 20 ) | ( index2 >> 19 ) );
}

void Morton_2D_Decode_5bit( const unsigned int morton, unsigned int& index1, unsigned int& index2 )
{
	unsigned int value1 = morton;
	unsigned int value2 = ( value1 >> 1 );
	value1 &= 0x00000155;
	value2 &= 0x00000155;
	value1 |= ( value1 >> 1 );
	value2 |= ( value2 >> 1 );
	value1 &= 0x00000133;
	value2 &= 0x00000133;
	value1 |= ( value1 >> 2 );
	value2 |= ( value2 >> 2 );
	value1 &= 0x0000010f;
	value2 &= 0x0000010f;
	value1 |= ( value1 >> 4 );
	value2 |= ( value2 >> 4 );
	value1 &= 0x0000001f;
	value2 &= 0x0000001f;
	index1 = value1;
	index2 = value2;
}

unsigned int Morton_2D_Index1_5bit( unsigned int morton )
{
	morton &= 0x00000155;
	morton |= ( morton >> 1 );
	morton &= 0x00000133;
	morton |= ( morton >> 2 );
	morton &= 0x0000010f;
	return( ( morton & 0x0000000f ) | ( morton >> 4 ) );
}

unsigned int Morton_2D_Encode_16bit( unsigned int index1, unsigned int index2 )
{
	index1 &= 0x0000ffff;
	index2 &= 0x0000ffff;
	index1 |= ( index1 << 8 );
	index2 |= ( index2 << 8 );
	index1 &= 0x00ff00ff;
	index2 &= 0x00ff00ff;
	index1 |= ( index1 << 4 );
	index2 |= ( index2 << 4 );
	index1 &= 0x0f0f0f0f;
	index2 &= 0x0f0f0f0f;
	index1 |= ( index1 << 2 );
	index2 |= ( index2 << 2 );
	index1 &= 0x33333333;
	index2 &= 0x33333333;
	index1 |= ( index1 << 1 );
	index2 |= ( index2 << 1 );
	index1 &= 0x55555555;
	index2 &= 0x55555555;
	return( index1 | ( index2 << 1 ) );
}

void Morton_2D_Decode_16bit( const unsigned int morton, unsigned int& index1, unsigned int& index2 )
{
	unsigned int value1 = morton;
	unsigned int value2 = ( value1 >> 1 );
	value1 &= 0x55555555;
	value2 &= 0x55555555;
	value1 |= ( value1 >> 1 );
	value2 |= ( value2 >> 1 );
	value1 &= 0x33333333;
	value2 &= 0x33333333;
	value1 |= ( value1 >> 2 );
	value2 |= ( value2 >> 2 );
	value1 &= 0x0f0f0f0f;
	value2 &= 0x0f0f0f0f;
	value1 |= ( value1 >> 4 );
	value2 |= ( value2 >> 4 );
	value1 &= 0x00ff00ff;
	value2 &= 0x00ff00ff;
	value1 |= ( value1 >> 8 );
	value2 |= ( value2 >> 8 );
	value1 &= 0x0000ffff;
	value2 &= 0x0000ffff;
	index1 = value1;
	index2 = value2;
}

unsigned int Morton_2D_Index1_16bit( unsigned int morton )
{
	morton &= 0x55555555;
	morton |= ( morton >> 1 );
	morton &= 0x33333333;
	morton |= ( morton >> 2 );
	morton &= 0x0f0f0f0f;
	morton |= ( morton >> 4 );
	morton &= 0x00ff00ff;
	morton |= ( morton >> 8 );
	morton &= 0x0000ffff;
	return( morton );
}

unsigned int Morton_3D_Encode_5bit( unsigned int index1, unsigned int index2, unsigned int index3 )
{
	index1 &= 0x0000001f;
	index2 &= 0x0000001f;
	index3 &= 0x0000001f;
	index1 *= 0x01041041;
	index2 *= 0x01041041;
	index3 *= 0x01041041;
	index1 &= 0x10204081;
	index2 &= 0x10204081;
	index3 &= 0x10204081;
	index1 *= 0x00011111;
	index2 *= 0x00011111;
	index3 *= 0x00011111;
	index1 &= 0x12490000;
	index2 &= 0x12490000;
	index3 &= 0x12490000;
	return( ( index1 >> 16 ) | ( index2 >> 15 ) | ( index3 >> 14 ) );
}

void Morton_3D_Decode_5bit( const unsigned int morton, unsigned int& index1, unsigned int& index2, unsigned int& index3 )
{
	unsigned int value1 = morton;
	unsigned int value2 = ( value1 >> 1 );
	unsigned int value3 = ( value1 >> 2 );
	value1 &= 0x00001249;
	value2 &= 0x00001249;
	value3 &= 0x00001249;
	value1 |= ( value1 >> 2 );
	value2 |= ( value2 >> 2 );
	value3 |= ( value3 >> 2 );
	value1 &= 0x000010c3;
	value2 &= 0x000010c3;
	value3 &= 0x000010c3;
	value1 |= ( value1 >> 4 );
	value2 |= ( value2 >> 4 );
	value3 |= ( value3 >> 4 );
	value1 &= 0x0000100f;
	value2 &= 0x0000100f;
	value3 &= 0x0000100f;
	value1 |= ( value1 >> 8 );
	value2 |= ( value2 >> 8 );
	value3 |= ( value3 >> 8 );
	value1 &= 0x0000001f;
	value2 &= 0x0000001f;
	value3 &= 0x0000001f;
	index1 = value1;
	index2 = value2;
	index3 = value3;
}

unsigned int Morton_3D_Index1_5bit( unsigned int morton )
{
	morton &= 0x00001249;
	morton |= ( morton >> 2 );
	morton &= 0x000010c3;
	morton |= ( morton >> 4 );
	morton &= 0x0000100f;
	return( ( morton & 0x0000000f ) | ( morton >> 8 ) );
}

unsigned int Morton_3D_Encode_10bit( unsigned int index1, unsigned int index2, unsigned int index3 )
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

unsigned int Morton_3D_Index1_10bit( unsigned int morton )
{
	morton &= 0x09249249;
	morton |= ( morton >> 2 );
	morton &= 0x030c30c3;
	morton |= ( morton >> 4 );
	morton &= 0x0300f00f;
	morton |= ( morton >> 8 );
	morton &= 0x030000ff;
	morton |= ( morton >> 16 );
	morton &= 0x000003ff;
	return( morton );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    hilbert code
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int MortonToHilbert2D( const unsigned int morton, const unsigned int bits )
{
	unsigned int index = 0;
	unsigned int remap = 0xb4;
	unsigned int block = ( bits << 1 );
	while( block )
	{
		block -= 2;
		unsigned int mcode = ( ( morton >> block ) & 3 );
		unsigned int hcode = ( ( remap >> ( mcode << 1 ) ) & 3 );
		remap ^= ( 0x82000028 >> ( hcode << 3 ) );
		index = ( ( index << 2 ) + hcode );
	}
	return( index );
}

unsigned int HilbertToMorton2D( const unsigned int hilbert, const unsigned int bits )
{
	unsigned int index = 0;
	unsigned int remap = 0xb4;
	unsigned int block = ( bits << 1 );
	while( block )
	{
		block -= 2;
		unsigned int hcode = ( ( hilbert >> block ) & 3 );
		unsigned int mcode = ( ( remap >> ( hcode << 1 ) ) & 3 );
		remap ^= ( 0x330000cc >> ( hcode << 3 ) );
		index = ( ( index << 2 ) + mcode );
	}
	return( index );
}

unsigned int MortonToHilbert2D_Auto( const unsigned int morton )
{
	unsigned int hilbert = 0;
	unsigned int remap = 0xb4;
	unsigned int block = ( ( ( 0 - ( morton >> 16 ) ) >> 16 ) & 16 );
	block += ( ( ( 0 - ( morton >> ( block + 8 ) ) ) >> 8 ) & 8 );
	block += ( ( ( 0 - ( morton >> ( block + 4 ) ) ) >> 4 ) & 4 );
	block += 2;
	block += ( ( ( 0 - ( morton >> block ) ) >> 2 ) & 2 );
	block |= 2;
	block += 2;
	while( block )
	{
		block -= 2;
		unsigned int mcode = ( ( morton >> block ) & 3 );
		unsigned int hcode = ( ( remap >> ( mcode << 1 ) ) & 3 );
		remap ^= ( 0x82000028 >> ( hcode << 3 ) );
		hilbert = ( ( hilbert << 2 ) + hcode );
	}
	return( hilbert );
}

unsigned int HilbertToMorton2D_Auto( const unsigned int hilbert )
{
	unsigned int morton = 0;
	unsigned int remap = 0xb4;
	unsigned int block = ( ( ( 0 - ( hilbert >> 16 ) ) >> 16 ) & 16 );
	block += ( ( ( 0 - ( hilbert >> ( block + 8 ) ) ) >> 8 ) & 8 );
	block += ( ( ( 0 - ( hilbert >> ( block + 4 ) ) ) >> 4 ) & 4 );
	block += 2;
	block += ( ( ( 0 - ( hilbert >> block ) ) >> 2 ) & 2 );
	block |= 2;
	block += 2;
	while( block )
	{
		block -= 2;
		unsigned int hcode = ( ( hilbert >> block ) & 3 );
		unsigned int mcode = ( ( remap >> ( hcode << 1 ) ) & 3 );
		remap ^= ( 0x330000cc >> ( hcode << 3 ) );
		morton = ( ( morton << 2 ) + mcode );
	}
	return( morton );
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

unsigned int MortonToHilbert3D_Auto( const unsigned int morton )
{
	unsigned int index = morton;
	if( index )
	{
		unsigned int block = ( ( ( 0 - ( index >> 16 ) ) >> 16 ) & 16 );
		block += ( ( ( 0 - ( index >> ( block + 8 ) ) ) >> 8 ) & 8 );
		block += ( ( ( 0 - ( index >> ( block + 4 ) ) ) >> 4 ) & 4 );
		block += ( ( ( 0 - ( index >> ( block + 2 ) ) ) >> 2 ) & 2 );
		block += ( index >> ( block + 1 ) );
		block = ( ( block + 8 ) / 9 );
        block = ( ( block * 9 ) - 3 );
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

unsigned int HilbertToMorton3D_Auto( const unsigned int hilbert )
{
	unsigned int index = hilbert;
	index ^= ( ( index & 0x92492492 ) >> 1 );
	index ^= ( ( index >> 1 ) & 0x92492492 );
	if( index )
	{
		unsigned int block = ( ( ( 0 - ( index >> 16 ) ) >> 16 ) & 16 );
		block += ( ( ( 0 - ( index >> ( block + 8 ) ) ) >> 8 ) & 8 );
		block += ( ( ( 0 - ( index >> ( block + 4 ) ) ) >> 4 ) & 4 );
		block += ( ( ( 0 - ( index >> ( block + 2 ) ) ) >> 2 ) & 2 );
		block += ( index >> ( block + 1 ) );
		block = ( ( block + 8 ) / 9 );
		block = ( ( block * 9 ) - 3 );
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
