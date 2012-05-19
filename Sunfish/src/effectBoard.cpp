/*
 * effectBoard.cpp
 *
 *  Created on: 2012/05/15
 *      Author: ryosuke
 */

#include <sstream>
#include "effectBoard.h"

namespace Shogi {
	void EffectBoard::init() {
		for (Square sq(Square::TOP_W); sq.valid(); sq.inc()) {
			set<true>(sq, DirectionFlags::WALL);
			set<false>(sq, DirectionFlags::WALL);
		}
		for (Square sq(Square::TOP); sq.valid(); sq.next()) {
			set<true>(sq, DirectionFlags::NON);
			set<false>(sq, DirectionFlags::NON);
		}
	}

	void EffectBoard::init(const Board& board) {
		init();
		for (Square sq(Square::TOP); sq.valid(); sq.next()) {
			if (board.get(sq).isBlack()) {
				createEffect<true>(board, sq);
			} else if (board.get(sq).isWhite()) {
				createEffect<false>(board, sq);
			}
		}
	}

	template <bool black>
	void EffectBoard::createEffect(const Board& board, const Square& sq) {
		const Piece& piece = board.get(sq);
		if (!piece.isEmpty()) {
			DirectionFlags dirFlags = piece.getMoveableDirection();
			while (dirFlags.isNonZero()) {
				DirectionAndRange dir = dirFlags.pop().toDirection();
				for (Square to = sq + dir; !board.get(to).isWall(); to += dir) {
					effectBoard<black>()[to.getIndex()].add(dir);
					if (!board.get(to).isEmpty() || dir.isShortRange()) {
						break;
					}
				}
			}
		}
	}

	std::string EffectBoard::toString() const {
		std::ostringstream oss;
		Square sq(Square::TOP);
		for (sq.leftmost(); !get<true>(sq).isWall(); sq.leftmost(), sq.down()) {
			for (; !get<true>(sq).isWall(); sq.right()) {
				bool black = get<true>(sq).isNonZero();
				bool white = get<false>(sq).isNonZero();
				if (black && white) {
					oss << " X ";
				} else if (black) {
					oss << " b ";
				} else if (white) {
					oss << " w ";
				} else {
					oss << " * ";
				}
			}
			oss << '\n';
		}
		return oss.str();
	}
}

