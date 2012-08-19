/*
 * searcher.cpp
 *
 *  Created on: 2012/07/03
 *      Author: ryosuke
 */

#include "aspWindow.h"
#include "searcher.h"

namespace Search {
	using namespace Shogi;
	using namespace Evaluate;
	using namespace Table;

	/***************************************************************
	 * quiesence search                                            *
	 * ply   : 静止探索を開始した地点からの手数                    *
	 * alpha : alpha値                                             *
	 * beta  : beta値                                              *
	 ***************************************************************/
	Value Searcher::quies(Tree& tree, int ply, Value alpha, Value beta) {
		cntNodes++;

		tree.initNode();

		// stand-pat
		const Value standPat = tree.negaEvaluate();
		// 静的評価値がbeta値を越えた場合
		if (standPat >= beta) {
			return standPat;
		}

		Value value = standPat;

		// 合法手の列挙
		if (ply < 7) {
			// 取る手と成る手を生成
			tree.generateTacticalMoves();
		} else {
			// 取る手のみを生成
			tree.generateCaptures();
		}

		while (tree.next()) {
			// alpha値
			Value newAlpha = Value::max(alpha, value);

			// futility pruning
			if (!tree.isCheck() && !tree.isCheckMove()) {
				Estimate<Value> estimate = tree.negaEstimate();
				if (estimate.getValue() + estimate.getError() <= newAlpha) {
					continue;
				}
			}

			// 子ノードを展開
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

	/***************************************************************
	 * nega-max search                                             *
	 * depth : 残り深さ(PLY1倍)                                    *
	 * alpha : alpha値                                             *
	 * beta  : beta値                                              *
	 ***************************************************************/
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
					// パスして詰まされたら自玉は詰めろ
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

	/***************************************************************
	 * 探索(反復深化を行わない。)                                  *
	 * result : 結果取得用構造体                                   *
	 * pvHandlerは呼ばれない。                                     *
	 ***************************************************************/
	bool Searcher::search(SearchResult& result) {
		// 前処理
		before(result);
		// 探索
		Value value = negaMax<true, true>(tree, config.depth * PLY1,
				Value::MIN, Value::MAX);
		// 後処理
		return after(result, value);
	}

	/***************************************************************
	 * 反復深化探索                                                *
	 * result : 結果取得用構造体                                   *
	 * 深さ毎にresult.pvHandler()を呼び出す。                      *
	 ***************************************************************/
	bool Searcher::idSearch(SearchResult& result) {
		// 各指し手毎の評価値
		Value values[MoveGenerator::MAX_MOVES_NUM];
		// 最善手の評価値
		Value value = Value::MIN;

		// 前処理
		before(result);

		// ノード初期化
		tree.initNode();
		// 合法手生成
		tree.generateMoves();
		// 反復進化探索
		for (unsigned depth = 0; depth < config.depth; depth++) {
			// 段階的に広がる探索窓 (aspiration search)
			AspWindow<-1> aspAlpha;
			AspWindow<1> aspBeta;
			// 初回ではない場合のみ前回の評価値を元にウィンドウを決定
			if (depth != 0) {
				tree.sort(values); // 前回深さの結果で並べ替え
				aspAlpha = AspWindow<-1>(values[0]);
				aspBeta = AspWindow<1>(values[0]);
			}
			// alpha値
			Value alpha = (int)aspAlpha;
			// 合法手を順に調べる。
			tree.setMoveIndex(0);
			while (tree.next()) {
				unsigned moveCount = tree.getMoveIndex();
				// 手を進める。
				tree.makeMove();
				Value vtemp;
revaluation:
				if (moveCount == 1) {
					vtemp = -negaMax<true, true>(tree,
							depth * PLY1, -aspBeta, -alpha);
				} else {
					// null window searchを試みる。
					vtemp = -negaMax<false, true>(tree,
							depth * PLY1, -alpha - 1, -alpha);
					if (vtemp >= alpha) {
						// 再探索
						vtemp = -negaMax<true, true>(tree,
								depth * PLY1, -aspBeta, -alpha);
					}
				}
				// fail-high
				if (vtemp >= aspBeta && aspBeta.next()) {
					// ウィンドウを広げたら再探索 (aspiration search)
					alpha = vtemp;
					goto revaluation;
				}
				// fail-low
				if (alpha == aspAlpha && vtemp <= aspAlpha && aspAlpha.next()) {
					// ウィンドウを広げたら再探索 (aspiration search)
					alpha = (int)aspAlpha;
					goto revaluation;
				}
				// 手を戻す。
				tree.unmakeMove();

				// 最初の手かalphaを越えた場合
				if (moveCount == 1 || vtemp > alpha) {
					// 最善手の評価値とalpha値を更新
					value = alpha = vtemp;
					// PVを更新
					tree.updatePv();
				}
				// 評価値を記憶する。
				values[moveCount-1] = vtemp;
			}

			if (config.pvHandler != NULL) {
				config.pvHandler->pvHandler(tree.getPv(), value);
			}

			// 詰み
			if (value >= Value::MATE || value <= -Value::MATE) {
				break;
			}
		}

		// 後処理
		return after(result, value);
	}
}
