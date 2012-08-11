/*
 * feature.h
 *
 *  Created on: 2012/07/07
 *      Author: ryosuke
 */

#ifndef FEATURE_H_
#define FEATURE_H_

#include "estimate.h"
#include "../Shogi/position.h"

namespace Evaluate {
	class Feature {
	private:
		static const int KING_ERROR  = 100 * 16;
		static const int PIECE_ERROR = 100 *  8;

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
			return value;
		}

		template<class X, class T, class U>
		static Estimate<X> estimate(const Shogi::Position& pos,
				const TempParam<T, U>* pparam,
				const Shogi::Move& move) {
			if (move.getPiece().isKing()) {
				return Estimate<X>(X(0), X(KING_ERROR));
			} else {
				Kings kings(pos);
				X value(0);
				value -= pparam->getKKP(kings,
					move.getPiece(), move.getFrom());
				Shogi::Piece piece = move.isPromotion() ?
					move.getPiece().getPromoted() : move.getPiece();
				value += pparam->getKKP(kings, piece, move.getTo());
				return Estimate<X>(value, X(PIECE_ERROR));
			}
		}
	};
}

#endif // FEATURE_H_
