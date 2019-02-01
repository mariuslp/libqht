#include <cstdlib>
#include <iostream>
#include "qht.h"
#include "qqhtd.h"
#include "xxhash.h"

int main() {
	QHTFilter<std::basic_string<char>> bla = QHTFilter<std::basic_string<char>>(65000, 1, 1);

	bla.Stream(std::basic_string<char>("42"));
	std::cout << "yo" << std::endl;

	bla.Stream("43");
	bla.Delete("42");
	std::cout << bla.Lookup("42") << std::endl;
	std::cout << bla.Lookup("43") << std::endl;
	
	QQHTDFilter<std::basic_string<char>> blabla = QQHTDFilter<std::basic_string<char>>(65000, 1, 1);

	blabla.Stream("42");
	blabla.Stream("43");
	blabla.Delete("42");
	std::cout << blabla.Lookup("42") << std::endl;
	std::cout << blabla.Lookup("43") << std::endl;
}
