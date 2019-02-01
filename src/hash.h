#pragma once

#include <cstdint>

#include "xxhash.h"
#include "xxhash.hpp"

/** Syntactic sugar for hash values */
typedef xxh::hash_t<64> HashValue;

template<class T> HashValue Hash1(T t) {
	/** Computes a hash for an element of type T 
	 *   @param t: object to be hashed
	 *   @returns A HashValue which is likely to be different for different inputs
	 */
	return xxh::xxhash<64>(t);
}

template<class T> HashValue Hash2(T t) {
	/** Computes a hash for an element of type T
	 * @param t: object to be hashed
	 * @returns A HashValue (more or less) independent of Hash1(t)
	 */
	size_t seed = 0x1234567890abcdef;

	return xxh::xxhash<64>(t, seed);
}

