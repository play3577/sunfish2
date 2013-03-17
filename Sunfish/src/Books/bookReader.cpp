/*
 * bookReader.cpp
 *
 *  Created on: 2013/03/14
 *      Author: ryosuke
 */

#include "bookReader.h"
#include "../Log/logger.h"
#include <fstream>

namespace Books {
	using namespace Shogi;

	bool BookReader::read(const char* filename, Book& book) {
		Log::message << "reading a file:[" << filename << "]\n";
		std::ifstream fin(filename, std::ios::in | std::ios::binary);
		if (!fin) {
			Log::warning << "could not open:[" << filename << "]\n";
			return false;
		}
		bool ok = read(fin, book);
		fin.close();
		return ok;
	}

	bool BookReader::read(std::istream& in, Book& book) {
		while (true) {
			Util::uint64 hash;
			in.read((char*)&hash, sizeof(hash));
			if (in.eof()) { break; }
			if (!in) {
				Log::error << __THIS__ << ": unknown error\n";
			}
			if (!read(in, book, hash)) {
				return false;
			}
		}
		return true;
	}

	bool BookReader::read(std::istream& in, Book& book, Util::uint64 hash) {
		BookMoves& bookMoves = book.putMoves(hash, false);
		while (true) {
			unsigned m, count;
			in.read((char*)&m, sizeof(m));
			if (in && m == 0) { break; }
			in.read((char*)&count, sizeof(count));
			if (in.eof()) {
				Log::error << __THIS__ << ": invalid format\n";
				return false;
			}
			if (!in) {
				Log::error << __THIS__ << ": unknown error\n";
			}
			Move move(m);
			bookMoves.setMove(move, count, false);
		}
		return true;
	}
}
