/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <boost/thread.hpp>
#include <iostream>
#include "sunfish.h"
#include "moveGenerator.h"

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	Shogi::Position position(Shogi::EVEN);
	std::cout << position.toString();

	/*
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 16; j++) {
			std::cout << position.getBoard(Shogi::Square(i*16+j)) << ' ';
		}
		std::cout << '\n';
	}
	*/

	position.turn();

	Shogi::MoveGenerator gen(position);
	const Shogi::Move* pmove;
	gen.generate();
	while ((pmove = gen.next()) != NULL) {
		std::cout << pmove->toString() << ' ';
	}

	return 0;
}
