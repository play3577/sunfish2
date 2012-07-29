/*
 * attackers.h
 *
 *  Created on: 2012/07/10
 *      Author: ryosuke
 */

#ifndef ATTACKERS_H_
#define ATTACKERS_H_

#include "../Shogi/position.h"
#include "../Evaluate/param.h"

namespace Search {
	class Attackers {
	private:
		static const int SIZE = 32;
		const Evaluate::Param& param;
		int blackNum;
		Evaluate::Value blackAttackers[SIZE];
		int whiteNum;
		Evaluate::Value whiteAttackers[SIZE];
		bool blackTurn;
		Evaluate::Value firstValue;
		Evaluate::Value defaultValue;

		template <bool black>
		void create(const Shogi::Position& pos, Shogi::Move move);

		void sort(Evaluate::Value attackers[], int num);

		template <bool black>
		Evaluate::Value see(int b, int w, Evaluate::Value v) const;

	public:
		Attackers(const Evaluate::Param& param) :
				param(param) {
		}

		Attackers(const Evaluate::Param& param,
				const Shogi::Position& pos, Shogi::Move move) :
				param(param) {
			create(pos, move);
		}

		void create(const Shogi::Position& pos, Shogi::Move move) {
			blackTurn = pos.isBlackTurn();
			firstValue = param.getPieceExchangeAbs(move.getPiece());
			Shogi::Piece piece = pos.getBoard(move.getTo());
			if (piece.isEmpty()) {
				defaultValue = Evaluate::Value(0);
			} else {
				defaultValue = param.getPieceExchangeAbs(piece);
			}
			create<true>(pos, move);
			create<false>(pos, move);
		}

		Evaluate::Value see() const {
			if (blackTurn) {
				return defaultValue - see<false>(0, 0, firstValue);
			} else {
				return defaultValue - see<true>(0, 0, firstValue);
			}
		}
	};
}

#endif // ATTACKERS_H_