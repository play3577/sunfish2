/*
 * shekEntity.h
 *
 *  Created on: 2012/11/03
 *      Author: ryosuke
 */

#ifndef SHEKENTITY_H_
#define SHEKENTITY_H_

#include "handSet.h"

namespace Shek {
	class ShekEntity {
	private:
		HandSet handSet;
		bool blackTurn;
		unsigned cnt;

	public:
		void init() {
			cnt = 0;
		}

		ShekStat check(const HandSet& handSet, bool blackTurn) const {
			if (cnt == 0) {
				return NONE;
			} else {
				ShekStat stat = handSet.compareTo(this->handSet, blackTurn);
				if (this->blackTurn != blackTurn) {
					if (stat == EQUAL) {
						stat = SUPERIOR;
					} else if (stat == INFERIOR) {
						stat = NONE;
					}
				}
				return stat;
			}
		}

		void set(const HandSet& handSet, bool blackTurn) {
			if (cnt == 0) {
				this->handSet = handSet;
				this->blackTurn = blackTurn;
			}
			cnt++;
		}

		void unset() {
			assert(cnt > 0);
			cnt--;
		}

		unsigned getCount() const {
			return cnt;
		}

		void debugPrint(const HandSet&) const {
			Log::debug << "ShekEntity::cnt=[" << cnt << "]\n";
		}
	};
}
#endif // SHEKENTITY_H_
