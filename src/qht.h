#pragma once

#include <cassert>
#include <random>
#include <boost/functional/hash.hpp>

#include "hash.h"

template <class T> struct QHTFilter {

protected:
	size_t array_size;
	size_t n_cells;
	size_t n_buckets;
	size_t fingerprint_size;

	std::mt19937 rng;
	std::uniform_int_distribution<size_t> bucket_selector;

	std::vector<bool> qht;

	uint64_t Fingerprint(const T& e);
	size_t Address(const T& e);
	bool InCell(const uint64_t address, const uint64_t fingerprint) const;
	bool InsertFingerprintInBucket(const uint64_t address, const size_t bucket_number, const uint64_t fingerprint);
	uint64_t GetFingerprintFromBucket(const uint64_t address, const size_t bucket_number) const;

public:
	QHTFilter(const uint64_t memory_size, const size_t n_n_buckets, const size_t n_fingerprint_size);
	bool Lookup(const T& e);
	bool Insert(const T& e);
	bool Stream(const T& e);
	bool Delete(const T& e);
	void Reset();
};

template <class T> QHTFilter<T>::QHTFilter(
	const uint64_t memory_size,
	const size_t n_n_buckets,
	const size_t n_fingerprint_size
) : n_buckets(n_n_buckets), fingerprint_size(n_fingerprint_size)
{
	n_cells = memory_size / (n_buckets * fingerprint_size);
	assert(n_cells > 0);
	assert(fingerprint_size < 64); // Fingerprints are stored in uint64_t
	Reset();
}

template <class T> size_t QHTFilter<T>::Address(const T& e) {
	/** Get the address of an element
	 *
	 * @param T e the element
	 * @return size_t address the address of the element in the filter
	 */
	return Hash1(e) % n_cells;
}


template <class T> uint64_t QHTFilter<T>::Fingerprint(const T& e) {
	/** Get the fingerprint of an element
	 * 0 is a reserved value and as such cannot be used as a fingerprint
	 * For this reason we iterate on hashing until we find a nonzero fingerprint
	 * (Averaged number of hashes: 1 + 1/2^fingerprint_size)
	 * This configuration makes sure every fingerprint is equiprobable
	 * (at the cost of slight computing overhead)
	 *
	 * @param T e the element
	 * @return int fingerprint of e
	 */

	// Note: the hash must be independent from Hash1 which already provides `address`
	HashValue hash = Hash2(e);

	uint64_t fingerprint = hash & ((1 << fingerprint_size) - 1);

	int adder = 0;
	// TODO std::hash(hash + ++adder) seems to have poor statistical properties as this loop is run a bit too often to my taste
	while(fingerprint == 0) {
		boost::hash_combine(hash, hash + ++adder);  // adder avoids potential infinite loops with fixed points (such as 11754104648456392440)
		fingerprint = hash & ((1 << fingerprint_size) - 1);
	}

	return fingerprint;
}

template <class T> bool QHTFilter<T>::Lookup(const T& e) {

	/** Returns true if the element e is detected inside the filter
	 * @param e
	 * @returns boolean
	 */

	auto address = Address(e);
	auto fingerprint = Fingerprint(e);

	return InCell(address, fingerprint);
}

template <class T> bool QHTFilter<T>::Insert(const T& e) {

	/** Inserts element e in the filter if not already present
	 * @param e
	 * @returns true
	 */

	auto address = Address(e);
	auto fingerprint = Fingerprint(e);

	// Run from all buckets, left to right. Stops at element, or first empty cell.
	// If it stops at empty cell the element is not present.
	bool detected = false;
	size_t bucket_number = 0;
	auto current_fingerprint = GetFingerprintFromBucket(address, bucket_number);

	while(!detected && current_fingerprint != 0 && bucket_number < n_buckets) {
		detected = (fingerprint == GetFingerprintFromBucket(address, bucket_number));

		if(!detected) {
			++bucket_number;
		}
	}

	// Do not insert an element already present
	if(detected) {
		return true;
	}

	// Try to insert in empty bucket if possible
	if(current_fingerprint == 0) {
		InsertFingerprintInBucket(address, bucket_number, fingerprint);
		return true;
	}

	// No empty bucket, inserting in random bucket (erasing previous content)
	InsertFingerprintInBucket(address, bucket_selector(rng), fingerprint);

	return true;
}

