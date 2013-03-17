/*
 * bookManager.h
 *
 *  Created on: 2013/03/10
 *      Author: ryosuke
 */

#ifndef BOOKMANAGER_H_
#define BOOKMANAGER_H_

#include "book.h"
#include "bookReader.h"
#include "bookWriter.h"

namespace Books {
	class BookManager {
	private:
		static const unsigned DEFAULT_BITS = 15;
		const static unsigned DEFAULT_COUNT_THRESHOLD = 2;
		const static unsigned DEFAULT_RECORD_THRESHOLD = 60;

		const char* filename;
		Book book;

	public:
		BookManager(const char* filename = "book",
				bool autoRead = true,
				unsigned bits = DEFAULT_BITS,
				unsigned countThreshold = DEFAULT_COUNT_THRESHOLD) :
				filename(filename),
				book(bits, countThreshold) {
			if (autoRead) {
				read();
			}
		}

		bool read() {
			return read(filename);
		}

		bool read(const char* filename) {
			return BookReader::read(filename, book);
			return false;
		}

		bool write() {
			return write(filename);
		}

		bool write(const char* filename) {
			return BookWriter::write(filename, book);
		}

		bool importFile(const char* path,
				int threshold = DEFAULT_RECORD_THRESHOLD);

		bool importDirectory(const char* directory,
				int threshold = DEFAULT_RECORD_THRESHOLD);

		const Shogi::Move* getMove(Util::uint64 hash) {
			return book.getMove(hash);
		}

		const BookMoves* getMoves(Util::uint64 hash) const {
			return book.getMoves(hash);
		}
	};
}

#endif // BOOKMANAGER_H_
