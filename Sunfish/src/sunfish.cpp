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

	/*
	std::cout << pos.toStringCsa();
	std::cout << pos.toStringEffect();
	std::cout << pos.toStringBPawns() << '\n';
	std::cout << pos.toStringWPawns() << '\n';

	Shogi::MoveGenerator gen(pos);
	gen.generate();
	gen.sort();
	Shogi::MoveGenerator gen2(pos);
	gen2.generateTardy();
	gen2.sort();
	const Shogi::Move* pmove;
	while ((pmove = gen.next()) != NULL) {
		std::cout << pmove->toString() << ' ';
	}
	std::cout << std::endl;
	while ((pmove = gen2.next()) != NULL) {
		std::cout << pmove->toString() << ' '; // << pmove->toString4Debug() << ' ' << (unsigned)*pmove << ' ';
	}
	std::cout << std::endl;
	*/

	return 0;
}
