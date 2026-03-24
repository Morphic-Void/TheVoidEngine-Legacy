
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   crc.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Cyclic redundancy check calculation class.
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

#include "crc.h"
#include "libs/system/debug/asserts.h"
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
////    CRC class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	CalcCRC ControlFlags bits and masks:

const uint32_t UniCRC::CRC_08BITS = 0x00000000;			//	CRC is 8 bit
const uint32_t UniCRC::CRC_16BITS = 0x00000008;			//	CRC is 16 bit
const uint32_t UniCRC::CRC_24BITS = 0x00000010;			//	CRC is 24 bit
const uint32_t UniCRC::CRC_32BITS = 0x00000018;			//	CRC is 32 bit
const uint32_t UniCRC::CRC_ALTPLY = 0x00000020;			// 	use alternative source polynomial
const uint32_t UniCRC::CRC_REVPLY = 0x00000001;			//	Reverse poly
const uint32_t UniCRC::CRC_RFLPLY = 0x00000002;			//	Reflect poly ( reflecting a reversed poly is valid )
const uint32_t UniCRC::CRC_REVDAT = 0x00000004;			//	Reverse data ( also reverses the poly )
const uint32_t UniCRC::CRC_REVCRC = 0x00000040;			//	Reverse output CRC
const uint32_t UniCRC::CRC_INVCRC = 0x00000080;			//	Invert the output CRC ( and the input )
const uint32_t UniCRC::CRC_BITMSK = 0x00000018;			//	CRC size mask
const uint32_t UniCRC::CRC_PLYSEL = 0x0000003f;			//	Polynomial selection
const uint32_t UniCRC::CRC_OPTMSK = 0x000000ff;			//	CRC options mask

const uint32_t UniCRC::mk_Polynomials[ 64 ] =
{
	0x00000007, 0x000000e0, 0x000000c1, 0x0000000f, 0x000000e0, 0x00000007, 0x00000083, 0x000000f0,
	0x00001021, 0x00008408, 0x00000811, 0x00002043, 0x00008408, 0x00001021, 0x00008810, 0x0000c204,
	0x00864cfb, 0x00df3261, 0x00be64c3, 0x000c99f7, 0x00df3261, 0x00864cfb, 0x00c3267d, 0x00ef9930,
	0x04c11db7, 0xedb88320, 0xdb710641, 0x09823b6f, 0xedb88320, 0x04c11db7, 0x82608edb, 0xf6dc4190,
	0x00000031, 0x0000008c, 0x00000019, 0x00000063, 0x0000008c, 0x00000031, 0x00000098, 0x000000c6,
	0x00008005, 0x0000a001, 0x00004003, 0x0000000b, 0x0000a001, 0x00008005, 0x0000c002, 0x0000d000,
	0x00864cfb, 0x00df3261, 0x00be64c3, 0x000c99f7, 0x00df3261, 0x00864cfb, 0x00c3267d, 0x00ef9930,
	0x1edc6f41, 0x82f63b78, 0x05ec76f1, 0x3db8de83, 0x82f63b78, 0x1edc6f41, 0x8f6e37a0, 0xc17b1dbc
};

