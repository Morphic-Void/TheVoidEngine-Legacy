
//	File:   endian.h
//	Author: Ritchie Brannan
//	Date:   11 Nov 10
//
//	Description:
//
//		Endian swapping support.

#pragma once

#ifndef	__SYSTEM_ENDIAN_INCLUDED__
#define	__SYSTEM_ENDIAN_INCLUDED__

#include <cstdint>

namespace endian
{

//!	endian swapping functions

inline bool isNativeLE(void)
{
	const uint16_t check = 1;
	return(*reinterpret_cast<const uint8_t*>(&check) ? true : false);
}

inline bool isNativeBE(void)
{
	const uint16_t check = 1;
	return(*reinterpret_cast<const uint8_t*>(&check) ? false : true);
}

inline uint16_t swap16(uint16_t swap)
{
	return((swap >> 8) | (swap << 8));
}

inline uint32_t swap32(uint32_t swap)
{
	swap = (((swap >> 8) & 0x00ff00ff) | ((swap & 0x00ff00ff) << 8));
	return((swap >> 16) | (swap << 16));
}

inline uint64_t swap64(uint64_t swap)
{
	swap = (((swap >> 8) & 0x00ff00ff00ff00fful) | ((swap & 0x00ff00ff00ff00fful) << 8));
	swap = (((swap >> 16) & 0x00000000fffffffful) | ((swap & 0x00000000fffffffful) << 16));
	return((swap >> 32) | (swap << 32));
}

inline uint16_t swapToLE16(const uint16_t& swap)
{	//	little endian systems will return a value that is the same as the swap parameter
	const uint8_t* const bytes = reinterpret_cast<const uint8_t* const>(&swap);
	uint16_t data = 0;
	data += bytes[1];
	data <<= 8;
	data += bytes[0];
	return(data);
}

inline uint32_t swapToLE32(const uint32_t& swap)
{	//	little endian systems will return a value that is the same as the swap parameter
	const uint8_t* const bytes = reinterpret_cast<const uint8_t* const>(&swap);
	uint32_t data = 0;
	data += bytes[3];
	data <<= 8;
	data += bytes[2];
	data <<= 8;
	data += bytes[1];
	data <<= 8;
	data += bytes[0];
	return(data);
}

inline uint64_t swapToLE64(const uint64_t& swap)
{	//	little endian systems will return a value that is the same as the swap parameter
	const uint8_t* const bytes = reinterpret_cast<const uint8_t* const>(&swap);
	uint64_t data = 0;
	data += bytes[7];
	data <<= 8;
	data += bytes[6];
	data <<= 8;
	data += bytes[5];
	data <<= 8;
	data += bytes[4];
	data <<= 8;
	data += bytes[3];
	data <<= 8;
	data += bytes[2];
	data <<= 8;
	data += bytes[1];
	data <<= 8;
	data += bytes[0];
	return(data);
}

inline uint16_t swapToBE16(const uint16_t& swap)
{	//	big endian systems will return a value that is the same as the swap parameter
	const uint8_t* const bytes = reinterpret_cast<const uint8_t* const>(&swap);
	uint16_t data = 0;
	data += bytes[0];
	data <<= 8;
	data += bytes[1];
	return(data);
}

inline uint32_t swapToBE32(const uint32_t& swap)
{	//	big endian systems will return a value that is the same as the swap parameter
	const uint8_t* const bytes = reinterpret_cast<const uint8_t* const>(&swap);
	uint32_t data = 0;
	data += bytes[0];
	data <<= 8;
	data += bytes[1];
	data <<= 8;
	data += bytes[2];
	data <<= 8;
	data += bytes[3];
	return(data);
}

inline uint64_t swapToBE64(const uint64_t& swap)
{	//	big endian systems will return a value that is the same as the swap parameter
	const uint8_t* const bytes = reinterpret_cast<const uint8_t* const>(&swap);
	uint64_t data = 0;
	data += bytes[0];
	data <<= 8;
	data += bytes[1];
	data <<= 8;
	data += bytes[2];
	data <<= 8;
	data += bytes[3];
	data <<= 8;
	data += bytes[4];
	data <<= 8;
	data += bytes[5];
	data <<= 8;
	data += bytes[6];
	data <<= 8;
	data += bytes[7];
	return(data);
}

//!	endian swapping macros

#define	ENDIAN16( d )	endian::swap16( *static_cast< uint16_t* >( &( d ) ) )
#define	ENDIAN32( d )	endian::swap32( *static_cast< uint32_t* >( &( d ) ) )
#define	ENDIAN64( d )	endian::swap64( *static_cast< uint64_t* >( &( d ) ) )

#define	FORCE_LE16( d )	endian::swapToLE16( *static_cast< uint16_t* >( &( d ) ) )
#define	FORCE_LE32( d )	endian::swapToLE32( *static_cast< uint32_t* >( &( d ) ) )
#define	FORCE_LE64( d )	endian::swapToLE64( *static_cast< uint64_t* >( &( d ) ) )

#define	FORCE_BE16( d )	endian::swapToBE16( *static_cast< uint16_t* >( &( d ) ) )
#define	FORCE_BE32( d )	endian::swapToBE32( *static_cast< uint32_t* >( &( d ) ) )
#define	FORCE_BE64( d )	endian::swapToBE64( *static_cast< uint64_t* >( &( d ) ) )

};	//	namespace endian

