/*
 * hand.h
 *
 *  Created on: 2012/05/12
 *      Author: ryosuke
 */

#ifndef HAND_H_
#define HAND_H_

#include <cstring>
#include "piece.h"

namespace Shogi {
	class Hand {
	private:
		int hand[Piece::ROOK+1];

	public:
		Hand() {
			init();
		}

		void init() {
			memset(hand, 0, sizeof(hand));
		}

		void set(Piece& piece, int num) {
			hand[piece.getInteger()] = num;
		}

		void inc(Piece& piece) {
			hand[piece.getInteger()]++;
		}

		void dec(Piece& piece) {
			hand[piece.getInteger()]--;
		}

		int get(const Piece& piece) const {
			return hand[piece.getInteger()];
		}

		std::string toString() const;
	};
}

#endif /* HAND_H_ */
