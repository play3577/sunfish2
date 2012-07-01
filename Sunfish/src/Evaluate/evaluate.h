/*
 * evaluate.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef EVALUATE_H_
#define EVALUATE_H_

#include "param.h"

namespace Evaluate {
	class Evaluate {
	private:
		const Param& param;
		Value value;

	public:
		Evaluate(const Param& param, Value value = Value(0)) :
				param(param), value(value) {
		}

		Evaluate(const Param& param, const Shogi::Position& pos) :
				param(param) {
			init(pos);
		}

		void init(const Shogi::Position& pos);

		void setValue(Value value) {
			this->value = value;
		}

		Value getValue() const {
			return value;
		}
	};
}

#endif // EVALUATE_H_
