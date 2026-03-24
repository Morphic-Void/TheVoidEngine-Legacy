
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   rle.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Run-Length Encoding (RLE) compression and decompression functions.
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

#include "rle.h"
#include "libs/system/debug/asserts.h"
#include <memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin compression namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace compression
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin rle namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rle
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Run-Length Encoding (RLE) compression and decompression functions.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!	Return the maximum buffer size required to encode the specified amount of data.
uint32_t MaxEncodedSize( const uint32_t MaxCopySize, const uint32_t DecodedSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	return( DecodedSize + ( ( DecodedSize + MaxCopySize - 2 ) / MaxCopySize ) + 4 );
}

//!	Encodes data as RLE compressed.
//!	Returns RLE_SUCCESS if successful.
//!	If successful, EncodedSize will contain the actual size of the required encoding buffer.
//!	Specifying a non-zero EncodedSize indicates that the actual encoded size should be error checked against this value.
RLE_ERROR Encode( const uint32_t MaxCopySize, const uint8_t* const DecodedBuffer, uint8_t* const EncodedBuffer, const uint32_t DecodedSize, uint32_t& EncodedSize, const uint32_t EncodedBufferSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	const uint32_t MaxFillSize = ( 257 - MaxCopySize );
	uint32_t EncodedLimit = ( EncodedSize ? EncodedSize : ( EncodedBufferSize ? EncodedBufferSize : static_cast< uint32_t >( int( -1 ) ) ) );
	uint32_t ValidateEncodedSize = EncodedSize;
	EncodedSize = 0;
	if( DecodedSize )
	{	//	uncompressed input buffer has data available, prime the first byte of the compressed output buffer
		uint32_t EncodedIndex = 1;
		uint32_t DecodedIndex = 1;
		uint32_t DecodedFlush = 1;
		uint8_t Byte = DecodedBuffer[ 0 ];
		EncodedBuffer[ 0 ] = Byte;
		while( DecodedIndex < DecodedSize )
		{	//	scan decoded data
			uint8_t Prev = Byte;
			Byte = DecodedBuffer[ DecodedIndex ];
			++DecodedIndex;
			if( Byte == Prev )
			{	//	potential start of fill operations
				if( DecodedIndex < DecodedSize )
				{	//	decoded buffer contains sufficient data for possible fill operations
					Byte = DecodedBuffer[ DecodedIndex ];
					++DecodedIndex;
					if( Byte == Prev )
					{	//	confirmed start of fill operations - flush pending copy operations
						uint32_t Length = ( DecodedIndex - DecodedFlush - 2 );
						uint32_t Chunks = ( Length / MaxCopySize );
						uint32_t Remain = ( Length % MaxCopySize );
						if( ( EncodedIndex + Length + Chunks + ( Remain ? 1 : 0 ) ) > EncodedLimit )
						{	//	the copy operations will overflow the output buffer
							return( RLE_ERROR_Encoded_Overflow );
						}
						while( Chunks )
						{	//	flush maximum length copy operations
							EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( MaxCopySize - 1 );
							++EncodedIndex;
							memcpy( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), reinterpret_cast< const void* >( &DecodedBuffer[ DecodedFlush ] ), MaxCopySize );
							EncodedIndex += MaxCopySize;
							DecodedFlush += MaxCopySize;
							--Chunks;
						}
						if( Remain )
						{	//	flush remaining copy operation
							EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( Remain - 1 );
							++EncodedIndex;
							memcpy( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), reinterpret_cast< const void* >( &DecodedBuffer[ DecodedFlush ] ), Remain );
							EncodedIndex += Remain;
							DecodedFlush += Remain;
						}
						Length = ( DecodedSize - DecodedFlush );	//	initialize fill length assuming that fill continues to end of data
						while( DecodedIndex < DecodedSize )
						{	//	scan for end of fill operations
							Byte = DecodedBuffer[ DecodedIndex ];
							++DecodedIndex;
							if( Byte != Prev )
							{	//	end of fill operations found
								Length = ( DecodedIndex - DecodedFlush - 1 );
								break;
							}
						}
						Chunks = ( Length / MaxFillSize );
						Remain = ( Length % MaxFillSize );
						if( Remain == 1 )
						{	//	eliminate terminating invalid single byte fill operation
							Remain = 0;
							--Length;
						}
						if( ( EncodedIndex + Chunks + ( Remain ? 1 : 0 ) ) > EncodedLimit )
						{	//	the fill operations will overflow the output buffer
							return( RLE_ERROR_Encoded_Overflow );
						}
						if( Chunks )
						{	//	flush maximum length fill operations
							memset( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), 255, Chunks );
							EncodedIndex += Chunks;
						}
						if( Remain )
						{	//	flush remaining fill operation
							EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( Remain + MaxCopySize - 2 );
							++EncodedIndex;
						}
						DecodedFlush += Length;
					}
				}
			}
		}
		if( DecodedFlush != DecodedIndex )
		{	//	flush pending copy operations
			uint32_t Length = ( DecodedIndex - DecodedFlush );
			uint32_t Chunks = ( Length / MaxCopySize );
			uint32_t Remain = ( Length % MaxCopySize );
			if( ( EncodedIndex + Length + Chunks + ( Remain ? 1 : 0 ) ) > EncodedLimit )
			{	//	the copy operations will overflow the output buffer
				return( RLE_ERROR_Encoded_Overflow );
			}
			while( Chunks )
			{	//	flush maximum length copy operations
				EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( MaxCopySize - 1 );
				++EncodedIndex;
				memcpy( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), reinterpret_cast< const void* >( &DecodedBuffer[ DecodedFlush ] ), MaxCopySize );
				EncodedIndex += MaxCopySize;
				DecodedFlush += MaxCopySize;
				--Chunks;
			}
			if( Remain )
			{	//	flush remaining copy operation
				EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( Remain - 1 );
				++EncodedIndex;
				memcpy( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), reinterpret_cast< const void* >( &DecodedBuffer[ DecodedFlush ] ), Remain );
				EncodedIndex += Remain;
				DecodedFlush += Remain;
			}
		}
		if( ( EncodedIndex + 4 ) > EncodedLimit )
		{	//	the terminator operation will overflow the output buffer
			return( RLE_ERROR_Encoded_Overflow );
		}
		EncodedBuffer[ EncodedIndex ] = EncodedBuffer[ EncodedIndex + 1 ] = EncodedBuffer[ EncodedIndex + 2 ] = EncodedBuffer[ EncodedIndex + 3 ] = 0;
		EncodedIndex += 4;
		if( ValidateEncodedSize )
		{	//	encoded data size validation requested
			if( EncodedIndex != ValidateEncodedSize )
			{	//	encoded data size validation failed
				return( RLE_ERROR_Encoded_Size_Incorrect );
			}
		}
		EncodedSize = EncodedIndex;
	}
	else if( ValidateEncodedSize )
	{	//	encoded data size validation failed
		return( RLE_ERROR_Encoded_Size_Incorrect );
	}
	return( RLE_SUCCESS );
}

