/*
 * positionHash.cpp
 *
 *  Created on: 2012/06/10
 *      Author: ryosuke
 */

#include <fstream>
#include <iostream>
#include <boost/random.hpp>
#include "positionHash.h"
#include "../Log/logger.h"

namespace Shogi {
	const char* PositionHash::FILE_NAME = "hash";

	void PositionHash::init() {
		boost::mt19937 rgen(static_cast<unsigned>(time(NULL)));
		boost::uniform_int<> dst(0, 0xffff);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > r(rgen, dst);

		for (unsigned i = 0; i < sizeof(board) / sizeof(board[0][0]); i++) {
			board[0][i] = ((Util::uint64)r() << 48) ^ ((Util::uint64)r() << 32)
					^ ((Util::uint64)r() << 16) ^ (Util::uint64)r();
		}
		for (unsigned i = 0; i < sizeof(blackHand) / sizeof(blackHand[0][0]); i++) {
			blackHand[0][i] = ((Util::uint64)r() << 48) ^ ((Util::uint64)r() << 32)
					^ ((Util::uint64)r() << 16) ^ (Util::uint64)r();
		}
		for (unsigned i = 0; i < sizeof(whiteHand) / sizeof(whiteHand[0][0]); i++) {
			whiteHand[0][i] = ((Util::uint64)r() << 48) ^ ((Util::uint64)r() << 32)
					^ ((Util::uint64)r() << 16) ^ (Util::uint64)r();
		}
		black = ((Util::uint64)r() << 48) ^ ((Util::uint64)r() << 32)
				^ ((Util::uint64)r() << 16) ^ (Util::uint64)r();
	}

	bool PositionHash::read(const char* filename) {
		std::ifstream fin(filename, std::ios::in | std::ios::binary);
		if (!fin) {
			Log::error << "ERROR : can not open.. \"" << filename << "\"\n";
			return false;
		}
		fin.read((char*)board, sizeof(board));
		fin.read((char*)blackHand, sizeof(blackHand));
		fin.read((char*)whiteHand, sizeof(whiteHand));
		fin.read((char*)&black, sizeof(black));
		if (fin.fail()) {
			Log::error << "ERROR : failed to read.. \"" << filename << "\"\n";
			return false;
		}
		fin.close();
		return true;
	}

	bool PositionHash::write(const char* filename) {
		std::ofstream fout(filename, std::ios::out | std::ios::binary);
		if (!fout) {
			Log::error << "ERROR : can not open.. \"" << filename << "\"\n";
			return false;
		}
		fout.write((char*)board, sizeof(board));
		fout.write((char*)blackHand, sizeof(blackHand));
		fout.write((char*)whiteHand, sizeof(whiteHand));
		fout.write((char*)&black, sizeof(black));
		if (fout.fail()) {
			Log::error << "ERROR : failed to read.. \"" << filename << "\"\n";
			return false;
		}
		fout.close();
		return true;
	}
}
