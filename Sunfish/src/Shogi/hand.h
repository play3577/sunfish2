/*
 * hand.h
 *
 *  Created on: 2012/05/12
 *      Author: ryosuke
 */

#ifndef HAND_H_
#define HAND_H_

#include <cstdlib>
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

		Hand(const Hand& hand) {
			init(hand);
		}

		void init() {
			memset((void*)hand, 0, sizeof(hand));
		}

		void init(const Hand& source) {
			memcpy((void*)hand, (const void*)source.hand, sizeof(hand));
		}

		void set(const Piece& piece, int num) {
			hand[piece.getTurnedBlack()] = num;
		}

		void inc(const Piece& piece) {
			hand[piece.getTurnedBlack()]++;
		}

		void dec(const Piece& piece) {
			hand[piece.getTurnedBlack()]--;
		}

		int get(const Piece& piece) const {
			return hand[piece.getTurnedBlack()];
		}

		std::string toString() const;
	};
}

#endif /* HAND_H_ */
