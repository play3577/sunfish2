/*
 * piece.cpp
 *
 *  Created on: 2012/05/12
 *      Author: ryosuke
 */

#include "piece.h"

namespace Shogi {
	const char Piece::pieceName[][4] = {
			"* ",
			"FU", "KY", "KE", "GI", "KI", "KA", "HI", "OU",
			"TO", "NY", "NK", "NG", "??", "UM", "RY", "??",
	};

	const char Piece::UNKNOWN_NAME[] = "??";
}
