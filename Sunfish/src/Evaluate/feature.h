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
		template<class T, class U, bool get, bool cum>
		static void Extract(const Shogi::Position& pos,
				const TempParam<T, U>* iparam, const T* ivalue,
				TempParam<T, U>* oparam, T* ovalue);

	public:
		template<class T, class U>
		static T getValue(const Shogi::Position& pos,
				const TempParam<T, U>* pparam) {
			T value(0);
			Extract<T, U, true, false>(pos, pparam, NULL, NULL, &value);
			return value;
		}
	};
}

#endif // FEATURE_H_
