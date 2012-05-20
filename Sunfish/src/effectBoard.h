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
#include "directionFlags.h"

namespace Shogi {
	class EffectBoard {
	private:
		DirectionFlags blackEffectBoard[Square::SIZE];
		DirectionFlags whiteEffectBoard[Square::SIZE];

		template <bool black>
		DirectionFlags* effectBoard() {
			return black ? blackEffectBoard : whiteEffectBoard;
		}

		template <bool black>
		const DirectionFlags* effectBoardConst() const {
			return black ? blackEffectBoard : whiteEffectBoard;
		}

		template <bool black>
		void createEffect(const Board& board, const Square& sq);

		template <bool black, bool addition>
		void changeStraight(const Square& sq, const Direction dir, const DirectionFlags& dirFlags, const Board& board);

		template <bool black, bool addition>
		void changeAround(const Square& sq, const Board& board);

	public:
		EffectBoard() {
			init();
		}

		EffectBoard(const Board& board) {
			init(board);
		}

		void init();

		void init(const Board& board);

		template <bool black>
		void set(const Square& sq, const DirectionFlags& effect) {
			effectBoard<black>()[sq.getIndex()].set(effect);
		}

		template <bool black>
		void add(const Square& sq, const DirectionFlags& effect) {
			effectBoard<black>()[sq.getIndex()].add(effect);
		}

		template <bool black>
		void remove(const Square& sq, const DirectionFlags& effect) {
			effectBoard<black>()[sq.getIndex()].remove(effect);
		}

		template <bool black>
		bool isWall(const Square& sq) const {
			return effectBoard<black>()[sq.getIndex()].isWall();
		}

		template <bool black>
		const DirectionFlags& get(const Square& sq) const {
			return effectBoardConst<black>()[sq.getIndex()];
		}

		template <bool black, bool addition>
		void change(const Square& sq, const DirectionFlags& dirFlags, const Board& board);

		std::string toString() const;
	};
}

#endif /* EFFECTBOARD_H_ */
