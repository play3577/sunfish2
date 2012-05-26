/*
 * hand.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <sstream>
#include "debug.h"
#include "hand.h"

namespace Shogi {
	std::string Hand::toString() const {
		std::ostringstream oss;
		for (Piece piece = Piece::PAWN; piece != Piece::KING; piece.toNext()) {
			int num = hand[piece];
			if (num > 0) {
				oss << ' ' << piece.toStringNameOnly();
				if (num != 1) {
					oss << num;
				}
			}
		}
		oss << '\n';
		return oss.str();
	}
}