//!	Decodes RLE compressed data.
//!	Returns RLE_SUCCESS if successful.
//!	If successful, EncodedSize will contain the actual size of the encoded data and DecodedSize will contain the actual size of the required decoding buffer.
//!	Specifying a non-zero EncodedSize indicates that the actual encoded size should be error checked against this value.
//!	Specifying a non-zero DecodedSize indicates that the actual decoded size should be error checked against this value.
RLE_ERROR Decode( const uint32_t MaxCopySize, const uint8_t* const EncodedBuffer, uint8_t* const DecodedBuffer, uint32_t& EncodedSize, uint32_t& DecodedSize, const uint32_t EncodedBufferSize, const uint32_t DecodedBufferSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	uint32_t EncodedLimit = ( EncodedSize ? EncodedSize : ( EncodedBufferSize ? EncodedBufferSize : static_cast< uint32_t >( int( -1 ) ) ) );
	uint32_t DecodedLimit = ( DecodedSize ? DecodedSize : ( DecodedBufferSize ? DecodedBufferSize : static_cast< uint32_t >( int( -1 ) ) ) );
	uint32_t ValidateEncodedSize = EncodedSize;
	uint32_t ValidateDecodedSize = DecodedSize;
	EncodedSize = 0;
	DecodedSize = 0;
	if( EncodedLimit > 4 )
	{	//	compressed input buffer has data available, prime the first byte of the uncompressed output buffer
		DecodedBuffer[ 0 ] = EncodedBuffer[ 0 ];
		uint32_t EncodedIndex = 1;
		uint32_t DecodedIndex = 1;
		while( EncodedIndex < EncodedLimit )
		{	//	loop while input remaining
			uint32_t Count = EncodedBuffer[ EncodedIndex ];
			++EncodedIndex;
			if( Count < MaxCopySize )
			{	//	byte copy operation
				if( Count )
				{	//	multi-byte copy
					++Count;
					if( ( EncodedIndex + Count ) > EncodedLimit )
					{	//	the input buffer does not contain enough data for the copy operation
						return( RLE_ERROR_Encoded_Overflow );
					}
					if( ( DecodedIndex + Count ) > DecodedLimit )
					{	//	the output buffer does not have enough space for the copy operation
						return( RLE_ERROR_Decoded_Overflow );
					}
					memcpy( reinterpret_cast< void* >( &DecodedBuffer[ DecodedIndex ] ), reinterpret_cast< const void* >( &EncodedBuffer[ EncodedIndex ] ), Count );
					EncodedIndex += Count;
					DecodedIndex += Count;
				}
				else
				{	//	single byte copy or terminator
					if( ( EncodedIndex + 3 ) > EncodedLimit )
					{	//	the input buffer does not contain enough data for the terminator
						return( RLE_ERROR_Encoded_Overflow );
					}
					if( EncodedBuffer[ EncodedIndex ] | EncodedBuffer[ EncodedIndex + 1 ] | EncodedBuffer[ EncodedIndex + 2 ] )
					{	//	single byte copy
						DecodedBuffer[ DecodedIndex ] = EncodedBuffer[ EncodedIndex ];
						++EncodedIndex;
						++DecodedIndex;
					}
					else
					{	//	terminator
						EncodedIndex += 3;
						if( ValidateEncodedSize )
						{	//	encoded data size validation requested
							if( EncodedIndex != ValidateEncodedSize )
							{	//	encoded data size validation failed
								return( RLE_ERROR_Encoded_Size_Incorrect );
							}
						}
						if( ValidateDecodedSize )
						{	//	decoded data size validation requested
							if( DecodedIndex != ValidateDecodedSize )
							{	//	decoded data size validation failed
								return( RLE_ERROR_Decoded_Size_Incorrect );
							}
						}
						EncodedSize = EncodedIndex;
						DecodedSize = DecodedIndex;
						return( RLE_SUCCESS );
					}
				}
			}
			else
			{	//	byte fill operation
				Count += ( 2 - MaxCopySize );
				if( ( DecodedIndex + Count ) > DecodedLimit )
				{	//	the output buffer does not have enough space for the byte fill operation
					return( RLE_ERROR_Decoded_Overflow );
				}
				memset( reinterpret_cast< void* >( &DecodedBuffer[ DecodedIndex ] ), DecodedBuffer[ DecodedIndex - 1 ], Count );
				DecodedIndex += Count;
			}
		}
	}
	return( RLE_ERROR_Encoded_Overflow );
}

