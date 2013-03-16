/*
 * bookMoves.h
 *
 *  Created on: 2013/03/10
 *      Author: ryosuke
 */

#ifndef BOOKMOVES_H_
#define BOOKMOVES_H_

#include "bookMove.h"
#include "../Util/int.h"
#include "../Util/random.h"
#include <vector>

namespace Books {
	class BookMoves {
	private:
		static const int NOT_EXISTS = -1;

		Util::uint64 hash;
		std::vector<BookMove> moves;
		unsigned count;

		int getIndex(const Shogi::Move& move) const {
			for (unsigned i = 0; i < moves.size(); i++) {
				if (moves[i].is(move)) {
					return i;
				}
			}
			return NOT_EXISTS;
		}

	public:
		BookMoves(const BookMoves& bookMoves) :
				hash(bookMoves.hash),
				moves(bookMoves.moves),
				count(bookMoves.count) {
		}

		BookMoves(Util::uint64 hash) {
			this->hash = hash;
			count = 0;
		}

		BookMoves(Util::uint64 hash, const Shogi::Move& move) {
			this->hash = hash;
			moves.push_back(BookMove(move));
			count = 1;
		}

		BookMoves(Util::uint64 hash, const Shogi::Move& move,
				unsigned count) {
			this->hash = hash;
			moves.push_back(BookMove(move));
			this->count = count;
		}

		Util::uint64 getHash() const {
			return hash;
		}

		int addMove(const Shogi::Move& move) {
			count++;
			int index = getIndex(move);
			if (index == NOT_EXISTS) {
				moves.push_back(BookMove(move));
				return 1;
			} else {
				return moves[index].addCount();
			}
		}

		void setMove(const Shogi::Move& move, unsigned count,
				bool overwrite = true) {
			this->count += count;
			int index;
			if (overwrite &&
					(index = getIndex(move)) != NOT_EXISTS) {
				moves[index].setCount(count);
			} else {
				moves.push_back(BookMove(move, count));
			}
		}

		unsigned getCount() const {
			return count;
		}

		unsigned getSize() const {
			return moves.size();
		}

		unsigned getCount(unsigned index) const {
			if (index >= moves.size()) {
				return 0;
			}
			return moves[index].getCount();
		}

		const Shogi::Move* getMove(unsigned index) const {
			if (index >= moves.size()) {
				return NULL;
			}
			return &moves[index].getMove();
		}

		const Shogi::Move* getMove(Util::Random& random) const {
			unsigned r = random.getInt32(count);
			unsigned i = 0;
			for (; i < moves.size(); i++) {
				unsigned c = moves[i].getCount();
				if (r < c) {
					return &moves[i].getMove();
				}
				r -= c;
			}
			return NULL;
		}
	};
}

#endif // BOOKMOVES_H_
