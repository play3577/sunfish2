/*
 * csaReader.h
 * 
 *  Created on: 2012/06/09
 *      Author: Kubo Ryosuke
 */

#ifndef CSA_READER_H_
#define CSA_READER_H_

#include <iostream>
#include "csa.h"
#include "../Record/record.h"

namespace Csa {
	class CsaReader {
		private:
			enum LineStat {
				LINE_ERROR,
				LINE_EMPTY,
				LINE_TURN,
				LINE_BOARD,
				LINE_HAND,
				LINE_MOVE,
			};

			static const int LINE_BUFFER_SIZE = 1024;
			CsaReader() {
			}
			static bool parseLineBoard(const char* line, unsigned rank, Shogi::Position& pos);
			static bool parseLineHand(const char* line, Shogi::Position& pos, bool black);
			static LineStat parseLine(const char* line, Shogi::Position& pos);
			static LineStat parseLine(const char* line, Record::Record& pos);
			static bool read(std::istream& in, Shogi::Position& pos);

		public:
			static bool read(const char* filename, Shogi::Position& pos);
			static bool read(const char* filename, Record::Record& record);
	};
}

#endif // CSA_READER_H_
