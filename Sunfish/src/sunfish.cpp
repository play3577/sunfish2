/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <iostream>
#include "sunfish.h"
#include "board.h"

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << '\n';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	Shogi::Board board(Shogi::EVEN);
	std::cout << board.toString();

	return 0;
}
