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

namespace Table {
	// TODO: lock less hash
	class TTEntity {
	private:
		Util::uint64 hash;
		Evaluates::Value value;
		int valueType;
		int depth;
		Search::HashMove hashMove;
		Search::NodeStat stat;

		bool update(Util::uint64 newHash,
				Evaluates::Value newValue,
				int newValueType,
				int newDepth,
				const Search::NodeStat& newStat,
				const Shogi::Move* pmove) {
			if (newDepth < 0) { newDepth = 0; }

			if (valueType == UNKNOWN) {
				hashMove.init();
			} else if (newDepth < depth) {
				return false;
			}

			hash = newHash;
			value = newValue;
			valueType = newValueType;
			depth = newDepth;
			stat = newStat;
			if (pmove != NULL) { hashMove.update(*pmove); }

			return true;
		}

	public:
		enum {
			UNKNOWN = 0,
			EXACT,
			UPPER,
			LOWER,
		};

		TTEntity() {
			init();
		}

		void init() {
			valueType = UNKNOWN;
		}

		bool update(Util::uint64 newHash,
				Evaluates::Value alpha,
				Evaluates::Value beta,
				Evaluates::Value newValue,
				int newDepth,
				const Search::NodeStat& newStat,
				const Shogi::Move* pmove) {
			int newValueType;
			if (newValue >= beta) {
				newValueType = LOWER;
			} else if (newValue <= alpha) {
				newValueType = UPPER;
			} else {
				newValueType = EXACT;
			}
			return update(newHash, newValue, newValueType, newDepth, newStat, pmove);
		}

		bool is(Util::uint64 hash) const {
			return this->hash == hash;
		}

		bool isSuperior(int curDepth) const {
			if (depth >= curDepth) {
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
	};
}

#endif // TTENTITY_H_
