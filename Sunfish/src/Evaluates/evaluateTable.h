/*
 * evaluateTable.h
 *
 *  Created on: 2012/07/09
 *      Author: ryosuke
 */

#ifndef EVALUATETABLE_H_
#define EVALUATETABLE_H_

#include "evEntity.h"
#include "../Table/baseTable.h"

namespace Evaluates {
	using namespace Table;

	class EvaluateTable : public BaseTable<EvEntity> {
	public:
		// TODO: サイズの指定を設定ファイルから読み込めるように
		EvaluateTable(unsigned bits = 21) :
				BaseTable<EvEntity>(bits) {
		}

		bool get(Util::uint64 hash, Value& value) const {
			return getEntity(hash).get(hash, value);
		}

		void set(Util::uint64 hash, const Value& value) {
			_getEntity(hash).set(hash, value);
		}
	};
}

#endif // EVALUATETABLE_H_
