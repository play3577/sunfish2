/*
 * param.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef PARAM_H_
#define PARAM_H_

#include "../Shogi/position.h"
#include "value.h"

namespace Evaluate {
	template <class T, class U>
	class TempParam {
	private:
		T piece[Shogi::Piece::DRAGON+1];

	public:
		TempParam() {
			init();
		}

		virtual ~TempParam() {
		}

		void init() {
			memset(this, 0, sizeof(*this));
		}

		void setPiece(const Shogi::Piece& p, T value) {
			piece[p.getTurnedBlack()] = value;
		}

		T getPiece(const Shogi::Piece& p) const {
			return piece[p.getTurnedBlack()];
		}
	};

	typedef TempParam<Value, ValueS> Param;
	typedef TempParam<ValueF, ValueF> Gradient;
};

#endif // PARAM_H_