//!	consts

namespace consts
{
	extern const uint8_t		HIGH_BIT_MASK_8BIT;
	extern const uint16_t		HIGH_BIT_MASK_16BIT;
	extern const uint32_t		HIGH_BIT_MASK_32BIT;
	extern const uint64_t		HIGH_BIT_MASK_64BIT;
	namespace endian
	{
		extern const uint8_t	BYTE0_INDEX_16BIT;
		extern const uint8_t	BYTE1_INDEX_16BIT;
		extern const uint8_t	BYTE0_INDEX_32BIT;
		extern const uint8_t	BYTE1_INDEX_32BIT;
		extern const uint8_t	BYTE2_INDEX_32BIT;
		extern const uint8_t	BYTE3_INDEX_32BIT;
		extern const uint8_t	BYTE0_INDEX_64BIT;
		extern const uint8_t	BYTE1_INDEX_64BIT;
		extern const uint8_t	BYTE2_INDEX_64BIT;
		extern const uint8_t	BYTE3_INDEX_64BIT;
		extern const uint8_t	BYTE4_INDEX_64BIT;
		extern const uint8_t	BYTE5_INDEX_64BIT;
		extern const uint8_t	BYTE6_INDEX_64BIT;
		extern const uint8_t	BYTE7_INDEX_64BIT;
		extern const uint8_t	BYTES_INDEX_16BIT[2];
		extern const uint8_t	BYTES_INDEX_32BIT[4];
		extern const uint8_t	BYTES_INDEX_64BIT[8];
		extern const uint8_t	BYTE0_SHIFT_16BIT;
		extern const uint8_t	BYTE1_SHIFT_16BIT;
		extern const uint8_t	BYTE0_SHIFT_32BIT;
		extern const uint8_t	BYTE1_SHIFT_32BIT;
		extern const uint8_t	BYTE2_SHIFT_32BIT;
		extern const uint8_t	BYTE3_SHIFT_32BIT;
		extern const uint8_t	BYTE0_SHIFT_64BIT;
		extern const uint8_t	BYTE1_SHIFT_64BIT;
		extern const uint8_t	BYTE2_SHIFT_64BIT;
		extern const uint8_t	BYTE3_SHIFT_64BIT;
		extern const uint8_t	BYTE4_SHIFT_64BIT;
		extern const uint8_t	BYTE5_SHIFT_64BIT;
		extern const uint8_t	BYTE6_SHIFT_64BIT;
		extern const uint8_t	BYTE7_SHIFT_64BIT;
		extern const uint8_t	BYTES_SHIFT_16BIT[2];
		extern const uint8_t	BYTES_SHIFT_32BIT[4];
		extern const uint8_t	BYTES_SHIFT_64BIT[8];
		extern const uint16_t	MASK_BYTE0_16BIT;
		extern const uint16_t	MASK_BYTE1_16BIT;
		extern const uint32_t	MASK_BYTE0_32BIT;
		extern const uint32_t	MASK_BYTE1_32BIT;
		extern const uint32_t	MASK_BYTE2_32BIT;
		extern const uint32_t	MASK_BYTE3_32BIT;
		extern const uint64_t	MASK_BYTE0_64BIT;
		extern const uint64_t	MASK_BYTE1_64BIT;
		extern const uint64_t	MASK_BYTE2_64BIT;
		extern const uint64_t	MASK_BYTE3_64BIT;
		extern const uint64_t	MASK_BYTE4_64BIT;
		extern const uint64_t	MASK_BYTE5_64BIT;
		extern const uint64_t	MASK_BYTE6_64BIT;
		extern const uint64_t	MASK_BYTE7_64BIT;
		extern const uint16_t	MASK_BYTES_16BIT[2];
		extern const uint32_t	MASK_BYTES_32BIT[4];
		extern const uint64_t	MASK_BYTES_64BIT[8];
	}
};

#endif	//	#ifndef	__SYSTEM_ENDIAN_INCLUDED__
