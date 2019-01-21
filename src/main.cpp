#include <cstdlib>
#include <iostream>
#include "qht.h"

int main() {
	QHTFilter<uint8_t> bla = QHTFilter<uint8_t>(65000, 1, 1);
	bla.Reset();

	bla.Insert(42);
	std::cout << bla.Lookup(42) << std::endl;
}

