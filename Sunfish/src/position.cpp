/*
 * position.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <sstream>
#include "position.h"

namespace Shogi {
	std::string Position::toString() const {
		std::ostringstream oss;
		oss << "Black:" << blackHand.toString();
		oss << board.toString();
		oss << "White:" << whiteHand.toString();
		return oss.str();
	}
}

