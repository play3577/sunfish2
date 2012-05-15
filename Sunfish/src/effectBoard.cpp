/*
 * effectBoard.cpp
 *
 *  Created on: 2012/05/15
 *      Author: ryosuke
 */

#include "effectBoard.h"

namespace Shogi {
	void EffectBoard::init() {
		for (Square sq(Square::TOP_W); sq.inside(); sq.inc()) {
			set(sq, DirectionBit::NON);
		}
	}

	void EffectBoard::init(const Board& board) {
		init();
		for (Square sq(Square::TOP); sq.inside(); sq.next()) {
			const Piece& piece = board.get(sq);
			if (!piece.isEmpty()) {

			}
		}
	}
}

