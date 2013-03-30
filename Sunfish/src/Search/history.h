/*
 * history.h
 *
 *  Created on: 2012/07/14
 *      Author: ryosuke
 */

#ifndef HISTORY_H_
#define HISTORY_H_

#include "../Shogi/move.h"

namespace Search {
	class History {
	private:
		static const unsigned FROM_BOARD = Shogi::Square::END + 1;
		static const unsigned FROM_HAND = Shogi::Piece::ROOK + 1;
		static const unsigned FROM = FROM_BOARD + FROM_HAND;
		static const unsigned TO = Shogi::Square::END + 1;
		volatile unsigned (*all)[TO];
		volatile unsigned (*good)[TO];

		unsigned from(const Shogi::Move& move) const {
			if (move.isHand()) {
				return FROM_BOARD + move.getPiece().getTurnedBlack();
			} else {
				return move.getFrom();
			}
		}

	public:
		static const unsigned SCALE = 0x100;

		History() {
			all = new unsigned[FROM][TO];
			good = new unsigned[FROM][TO];
			assert(all != NULL);
			assert(good != NULL);
		}

		virtual ~History() {
			delete[] all;
			delete[] good;
		}

		void clear() {
			memset((void*)all, 0, sizeof(unsigned) * FROM * TO);
			memset((void*)good, 0, sizeof(unsigned) * FROM * TO);
		}

		void addAppear(const Shogi::Move& move, int depth) {
			unsigned index = from(move);
			if (all[index][move.getTo()] * SCALE >= UINT_MAX - depth * SCALE) {
				all[index][move.getTo()] /= 2U;
				good[index][move.getTo()] /= 2U;
			}
			all[index][move.getTo()] += depth;
		}

		void addGood(const Shogi::Move& move, int depth) {
			unsigned index = from(move);
			assert(index < FROM);
			assert(move.getTo() < TO);
			good[index][move.getTo()] += depth;
		}

		unsigned get(const Shogi::Move& move) const {
			unsigned index = from(move);
			assert(index < FROM);
			assert(move.getTo() < TO);
			unsigned g = good[index][move.getTo()];
			unsigned a = all[index][move.getTo()];
			return (g + 1) * SCALE / (a + 2);
		}
	};
}

#endif // HISTORY_H_
