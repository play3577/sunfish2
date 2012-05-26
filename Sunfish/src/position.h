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
		Square bking;
		Square wking;
		bool black;

	public:
		Position(bool black = true) : black(black) {
			effectBoard.init(board);
			bking = board.getKingSquare<true>();
			wking = board.getKingSquare<false>();
		}

		Position(Handicap handicap) : board(handicap) {
			black = (handicap == EVEN);
			effectBoard.init(board);
			bking = board.getKingSquare<true>();
			wking = board.getKingSquare<false>();
		}

		Position(Handicap handicap, bool black) : board(handicap), black(black) {
			effectBoard.init(board);
			bking = board.getKingSquare<true>();
			wking = board.getKingSquare<false>();
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

		bool isCheck() const {
			if (black) {
				return (bool)effectBoard.get<false>(bking);
			} else {
				return (bool)effectBoard.get<true>(wking);
			}
		}

		template <bool black>
		DirectionFlags pin(const Square& sq) const {
			if (black) {
				return effectBoard.get<true>(sq).pin(effectBoard.get<false>(sq));
			} else {
				return effectBoard.get<false>(sq).pin(effectBoard.get<true>(sq));
			}
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

		std::string toStringEffect() const {
			return effectBoard.toString();
		}
	};
}

#endif /* POSITION_H_ */
