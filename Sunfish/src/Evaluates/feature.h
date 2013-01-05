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
		template<class X, class T, class U, bool get, bool cum>
		static void extract(const Shogi::Position& pos,
				const TempParam<T, U>* iparam, const X* ivalue,
				TempParam<T, U>* oparam, X* ovalue);

		Feature();

	public:
		template<class X, class T, class U>
		static X getValue(const Shogi::Position& pos,
				const TempParam<T, U>* pparam) {
			X value(0);
			extract<X, T, U, true, false>(pos, pparam, NULL, NULL, &value);
			return value / Param::SCALE;
		}

		template<class X, class T, class U>
		static Estimate<X> estimate(const Shogi::Position& pos,
				const TempParam<T, U>* pparam,
				const Shogi::Move& move) {
			X value0(0);
			X value1(0);
			X error(0);
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
			return Estimate<X>(value0 + value1 / Param::SCALE, error);
		}
	};
}

#endif // FEATURE_H_
