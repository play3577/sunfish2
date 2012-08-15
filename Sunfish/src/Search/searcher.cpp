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

	Value Searcher::quies(Tree& tree, int ply, Value alpha, Value beta) {
		cntNodes++;

		tree.initNode();

		const Value standPat = tree.negaEvaluate();
		Value value = standPat;

		if (value >= beta) {
			// return stand-pat
			return value;
		}

		// initialize move generator
		if (ply < 7) {
			tree.generateTacticalMoves();
		} else {
			tree.generateCaptures();
		}

		while (tree.next()) {
			Value newAlpha = Value::max(alpha, value);

			// futility pruning
			if (!tree.isCheck() && !tree.isCheckMove()) {
				Estimate<Value> estimate = tree.negaEstimate();
				if (estimate.getValue() + estimate.getError() <= newAlpha) {
					continue;
				}
			}

			tree.makeMove();
			Value newValue = -quies(tree, ply+1, -beta, -newAlpha);
			tree.unmakeMove();
			if (newValue > value) {
				value = newValue;
				tree.updatePv();

				// beta cut
				if (value >= beta) {
					break;
				}
			}
		}

		return value;
	}

	template <bool pvNode, bool nullMoveNode>
	Value Searcher::negaMax(Tree& tree, int depth, Value alpha, Value beta) {
		cntNodes++;

		tree.initNode();

		// leaf node
		if (depth <= 0 || tree.isMaxDepth()) {
			// quiesence search
			return quies(tree, 0, alpha, beta);
		}

		// initialize
		Util::uint64 hash = tree.getPosition().getHash();

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
		} else if (depth >= PLY1 * 3) {
			// recursive iterative-deepening search
			if (pvNode) {
				negaMax<true, true>(tree, depth - PLY1 * 2, alpha, beta);
			} else {
				negaMax<false, true>(tree, depth - PLY1 / 2, alpha, beta);
			}
		}

		// null move pruning
		bool mate = false;
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
				} else if (newValue <= -Value::MATE) {
					mate = true;
				}
			}
		}

		// initialize move generator
		tree.generateMoves();

		Value value = Value::MIN;
		const Move* best = NULL;
		while (tree.next()) {
			unsigned moveCount = tree.getMoveIndex();
			bool isHash = tree.isHashMove();

			assert(tree.getCurrentMove() != NULL);

			Value newAlpha = Value::max(alpha, value);

			int newDepth = depth - PLY1;
			int reduction = 0;
			if (!isHash && !mate && !tree.isCheck() && !tree.isCheckMove() && !tree.isTacticalMove()) {
				// late move reduction
				unsigned hist = history.get(*tree.getCurrentMove());
				if        (hist * 16U < History::SCALE) {
					reduction = PLY1 * 2;
				} else if (hist * 12U < History::SCALE) {
					reduction = PLY1 * 3 / 2;
				} else if (hist *  8U < History::SCALE) {
					reduction = PLY1;
				} else if (hist *  6U < History::SCALE) {
					reduction = PLY1 * 3 / 4;
				} else if (hist *  4U < History::SCALE) {
					reduction = PLY1 / 2;
				} else if (hist *  2U < History::SCALE) {
					reduction = PLY1 / 4;
				}
				newDepth -= reduction;

				// futility pruning
				Estimate<Value> estimate = tree.negaEstimate();
				if (tree.negaEvaluate() + estimate.getValue() + estimate.getError()
						+ getFutMgn(newDepth, moveCount) <= newAlpha) {
					value = newAlpha; // fail soft
					continue;
				}
			}

			// make move
			Value newValue;
			tree.makeMove();

			// extended futility pruning
			if (!isHash && !mate && !tree.isCheck() && !tree.isTacticalMove()) {
				if (tree.negaEvaluate() - getFutMgn(newDepth, moveCount) >= -newAlpha) {
					tree.unmakeMove();
					value = newAlpha; // fail soft
					continue;
				}
			}

			// recurcive search
			if (moveCount == 1 && reduction == 0) {
				newValue = -negaMax<pvNode, true>(tree, newDepth, -beta, -newAlpha);
			} else {
				// nega-scout
				newValue = -negaMax<false, true>(tree, newDepth, -newAlpha-1, -newAlpha);
				if (newValue >= newAlpha && (beta > newAlpha + 1 || reduction != 0)) {
					newDepth += reduction;
					newValue = -negaMax<pvNode, true>(tree, newDepth, -beta, -newAlpha);
				}
			}

			// unmake move
			tree.unmakeMove();

			if (newValue > value) {
				value = newValue;
				tree.updatePv();
				best = tree.getCurrentMove();
				tree.getHistory(history, depth);

				// beta cut
				if (value >= beta) {
					break;
				}
			}
		}

		// there is no legal moves
		if (tree.getMoveIndex() == 0) {
			return Value::MIN + tree.getDepth();
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
		Value values[MoveGenerator::MAX_MOVES_NUM];
		Value value = Value::MIN;

		before(result);

		tree.initNode();
		tree.generateMoves();
		for (unsigned depth = 0; depth < config.depth; depth++) {
			if (depth != 0) { tree.sort(values); }
			tree.setMoveIndex(0);
			Value alpha = Value::MIN - 1;
			while (tree.next()) {
				tree.makeMove();
				Value vtemp = -negaMax<true, true>(tree,
						depth * PLY1, -alpha + 1, -alpha);
				if (vtemp >= alpha) {
					vtemp = -negaMax<true, true>(tree,
							depth * PLY1, -alpha + 1, Value::MAX);
				}
				tree.unmakeMove();
				if (vtemp > alpha) {
					value = alpha = vtemp;
					tree.updatePv();
				}
				values[tree.getMoveIndex()-1] = vtemp;
			}

			if (config.pvHandler != NULL) {
				config.pvHandler->pvHandler(tree.getPv(), value);
			}
			if (value >= Value::MATE || value <= -Value::MATE) {
				break;
			}
		}

		return after(result, value);
	}
}
