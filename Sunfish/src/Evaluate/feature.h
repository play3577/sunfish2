/*
 * feature.h
 *
 *  Created on: 2012/07/07
 *      Author: ryosuke
 */

#ifndef FEATURE_H_
#define FEATURE_H_

#include "param.h"
#include "../Shogi/position.h"

namespace Evaluate {
	class Feature {
	private:
		static const int blackPiece[];
		static const int whitePiece[];

		template<class X, class T, class U, bool get, bool cum>
		static void Extract(const Shogi::Position& pos,
				const TempParam<T, U>* iparam, const X* ivalue,
				TempParam<T, U>* oparam, X* ovalue);

	public:
		template<class R, class T, class U>
		static R getValue(const Shogi::Position& pos,
				const TempParam<T, U>* pparam) {
			R value(0);
			Extract<R, T, U, true, false>(pos, pparam, NULL, NULL, &value);
			return value;
		}
	};
}

#endif // FEATURE_H_
