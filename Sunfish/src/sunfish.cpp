/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <iostream>
#include "Tools/debug.h"
#include "sunfish.h"

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	const char* filename = NULL;
	if (argc >= 2) {
		filename = argv[1];
	}

	Tools::Debug::GeneratorRandomTest(filename);

	return 0;
}
