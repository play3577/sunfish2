/*
 * searcher.cpp
 *
 *  Created on: 2012/07/03
 *      Author: ryosuke
 */

#include "aspWindow.h"
#include "timeManager.h"
#include "searcher.h"
#include "../Util/tableString.h"
#include <sstream>

#define ROOT_NODE_DEBUG				0
#define NODE_DEBUG					0
#define VARIATION_DEBUG				0

namespace Search {
	using namespace Shogi;
	using namespace Evaluates;
	using namespace Table;

	std::string SearchResult::toString() const {
		Util::TableString table("* ", " :", "");
		table.row() << "VALUE" << value;
		for (unsigned i = 0; i < counters.size(); i++) {
			std::ostringstream oss;
			oss << "NODES(" << i << ")";
			table.row() << oss.str() << counters[i].nodes;
		}
		table.row() << "NODES(ALL)" << counter.nodes;
		table.row() << "SEC" << sec;
		table.row() << "NPS" << nps;
		table.row() << "HASH PRUNING" << counter.hashPruning;
		table.row() << "NULL MOVE PRUNING" << counter.nullMovePruning;
		table.row() << "FUTILITY PRUNING" << counter.futilityPruning;
		table.row() << "EXTENDED FUTILITY PRUNING" << counter.exFutilityPruning;
		table.row() << "SHEK PRUNING" << counter.shekPruning;
		if (!resign) {
			table.row() << "MOVE" << move.toString();
		}
		return table.get();
	}

	bool Searcher::interrupt() {
		boost::mutex::scoped_lock lock(flagMutex);
		if (running) {
			bool ret = !signalInterrupt;
			signalInterrupt = true;
			return ret;
		}
		return false;
	}

	bool Searcher::forceInterrupt() {
		boost::mutex::scoped_lock lock(flagMutex);
		if (running) {
			bool ret = !signalForceInterrupt;
			signalForceInterrupt = true;
			return ret;
		}
		return false;
	}

	void Searcher::before(SearchResult& result) {
		// 探索状態フラグ
		{
			boost::mutex::scoped_lock lock(flagMutex);
			running = true;
			signalInterrupt = false;
			signalForceInterrupt = false;
		}
		// SHEKテーブルのセット
		if (hashStack.stack != NULL) {
			for (int i = 0; i < treeSize; i++) {
				trees[i].shekSet(hashStack);
			}
		}
		// 初期化
		memset(&result, 0, sizeof(SearchResult));
		memset(&gain, 0, sizeof(gain));
		history.clear();
		tt.init();
		// 並列探索用
		idleTree = treeSize - 1;
		idleWorker = workerSize - 1;
		trees[0].use(0);
		for (int i = 1; i < treeSize; i++) {
			trees[i].unuse();
		}
		for (int i = 0; i < workerSize; i++) {
		}
		workers[0].startCurrentThread(0);
		workers[0].getCounter().init();
		for (int i = 1; i < workerSize; i++) {
			workers[i].start();
			workers[i].getCounter().init();
		}
		// 時間計測開始
		timer.set();
	}

	bool Searcher::after(SearchResult& result, Evaluates::Value value) {
		// 並列探索用
		for (int i = 1; i < workerSize; i++) {
			workers[i].stop();
		}
		// SHEKテーブルのアンセット
		if (hashStack.stack != NULL) {
			for (int i = 0; i < treeSize; i++) {
				trees[i].shekUnset(hashStack);
			}
		}
		// 結果のセット
		result.value = value;
		const Shogi::Move* pmove = trees[0].getPv().getTop();
		result.pv.copy(trees[0].getPv());
		result.sec = timer.get();
		SearchCounter counter;
		for (int i = 0; i < workerSize; i++) {
			result.counters.push_back(workers[i].getCounter());
			counter += workers[i].getCounter();
		}
		result.counter = counter;
		result.nps = result.counter.nodes / result.sec;
		bool ok;
		if (pmove != NULL) {
			result.resign = false;
			result.move = *pmove;
			ok = true;
		} else {
			result.resign = true;
			ok = false;
		}
		// 探索状態フラグ
		{
			boost::mutex::scoped_lock lock(flagMutex);
			running = false;
			signalInterrupt = false;
			signalForceInterrupt = false;
		}
		return ok;
	}

