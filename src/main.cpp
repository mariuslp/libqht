#include <cstdlib>
#include <iostream>
#include "qht.h"
#include "qqhtd.h"
#include "xxhash.h"

int main() {

	// basic_string<char> elements
	QHTFilter<std::basic_string<char>> bla = QHTFilter<std::basic_string<char>>(65000, 1, 1);

	bla.Stream(std::basic_string<char>("42"));

	bla.Stream("43");
	bla.Delete("42");
	std::cout << bla.Lookup("42") << std::endl;
	std::cout << bla.Lookup("43") << std::endl;

	// vector<T> elements
	QQHTDFilter<std::vector<int>> filter2 = QQHTDFilter<std::vector<int>>(65000, 2, 1);
	
	std::vector<int> e2_1 = {1, 2, 3};
	filter2.Stream(e2_1);
	filter2.Lookup(e2_1);

	//
	QQHTDFilter<std::pair<int*, int>> filter3 = QQHTDFilter<std::pair<int*, int>>(65000, 2, 1);
	
	auto e3_1 = {1, 2, 3};
	filter2.Stream(std::pair<int*, int>(e3_1, 3));
	filter2.Lookup(std::pair<int*, int>(e3_1, 3));



	return 0;
}


