#include <cstdlib>
#include <iostream>
#include "qht.h"

int main() {
	QHTFilter<uint8_t> bla = QHTFilter<uint8_t>(65000, 1, 1);

	bla.Insert(42);
        bla.Insert(43);
        bla.Delete(42);
	std::cout << bla.Lookup(42) << std::endl;
	std::cout << bla.Lookup(43) << std::endl;
}

