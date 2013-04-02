/*
 * ttEntity.cpp
 *
 *  Created on: 2012/07/05
 *      Author: ryosuke
 */

#include "ttEntity.h"

namespace Table {
	bool TTEntity::update(Util::uint64 newHash,
			Evaluates::Value newValue,
			int newValueType,
			int newDepth,
			const Search::NodeStat& newStat,
			const Shogi::Move& move,
			int newAge) {
		if (newDepth < 0) { newDepth = 0; }

		if (isOk()) {
			// 深さが劣るものは登録させない。
			if (newDepth < depth) {
				return false;
			}
			assert(hash == newHash);
		} else {
			hash = newHash;
			hashMove.init();
		}

		value = newValue;
		valueType = newValueType;
		depth = newDepth;
		stat = newStat;
		if (!move.isEmpty()) { hashMove.update(move); }
		age = newAge;
		checkSum = generateCheckSum();

		return true;
	}
}
