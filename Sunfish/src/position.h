/*
 * position.h
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#ifndef POSITION_H_
#define POSITION_H_

#include "board.h"
#include "hand.h"
#include "move.h"

namespace Shogi {
	class Position {
	private:
		Board board;
		Hand blackHand;
		Hand whiteHand;
		bool black;

	public:
		Position(bool black = true) : black(black) {
		}

		Position(Handicap handicap, bool black = true) : board(handicap), black(black) {
		}

		const Piece& getBoard(const Square& square) const {
			return board.get(square);
		}

		int getBlackHand(const Piece& piece) const {
			return blackHand.get(piece);
		}

		int getWhiteHand(const Piece& piece) const {
			return whiteHand.get(piece);
		}

		bool isBlackTurn() const {
			return black;
		}

		bool isWhiteTurn() const {
			return !black;
		}

		void turn() {
			black = !black;
		}

		template <bool black>
		bool move(const Move& move);

		void moveUnsafe(const Move& move) {
			if (black) {
				moveUnsafe<true>(move);
			} else {
				moveUnsafe<false>(move);
			}
		}

		template <bool black>
		void moveUnsafe(const Move& move);

		std::string toString() const;
	};
}

#endif /* POSITION_H_ */