//!	Test RLE encoding without writing to an output buffer.
//!	Returns RLE_SUCCESS if successful.
//!	If successful, EncodedSize will contain the actual size of the required encoding buffer.
//!	Specifying a non-zero EncodedSize indicates that the actual encoded size should be error checked against this value.
RLE_ERROR Encode( const uint32_t MaxCopySize, const uint8_t* const DecodedBuffer, const uint32_t DecodedSize, uint32_t& EncodedSize, const uint32_t EncodedBufferSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	const uint32_t MaxFillSize = ( 257 - MaxCopySize );
	uint32_t EncodedLimit = ( EncodedSize ? EncodedSize : ( EncodedBufferSize ? EncodedBufferSize : static_cast< uint32_t >( int( -1 ) ) ) );
	uint32_t ValidateEncodedSize = EncodedSize;
	EncodedSize = 0;
	if( DecodedSize )
	{	//	uncompressed input buffer has data available, prime the first byte of the compressed output buffer
		uint32_t EncodedIndex = 1;
		uint32_t DecodedIndex = 1;
		uint32_t DecodedFlush = 1;
		uint8_t Byte = DecodedBuffer[ 0 ];
		while( DecodedIndex < DecodedSize )
		{	//	scan decoded data
			uint8_t Prev = Byte;
			Byte = DecodedBuffer[ DecodedIndex ];
			++DecodedIndex;
			if( Byte == Prev )
			{	//	potential start of fill operations
				if( DecodedIndex < DecodedSize )
				{	//	decoded buffer contains sufficient data for possible fill operations
					Byte = DecodedBuffer[ DecodedIndex ];
					++DecodedIndex;
					if( Byte == Prev )
					{	//	confirmed start of fill operations - flush pending copy operations
						uint32_t Length = ( DecodedIndex - DecodedFlush - 2 );
						uint32_t Remain = ( Length % MaxCopySize );
						EncodedIndex += ( Length + ( Length / MaxCopySize ) + ( Remain ? 1 : 0 ) );
						DecodedFlush += Length;
						if( EncodedIndex > EncodedLimit )
						{	//	the copy operations will overflow the output buffer
							return( RLE_ERROR_Encoded_Overflow );
						}
						Length = ( DecodedSize - DecodedFlush );	//	initialize fill length assuming that fill continues to end of data
						while( DecodedIndex < DecodedSize )
						{	//	scan for end of fill operations
							Byte = DecodedBuffer[ DecodedIndex ];
							++DecodedIndex;
							if( Byte != Prev )
							{	//	end of fill operations found
								Length = ( DecodedIndex - DecodedFlush - 1 );
								break;
							}
						}
						Remain = ( Length % MaxFillSize );
						if( Remain == 1 )
						{	//	eliminate terminating invalid single byte fill operation
							Remain = 0;
							--Length;
						}
						EncodedIndex += ( ( Length / MaxFillSize ) + ( Remain ? 1 : 0 ) );
						DecodedFlush += Length;
						if( EncodedIndex > EncodedLimit )
						{	//	the fill operations will overflow the output buffer
							return( RLE_ERROR_Encoded_Overflow );
						}
					}
				}
			}
		}
		if( DecodedFlush != DecodedIndex )
		{	//	flush pending copy operations
			uint32_t Length = ( DecodedIndex - DecodedFlush );
			uint32_t Remain = ( Length % MaxCopySize );
			EncodedIndex += ( Length + ( Length / MaxCopySize ) + ( Remain ? 1 : 0 ) );
			DecodedFlush += Length;
			if( EncodedIndex > EncodedLimit )
			{	//	the copy operations will overflow the output buffer
				return( RLE_ERROR_Encoded_Overflow );
			}
		}
		if( ( EncodedIndex + 4 ) > EncodedLimit )
		{	//	the terminator operation will overflow the output buffer
			return( RLE_ERROR_Encoded_Overflow );
		}
		EncodedIndex += 4;
		if( ValidateEncodedSize )
		{	//	encoded data size validation requested
			if( EncodedIndex != ValidateEncodedSize )
			{	//	encoded data size validation failed
				return( RLE_ERROR_Encoded_Size_Incorrect );
			}
		}
		EncodedSize = EncodedIndex;
	}
	else if( ValidateEncodedSize )
	{	//	encoded data size validation failed
		return( RLE_ERROR_Encoded_Size_Incorrect );
	}
	return( RLE_SUCCESS );
}

