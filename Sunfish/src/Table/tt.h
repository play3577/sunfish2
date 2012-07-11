/*
 * tt.h
 *
 *  Created on: 2012/07/05
 *      Author: ryosuke
 */

#ifndef TT_H_
#define TT_H_

#include "ttEntity.h"

namespace Table {
	class TT {
	private:
		static const unsigned DEF_BITS = 20;
		unsigned size;
		unsigned mask;
		TTEntity* table;

	public:
		TT(unsigned bits = DEF_BITS) : 
				size(0), table(NULL) {
			init(bits);
		}

		virtual ~TT() {
			delete table;
		}

		void init(unsigned bits = 0) {
			unsigned newSize = 1 << bits;
			if (bits != 0 && size != newSize) {
				size = newSize;
				mask = size - 1;
				if (table != NULL) { delete[] table; }
				table = new TTEntity[size];
			} else {
				for (unsigned i = 0; i < size; i++) {
					table[i].init();
				}
			}
		}

		bool entry(Util::uint64 hash,
				Evaluate::Value alpha,
				Evaluate::Value beta,
				Evaluate::Value value,
				int depth,
				const Shogi::Move* pmove) {
			return table[hash&mask].update(hash, alpha, beta, value, depth, pmove);
		}

		const TTEntity& getEntity(Util::uint64 hash) const {
			return table[hash&mask];
		}
	};
}

#endif // TT_H_
