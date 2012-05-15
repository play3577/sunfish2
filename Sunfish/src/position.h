/*
 * position.h
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#ifndef POSITION_H_
#define POSITION_H_

#include <iostream>
#include "board.h"
#include "hand.h"
#include "move.h"
#include "effectBoard.h"

namespace Shogi {
	class Position {
	private:
		Board board;
		Hand blackHand;
		Hand whiteHand;
		EffectBoard effectBoard;
		bool black;

	public:
		Position(bool black = true) : black(black) {
			effectBoard.init(board);
		}

		Position(Handicap handicap) : board(handicap) {
			black = (handicap == EVEN);
			effectBoard.init(board);
		}

		Position(Handicap handicap, bool black) : board(handicap), black(black) {
			effectBoard.init(board);
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
