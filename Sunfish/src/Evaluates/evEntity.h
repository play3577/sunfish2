/*
 * evEntity.h
 *
 *  Created on: 2012/07/09
 *      Author: ryosuke
 */

#ifndef EVENTITY_H_
#define EVENTITY_H_

#include "param.h"

namespace Evaluates {
	class EvEntity {
	private:
		Util::uint64 hash;
		Value value;

	public:
		EvEntity() {
			init();
		}

		void init() {
			hash = U64(0);
		}

		bool get(Util::uint64 hash, Value& value) const {
			if (this->hash == hash) {
				value = this->value;
				return true;
			}
			return false;
		}

		void set(Util::uint64 hash, const Value& value) {
			this->hash = hash;
			this->value = value;
		}
	};
}

#endif // EVENTITY_H_
