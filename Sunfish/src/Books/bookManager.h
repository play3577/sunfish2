/*
 * bookManager.h
 *
 *  Created on: 2013/03/10
 *      Author: ryosuke
 */

#ifndef BOOKMANAGER_H_
#define BOOKMANAGER_H_

#include "book.h"
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

		bool read(const char*) {
			// TODO
			return false;
		}

		bool write() {
			return write(filename);
		}

		bool write(const char* filename) {
			return BookWriter::write(filename, book);
		}

		bool import(const char* dirname);
	};
}

#endif // BOOKMANAGER_H_
