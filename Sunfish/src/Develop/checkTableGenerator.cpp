/*
 * checkTableGenerator.cpp
 *
 *  Created on: 2012/09/29
 *      Author: ryosuke
 */

#include "checkTableGenerator.h"
#include "../Shogi/piece.h"
#include "../Shogi/squareDiff.h"
#include <sstream>

namespace Develop {
	using namespace Shogi;

	std::string CheckTableGenerator::develop() {
		std::ostringstream oss;

		for (Piece piece = Piece::BPAWN; piece <= Piece::WDRAGON; piece.toNext()) {
			for (int rank = -8; rank <= 8; rank++) {
				for (int file = -8; file <= 8; file++) {
					SquareDiff diff(file+(rank<<Square::SHIFT_RANK));
				}
			}
		}

		return oss.str();
	}
}