const uint32_t UniCRC::mk_ControlFlags[ CRC_TYPE_COUNT ] =
{
	( CRC_32BITS | CRC_REVDAT | CRC_INVCRC ),	//	CRC_CCITT_CRC_32 and CRC_DEFAULT (checked and valid)
	( CRC_32BITS | CRC_REVDAT | CRC_INVCRC ),	//	CRC_ETHERNET_32 (unchecked)
	( CRC_32BITS | CRC_REVDAT | CRC_INVCRC ),	//	CRC_AUTODIN_II (unchecked)
	( CRC_32BITS | CRC_REVDAT ),				//	CRC_JAM_CRC_32 (checked and valid)
	( CRC_32BITS | CRC_REVDAT | CRC_INVCRC ),	//	CRC_ADCCP_32 (checked and valid)
	( CRC_32BITS | CRC_REVDAT | CRC_INVCRC ),	//	CRC_CRC_32 (checked and valid)
	( CRC_32BITS | CRC_REVDAT | CRC_INVCRC ),	//	CRC_PKZIP (unchecked)
	( CRC_32BITS | CRC_REVDAT | CRC_INVCRC ),	//	CRC_FDDI (unchecked)
	( CRC_24BITS | CRC_REVDAT ),				//	CRC_MIL188 (unchecked)
	( CRC_16BITS ),								//	CRC_CCITT_CRC_16 (checked and valid)
	( CRC_16BITS | CRC_REVDAT ),				//	CRC_BISYNC_CRC_16 (unchecked)
	( CRC_16BITS | CRC_REVDAT ),				//	CRC_ADCCP_16 (unchecked)
	( CRC_16BITS | CRC_ALTPLY | CRC_REVDAT ),	//	CRC_CRC_16 (checked and valid)
	( CRC_16BITS | CRC_REVPLY | CRC_REVDAT ),	//	CRC_XMODEM (checked and valid)
	( CRC_16BITS ),								//	CRC_ZMODEM (checked and valid)
	( CRC_16BITS | CRC_REVPLY | CRC_REVDAT ),	//	CRC_KERMIT (checked and valid)
	( CRC_16BITS | CRC_REVDAT ),				//	CRC_SDLC (unchecked)
	( CRC_16BITS | CRC_REVDAT ),				//	CRC_HDLC (unchecked)
	( CRC_16BITS | CRC_ALTPLY | CRC_REVDAT ),	//	CRC_ARC (checked and valid)
	( CRC_16BITS | CRC_ALTPLY | CRC_REVDAT ),	//	CRC_LHA (checked and valid)
	( CRC_16BITS ),								//	CRC_X25 (unchecked)
	( CRC_16BITS | CRC_REVDAT ),				//	CRC_X25REV (unchecked)
	( CRC_08BITS ),								//	CRC_HEARD_CRC_08 (unchecked)
};

const uint32_t UniCRC::mk_InitialValues[ CRC_TYPE_COUNT ] =
{
	0x00000000,									//	CRC_CCITT_CRC_32 and CRC_DEFAULT (checked and valid)
	0x00000000,									//	CRC_ETHERNET_32 (unchecked)
	0x00000000,									//	CRC_AUTODIN_II (unchecked)
	0xffffffff,									//	CRC_JAM_CRC_32 (checked and valid)
	0x00000000,									//	CRC_ADCCP_32 (checked and valid)
	0x00000000,									//	CRC_CRC_32 (checked and valid)
	0x00000000,									//	CRC_PKZIP (unchecked)
	0x00000000,									//	CRC_FDDI (unchecked)
	0x00000000,									//	CRC_MIL188 (unchecked)
	0x0000ffff,									//	CRC_CCITT_CRC_16 (checked and valid)
	0x0000ffff,									//	CRC_BISYNC_CRC_16 (unchecked)
	0x00000000,									//	CRC_ADCCP_16 (unchecked)
	0x00000000,									//	CRC_CRC_16 (checked and valid)
	0x00000000,									//	CRC_XMODEM (checked and valid)
	0x00000000,									//	CRC_ZMODEM (checked and valid)
	0x00000000,									//	CRC_KERMIT (checked and valid)
	0x00000000,									//	CRC_SDLC (unchecked)
	0x00000000,									//	CRC_HDLC (unchecked)
	0x00000000,									//	CRC_ARC (checked and valid)
	0x00000000,									//	CRC_LHA (checked and valid)
	0x0000ffff,									//	CRC_X25 (unchecked)
	0x0000ffff,									//	CRC_X25REV (unchecked)
	0x00000000,									//	CRC_HEARD_CRC_08 (unchecked)
};

