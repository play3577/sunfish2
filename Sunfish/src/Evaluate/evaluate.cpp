/*
 * evaluate.cpp
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#include "evaluate.h"
#include "feature.h"

namespace Evaluate {
	using namespace Shogi;

	void Evaluate::init(const Position& pos) {
		baseValue = 0;
		for (Square square = Square::TOP; square.valid(); square.next()) {
			Piece piece = pos.getBoard(square);
			baseValue += param.getPiece(piece);
		}
		for (Piece piece = Piece::PAWN; piece <= Piece::ROOK; piece.toNext()) {
			int bnum = pos.getBlackHand(piece);
			baseValue += param.getPiece(piece) * bnum;
			int wnum = pos.getBlackHand(piece);
			baseValue -= param.getPiece(piece) * wnum;
		}
	}

	Value Evaluate::getAdditionalValue(const Position& pos) const {
		return Feature::getValue<Value, ValueS>(pos, &param);
	}

}
