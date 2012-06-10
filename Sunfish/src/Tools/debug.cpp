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

namespace Tools{
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

	bool Debug::GeneratorRandomTest(const char* filename) {
		Shogi::Position pos(Shogi::EVEN);
		if (filename) {
			Csa::CsaReader::read(filename, pos);
		}
		std::cout << pos.toStringCsa();
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
				return false;;
			}
		}
		return true;
	}
}

#endif // NDEBUG
