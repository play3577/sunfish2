/*
 * position.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <cassert>
#include <sstream>
#include "../Debug/debug.h"
#include "position.h"
#include "squareDiff.h"

namespace Shogi {
	std::string Position::toString() const {
		std::ostringstream oss;
		oss << "White:" << whiteHand.toString();
		oss << board.toString();
		oss << "Black:" << blackHand.toString();
		return oss.str();
	}

	template <bool black>
	bool Position::isLegalMove(const Move& move) const {
		// TODO: Debug.txtの局面を検証
		Piece piece = move.getPiece();
		if (move.isHand()) { // 持ち駒を打つ場合
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
			if (move.getTo().isCompulsoryPromotion(piece)) {
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
			// TODO: uchi fu zume!!
			return true;
		} else {
			if (move.isPromotion() && (!piece.isPromotable() || !move.getTo().isPromotableRank(black))) {
				return false; // 成れない駒
			}
			if (!move.isPromotion() && move.getTo().isCompulsoryPromotion(piece)) {
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
				if (effectFrom.getLongRangeOnly().check(dir) // 利きの方向に移動しようとしている。
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
	template bool Position::isLegalMove<true>(const Move& move) const;
	template bool Position::isLegalMove<false>(const Move& move) const;

	template <bool black>
	void Position::moveUnsafe(const Move& move) {
		if (move.isHand()) {
			Piece piece = move.getPiece();
			if (black) {
				assert(piece.isBlack());
				blackHand.dec(piece);
				board.set(move.getTo(), piece);
				if (piece == Piece::BPAWN) {
					bpawns.set(move.getTo().getFile());
				}
				std::cout << piece.toString() << std::endl;
				effectBoard.change<true, true>(move.getTo(), piece.getMoveableDirection(), board);
			} else {
				assert(piece.isWhite());
				whiteHand.dec(piece);
				board.set(move.getTo(), piece);
				if (piece == Piece::WPAWN) {
					wpawns.set(move.getTo().getFile());
				}
				std::cout << piece.toString() << std::endl;
				effectBoard.change<false, true>(move.getTo(), piece.getMoveableDirection(), board);
			}
		} else {
			Piece piece = board.set(move.getFrom(), Piece::EMPTY);
			if (black) {
				effectBoard.change<true, false>(move.getFrom(), piece.getMoveableDirection(), board);
			} else {
				effectBoard.change<false, false>(move.getFrom(), piece.getMoveableDirection(), board);
			}
			if (move.isPromotion()) {
				if (black && piece == Piece::BPAWN) {
					bpawns.unset(move.getFrom().getFile());
				} else if (!black && piece == Piece::WPAWN) {
					wpawns.unset(move.getFrom().getFile());
				}
				piece.promote();
			} else if (black && piece.isKing<true>()) {
				bking = move.getTo();
			} else if (!black && piece.isKing<false>()) {
				wking = move.getTo();
			}
			Piece capture = board.set(move.getTo(), piece);
			assert(capture != Piece::BKING);
			assert(capture != Piece::WKING);
			if (!capture.isEmpty()) {
				if (black) {
					blackHand.inc(capture.getUnPromoted());
					effectBoard.change<false, false>(move.getTo(), capture.getMoveableDirection(), board);
					if (capture == Piece::WPAWN) {
						wpawns.unset(move.getTo().getFile());
					}
				} else {
					whiteHand.inc(capture.getUnPromoted());
					effectBoard.change<true, false>(move.getTo(), capture.getMoveableDirection(), board);
					if (capture == Piece::BPAWN) {
						bpawns.unset(move.getTo().getFile());
					}
				}
			}
			if (black) {
				effectBoard.change<true, true>(move.getTo(), piece.getMoveableDirection(), board);
			} else {
				effectBoard.change<false, true>(move.getTo(), piece.getMoveableDirection(), board);
			}
		}
		turn();
	}

	template void Position::moveUnsafe<true>(const Move& move);
	template void Position::moveUnsafe<false>(const Move& move);
}

