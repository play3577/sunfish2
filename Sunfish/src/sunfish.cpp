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

using namespace boost;

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	Shogi::Position position(Shogi::EVEN);
	std::cout << position.toString();
	std::cout << position.toStringEffect();
	std::cout << position.toStringBPawns() << '\n';
	std::cout << position.toStringWPawns() << '\n';

	position.moveUnsafe(Shogi::Move(Shogi::Square(7, 7), Shogi::Square(7, 6), false, false, Shogi::Piece::BPAWN));
	position.moveUnsafe(Shogi::Move(Shogi::Square(3, 3), Shogi::Square(3, 4), false, false, Shogi::Piece::WPAWN));
//	position.moveUnsafe(Shogi::Move(Shogi::Square(8, 8), Shogi::Square(2, 2), true, false, Shogi::Piece::BBISHOP));
//	position.moveUnsafe(Shogi::Move(Shogi::Square(3, 1), Shogi::Square(2, 2), false, false, Shogi::Piece::WSILVER));

	position.moveUnsafe(Shogi::Move(Shogi::Square(8, 8), Shogi::Square(7, 7), false, false, Shogi::Piece::BBISHOP));
	position.moveUnsafe(Shogi::Move(Shogi::Square(2, 2), Shogi::Square(7, 7), true, false, Shogi::Piece::WBISHOP));

//	position.moveUnsafe(Shogi::Move(Shogi::Square(2, 7), Shogi::Square(2, 6), false, false, Shogi::Piece::BPAWN));
//	position.moveUnsafe(Shogi::Move(Shogi::Square(2, 3), Shogi::Square(2, 4), false, false, Shogi::Piece::WPAWN));
//	position.moveUnsafe(Shogi::Move(Shogi::Square(2, 6), Shogi::Square(2, 5), false, false, Shogi::Piece::BPAWN));
//	position.moveUnsafe(Shogi::Move(Shogi::Square(2, 4), Shogi::Square(2, 5), false, false, Shogi::Piece::WPAWN));
//	position.moveUnsafe(Shogi::Move(Shogi::Square(2, 8), Shogi::Square(2, 5), false, false, Shogi::Piece::BROOK));
//	position.moveUnsafe(Shogi::Move(Shogi::Square::NON, Shogi::Square(2, 8), false, true, Shogi::Piece::WPAWN));

	std::cout << position.toString();
	std::cout << position.toStringEffect();
	std::cout << position.toStringBPawns() << '\n';
	std::cout << position.toStringWPawns() << '\n';

	Shogi::MoveGenerator gen(position);
	gen.generate();
	const Shogi::Move* pmove;
	while ((pmove = gen.next()) != NULL) {
		std::cout << pmove->toString() << ' ';
	}
	std::cout << '\n';

//	boost::mt19937 rgen(static_cast<unsigned>(time(NULL)));
//	for (int i = 0; i < 10000; i++) {
//		std::cout << i << '\n';
//		Shogi::MoveGenerator gen(position);
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
//		std::cout << position.toStringEffect(true);
//		std::cout.flush();
//
//		// Debug
//		Shogi::Position temp(position);
//		temp.update();
//		if (!temp.getEffectBoard().equals(position.getEffectBoard())) {
//			std::cout << '\n';
//			std::cout << temp.toStringEffect(true);
//			DEBUG_PRINT_LINE;
//			break;
//		}
//	}

	return 0;
}
