/*
 * position.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <sstream>
#include "position.h"

namespace Shogi {
	std::string Position::toString() const {
		std::ostringstream oss;
		oss << "Black:" << blackHand.toString();
		oss << board.toString();
		oss << "White:" << whiteHand.toString();
		return oss.str();
	}

	template <bool black>
	bool Position::move(const Move& move) {
		// TODO:
		return false;
	}

	template bool Position::move<true>(const Move& move);
	template bool Position::move<false>(const Move& move);

	template <bool black>
	void Position::moveUnsafe(const Move& move) {
		if (move.isHand()) {
			if (black) {
				blackHand.dec(move.getPiece());
				board.set(move.getTo(), move.getPiece().getTurnedBlack());
			} else {
				whiteHand.dec(move.getPiece());
				board.set(move.getTo(), move.getPiece().getTurnedWhite());
			}
		} else {
			Piece piece = board.set(move.getFrom(), Piece::EMPTY);
			if (black) {
				effectBoard.change<true, false>(move.getFrom(), piece.getMoveableDirection(), board);
			} else {
				effectBoard.change<false, false>(move.getFrom(), piece.getMoveableDirection(), board);
			}
			if (move.isPromotion()) {
				piece.promote();
			}
			Piece capture = board.set(move.getTo(), piece);
			if (black) {
				effectBoard.change<true, true>(move.getTo(), piece.getMoveableDirection(), board);
			} else {
				effectBoard.change<false, true>(move.getTo(), piece.getMoveableDirection(), board);
			}
			if (!capture.isEmpty()) {
				if (black) {
					blackHand.inc(capture.getUnPromoted().getPieceNumber());
					effectBoard.change<false, false>(move.getTo(), capture.getMoveableDirection(), board);
				} else {
					whiteHand.inc(capture.getUnPromoted().getPieceNumber());
					effectBoard.change<true, false>(move.getTo(), capture.getMoveableDirection(), board);
				}
			}
		}
		turn();
	}

	template void Position::moveUnsafe<true>(const Move& move);
	template void Position::moveUnsafe<false>(const Move& move);
}

