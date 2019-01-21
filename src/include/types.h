#pragma once

#include <bitset>
#include <cstdint>
#include <vector>
#include <string>

/** Selectively inform the compiler that an argument is not used */
#define UNUSED(expr) (void)(expr);

/** Size of a stream element, in bits
 * Number of bits in which we will store an element.
 * Given that real data is 64-bits hashes, we require 64 bits
 */
const std::uint8_t element_size = 64;

/** Element */
typedef std::bitset<element_size> Element;

/** Hash value */
typedef std::size_t HashValue;

/** One bucket contains one fingerprint */
typedef std::uint8_t Bucket;

/** Vector of buckets */
typedef std::vector<Bucket> Buckets;
