/*
 * feature.cpp
 *
 *  Created on: 2012/07/07
 *      Author: ryosuke
 */

#include "feature.h"

namespace Evaluate {
	template<class T, class U, bool get, bool cum>
	void Feature::Extract(const Shogi::Position& pos,
			const TempParam<T, U>* iparam, const T* ivalue,
			TempParam<T, U>* oparam, T* ovalue) {
	}
	template void Feature::Extract<Value, ValueS, true, false>
			(const Shogi::Position& pos,
			const Param* iparam, const Value* ivalue,
			Param* oparam, Value* ovalue);
}
