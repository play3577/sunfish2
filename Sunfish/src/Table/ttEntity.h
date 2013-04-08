/*
 * ttEntity.h
 *
 *  Created on: 2012/07/05
 *      Author: ryosuke
 */

#ifndef TTENTITY_H_
#define TTENTITY_H_

#include "../Util/int.h"
#include "../Evaluates/value.h"
#include "../Search/hashMove.h"
#include "../Search/nodeStat.h"
#include <cassert>

namespace Table {
	class TTEntity {
	private:
		Util::uint64 hash;
		Evaluates::Value value;
		int valueType;
		int depth;
		Search::HashMove hashMove;
		Search::NodeStat stat;
		unsigned age;
		Util::uint64 checkSum;

		Util::uint64 generateCheckSum() const {
			return hash ^ (Util::uint64)(int)value
					^ ((Util::uint64)valueType << 16)
					^ ((Util::uint64)depth << 32)
					^ (Util::uint64)hashMove
					^ (Util::uint64)age
					^ ((Util::uint64)(unsigned)stat << 48);
		}

		bool update(Util::uint64 newHash,
				Evaluates::Value newValue,
				int newValueType,
				int newDepth,
				const Search::NodeStat& newStat,
				const Shogi::Move& move,
				unsigned newAge);

	public:
		enum {
			EXACT = 0,
			UPPER,
			LOWER,
		};

		TTEntity() {
			init();
		}

		void init() {
			checkSum = generateCheckSum() + U64(1);
		}

		bool update(Util::uint64 newHash,
				Evaluates::Value alpha,
				Evaluates::Value beta,
				Evaluates::Value newValue,
				int newDepth,
				const Search::NodeStat& newStat,
				const Shogi::Move& move,
				unsigned newAge) {
			int newValueType;
			if (newValue >= beta) {
				newValueType = LOWER;
			} else if (newValue <= alpha) {
				newValueType = UPPER;
			} else {
				newValueType = EXACT;
			}
			return update(newHash, newValue, newValueType,
					newDepth, newStat, move, newAge);
		}

		bool isOk() const {
			return checkSum == generateCheckSum();
		}

		bool isBroken() const {
			return !isOk();
		}

		bool is(Util::uint64 hash) const {
			return this->hash == hash && isOk();
		}

		bool isSuperior(int curDepth) const {
			using namespace Evaluates;
			if (depth >= curDepth) {
				return true;
			}
			if (value >= Value::MATE && valueType == LOWER) {
				return true;
			}
			if (value <= Value::MATE && valueType == UPPER) {
				return true;
			}
			return false;
		}

		Util::uint64 getHash() const {
			return hash;
		}

		Evaluates::Value getValue() const {
			return value;
		}

		int getValueType() const {
			return valueType;
		}

		int getDepth() const {
			return depth;
		}

		const Search::NodeStat& getStat() const {
			return stat;
		}

		const Search::HashMove getHashMove() const {
			return hashMove;
		}

		unsigned getAge() const {
			return age;
		}
	};

	class TTEntities {
	private:
		static const unsigned SIZE = 4;
		TTEntity entities[SIZE];
		volatile unsigned lastAccess;

	public:
		TTEntities() : lastAccess(0) {
		}

		void init(unsigned) {
			for (unsigned i = 0; i < SIZE; i++) {
				entities[i].init();
			}
		}

		void set(const TTEntity& entity);

		bool get(Util::uint64 hash, TTEntity& entity);
	};
}

#endif // TTENTITY_H_