//!	Test RLE decoding without writing to an output buffer.
//!	Returns RLE_SUCCESS if successful.
//!	If successful, EncodedSize will contain the actual size of the encoded data and DecodedSize will contain the actual size of the required decoding buffer.
//!	Specifying a non-zero EncodedSize indicates that the actual encoded size should be error checked against this value.
//!	Specifying a non-zero DecodedSize indicates that the actual decoded size should be error checked against this value.
RLE_ERROR Decode( const uint32_t MaxCopySize, const uint8_t* const EncodedBuffer, uint32_t& EncodedSize, uint32_t& DecodedSize, const uint32_t EncodedBufferSize, const uint32_t DecodedBufferSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	uint32_t EncodedLimit = ( EncodedSize ? EncodedSize : ( EncodedBufferSize ? EncodedBufferSize : static_cast< uint32_t >( int( -1 ) ) ) );
	uint32_t DecodedLimit = ( DecodedSize ? DecodedSize : ( DecodedBufferSize ? DecodedBufferSize : static_cast< uint32_t >( int( -1 ) ) ) );
	uint32_t ValidateEncodedSize = EncodedSize;
	uint32_t ValidateDecodedSize = DecodedSize;
	EncodedSize = 0;
	DecodedSize = 0;
	if( EncodedLimit > 4 )
	{	//	compressed input buffer has data available, prime the first byte of the uncompressed output buffer
		uint32_t EncodedIndex = 1;
		uint32_t DecodedIndex = 1;
		while( EncodedIndex < EncodedLimit )
		{	//	loop while input remaining
			uint32_t Count = EncodedBuffer[ EncodedIndex ];
			++EncodedIndex;
			if( Count < MaxCopySize )
			{	//	byte copy operation
				if( Count )
				{	//	multi-byte copy
					++Count;
					EncodedIndex += Count;
					DecodedIndex += Count;
					if( EncodedIndex > EncodedLimit )
					{	//	the input buffer does not contain enough data for the copy operation
						return( RLE_ERROR_Encoded_Overflow );
					}
					if( DecodedIndex > DecodedLimit )
					{	//	the output buffer does not have enough space for the copy operation
						return( RLE_ERROR_Decoded_Overflow );
					}
				}
				else
				{	//	single byte copy or terminator
					if( ( EncodedIndex + 3 ) > EncodedLimit )
					{	//	the input buffer does not contain enough data for the terminator
						return( RLE_ERROR_Encoded_Overflow );
					}
					if( EncodedBuffer[ EncodedIndex ] | EncodedBuffer[ EncodedIndex + 1 ] | EncodedBuffer[ EncodedIndex + 2 ] )
					{	//	single byte copy
						++EncodedIndex;
						++DecodedIndex;
					}
					else
					{	//	terminator
						EncodedIndex += 3;
						if( ValidateEncodedSize )
						{	//	encoded data size validation requested
							if( EncodedIndex != ValidateEncodedSize )
							{	//	encoded data size validation failed
								return( RLE_ERROR_Encoded_Size_Incorrect );
							}
						}
						if( ValidateDecodedSize )
						{	//	decoded data size validation requested
							if( DecodedIndex != ValidateDecodedSize )
							{	//	decoded data size validation failed
								return( RLE_ERROR_Decoded_Size_Incorrect );
							}
						}
						EncodedSize = EncodedIndex;
						DecodedSize = DecodedIndex;
						return( RLE_SUCCESS );
					}
				}
			}
			else
			{	//	byte fill operation
				Count += ( 2 - MaxCopySize );
				if( ( DecodedIndex + Count ) > DecodedLimit )
				{	//	the output buffer does not have enough space for the byte fill operation
					return( RLE_ERROR_Decoded_Overflow );
				}
				DecodedIndex += Count;
			}
		}
	}
	return( RLE_ERROR_Encoded_Overflow );
}

