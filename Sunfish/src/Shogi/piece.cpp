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

	const DirectionFlags Piece::direction[] = {
			DirectionFlags(DirectionFlags::NON),

			DirectionFlags(DirectionFlags::BPIECE),
			DirectionFlags(DirectionFlags::BLANCE),
			DirectionFlags(DirectionFlags::BKNIGHT),
			DirectionFlags(DirectionFlags::BSILVER),
			DirectionFlags(DirectionFlags::BGOLD),
			DirectionFlags(DirectionFlags::BBISHOP),
			DirectionFlags(DirectionFlags::BROOK),
			DirectionFlags(DirectionFlags::BKING),
			DirectionFlags(DirectionFlags::BGOLD),
			DirectionFlags(DirectionFlags::BGOLD),
			DirectionFlags(DirectionFlags::BGOLD),
			DirectionFlags(DirectionFlags::BGOLD),
			DirectionFlags(DirectionFlags::NON),
			DirectionFlags(DirectionFlags::BHORSE),
			DirectionFlags(DirectionFlags::BDRAGON),

			DirectionFlags(DirectionFlags::NON),

			DirectionFlags(DirectionFlags::WPIECE),
			DirectionFlags(DirectionFlags::WLANCE),
			DirectionFlags(DirectionFlags::WKNIGHT),
			DirectionFlags(DirectionFlags::WSILVER),
			DirectionFlags(DirectionFlags::WGOLD),
			DirectionFlags(DirectionFlags::WBISHOP),
			DirectionFlags(DirectionFlags::WROOK),
			DirectionFlags(DirectionFlags::WKING),
			DirectionFlags(DirectionFlags::WGOLD),
			DirectionFlags(DirectionFlags::WGOLD),
			DirectionFlags(DirectionFlags::WGOLD),
			DirectionFlags(DirectionFlags::WGOLD),
			DirectionFlags(DirectionFlags::NON),
			DirectionFlags(DirectionFlags::WHORSE),
			DirectionFlags(DirectionFlags::WDRAGON),
	};
}
