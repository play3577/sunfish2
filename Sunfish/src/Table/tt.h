/*
 * tt.h
 *
 *  Created on: 2012/07/05
 *      Author: ryosuke
 */

#ifndef TT_H_
#define TT_H_

#include "../Util/int.h"
#include "baseTable.h"
#include "ttEntity.h"

namespace Table {
	class TT : public BaseTable<TTEntity> {
	public:
		TT() : BaseTable<TTEntity>() {
		}

		TT(unsigned bits) : BaseTable<TTEntity>(bits) {
		}

		bool entry(Util::uint64 hash,
				Evaluates::Value alpha,
				Evaluates::Value beta,
				Evaluates::Value value,
				int depth,
				const Search::NodeStat& stat,
				const Shogi::Move* pmove = NULL) {
			return table[hash&mask].update(hash, alpha, beta, value, depth, stat, pmove);
		}
	};
}

#endif // TT_H_
