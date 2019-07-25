# libQHT - C++ Quotient Hash Table Implementation

This is an implmentation of QHT and QQHTD, as described in the paper [*"Quotient Hash Tables - Efficiently Detecting Duplicates in Streaming Data"*](https://arxiv.org/abs/1901.04358), published in SAC'19. QHT is a very efficient duplicate detection algorithm.

This implementation is written in C++17. [A Rust implementation](https://github.com/ovheurdrive/qht-rs) is also available.

The current implementation also supports item deletion. Note however that by definition of probabilistic filters, we cannot be sure a deletion does not have side effects (i.e., also deleting another element with same hash and fingerprint at the same time).

# How to use

The file src/main.cpp contains examples, we reproduce here a basic use:
```
#include <qht.h>  // Or qqhtd.h

int main() {
    // Instanciate QHT of 1024 bits, 2 buckets per cell, 1 bit per bucket
    auto filter = QHTFilter<std::basic_string<char>>(6500, 2, 1);

    filter.Stream("42");  // Stores the element "42", returns false as 42 was unseen
    filter.Stream("43");
    filter.Delete("42");  // Removes the element "42" from the filter

    filter.Lookup("42");  // returns false
    filter.Lookup("43");  // returns true
    return 0;
}
```

Currently, a filter can store one of the following types:

* `const std::vector<T>&`
* `const std::basic_string<T>&`
* `const std::array<T, N>&`
* `const std::initializer_list<T>&`


QHT relies on xxhash64 hashing, especially, we use [RedSpah implementation](https://github.com/RedSpah/xxhash_cpp) (BSD-2 license, cloned the 2019/04/01). See xxhash\_cpp license in lib/xxhash64/LICENSE.
