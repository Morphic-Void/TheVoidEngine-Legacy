
//  File:   endian.cpp
//  Author: Ritchie Brannan
//  Date:   11 Nov 10
//
//  Description:
//
//  	Endian swapping support.

#include "endian.h"

//! consts

namespace consts
{
    const uint8_t		HIGH_BIT_MASK_8BIT = 0x80;
    const uint16_t		HIGH_BIT_MASK_16BIT = 0x8000;
    const uint32_t		HIGH_BIT_MASK_32BIT = 0x80000000;
    const uint64_t		HIGH_BIT_MASK_64BIT = 0x8000000000000000;
    namespace endian
    {
        const uint16_t	BYTE_ORDER_16BIT = 0x0100;
        const uint32_t	BYTE_ORDER_32BIT = 0x03020100;
        const uint64_t	BYTE_ORDER_64BIT = 0x0706050403020100;
        const uint16_t	BYTE_SHIFT_16BIT = 0x0800;
        const uint32_t	BYTE_SHIFT_32BIT = 0x18100800;
        const uint64_t	BYTE_SHIFT_64BIT = 0x3830282018100800;
        const uint8_t	BYTE0_INDEX_16BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_16BIT)[0];
        const uint8_t	BYTE1_INDEX_16BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_16BIT)[1];
        const uint8_t	BYTE0_INDEX_32BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_32BIT)[0];
        const uint8_t	BYTE1_INDEX_32BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_32BIT)[1];
        const uint8_t	BYTE2_INDEX_32BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_32BIT)[2];
        const uint8_t	BYTE3_INDEX_32BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_32BIT)[3];
        const uint8_t	BYTE0_INDEX_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_64BIT)[0];
        const uint8_t	BYTE1_INDEX_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_64BIT)[1];
        const uint8_t	BYTE2_INDEX_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_64BIT)[2];
        const uint8_t	BYTE3_INDEX_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_64BIT)[3];
        const uint8_t	BYTE4_INDEX_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_64BIT)[4];
        const uint8_t	BYTE5_INDEX_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_64BIT)[5];
        const uint8_t	BYTE6_INDEX_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_64BIT)[6];
        const uint8_t	BYTE7_INDEX_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_ORDER_64BIT)[7];
        const uint8_t	BYTES_INDEX_16BIT[2] = { BYTE0_INDEX_16BIT, BYTE1_INDEX_16BIT };
        const uint8_t	BYTES_INDEX_32BIT[4] = { BYTE0_INDEX_32BIT, BYTE1_INDEX_32BIT, BYTE2_INDEX_32BIT, BYTE3_INDEX_32BIT };
        const uint8_t	BYTES_INDEX_64BIT[8] = { BYTE0_INDEX_64BIT, BYTE1_INDEX_64BIT, BYTE2_INDEX_64BIT, BYTE3_INDEX_64BIT, BYTE4_INDEX_64BIT, BYTE5_INDEX_64BIT, BYTE6_INDEX_64BIT, BYTE7_INDEX_64BIT };
        const uint8_t	BYTE0_SHIFT_16BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_16BIT)[0];
        const uint8_t	BYTE1_SHIFT_16BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_16BIT)[1];
        const uint8_t	BYTE0_SHIFT_32BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_32BIT)[0];
        const uint8_t	BYTE1_SHIFT_32BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_32BIT)[1];
        const uint8_t	BYTE2_SHIFT_32BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_32BIT)[2];
        const uint8_t	BYTE3_SHIFT_32BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_32BIT)[3];
        const uint8_t	BYTE0_SHIFT_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_64BIT)[0];
        const uint8_t	BYTE1_SHIFT_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_64BIT)[1];
        const uint8_t	BYTE2_SHIFT_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_64BIT)[2];
        const uint8_t	BYTE3_SHIFT_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_64BIT)[3];
        const uint8_t	BYTE4_SHIFT_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_64BIT)[4];
        const uint8_t	BYTE5_SHIFT_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_64BIT)[5];
        const uint8_t	BYTE6_SHIFT_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_64BIT)[6];
        const uint8_t	BYTE7_SHIFT_64BIT = reinterpret_cast<const uint8_t* const>(&BYTE_SHIFT_64BIT)[7];
        const uint8_t	BYTES_SHIFT_16BIT[2] = { BYTE0_SHIFT_16BIT, BYTE1_SHIFT_16BIT };
        const uint8_t	BYTES_SHIFT_32BIT[4] = { BYTE0_SHIFT_32BIT, BYTE1_SHIFT_32BIT, BYTE2_SHIFT_32BIT, BYTE3_SHIFT_32BIT };
        const uint8_t	BYTES_SHIFT_64BIT[8] = { BYTE0_SHIFT_64BIT, BYTE1_SHIFT_64BIT, BYTE2_SHIFT_64BIT, BYTE3_SHIFT_64BIT, BYTE4_SHIFT_64BIT, BYTE5_SHIFT_64BIT, BYTE6_SHIFT_64BIT, BYTE7_SHIFT_64BIT };
        const uint16_t	BYTE0_MASK_16BIT = (uint16_t(255) << BYTE0_SHIFT_16BIT);
        const uint16_t	BYTE1_MASK_16BIT = (uint16_t(255) << BYTE1_SHIFT_16BIT);
        const uint32_t	BYTE0_MASK_32BIT = (uint32_t(255) << BYTE0_SHIFT_32BIT);
        const uint32_t	BYTE1_MASK_32BIT = (uint32_t(255) << BYTE1_SHIFT_32BIT);
        const uint32_t	BYTE2_MASK_32BIT = (uint32_t(255) << BYTE2_SHIFT_32BIT);
        const uint32_t	BYTE3_MASK_32BIT = (uint32_t(255) << BYTE3_SHIFT_32BIT);
        const uint64_t	BYTE0_MASK_64BIT = (uint64_t(255) << BYTE0_SHIFT_64BIT);
        const uint64_t	BYTE1_MASK_64BIT = (uint64_t(255) << BYTE1_SHIFT_64BIT);
        const uint64_t	BYTE2_MASK_64BIT = (uint64_t(255) << BYTE2_SHIFT_64BIT);
        const uint64_t	BYTE3_MASK_64BIT = (uint64_t(255) << BYTE3_SHIFT_64BIT);
        const uint64_t	BYTE4_MASK_64BIT = (uint64_t(255) << BYTE4_SHIFT_64BIT);
        const uint64_t	BYTE5_MASK_64BIT = (uint64_t(255) << BYTE5_SHIFT_64BIT);
        const uint64_t	BYTE6_MASK_64BIT = (uint64_t(255) << BYTE6_SHIFT_64BIT);
        const uint64_t	BYTE7_MASK_64BIT = (uint64_t(255) << BYTE7_SHIFT_64BIT);
        const uint16_t	BYTES_MASK_16BIT[2] = { BYTE0_MASK_16BIT, BYTE1_MASK_16BIT };
        const uint32_t	BYTES_MASK_32BIT[4] = { BYTE0_MASK_32BIT, BYTE1_MASK_32BIT, BYTE2_MASK_32BIT, BYTE3_MASK_32BIT };
        const uint64_t	BYTES_MASK_64BIT[8] = { BYTE0_MASK_64BIT, BYTE1_MASK_64BIT, BYTE2_MASK_64BIT, BYTE3_MASK_64BIT, BYTE4_MASK_64BIT, BYTE5_MASK_64BIT, BYTE6_MASK_64BIT, BYTE7_MASK_64BIT };
    };
};