	/***************************************************************
	 * mate 1 ply search                                           *
	 * tree  : 探索木                                              *
	***************************************************************/
	bool Searcher::isMate1Ply(Tree& tree) {
		tree.generateCheck();

		while (tree.next()) {
			makeMove(tree, false);
			if (tree.isMate()) {
				unmakeMove(tree);
				return true;
			}
			unmakeMove(tree);
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
		Worker& worker = workers[tree.split.worker];
		SearchCounter& counter = worker.getCounter();

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
			makeMove(tree, false);
			Value newValue = -quies(tree, ply+1, -beta, -newAlpha);
			unmakeMove(tree);
			if (isInterrupted(tree)) { return Value(0); }

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
		Worker& worker = workers[tree.split.worker];
		SearchCounter& counter = worker.getCounter();

#if NODE_DEBUG
		bool debugNode = false;
		//if (tree.is("+2726FU -2255KA")) {
		if (tree.is("-6171KI")) {
			Log::debug << " ***** {" << alpha << ',' << beta << '}';
			debugNode = true;
		}
#endif // NODE_DEBUG
#if 0
		if (tree.is("-2332GI")) {
			tree.shekDebug();
		}
#endif

#if VARIATION_DEBUG
		tree.debugPrint();
#endif

		// TODO: distance pruning

#ifndef NLEARN
		if (!config.isLearning)
#endif // NLEARN
		{
			switch (shekCheck(tree)) {
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
		TTEntity tte = tt.getEntity(hash);
		Move hash1;
		Move hash2;
		bool hashOk = false;
		if (tte.is(hash)) { // 局面が一致したら
			if (tte.isSuperior(depth)
#ifndef NLEARN
					&& !config.isLearning
#endif // NLEARN
					) {
				switch (tte.getValueType()) {
				case TTEntity::EXACT: // 確定
					if (!pvNode && stat.isHashCut()) {
						counter.hashPruning++;
						return tte.getValue();
					}
				case TTEntity::LOWER: // 下界値
					if (!pvNode && stat.isHashCut() && tte.getValue() >= beta) {
						counter.hashPruning++;
						return tte.getValue();
					}
					break;
				case TTEntity::UPPER: // 上界値
					if (!pvNode && stat.isHashCut() && tte.getValue() <= alpha) {
						counter.hashPruning++;
						return tte.getValue();
					}
					break;
				}
			}
			if (tte.getDepth() >= depth - PLY1 * 3) {
				tree.setHashMove(tte.getHashMove());
				hashOk = true;
#if NODE_DEBUG
				if (debugNode) {
					Log::warning << "hash=" << tte.getHashMove().getHash1().toString();
					Log::warning << ' ' << tte.getHashMove().getHash2().toString();
				}
#endif // NODE_DEBUG
			}
		}

		// stand-pat
		Value standPat = tree.negaEvaluate();

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
					beta <= standPat){
				int nullDepth = 
						(depth >= PLY1*15/2 ? depth - PLY1*4 :
						(depth >= PLY1*9/2 ? depth - PLY1*3 : depth - PLY1*2));
				if (nullMove(tree, false)) {
					Value newValue = -negaMax<false>(tree, nullDepth, -beta, -beta+1, NodeStat().unsetNullMove());
					unmakeMove(tree);
					if (isInterrupted(tree)) { return Value(0); }
					if (newValue >= beta) {
						counter.nullMovePruning++;
						// TT entry
						if (nullDepth < PLY1) {
							tt.entry(hash, alpha, beta, newValue, depth, stat, Move());
						}
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
			if (isInterrupted(tree)) { return Value(0); }
			TTEntity tte = tt.getEntity(hash);
			if (tte.is(hash)) {
				tree.setHashMove(tte.getHashMove());
			}
		}

		// initialize move generator
		tree.generateMoves();

		Value value = Value::MIN;
		Move best;
		while (tree.next()) {
			assert(tree.getCurrentMove() != NULL);
#if NODE_DEBUG
			if (debugNode) {
				Log::debug << ' ' << tree.getCurrentMove()->toString();
			}
#endif // NODE_DEBUG

			Value newAlpha = Value::max(alpha, value);

			NodeController node(*this, tree, tree, rootDepth, stat,
					depth - PLY1, newAlpha, standPat,
					beta == alpha + 1, mate);
			node.execute();

			if (node.isPruning()) {
				value = newAlpha; // fail soft
				counter.futilityPruning++;
				continue;
			}

			// make move
			Value newValue;
			makeMove(tree);

			// new stand-pat
			node.setNewStandPat(tree.negaEvaluate());
			node.executeInterior();

			if (node.isPruning()) {
				unmakeMove(tree);
				value = newAlpha; // fail soft
				counter.exFutilityPruning++;
				continue;
			}

			updateGain(node.getMove(),
					standPat + node.getEstimate().getValue(),
					node.getNewStandPat());

#if NODE_DEBUG
			Util::uint64 beforeNodes = counter.nodes;
			double beforeSec = timer.get();
#endif // NODE_DEBUG
			// recurcive search
			if (node.getMoveCount() == 1) {
				newValue = -negaMax<pvNode>(tree, node.getDepth(), -beta, -newAlpha, node.getStat());
#if NODE_DEBUG
				if (debugNode) {
					Log::debug << ',' << __LINE__;
				}
#endif // NODE_DEBUG
			} else {
				// nega-scout
				newValue = -negaMax<false>(tree, node.getDepth(), -newAlpha-1, -newAlpha, node.getStat());
#if NODE_DEBUG
				if (debugNode) {
					Log::debug << ',' << __LINE__;
				}
#endif // NODE_DEBUG
				if (!isInterrupted(tree) && newValue > newAlpha
						&& node.isReduced() != 0) {
					// reductionをなくして再探索
					newValue = -negaMax<false>(tree,
							node.getDepth(false),
							-newAlpha-1, -newAlpha, node.getStat());
#if NODE_DEBUG
					if (debugNode) {
						Log::debug << ',' << __LINE__;
					}
#endif // NODE_DEBUG
				}
				if (!isInterrupted(tree) && newValue > newAlpha && !node.isNullWindow()) {
					// windowを広げて再探索
					newValue = -negaMax<pvNode>(tree,
							node.getDepth(false),
							-beta, -newAlpha, node.getStat());
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
				double afterSec = timer.get();
				double sec = afterSec - beforeSec;
				Log::debug << "(" << newValue << ")[" << nodes << "]";
				Log::warning << (int)(sec*1000);
			}
#endif // NODE_DEBUG

			// unmake move
			unmakeMove(tree);

			if (isInterrupted(tree)) {
				return Value(0);
			}

			if (newValue > value) {
				value = newValue;
				tree.updatePv();
				best = node.getMove();

				// beta cut
				if (value >= beta) {
					if (!node.isCapture()) {
						tree.addHistory(depth);
						tree.addKiller(newValue - standPat);
					} else if (!node.isHash()) {
						tree.addKiller(newValue - standPat);
					}
#if NODE_DEBUG
					if (debugNode) {
						Log::debug << ";bc";
					}
#endif // NODE_DEBUG
					break;
				}
			}

			if (workerSize >= 2 &&
					(depth >= 6 || (!node.isNullWindow() && (depth >= PLY1 * 4 ||
							(depth >= PLY1 * 3 && rootDepth <= 10)))) &&
					(!tree.isCheck() || tree.getNumberOfMoves() >= 4)) {
				if (split(tree, depth, alpha, beta, value, stat, standPat, mate, pvNode)) {
					if (isInterrupted(tree)) {
						return Value(0);
					}
					if (tree.split.value > value) {
						value = tree.split.value;
						best = tree.split.best;
					}
					break;
				}
			}
		}

		// there is no legal moves
		if (tree.getMoveIndex() == 0) {
			return Value::MIN + tree.getDepth();
		}

#if NODE_DEBUG
		if (debugNode) {
			Log::warning << ' ' << best.toString();
		}
#endif // NODE_DEBUG
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
		Tree& tree = trees[0];

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
		Tree& tree = trees[0];
		Worker& worker = workers[tree.split.worker];
		SearchCounter& counter = worker.getCounter();

		// 最善手の評価値
		Value maxValue = Value::MIN;
		// 各指し手毎の評価値
		Value values[MoveGenerator::MAX_MOVES_NUM];

		// 前処理
		before(result);

		Value standPat = tree.negaEvaluate();

		// ノード初期化
		tree.initNode();
		// 合法手生成
		tree.generateMoves();
		// TODO: 指し手がない場合
		// TODO: 確定手
		// 反復進化探索
		TimeManager tm(config.limitEnable, config.limitSeconds);
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
				bool doNullWind = true;
				unsigned moveCount = tree.getMoveIndex();
#if ROOT_NODE_DEBUG
				// debug
				Util::uint64 beforeNodes= counter.nodes;
				double beforeSec = timer.get();
				//Log::debug << '<' << tree.getNumberOfMoves() << ',' << moveCount << '>';
#endif // ROOT_NODE_DEBUG

				NodeController node(*this, tree, tree, rootDepth,
						NodeStat(), depth * PLY1, alpha,
						standPat, false, false);
				node.execute(true);

revaluation:
				// 手を進める。
				makeMove(tree);
				Value vtemp;
				if (moveCount == 1) {
					// 前回の最善手
					vtemp = -negaMax<true>(tree,
							depth * PLY1, -aspBeta, -alpha);
#if ROOT_NODE_DEBUG
					Log::debug << "f";
#endif // ROOT_NODE_DEBUG
				} else {
					if (doNullWind) {
						// null window searchを試みる。
						vtemp = -negaMax<false>(tree,
								node.getDepth(), -alpha - 1, -alpha);
						if (!isInterrupted(tree) && vtemp > alpha && node.isReduced()) {
							vtemp = -negaMax<false>(tree,
									node.getDepth(false), -alpha - 1, -alpha);
						}
#if ROOT_NODE_DEBUG
						Log::debug << "n";
#endif // ROOT_NODE_DEBUG
					}
					if (!doNullWind || (!isInterrupted(tree) && vtemp > alpha && vtemp < Value::MATE)) {
#if ROOT_NODE_DEBUG
						Log::debug << "(" << vtemp << ")";
#endif // ROOT_NODE_DEBUG
						// 再探索
						vtemp = -negaMax<true>(tree,
								node.getDepth(false), -aspBeta, -alpha,
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
				double afterSec = timer.get();
				double sec = afterSec - beforeSec;
				Log::debug << tree.getPrevMove()->toString() << "(" << vtemp << ")"
						<< "[" << nodes << "]"
						<< "{" << (int)alpha << "," << (int)aspBeta << "}";
				Log::warning << (int)(sec*1000);
#endif // ROOT_NODE_DEBUG
				// 手を戻す。
				unmakeMove(tree);
				if (isInterrupted(tree)) {
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
					doNullWind = false;
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

				// 時間制御
				tm.value(vtemp);
			}

			if (config.pvHandler != NULL) {
#if ROOT_NODE_DEBUG
				// debug
				Log::debug << '\n';
#endif // ROOT_NODE_DEBUG
				config.pvHandler->pvHandler(tree.getPv(), maxValue,
						counter.nodes, depth + 1, timer.get());
			}

			// 詰み
			if (maxValue <= -Value::MATE) {
				break;
			}

			// 時間制御
			if (tm.closure(timer.get())) {
				break;
			}
			tm.next();
		}
lab_search_end:

		if (config.pvHandler != NULL) {
#if ROOT_NODE_DEBUG
			// debug
			Log::debug << '\n';
#endif // ROOT_NODE_DEBUG
			int lastDepth = depth < config.depth ? depth + 1 : config.depth;
			config.pvHandler->pvHandler(tree.getPv(), maxValue,
					counter.nodes, lastDepth, timer.get());
		}

		// 後処理
		return after(result, maxValue);
	}
}
