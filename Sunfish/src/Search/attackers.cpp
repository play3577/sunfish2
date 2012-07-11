/*
 * attackers.cpp
 *
 *  Created on: 2012/07/10
 *      Author: ryosuke
 */

#include "attackers.h"

namespace Search {
	using namespace Shogi;
	using namespace Evaluate;

	template <bool black>
	Value Attackers::see(int b, int w, Value v) const {
		if (black) {
			if (b < blackNum) {
				Value value = v + see<false>(b+1, w, blackAttackers[b]);
				if (value > Value(0)) {
					return value;
				}
			}
		} else {
			if (w < whiteNum) {
				Value value = v + see<true>(b, w+1, whiteAttackers[w]);
				if (value < Value(0)) {
					return value;
				}
			}
		}
		return Value(0);
	}
	template Value Attackers::see<true>(int b, int w, Value v) const;
	template Value Attackers::see<false>(int b, int w, Value v) const;

	void Attackers::sort(Value attackers[], int num) {
		for (int i = 1; i < num; i++) {
			Value attacker = attackers[i];
			int j;
			for (j = i; j > 0; j--) {
				if (attackers[j-1] <= attacker) {
					break;
				}
				attackers[j] = attackers[j-1];
			}
			attackers[j] = attacker;
		}
	}

	template <bool black>
	void Attackers::create(const Position& pos, Move move) {
		if (black) {
			blackNum = 0;
		} else {
			whiteNum = 0;
		}

		DirectionFlags flags = pos.getEffect(move.getTo(), black).getExcludeKing();
		while (flags.isNonZero()) {
			Direction dir = flags.pop().toDirection().reverse();
			Square from = move.getTo();
			for (from += dir; pos.getBoard(from) == Piece::EMPTY; from += dir)
				;
			if (from == move.getFrom()) {
				continue; // TODO
			}
			Piece piece = pos.getBoard(from);
			if (black) {
				blackAttackers[blackNum] = (piece != Piece::BKING ?
						(Value)param.getPieceExchange(piece) :
						(Value)Value::MAX);
				blackNum++;
			} else {
				whiteAttackers[whiteNum] = (piece != Piece::WKING ?
						(Value)param.getPieceExchange(piece) :
						(Value)Value::MAX);
				whiteNum++;
			}
		}
		if (black) {
			sort(blackAttackers, blackNum);
		} else {
			sort(whiteAttackers, whiteNum);
		}
	}
	template void Attackers::create<true>(const Position& pos, Move move);
	template void Attackers::create<false>(const Position& pos, Move move);
}
