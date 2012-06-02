/*
 * move.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <sstream>
#include "square.h"
#include "piece.h"
#include "move.h"

namespace Shogi{
	std::string Move::toString() const {
		std::ostringstream oss;
		oss << Square(m.to).toString();
		oss << Piece(m.piece).toStringNameOnly();
		if (m.promote) {
			oss << 'n';
		}
		if (!m.hand) {
			oss << '(' << Square(m.from).toString() << ')';
		} else {
			oss << "(00)";
		}
		return oss.str();
	}

	std::string Move::toString4Debug() const {
		std::ostringstream oss;
		oss << m.from << ',';
		oss << m.to << ',';
		oss << m.promote << ',';
		oss << m.hand << ',';
		oss << m.piece;
		return oss.str();
	}
}

