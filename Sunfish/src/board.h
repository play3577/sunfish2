/*
 * board.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "position.h"
#include "piece.h"
#include "handicap.h"

class Board {
private:
	static const Piece even[Position::RANK_NUM][Position::FILE_NUM];
	Piece board[Position::SIZE];

public:
	Board() {
		init();
	}

	Board(const Board& b) {
		init(b);
	}

	void init(Handicap handicap = Handicap::EVEN);

	void init(const Board& b);
};

#endif /* BOARD_H_ */
