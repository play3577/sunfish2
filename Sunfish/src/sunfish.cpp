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
	std::cout << position.toStringEffect() << '\n';

	position.moveUnsafe(Shogi::Move(Shogi::Square(7, 7), Shogi::Square(7, 6), false, false, Shogi::Piece::PAWN));
	position.moveUnsafe(Shogi::Move(Shogi::Square(3, 1), Shogi::Square(4, 2), false, false, Shogi::Piece::PAWN));
	position.moveUnsafe(Shogi::Move(Shogi::Square(8, 8), Shogi::Square(3, 3), true, false, Shogi::Piece::PAWN));

	std::cout << position.toString() << '\n';
	std::cout << position.toStringEffect() << '\n';

	Shogi::MoveGenerator gen(position);
	gen.generate();
	const Shogi::Move* pmove;
	while ((pmove = gen.next()) != NULL) {
		std::cout << pmove->toString() << ' ';
	}
	std::cout << '\n';

//	Shogi::MoveGenerator gen(position);
//	boost::mt19937 rgen(static_cast<unsigned>(time(NULL)));
//	for (int i = 0; i < 10; i++) {
//		gen.generate();
//		if (gen.getNumber() == 0) {
//			std::cout << "There is no a legal move.\n";
//			break;
//		}
//		boost::uniform_smallint<> dst(0, gen.getNumber()-1);
//		boost::variate_generator<boost::mt19937&, boost::uniform_smallint<> > r(rgen, dst);
//		const Shogi::Move& move = gen.get(r());
//		position.moveUnsafe(move);
//		std::cout << move.toString() << '\n';
//		std::cout << position.toString();
//	}

	return 0;
}
