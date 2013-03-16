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
		const char* filename;
		Book book;

	public:
		BookManager(const char* filename = "book",
				bool autoRead = true)
				: filename(filename) {
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

		bool importFile(const char* path);

		bool importDirectory(const char* directory);

		const Shogi::Move* getMove(Util::uint64 hash) {
			return book.getMove(hash);
		}

		const BookMoves* getAllMoves(Util::uint64 hash) const {
			return book.getAllMoves(hash);
		}
	};
}

#endif // BOOKMANAGER_H_
