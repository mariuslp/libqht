#include <vector>
#include <boost/functional/hash.hpp>

#include "types.h"

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

HashValue Hash3(const HashValue t);

template<class T> std::vector<HashValue> getHashes(const size_t k, const T& t, const size_t m) {
	/** Generates k hashes for an element t
	 *   @param k: number of hashes to generate
	 *   @param t: object to be hashed
	 *   @param m: the size of the hashes
	 *
	 *   @returns A family of HashValues which are supposed to have a similar influence than independent hashes
	 *   @see "Less Hashing, Same Performance: Building a Better Bloom Filter" by A. Kirsch & M. Mitzenmacher
	 */
	std::vector<HashValue> results;

	HashValue hash1 = Hash1<T>(t);
	HashValue hash2 = Hash2<T>(t);

	results.push_back(hash1 % m);
	for(size_t i=1; i<k; ++i) {
		results.push_back((results[i - 1] + hash2) % m);
	}

	return results;
}
