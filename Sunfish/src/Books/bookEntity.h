/*
 * bookEntity.h
 *
 *  Created on: 2013/03/10
 *      Author: ryosuke
 */

#ifndef BOOKENTITY_H_
#define BOOKENTITY_H_

#include "bookMoves.h"

namespace Books {
	class BookEntity {
	private:
		static const int NOT_EXISTS = -1;

		std::vector<BookMoves> chain;

		int getIndex(Util::uint64 hash) const {
			for (unsigned i = 0; i < chain.size(); i++) {
				if (chain[i].getHash() == hash) {
					return i;
				}
			}
			return NOT_EXISTS;
		}

	public:
		void init() {
		}

		int addMove(Util::uint64 hash, const Shogi::Move& move) {
			int index = getIndex(hash);
			if (index == NOT_EXISTS) {
				chain.push_back(BookMoves(hash, move));
				return 1;
			} else {
				return chain[index].addMove(move);
			}
		}

		void setMove(Util::uint64 hash, const Shogi::Move& move,
				unsigned count) {
			int index = getIndex(hash);
			if (index == NOT_EXISTS) {
				chain.push_back(BookMoves(hash, move, count));
			} else {
				chain[index].setMove(move, count);
			}
		}

		const Shogi::Move* getMove(Util::uint64 hash,
				Util::Random& random) const {
			int index = getIndex(hash);
			if (index != NOT_EXISTS) {
				return chain[index].getMove(random);
			}
			return NULL;
		}

		unsigned getSize() const {
			return chain.size();
		}

		const BookMoves& getMoves(unsigned index) const {
			return chain[index];
		}

		const BookMoves* getMoves(Util::uint64 hash) const {
			int index = getIndex(hash);
			if (index != NOT_EXISTS) {
				return &chain[index];
			}
			return NULL;
		}
	};
}

#endif // BOOKENTITY_H_