template <class T> bool QHTFilter<T>::Stream(const T& e) {
	/** Inserts element e in the filter if not already present
	 * Is equivalent to Detect(e) followed by Insert(e), but faster (only one round of hashing)
	 *
	 * @param e
	 * @returns boolean being true if the element was already in the filter, false otherwise
	 */
	auto address = Address(e);
	auto fingerprint = Fingerprint(e);

	// Run from all buckets, left to right. Stops at element, or first empty cell (empty cells contain 0).
	// If it stops at empty cell the element is not present.
	bool detected = false;
	size_t bucket_number = 0;
	auto current_fingerprint = GetFingerprintFromBucket(address, bucket_number);

	while(!detected && current_fingerprint != 0 && bucket_number < n_buckets) {
		detected = (fingerprint == GetFingerprintFromBucket(address, bucket_number));

		if(!detected) {
			++bucket_number;
		}
	}

	// Do not insert an element already present
	if(detected) {
		return true;
	}

	// Try to insert in empty bucket if possible
	if(current_fingerprint == 0) {
		InsertFingerprintInBucket(address, bucket_number, fingerprint);
		return false;
	}

	// No empty bucket, inserting in random bucket (erasing previous content)
	InsertFingerprintInBucket(address, bucket_selector(rng), fingerprint);

	return false;
}

template <class T> bool QHTFilter<T>::Delete(const T& e) {
	/**
	 * Deletes an element e from the QHT.
	 * This function deletes one element in the QHT that has the same hash and the same fingerprint as e
	 * and returns true.
	 * If no such element is found, returns false
	 *
	 * If e is present several times in the filter (which is possible in QQHTD), only one copy will be deleted
	 *
	 * However, this function cannot distinguish between e and a false duplicate of e
	 * (same address and hash, but not e). This is unavoidable and can subsequently lead to false negatives.
	 *
	 * @param e: the element to remove from the filter
	 * @returns bool: true if the element, or a false duplicate, is found (and deleted),
	 *                false if no such element is found.
	 */
	bool element_found = false;

	auto address = Address(e);
	auto fingerprint = Fingerprint(e);

	size_t i = 0;
	while(! element_found && i < n_buckets) {
		if(GetFingerprintFromBucket(address, i) == fingerprint) {
			element_found = true;
		} else {
			++i;
		}
	}

	if(! element_found) {
		return false;
	}

	// Remove the element from the list by shifting the following elements one cell to the left
	// We must do this because we assume that all empty buckets are filled from lowest indice to highest indice 
	for(; i < n_buckets - 1; ++i) {
		InsertFingerprintInBucket(address, i, GetFingerprintFromBucket(address, i + 1));
	}

	// Re-set the last element of the list to `Empty`. Also covers the case where the e to be removed
	// is the last element of the list.
	InsertFingerprintInBucket(address, n_buckets - 1, 0);

	return true;
}

template <class T> uint64_t QHTFilter<T>::GetFingerprintFromBucket(const uint64_t address, const size_t bucket_number) const {

	/**
	 * All bits are stored in sequence.
	 * One cell has n_buckets buckets, each containing fingerprint_size (f_s) bits.
	 * The f_s bits of a fingerprint are stored consecutively, the buckets of a cell too.
	 * Hence the computation of the offset for a given bucket of a given cell.
	 * @param address: to be documented
	 * @param bucket_number: to be documented
	 * @returns to be documented
	 */

	uint64_t fingerprint = 0;

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


template <class T> bool QHTFilter<T>::InsertFingerprintInBucket(const uint64_t address, const size_t bucket_number, const uint64_t fingerprint) {
	
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

template <class T> bool QHTFilter<T>::InCell(const uint64_t address, const uint64_t fingerprint) const {

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

template <class T> void QHTFilter<T>::Reset() {
	/**
	 * Re-set all cells to 0 (Empty)
	 * Also sets the QHT table to its assigned capacity, if not already done.
	 */
	qht.assign(n_cells * n_buckets * fingerprint_size, 0);
}
