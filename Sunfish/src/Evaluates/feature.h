/*
 * feature.h
 *
 *  Created on: 2012/07/07
 *      Author: ryosuke
 */

#ifndef FEATURE_H_
#define FEATURE_H_

#include <cassert>
#include "estimate.h"
#include "../Shogi/position.h"

namespace Evaluates {
	static const int KING_ERROR  = 100 * 24;
	static const int PIECE_ERROR = 100 *  8;

	class Feature {
	private:
		template<bool get, bool cum>
		static Value extract(const Shogi::Position& pos,
				const Param* pp, Gradient* pg, double inc);

		Feature();

	public:
		static Value getValue(const Shogi::Position& pos,
				const Param* pparam) {
			return Value(extract<true, false>(pos, pparam, NULL, 0.0) / Param::SCALE);
		}

		static void incValue(const Shogi::Position& pos,
				Gradient* pg, double d) {
			extract<false, true>(pos, NULL, pg, d);
		}

		static Estimate estimate(const Shogi::Position& pos,
				const Param* pparam, const Shogi::Move& move) {
			Value value0(0);
			Value value1(0);
			Value error(0);
			Shogi::Piece cap = pos.getBoard(move.getTo());
			if (!cap.isEmpty()) {
				value0 -= pparam->getPieceExchange(cap);
			}
			if (move.getPiece().isKing()) {
				error = KING_ERROR;
			} else {
				Kings kings(pos);
				if (!move.isHand()) {
					value1 -= pparam->getKKP(kings,
						move.getPiece(), move.getFrom());
				}
				Shogi::Piece piece = move.isPromotion() ?
					move.getPiece().getPromoted() : move.getPiece();
				value1 += pparam->getKKP(kings, piece, move.getTo());
				error = PIECE_ERROR;
			}
			return Estimate(value0 + value1 / Param::SCALE, error);
		}
	};
}

#endif // FEATURE_H_
