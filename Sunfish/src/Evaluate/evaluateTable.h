/*
 * evaluateTable.h
 *
 *  Created on: 2012/07/09
 *      Author: ryosuke
 */

#include "evEntity.h"

namespace Evaluate {
	class EvaluateTable {
	private:
		static const unsigned DEF_BITS = 20;
		unsigned size;
		unsigned mask;
		EvEntity* table;

	public:
		EvaluateTable(unsigned bits = DEF_BITS) :
				size(0), table(NULL) {
			init(bits);
		}

		virtual ~EvaluateTable() {
			delete table;
		}

		void init(unsigned bits = 0) {
			unsigned newSize = 1 << bits;
			if (bits != 0 && size != newSize) {
				size = newSize;
				mask = size - 1;
				if (table != NULL) { delete[] table; }
				table = new EvEntity[size];
			} else {
				for (unsigned i = 0; i < size; i++) {
					table[i].init();
				}
			}
		}

		bool get(Util::uint64 hash, Value& value) const {
			return table[hash&mask].get(hash, value);
		}

		void set(Util::uint64 hash, const Value& value) {
			table[hash&mask].set(hash, value);
		}
	};
}
