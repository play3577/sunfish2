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

		void initNoUpdate() {
			board.init(NOPIECES);
			blackHand.init();
			whiteHand.init();
		}

		void init() {
			initNoUpdate();
			update();
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

		void setBoard(const Square& square, const Piece& piece) {
			board.set(square, piece);
		}

		int getBlackHand(const Piece& piece) const {
			return blackHand.get(piece);
		}

		int getWhiteHand(const Piece& piece) const {
			return whiteHand.get(piece);
		}

		void setBlackHand(const Piece& piece, int num) {
			blackHand.set(piece, num);
		}

		void setWhiteHand(const Piece& piece, int num) {
			whiteHand.set(piece, num);
		}

		void incBlackHand(const Piece& piece) {
			blackHand.inc(piece);
		}

		void incWhiteHand(const Piece& piece) {
			whiteHand.inc(piece);
		}

		void decBlackHand(const Piece& piece) {
			blackHand.dec(piece);
		}

		void decWhiteHand(const Piece& piece) {
			whiteHand.dec(piece);
		}

		int getHand(const Piece& piece) const {
			return black ? getBlackHand(piece) : getWhiteHand(piece);
		}

		const DirectionFlags& getEffect(const Square& square, bool black) const {
			return effectBoard.get(square, black);
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

		bool setBlackTurn() {
			bool prev = black;
			black = true;
			return prev;
		}

		bool setWhiteTurn() {
			bool prev = black;
			black = false;
			return !prev;
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
				return (bool)effectBoard.get(bking, false).getExcludeKing();
			} else {
				return (bool)effectBoard.get(wking, true).getExcludeKing();
			}
		}

		DirectionFlags getCheckDirection() const {
			if (black) {
				return effectBoard.get(bking, false).getExcludeKing();
			} else {
				return effectBoard.get(wking, true).getExcludeKing();
			}
		}

		DirectionFlags pin(const Square& sq, bool black) const {
			if (black) {
				return effectBoard.get(sq, true).pin(effectBoard.get(sq, false));
			} else {
				return effectBoard.get(sq, false).pin(effectBoard.get(sq, true));
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
