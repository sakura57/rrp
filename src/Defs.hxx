/*
	Part of the R2A03 Recompiler Project
*/

#pragma once

#include <array>
#include <cstdint>
#include <tuple>

//preprocessor to detect endianness
#if defined(_WIN32)
#define RRP_LITTLE_ENDIAN
#elif defined(__ORDER_LITTLE_ENDIAN__)
#define RRP_LITTLE_ENDIAN
#elif defined(__ORDER_BIG_ENDIAN__)
#define RRP_BIG_ENDIAN
#endif

namespace RRP
{
	typedef uint8_t byte_t;
	typedef int8_t offset_t;
	typedef uint16_t word_t;

	inline word_t makeword(byte_t const &lo, byte_t const &hi)
	{
#if defined(RRP_LITTLE_ENDIAN)
		return ((word_t)hi << 8) | lo;
#elif defined(RRP_BIG_ENDIAN)
		return ((word_t)lo << 8) | hi;
#endif
	};
}
