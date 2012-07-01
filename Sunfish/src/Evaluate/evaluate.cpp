/*
 * evaluate.cpp
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */
#include "evaluate.h"

namespace Evaluate {
	using namespace Shogi;

	void Evaluate::init(const Position& pos) {
		value = 0;
		for (Square square = Square::TOP; square.valid(); square.next()) {
			Piece piece = pos.getBoard(square);
			value += param.getPiece(piece);
		}
		for (Piece piece = Piece::PAWN; piece <= Piece::ROOK; piece.toNext()) {
			int bnum = pos.getBlackHand(piece);
			value += param.getPiece(piece) * bnum;
			int wnum = pos.getBlackHand(piece);
			value -= param.getPiece(piece) * wnum;
		}
	}
}
