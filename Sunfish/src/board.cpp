/*
 * board.cpp
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#include <sstream>
#include <iostream>
#include "board.h"

namespace Shogi {
	const Piece Board::even[Square::RANK_NUM][Square::FILE_NUM] = {
			{ Piece::WLANCE, Piece::WKNIGHT, Piece::WSILVER, Piece::WGOLD, Piece::WKING, Piece::WGOLD, Piece::WSILVER, Piece::WKNIGHT, Piece::WLANCE, },
			{ Piece::EMPTY , Piece::WBISHOP, Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::WROOK  , Piece::EMPTY, },
			{ Piece::WPAWN , Piece::WPAWN  , Piece::WPAWN  , Piece::WPAWN, Piece::WPAWN, Piece::WPAWN, Piece::WPAWN  , Piece::WPAWN  , Piece::WPAWN, },
			{ Piece::EMPTY , Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, },
			{ Piece::EMPTY , Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, },
			{ Piece::EMPTY , Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, },
			{ Piece::BPAWN , Piece::BPAWN  , Piece::BPAWN  , Piece::BPAWN, Piece::BPAWN, Piece::BPAWN, Piece::BPAWN  , Piece::BPAWN  , Piece::BPAWN, },
			{ Piece::EMPTY , Piece::BROOK  , Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::BBISHOP, Piece::EMPTY, },
			{ Piece::BLANCE, Piece::BKNIGHT, Piece::BSILVER, Piece::BGOLD, Piece::BKING, Piece::BGOLD, Piece::BSILVER, Piece::BKNIGHT, Piece::BLANCE, },
	};

	void Board::init(Handicap handicap) {
		for (Square sq(Square::TOP_W); sq.inside(); sq.inc()) {
			set(sq, Piece::WALL);
		}

		if (handicap == EMPTY) {
			for (Square sq(Square::TOP); sq.inside(); sq.next()) {
				set(sq, Piece::EMPTY);
			}
		} else {
			for (Square sq(Square::TOP); sq.inside(); sq.next()) {
				set(sq, even[sq.getRank()-1][sq.getFile()-1]);
			}
			switch (handicap) {
			case HANDICAP_8PIECES:
				set(Square(3, 1), Piece::EMPTY);
				set(Square(7, 1), Piece::EMPTY);
			case HANDICAP_6PIECES:
				set(Square(2, 1), Piece::EMPTY);
				set(Square(8, 1), Piece::EMPTY);
			case HANDICAP_4PIECES:
				set(Square(1, 1), Piece::EMPTY);
				set(Square(9, 1), Piece::EMPTY);
			case HANDICAP_2PIECES:
				set(Square(2, 2), Piece::EMPTY);
				set(Square(8, 2), Piece::EMPTY);
				break;
			case HANDICAP_ROOK_LANCE:
				set(Square(1, 1), Piece::EMPTY);
			case HANDICAO_ROOK:
				set(Square(8, 2), Piece::EMPTY);
				break;
			case HANDICAP_BISHOP:
				set(Square(2, 2), Piece::EMPTY);
				break;
			case HANDICAP_LANCE:
				set(Square(1, 1), Piece::EMPTY);
				break;
			default:
				break;
			}
		}
	}

	void Board::init(const Board& b) {
		for (Square pos(Square::TOP_W); pos.inside(); pos.inc()) {
			set(pos, b.get(pos));
		}
	}

	std::string Board::toString() const {
		std::ostringstream oss;
		Square pos(Square::TOP);
		for (pos.leftmost(); !get(pos).isWall(); pos.leftmost(), pos.down()) {
			for (; !get(pos).isWall(); pos.right()) {
				oss << get(pos).toString();
			}
			oss << '\n';
		}
		return oss.str();
	}
}
