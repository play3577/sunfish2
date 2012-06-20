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
#include "positionHash.h"
#include "change.h"

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
		bool blackTurn;
		static PositionHash* pPositionHash;
		Util::uint64 hash;

		static Util::int64 hashBoard(const Piece& piece, const Square& square) {
			return pPositionHash->getBoard(piece, square);
		}

		static Util::int64 hashHand(const Piece& piece, int h, bool black) {
			if (black) {
				return pPositionHash->getBlackHand(piece, h);
			} else {
				return pPositionHash->getWhiteHand(piece, h);
			}
		}

		static Util::int64 hashBlack() {
			return pPositionHash->getBlack();
		}

		template <bool black>
		bool isLegalMove(const Move& move) const;

		template <bool black, bool chNotNull>
		void moveUnsafe(const Move& move, Change* change);

		template <bool chNotNull>
		void moveUnsafe(const Move& move, Change* change) {
			if (blackTurn) {
				moveUnsafe<true, chNotNull>(move, change);
			} else {
				moveUnsafe<false, chNotNull>(move, change);
			}
		}

		template <bool black>
		void back(const Change& change);

		template <bool chNotNull>
		bool move(const Move& move, Change* change) {
			if (isLegalMove(move)) {
				moveUnsafe<chNotNull>(move, change);
				return true;
			}
			return false;
		}

		template <bool black, unsigned excludingFlag>
		bool isKingMoveable(Direction dir) const;

		template<bool black>
		bool canPawnDropCheck() const;

	public:
		Position(bool blackTurn = true) : blackTurn(blackTurn) {
			update();
		}

		Position(Handicap handicap) : board(handicap) {
			blackTurn = (handicap == EVEN);
			update();
		}

		Position(Handicap handicap, bool blackTurn) : board(handicap), blackTurn(blackTurn) {
			update();
		}

		Position(const Position& position) : board(position.board), blackHand(position.blackHand),
				whiteHand(position.whiteHand), effectBoard(position.effectBoard),
				bking(position.bking), wking(position.wking),
				bpawns(position.bpawns), wpawns(position.wpawns),
				hash(position.hash) {
		}

		static void setPositionHash(PositionHash* pPositionHash) {
			Position::pPositionHash = pPositionHash;
		}

		Util::uint64 getHash() const {
			return hash;
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
			updateHash();
		}

		void updateHash() {
			hash = generateHash();
		}

		Util::uint64 generateHash() const;

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
			return blackTurn ? getBlackHand(piece) : getWhiteHand(piece);
		}

		const DirectionFlags& getEffect(const Square& square, bool blackTurn) const {
			return effectBoard.get(square, blackTurn);
		}

		const EffectBoard& getEffectBoard() const {
			return effectBoard;
		}

		bool isBlackTurn() const {
			return blackTurn;
		}

		bool isWhiteTurn() const {
			return !blackTurn;
		}

		bool setBlackTurn() {
			bool prev = blackTurn;
			blackTurn = true;
			return prev;
		}

		bool setWhiteTurn() {
			bool prev = blackTurn;
			blackTurn = false;
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
			if (blackTurn) {
				return bking.valid() && (bool)effectBoard.get(bking, false).getExcludeKing();
			} else {
				return wking.valid() && (bool)effectBoard.get(wking, true).getExcludeKing();
			}
		}

		bool isPawnDropMate(const Square& sq, bool blackTurn) const {
			if (blackTurn) {
				return sq == wking + Direction::DOWN && !canPawnDropCheck<true>();
			} else {
				return sq == bking + Direction::UP && !canPawnDropCheck<false>();
			}
		}

		DirectionFlags getCheckDirection() const {
			if (blackTurn) {
				return effectBoard.get(bking, false).getExcludeKing();
			} else {
				return effectBoard.get(wking, true).getExcludeKing();
			}
		}

		DirectionFlags pin(const Square& sq, bool blackTurn) const {
			if (blackTurn) {
				return effectBoard.get(sq, true).pin(effectBoard.get(sq, false));
			} else {
				return effectBoard.get(sq, false).pin(effectBoard.get(sq, true));
			}
		}

		void turn() {
			blackTurn = !blackTurn;
		}

		bool isLegalMove(const Move& move) const {
			return (blackTurn ? isLegalMove<true>(move) : isLegalMove<false>(move));
		}

		bool move(const Move& move) {
			return this->move<false>(move, NULL);
		}

		bool move(const Move& move, Change& change) {
			return this->move<true>(move, &change);
		}

		void moveUnsafe(const Move& move) {
			moveUnsafe<false>(move, NULL);
		}

		void moveUnsafe(const Move& move, Change& change) {
			moveUnsafe<true>(move, &change);
		}

		std::string toString() const;

		std::string toStringCsa() const;

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
