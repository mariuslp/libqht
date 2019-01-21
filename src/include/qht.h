#include <cassert>
#include <random>

#include "fastmath.h"
#include "hash.h"
#include "types.h"

template <class T> struct QHTFilter	 {

protected:
	size_t array_size;
	size_t n_cells;
	size_t n_buckets;
	size_t fingerprint_size;

	std::mt19937 rng;
	std::uniform_int_distribution<size_t> bucket_selector;

	std::vector<bool> qht;

	uint8_t Fingerprint(const T& e);
	bool InCell(const uint64_t address, const uint8_t fingerprint) const;
	bool InsertEmpty(const uint64_t address, const uint8_t fingerprint);
	bool InsertFingerprintInBucket(const uint64_t address, const size_t bucket_number, const uint8_t fingerprint);
	uint8_t GetFingerprintFromBucket(const uint64_t address, const size_t bucket_number) const;

public:
	QHTFilter(const uint64_t memory_size, const size_t n_n_buckets, const size_t n_fingerprint_size);
	bool Lookup(const T& e);
	bool Insert(const T& e);
	void Reset();
};


template <class T> uint8_t QHTFilter<T>::Fingerprint(const T& e) {
	/** Get the fingerprint of an element
	 * 0 is a reserved value and as such cannot be used as a fingerprint
	 * For this reason we iterate on hashing until we find a nonzero fingerprint
	 * (Averaged number of hashes: 1 + 1/2^fingerprint_size)
	 * This configuration makes sure every fingerprint is equiprobable
	 * (at the cost of slight computing overhead)
	 *
	 * @param Element e
	 * @return int fingerprint of e
	 */

	// Note: the hash must be independent from Hash1 which already provides `address`
	HashValue hash = Hash2<T>(e);

	uint8_t fingerprint = static_cast<uint8_t>(hash % pow2(fingerprint_size));

	int adder = 0;
	// TODO std::hash(hash + ++adder) seems to have poor statistical properties as this loop is run a bit too often to my taste
	while(fingerprint == 0) {
		boost::hash_combine(hash, hash + ++adder);  // adder avoids potential infinite loops with fixed points (such as 11754104648456392440)
		fingerprint = static_cast<uint8_t>(hash % pow2(fingerprint_size));
	}

	return fingerprint;
}

template <class T> bool QHTFilter<T>::Lookup(const T& e) {

	/** Returns true if the element e is detected inside the filter
     * @param e
	 * @returns boolean
	 */

	size_t address = Hash1(e) % n_cells;
	auto fingerprint = Fingerprint(e);

	return InCell(address, fingerprint);
}

template <class T> bool QHTFilter<T>::Insert(const T& e) {

	/** Inserts element e in the filter if not already present
	 * @param e
	 * @returns boolean being true if the element was already in the filter
	 */

	auto detected = Lookup(e);
        
	// Does not insert if the element is already present in the filter
	if(detected) {
		return detected;
	}

	size_t address = Hash1(e) % n_cells;
	auto fingerprint = Fingerprint(e);

	// First try inserting in empty bucket
	if(InsertEmpty(address, fingerprint)) {
		return detected;
	}

	// No empty bucket, inserting in random bucket (erasing previous content)
	InsertFingerprintInBucket(address, bucket_selector(rng), fingerprint);

	return detected;
}


template <class T> uint8_t QHTFilter<T>::GetFingerprintFromBucket(const uint64_t address, const size_t bucket_number) const {

	/**
	 * All bits are stored in sequence.
	 * One cell has n_buckets buckets, each containing fingerprint_size (f_s) bits.
	 * The f_s bits of a fingerprint are stored consecutively, the buckets of a cell too.
	 * Hence the computation of the offset for a given bucket of a given cell.
	 * @param address: to be documented
	 * @param bucket_number: to be documented
	 * @returns to be documented
	 */

	uint8_t fingerprint = 0;

	// The cell `address` starts here
	auto offset = address * n_buckets * fingerprint_size;

	// We add the offset of the number of buckets we want
	offset += bucket_number * fingerprint_size;

	// We rebuild the fingerprint bit per bit
	// Most significant bit has the lowest index
	for(size_t i = 0; i < fingerprint_size; ++i) {
		fingerprint = (fingerprint << 1) + qht.at(offset + i);
	}

	return fingerprint;
}


template <class T> bool QHTFilter<T>::InsertFingerprintInBucket(const uint64_t address, const size_t bucket_number, const uint8_t fingerprint) {
	
	/** Takes a fingerprint, and inserts it in the given bucket number of a given cell (address)
	 * @param address
	 * @param bucket_number: int in 0..bucket_number - 1
	 * @param fingerprint
	 * @returns true
	 */

	auto offset = address * n_buckets * fingerprint_size + bucket_number * fingerprint_size;

	auto bits_to_insert = fingerprint;
	// We store bits in the allocated slots
	// Most significant bit has the smallest index
	for(size_t i = 0; i < fingerprint_size; ++i) {
		qht.at(offset + (fingerprint_size - i - 1)) = bits_to_insert & 0b1;
		bits_to_insert = bits_to_insert >> 1;
	}

	return true;
}

template <class T> bool QHTFilter<T>::InCell(const uint64_t address, const uint8_t fingerprint) const {

	/** Return true if a fingerprint is in one of the buckets of a given cell (address)
	 * @param address
	 * @param fingerprint
	 * @returns boolean
	 */

	for(size_t i = 0; i < n_buckets; ++i) {
		if(GetFingerprintFromBucket(address, i) == fingerprint) {
			return true;
		}
	}

	return false;
}

template <class T> bool QHTFilter<T>::InsertEmpty(const uint64_t address, const uint8_t fingerprint) {

	/** Inserts fingerprint in an empty bucket of the cell (address), if such bucket exists
	 * @param address
	 * @param fingerprint
	 * @returns true if element has been implemented, false otherwise
	 */

	for(size_t i = 0; i < n_buckets; ++i) {
		if(GetFingerprintFromBucket(address, i) == 0) {
			InsertFingerprintInBucket(address, i, fingerprint);
			return true;
		}
	}

	return false;
}