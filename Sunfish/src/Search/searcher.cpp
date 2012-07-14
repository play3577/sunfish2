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

	template <bool pvNode, bool nullMoveNode>
	Value Searcher::negaMax(Tree& tree, int depth, Value alpha, Value beta) {
		// leaf node
		if (depth <= 0 || tree.isMaxDepth()) {
			// return static evaluation value
			return tree.negaEvaluate();
		}

		// initialize
		Util::uint64 hash = tree.getPosition().getHash();
		tree.initNode();

		// transposition table
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
		} else {
			// TODO: recursive iterative-deepening search
		}

		// null move pruning
		int nullDepth = depth - (depth >= PLY1*8 ? depth*2/3 : (depth >= 4 ? depth/2 : PLY1*1));
		if (nullMoveNode &&
				beta == alpha + 1 &&
				nullDepth > PLY1*tree.getDepth() &&
				beta < tree.negaEvaluate()){
			if (tree.nullMove()) {
				Value newValue = -negaMax<false, false>(tree, nullDepth, -beta, -beta+1);
				tree.unmakeMove();
				if (newValue >= beta) {
					return beta;
				}
			}
		}

		// initialize move generator
		tree.generateMoves();

		Value value = Value::MIN;
		const Move* best = NULL;
		int moveCount = 0;
		while (tree.next()) {
			moveCount++;

			// recurcive search
			Value newAlpha = Value::max(alpha, value);
			Value newValue;
			tree.makeMove();
			if (moveCount == 1) {
				newValue = -negaMax<pvNode, true>(tree, depth-PLY1, -beta, -newAlpha);
			} else {
				// nega-scout
				newValue = -negaMax<false, true>(tree, depth-PLY1, -newAlpha-1, -newAlpha);
				if (newValue >= newAlpha) {
					newValue = -negaMax<pvNode, true>(tree, depth-PLY1, -beta, -newAlpha);
				}
			}
			tree.unmakeMove();

			if (newValue > value) {
				value = newValue;
				tree.updatePv();
				best = tree.getCurrentMove();

				// beta cut
				if (value >= beta) {
					break;
				}
			}
		}

		// there is no legal moves
		if (moveCount == 0) {
			return Value::MIN;
		}

		// TT entry
		tt.entry(hash, alpha, beta, value, depth, best);

		return value;
	}
	template Value Searcher::negaMax<true, true>(Tree& tree, int depth, Value alpha, Value beta);
	template Value Searcher::negaMax<true, false>(Tree& tree, int depth, Value alpha, Value beta);
	template Value Searcher::negaMax<false, true>(Tree& tree, int depth, Value alpha, Value beta);
	template Value Searcher::negaMax<false, false>(Tree& tree, int depth, Value alpha, Value beta);

	bool Searcher::search(SearchResult& result) {
		before(result);
		Value value = negaMax<true, true>(tree, config.depth * PLY1,
				Value::MIN, Value::MAX);
		return after(result, value);
	}

	bool Searcher::idSearch(SearchResult& result) {
		Value value;
		before(result);
		value = negaMax<true, true>(tree, 0, Value::MIN, Value::MAX);
		for (int depth = 1; depth <= config.depth; depth++) {
			value = negaMax<true, true>(tree, depth * PLY1,
					Value::MIN, Value::MAX);
			if (config.pvHandler != NULL) {
				config.pvHandler->pvHandler(tree.getPv(), value);
			}
		}
		return after(result, value);
	}
}
