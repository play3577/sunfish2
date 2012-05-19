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
		clear();
		if (pos.isBlackTurn()) {
			generateOnBoard<true>();
		} else {
			generateOnBoard<false>();
		}
		return num;
	}

	template <bool black>
	void MoveGenerator::generateOnBoard() {
		for (Square sq = Square::TOP; sq.valid(); sq.next()) {
			const Piece& piece = pos.getBoard(sq);
			if (black && piece.isBlack()) {
				switch (piece.getInteger()) {
				case Piece::BPAWN:
					generateStraight<true, true>(Piece::BPAWN, sq, Direction::UP);
					break;
				case Piece::BLANCE:
					generateStraight<true, false>(Piece::BLANCE, sq, Direction::UP);
					break;
				case Piece::BKNIGHT:
					generateStraight<true, true>(Piece::BKNIGHT, sq, Direction::LEFT_UP2);
					generateStraight<true, true>(Piece::BKNIGHT, sq, Direction::RIGHT_UP2);
					break;
				case Piece::BSILVER:
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::LEFT_UP);
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::UP);
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::RIGHT_UP);
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::LEFT_DOWN);
					generateStraight<true, true>(Piece::BSILVER, sq, Direction::RIGHT_DOWN);
					break;
				case Piece::BGOLD:
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::LEFT_UP);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::UP);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::RIGHT_UP);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::LEFT);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::RIGHT);
					generateStraight<true, true>(Piece::BGOLD, sq, Direction::DOWN);
					break;
				case Piece::BBISHOP:
					generateStraight<true, false>(Piece::BBISHOP, sq, Direction::LEFT_UP);
					generateStraight<true, false>(Piece::BBISHOP, sq, Direction::RIGHT_UP);
					generateStraight<true, false>(Piece::BBISHOP, sq, Direction::LEFT_DOWN);
					generateStraight<true, false>(Piece::BBISHOP, sq, Direction::RIGHT_DOWN);
					break;
				case Piece::BROOK:
					generateStraight<true, false>(Piece::BROOK, sq, Direction::UP);
					generateStraight<true, false>(Piece::BROOK, sq, Direction::DOWN);
					generateStraight<true, false>(Piece::BROOK, sq, Direction::LEFT);
					generateStraight<true, false>(Piece::BROOK, sq, Direction::RIGHT);
					break;
				case Piece::BKING:
					generateStraight<true, true>(Piece::BKING, sq, Direction::LEFT_UP);
					generateStraight<true, true>(Piece::BKING, sq, Direction::UP);
					generateStraight<true, true>(Piece::BKING, sq, Direction::RIGHT_UP);
					generateStraight<true, true>(Piece::BKING, sq, Direction::LEFT);
					generateStraight<true, true>(Piece::BKING, sq, Direction::RIGHT);
					generateStraight<true, true>(Piece::BKING, sq, Direction::LEFT_DOWN);
					generateStraight<true, true>(Piece::BKING, sq, Direction::DOWN);
					generateStraight<true, true>(Piece::BKING, sq, Direction::RIGHT_DOWN);
					break;
				case Piece::BTOKIN:
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::LEFT_UP);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::UP);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::RIGHT_UP);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::LEFT);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::RIGHT);
					generateStraight<true, true>(Piece::BTOKIN, sq, Direction::DOWN);
					break;
				case Piece::BPLANCE:
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::LEFT_UP);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::UP);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::RIGHT_UP);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::LEFT);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::RIGHT);
					generateStraight<true, true>(Piece::BPLANCE, sq, Direction::DOWN);
					break;
				case Piece::BPKNIGHT:
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::LEFT_UP);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::UP);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::RIGHT_UP);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::LEFT);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::RIGHT);
					generateStraight<true, true>(Piece::BPKNIGHT, sq, Direction::DOWN);
					break;
				case Piece::BPSILVER:
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::LEFT_UP);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::UP);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::RIGHT_UP);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::LEFT);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::RIGHT);
					generateStraight<true, true>(Piece::BPSILVER, sq, Direction::DOWN);
					break;
				case Piece::BHORSE:
					generateStraight<true, false>(Piece::BHORSE, sq, Direction::LEFT_UP);
					generateStraight<true, false>(Piece::BHORSE, sq, Direction::RIGHT_UP);
					generateStraight<true, false>(Piece::BHORSE, sq, Direction::LEFT_DOWN);
					generateStraight<true, false>(Piece::BHORSE, sq, Direction::RIGHT_DOWN);
					generateStraight<true, true>(Piece::BHORSE, sq, Direction::UP);
					generateStraight<true, true>(Piece::BHORSE, sq, Direction::LEFT);
					generateStraight<true, true>(Piece::BHORSE, sq, Direction::RIGHT);
					generateStraight<true, true>(Piece::BHORSE, sq, Direction::DOWN);
					break;
				case Piece::BDRAGON:
					generateStraight<true, false>(Piece::BDRAGON, sq, Direction::UP);
					generateStraight<true, false>(Piece::BDRAGON, sq, Direction::DOWN);
					generateStraight<true, false>(Piece::BDRAGON, sq, Direction::LEFT);
					generateStraight<true, false>(Piece::BDRAGON, sq, Direction::RIGHT);
					generateStraight<true, true>(Piece::BDRAGON, sq, Direction::LEFT_UP);
					generateStraight<true, true>(Piece::BDRAGON, sq, Direction::RIGHT_UP);
					generateStraight<true, true>(Piece::BDRAGON, sq, Direction::LEFT_DOWN);
					generateStraight<true, true>(Piece::BDRAGON, sq, Direction::RIGHT_DOWN);
					break;
				}
			} else if (!black && piece.isWhite()) {
				switch (piece.getInteger()) {
				case Piece::WPAWN:
					generateStraight<false, true>(Piece::WPAWN, sq, Direction::UP_R);
					break;
				case Piece::WLANCE:
					generateStraight<false, false>(Piece::WLANCE, sq, Direction::UP_R);
					break;
				case Piece::WKNIGHT:
					generateStraight<false, true>(Piece::WKNIGHT, sq, Direction::LEFT_UP2_R);
					generateStraight<false, true>(Piece::WKNIGHT, sq, Direction::RIGHT_UP2_R);
					break;
				case Piece::WSILVER:
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::LEFT_UP_R);
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::UP_R);
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::RIGHT_UP_R);
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::LEFT_DOWN_R);
					generateStraight<false, true>(Piece::WSILVER, sq, Direction::RIGHT_DOWN_R);
					break;
				case Piece::WGOLD:
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::LEFT_UP_R);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::UP_R);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::RIGHT_UP_R);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::LEFT_R);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::RIGHT_R);
					generateStraight<false, true>(Piece::WGOLD, sq, Direction::DOWN_R);
					break;
				case Piece::WBISHOP:
					generateStraight<false, false>(Piece::WBISHOP, sq, Direction::LEFT_UP_R);
					generateStraight<false, false>(Piece::WBISHOP, sq, Direction::RIGHT_UP_R);
					generateStraight<false, false>(Piece::WBISHOP, sq, Direction::LEFT_DOWN_R);
					generateStraight<false, false>(Piece::WBISHOP, sq, Direction::RIGHT_DOWN_R);
					break;
				case Piece::WROOK:
					generateStraight<false, false>(Piece::WROOK, sq, Direction::UP_R);
					generateStraight<false, false>(Piece::WROOK, sq, Direction::DOWN_R);
					generateStraight<false, false>(Piece::WROOK, sq, Direction::LEFT_R);
					generateStraight<false, false>(Piece::WROOK, sq, Direction::RIGHT_R);
					break;
				case Piece::WKING:
					generateStraight<false, true>(Piece::WKING, sq, Direction::LEFT_UP_R);
					generateStraight<false, true>(Piece::WKING, sq, Direction::UP_R);
					generateStraight<false, true>(Piece::WKING, sq, Direction::RIGHT_UP_R);
					generateStraight<false, true>(Piece::WKING, sq, Direction::LEFT_R);
					generateStraight<false, true>(Piece::WKING, sq, Direction::RIGHT_R);
					generateStraight<false, true>(Piece::WKING, sq, Direction::LEFT_DOWN_R);
					generateStraight<false, true>(Piece::WKING, sq, Direction::DOWN_R);
					generateStraight<false, true>(Piece::WKING, sq, Direction::RIGHT_DOWN_R);
					break;
				case Piece::WTOKIN:
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::LEFT_UP_R);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::UP_R);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::RIGHT_UP_R);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::LEFT_R);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::RIGHT_R);
					generateStraight<false, true>(Piece::WTOKIN, sq, Direction::DOWN_R);
					break;
				case Piece::WPLANCE:
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::LEFT_UP_R);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::UP_R);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::RIGHT_UP_R);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::LEFT_R);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::RIGHT_R);
					generateStraight<false, true>(Piece::WPLANCE, sq, Direction::DOWN_R);
					break;
				case Piece::WPKNIGHT:
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::LEFT_UP_R);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::UP_R);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::RIGHT_UP_R);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::LEFT_R);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::RIGHT_R);
					generateStraight<false, true>(Piece::WPKNIGHT, sq, Direction::DOWN_R);
					break;
				case Piece::WPSILVER:
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::LEFT_UP_R);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::UP_R);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::RIGHT_UP_R);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::LEFT_R);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::RIGHT_R);
					generateStraight<false, true>(Piece::WPSILVER, sq, Direction::DOWN_R);
					break;
				case Piece::WHORSE:
					generateStraight<false, false>(Piece::WHORSE, sq, Direction::LEFT_UP_R);
					generateStraight<false, false>(Piece::WHORSE, sq, Direction::RIGHT_UP_R);
					generateStraight<false, false>(Piece::WHORSE, sq, Direction::LEFT_DOWN_R);
					generateStraight<false, false>(Piece::WHORSE, sq, Direction::RIGHT_DOWN_R);
					generateStraight<false, true>(Piece::WHORSE, sq, Direction::UP_R);
					generateStraight<false, true>(Piece::WHORSE, sq, Direction::LEFT_R);
					generateStraight<false, true>(Piece::WHORSE, sq, Direction::RIGHT_R);
					generateStraight<false, true>(Piece::WHORSE, sq, Direction::DOWN_R);
					break;
				case Piece::WDRAGON:
					generateStraight<false, false>(Piece::WDRAGON, sq, Direction::UP_R);
					generateStraight<false, false>(Piece::WDRAGON, sq, Direction::DOWN_R);
					generateStraight<false, false>(Piece::WDRAGON, sq, Direction::LEFT_R);
					generateStraight<false, false>(Piece::WDRAGON, sq, Direction::RIGHT_R);
					generateStraight<false, true>(Piece::WDRAGON, sq, Direction::LEFT_UP_R);
					generateStraight<false, true>(Piece::WDRAGON, sq, Direction::RIGHT_UP_R);
					generateStraight<false, true>(Piece::WDRAGON, sq, Direction::LEFT_DOWN_R);
					generateStraight<false, true>(Piece::WDRAGON, sq, Direction::RIGHT_DOWN_R);
					break;
				}
			}
		}
	}

//	template <bool black>
//	void MoveGenerator::generate1Step(const Piece piece, const Square from, const Direction dir) {
//		Square to = from + dir;
//		const Piece& piece2 = pos.getBoard(to);
//		if (piece2.isEmpty() || (black && piece2.isWhite()) || (!black && piece2.isBlack())) {
//			if (!piece.isPromoted() && !piece.isKing<black>() && to.isPromotableRank<black>()) {
//				moves[num++] = Move(from, to, true, false, piece);
//			}
//			if (!to.isCompulsoryPromotion(piece)) {
//				moves[num++] = Move(from, to, false, false, piece);
//			}
//		}
//	}

	template <bool black, bool oneStep>
	void MoveGenerator::generateStraight(const Piece piece, const Square from, const Direction dir) {
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
