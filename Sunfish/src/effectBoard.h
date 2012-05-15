/*
 * effectBoard.h
 *
 *  Created on: 2012/05/15
 *      Author: ryosuke
 */

#ifndef EFFECTBOARD_H_
#define EFFECTBOARD_H_

#include "board.h"
#include "square.h"
#include "directionBit.h"

namespace Shogi {
	class EffectBoard {
	private:
		DirectionBit effectBoard[Square::SIZE];

	public:
		EffectBoard() {
			init();
		}

		EffectBoard(const Board& board) {
			init(board);
		}

		void init();

		void init(const Board& board);

		void set(const Square& sq, const DirectionBit& effect) {
			effectBoard[sq.getIndex()].set(effect);
		}

		void add(const Square& sq, const DirectionBit& effect) {
			effectBoard[sq.getIndex()].add(effect);
		}

		void remove(const Square& sq, const DirectionBit& effect) {
			effectBoard[sq.getIndex()].remove(effect);
		}

		const DirectionBit& get(const Square& sq) const {
			return effectBoard[sq.getIndex()];
		}
	};
}

#endif /* EFFECTBOARD_H_ */
