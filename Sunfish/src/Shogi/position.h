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

		template <bool black>
		bool isLegalMove(const Move& move) const;

		template <bool black>
		void moveUnsafe(const Move& move);

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

		Position(const Position& position) : board(position.board), blackHand(position.blackHand),
				whiteHand(position.whiteHand), effectBoard(position.effectBoard),
				bking(position.bking), wking(position.wking),
				bpawns(position.bpawns), wpawns(position.wpawns) {
		}

		void update() {
			effectBoard.init(board);
			bking = board.getKingSquare<true>();
			wking = board.getKingSquare<false>();
			bpawns = board.getPawnFiles<true>();
			wpawns = board.getPawnFiles<false>();
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

		int getHand(const Piece& piece) const {
			return black ? getBlackHand(piece) : getWhiteHand(piece);
		}

		const DirectionFlags& getEffect(const Square& square, bool black) const {
			if (black) {
				return effectBoard.get<true>(square);
			} else {
				return effectBoard.get<false>(square);
			}
		}

		const EffectBoard& getEffectBoard() const {
			return effectBoard;
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
				return (bool)effectBoard.get<false>(bking).getExcludeKing();
			} else {
				return (bool)effectBoard.get<true>(wking).getExcludeKing();
			}
		}

		DirectionFlags getCheckDirection() const {
			if (black) {
				return effectBoard.get<false>(bking).getExcludeKing();
			} else {
				return effectBoard.get<true>(wking).getExcludeKing();
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

		bool isLegalMove(const Move& move) const {
			return (black ? isLegalMove<true>(move) : isLegalMove<false>(move));
		}

		bool move(const Move& move) {
			if (isLegalMove(move)) {
				moveUnsafe(move);
				return true;
			}
			return false;
		}

		void moveUnsafe(const Move& move) {
			if (black) {
				moveUnsafe<true>(move);
			} else {
				moveUnsafe<false>(move);
			}
		}

		std::string toString() const;

		std::string toStringEffect(bool king = false) const {
			return effectBoard.toString(king);
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
