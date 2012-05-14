/*
 * board.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <string>
#include "piece.h"
#include "handicap.h"
#include "square.h"

namespace Shogi {
	class Board {
	private:
		static const Piece even[Square::RANK_NUM][Square::FILE_NUM];
		Piece board[Square::SIZE];

	public:
		Board(Handicap handicap = EVEN) {
			init(handicap);
		}

		Board(const Board& b) {
			init(b);
		}

		void init(Handicap handicap = EVEN);

		void init(const Board& b);

		const Piece& get(const Square& sq) const {
			return board[sq.getIndex()];
		}

		Piece set(const Square& sq, const Piece& piece) {
			Piece oldPiece = board[sq.getIndex()];
			board[sq.getIndex()] = piece;
			return oldPiece;
		}

		std::string toString() const;
	};
}

#endif /* BOARD_H_ */
