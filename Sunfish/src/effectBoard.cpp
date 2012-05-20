/*
 * effectBoard.cpp
 *
 *  Created on: 2012/05/15
 *      Author: ryosuke
 */

#include <iostream>
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
				DirectionAndRange dir = dirFlags.pop().toDirectionAndRange();
				for (Square to = sq + dir; !board.get(to).isWall(); to += dir) {
					effectBoard<black>()[to.getIndex()].add(dir);
					if (!board.get(to).isEmpty() || dir.isShortRange()) {
						break;
					}
				}
			}
		}
	}

	template <bool black, bool addition>
	void EffectBoard::changeStraight(const Square& sq, const Direction dir, const DirectionFlags& dirFlags, const Board& board) {
		for (Square to = sq + dir; !effectBoardConst<black>()[to.getIndex()].isWall(); to += dir) {
			if (addition) {
				effectBoard<black>()[to.getIndex()].add(dirFlags);
			} else {
				effectBoard<black>()[to.getIndex()].remove(dirFlags);
			}
			if (!board.get(to).isEmpty()) {
				break;
			}
		}
	}

	template <bool black, bool addition>
	void EffectBoard::changeAround(const Square& sq, const Board& board) {
		DirectionFlags flags = effectBoardConst<black>()[sq.getIndex()].getLongRangeOnly();
		while (flags.isNonZero()) {
			DirectionFlags flag = flags.pop();
			DirectionAndRange dir = flag.toDirectionAndRange();
			changeStraight<black, addition>(sq, dir, flag, board);
		}
	}

	template <bool black, bool addition>
	void EffectBoard::change(const Square& sq, const DirectionFlags& dirFlags, const Board& board) {
		if (addition) {
			changeAround<true, false>(sq, board);
			changeAround<false, false>(sq, board);
		}
		DirectionFlags flags = dirFlags.getLongRangeOnly();
		while (flags.isNonZero()) {
			DirectionFlags flag = flags.pop();
			Direction dir = flag.toDirection();
			changeStraight<black, addition>(sq, dir, flag, board);
		}
		flags = dirFlags.getShortRangeOnly();
		while (flags.isNonZero()) {
			DirectionFlags flag = flags.pop();
			Direction dir = flag.toDirection();
			Square to = sq + dir;
			if (addition) {
				effectBoard<black>()[to.getIndex()].add(dirFlags);
			} else {
				effectBoard<black>()[to.getIndex()].remove(dirFlags);
			}
		}
		if (!addition) {
			changeAround<true, true>(sq, board);
			changeAround<false, true>(sq, board);
		}
	}
	template void EffectBoard::change<true, true>(const Square& sq, const DirectionFlags& dirFlags, const Board& board);
	template void EffectBoard::change<true, false>(const Square& sq, const DirectionFlags& dirFlags, const Board& board);
	template void EffectBoard::change<false, true>(const Square& sq, const DirectionFlags& dirFlags, const Board& board);
	template void EffectBoard::change<false, false>(const Square& sq, const DirectionFlags& dirFlags, const Board& board);

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

