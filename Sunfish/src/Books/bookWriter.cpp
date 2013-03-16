/*
 * bookWriter.cpp
 *
 *  Created on: 2013/03/10
 *      Author: ryosuke
 */

#include "bookWriter.h"
#include "../Log/logger.h"
#include <fstream>

namespace Books {
	using namespace Shogi;

	bool BookWriter::write(const char* filename, const Book& book) {
		Log::message << "writing opening-books:[" << filename << "]\n";
		std::ofstream fout(filename, std::ios::out | std::ios::binary);
		if (!fout) {
			Log::warning << "could not open:[" << filename << "]\n";
			return false;
		}
		bool ok = true;
		for (unsigned index = 0; index < book.getSize(); index++) {
			const BookEntity& entity = book.getEntity(index);
			if (!write(fout, entity)) {
				ok = false;
				break;
			}
		}
		fout.close();
		return ok;
	}

	bool BookWriter::write(std::ostream& out, const BookEntity& entity) {
		for (unsigned index = 0; index < entity.getSize(); index++) {
			const BookMoves& moves = entity.getMoves(index);
			if (!write(out, moves)) {
				return false;
			}
		}
		return true;
	}

	bool BookWriter::write(std::ostream& out, const BookMoves& moves) {
		if (moves.getSize() > 0) {
			Util::uint64 hash = moves.getHash();
			out.write((char*)&hash, sizeof(hash));
			for (unsigned index = 0; index < moves.getSize(); index++) {
				unsigned move = (unsigned)*moves.getMove(index);
				unsigned count = moves.getCount(index);
				out.write((char*)&move, sizeof(move));
				out.write((char*)&count, sizeof(count));
			}
			unsigned empty = 0;
			out.write((char*)&empty, sizeof(empty));
		}
		return true;
	}
}