//!	Encodes data as RLE compressed without error checking.
//!	If successful, EncodedSize will contain the actual size of the required encoding buffer.
void QuickEncode( const uint32_t MaxCopySize, const uint8_t* const DecodedBuffer, uint8_t* const EncodedBuffer, const uint32_t DecodedSize, uint32_t& EncodedSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	const uint32_t MaxFillSize = ( 257 - MaxCopySize );
	EncodedSize = 0;
	if( DecodedSize )
	{	//	uncompressed input buffer has data available, prime the first byte of the compressed output buffer
		uint32_t EncodedIndex = 1;
		uint32_t DecodedIndex = 1;
		uint32_t DecodedFlush = 1;
		uint8_t Byte = DecodedBuffer[ 0 ];
		EncodedBuffer[ 0 ] = Byte;
		while( DecodedIndex < DecodedSize )
		{	//	scan decoded data
			uint8_t Prev = Byte;
			Byte = DecodedBuffer[ DecodedIndex ];
			++DecodedIndex;
			if( Byte == Prev )
			{	//	potential start of fill operations
				if( DecodedIndex < DecodedSize )
				{	//	decoded buffer contains sufficient data for possible fill operations
					Byte = DecodedBuffer[ DecodedIndex ];
					++DecodedIndex;
					if( Byte == Prev )
					{	//	confirmed start of fill operations - flush pending copy operations
						uint32_t Length = ( DecodedIndex - DecodedFlush - 2 );
						uint32_t Chunks = ( Length / MaxCopySize );
						uint32_t Remain = ( Length % MaxCopySize );
						while( Chunks )
						{	//	flush maximum length copy operations
							EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( MaxCopySize - 1 );
							++EncodedIndex;
							memcpy( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), reinterpret_cast< const void* >( &DecodedBuffer[ DecodedFlush ] ), MaxCopySize );
							EncodedIndex += MaxCopySize;
							DecodedFlush += MaxCopySize;
							--Chunks;
						}
						if( Remain )
						{	//	flush remaining copy operation
							EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( Remain - 1 );
							++EncodedIndex;
							memcpy( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), reinterpret_cast< const void* >( &DecodedBuffer[ DecodedFlush ] ), Remain );
							EncodedIndex += Remain;
							DecodedFlush += Remain;
						}
						Length = ( DecodedSize - DecodedFlush );	//	initialize fill length assuming that fill continues to end of data
						while( DecodedIndex < DecodedSize )
						{	//	scan for end of fill operations
							Byte = DecodedBuffer[ DecodedIndex ];
							++DecodedIndex;
							if( Byte != Prev )
							{	//	end of fill operations found
								Length = ( DecodedIndex - DecodedFlush - 1 );
								break;
							}
						}
						Chunks = ( Length / MaxFillSize );
						Remain = ( Length % MaxFillSize );
						if( Remain == 1 )
						{	//	eliminate terminating invalid single byte fill operation
							Remain = 0;
							--Length;
						}
						if( Chunks )
						{	//	flush maximum length fill operations
							memset( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), 255, Chunks );
							EncodedIndex += Chunks;
						}
						if( Remain )
						{	//	flush remaining fill operation
							EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( Remain + MaxCopySize - 2 );
							++EncodedIndex;
						}
						DecodedFlush += Length;
					}
				}
			}
		}
		if( DecodedFlush != DecodedIndex )
		{	//	flush pending copy operations
			uint32_t Length = ( DecodedIndex - DecodedFlush );
			uint32_t Chunks = ( Length / MaxCopySize );
			uint32_t Remain = ( Length % MaxCopySize );
			while( Chunks )
			{	//	flush maximum length copy operations
				EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( MaxCopySize - 1 );
				++EncodedIndex;
				memcpy( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), reinterpret_cast< const void* >( &DecodedBuffer[ DecodedFlush ] ), MaxCopySize );
				EncodedIndex += MaxCopySize;
				DecodedFlush += MaxCopySize;
				--Chunks;
			}
			if( Remain )
			{	//	flush remaining copy operation
				EncodedBuffer[ EncodedIndex ] = static_cast< uint8_t >( Remain - 1 );
				++EncodedIndex;
				memcpy( reinterpret_cast< void* >( &EncodedBuffer[ EncodedIndex ] ), reinterpret_cast< const void* >( &DecodedBuffer[ DecodedFlush ] ), Remain );
				EncodedIndex += Remain;
				DecodedFlush += Remain;
			}
		}
		EncodedBuffer[ EncodedIndex ] = EncodedBuffer[ EncodedIndex + 1 ] = EncodedBuffer[ EncodedIndex + 2 ] = EncodedBuffer[ EncodedIndex + 3 ] = 0;
		EncodedIndex += 4;
		EncodedSize = EncodedIndex;
	}
}

