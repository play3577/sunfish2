/*
 * attackTableGenerator.cpp
 *
 *  Created on: 2012/09/29
 *      Author: ryosuke
 */

#include "attackTableGenerator.h"
#include "../Shogi/piece.h"
#include "../Shogi/squareDiff.h"
#include <sstream>

namespace Develop {
	using namespace Shogi;

	std::string AttackTableGenerator::develop() {
		std::ostringstream oss;

		oss << "#include \"../squareDiff.h\"\n";
		oss << "namespace Shogi {\n";

		oss << "const int SquareDiff::_attackTable[][17][17] =\n";
		oss << "{\n";
		for (Piece piece = Piece::BPAWN; piece <= Piece::WDRAGON; piece.toNext()) {
			DirectionFlags flags = piece.getMoveableDirection();
			oss << "{\n";
			for (int rank = -8; rank <= 8; rank++) {
				oss << "{";
				for (int file = -8; file <= 8; file++) {
					SquareDiff diff(file+(rank<<Square::SHIFT_RANK));
					Direction dir = diff.toDirection().reverse();
					bool longRange = diff.toDistance() > 1;
					int result = 0;
					if (flags.check(dir, true) || (!longRange && flags.check(dir, false))) {
						result = (int)dir;
					}
					oss.width(3);
					oss << result << ",";
				}
				oss << "},\n";
			}
			oss << "},\n";
		}
		oss << "};\n";

		oss << "const int (*SquareDiff::attackTable)[17][17] =\n";
		oss << "(int (*)[17][17])&_attackTable[0][8][8];\n";

		oss << "}\n"; // namespace

		return oss.str();
	}
}
