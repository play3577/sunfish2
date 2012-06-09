/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <boost/random.hpp>
#include <iostream>
#include "Debug/debug.h"
#include "sunfish.h"
#include "Shogi/moveGenerator.h"
#include "Csa/csaReader.h"

using namespace boost;

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	Shogi::Position pos(Shogi::EVEN);

	Csa::CsaReader::read("test.csa", pos);

	std::cout << pos.toString();
	std::cout << pos.toStringEffect();
	std::cout << pos.toStringBPawns() << '\n';
	std::cout << pos.toStringWPawns() << '\n';

	Shogi::MoveGenerator gen(pos);
	gen.generate();
	const Shogi::Move* pmove;
	while ((pmove = gen.next()) != NULL) {
		std::cout << pmove->toString() << ' ';
	}
	std::cout << std::endl;
/*
	boost::mt19937 rgen(static_cast<unsigned>(time(NULL)));
	for (int i = 0; i < 10000; i++) {
		std::cout << '[' << i << ']' << '\n';
		Shogi::MoveGenerator gen(pos);
		gen.generate();
		Shogi::MoveGenerator gen2(pos);
		gen2.generateTardy();

		gen.sort();
		gen2.sort();
		if (!gen.equals(gen2)) {
			const Shogi::Move* pmove;
			while ((pmove = gen.next()) != NULL) {
				std::cout << pmove->toString() << ' '; // << pmove->toString4Debug() << ' ' << (unsigned)*pmove << ' ';
			}
			std::cout << std::endl;
			while ((pmove = gen2.next()) != NULL) {
				std::cout << pmove->toString() << ' '; // << pmove->toString4Debug() << ' ' << (unsigned)*pmove << ' ';
			}
			std::cout << std::endl;
			break;
		}

		if (gen.getNumber() == 0) {
			std::cout << "There is no a legal move.\n";
			break;
		}

		boost::uniform_smallint<> dst(0, gen.getNumber()-1);
		boost::variate_generator<boost::mt19937&, boost::uniform_smallint<> > r(rgen, dst);
		const Shogi::Move& move = gen.get(r());
		pos.moveUnsafe(move);
		std::cout << move.toString() << '\n';
		std::cout << pos.toString();
		std::cout << pos.toStringEffect(true);
		std::cout.flush();

		// effect
		Shogi::Position temp(pos);
		temp.update();
		if (!temp.getEffectBoard().equals(pos.getEffectBoard())) {
			std::cout << '\n';
			std::cout << temp.toStringEffect(true);
			DEBUG_PRINT_LINE;
			break;
		}
	}
*/

	return 0;
}
