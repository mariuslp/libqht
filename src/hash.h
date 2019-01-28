#pragma once

#include <boost/functional/hash.hpp>
#include <cstdint>

/** Syntactic sugar for hash values */
typedef std::size_t HashValue;

template<class T> HashValue Hash1(T t) {
	/** Computes a hash for an element of type T 
	 *   @param t: object to be hashed
	 *   @returns A HashValue which is likely to be different for different inputs
	 */
	return std::hash<T>{}(t);
}

template<class T> HashValue Hash2(T t) {
	/** Computes a hash for an element of type T
	 * @param t: object to be hashed
	 * @returns A HashValue (more or less) independent of Hash1(t)
	 */
	size_t seed = 0x1234567890abcdef;
	boost::hash_combine(seed, Hash1(t));

	return seed;
}

