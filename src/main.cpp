#include <cstdlib>
#include <iostream>
#include "qht.h"

int main() {
	QHTFilter bla = QHTFilter<Element>(65000, 1, 1);
	bla.Reset();

	bla.Insert<Element>(Element(42));
	std::cout << bla.Lookup<Element>(Element(43)) << std::endl;
}

