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
		for (Square sq = Square::TOP; sq.inside(); sq.next()) {
			const Piece& piece = pos.getBoard(sq);
			if (black && piece.isBlack()) {
				switch (piece.getInteger()) {
				case Piece::BPAWN:
					generate1Step<true>(Piece::BPAWN, sq, Direction::UP);
					break;
				case Piece::BLANCE:
					generateStraight<true>(Piece::BLANCE, sq, Direction::UP);
					break;
				case Piece::BKNIGHT:
					generate1Step<true>(Piece::BKNIGHT, sq, Direction::LEFT_UP2);
					generate1Step<true>(Piece::BKNIGHT, sq, Direction::RIGHT_UP2);
					break;
				case Piece::BSILVER:
					generate1Step<true>(Piece::BSILVER, sq, Direction::LEFT_UP);
					generate1Step<true>(Piece::BSILVER, sq, Direction::UP);
					generate1Step<true>(Piece::BSILVER, sq, Direction::RIGHT_UP);
					generate1Step<true>(Piece::BSILVER, sq, Direction::LEFT_DOWN);
					generate1Step<true>(Piece::BSILVER, sq, Direction::RIGHT_DOWN);
					break;
				case Piece::BGOLD:
					generate1Step<true>(Piece::BGOLD, sq, Direction::LEFT_UP);
					generate1Step<true>(Piece::BGOLD, sq, Direction::UP);
					generate1Step<true>(Piece::BGOLD, sq, Direction::RIGHT_UP);
					generate1Step<true>(Piece::BGOLD, sq, Direction::LEFT);
					generate1Step<true>(Piece::BGOLD, sq, Direction::RIGHT);
					generate1Step<true>(Piece::BGOLD, sq, Direction::DOWN);
					break;
				case Piece::BBISHOP:
					generateStraight<true>(Piece::BBISHOP, sq, Direction::LEFT_UP);
					generateStraight<true>(Piece::BBISHOP, sq, Direction::RIGHT_UP);
					generateStraight<true>(Piece::BBISHOP, sq, Direction::LEFT_DOWN);
					generateStraight<true>(Piece::BBISHOP, sq, Direction::RIGHT_DOWN);
					break;
				case Piece::BROOK:
					generateStraight<true>(Piece::BROOK, sq, Direction::UP);
					generateStraight<true>(Piece::BROOK, sq, Direction::DOWN);
					generateStraight<true>(Piece::BROOK, sq, Direction::LEFT);
					generateStraight<true>(Piece::BROOK, sq, Direction::RIGHT);
					break;
				case Piece::BKING:
					generate1Step<true>(Piece::BKING, sq, Direction::LEFT_UP);
					generate1Step<true>(Piece::BKING, sq, Direction::UP);
					generate1Step<true>(Piece::BKING, sq, Direction::RIGHT_UP);
					generate1Step<true>(Piece::BKING, sq, Direction::LEFT);
					generate1Step<true>(Piece::BKING, sq, Direction::RIGHT);
					generate1Step<true>(Piece::BKING, sq, Direction::LEFT_DOWN);
					generate1Step<true>(Piece::BKING, sq, Direction::DOWN);
					generate1Step<true>(Piece::BKING, sq, Direction::RIGHT_DOWN);
					break;
				case Piece::BTOKIN:
					generate1Step<true>(Piece::BTOKIN, sq, Direction::LEFT_UP);
					generate1Step<true>(Piece::BTOKIN, sq, Direction::UP);
					generate1Step<true>(Piece::BTOKIN, sq, Direction::RIGHT_UP);
					generate1Step<true>(Piece::BTOKIN, sq, Direction::LEFT);
					generate1Step<true>(Piece::BTOKIN, sq, Direction::RIGHT);
					generate1Step<true>(Piece::BTOKIN, sq, Direction::DOWN);
					break;
				case Piece::BPLANCE:
					generate1Step<true>(Piece::BPLANCE, sq, Direction::LEFT_UP);
					generate1Step<true>(Piece::BPLANCE, sq, Direction::UP);
					generate1Step<true>(Piece::BPLANCE, sq, Direction::RIGHT_UP);
					generate1Step<true>(Piece::BPLANCE, sq, Direction::LEFT);
					generate1Step<true>(Piece::BPLANCE, sq, Direction::RIGHT);
					generate1Step<true>(Piece::BPLANCE, sq, Direction::DOWN);
					break;
				case Piece::BPKNIGHT:
					generate1Step<true>(Piece::BPKNIGHT, sq, Direction::LEFT_UP);
					generate1Step<true>(Piece::BPKNIGHT, sq, Direction::UP);
					generate1Step<true>(Piece::BPKNIGHT, sq, Direction::RIGHT_UP);
					generate1Step<true>(Piece::BPKNIGHT, sq, Direction::LEFT);
					generate1Step<true>(Piece::BPKNIGHT, sq, Direction::RIGHT);
					generate1Step<true>(Piece::BPKNIGHT, sq, Direction::DOWN);
					break;
				case Piece::BPSILVER:
					generate1Step<true>(Piece::BPSILVER, sq, Direction::LEFT_UP);
					generate1Step<true>(Piece::BPSILVER, sq, Direction::UP);
					generate1Step<true>(Piece::BPSILVER, sq, Direction::RIGHT_UP);
					generate1Step<true>(Piece::BPSILVER, sq, Direction::LEFT);
					generate1Step<true>(Piece::BPSILVER, sq, Direction::RIGHT);
					generate1Step<true>(Piece::BPSILVER, sq, Direction::DOWN);
					break;
				case Piece::BHORSE:
					generateStraight<true>(Piece::BHORSE, sq, Direction::LEFT_UP);
					generateStraight<true>(Piece::BHORSE, sq, Direction::RIGHT_UP);
					generateStraight<true>(Piece::BHORSE, sq, Direction::LEFT_DOWN);
					generateStraight<true>(Piece::BHORSE, sq, Direction::RIGHT_DOWN);
					generate1Step<true>(Piece::BHORSE, sq, Direction::UP);
					generate1Step<true>(Piece::BHORSE, sq, Direction::LEFT);
					generate1Step<true>(Piece::BHORSE, sq, Direction::RIGHT);
					generate1Step<true>(Piece::BHORSE, sq, Direction::DOWN);
					break;
				case Piece::BDRAGON:
					generateStraight<true>(Piece::BDRAGON, sq, Direction::UP);
					generateStraight<true>(Piece::BDRAGON, sq, Direction::DOWN);
					generateStraight<true>(Piece::BDRAGON, sq, Direction::LEFT);
					generateStraight<true>(Piece::BDRAGON, sq, Direction::RIGHT);
					generate1Step<true>(Piece::BDRAGON, sq, Direction::LEFT_UP);
					generate1Step<true>(Piece::BDRAGON, sq, Direction::RIGHT_UP);
					generate1Step<true>(Piece::BDRAGON, sq, Direction::LEFT_DOWN);
					generate1Step<true>(Piece::BDRAGON, sq, Direction::RIGHT_DOWN);
					break;
				}
			} else if (!black && piece.isWhite()) {
				switch (piece.getInteger()) {
				case Piece::WPAWN:
					generate1Step<false>(Piece::WPAWN, sq, Direction::UP_R);
					break;
				case Piece::WLANCE:
					generateStraight<false>(Piece::WLANCE, sq, Direction::UP_R);
					break;
				case Piece::WKNIGHT:
					generate1Step<false>(Piece::WKNIGHT, sq, Direction::LEFT_UP2_R);
					generate1Step<false>(Piece::WKNIGHT, sq, Direction::RIGHT_UP2_R);
					break;
				case Piece::WSILVER:
					generate1Step<false>(Piece::WSILVER, sq, Direction::LEFT_UP_R);
					generate1Step<false>(Piece::WSILVER, sq, Direction::UP_R);
					generate1Step<false>(Piece::WSILVER, sq, Direction::RIGHT_UP_R);
					generate1Step<false>(Piece::WSILVER, sq, Direction::LEFT_DOWN_R);
					generate1Step<false>(Piece::WSILVER, sq, Direction::RIGHT_DOWN_R);
					break;
				case Piece::WGOLD:
					generate1Step<false>(Piece::WGOLD, sq, Direction::LEFT_UP_R);
					generate1Step<false>(Piece::WGOLD, sq, Direction::UP_R);
					generate1Step<false>(Piece::WGOLD, sq, Direction::RIGHT_UP_R);
					generate1Step<false>(Piece::WGOLD, sq, Direction::LEFT_R);
					generate1Step<false>(Piece::WGOLD, sq, Direction::RIGHT_R);
					generate1Step<false>(Piece::WGOLD, sq, Direction::DOWN_R);
					break;
				case Piece::WBISHOP:
					generateStraight<false>(Piece::WBISHOP, sq, Direction::LEFT_UP_R);
					generateStraight<false>(Piece::WBISHOP, sq, Direction::RIGHT_UP_R);
					generateStraight<false>(Piece::WBISHOP, sq, Direction::LEFT_DOWN_R);
					generateStraight<false>(Piece::WBISHOP, sq, Direction::RIGHT_DOWN_R);
					break;
				case Piece::WROOK:
					generateStraight<false>(Piece::WROOK, sq, Direction::UP_R);
					generateStraight<false>(Piece::WROOK, sq, Direction::DOWN_R);
					generateStraight<false>(Piece::WROOK, sq, Direction::LEFT_R);
					generateStraight<false>(Piece::WROOK, sq, Direction::RIGHT_R);
					break;
				case Piece::WKING:
					generate1Step<false>(Piece::WKING, sq, Direction::LEFT_UP_R);
					generate1Step<false>(Piece::WKING, sq, Direction::UP_R);
					generate1Step<false>(Piece::WKING, sq, Direction::RIGHT_UP_R);
					generate1Step<false>(Piece::WKING, sq, Direction::LEFT_R);
					generate1Step<false>(Piece::WKING, sq, Direction::RIGHT_R);
					generate1Step<false>(Piece::WKING, sq, Direction::LEFT_DOWN_R);
					generate1Step<false>(Piece::WKING, sq, Direction::DOWN_R);
					generate1Step<false>(Piece::WKING, sq, Direction::RIGHT_DOWN_R);
					break;
				case Piece::WTOKIN:
					generate1Step<false>(Piece::WTOKIN, sq, Direction::LEFT_UP_R);
					generate1Step<false>(Piece::WTOKIN, sq, Direction::UP_R);
					generate1Step<false>(Piece::WTOKIN, sq, Direction::RIGHT_UP_R);
					generate1Step<false>(Piece::WTOKIN, sq, Direction::LEFT_R);
					generate1Step<false>(Piece::WTOKIN, sq, Direction::RIGHT_R);
					generate1Step<false>(Piece::WTOKIN, sq, Direction::DOWN_R);
					break;
				case Piece::WPLANCE:
					generate1Step<false>(Piece::WPLANCE, sq, Direction::LEFT_UP_R);
					generate1Step<false>(Piece::WPLANCE, sq, Direction::UP_R);
					generate1Step<false>(Piece::WPLANCE, sq, Direction::RIGHT_UP_R);
					generate1Step<false>(Piece::WPLANCE, sq, Direction::LEFT_R);
					generate1Step<false>(Piece::WPLANCE, sq, Direction::RIGHT_R);
					generate1Step<false>(Piece::WPLANCE, sq, Direction::DOWN_R);
					break;
				case Piece::WPKNIGHT:
					generate1Step<false>(Piece::WPKNIGHT, sq, Direction::LEFT_UP_R);
					generate1Step<false>(Piece::WPKNIGHT, sq, Direction::UP_R);
					generate1Step<false>(Piece::WPKNIGHT, sq, Direction::RIGHT_UP_R);
					generate1Step<false>(Piece::WPKNIGHT, sq, Direction::LEFT_R);
					generate1Step<false>(Piece::WPKNIGHT, sq, Direction::RIGHT_R);
					generate1Step<false>(Piece::WPKNIGHT, sq, Direction::DOWN_R);
					break;
				case Piece::WPSILVER:
					generate1Step<false>(Piece::WPSILVER, sq, Direction::LEFT_UP_R);
					generate1Step<false>(Piece::WPSILVER, sq, Direction::UP_R);
					generate1Step<false>(Piece::WPSILVER, sq, Direction::RIGHT_UP_R);
					generate1Step<false>(Piece::WPSILVER, sq, Direction::LEFT_R);
					generate1Step<false>(Piece::WPSILVER, sq, Direction::RIGHT_R);
					generate1Step<false>(Piece::WPSILVER, sq, Direction::DOWN_R);
					break;
				case Piece::WHORSE:
					generateStraight<false>(Piece::WHORSE, sq, Direction::LEFT_UP_R);
					generateStraight<false>(Piece::WHORSE, sq, Direction::RIGHT_UP_R);
					generateStraight<false>(Piece::WHORSE, sq, Direction::LEFT_DOWN_R);
					generateStraight<false>(Piece::WHORSE, sq, Direction::RIGHT_DOWN_R);
					generate1Step<false>(Piece::WHORSE, sq, Direction::UP_R);
					generate1Step<false>(Piece::WHORSE, sq, Direction::LEFT_R);
					generate1Step<false>(Piece::WHORSE, sq, Direction::RIGHT_R);
					generate1Step<false>(Piece::WHORSE, sq, Direction::DOWN_R);
					break;
				case Piece::WDRAGON:
					generateStraight<false>(Piece::WDRAGON, sq, Direction::UP_R);
					generateStraight<false>(Piece::WDRAGON, sq, Direction::DOWN_R);
					generateStraight<false>(Piece::WDRAGON, sq, Direction::LEFT_R);
					generateStraight<false>(Piece::WDRAGON, sq, Direction::RIGHT_R);
					generate1Step<false>(Piece::WDRAGON, sq, Direction::LEFT_UP_R);
					generate1Step<false>(Piece::WDRAGON, sq, Direction::RIGHT_UP_R);
					generate1Step<false>(Piece::WDRAGON, sq, Direction::LEFT_DOWN_R);
					generate1Step<false>(Piece::WDRAGON, sq, Direction::RIGHT_DOWN_R);
					break;
				}
			}
		}
	}

	template <bool black>
	void MoveGenerator::generate1Step(const Piece piece, const Square from, const Direction dir) {
		Square to = from + dir;
		const Piece& piece2 = pos.getBoard(to);
		if (piece2.isEmpty() || (black && piece2.isWhite()) || (!black && piece2.isBlack())) {
			if (!piece.isPromoted() && !piece.isKing<black>() && to.isPromotableRank<black>()) {
				moves[num++] = Move((unsigned)from, (unsigned)to, true, false, (unsigned)piece);
			}
			if (!to.isCompulsoryPromotion(piece)) {
				moves[num++] = Move((unsigned)from, (unsigned)to, false, false, (unsigned)piece);
			}
		}
	}

	template <bool black>
	void MoveGenerator::generateStraight(const Piece piece, const Square from, const Direction dir) {
		Square to = from + dir;
		while (true) {
			const Piece& piece2 = pos.getBoard(to);
			if (piece2.isWall() || (black && piece2.isBlack()) || (!black && piece2.isWhite())) {
				break;
			}
			if (!piece.isPromoted() && !piece.isKing<black>() && to.isPromotableRank<black>()) {
				moves[num++] = Move((unsigned)from, (unsigned)to, true, false, (unsigned)piece);
			}
			if (!to.isCompulsoryPromotion(piece)) {
				moves[num++] = Move((unsigned)from, (unsigned)to, false, false, (unsigned)piece);
			}
			if (!piece2.isEmpty()) {
				break;
			}
			to += dir;
		}
	}
}
