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
#include "../Evaluate/initializer.h"
#include "../Evaluate/evaluate.h"
#include "../Search/attackers.h"

namespace Tools{
	using namespace Shogi;
	using namespace Evaluate;
	using namespace Search;

	void Debug::Print( const char* str, ... ){
		std::ofstream file;
		char buf[1024];
		time_t t;
		struct tm* tmDate;
		char strDate[64];
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);

		va_list argx;
		va_start( argx, str );

		// ファイルオープン
		file.open( "debug.txt", std::ios::out | std::ios::app );
		if( file.is_open() ){
			// 時刻
			t = time( NULL );
			tmDate = localtime( &t );
			strftime( strDate, sizeof(strDate), "%F %H:%M:%S ", tmDate );

			// 出力文字列
			vsprintf( buf, str, argx );

			// 出力
			file << strDate << buf;
			file.close();
		}

		va_end( argx );
	}

	bool Debug::PositionOk(const Position pos) {
		// hash
		if (pos.getHash() != pos.generateHash()) {
			std::cout << "***** HASH CODE ERROR!!! *****\n";
			std::cout << std::hex << pos.generateHash() << '\n';
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

	void Debug::TreeSearch(Search::Tree& tree) {
		Value value = tree.evaluate();
		if (value != Value(0)) {
			std::cout << tree.toString();
			std::cout << (int)value << '\n';
			static char line[1024];
			std::cin.getline(line, sizeof(line));
		}
#if 0
		std::cout << tree.getDepth() << '/'
			<< tree.getMaxDepth() << '\n';
		std::cout << tree.toString();
#endif
#if 0
		static char line[1024];
		std::cin.getline(line, sizeof(line));
#endif
		if (tree.isMaxDepth()) {
			return;
		}
		tree.generateMoves();
		while (tree.next()) {
			tree.makeMove();
#if 0
			std::cout << tree.getChange()->toString() << '\n';
			std::cout << tree.getPrevMove()->toString() << '\n';
#endif
			if (!PositionOk(tree.getPosition())) {
				std::cout << tree.toString();
				abort();
			}
			TreeSearch(tree);
			tree.unmakeMove();
			if (!PositionOk(tree.getPosition())) {
				std::cout << tree.toString();
				abort();
			}
		}
	}

	bool Debug::TreeTest(const char* filename) {
		Position pos(EVEN);
		Param* pparam = new Param();
		History history;
		Initializer::apply(*pparam);
		if (filename) {
			Csa::CsaReader::read(filename, pos);
		}
		Search::Tree tree(*pparam, pos, history, 4);
		TreeSearch(tree);
		return true;
	}
}

#endif // NDEBUG
