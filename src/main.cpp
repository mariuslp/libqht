//#include <cstdlib>
#include <iostream>
#include "qht.h"
#include "qqhtd.h"
//#include "xxhash.h"

int main() {

// basic_string<char> elements
	auto filter1 = QHTFilter<std::basic_string<char>>(65000, 1, 1);

    // Basic usage of Stream, Delete and Lookup
	filter1.Stream("42");
	filter1.Stream("43");
	filter1.Delete("42");
	std::cout << filter1.Lookup("42") << std::endl;
	std::cout << filter1.Lookup("43") << std::endl;

// vector<T> elements
	auto filter2 = QQHTDFilter<std::vector<int>>(65000, 2, 1);
	
	std::vector<int> e2 = {1, 2, 3};
	filter2.Stream(e2);
	filter2.Lookup(e2);
    filter2.Delete(e2);

// std::initializer_list<T> elements
	auto filter3 = QQHTDFilter<std::initializer_list<int>>(65000, 2, 1);
	
    std::initializer_list<int> e3 = {1, 2, 3};
	filter3.Stream(e3);
	filter3.Lookup(e3);
    filter3.Delete(e3);

// std::array<T, N> elements
    auto filter4 = QQHTDFilter<std::array<int, 4>>(6500, 2, 1);

    std::array<int, 4> e4 = {1, 2, 3, 4};
    filter4.Stream(e4);
    filter4.Lookup(e4);
    filter4.Delete(e4);

    return 0;
}


