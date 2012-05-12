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
	const Piece Board::even[Position::RANK_NUM][Position::FILE_NUM] = {
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
		for (Position pos(Position::TOP_W); pos.inside(); pos.inc()) {
			set(pos, Piece::WALL);
		}

		if (handicap == EMPTY) {
			for (Position pos(Position::TOP); pos.inside(); pos.next()) {
				set(pos, Piece::EMPTY);
			}
		} else {
			for (Position pos(Position::TOP); pos.inside(); pos.next()) {
				set(pos, even[pos.getRank()][pos.getFile()]);
				std::cout << pos.getRank() << ',' << pos.getFile() << '\n';
			}
			std::cout << '\n';
			switch (handicap) {
			case HANDICAP_8PIECES:
			case HANDICAP_6PIECES:
			case HANDICAP_4PIECES:
			case HANDICAP_2PIECES:
				break;
			case ROOK_LANCE:
			case ROOK:
				break;
			case BISHOP:
				break;
			case LANCE:
				break;
			default:
				break;
			}
		}
	}

	void Board::init(const Board& b) {
		for (Position pos(Position::TOP_W); pos.inside(); pos.inc()) {
			set(pos, b.get(pos));
		}
	}

	std::string Board::toString() const {
		std::ostringstream oss;
		Position pos(Position::TOP);
		for (pos.leftmost(); !get(pos).isWall(); pos.leftmost(), pos.down()) {
			for (; !get(pos).isWall(); pos.right()) {
				oss << get(pos).toString();
				//oss << pos.getIndex() << ' ';
			}
			oss << '\n';
		}
		return oss.str();
	}
}
