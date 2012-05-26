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
#include "pawnFlags.h"

namespace Shogi {
	class Position {
	private:
		Board board;
		Hand blackHand;
		Hand whiteHand;
		EffectBoard effectBoard;
		Square bking;
		Square wking;
		PawnFlags bpawns;
		PawnFlags wpawns;
		bool black;

		void update() {
			effectBoard.init(board);
			bking = board.getKingSquare<true>();
			wking = board.getKingSquare<false>();
			bpawns = board.getPawnFiles<true>();
			wpawns = board.getPawnFiles<false>();
		}

	public:
		Position(bool black = true) : black(black) {
			update();
		}

		Position(Handicap handicap) : board(handicap) {
			black = (handicap == EVEN);
			update();
		}

		Position(Handicap handicap, bool black) : board(handicap), black(black) {
			update();
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

		const DirectionFlags& getEffect(const Square& square, bool black) const {
			if (black) {
				return effectBoard.get<true>(square);
			} else {
				return effectBoard.get<false>(square);
			}
		}

		bool isBlackTurn() const {
			return black;
		}

		bool isWhiteTurn() const {
			return !black;
		}

		Square getBKing() const {
			return bking;
		}

		Square getWKing() const {
			return wking;
		}

		PawnFlags getBPawnFiles() const {
			return bpawns;
		}

		PawnFlags getWPawnFiles() const {
			return wpawns;
		}

		bool isCheck() const {
			if (black) {
				return (bool)effectBoard.get<false>(bking);
			} else {
				return (bool)effectBoard.get<true>(wking);
			}
		}

		DirectionFlags pin(const Square& sq, bool black) const {
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

		std::string toStringBPawns() const {
			return bpawns.toString();
		}

		std::string toStringWPawns() const {
			return wpawns.toString();
		}
	};
}

#endif /* POSITION_H_ */
