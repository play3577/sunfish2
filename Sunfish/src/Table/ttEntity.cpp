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
			unsigned newAge) {
		if (newDepth < 0) { newDepth = 0; }

		if (isOk()) {
			assert(hash == newHash);
			// 深さが劣るものは登録させない。
			if (newDepth < depth && age == newAge) {
				return false;
			}
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

	void TTEntities::set(const TTEntity& entity) {
		unsigned l = lastAccess;
		for (unsigned i = 0; i < SIZE; i++) {
			const unsigned index = (l + i) % SIZE;
			if (entities[index].getHash() == entity.getHash()) {
				entities[index] = entity;
				lastAccess = index;
				return;
			}
		}
		l++;
		for (unsigned i = 0; i < SIZE; i++) {
			const unsigned index = (l + i) % SIZE;
			if (entities[index].isBroken() ||
					entities[index].getAge() != entity.getAge()) {
				entities[index] = entity;
				lastAccess = index;
				return;
			}
		}
		const unsigned index = l % SIZE;
		entities[index] = entity;
		lastAccess = index;
	}

	bool TTEntities::get(Util::uint64 hash, TTEntity& entity) {
		unsigned l = lastAccess;
		for (unsigned i = 0; i < SIZE; i++) {
			const unsigned index = (l + i) % SIZE;
			if (entities[index].getHash() == hash) {
				entity = entities[index];
				lastAccess = index;
				return true;
			}
		}
		return false;
	}
}