//!	Decodes RLE compressed data without error checking.
//!	If successful, EncodedSize will contain the actual size of the encoded data and DecodedSize will contain the actual size of the required decoding buffer.
void QuickDecode( const uint32_t MaxCopySize, const uint8_t* const EncodedBuffer, uint8_t* const DecodedBuffer, uint32_t& EncodedSize, uint32_t& DecodedSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	DecodedBuffer[ 0 ] = EncodedBuffer[ 0 ];
	for(;;)
	{
		uint32_t EncodedIndex = 1;
		uint32_t DecodedIndex = 1;
		uint32_t Count = EncodedBuffer[ EncodedIndex ];
		++EncodedIndex;
		if( Count < MaxCopySize )
		{	//	copy or terminator
			if( Count )
			{	//	multi-byte copy
				++Count;
				memcpy( reinterpret_cast< void* >( &DecodedBuffer[ DecodedIndex ] ), reinterpret_cast< const void* >( &EncodedBuffer[ EncodedIndex ] ), Count );
				EncodedIndex += Count;
				DecodedIndex += Count;
			}
			else if( EncodedBuffer[ EncodedIndex ] | EncodedBuffer[ EncodedIndex + 1 ] | EncodedBuffer[ EncodedIndex + 2 ] )
			{	//	single byte copy
				DecodedBuffer[ DecodedIndex ] = EncodedBuffer[ EncodedIndex ];
				++EncodedIndex;
				++DecodedIndex;
			}
			else
			{	//	terminator
				EncodedIndex += 3;
				EncodedSize = EncodedIndex;
				DecodedSize = DecodedIndex;
				return;
			}
		}
		else
		{	//	multi-byte fill
			Count += ( 2 - MaxCopySize );
			memset( reinterpret_cast< void* >( &DecodedBuffer[ DecodedIndex ] ), DecodedBuffer[ DecodedIndex - 1 ], Count );
			DecodedIndex += Count;
		}
	}
}

