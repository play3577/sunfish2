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

		SquareDiff(const Square from, const Square to) {
			diff = (unsigned)to-(unsigned)from;
		}

		operator int() {
			return diff;
		}

		Direction toDirection() const {
			return diff2dir[diff];
		}

		DirectionAndRange toDirectionAndRange() const {
			int dir = diff2dir[diff];
			return DirectionAndRange(dir, diff != dir);
		}
	};
}

#endif /* SQUAREDIFF_H_ */
