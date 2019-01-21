#include "qht.h"

template <class T> QHTFilter<T>::QHTFilter(
	const uint64_t memory_size,
	const size_t n_n_buckets,
	const size_t n_fingerprint_size
) : n_buckets(n_n_buckets), fingerprint_size(n_fingerprint_size)
{
	n_cells = memory_size / (n_buckets * fingerprint_size);
	assert(n_cells > 0);
	assert(fingerprint_size < 8); // Fingerprints are stored in uint8_t
}


template <class T> void QHTFilter<T>::Reset() {
	qht.assign(n_cells * n_buckets * fingerprint_size, 0);
}
