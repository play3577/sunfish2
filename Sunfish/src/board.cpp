/*
 * board.cpp
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#include "board.h"

const Piece Board::even[Position::RANK_NUM][Position::FILE_NUM] = {
		Piece::WLANCE, Piece::WKNIGHT, Piece::WSILVER, Piece::WGOLD, Piece::WKING, Piece::WGOLD, Piece::WSILVER, Piece::WKNIGHT, Piece::WLANCE,
		Piece::EMPTY , Piece::WBISHOP, Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::WROOK  , Piece::EMPTY,
		Piece::WPAWN , Piece::WPAWN  , Piece::WPAWN  , Piece::WPAWN, Piece::WPAWN, Piece::WPAWN, Piece::WPAWN  , Piece::WPAWN  , Piece::WPAWN,
		Piece::EMPTY , Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY,
		Piece::EMPTY , Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY,
		Piece::EMPTY , Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::EMPTY  , Piece::EMPTY,
		Piece::BPAWN , Piece::BPAWN  , Piece::BPAWN  , Piece::BPAWN, Piece::BPAWN, Piece::BPAWN, Piece::BPAWN  , Piece::BPAWN  , Piece::BPAWN,
		Piece::EMPTY , Piece::BROOK  , Piece::EMPTY  , Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY  , Piece::BBISHOP, Piece::EMPTY,
		Piece::BLANCE, Piece::BKNIGHT, Piece::BSILVER, Piece::BGOLD, Piece::BKING, Piece::BGOLD, Piece::BSILVER, Piece::BKNIGHT, Piece::BLANCE,
};

void Board::init(Handicap handicap) {
	for (Position pos(Position::WTOP); pos.inside(); pos.inc()) {
		board[pos.getIndex()] = Piece::WALL;
	}

	if (handicap == EMPTY) {
		for (Position pos(Position::TOP); pos.inside(); pos.next()) {
			board[pos.getIndex()] = Piece::EMPTY;
		}
	} else {
		for (Position pos(Position::TOP); pos.inside(); pos.next()) {
			board[pos.getIndex()] = even[pos.getRank()][pos.getFile()];
		}
		switch (handicap) {
		case Handicap::HANDICAP_8PIECES:
		case Handicap::HANDICAP_6PIECES:
		case Handicap::HANDICAP_4PIECES:
		case Handicap::HANDICAP_2PIECES:
			break;
		case Handicap::ROOK_LANCE:
		case Handicap::ROOK:
			break;
		case Handicap::BISHOP:
			break;
		case Handicap::LANCE:
			break;
		}
	}
}

void Board::init(const Board& b) {
	for (Position pos(Position::WTOP); pos.inside(); pos.inc()) {
		board[pos.getIndex()] = b.board[pos.getIndex()];
	}
}
