/*
 * moveGenerator.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include <cassert>
#include <iostream>
#include "../Tools/debug.h"
#include "square.h"
#include "moveGenerator.h"

namespace Shogi {
	template <bool genCap, bool genNocap>
	unsigned MoveGenerator::generate() {
		if (pos.isCheck()) {
			if (pos.isBlackTurn()) {
				generateEvasion<true, genCap, genNocap>();
				generateKing<true, true, genCap, genNocap>();
			} else {
				generateEvasion<false, genCap, genNocap>();
				generateKing<false, true, genCap, genNocap>();
			}
		} else {
			if (pos.isBlackTurn()) {
				generateOnBoard<true, genCap, genNocap, false>();
				generateKing<true, false, genCap, genNocap>();
				if (genNocap) { generateDrop<true>(); }
			} else {
				generateOnBoard<false, genCap, genNocap, false>();
				generateKing<false, false, genCap, genNocap>();
				if (genNocap) { generateDrop<false>(); }
			}
		}
		return num;
	}
	template unsigned MoveGenerator::generate<true, true>();
	template unsigned MoveGenerator::generate<true, false>();
	template unsigned MoveGenerator::generate<false, true>();

	unsigned MoveGenerator::generateTactical() {
		if (pos.isCheck()) {
			if (pos.isBlackTurn()) {
				generateEvasion<true, true, true>();
				generateKing<true, true, true, true>();
			} else {
				generateEvasion<false, true, true>();
				generateKing<false, true, true, true>();
			}
		} else {
			if (pos.isBlackTurn()) {
				generateOnBoard<true, true, false, true>();
				generateKing<true, false, true, false>();
			} else {
				generateOnBoard<false, true, false, true>();
				generateKing<false, false, true, false>();
			}
		}
		return num;
	}

	template <bool black, bool genCap, bool genNocap, bool genPro>
	void MoveGenerator::generateOnBoard() {
		for (Square sq = Square::TOP; sq.valid(); sq.next()) {
			const Piece& piece = pos.getBoard(sq);
			const Direction pin = pos.pin(sq, black).toDirection();
			if (black && piece.isBlack()) {
				switch (piece.getInteger()) {
				case Piece::BPAWN:
					generateStraight<true, true, true, genCap, genNocap, genPro>(Piece::BPAWN, sq, Direction::UP, pin);
					break;
				case Piece::BLANCE:
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BLANCE, sq, Direction::UP, pin);
					break;
				case Piece::BKNIGHT:
					generateStraight<true, true, true, genCap, genNocap, genPro>(Piece::BKNIGHT, sq, Direction::LEFT_UP2, pin);
					generateStraight<true, true, true, genCap, genNocap, genPro>(Piece::BKNIGHT, sq, Direction::RIGHT_UP2, pin);
					break;
				case Piece::BSILVER:
					generateStraight<true, true, true, genCap, genNocap, genPro>(Piece::BSILVER, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, true, genCap, genNocap, genPro>(Piece::BSILVER, sq, Direction::UP, pin);
					generateStraight<true, true, true, genCap, genNocap, genPro>(Piece::BSILVER, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, true, genCap, genNocap, genPro>(Piece::BSILVER, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, true, true, genCap, genNocap, genPro>(Piece::BSILVER, sq, Direction::RIGHT_DOWN, pin);
					break;
				case Piece::BGOLD:
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BGOLD, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BGOLD, sq, Direction::UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BGOLD, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BGOLD, sq, Direction::LEFT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BGOLD, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BGOLD, sq, Direction::DOWN, pin);
					break;
				case Piece::BBISHOP:
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BBISHOP, sq, Direction::LEFT_UP, pin);
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BBISHOP, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BBISHOP, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BBISHOP, sq, Direction::RIGHT_DOWN, pin);
					break;
				case Piece::BROOK:
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BROOK, sq, Direction::UP, pin);
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BROOK, sq, Direction::DOWN, pin);
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BROOK, sq, Direction::LEFT, pin);
					generateStraight<true, false, true, genCap, genNocap, genPro>(Piece::BROOK, sq, Direction::RIGHT, pin);
					break;
				case Piece::BTOKIN:
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BTOKIN, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BTOKIN, sq, Direction::UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BTOKIN, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BTOKIN, sq, Direction::LEFT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BTOKIN, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BTOKIN, sq, Direction::DOWN, pin);
					break;
				case Piece::BPLANCE:
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPLANCE, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPLANCE, sq, Direction::UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPLANCE, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPLANCE, sq, Direction::LEFT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPLANCE, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPLANCE, sq, Direction::DOWN, pin);
					break;
				case Piece::BPKNIGHT:
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPKNIGHT, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPKNIGHT, sq, Direction::UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPKNIGHT, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPKNIGHT, sq, Direction::LEFT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPKNIGHT, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPKNIGHT, sq, Direction::DOWN, pin);
					break;
				case Piece::BPSILVER:
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPSILVER, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPSILVER, sq, Direction::UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPSILVER, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPSILVER, sq, Direction::LEFT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPSILVER, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BPSILVER, sq, Direction::DOWN, pin);
					break;
				case Piece::BHORSE:
					generateStraight<true, false, false, genCap, genNocap, genPro>(Piece::BHORSE, sq, Direction::LEFT_UP, pin);
					generateStraight<true, false, false, genCap, genNocap, genPro>(Piece::BHORSE, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, false, false, genCap, genNocap, genPro>(Piece::BHORSE, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, false, false, genCap, genNocap, genPro>(Piece::BHORSE, sq, Direction::RIGHT_DOWN, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BHORSE, sq, Direction::UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BHORSE, sq, Direction::LEFT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BHORSE, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BHORSE, sq, Direction::DOWN, pin);
					break;
				case Piece::BDRAGON:
					generateStraight<true, false, false, genCap, genNocap, genPro>(Piece::BDRAGON, sq, Direction::UP, pin);
					generateStraight<true, false, false, genCap, genNocap, genPro>(Piece::BDRAGON, sq, Direction::DOWN, pin);
					generateStraight<true, false, false, genCap, genNocap, genPro>(Piece::BDRAGON, sq, Direction::LEFT, pin);
					generateStraight<true, false, false, genCap, genNocap, genPro>(Piece::BDRAGON, sq, Direction::RIGHT, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BDRAGON, sq, Direction::LEFT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BDRAGON, sq, Direction::RIGHT_UP, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BDRAGON, sq, Direction::LEFT_DOWN, pin);
					generateStraight<true, true, false, genCap, genNocap, genPro>(Piece::BDRAGON, sq, Direction::RIGHT_DOWN, pin);
					break;
				}
			} else if (!black && piece.isWhite()) {
				switch (piece.getInteger()) {
				case Piece::WPAWN:
					generateStraight<false, true, true, genCap, genNocap, genPro>(Piece::WPAWN, sq, Direction::UP_R, pin);
					break;
				case Piece::WLANCE:
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WLANCE, sq, Direction::UP_R, pin);
					break;
				case Piece::WKNIGHT:
					generateStraight<false, true, true, genCap, genNocap, genPro>(Piece::WKNIGHT, sq, Direction::LEFT_UP2_R, pin);
					generateStraight<false, true, true, genCap, genNocap, genPro>(Piece::WKNIGHT, sq, Direction::RIGHT_UP2_R, pin);
					break;
				case Piece::WSILVER:
					generateStraight<false, true, true, genCap, genNocap, genPro>(Piece::WSILVER, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, true, genCap, genNocap, genPro>(Piece::WSILVER, sq, Direction::UP_R, pin);
					generateStraight<false, true, true, genCap, genNocap, genPro>(Piece::WSILVER, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, true, genCap, genNocap, genPro>(Piece::WSILVER, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, true, true, genCap, genNocap, genPro>(Piece::WSILVER, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				case Piece::WGOLD:
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WGOLD, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WGOLD, sq, Direction::UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WGOLD, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WGOLD, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WGOLD, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WGOLD, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WBISHOP:
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WBISHOP, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WBISHOP, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WBISHOP, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WBISHOP, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				case Piece::WROOK:
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WROOK, sq, Direction::UP_R, pin);
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WROOK, sq, Direction::DOWN_R, pin);
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WROOK, sq, Direction::LEFT_R, pin);
					generateStraight<false, false, true, genCap, genNocap, genPro>(Piece::WROOK, sq, Direction::RIGHT_R, pin);
					break;
				case Piece::WTOKIN:
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WTOKIN, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WTOKIN, sq, Direction::UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WTOKIN, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WTOKIN, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WTOKIN, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WTOKIN, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPLANCE:
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPLANCE, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPLANCE, sq, Direction::UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPLANCE, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPLANCE, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPLANCE, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPLANCE, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPKNIGHT:
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPKNIGHT, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPKNIGHT, sq, Direction::UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPKNIGHT, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPKNIGHT, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPKNIGHT, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPKNIGHT, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WPSILVER:
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPSILVER, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPSILVER, sq, Direction::UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPSILVER, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPSILVER, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPSILVER, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WPSILVER, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WHORSE:
					generateStraight<false, false, false, genCap, genNocap, genPro>(Piece::WHORSE, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, false, false, genCap, genNocap, genPro>(Piece::WHORSE, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, false, false, genCap, genNocap, genPro>(Piece::WHORSE, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, false, false, genCap, genNocap, genPro>(Piece::WHORSE, sq, Direction::RIGHT_DOWN_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WHORSE, sq, Direction::UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WHORSE, sq, Direction::LEFT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WHORSE, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WHORSE, sq, Direction::DOWN_R, pin);
					break;
				case Piece::WDRAGON:
					generateStraight<false, false, false, genCap, genNocap, genPro>(Piece::WDRAGON, sq, Direction::UP_R, pin);
					generateStraight<false, false, false, genCap, genNocap, genPro>(Piece::WDRAGON, sq, Direction::DOWN_R, pin);
					generateStraight<false, false, false, genCap, genNocap, genPro>(Piece::WDRAGON, sq, Direction::LEFT_R, pin);
					generateStraight<false, false, false, genCap, genNocap, genPro>(Piece::WDRAGON, sq, Direction::RIGHT_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WDRAGON, sq, Direction::LEFT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WDRAGON, sq, Direction::RIGHT_UP_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WDRAGON, sq, Direction::LEFT_DOWN_R, pin);
					generateStraight<false, true, false, genCap, genNocap, genPro>(Piece::WDRAGON, sq, Direction::RIGHT_DOWN_R, pin);
					break;
				}
			}
		}
	}

	template <bool black, bool oneStep, bool promotable, bool genCap, bool genNocap, bool genPro>
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
			if ((genCap && !piece2.isEmpty()) || (genNocap && piece2.isEmpty())) {
				if (promotable) {
					generateMoveOneMovePro<black>(piece, from, to);
				}
				generateMoveOneMove<black>(piece, from, to);
			} else if (promotable && genPro) {
				generateMoveOneMovePro<black>(piece, from, to);
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

	template <bool black, bool check, bool genCap, bool genNocap>
	void MoveGenerator::generateKing() {
		if (black ? pos.getBKing().valid() : pos.getWKing().valid()) {
			generateKingDirection<black, check, genCap, genNocap>(Direction::LEFT_UP);
			generateKingDirection<black, check, genCap, genNocap>(Direction::UP);
			generateKingDirection<black, check, genCap, genNocap>(Direction::RIGHT_UP);
			generateKingDirection<black, check, genCap, genNocap>(Direction::LEFT);
			generateKingDirection<black, check, genCap, genNocap>(Direction::RIGHT);
			generateKingDirection<black, check, genCap, genNocap>(Direction::LEFT_DOWN);
			generateKingDirection<black, check, genCap, genNocap>(Direction::DOWN);
			generateKingDirection<black, check, genCap, genNocap>(Direction::RIGHT_DOWN);
		}
	}

	template <bool black, bool check, bool genCap, bool genNocap>
	void MoveGenerator::generateKingDirection(const Direction& dir) {
		Square from = (black ? pos.getBKing() : pos.getWKing());
		Square to = from + dir;
		Piece piece = pos.getBoard(to);
		if (!genCap && !piece.isEmpty()) { // 駒を取る手
			return;
		}
		if (!genNocap && piece.isEmpty()) { // 駒を取らない手
			return;
		}
		DirectionFlags effectTo = pos.getEffect(to, !black);
		DirectionFlags effectFrom = pos.getEffect(from, !black);
		if ((black && !piece.isBlackMovable()) || (!black && !piece.isWhiteMovable())
				|| effectTo.longOrShortRange() || (check && effectFrom.getLongRangeOnly().check(dir, true))) {
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
						if (black && piece == Piece::BPAWN && pos.isPawnDropMate(sq, true)) {
							continue;
						} else if (!black && piece == Piece::WPAWN && pos.isPawnDropMate(sq, false)) {
							continue;
						}
						moves[num++] = Move(Square::NON, sq, false, true, piece);
					}
				}
			}
		}
	}

	template <bool black, bool genCap, bool genNocap>
	void MoveGenerator::generateEvasion() {
		DirectionFlags flags = pos.getCheckDirection();
		if (!flags.isPlural()) { // 両王手でない場合
			Direction dir = flags.toDirection().reverse();
			Square sq = (black ? pos.getBKing() : pos.getWKing());
			for (sq += dir; pos.getBoard(sq) == Piece::EMPTY; sq += dir) {
				if (genNocap) { // 駒を取らない手
					generateEvasionOnBoard<black>(sq);
					generateEvasionDrop<black, (black ? Piece::BPAWN : Piece::WPAWN)>(sq);
					generateEvasionDrop<black, (black ? Piece::BLANCE : Piece::WLANCE)>(sq);
					generateEvasionDrop<black, (black ? Piece::BKNIGHT : Piece::WKNIGHT)>(sq);
					generateEvasionDrop<black, (black ? Piece::BSILVER : Piece::WSILVER)>(sq);
					generateEvasionDrop<black, (black ? Piece::BGOLD : Piece::WGOLD)>(sq);
					generateEvasionDrop<black, (black ? Piece::BBISHOP : Piece::WBISHOP)>(sq);
					generateEvasionDrop<black, (black ? Piece::BROOK : Piece::WROOK)>(sq);
				}
			}
			if (genCap) { // 駒を取る手
				generateEvasionOnBoard<black>(sq);
			}
		}
	}

	template <bool black>
	void MoveGenerator::generateEvasionOnBoard(Square to) {
		DirectionFlags flags = pos.getEffect(to, black).getExcludeKing();
		while (flags.isNonZero()) {
			Direction dir = flags.pop().toDirection().reverse();
			Square from = to;
			for (from += dir; pos.getBoard(from) == Piece::EMPTY; from += dir)
				;
			Piece piece = pos.getBoard(from);
			if (!piece.isKing<black>() && pos.pin(from, black).isZero()) {
				if (piece.isPromotable()) {
					generateMoveOneMovePro<black>(piece, from, to);
				}
				generateMoveOneMove<black>(piece, from, to);
			}
		}
	}

	template <bool black, unsigned piece>
	void MoveGenerator::generateEvasionDrop(Square to) {
		assert(black != (piece & Piece::TURN));
		if ((black ? pos.getBlackHand(piece) : pos.getWhiteHand(piece)) != 0) {
			if (black && piece == Piece::BPAWN && pos.getBPawnFiles().exist(to.getFile())) {
				return;
			}
			if (!black && piece == Piece::WPAWN && pos.getWPawnFiles().exist(to.getFile())) {
				return;
			}
			if (to.isCompulsoryPromotion(piece)) {
				return;
			}
			if (black && piece == Piece::BPAWN && pos.isPawnDropMate(to, true)) {
				return;
			} else if (!black && piece == Piece::WPAWN && pos.isPawnDropMate(to, false)) {
				return;
			}
			moves[num++] = Move(Square::NON, to, false, true, piece);
		}
	}

	unsigned MoveGenerator::generateTardy() {
		for (Square from = Square::TOP; from.valid(); from.next()) {
			Piece piece = pos.getBoard(from);
			if (!piece.isEmpty() && piece.isBlack() == pos.isBlackTurn()) {
				for (Square to = Square::TOP; to.valid(); to.next()) {
					Move move(from, to, false, false, piece);
					if (pos.isLegalMove(move)) {
						moves[num++] = move;
					}
					move.setPromotion(true);
					if (pos.isLegalMove(move)) {
						moves[num++] = move;
					}
				}
			}
		}
		for (Piece piece = Piece::PAWN; piece != Piece::KING; piece.toNext()) {
			if (pos.getHand(piece) != 0) {
				Piece drop = pos.isBlackTurn() ? piece : piece.getTurnedWhite();
				for (Square to = Square::TOP; to.valid(); to.next()) {
					Move move(Square::NON, to, false, true, drop);
					if (pos.isLegalMove(move)) {
						moves[num++] = move;
					}
				}
			}
		}
		return num;
	}

	void MoveGenerator::sort(unsigned begin, unsigned size) {
		// TODO: use boost
//		boost::sort(moves);
		for (unsigned i = begin + 1; (unsigned)i < begin + size; i++) {
			Move move = moves[i];
			unsigned j;
			for (j = i - 1; j >= begin; j--) {
				if (move < moves[j]) {
					break;
				}
				moves[j+1] = moves[j];
			}
			moves[j+1] = move;
		}
	}

	bool MoveGenerator::equals(const MoveGenerator gen) const {
		if (num != gen.num) {
			return false;
		}
		for (unsigned i = 0; i < num; i++) {
			if (moves[i] != gen.moves[i]) {
				return false;
			}
		}
		return true;
	}
}
