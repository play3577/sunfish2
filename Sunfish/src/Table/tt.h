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
	class TT : public BaseTable<TTEntities> {
	private:
		static const int AGE_MAX = 8;
		int age;

	public:
		TT() : BaseTable<TTEntities>(), age(0) {
		}

		TT(unsigned bits) : BaseTable<TTEntities>(bits), age(0) {
		}

		void evolve() {
			age = (age + 1) % AGE_MAX;
		}

		bool entry(Util::uint64 hash,
				Evaluates::Value alpha,
				Evaluates::Value beta,
				Evaluates::Value value,
				int depth,
				const Search::NodeStat& stat,
				const Shogi::Move& move) {
			TTEntity entity;
			TTEntities& entities = _getEntity(hash);
			entities.get(hash, entity);
			if (entity.update(hash, alpha, beta, value,
					depth, stat, move, age)) {
				entities.set(entity);
				return true;
			}
			return false;
		}

		bool get(Util::uint64 hash, TTEntity& entity) {
			return _getEntity(hash).get(hash, entity)
					&& entity.is(hash);
		}
	};
}

#endif // TT_H_
