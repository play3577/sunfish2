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
		unsigned cnt;

	public:
		void init() {
			cnt = 0;
		}

		ShekStat check(const HandSet& handSet) const {
			if (cnt == 0) {
				return NONE;
			} else {
				return this->handSet.compareTo(handSet);
			}
		}

		void set(const HandSet& handSet) {
			if (cnt == 0) {
				this->handSet = handSet;
			}
			cnt++;
		}

		void unset() {
			assert(cnt > 0);
			cnt--;
		}

		void debugPrint(const HandSet&) const {
			Log::debug << "ShekEntity::cnt=[" << cnt << "]\n";
		}
	};
}
#endif // SHEKENTITY_H_
