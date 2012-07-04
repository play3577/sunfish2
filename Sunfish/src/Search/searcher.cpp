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

	template <bool root>
	Value Searcher::negaMax(Tree& tree, int depth, Value alpha, Value beta, Move* pmove) {
		if (depth <= 0 || tree.isMaxDepth()) {
			return tree.negaEvaluate();
		}

		tree.initPv();
		tree.generateMoves();
		while (tree.next()) {
			tree.makeMove();
			Value value = -negaMax<false>(tree, depth-1, -beta, -alpha, NULL);
			tree.unmakeMove();

#if 0
			for (int i = 0; i < depth; i++) {
				std::cout << '\t';
			}
			std::cout << tree.getCurrentMove()->toString();
			std::cout << ' ' << value << '(' << alpha << ',' << beta << ')';
			std::cout << '\n';
#endif

			if (value > alpha) {
				alpha = value;
				tree.updatePv();
				if (root) {
					*pmove = *tree.getCurrentMove();
				}
				if (alpha >= beta) {
					break;
				}
			}
		}

		return alpha;
	}

	bool Searcher::search(SearchResult& result) {
		memset(&result, 0, sizeof(SearchResult));
		result.value = negaMax<true>(tree, config.depth,
				Value::MIN, Value::MAX, &result.move);
		result.pv.copy(tree.getPv());
		return true;
	}
}
