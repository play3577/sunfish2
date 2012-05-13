/*
 * position.h
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#ifndef POSITION_H_
#define POSITION_H_

#include "board.h"
#include "hand.h"

namespace Shogi {
	class Position {
	private:
		Board board;
		Hand blackHand;
		Hand whiteHand;

	public:
		Position() {
		}

		Position(Handicap handicap) : board(handicap) {
		}

		std::string toString() const;
	};
}

#endif /* POSITION_H_ */
