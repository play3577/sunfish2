/*
 * debug.cpp
 *
 *  Created on: 2012/05/12
 *      Author: ryosuke
 */


#ifndef NDEBUG

#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <ctime>
#include <fstream>
#include <cstdarg>
#include "debug.h"
#include "../Csa/csaReader.h"
#include "../Shogi/moveGenerator.h"
#include "../Evaluates/initializer.h"
#include "../Evaluates/evaluate.h"
#include "../Search/attackers.h"

namespace Tools{
	using namespace Shogi;
	using namespace Evaluates;
	using namespace Search;

	bool Debug::PositionOk(const Position pos) {
		// hash
		Util::uint64 hash = pos.generateBoardHash() ^ pos.generateHandHash() ^ pos.getTurnHash();
		if (pos.getHash() != hash) {
			std::cout << "***** HASH CODE ERROR!!! *****\n";
			std::cout << std::hex << hash << '\n';
			DEBUG_PRINT_LINE;
			std::cout.flush();
			return false;
		}
		// effect
		Position temp(pos);
		temp.update();
		if (!temp.getEffectBoard().equals(pos.getEffectBoard())) {
			std::cout << "***** EFFECT BOARD ERROR!!! *****\n";
			std::cout << temp.toStringEffect(true);
			DEBUG_PRINT_LINE;
			std::cout.flush();
			return false;;
		}
		return true;
	}

	bool Debug::GeneratorRandomTest(const char* filename) {
		Position pos(EVEN);
		if (filename) {
			Csa::CsaReader::read(filename, pos);
		}
		std::cout << pos.toStringCsa();
		boost::mt19937 rgen(static_cast<unsigned>(time(NULL)));
		for (int i = 0; i < 10000; i++) {
			std::cout << '[' << i << ']' << '\n';
			MoveGenerator gen(pos);
			gen.generate();
			MoveGenerator gen2(pos);
			gen2.generateTardy();
	
			gen.sort();
			gen2.sort();
			if (!gen.equals(gen2)) {
				const Move* pmove;
				while ((pmove = gen.next()) != NULL) {
					std::cout << pmove->toString() << ' ';
				}
				std::cout << std::endl;
				while ((pmove = gen2.next()) != NULL) {
					std::cout << pmove->toString() << ' ';
				}
				std::cout << std::endl;
				return false;
			}
	
			if (gen.getNumber() == 0) {
				std::cout << "There is no a legal move.\n";
				return true;
			}
	
			boost::uniform_smallint<> dst(0, gen.getNumber()-1);
			boost::variate_generator<boost::mt19937&, boost::uniform_smallint<> > r(rgen, dst);
			const Move& move = gen.get(r());
			pos.moveUnsafe(move);
			std::cout << move.toString() << '\n';
			std::cout << pos.toStringCsa();
			std::cout << pos.toStringEffect(true);
			std::cout << Util::Int::toString64(pos.getHash()) << '\n';
			std::cout.flush();

			if (!PositionOk(pos)) {
				return false;
			}
		}
		return true;
	}

	bool Debug::GeneratorTest(const char* filename) {
		Position pos(EVEN);
		if (filename) {
			Csa::CsaReader::read(filename, pos);
		}
		std::cout << pos.toStringCsa();
		std::cout << pos.toStringEffect();
		std::cout << pos.toStringBPawns() << '\n';
		std::cout << pos.toStringWPawns() << '\n';

		MoveGenerator gen(pos);
		gen.generate();
		gen.sort();
		MoveGenerator gen2(pos);
		gen2.generateTardy();
		gen2.sort();
		const Move* pmove;
		while ((pmove = gen.next()) != NULL) {
			std::cout << pmove->toString() << ' ';
		}
		std::cout << std::endl;
		while ((pmove = gen2.next()) != NULL) {
			std::cout << pmove->toString() << ' ';
		}
		std::cout << std::endl;
		return true;
	}

	bool Debug::PositionError(const Position& pos) {
		for (Square square = Square::TOP; square.valid(); square.next()) {
			Piece piece = pos.getBoard(square);
			if (piece.isWall()) {
				return true;
			}
		}
		return false;
	}
}

#endif // NDEBUG
