/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <boost/random.hpp>
#include <iostream>
#include "sunfish.h"
#include "moveGenerator.h"

using namespace boost;

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	Shogi::Position position(Shogi::EVEN);
	std::cout << position.toString();

	Shogi::MoveGenerator gen(position);
	boost::mt19937 rgen(static_cast<unsigned>(time(NULL)));
	for (int i = 0; i < 10; i++) {
		gen.generate();
		if (gen.getNumber() == 0) {
			std::cout << "There is no a legal move.\n";
			break;
		}
		boost::uniform_smallint<> dst(0, gen.getNumber()-1);
		boost::variate_generator<boost::mt19937&, boost::uniform_smallint<> > r(rgen, dst);
		const Shogi::Move& move = gen.get(r());
		position.moveUnsafe(move);
		std::cout << move.toString() << '\n';
		std::cout << position.toString();
	}

	return 0;
}
