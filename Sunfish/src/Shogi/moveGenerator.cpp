/*
 * moveGenerator.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <cassert>
#include <iostream>
#include "../Debug/debug.h"
#include "square.h"
#include "moveGenerator.h"

namespace Shogi {
	MoveGenerator::MoveGenerator(const Position& pos) : pos(pos) {
		clear();
	}

	unsigned MoveGenerator::generate() {
		if (pos.isCheck()) {
			if (pos.isBlackTurn()) {
				generateEvasion<true>();
				generateKing<true, true>();
			} else {
				generateEvasion<false>();
				generateKing<false, true>();
			}
		} else {
			if (pos.isBlackTurn()) {
				generateOnBoard<true>();
				generateKing<true, false>();
				generateDrop<true>();
			} else {
				generateOnBoard<false>();
				generateKing<false, false>();
				generateDrop<false>();
			}
		}
		return num;
	}

	template <bool black>
	void MoveGenerator::generateOnBoard() {
		for (Square sq = Square::TOP; sq.valid(); sq.next()) {
			const Piece& piece = pos.getBoard(sq);
			const Direction pin = pos.pin(sq, black).toDirection();
			if (black && piece.isBlack()) {
				switch (piece.getInteger()) {
				case Piece::BPAWN:
					generateStraight<true, true, true>(Piece::BPAWN, sq, Direction::UP, pin);
					break;
				case Piece::BLANCE:
					generateStraight<true, false, true>(Piece::BLANCE, sq, Direction::UP, pin);
					break;
				case Piece::BKNIGHT:
					generateStraight<true, true, true>(Piece::BKNIGHT, sq, Direction::LEFT_UP2, pin);
					generateStraight<true, true, true>(Piece::BKNIGHT, sq, Direction::RIGHT_UP2, pin);
					break;
				case Piece::BSILVER:
					generateStraight<true, true, true>(Piece::BSILVER, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, true>(Piece::BSILVER, sq, Direction::UP, pin);
					generateStraight<true, true, true>(Piece::BSILVER, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, true>(Piece::BSILVER, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, true, true>(Piece::BSILVER, sq, Direction::RIGHT_DOWN, pin);
					break;
				case Piece::BGOLD:
					generateStraight<true, true, false>(Piece::BGOLD, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false>(Piece::BGOLD, sq, Direction::UP, pin);
					generateStraight<true, true, false>(Piece::BGOLD, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false>(Piece::BGOLD, sq, Direction::LEFT, pin);
					generateStraight<true, true, false>(Piece::BGOLD, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false>(Piece::BGOLD, sq, Direction::DOWN, pin);
					break;
				case Piece::BBISHOP:
					generateStraight<true, false, true>(Piece::BBISHOP, sq, Direction::LEFT_UP, pin);
					generateStraight<true, false, true>(Piece::BBISHOP, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, false, true>(Piece::BBISHOP, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, false, true>(Piece::BBISHOP, sq, Direction::RIGHT_DOWN, pin);
					break;
				case Piece::BROOK:
					generateStraight<true, false, true>(Piece::BROOK, sq, Direction::UP, pin);
					generateStraight<true, false, true>(Piece::BROOK, sq, Direction::DOWN, pin);
					generateStraight<true, false, true>(Piece::BROOK, sq, Direction::LEFT, pin);
					generateStraight<true, false, true>(Piece::BROOK, sq, Direction::RIGHT, pin);
					break;
				case Piece::BTOKIN:
					generateStraight<true, true, false>(Piece::BTOKIN, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false>(Piece::BTOKIN, sq, Direction::UP, pin);
					generateStraight<true, true, false>(Piece::BTOKIN, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false>(Piece::BTOKIN, sq, Direction::LEFT, pin);
					generateStraight<true, true, false>(Piece::BTOKIN, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false>(Piece::BTOKIN, sq, Direction::DOWN, pin);
					break;
				case Piece::BPLANCE:
					generateStraight<true, true, false>(Piece::BPLANCE, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false>(Piece::BPLANCE, sq, Direction::UP, pin);
					generateStraight<true, true, false>(Piece::BPLANCE, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false>(Piece::BPLANCE, sq, Direction::LEFT, pin);
					generateStraight<true, true, false>(Piece::BPLANCE, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false>(Piece::BPLANCE, sq, Direction::DOWN, pin);
					break;
				case Piece::BPKNIGHT:
					generateStraight<true, true, false>(Piece::BPKNIGHT, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false>(Piece::BPKNIGHT, sq, Direction::UP, pin);
					generateStraight<true, true, false>(Piece::BPKNIGHT, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false>(Piece::BPKNIGHT, sq, Direction::LEFT, pin);
					generateStraight<true, true, false>(Piece::BPKNIGHT, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false>(Piece::BPKNIGHT, sq, Direction::DOWN, pin);
					break;
				case Piece::BPSILVER:
					generateStraight<true, true, false>(Piece::BPSILVER, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false>(Piece::BPSILVER, sq, Direction::UP, pin);
					generateStraight<true, true, false>(Piece::BPSILVER, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false>(Piece::BPSILVER, sq, Direction::LEFT, pin);
					generateStraight<true, true, false>(Piece::BPSILVER, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false>(Piece::BPSILVER, sq, Direction::DOWN, pin);
					break;
				case Piece::BHORSE:
					generateStraight<true, false, false>(Piece::BHORSE, sq, Direction::LEFT_UP, pin);
					generateStraight<true, false, false>(Piece::BHORSE, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, false, false>(Piece::BHORSE, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, false, false>(Piece::BHORSE, sq, Direction::RIGHT_DOWN, pin);
					generateStraight<true, true, false>(Piece::BHORSE, sq, Direction::UP, pin);
					generateStraight<true, true, false>(Piece::BHORSE, sq, Direction::LEFT, pin);
					generateStraight<true, true, false>(Piece::BHORSE, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false>(Piece::BHORSE, sq, Direction::DOWN, pin);
					break;
				case Piece::BDRAGON:
					generateStraight<true, false, false>(Piece::BDRAGON, sq, Direction::UP, pin);
					generateStraight<true, false, false>(Piece::BDRAGON, sq, Direction::DOWN, pin);
					generateStraight<true, false, false>(Piece::BDRAGON, sq, Direction::LEFT, pin);
					generateStraight<true, false, false>(Piece::BDRAGON, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false>(Piece::BDRAGON, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false>(Piece::BDRAGON, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false>(Piece::BDRAGON, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, true, false>(Piece::BDRAGON, sq, Direction::RIGHT_DOWN, pin);
					break;
				}
			} else if (!black && piece.isWhite()) {
				switch (piece.getInteger()) {
				case Piece::WPAWN:
					generateStraight<false, true, true>(Piece::WPAWN, sq, Direction::UP_R, pin);
					break;
				case Piece::WLANCE:
					generateStraight<false, false, true>(Piece::WLANCE, sq, Direction::UP_R, pin);
					break;
				case Piece::WKNIGHT:
					generateStraight<false, true, true>(Piece::WKNIGHT, sq, Direction::LEFT_UP2_R, pin);
					generateStraight<false, true, true>(Piece::WKNIGHT, sq, Direction::RIGHT_UP2_R, pin);
					break;
				case Piece::WSILVER:
					generateStraight<false, true, true>(Piece::WSILVER, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, true>(Piece::WSILVER, sq, Direction::UP_R, pin);
					generateStraight<false, true, true>(Piece::WSILVER, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, true>(Piece::WSILVER, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, true, true>(Piece::WSILVER, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				case Piece::WGOLD:
					generateStraight<false, true, false>(Piece::WGOLD, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WGOLD, sq, Direction::UP_R, pin);
					generateStraight<false, true, false>(Piece::WGOLD, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WGOLD, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false>(Piece::WGOLD, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false>(Piece::WGOLD, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WBISHOP:
					generateStraight<false, false, true>(Piece::WBISHOP, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, false, true>(Piece::WBISHOP, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, false, true>(Piece::WBISHOP, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, false, true>(Piece::WBISHOP, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				case Piece::WROOK:
					generateStraight<false, false, true>(Piece::WROOK, sq, Direction::UP_R, pin);
					generateStraight<false, false, true>(Piece::WROOK, sq, Direction::DOWN_R, pin);
					generateStraight<false, false, true>(Piece::WROOK, sq, Direction::LEFT_R, pin);
					generateStraight<false, false, true>(Piece::WROOK, sq, Direction::RIGHT_R, pin);
					break;
				case Piece::WTOKIN:
					generateStraight<false, true, false>(Piece::WTOKIN, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WTOKIN, sq, Direction::UP_R, pin);
					generateStraight<false, true, false>(Piece::WTOKIN, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WTOKIN, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false>(Piece::WTOKIN, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false>(Piece::WTOKIN, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPLANCE:
					generateStraight<false, true, false>(Piece::WPLANCE, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WPLANCE, sq, Direction::UP_R, pin);
					generateStraight<false, true, false>(Piece::WPLANCE, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WPLANCE, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false>(Piece::WPLANCE, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false>(Piece::WPLANCE, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPKNIGHT:
					generateStraight<false, true, false>(Piece::WPKNIGHT, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WPKNIGHT, sq, Direction::UP_R, pin);
					generateStraight<false, true, false>(Piece::WPKNIGHT, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WPKNIGHT, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false>(Piece::WPKNIGHT, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false>(Piece::WPKNIGHT, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPSILVER:
					generateStraight<false, true, false>(Piece::WPSILVER, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WPSILVER, sq, Direction::UP_R, pin);
					generateStraight<false, true, false>(Piece::WPSILVER, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WPSILVER, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false>(Piece::WPSILVER, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false>(Piece::WPSILVER, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WHORSE:
					generateStraight<false, false, false>(Piece::WHORSE, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, false, false>(Piece::WHORSE, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, false, false>(Piece::WHORSE, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, false, false>(Piece::WHORSE, sq, Direction::RIGHT_DOWN_R, pin);
					generateStraight<false, true, false>(Piece::WHORSE, sq, Direction::UP_R, pin);
					generateStraight<false, true, false>(Piece::WHORSE, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false>(Piece::WHORSE, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false>(Piece::WHORSE, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WDRAGON:
					generateStraight<false, false, false>(Piece::WDRAGON, sq, Direction::UP_R, pin);
					generateStraight<false, false, false>(Piece::WDRAGON, sq, Direction::DOWN_R, pin);
					generateStraight<false, false, false>(Piece::WDRAGON, sq, Direction::LEFT_R, pin);
					generateStraight<false, false, false>(Piece::WDRAGON, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false>(Piece::WDRAGON, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WDRAGON, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false>(Piece::WDRAGON, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, true, false>(Piece::WDRAGON, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				}
			}
		}
	}

	template <bool black, bool oneStep, bool promotable>
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
			if (promotable && to.isPromotableRank<black>()) {
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

	template <bool black, bool check>
	void MoveGenerator::generateKing() {
		generateKingDirection<black, check>(Direction::LEFT_UP);
		generateKingDirection<black, check>(Direction::UP);
		generateKingDirection<black, check>(Direction::RIGHT_UP);
		generateKingDirection<black, check>(Direction::LEFT);
		generateKingDirection<black, check>(Direction::RIGHT);
		generateKingDirection<black, check>(Direction::LEFT_DOWN);
		generateKingDirection<black, check>(Direction::DOWN);
		generateKingDirection<black, check>(Direction::RIGHT_DOWN);
	}

	template <bool black, bool check>
	void MoveGenerator::generateKingDirection(const Direction& dir) {
		Square from = (black ? pos.getBKing() : pos.getWKing());
		Square to = from + dir;
		Piece piece = pos.getBoard(to);
		DirectionFlags effectTo = pos.getEffect(to, !black);
		DirectionFlags effectFrom = pos.getEffect(from, !black);
		if ((black && !piece.isBlackMovable()) || (!black && !piece.isWhiteMovable())
				|| effectTo.isNonZero() || (check && effectFrom.getLongRangeOnly().check(dir))) {
			return;
		}
		moves[num++] = Move(from, to, false, false, (black ? Piece::BKING : Piece::WKING));
	}

	template <bool black>
	void MoveGenerator::generateDrop() {
		generateDropPieces<black, (black ? Piece::BPAWN : Piece::WPAWN)>();
		generateDropPieces<black, (black ? Piece::BLANCE : Piece::WLANCE)>();
		generateDropPieces<black, (black ? Piece::BKNIGHT : Piece::WKNIGHT)>();
		generateDropPieces<black, (black ? Piece::BSILVER : Piece::WSILVER)>();
		generateDropPieces<black, (black ? Piece::BGOLD : Piece::WGOLD)>();
		generateDropPieces<black, (black ? Piece::BBISHOP : Piece::WBISHOP)>();
		generateDropPieces<black, (black ? Piece::BROOK : Piece::WROOK)>();
	}

	template <bool black, unsigned piece>
	void MoveGenerator::generateDropPieces() {
		assert(black != (piece & Piece::TURN));
		if ((black ? pos.getBlackHand(piece) : pos.getWhiteHand(piece)) != 0) {
			for (unsigned file = 1; file <= Square::FILE_NUM; file++) {
				if (black && piece == Piece::BPAWN && pos.getBPawnFiles().exist(file)) {
					continue;
				}
				if (!black && piece == Piece::WPAWN && pos.getWPawnFiles().exist(file)) {
					continue;
				}
				unsigned rank_top = 1;
				unsigned rank_bottom = Square::RANK_NUM;
				if (piece == Piece::BPAWN || piece == Piece::BLANCE) {
					rank_top = 2;
				} else if (piece == Piece::BKNIGHT) {
					rank_top = 3;
				} else if (piece == Piece::WPAWN || piece == Piece::WLANCE) {
					rank_bottom = Square::RANK_NUM - 1;
				} else if (piece == Piece::WKNIGHT) {
					rank_bottom = Square::RANK_NUM - 2;
				}
				for (unsigned rank = rank_top; rank <= rank_bottom; rank++) {
					Square sq(file, rank);
					if (pos.getBoard(sq).isEmpty()) {
						// TODO:
						/*
						if (black && piece == Piece::BPAWN &&) {
							continue;
						} else if (!black && piece == Piece::WPAWN &&) {
							continue;
						}
						*/
						moves[num++] = Move(Square::NON, sq, false, true, piece);
					}
				}
			}
		}
	}

	template <bool black>
	void MoveGenerator::generateEvasion() {
		// TODO:
	}
}