//!	Test RLE encoding without error checking and without writing to an output buffer.
//!	If successful, EncodedSize will contain the actual size of the required encoding buffer.
void QuickEncode( const uint32_t MaxCopySize, const uint8_t* const DecodedBuffer, const uint32_t DecodedSize, uint32_t& EncodedSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	const uint32_t MaxFillSize = ( 257 - MaxCopySize );
	EncodedSize = 0;
	if( DecodedSize )
	{	//	uncompressed input buffer has data available, prime the first byte of the compressed output buffer
		uint32_t EncodedIndex = 1;
		uint32_t DecodedIndex = 1;
		uint32_t DecodedFlush = 1;
		uint8_t Byte = DecodedBuffer[ 0 ];
		while( DecodedIndex < DecodedSize )
		{	//	scan decoded data
			uint8_t Prev = Byte;
			Byte = DecodedBuffer[ DecodedIndex ];
			++DecodedIndex;
			if( Byte == Prev )
			{	//	potential start of fill operations
				if( DecodedIndex < DecodedSize )
				{	//	decoded buffer contains sufficient data for possible fill operations
					Byte = DecodedBuffer[ DecodedIndex ];
					++DecodedIndex;
					if( Byte == Prev )
					{	//	confirmed start of fill operations - flush pending copy operations
						uint32_t Length = ( DecodedIndex - DecodedFlush - 2 );
						uint32_t Remain = ( Length % MaxCopySize );
						EncodedIndex += ( Length + ( Length / MaxCopySize ) + ( Remain ? 1 : 0 ) );
						DecodedFlush += Length;
						Length = ( DecodedSize - DecodedFlush );	//	initialize fill length assuming that fill continues to end of data
						while( DecodedIndex < DecodedSize )
						{	//	scan for end of fill operations
							Byte = DecodedBuffer[ DecodedIndex ];
							++DecodedIndex;
							if( Byte != Prev )
							{	//	end of fill operations found
								Length = ( DecodedIndex - DecodedFlush - 1 );
								break;
							}
						}
						Remain = ( Length % MaxFillSize );
						if( Remain == 1 )
						{	//	eliminate terminating invalid single byte fill operation
							Remain = 0;
							--Length;
						}
						EncodedIndex += ( ( Length / MaxFillSize ) + ( Remain ? 1 : 0 ) );
						DecodedFlush += Length;
					}
				}
			}
		}
		if( DecodedFlush != DecodedIndex )
		{	//	flush pending copy operations
			uint32_t Length = ( DecodedIndex - DecodedFlush );
			uint32_t Remain = ( Length % MaxCopySize );
			EncodedIndex += ( Length + ( Length / MaxCopySize ) + ( Remain ? 1 : 0 ) );
			DecodedFlush += Length;
		}
		EncodedIndex += 4;
		EncodedSize = EncodedIndex;
	}
}

