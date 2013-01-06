/*
 * killer.h
 *
 *  Created on: 2013/01/06
 *      Author: ryosuke
 */

#ifndef KILLER_H_
#define KILLER_H_

#include "../Shogi/move.h"

namespace Search {
	class Killer {
	private:
		Shogi::Move killer1;
		Shogi::Move killer2;
		Evaluates::Value value1;
		Evaluates::Value value2;

	public:
		void init() {
			killer1.setEmpty();
			killer2.setEmpty();
		}

		void add(const Shogi::Move& move, const Evaluates::Value& value) {
			if (value > value1) {
				killer2 = killer1;
				value2 = value1;
				killer1 = move;
				value1 = value;
			} else if (value > value2) {
				killer2 = move;
				value2 = value;
			}
		}
	};
}

#endif // KILLER_H_
