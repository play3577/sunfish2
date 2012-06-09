/*
 * csaReader.h
 * 
 *  Created on: 2012/06/09
 *      Author: Kubo Ryosuke
 */

#ifndef CSA_READER_H_
#define CSA_READER_H_

#include "csa.h"
#include "../Shogi/position.h"

namespace Csa {
	class CsaReader {
		private:
			static const int LINE_BUFFER_SIZE = 1024;
			CsaReader() {
			}
			static bool parseLineBoard(const char* line, unsigned rank, Shogi::Position& pos);
			static bool parseLineHand(const char* line, Shogi::Position& pos, bool black);
			static bool parseLine(const char* line, Shogi::Position& pos);

		public:
			static bool read(const char* filename, Shogi::Position& pos);
	};
}

#endif // CSA_READER_H_
