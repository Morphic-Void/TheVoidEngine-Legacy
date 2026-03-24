
//	File:   type_info.h
//	Author: Ritchie Brannan
//	Date:   11 Nov 10
//
//	Description:
//
//		Standard type descriptions.

#pragma once

#ifndef	__SYSTEM_TYPE_INFO_INCLUDED__
#define	__SYSTEM_TYPE_INFO_INCLUDED__

#include "types.h"

//!	type info template

template< typename T >
struct type_desc
{
	static const int	bytes = sizeof( T );
	static const int	bits = ( sizeof( T ) << 3 );
	static const int	high_bit_index = ( ( sizeof( T ) << 3 ) - 1 );
};

//!	consts

namespace consts
{
	extern const type_desc< char >		char_info;
	extern const type_desc< short >		short_info;
	extern const type_desc< long >		long_info;
	extern const type_desc< long long >	long_long_info;
	extern const type_desc< int >		int_info;
	extern const type_desc< float >		float_info;
	extern const type_desc< double >	double_info;
	extern const type_desc< int8_t >	int8_info;
	extern const type_desc< uint8_t >	uint8_info;
	extern const type_desc< int16_t >	int16_info;
	extern const type_desc< uint16_t >	uint16_info;
	extern const type_desc< int32_t >	int32_info;
	extern const type_desc< uint32_t >	uint32_info;
	extern const type_desc< float32_t >	float32_info;
	extern const type_desc< uint64_t >	uint64_info;
	extern const type_desc< int64_t >	int64_info;
	extern const type_desc< float64_t >	float64_info;
	extern const type_desc< intptr_t >	intptr_info;
	extern const type_desc< uintptr_t >	uintptr_info;
};

#endif	//	#ifndef	__SYSTEM_TYPE_INFO_INCLUDED__

