/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <iostream>
#include "Cui/controller.h"
#include "Shogi/position.h"
#include "sunfish.h"

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	Shogi::PositionHash hash(Shogi::PositionHash::FILE_NAME);
	Shogi::Position::setPositionHash(&hash);

#if 0 // tree debug
	const char* filename = NULL;
	if (argc >= 2) {
		filename = argv[1];
	}

	Tools::Debug::GeneratorRandomTest(filename);
#else
	Cui::Controller controller;
	controller.play();
#endif

	return 0;
}
