/*
 * searcher.cpp
 *
 *  Created on: 2012/07/03
 *      Author: ryosuke
 */

#include "aspWindow.h"
#include "searcher.h"

#define ROOT_NODE_DEBUG				0
#define NODE_DEBUG					0
#define VARIATION_DEBUG				0

namespace Search {
	using namespace Shogi;
	using namespace Evaluates;
	using namespace Table;

	/***************************************************************
	 * mate 1 ply search                                           *
	 * tree  : 探索木                                              *
	***************************************************************/
	bool Searcher::isMate1Ply(Tree& tree) {
		tree.generateCheck();

		while (tree.next()) {
			makeMove(false);
			if (tree.isMate()) {
				unmakeMove(false);
				return true;
			}
			unmakeMove(false);
		}
		return false; // 不詰め
	}

	/***************************************************************
	 * quiesence search                                            *
	 * ply   : 静止探索を開始した地点からの手数                    *
	 * alpha : alpha値                                             *
	 * beta  : beta値                                              *
	 ***************************************************************/
	Value Searcher::quies(Tree& tree, int ply, Value alpha, Value beta) {
		counter.nodes++;

#if VARIATION_DEBUG
		tree.debugPrint();
#endif

		tree.initNode();

		// stand-pat
		const Value standPat = tree.negaEvaluate();

		// 静的評価値がbeta値を越えた場合
		if (standPat >= beta) {
			return standPat;
		}

		// mate
		if (!tree.isCheck()) {
			if (isMate1Ply(tree)) {
				return Value::MAX;
			}
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
				Estimate estimate = tree.negaEstimate();
				if (standPat + estimate.getValue() + estimate.getError() <= newAlpha) {
					continue;
				}
			}

			// 子ノードを展開
			makeMove(false);
			Value newValue = -quies(tree, ply+1, -beta, -newAlpha);
			unmakeMove(false);
			if (isInterrupted()) { return Value(0); }

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
	template <bool pvNode>
	Value Searcher::negaMax(Tree& tree, int depth,
			Value alpha, Value beta, NodeStat stat) {
#if NODE_DEBUG
		bool debugNode = false;
		if (tree.is("-1314FU")) {
			Log::debug << " ***** {" << alpha << ',' << beta << '}';
			debugNode = true;
		}
#endif // NODE_DEBUG

#if VARIATION_DEBUG
		tree.debugPrint();
#endif

		// TODO: distance pruning

#ifndef NLEARN
		if (config.isLearning)
#endif // NLEARN
		{
			switch (shekCheck()) {
			case Shek::NONE:
				break;
			case Shek::SUPERIOR:
				counter.shekPruning++;
				return Value::MAX;
			case Shek::INFERIOR:
				counter.shekPruning++;
				return Value::MIN;
			case Shek::EQUAL:
				counter.shekPruning++;
				return Value(0);
			}
		}

		// end of stack
		if (tree.isMaxDepth()) {
			return tree.negaEvaluate();
		}

		// leaf node
		if (depth < PLY1) {
			// quiesence search
			return quies(tree, 0, alpha, beta);
		}

		tree.initNode();

		counter.nodes++;

		// 局面ハッシュ
		Util::uint64 hash = tree.getPosition().getHash();

		// transposition table
		const TTEntity& ttEntity = tt.getEntity(hash);
		Move hash1;
		Move hash2;
		bool hashOk = false;
		if (ttEntity.is(hash)) { // 局面が一致したら
			if (ttEntity.isSuperior(depth)
#ifndef NLEARN
					&& config.isLearning
#endif // NLEARN
					) {
				switch (ttEntity.getValueType()) {
				case TTEntity::EXACT: // 確定
					if (!pvNode && stat.isHashCut()) {
						counter.hashPruning++;
						return ttEntity.getValue();
					}
				case TTEntity::LOWER: // 下界値
					if (!pvNode && stat.isHashCut() && ttEntity.getValue() >= beta) {
						counter.hashPruning++;
						return ttEntity.getValue();
					}
					break;
				case TTEntity::UPPER: // 上界値
					if (!pvNode && stat.isHashCut() && ttEntity.getValue() <= alpha) {
						counter.hashPruning++;
						return ttEntity.getValue();
					}
					break;
				}
			}
			if (ttEntity.getDepth() >= depth - PLY1 * 3) {
				tree.setHashMove(ttEntity.getHashMove());
				hashOk = true;
			}
		}

		// stand-pat
		Value standPat = Value::MIN;
#define STAND_PAT		(standPat == Value::MIN ? standPat = tree.negaEvaluate() : standPat)

		// 詰めろ
		bool mate = false;

		if (!tree.isCheck()) {
			// mate
			if (stat.isMate()) {
				if (isMate1Ply(tree)) {
					// TODO: 深さの考慮
					return Value::MAX;
				}
			}

			// null move pruning
			if (stat.isNullMove() &&
					!pvNode &&
					depth >= PLY1 * 2 &&
					beta <= STAND_PAT){
				int nullDepth = 
						(depth >= PLY1*15/2 ? depth - PLY1*4 :
						(depth >= PLY1*9/2 ? depth - PLY1*3 : depth - PLY1*2));
				if (nullMove(false)) {
					Value newValue = -negaMax<false>(tree, nullDepth, -beta, -beta+1, NodeStat().unsetNullMove());
					unmakeMove(false);
					if (isInterrupted()) { return Value(0); }
					if (newValue >= beta) {
						counter.nullMovePruning++;
						// TT entry
						//if (nullDepth < PLY1) {
						//	tt.entry(hash, alpha, beta, newValue, depth, stat);
						//}
						return beta;
					} else if (newValue <= -Value::MATE) {
						// パスして詰まされたら自玉は詰めろ
						mate = true;
					}
				}
			}
		}

		if (!hashOk && depth >= PLY1 * 3 && pvNode) {
			int newDepth = depth >= PLY1*9/2 ? depth - PLY1*3 : PLY1*3/2;
			negaMax<pvNode>(tree, newDepth, alpha, beta,
					NodeStat(stat).unsetNullMove().unsetMate().unsetHashCut());
			if (isInterrupted()) { return Value(0); }
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
			// node status
			NodeStat newStat;

			unsigned moveCount = tree.getMoveIndex();
			bool isHash = tree.isHashMove();

			assert(tree.getCurrentMove() != NULL);

#if NODE_DEBUG
			if (debugNode) {
				Log::debug << ' ' << tree.getCurrentMove()->toString();
			}
#endif // NODE_DEBUG

			Value newAlpha = Value::max(alpha, value);

			int newDepth = depth - PLY1;

			// TODO: クラスにまとめる。
			bool isCheckMove = tree.isCheckMove();
			bool isTacticalMove = tree.isTacticalMove();
			bool isRecapture = tree.isRecapture();

			// extensions
			if (isCheckMove) {
				newDepth += extension(tree);
			} else if (stat.isRecapture() && isRecapture) {
				newDepth += extension(tree) * 3 / 4;
				newStat.unsetRecapture();
			} else if (mate) {
				newDepth += extension(tree) / 2;
			}

			Estimate estimate = tree.negaEstimate();
			int reduction = 0;
			if (!isHash && moveCount != 1 && !mate && !tree.isCheck() && !isCheckMove && !isTacticalMove) {
				// late move reduction
				unsigned hist = history.get(*tree.getCurrentMove());
				if (beta != alpha + 1) {
					if        (hist * 32U < History::SCALE) {
						reduction = PLY1 * 3 / 2;
					} else if (hist * 16U < History::SCALE) {
						reduction = PLY1;
					} else if (hist *  4U < History::SCALE) {
						reduction = PLY1 / 2;
					}
				} else {
					if        (hist * 32U < History::SCALE) {
						reduction = PLY1 * 2;
					} else if (hist * 20U < History::SCALE) {
						reduction = PLY1 * 3 / 2;
					} else if (hist *  8U < History::SCALE) {
						reduction = PLY1;
					} else if (hist *  2U < History::SCALE) {
						reduction = PLY1 / 2;
					}
				}
				newDepth -= reduction;

				// futility pruning
				if (STAND_PAT + estimate.getValue() + estimate.getError()
						+ getFutMgn(newDepth, moveCount)
						+ getGain(tree) <= newAlpha) {
					value = newAlpha; // fail soft
					counter.futilityPruning++;
#if NODE_DEBUG
			if (debugNode) {
				Log::debug << ',' << __LINE__;
				if (tree.getCurrentMove()->toStringCsa() == "+4748KI") {
					Log::debug << ",fut_mgn=[" << getFutMgn(newDepth, moveCount) << "]"
							<< " gain=[" << getGain(tree) << "]"
							<< " depth=[" << newDepth << "]";
				}
			}
#endif // NODE_DEBUG
					continue;
				}
			}

			// make move
			Value newValue;
			makeMove();

			// new stand-pat
			Value newStandPat = tree.negaEvaluate();

			// extended futility pruning
			if (!isHash && !mate && !isCheckMove && !isTacticalMove) {
				if (newStandPat - getFutMgn(newDepth, moveCount) >= -newAlpha) {
					unmakeMove();
					value = newAlpha; // fail soft
					counter.exFutilityPruning++;
#if NODE_DEBUG
					if (debugNode) {
						Log::debug << ',' << __LINE__;
					}
#endif // NODE_DEBUG
					continue;
				}
			}

			updateGain(tree, STAND_PAT + estimate.getValue(), newStandPat);

#if NODE_DEBUG
			Util::uint64 beforeNodes = counter.nodes;
#endif // NODE_DEBUG
			// recurcive search
			if (moveCount == 1) {
				newValue = -negaMax<pvNode>(tree, newDepth, -beta, -newAlpha, newStat);
#if NODE_DEBUG
				if (debugNode) {
					Log::debug << ',' << __LINE__;
				}
#endif // NODE_DEBUG
			} else {
				// nega-scout
				newValue = -negaMax<false>(tree, newDepth, -newAlpha-1, -newAlpha, newStat);
#if NODE_DEBUG
				if (debugNode) {
					Log::debug << ',' << __LINE__;
				}
#endif // NODE_DEBUG
				if (!isInterrupted() && newValue > newAlpha && reduction != 0) {
					// reductionをなくして再探索
					newDepth += reduction;
					newValue = -negaMax<false>(tree, newDepth, -newAlpha-1, -newAlpha, newStat);
#if NODE_DEBUG
					if (debugNode) {
						Log::debug << ',' << __LINE__;
					}
#endif // NODE_DEBUG
				}
				if (!isInterrupted() && newValue > newAlpha && beta > newAlpha + 1) {
					// windowを広げて再探索
					newValue = -negaMax<pvNode>(tree, newDepth, -beta, -newAlpha, newStat);
#if NODE_DEBUG
					if (debugNode) {
						Log::debug << ',' << __LINE__;
					}
#endif // NODE_DEBUG
				}
			}
#if NODE_DEBUG
			if (debugNode) {
				Util::uint64 afterNodes= counter.nodes;
				Util::uint64 nodes = afterNodes - beforeNodes;
				Log::debug << "(" << newValue << ")[" << nodes << "]";
			}
#endif // NODE_DEBUG

			// unmake move
			unmakeMove();

			if (isInterrupted()) {
				return Value(0);
			}

			if (newValue > value) {
				value = newValue;
				tree.updatePv();
				best = tree.getCurrentMove();

				// beta cut
				if (value >= beta) {
					if (!tree.isCapture()) {
						tree.getHistory(history, depth);
						tree.addKiller(newValue - STAND_PAT);
					} else if (!isHash) {
						tree.addKiller(newValue - STAND_PAT);
					}
#if NODE_DEBUG
					if (debugNode) {
						Log::debug << ";bc";
					}
#endif // NODE_DEBUG
					break;
				}
			}
		}

		// there is no legal moves
		if (tree.getMoveIndex() == 0) {
			return Value::MIN + tree.getDepth();
		}

		// TT entry
		tt.entry(hash, alpha, beta, value, depth, stat, best);

		return value;
	}
	template Value Searcher::negaMax<true>(Tree& tree, int depth,
			Value alpha, Value beta, NodeStat stat);
	template Value Searcher::negaMax<false>(Tree& tree, int depth,
			Value alpha, Value beta, NodeStat stat);

	/***************************************************************
	 * 探索(反復深化を行わない。)                                  *
	 * result : 結果取得用構造体                                   *
	 * pvHandlerは呼ばれない。                                     *
	 ***************************************************************/
	bool Searcher::search(SearchResult& result, Value alpha, Value beta) {
		// 前処理
		before(result);
		// 基本深さ
		rootDepth = config.depth;

		// 探索
		Value value = negaMax<true>(tree, config.depth * PLY1, alpha, beta);

		// 後処理
		return after(result, value);
	}

	/***************************************************************
	 * 反復深化探索                                                *
	 * result : 結果取得用構造体                                   *
	 * 深さ毎にresult.pvHandler()を呼び出す。                      *
	 ***************************************************************/
	bool Searcher::idSearch(SearchResult& result) {
		// 最善手の評価値
		Value maxValue = Value::MIN;
		// 各指し手毎の評価値
		Value values[MoveGenerator::MAX_MOVES_NUM];

		// 前処理
		before(result);

		// ノード初期化
		tree.initNode();
		// 合法手生成
		tree.generateMoves();
		// TODO: 指し手がない場合
		// TODO: 確定手
		// 反復進化探索
		unsigned depth;
		for (depth = 0; depth < config.depth; depth++) {
			Value minValue = Value::MIN;
			// 基本深さ
			rootDepth = depth + 1;
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
#if ROOT_NODE_DEBUG
				// debug
				Util::uint64 beforeNodes= counter.nodes;
				//Log::debug << '<' << tree.getNumberOfMoves() << ',' << moveCount << '>';
#endif // ROOT_NODE_DEBUG

revaluation:
				// 手を進める。
				makeMove();
				Value vtemp;
				if (moveCount == 1) {
					vtemp = -negaMax<true>(tree,
							depth * PLY1, -aspBeta, -alpha);
#if ROOT_NODE_DEBUG
					Log::debug << "f";
#endif // ROOT_NODE_DEBUG
				} else {
					// null window searchを試みる。
					vtemp = -negaMax<false>(tree,
							depth * PLY1, -alpha - 1, -alpha);
#if ROOT_NODE_DEBUG
					Log::debug << "n";
#endif // ROOT_NODE_DEBUG
					if (!isInterrupted() && vtemp > alpha && vtemp < Value::MATE) {
#if ROOT_NODE_DEBUG
						Log::debug << "(" << vtemp << ")";
#endif // ROOT_NODE_DEBUG
						// 再探索
						vtemp = -negaMax<true>(tree,
								depth * PLY1, -aspBeta, -alpha,
								NodeStat().unsetNullMove());
#if ROOT_NODE_DEBUG
						Log::debug << "f";
#endif // ROOT_NODE_DEBUG
					}
				}
#if ROOT_NODE_DEBUG
				// debug
				Util::uint64 afterNodes= counter.nodes;
				Util::uint64 nodes = afterNodes - beforeNodes;
				Log::debug << tree.getPrevMove()->toString() << "(" << vtemp << ")"
						<< "[" << nodes << "]"
						<< "{" << (int)alpha << "," << (int)aspBeta << "} ";
#endif // ROOT_NODE_DEBUG
				// 手を戻す。
				unmakeMove();
				if (isInterrupted()) {
					goto lab_search_end;
				}
				// fail-high
				if (vtemp < Value::MATE && vtemp >= aspBeta && aspBeta.next()) {
					// ウィンドウを広げたら再探索 (aspiration search)
					// TODO: fail-softだから上げ幅が少なすぎるケースを検出すべき?
#if ROOT_NODE_DEBUG
					Log::debug << "fail-high ";
#endif // ROOT_NODE_DEBUG
					// 最善手の評価値とalpha値を更新
					maxValue = alpha = vtemp - 1;
					// PVを更新
					tree.updatePv();
					goto revaluation;
				}
				// fail-low
				if (alpha == aspAlpha && vtemp <= aspAlpha && aspAlpha.next()) {
					// ウィンドウを広げたら再探索 (aspiration search)
					// TODO: fail-softだから下げ幅が少なすぎるケースを検出すべき?
#if ROOT_NODE_DEBUG
					Log::debug << "fail-low ";
#endif // ROOT_NODE_DEBUG
					alpha = (int)aspAlpha;
					goto revaluation;
				}

				// 最初の手かalphaを越えた場合
				if (moveCount == 1 || vtemp > alpha) {
					// 最善手の評価値とalpha値を更新
					maxValue = alpha = vtemp;
					// PVを更新
					tree.updatePv();
					// 評価値を記憶する。
					values[moveCount-1] = vtemp;
				} else {
					if (minValue > vtemp) {
						minValue = vtemp;
					}
					// 評価値を記憶する。
					values[moveCount-1] = minValue;
				}

				// 詰み
				if (vtemp >= Value::MATE) {
					goto lab_search_end;
				}
			}

			if (config.pvHandler != NULL) {
#if ROOT_NODE_DEBUG
				// debug
				Log::debug << '\n';
#endif // ROOT_NODE_DEBUG
				config.pvHandler->pvHandler(tree.getPv(), maxValue,
						counter.nodes, depth + 1, timer.elapsed());
			}

			// 詰み
			if (maxValue <= -Value::MATE) {
				break;
			}
		}
lab_search_end:

		if (config.pvHandler != NULL) {
#if ROOT_NODE_DEBUG
			// debug
			Log::debug << '\n';
#endif // ROOT_NODE_DEBUG
			int lastDepth = depth < config.depth ? depth + 1 : config.depth;
			config.pvHandler->pvHandler(tree.getPv(), maxValue,
					counter.nodes, lastDepth, timer.elapsed());
		}

		// 後処理
		return after(result, maxValue);
	}
}
