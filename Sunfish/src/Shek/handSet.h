/*
 * handSet.h
 *
 *  Created on: 2012/11/03
 *      Author: ryosuke
 */

#ifndef HANDSET_H_
#define HANDSET_H_

#include "../Util/int.h"
#include "../Shogi/hand.h"
#include "shekStat.h"

namespace Shek {
	class HandSet {
	private:
		Util::uint64 handSet;

		void setPiece(const Shogi::Hand& hand,
				Shogi::Piece piece, int shift) {
			handSet <<= shift;
			handSet |= ((Util::uint64)1U << hand.get(piece)) - 1;
		}

	public:
		HandSet() {
		}

		HandSet(const Shogi::Hand& hand) {
			set(hand);
		}

		void set(const Shogi::Hand& hand) {
			handSet = (Util::uint64)0U;
			setPiece(hand, Shogi::Piece::PAWN, 0);
			setPiece(hand, Shogi::Piece::LANCE, 4);
			setPiece(hand, Shogi::Piece::KNIGHT, 4);
			setPiece(hand, Shogi::Piece::SILVER, 4);
			setPiece(hand, Shogi::Piece::GOLD, 4);
			setPiece(hand, Shogi::Piece::BISHOP, 2);
			setPiece(hand, Shogi::Piece::ROOK, 2);
		}

		ShekStat compareTo(const HandSet& h) const {
			if (handSet == h.handSet) {
				return EQUAL;
			} else if (handSet & (~h.handSet)) {
				if ((~handSet) & h.handSet) {
					return NONE;
				} else {
					return INCLUDE;
				}
			} else {
				return LESS;
			}
		}
	};
}
#endif // HANDSET_H_
