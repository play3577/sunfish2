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
	using namespace Evaluates;
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
				if (standPat + estimate.getValue() + estimate.getError() <= newAlpha) {
					continue;
				}
			}

			// 子ノードを展開
			tree.makeMove();
			Value newValue = -quies(tree, ply+1, -beta, -newAlpha);
			tree.unmakeMove();
			if (interrupt()) { return Value(0); }

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
	 * mate 1 ply search                                           *
	 * tree  : 探索木                                              *
	***************************************************************/
	bool Searcher::isMate1Ply(Tree& tree) {
		tree.generateCheck();

		while (tree.next()) {
			tree.makeMove();
			if (tree.isMate()) {
				tree.unmakeMove();
				return true;
			}
			tree.unmakeMove();
		}
		return false; // 不詰め
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

		// 局面ハッシュ
		Util::uint64 hash = tree.getPosition().getHash();

		// transposition table
		const TTEntity& ttEntity = tt.getEntity(hash);
		Move hash1;
		Move hash2;
		bool hashOk = false;
		if (ttEntity.is(hash)) { // 局面が一致したら
			if (ttEntity.getDepth() >= depth) { // 深さ
				switch (ttEntity.getValueType()) {
				case TTEntity::EXACT: // 確定
					return ttEntity.getValue();
					break;
				case TTEntity::LOWER: // 下界値
					if (ttEntity.getValue() >= beta) {
						return ttEntity.getValue();
					}
					break;
				case TTEntity::UPPER: // 上界値
					if (ttEntity.getValue() <= alpha) {
						return ttEntity.getValue();
					}
					break;
				}
			}
			tree.setHashMove(ttEntity.getHashMove());
			hashOk = true;
		}

		// stand-pat
		Value standPat = Value::MIN;
#define STAND_PAT		(standPat == Value::MIN ? standPat = tree.negaEvaluate() : standPat)

		// null move pruning
		bool mate = false;
		if (nullMoveNode &&
				beta == alpha + 1 &&
				depth >= PLY1 * 2 &&
				beta <= STAND_PAT){
			int nullDepth = depth - (depth >= PLY1*8 ? depth*2/3 : (depth >= 4 ? depth/2 : PLY1*1));
			if (tree.nullMove()) {
				Value newValue = -negaMax<false, false>(tree, nullDepth, -beta, -beta+1);
				tree.unmakeMove();
				if (interrupt()) { return Value(0); }
				if (newValue >= beta) {
					return beta;
				} else if (newValue <= -Value::MATE) {
					// パスして詰まされたら自玉は詰めろ
					mate = true;
				}
			}
		}

		if (!hashOk && depth >= PLY1 * 3) {
			// recursive iterative-deepening search
			int newDepth = pvNode ? depth - PLY1 * 3 : depth / 2;
			if (newDepth < PLY1 * 2) {
				newDepth = PLY1 * 2;
			}
			negaMax<true, true>(tree, newDepth, alpha, beta);
			if (interrupt()) { return Value(0); }
			const TTEntity& tte = tt.getEntity(hash);
			if (tte.is(hash)) {
				tree.setHashMove(tte.getHashMove());
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

			// extensions
			if (tree.isCheckMove()) {
				newDepth += extension(tree);
			} else if (tree.isRecapture()) {
				newDepth += extension(tree) * 3 / 4;
			} else if (mate) {
				newDepth += extension(tree) / 2;
			}

			Estimate<Value> estimate = tree.negaEstimate();
			int reduction = 0;
			if (!isHash && !mate && !tree.isCheck() && !tree.isCheckMove() && !tree.isTacticalMove()) {
				// late move reduction
				unsigned hist = history.get(*tree.getCurrentMove());
				if (beta != alpha + 1) {
					if        (hist * 20U < History::SCALE) {
						reduction = PLY1 * 3 / 2;
					} else if (hist *  8U < History::SCALE) {
						reduction = PLY1;
					} else if (hist *  2U < History::SCALE) {
						reduction = PLY1 / 2;
					}
				} else {
					if        (hist * 16U < History::SCALE) {
						reduction = PLY1 * 2;
					} else if (hist *  8U < History::SCALE) {
						reduction = PLY1 * 3 / 2;
					} else if (hist *  3U < History::SCALE) {
						reduction = PLY1;
					} else if (hist *  1U < History::SCALE) {
						reduction = PLY1 / 2;
					}
				}
				newDepth -= reduction;

				// futility pruning
				if (STAND_PAT + estimate.getValue() + estimate.getError()
						+ getFutMgn(newDepth, moveCount)
						+ getGain(tree) <= newAlpha) {
					value = newAlpha; // fail soft
					continue;
				}
			}

			// make move
			Value newValue;
			tree.makeMove();

			// new stand-pat
			Value newStandPat = tree.negaEvaluate();

			// extended futility pruning
			if (!isHash && !mate && !tree.isCheck() && !tree.isTacticalMove()) {
				if (newStandPat - getFutMgn(newDepth, moveCount) >= -newAlpha) {
					tree.unmakeMove();
					value = newAlpha; // fail soft
					continue;
				}
			}

			updateGain(tree, STAND_PAT + estimate.getValue(), newStandPat);

			// recurcive search
			if (moveCount == 1 && reduction == 0) {
				newValue = -negaMax<pvNode, true>(tree, newDepth, -beta, -newAlpha);
			} else {
				// nega-scout
				newValue = -negaMax<false, true>(tree, newDepth, -newAlpha-1, -newAlpha);
				// 値がalpha値を超えて、かつnull windowでないかあるいはreductionが効いていたとき
				if (!interrupt() && newValue >= newAlpha && (beta > newAlpha + 1 || reduction != 0)) {
					// reductionをなくして再探索
					newDepth += reduction;
					newValue = -negaMax<pvNode, true>(tree, newDepth, -beta, -newAlpha);
				}
			}

			// unmake move
			tree.unmakeMove();

			if (interrupt()) {
				return Value(0);
			}

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
		// 基本深さ
		rootDepth = config.depth;

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
		// TODO: 指し手がない場合
		// TODO: 確定手
		// 反復進化探索
		for (unsigned depth = 0; depth < config.depth; depth++) {
			// 基本深さ
			rootDepth = depth;
			// 段階的に広がる探索窓 (aspiration search)
			AspWindow<-1> aspAlpha;
			AspWindow<1> aspBeta;
			// 初回ではない場合のみ前回の評価値を元にウィンドウを決定
			if (depth != 0) {
				tree.sort(values); // 前回深さの結果で並べ替え
				aspAlpha.init(values[0]);
				aspBeta.init(values[0]);
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
					if (!interrupt() && vtemp >= alpha) {
						// 再探索
						vtemp = -negaMax<true, true>(tree,
								depth * PLY1, -aspBeta, -alpha);
					}
				}
				if (interrupt()) {
					tree.unmakeMove();
					goto lab_search_end;
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
lab_search_end:

		if (config.pvHandler != NULL) {
			config.pvHandler->pvHandler(tree.getPv(), value);
		}

		// 後処理
		return after(result, value);
	}
}
