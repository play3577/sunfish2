/*
 * shogiTest.cpp
 *
 *  Created on: 2013/02/02
 *      Author: ryosuke
 */

#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <ctime>
#include "../Csa/csaReader.h"
#include "../Shogi/moveGenerator.h"
#include "../Evaluates/initializer.h"
#include "../Evaluates/evaluate.h"
#include "../Search/attackers.h"
#include "../Tools/debug.h"
#include "shogiTest.h"

#ifndef NDEBUG
namespace Tests {
	using namespace Shogi;
	using namespace Evaluates;
	using namespace Search;
	using namespace Tools;

	Test::TestResult ShogiTest::generatorRandomTest(const char* filename) {
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
				return TEST_RESULT(false);
			}
	
			if (gen.getNumber() == 0) {
				std::cout << "There is no a legal move.\n";
				return TEST_RESULT(true);
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

			if (!Debug::positionOk(pos)) {
				return TEST_RESULT(false);
			}
		}
		return TEST_RESULT(true);
	}

	Test::TestResult ShogiTest::generatorTest(const char* filename) {
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
		return TEST_RESULT(true);
	}
}
#endif // NDEBUG