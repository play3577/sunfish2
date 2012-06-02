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
		Piece piece = move.getPiece();
		if (move.isHand()) {
			if (black && blackHand.get(piece) == 0) {
				return false;
			}
			if (!black && whiteHand.get(piece) == 0) {
				return false;
			}
			int file = move.getTo().getFile();
			if (black && piece == Piece::BPAWN && bpawns.exist(file)) {
				return false;
			}
			if (!black && piece == Piece::WPAWN && wpawns.exist(file)) {
				return false;
			}
			if (move.getTo().isCompulsoryPromotion(piece)) {
				return false;
			}
			Piece pieceTo = board.get(move.getTo());
			if (!pieceTo.isEmpty()) {
				return false;
			}
			// TODO: uchi fu zume!!
			return true;
		} else {
			if (move.isPromotion() && (!piece.isPromotable() || !move.getTo().isPromotableRank(black))) {
				return false;
			}
			if (!move.isPromotion() && move.getTo().isCompulsoryPromotion(piece)) {
				return false;
			}
			SquareDiff diff = SquareDiff(move.getFrom(), move.getTo());
			DirectionAndRange dir = diff.toDirectionAndRange();
			if (DirectionFlags(dir, true).king()) {
				std::cout << dir << std::endl;
				DEBUG_PRINT_LINE;
			}
			if (dir == Direction::NON) {
				return false;
			} else if (dir.isShortRange()) {
				if (piece.getMoveableDirection().check(dir) || piece.getMoveableDirection().check(dir, true)) {
					Piece pieceTo = board.get(move.getTo());
					return pieceTo.isEmpty() || (black ? pieceTo.isWhite() : pieceTo.isBlack());
				}
				return false;
			} else {
				if (piece.getMoveableDirection().check(dir)) {
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

