/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <boost/thread.hpp>
#include <iostream>
#include "sunfish.h"
#include "position.h"

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	Shogi::Position position(Shogi::EVEN);
	std::cout << position.toString();

	return 0;
}
