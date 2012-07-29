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

	Cui::Controller controller;
	controller.init(argc, argv);
	controller.play();

	return 0;
}
