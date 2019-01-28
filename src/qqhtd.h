#pragma once

#include "qht.h"

template <class T> struct QQHTDFilter : QHTFilter<T> {

public:
	QQHTDFilter(const uint64_t memory_size, const size_t n_n_buckets, const size_t n_fingerprint_size);
	bool Insert(const T& e);
        bool Delete(const T& e);

protected:
    bool InsertFingerprintInLastBucket(const uint64_t address, const uint64_t fingerprint);
};

template <class T> QQHTDFilter<T>::QQHTDFilter(const uint64_t memory_size, const size_t n_n_buckets, const size_t n_fingerprint_size) : QHTFilter<T>(memory_size, n_n_buckets, n_fingerprint_size) {
}

template <class T> bool QQHTDFilter<T>::Insert(const T& e) {
	/**
	 * Inserts element e in the filter
         * @param e
         * @return bool : true if e is detected as a duplicate, false otherwise 
	 */
        auto detected = this->Lookup(e);

        size_t address = Hash1(e) % this->n_cells;
        auto fingerprint = this->Fingerprint(e);

        InsertFingerprintInLastBucket(address, fingerprint);

        return true;
}

template <class T> bool QQHTDFilter<T>::InsertFingerprintInLastBucket(const uint64_t address, const uint64_t fingerprint) {
        /**
         * In QQHTD, buckets behave like a queue. Therefore each element is inserted at the end of the queue.
         * Using a linked list would require additional bits of data (for storing pointers).
         * Therefore we manually reorder the buckets in the insertion.
         *
         * @param size_t address
         * @param uint64_t fingerprint
         *
         * @returns bool true
         */
        for(size_t i = 0; i < this->n_buckets - 1; ++i) {
                this->InsertFingerprintInBucket(address, i, this->GetFingerprintFromBucket(address, i + 1));
        }

        this->InsertFingerprintInBucket(address, this->n_buckets - 1, fingerprint);

        return true;
}

template <class T> bool QQHTDFilter<T>::Delete(const T& e) {
        /**
         * Overrides QHT's Delete method.
         * Same principle applies
         *
         * @param T e : the element to be deleted
         * @returns bool : true if deleted, false if not found
         */

        bool element_found = false;

        size_t address = Hash1(e) % this->n_cells;
        auto fingerprint = this->Fingerprint(e);

        size_t i = 0;
        while(! element_found && i < this->n_buckets) {
                if(this->GetFingerprintFromBucket(address, i) == fingerprint) {
                        element_found = true;
                        // InsertFingerprintInBucket(address, i, 0);
                } else {
                        ++i;
                }
        }

        if(! element_found) {
                return false;
        }

        for(; i < this->n_buckets - 1; ++i) {
                this->InsertFingerprintInBucket(address, i, this->GetFingerprintFromBucket(address, i + 1));
        }

        this->InsertFingerprintInBucket(address, this->n_buckets - 1, 0);

        return true;
}
