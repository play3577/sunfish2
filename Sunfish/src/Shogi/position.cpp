/*
 * position.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <cassert>
#include <sstream>
#include "../Tools/debug.h"
#include "../Csa/csa.h"
#include "position.h"
#include "squareDiff.h"

namespace Shogi {
	PositionHash* Position::pPositionHash = NULL;

	bool Position::inputMoveCsa(const char* str, Move& move) const {
		if (strlen(str) < 7) {
			return false;
		}
		bool black;
		if (str[0] == Csa::CHAR_BLK) {
			black = true;
		} else if (str[0] == Csa::CHAR_WHT) {
			black = false;
		} else {
			return false;
		}
		Square from = Square::parse(&str[1]);
		Square to = Square::parse(&str[3]);
		if (!from.valid() || !to.valid()) {
			return false;
		}
		bool hand = (from == Square::NON);
		Piece piece = Piece::parseCsa(&str[5]);
		if (piece == Piece::EMPTY) {
			return false;
		}
		black ? piece.turnBlack() : piece.turnWhite();
		Piece pieceB = getBoard(Square(from));
		if (piece == pieceB) {
			move.setPromotion(false);
		} else if (piece == pieceB.getPromoted()) {
			move.setPromotion(true);
		} else {
			return false;
		}
		move.setFrom(from);
		move.setTo(to);
		move.setHand(hand);
		move.setPiece(pieceB);
		return true;
	}

	bool Position::inputMove(const char* str, Move& move) const {
		if (strlen(str) < 4) {
			return false;
		}
		Square to = Square::parse(&str[2]);
		if (!to.valid()) {
			return false;
		}
		Square from = Square::parse(&str[0]);
		if (from.valid()) {
			Piece piece = getBoard(Square(from));
			move.setPromotion(str[4] == 'n');
			move.setFrom(from);
			move.setTo(to);
			move.setHand(false);
			move.setPiece(piece);
			return true;
		} else {
			Piece piece = Piece::parseCsa(&str[0]);
			if (piece >= Piece::PAWN && piece <= Piece::ROOK) {
				move.setPromotion(false);
				move.setFrom(Square::NON);
				move.setTo(to);
				move.setHand(true);
				move.setPiece(blackTurn ? piece : piece.getTurnedWhite());
				return true;
			}
		}
		return false;
	}

	void Position::copy(const Position& position) {
		board.init(position.board);
		blackHand.init(position.blackHand);
		whiteHand.init(position.whiteHand);
		effectBoard.init(position.effectBoard);
		bking = position.bking;
		wking = position.wking;
		bpawns = position.bpawns;
		wpawns = position.wpawns;
		blackTurn = position.blackTurn;
		hash = position.hash;
	}

	std::string Position::toString() const {
		std::ostringstream oss;
		oss << "White:" << whiteHand.toString();
		oss << board.toString();
		oss << "Black:" << blackHand.toString();
		return oss.str();
	}

	std::string Position::toStringCsa() const {
		std::ostringstream oss;
		oss << board.toStringCsa();
		oss << Csa::CHAR_POS << Csa::CHAR_BLK << blackHand.toStringCsa();
		oss << Csa::CHAR_POS << Csa::CHAR_WHT << whiteHand.toStringCsa();
		oss << (blackTurn ? Csa::CHAR_BLK : Csa::CHAR_WHT) << '\n';
		return oss.str();
	}

	Util::uint64 Position::generateHash() const {
		Util::uint64 _hash = U64(0);
		for (Square square = Square::TOP; square.valid(); square.next()) {
			Piece piece = board.get(square);
			if (!piece.isEmpty()) {
				_hash ^= hashBoard(piece, square);
			}
		}
		for (Piece piece = Piece::PAWN; piece != Piece::KING; piece.toNext()) {
			for (int h = 0; h < blackHand.get(piece); h++) {
				_hash ^= hashHand(piece, h, true);
			}
			for (int h = 0; h < whiteHand.get(piece); h++) {
				_hash ^= hashHand(piece, h, false);
			}
		}
		_hash ^= blackTurn ? hashBlack() : U64(0);
		return _hash;
	}

	template <bool black, bool cuick>
	bool Position::isLegalMove(const Move& move) const {
		if (move.isEmpty()) {
			return false;
		}
		Piece piece = move.getPiece();
		if (!cuick && black && !piece.isBlack()) {
			return false;
		}
		if (!cuick && !black && !piece.isWhite()) {
			return false;
		}
		if (move.isHand()) { // 持ち駒を打つ場合
			if (!cuick && (piece.getTurnedBlack() < Piece::BPAWN ||
					piece.getTurnedBlack() > Piece::BROOK)){
				return false; // 不正な駒番号
			}
			if (black && blackHand.get(piece) == 0) {
				return false; // 持ち駒が無い。
			}
			if (!black && whiteHand.get(piece) == 0) {
				return false; // 持ち駒が無い。
			}
			int file = move.getTo().getFile();
			if (black && piece == Piece::BPAWN && bpawns.exist(file)) {
				return false; // 二歩
			}
			if (!black && piece == Piece::WPAWN && wpawns.exist(file)) {
				return false; // 二歩
			}
			if (!cuick && move.getTo().isCompulsoryPromotion(piece)) {
				return false; // 行きどころのない駒
			}
			Piece pieceTo = board.get(move.getTo());
			if (!pieceTo.isEmpty()) {
				return false; // 移動先に駒がある。
			}
			if (isCheck()) { // 王手がかかっている場合
				DirectionFlags flags = getCheckDirection();
				if (flags.isPlural()) {
					return false; // 両王手
				}
				SquareDiff diff(move.getTo(), black? bking : wking);
				if (flags.toDirection() != diff.toDirection()){
					return false; // 王手している方向ではない。
				}
				if (!effectBoard.get(move.getTo(), black).king()) {
					return false; // 王手している駒より遠い。
				}
			}
			if (black && piece == Piece::BPAWN && isPawnDropMate(move.getTo(), true)) {
				return false;
			} else if (!black && piece == Piece::WPAWN && isPawnDropMate(move.getTo(), false)) {
				return false;
			}
			return true;
		} else { // 盤上の駒を動かす場合
			if (piece != getBoard(move.getFrom())) {
				return false; // 不正な駒
			}
			if (!cuick && move.isPromotion() && (!piece.isPromotable() || !move.getTo().isPromotableRank(black))) {
				return false; // 成れない駒
			}
			if (!cuick && !move.isPromotion() && move.getTo().isCompulsoryPromotion(piece)) {
				return false; // 行きどころのない駒
			}
			SquareDiff diff = SquareDiff(move.getFrom(), move.getTo());
			DirectionAndRange dir = diff.toDirectionAndRange();
			if (dir == Direction::NON) {
				return false; // 移動不能
			}
			if ((black && move.getPiece() == Piece::BKING) || // 先手玉
					(!black && move.getPiece() == Piece::WKING)) { // 後手玉
				DirectionFlags effectTo = effectBoard.get(move.getTo(), !black);
				DirectionFlags effectFrom = effectBoard.get(move.getFrom(), !black);
				if (effectFrom.getLongRangeOnly().check(dir, true) // 利きの方向に移動しようとしている。
						|| effectTo.longOrShortRange()) { // 移動先に利きがある。
					return false;
				}
			} else { // 玉以外
				if (isCheck()) {
					DirectionFlags flags = getCheckDirection();
					if (flags.isPlural()) {
						return false; // 両王手
					}
					SquareDiff diff(move.getTo(), black? bking : wking);
					if (flags.toDirection() != diff.toDirection()){
						return false; // 王手している方向ではない。
					}
					if (flags.shortRange()) {
						if (move.getTo() != (black ? bking : wking) - flags.toDirection()) {
							return false;
						}
					} else {
						if (!effectBoard.get(move.getTo(), black).king()) {
							return false; // 王手している駒より遠い。
						}
					}
				}
				Direction p = pin(move.getFrom(), black).toDirection();
				if (p != Direction::NON && dir != p && dir != p.reverse()) {
					return false;
				}
			}
			if (dir.isShortRange()) { // 1マス
				// 移動可能な方向か
				if (piece.getMoveableDirection().check(dir) || piece.getMoveableDirection().check(dir, true)) {
					Piece pieceTo = board.get(move.getTo());
					// 移動先に駒がいないか相手の駒か
					return pieceTo.isEmpty() || (black ? pieceTo.isWhite() : pieceTo.isBlack());
				}
				return false;
			} else { // 跳び駒の移動
				// 移動可能な方向か
				if (piece.getMoveableDirection().check(dir)) {
					// 間に駒がないか
					for (Square sq = move.getFrom() + dir; ; sq += dir) {
						Piece pieceTo = board.get(sq);
						if (!pieceTo.isEmpty()) {
							return sq == move.getTo() && (black ? pieceTo.isWhite() : pieceTo.isBlack());
						}
						if (sq == move.getTo()) {
							return true;
						}
					}
				}
				return false;
			}
		}
	}
	template bool Position::isLegalMove<true, true>(const Move& move) const;
	template bool Position::isLegalMove<false, true>(const Move& move) const;
	template bool Position::isLegalMove<true, false>(const Move& move) const;
	template bool Position::isLegalMove<false, false>(const Move& move) const;

	template <bool black, bool chNotNull, bool evNotNull>
	void Position::moveUnsafe(const Move& move, Change* change,
			Evaluate::Evaluate* eval) {
		if (chNotNull) {
			change->setHash(hash); // hash
			change->setBlackKing(bking); // black king's square
			change->setWhiteKing(wking); // white king's square
			change->setBlackPawns(bpawns); // black pawns
			change->setWhitePawns(wpawns); // white pawns
		}
		if (move.isHand()) { // 持ち駒を打つ場合
			if (chNotNull) { change->setType(Change::DROP); } // type of change
			Piece piece = move.getPiece();
			if (black) { // 先手
				assert(piece.isBlack());
				if (chNotNull) { change->setHandNum(blackHand.get(piece)); } // number of pieces
				if (chNotNull) { change->setHandPiece(piece); } // dropping piece
				if (chNotNull) { change->setToSquare(move.getTo()); } // dropping square
				blackHand.dec(piece); // hand
				hash ^= hashHand(piece, blackHand.get(piece), true); // hash
				board.set(move.getTo(), piece); // board
				hash ^= hashBoard(piece, move.getTo()); // hash
				if (piece == Piece::BPAWN) {
					bpawns.set(move.getTo().getFile());
				}
				effectBoard.change<true, true>(move.getTo(), piece.getMoveableDirection(), board); // effect
			} else { // 後手
				assert(piece.isWhite());
				if (chNotNull) { change->setHandNum(whiteHand.get(piece)); } // number of pieces
				if (chNotNull) { change->setHandPiece(piece); } // dropping piece
				if (chNotNull) { change->setToSquare(move.getTo()); } // dropping square
				whiteHand.dec(piece); // hand
				hash ^= hashHand(piece, whiteHand.get(piece), false); // hash
				board.set(move.getTo(), piece); // board
				hash ^= hashBoard(piece, move.getTo()); // hash
				if (piece == Piece::WPAWN) {
					wpawns.set(move.getTo().getFile());
				}
				effectBoard.change<false, true>(move.getTo(), piece.getMoveableDirection(), board); // effect
			}
		} else { // 盤上の駒
			Piece piece = board.set(move.getFrom(), Piece::EMPTY); // board
			hash ^= hashBoard(piece, move.getFrom()); // hash
			if (chNotNull) { change->setFromSquare(move.getFrom()); } // move from
			if (chNotNull) { change->setFromPiece(piece); } // moved piece
			if (black) { // 先手
				effectBoard.change<true, false>(move.getFrom(), piece.getMoveableDirection(), board); // effect
			} else { // 後手
				effectBoard.change<false, false>(move.getFrom(), piece.getMoveableDirection(), board); // effect
			}
			if (move.isPromotion()) { // 成
				if (black && piece == Piece::BPAWN) { // 先手の歩
					bpawns.unset(move.getFrom().getFile());
				} else if (!black && piece == Piece::WPAWN) { // 後手の歩
					wpawns.unset(move.getFrom().getFile());
				}
				if (evNotNull) { eval->addBaseValuePr(piece); } // evaluate
				piece.promote();
			} else if (black && piece.isKing<true>()) { // 先手玉
				bking = move.getTo();
			} else if (!black && piece.isKing<false>()) { // 後手玉
				wking = move.getTo();
			}
			Piece capture = board.set(move.getTo(), piece); // board
			hash ^= hashBoard(piece, move.getTo()); // hash
			if (chNotNull) { change->setToSquare(move.getTo()); } // move to
			if (chNotNull) { change->setToPiece(capture); } // captured piece
			assert(capture != Piece::BKING);
			assert(capture != Piece::WKING);
			if (!capture.isEmpty()) {
				assert(capture != Piece::BKING);
				assert(capture != Piece::WKING);
				hash ^= hashBoard(capture, move.getTo()); // hash
				if (chNotNull) { change->setType(Change::CAPTURE); } // type of change
				if (evNotNull) { eval->subBaseValueEx(capture); } // evaluate
				Piece captureUP = capture.getUnPromoted();
				if (black) {
					if (chNotNull) { change->setHandNum(blackHand.get(captureUP)); } // number of pieces
					if (chNotNull) { change->setHandPiece(captureUP); } // captured piece
					hash ^= hashHand(captureUP, blackHand.get(captureUP), true); // hash
					blackHand.inc(captureUP); // hand
					effectBoard.change<false, false>(move.getTo(), capture.getMoveableDirection(), board); // effect
					if (capture == Piece::WPAWN) { // 後手の歩
						wpawns.unset(move.getTo().getFile());
					}
				} else {
					if (chNotNull) { change->setHandNum(whiteHand.get(captureUP)); } // number of pieces
					if (chNotNull) { change->setHandPiece(captureUP); } // captured piece
					hash ^= hashHand(captureUP, whiteHand.get(captureUP), false); // hash
					whiteHand.inc(capture.getUnPromoted()); // hand
					effectBoard.change<true, false>(move.getTo(), capture.getMoveableDirection(), board); // effect
					if (capture == Piece::BPAWN) { // 先手の歩
						bpawns.unset(move.getTo().getFile());
					}
				}
			} else {
				if (chNotNull) { change->setType(Change::NO_CAPTURE); } // type of change
			}
			if (black) {
				effectBoard.change<true, true>(move.getTo(), piece.getMoveableDirection(), board);
			} else {
				effectBoard.change<false, true>(move.getTo(), piece.getMoveableDirection(), board);
			}
		}
		turn();
		hash ^= hashBlack();
	}
	template void Position::moveUnsafe<true, true, true>
			(const Move& move, Change* change,
			Evaluate::Evaluate* eval);
	template void Position::moveUnsafe<true, false, true>
			(const Move& move, Change* change,
			Evaluate::Evaluate* eval);
	template void Position::moveUnsafe<false, true, true>
			(const Move& move, Change* change,
			Evaluate::Evaluate* eval);
	template void Position::moveUnsafe<false, false, true>
			(const Move& move, Change* change,
			Evaluate::Evaluate* eval);
	template void Position::moveUnsafe<true, true, false>
			(const Move& move, Change* change,
			Evaluate::Evaluate* eval);
	template void Position::moveUnsafe<true, false, false>
			(const Move& move, Change* change,
			Evaluate::Evaluate* eval);
	template void Position::moveUnsafe<false, true, false>
			(const Move& move, Change* change,
			Evaluate::Evaluate* eval);
	template void Position::moveUnsafe<false, false, false>
			(const Move& move, Change* change,
			Evaluate::Evaluate* eval);

	template <bool chNotNull>
	bool Position::nullMove(Change* change) {
		if (isCheck()) {
			return false;
		}
		if (chNotNull) {
			change->setType(Change::NULL_MOVE); // type of change
		}
		turn();
		hash ^= hashBlack();
		return true;
	}
	template bool Position::nullMove<true>(Change* change);
	template bool Position::nullMove<false>(Change* change);

	template <bool black>
	void Position::back(const Change& change) {
		Piece handPiece;
		Piece fromPiece;
		Piece toPiece;
		Piece toAfterPiece;
		Square to;
		Square from;
		switch (change.getType()) {
		case Change::NULL_MOVE:
			turn();
			hash ^= hashBlack();
			return;
		case Change::DROP:
			handPiece = change.getHandPiece();
			to = change.getToSquare();
			if (black) {
				blackHand.set(handPiece, change.getHandNum());
			} else {
				whiteHand.set(handPiece, change.getHandNum());
			}
			board.set(to, Piece::EMPTY);
			if (black) {
				effectBoard.change<true, false>(to, handPiece.getMoveableDirection(), board); // effect
			} else {
				effectBoard.change<false, false>(to, handPiece.getMoveableDirection(), board); // effect
			}
			break;
		case Change::CAPTURE:
			from = change.getFromSquare();
			to = change.getToSquare();
			fromPiece = change.getFromPiece();
			toPiece = change.getToPiece();
			board.set(from, fromPiece);
			if (black) { // 先手
				effectBoard.change<true, true>(from, fromPiece.getMoveableDirection(), board); // effect
			} else { // 後手
				effectBoard.change<false, true>(from, fromPiece.getMoveableDirection(), board); // effect
			}
			toAfterPiece = board.set(to, toPiece);
			if (black) {
				effectBoard.change<true, false>(to, toAfterPiece.getMoveableDirection(), board); // effect
			} else {
				effectBoard.change<false, false>(to, toAfterPiece.getMoveableDirection(), board); // effect
			}
			handPiece = change.getHandPiece();
			if (black) {
				blackHand.set(handPiece, change.getHandNum());
				effectBoard.change<false, true>(to, toPiece.getMoveableDirection(), board); // effect
			} else {
				whiteHand.set(handPiece, change.getHandNum());
				effectBoard.change<true, true>(to, toPiece.getMoveableDirection(), board); // effect
			}
			break;
		case Change::NO_CAPTURE:
			from = change.getFromSquare();
			to = change.getToSquare();
			fromPiece = change.getFromPiece();
			toPiece = change.getToPiece();
			board.set(from, fromPiece);
			if (black) { // 先手
				effectBoard.change<true, true>(from, fromPiece.getMoveableDirection(), board); // effect
			} else { // 後手
				effectBoard.change<false, true>(from, fromPiece.getMoveableDirection(), board); // effect
			}
			toAfterPiece = board.set(to, toPiece);
			if (black) {
				effectBoard.change<true, false>(to, toAfterPiece.getMoveableDirection(), board); // effect
			} else {
				effectBoard.change<false, false>(to, toAfterPiece.getMoveableDirection(), board); // effect
			}
			break;
		}
		turn();
		hash = change.getHash();
		bking = change.getBlackKing();
		wking = change.getWhiteKing();
		bpawns = change.getBlackPawns();
		wpawns = change.getWhitePawns();
	}
	template void Position::back<true>(const Change& change);
	template void Position::back<false>(const Change& change);

	template <bool black, unsigned excludingFlag>
	bool Position::isKingMoveable(Direction dir) const {
		Square square = (black ? bking : wking) + dir;
		Piece piece = board.get(square);
		if (!piece.isEmpty()) {
			if (black && piece.isBlack()) {
				return false;
			} else if (!black && piece.isWhite()) {
				return false;
			}
		}
		DirectionFlags flags = effectBoard.get(square, !black);
		flags.remove(DirectionFlags(excludingFlag));
		return !flags.longOrShortRange();
	}

	template<bool black>
	bool Position::canPawnDropCheck() const {
		if (black) {
			return isKingMoveable<false, DirectionFlags::NON           >(Direction::LEFT_UP)
				|| isKingMoveable<false, DirectionFlags::NON           >(Direction::UP)
				|| isKingMoveable<false, DirectionFlags::NON           >(Direction::RIGHT_UP)
				|| isKingMoveable<false, DirectionFlags::LONG_LEFT_UP  >(Direction::LEFT)
				|| isKingMoveable<false, DirectionFlags::LONG_RIGHT_UP >(Direction::RIGHT)
				|| isKingMoveable<false, DirectionFlags::LONG_LEFT     >(Direction::LEFT_DOWN)
				|| isKingMoveable<false, DirectionFlags::NON           >(Direction::DOWN)
				|| isKingMoveable<false, DirectionFlags::LONG_RIGHT    >(Direction::RIGHT_DOWN);
		} else {
			return isKingMoveable<true, DirectionFlags::LONG_LEFT      >(Direction::LEFT_UP)
				|| isKingMoveable<true, DirectionFlags::NON            >(Direction::UP)
				|| isKingMoveable<true, DirectionFlags::LONG_RIGHT     >(Direction::RIGHT_UP)
				|| isKingMoveable<true, DirectionFlags::LONG_LEFT_DOWN >(Direction::LEFT)
				|| isKingMoveable<true, DirectionFlags::LONG_RIGHT_DOWN>(Direction::RIGHT)
				|| isKingMoveable<true, DirectionFlags::NON            >(Direction::LEFT_DOWN)
				|| isKingMoveable<true, DirectionFlags::NON            >(Direction::DOWN)
				|| isKingMoveable<true, DirectionFlags::NON            >(Direction::RIGHT_DOWN);
		}
	}

	template bool Position::canPawnDropCheck<true>() const;
	template bool Position::canPawnDropCheck<false>() const;
}

