/*
 * searcher.cpp
 *
 *  Created on: 2012/07/03
 *      Author: ryosuke
 */

#include "searcher.h"

namespace Search {
	using namespace Shogi;
	using namespace Evaluate;
	using namespace Table;

	Value Searcher::negaMax(Tree& tree, int depth, Value alpha, Value beta) {
		if (depth <= 0 || tree.isMaxDepth()) {
			return tree.negaEvaluate();
		}

		// initialize
		Util::uint64 hash = tree.getPosition().getHash();
		tree.initNode();

		const TTEntity& ttEntity = tt.getEntity(hash);
		Move hash1;
		Move hash2;
		if (ttEntity.is(hash)) {
			if (ttEntity.getDepth() >= depth) {
				switch (ttEntity.getValueType()) {
				case TTEntity::EXACT:
					return ttEntity.getValue();
					break;
				case TTEntity::LOWER:
					if (ttEntity.getValue() >= beta) {
						return ttEntity.getValue();
					}
					break;
				case TTEntity::UPPER:
					if (ttEntity.getValue() <= alpha) {
						return ttEntity.getValue();
					}
					break;
				}
			}
			tree.setHashMove(ttEntity.getHashMove());
		}

		tree.generateMoves();

		Value value = Value::MIN;
		const Move* best = NULL;
		while (tree.next()) {
			Value newAlpha = Value::max(alpha, value);
			tree.makeMove();
			Value newValue = -negaMax(tree, depth-PLY1, -beta, -newAlpha);
			tree.unmakeMove();

#if 0
			for (int i = 0; i < depth; i++) {
				std::cout << '\t';
			}
			std::cout << tree.getCurrentMove()->toString();
			std::cout << ' ' << value << '(' << alpha << ',' << beta << ')';
			std::cout << '\n';
#endif

			if (newValue > value) {
				value = newValue;
				tree.updatePv();
				best = tree.getCurrentMove();
				if (value >= beta) {
					break;
				}
			}
		}

		tt.entry(hash, alpha, beta, value, depth, best);

		return value;
	}

	bool Searcher::search(SearchResult& result) {
		before(result);
		Value value = negaMax(tree, config.depth * PLY1,
				Value::MIN, Value::MAX);
		return after(result, value);
	}

	bool Searcher::idSearch(SearchResult& result) {
		Value value;
		before(result);
		value = negaMax(tree, 0, Value::MIN, Value::MAX);
		for (int depth = 1; depth <= config.depth; depth++) {
			value = negaMax(tree, depth * PLY1,
					Value::MIN, Value::MAX);
			std::cout << tree.getPv().toString() << ':' << value << '\n';
		}
		return after(result, value);
	}
}