//!	Test RLE decoding without error checking and without writing to an output buffer.
//!	If successful, EncodedSize will contain the actual size of the encoded data and DecodedSize will contain the actual size of the required decoding buffer.
void QuickDecode( const uint32_t MaxCopySize, const uint8_t* const EncodedBuffer, uint32_t& EncodedSize, uint32_t& DecodedSize )
{
	ASSERT( ( MaxCopySize >= 1 ) && ( MaxCopySize <= 255 ) );
	for(;;)
	{
		uint32_t EncodedIndex = 1;
		uint32_t DecodedIndex = 1;
		uint32_t Count = EncodedBuffer[ EncodedIndex ];
		++EncodedIndex;
		if( Count < MaxCopySize )
		{	//	copy or terminator
			if( Count )
			{	//	multi-byte copy
				++Count;
				EncodedIndex += Count;
				DecodedIndex += Count;
			}
			else if( EncodedBuffer[ EncodedIndex ] | EncodedBuffer[ EncodedIndex + 1 ] | EncodedBuffer[ EncodedIndex + 2 ] )
			{	//	single byte copy
				++EncodedIndex;
				++DecodedIndex;
			}
			else
			{	//	terminator
				EncodedIndex += 3;
				EncodedSize = EncodedIndex;
				DecodedSize = DecodedIndex;
				return;
			}
		}
		else
		{	//	multi-byte fill
			Count += ( 2 - MaxCopySize );
			DecodedIndex += Count;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end rle namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace rle

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end compression namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace compression

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
