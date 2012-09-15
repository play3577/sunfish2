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

namespace Evaluates {
	class Evaluate;
}

namespace Shogi {
	class Position {
	private:
		static PositionHash* pPositionHash;
		Util::uint64 hash;
		Board board;
		Hand blackHand;
		Hand whiteHand;
		EffectBoard effectBoard;
		Square bking;
		Square wking;
		PawnFlags bpawns;
		PawnFlags wpawns;
		bool blackTurn;

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

		template <bool black, bool cuick>
		bool isLegalMove(const Move& move) const;

		template <bool black, bool chNotNull, bool evNotNull>
		void moveUnsafe(const Move& move, Change* change,
				Evaluates::Evaluate* eval);

		template <bool chNotNull, bool evNotNull>
		void moveUnsafe(const Move& move, Change* change,
				Evaluates::Evaluate* eval) {
			if (blackTurn) {
				moveUnsafe<true, chNotNull, evNotNull>
					(move, change, eval);
			} else {
				moveUnsafe<false, chNotNull, evNotNull>
					(move, change, eval);
			}
		}

		template <bool chNotNull>
		bool nullMove(Change* change);

		template <bool black>
		void back(const Change& change);

		template <bool chNotNull, bool evNotNull>
		bool move(const Move& move, Change* change,
				Evaluates::Evaluate* eval) {
			if (isLegalMove(move)) {
				moveUnsafe<chNotNull, evNotNull>(move, change, eval);
				return true;
			}
			return false;
		}

		template <bool black, unsigned excludingFlag>
		bool isKingMoveable(Direction dir) const;

		// 玉の移動以外で王手回避可能か
		template<bool black>
		bool isEvadable(const Square& square, const Direction& dir) const;

		template<bool black>
		bool canPawnDropCheck() const;

		// 動かした駒による王手
		bool isCheckMoveDirect(const Move& move) const {
			DirectionFlags king = effectBoard.get(move.getTo(), !blackTurn).getKingOnly();
			if (king.isZero()) { return Direction(Direction::NON); }
			Piece piece = move.getPiece();
			if (move.isPromotion()) { piece.promote(); }
			if (king.isCheck(piece.getMoveableDirection())) {
				return true;// 長い利き
			}
			Square ksq = blackTurn ? bking : wking;
			Direction kingDir = king.toDirection();
			if ((move.getFrom() == ksq + kingDir)) {
				return true;// 短い利き
			}
			return false;
		}

		// 開き王手
		bool isCheckMoveDiscovered(const Move& move) const {
			DirectionFlags king = effectBoard.get(move.getFrom(), !blackTurn);
			DirectionFlags attacker = effectBoard.get(move.getFrom(), blackTurn);
			if (king.isCheck(attacker)) {
				return attacker.toDirection();
			}
			return Direction(Direction::NON);
		}

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

		Position(const Position& position) : hash(position.hash),
				board(position.board),
				blackHand(position.blackHand), whiteHand(position.whiteHand),
				effectBoard(position.effectBoard),
				bking(position.bking), wking(position.wking),
				bpawns(position.bpawns), wpawns(position.wpawns),
				blackTurn(position.blackTurn) {
		}

		void copy(const Position& position);

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

		bool isCheckMove(const Move& move) const {
			return isCheckMoveDirect(move) ||
					isCheckMoveDiscovered(move);
		}

		bool isCapturingMove(const Move& move) const {
			return !getBoard(move.getTo()).isEmpty();
		}

		bool isPawnDropMate(const Square& sq, bool blackTurn) const {
			if (blackTurn) {
				return sq == wking + Direction::DOWN && !canPawnDropCheck<true>();
			} else {
				return sq == bking + Direction::UP && !canPawnDropCheck<false>();
			}
		}

		bool isMate() const;

		DirectionFlags getCheckDirection() const {
			if (blackTurn) {
				return effectBoard.get(bking, false).getExcludeKing();
			} else {
				return effectBoard.get(wking, true).getExcludeKing();
			}
		}

		DirectionFlags pin(const Square& sq, bool blackTurn) const {
			if (blackTurn) {
				return effectBoard.get(sq, true).isCheck(effectBoard.get(sq, false));
			} else {
				return effectBoard.get(sq, false).isCheck(effectBoard.get(sq, true));
			}
		}

		void turn() {
			blackTurn = !blackTurn;
		}

		bool isLegalMove(const Move& move, bool cuick = false) const {
			if (cuick) {
				return (blackTurn ? isLegalMove<true, true>(move) : isLegalMove<false, true>(move));
			} else {
				return (blackTurn ? isLegalMove<true, false>(move) : isLegalMove<false, false>(move));
			}
		}

		bool move(const Move& move) {
			return this->move<false, false>(move, NULL, NULL);
		}

		bool move(const Move& move, Change& change) {
			return this->move<true, false>(move, &change, NULL);
		}

		bool move(const Move& move, Change& change,
				Evaluates::Evaluate& eval) {
			return this->move<true, true>(move, &change, &eval);
		}

		void moveUnsafe(const Move& move) {
			moveUnsafe<false, false>(move, NULL, NULL);
		}

		void moveUnsafe(const Move& move, Change& change) {
			moveUnsafe<true, false>(move, &change, NULL);
		}

		void moveUnsafe(const Move& move, Change& change,
				Evaluates::Evaluate& eval) {
			moveUnsafe<true, true>(move, &change, &eval);
		}

		bool nullMove() {
			return nullMove<false>(NULL);
		}

		bool nullMove(Change& change) {
			return nullMove<true>(&change);
		}

		void back(const Change& change) {
			if (blackTurn) {
				back<false>(change);
			} else {
				back<true>(change);
			}
		}

		bool inputMoveCsa(const char* str, Move& move) const;

		bool inputMove(const char* str, Move& move) const;

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
