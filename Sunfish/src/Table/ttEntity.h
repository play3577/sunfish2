/*
 * ttEntity.h
 *
 *  Created on: 2012/07/05
 *      Author: ryosuke
 */

#ifndef TTENTITY_H_
#define TTENTITY_H_

#include "../Util/int.h"
#include "../Evaluate/value.h"
#include "../Shogi/move.h"

namespace Table {
	// TODO: lock less hash
	class TTEntity {
	private:
		Util::uint64 hash;
		Evaluate::Value value;
		int valueType;
		int depth;
		Search::HashMove hashMove; // TODO: HashMove => other namespace

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

		bool update(Util::uint64 hash,
				Evaluate::Value value,
				int valueType,
				int depth,
				const Shogi::Move* pmove) {
			if (this->valueType == UNKNOWN) {
				hashMove.init();
			} else if (depth > this->depth || (this->valueType != EXACT && valueType == EXACT)) {
			} else {
				return false;
			}
			this->hash = hash;
			this->value = value;
			this->valueType = valueType;
			this->depth = depth;
			if (pmove != NULL) { hashMove.update(*pmove); }
			return true;
		}

		bool update(Util::uint64 newHash,
				Evaluate::Value alpha,
				Evaluate::Value beta,
				Evaluate::Value newValue,
				int newDepth,
				const Shogi::Move* pmove) {
			int newValueType;
			if (newValue >= beta) {
				newValueType = LOWER;
			} else if (newValue <= alpha) {
				newValueType = UPPER;
			} else {
				newValueType = EXACT;
			}
			return update(newHash, newValue, newValueType, newDepth, pmove);
		}

		bool is(Util::uint64 hash) const {
			return this->hash == hash;
		}

		Util::uint64 getHash() const {
			return hash;
		}

		Evaluate::Value getValue() const {
			return value;
		}

		int getValueType() const {
			return valueType;
		}

		int getDepth() const {
			return depth;
		}

		const Search::HashMove getHashMove() const {
			return hashMove;
		}
	};
}

#endif // TTENTITY_H_
