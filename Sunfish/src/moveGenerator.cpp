/*
 * moveGenerator.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <iostream>
#include "square.h"
#include "moveGenerator.h"

namespace Shogi {
	MoveGenerator::MoveGenerator(const Position& pos) : pos(pos) {
		clear();
	}

	unsigned MoveGenerator::generate() {
		if (pos.isCheck()) {
		} else {
			if (pos.isBlackTurn()) {
				generateOnBoard<true>();
			} else {
				generateOnBoard<false>();
			}
		}
		return num;
	}

	template <bool black>
	void MoveGenerator::generateOnBoard() {
		for (Square sq = Square::TOP; sq.valid(); sq.next()) {
			const Piece& piece = pos.getBoard(sq);
			const Direction pin = pos.pin<black>(sq).toDirection();
			if (black && piece.isBlack()) {
				switch (piece.getInteger()) {
				case Piece::BPAWN:
					generateStraight<true, true>(Piece::BPAWN, sq, Direction::UP, pin);
					break;
				case Piece::BLANCE:
					generateStraight<true, false>(Piece::BLANCE, sq, Direction::UP, pin);
					break;
				case Piece::BKNIGHT:
					generateStraight<true, true>(Piece::BKNIGHT, sq, Direction::LEFT_UP2, pin);
					generateStraight<true, true>(Piece::BKNIGHT, sq, Direction::RIGHT_UP2, pin);
					break;
				case Piece::BSILVER:
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::UP, pin);
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::RIGHT_DOWN, pin);
					break;
				case Piece::BGOLD:
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::UP, pin);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::LEFT, pin);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::RIGHT, pin);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::DOWN, pin);
					break;
				case Piece::BBISHOP:
					generateStraight<true, false>(Piece::BBISHOP, sq, Direction::LEFT_UP, pin);
					generateStraight<true, false>(Piece::BBISHOP, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, false>(Piece::BBISHOP, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, false>(Piece::BBISHOP, sq, Direction::RIGHT_DOWN, pin);
					break;
				case Piece::BROOK:
					generateStraight<true, false>(Piece::BROOK, sq, Direction::UP, pin);
					generateStraight<true, false>(Piece::BROOK, sq, Direction::DOWN, pin);
					generateStraight<true, false>(Piece::BROOK, sq, Direction::LEFT, pin);
					generateStraight<true, false>(Piece::BROOK, sq, Direction::RIGHT, pin);
					break;
				case Piece::BKING:
					generateStraight<true, true>(Piece::BKING, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true>(Piece::BKING, sq, Direction::UP, pin);
					generateStraight<true, true>(Piece::BKING, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true>(Piece::BKING, sq, Direction::LEFT, pin);
					generateStraight<true, true>(Piece::BKING, sq, Direction::RIGHT, pin);
					generateStraight<true, true>(Piece::BKING, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, true>(Piece::BKING, sq, Direction::DOWN, pin);
					generateStraight<true, true>(Piece::BKING, sq, Direction::RIGHT_DOWN, pin);
					break;
				case Piece::BTOKIN:
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::UP, pin);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::LEFT, pin);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::RIGHT, pin);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::DOWN, pin);
					break;
				case Piece::BPLANCE:
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::UP, pin);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::LEFT, pin);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::RIGHT, pin);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::DOWN, pin);
					break;
				case Piece::BPKNIGHT:
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::UP, pin);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::LEFT, pin);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::RIGHT, pin);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::DOWN, pin);
					break;
				case Piece::BPSILVER:
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::UP, pin);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::LEFT, pin);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::RIGHT, pin);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::DOWN, pin);
					break;
				case Piece::BHORSE:
					generateStraight<true, false>(Piece::BHORSE, sq, Direction::LEFT_UP, pin);
					generateStraight<true, false>(Piece::BHORSE, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, false>(Piece::BHORSE, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, false>(Piece::BHORSE, sq, Direction::RIGHT_DOWN, pin);
					generateStraight<true, true>(Piece::BHORSE, sq, Direction::UP, pin);
					generateStraight<true, true>(Piece::BHORSE, sq, Direction::LEFT, pin);
					generateStraight<true, true>(Piece::BHORSE, sq, Direction::RIGHT, pin);
					generateStraight<true, true>(Piece::BHORSE, sq, Direction::DOWN, pin);
					break;
				case Piece::BDRAGON:
					generateStraight<true, false>(Piece::BDRAGON, sq, Direction::UP, pin);
					generateStraight<true, false>(Piece::BDRAGON, sq, Direction::DOWN, pin);
					generateStraight<true, false>(Piece::BDRAGON, sq, Direction::LEFT, pin);
					generateStraight<true, false>(Piece::BDRAGON, sq, Direction::RIGHT, pin);
					generateStraight<true, true>(Piece::BDRAGON, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true>(Piece::BDRAGON, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true>(Piece::BDRAGON, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, true>(Piece::BDRAGON, sq, Direction::RIGHT_DOWN, pin);
					break;
				}
			} else if (!black && piece.isWhite()) {
				switch (piece.getInteger()) {
				case Piece::WPAWN:
					generateStraight<false, true>(Piece::WPAWN, sq, Direction::UP_R, pin);
					break;
				case Piece::WLANCE:
					generateStraight<false, false>(Piece::WLANCE, sq, Direction::UP_R, pin);
					break;
				case Piece::WKNIGHT:
					generateStraight<false, true>(Piece::WKNIGHT, sq, Direction::LEFT_UP2_R, pin);
					generateStraight<false, true>(Piece::WKNIGHT, sq, Direction::RIGHT_UP2_R, pin);
					break;
				case Piece::WSILVER:
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::UP_R, pin);
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				case Piece::WGOLD:
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::UP_R, pin);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::LEFT_R, pin);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WBISHOP:
					generateStraight<false, false>(Piece::WBISHOP, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, false>(Piece::WBISHOP, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, false>(Piece::WBISHOP, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, false>(Piece::WBISHOP, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				case Piece::WROOK:
					generateStraight<false, false>(Piece::WROOK, sq, Direction::UP_R, pin);
					generateStraight<false, false>(Piece::WROOK, sq, Direction::DOWN_R, pin);
					generateStraight<false, false>(Piece::WROOK, sq, Direction::LEFT_R, pin);
					generateStraight<false, false>(Piece::WROOK, sq, Direction::RIGHT_R, pin);
					break;
				case Piece::WKING:
					generateStraight<false, true>(Piece::WKING, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true>(Piece::WKING, sq, Direction::UP_R, pin);
					generateStraight<false, true>(Piece::WKING, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true>(Piece::WKING, sq, Direction::LEFT_R, pin);
					generateStraight<false, true>(Piece::WKING, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true>(Piece::WKING, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, true>(Piece::WKING, sq, Direction::DOWN_R, pin);
					generateStraight<false, true>(Piece::WKING, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				case Piece::WTOKIN:
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::UP_R, pin);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::LEFT_R, pin);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPLANCE:
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::UP_R, pin);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::LEFT_R, pin);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPKNIGHT:
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::UP_R, pin);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::LEFT_R, pin);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPSILVER:
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::UP_R, pin);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::LEFT_R, pin);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WHORSE:
					generateStraight<false, false>(Piece::WHORSE, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, false>(Piece::WHORSE, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, false>(Piece::WHORSE, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, false>(Piece::WHORSE, sq, Direction::RIGHT_DOWN_R, pin);
					generateStraight<false, true>(Piece::WHORSE, sq, Direction::UP_R, pin);
					generateStraight<false, true>(Piece::WHORSE, sq, Direction::LEFT_R, pin);
					generateStraight<false, true>(Piece::WHORSE, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true>(Piece::WHORSE, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WDRAGON:
					generateStraight<false, false>(Piece::WDRAGON, sq, Direction::UP_R, pin);
					generateStraight<false, false>(Piece::WDRAGON, sq, Direction::DOWN_R, pin);
					generateStraight<false, false>(Piece::WDRAGON, sq, Direction::LEFT_R, pin);
					generateStraight<false, false>(Piece::WDRAGON, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true>(Piece::WDRAGON, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true>(Piece::WDRAGON, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true>(Piece::WDRAGON, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, true>(Piece::WDRAGON, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				}
			}
		}
	}

	template <bool black, bool oneStep>
	void MoveGenerator::generateStraight(const Piece piece, const Square from, const Direction dir, const Direction pin) {
		if (pin != Direction::NON && dir != pin && dir != pin.reverse()) {
			return;
		}
		Square to = from + dir;
		while (true) {
			const Piece& piece2 = pos.getBoard(to);
			if (piece2.isWall() || (black && piece2.isBlack()) || (!black && piece2.isWhite())) {
				break;
			}
			if (!piece.isPromoted() && !piece.isKing<black>() && to.isPromotableRank<black>()) {
				moves[num++] = Move(from, to, true, false, piece);
			}
			if (!to.isCompulsoryPromotion(piece)) {
				moves[num++] = Move(from, to, false, false, piece);
			}
			if(oneStep) {
				break;
			}
			if (!piece2.isEmpty()) {
				break;
			}
			to += dir;
		}
	}
}
