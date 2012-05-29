/*
 * squareDiff.h
 *
 *  Created on: 2012/05/30
 *      Author: ryosuke
 */

#ifndef SQUAREDIFF_H_
#define SQUAREDIFF_H_

#include "square.h"

namespace Shogi {
	class SquareDiff {
	private:
		int diff;
		static const int _diff2dir[];
		static const int* diff2dir;

	public:
		SquareDiff(int diff = 0) {
			this->diff = diff;
		}

		SquareDiff(const Square to, const Square from) {
			diff = diff2dir[(unsigned)to-(unsigned)from];
		}

		operator int() {
			return diff;
		}
	};
}

#endif /* SQUAREDIFF_H_ */