//! initialization for general CRC calculator
void UniCRC::SetType( const CRC_TYPE type )
{
	ASSERT( static_cast< uint >( type ) < CRC_TYPE_COUNT );
	m_type = ( ( static_cast< uint >( type ) < CRC_TYPE_COUNT ) ? type : CRC_DEFAULT );
	uint32_t flags = mk_ControlFlags[ static_cast< uint >( m_type ) ];
	uint32_t shift = ( flags & CRC_BITMSK );
	uint32_t bits = ( shift + 8 );
	uint32_t high = ( 1 << ( bits - 1 ) );
	uint32_t mask = ( ( high << 1 ) - 1 );
	uint32_t poly = mk_Polynomials[ flags & CRC_PLYSEL ];
	if( flags & CRC_REVDAT )
	{
		for( uint32_t byte = 0; byte < 256; ++byte )
		{
			uint32_t value = byte;
			for( uint32_t bit = 8; bit; bit-- )
			{
				value = ( ( value & 1 ) ? ( ( value >> 1 ) ^ poly ) : ( value >> 1 ) );
			}
			m_crc[ byte ] = ( value & mask );
		}
	}
	else
	{
		for( uint32_t byte = 0; byte < 256; ++byte )
		{
			uint32_t value = ( byte << shift );
			for( uint32_t bit = 8; bit; bit-- )
			{
				value = ( ( value & high ) ? ( ( value << 1 ) ^ poly ) : ( value << 1 ) );
			}
			m_crc[ byte ] = ( value & mask );
		}
	}
}

//! general CRC calculator
uint32_t UniCRC::ExtCRC( const uint8_t* const data, const uint length, const uint32_t prev ) const
{
	uint32_t flags = mk_ControlFlags[ static_cast< uint >( m_type ) ];
	uint32_t shift = ( flags & CRC_BITMSK );
	uint32_t bits = ( shift + 8 );
	uint32_t high = ( 1 << ( bits - 1 ) );
	uint32_t mask = ( ( high << 1 ) - 1 );
	uint32_t CRC = prev;
	if( flags & CRC_INVCRC )
	{
		CRC = ~CRC;
	}
	if( flags & CRC_REVCRC )
	{
		CRC = ( ( ( CRC >> 1 ) & 0x55555555 ) | ( ( CRC & 0x55555555 ) << 1 ) );
		CRC = ( ( ( CRC >> 2 ) & 0x33333333 ) | ( ( CRC & 0x33333333 ) << 2 ) );
		CRC = ( ( ( CRC >> 4 ) & 0x0f0f0f0f ) | ( ( CRC & 0x0f0f0f0f ) << 4 ) );
		CRC = ( ( ( CRC >> 8 ) & 0x00ff00ff ) | ( ( CRC & 0x00ff00ff ) << 8 ) );
		CRC = ( ( CRC >> 16 ) | ( CRC << 16 ) );
		CRC >>= shift;
	}
	CRC &= mask;
	if( flags & CRC_REVDAT )
	{
		for( uint32_t index = 0; index < length; ++index )
		{
			CRC = ( ( CRC >> 8 ) ^ m_crc[ ( CRC ^ data[ index ] ) & 255 ] );
		}
	}
	else
	{
		for( uint32_t index = 0; index < length; ++index )
		{
			CRC = ( ( CRC << 8 ) ^ m_crc[ ( ( CRC >> shift ) ^ data[ index ] ) & 255 ] );
		}
	}
	if( flags & CRC_INVCRC )
	{
		CRC = ~CRC;
	}
	if( flags & CRC_REVCRC )
	{
		CRC = ( ( ( CRC >> 1 ) & 0x55555555 ) | ( ( CRC & 0x55555555 ) << 1 ) );
		CRC = ( ( ( CRC >> 2 ) & 0x33333333 ) | ( ( CRC & 0x33333333 ) << 2 ) );
		CRC = ( ( ( CRC >> 4 ) & 0x0f0f0f0f ) | ( ( CRC & 0x0f0f0f0f ) << 4 ) );
		CRC = ( ( ( CRC >> 8 ) & 0x00ff00ff ) | ( ( CRC & 0x00ff00ff ) << 8 ) );
		CRC = ( ( CRC >> 16 ) | ( CRC << 16 ) );
		CRC >>= shift;
	}
	CRC &= mask;
    return( CRC );
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
