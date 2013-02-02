/*
 * baseTable.h
 *
 *  Created on: 2012/11/03
 *      Author: ryosuke
 */

#ifndef BASETABLE_H_
#define BASETABLE_H_

namespace Table {
	template <class Entity>
	class BaseTable {
	protected:
		unsigned size;
		unsigned mask;
		Entity* table;

		Entity& _getEntity(Util::uint64 hash) {
			return table[hash&mask];
		}

	public:
		static const unsigned DEF_BITS = 21;

		BaseTable(unsigned bits = DEF_BITS) : 
				size(0), table(NULL) {
			init(bits);
		}

		virtual ~BaseTable() {
			delete [] table;
		}

		void init(unsigned bits = 0) {
			unsigned newSize = 1 << bits;
			if (bits != 0 && size != newSize) {
				size = newSize;
				mask = size - 1;
				if (table != NULL) { delete[] table; }
				table = new Entity[size];
			} else {
				for (unsigned i = 0; i < size; i++) {
					table[i].init();
				}
			}
		}

		const Entity& getEntity(Util::uint64 hash) const {
			return table[hash&mask];
		}
	};
}

#endif // BASETABLE_H_
