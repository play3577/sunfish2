/*
 * searcher.h
 *
 *  Created on: 2012/07/03
 *      Author: ryosuke
 */

#ifndef SEARCHER_H_
#define SEARCHER_H_

#include "../Util/tableString.h"
#include "../Table/tt.h"
#include "../Shek/shekTable.h"
#include "pvHandler.h"
#include "tree.h"
#include "worker.h"
#include <algorithm>
#include <boost/timer.hpp>
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>

namespace Search {
	struct SearchConfig {
		unsigned depth;
		PvHandler* pvHandler;
		bool limitEnable;
		double limitSeconds;
#ifndef NLEARN
		bool isLearning;
#endif // NLEARN
		static SearchConfig getDefault() {
			SearchConfig config = {32, NULL, false, .0
#ifndef NLEARN
				, false
#endif // NLEARN
				};
			return config;
		}
	};

	struct SearchCounter {
		Util::uint64 nodes;
		Util::uint64 hashPruning;
		Util::uint64 nullMovePruning;
		Util::uint64 futilityPruning;
		Util::uint64 exFutilityPruning;
		Util::uint64 shekPruning;
	};

	struct SearchResult {
		bool resign;
		Shogi::Move move;
		Pv pv;
		Evaluates::Value value;
		SearchCounter counter;
		double sec;
		double nps;

		std::string toString() const {
			Util::TableString table("* ", " :", "");
			table.row() << "VALUE" << value;
			table.row() << "NODES" << counter.nodes;
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
	};

	class Searcher {
	private:
		static const int DEFAULT_WORKER_SIZE = 1;
		Tree* trees;
		int treeSize;
		Worker* workers;
		int workerSize;
		boost::mutex splitMutex;

		Table::TT tt;
		History history;
		SearchConfig config;
		SearchCounter counter;
		boost::timer timer;
		int rootDepth;
		Evaluates::Value gain[Shogi::Piece::WDRAGON+1][Shogi::Square::END+1];
		Records::HashStack hashStack;

		//flags 
		bool running;
		bool signalInterrupt;
		bool signalForceInterrupt;
		boost::mutex flagMutex;

		Evaluates::Value quies(Tree& tree, int ply,
				Evaluates::Value alpha,
				Evaluates::Value beta);

		bool isMate1Ply(Tree& tree);

		template <bool pvNode>
		Evaluates::Value negaMax(Tree& tree, int depth,
				Evaluates::Value alpha,
				Evaluates::Value beta,
				NodeStat stat = NodeStat());

		void before(SearchResult& result) {
			{
				boost::mutex::scoped_lock lock(flagMutex);
				running = true;
				signalInterrupt = false;
				signalForceInterrupt = false;
			}
			if (hashStack.stack != NULL) {
				for (int i = 0; i < treeSize; i++) {
					trees[i].shekSet(hashStack);
				}
			}
			memset(&counter, 0, sizeof(SearchCounter));
			memset(&result, 0, sizeof(SearchResult));
			memset(&gain, 0, sizeof(gain));
			history.clear();
			tt.init();
			timer.restart();
		}

		bool after(SearchResult& result, Evaluates::Value value) {
			if (hashStack.stack != NULL) {
				for (int i = 0; i < treeSize; i++) {
					trees[i].shekUnset(hashStack);
				}
			}
			result.value = value;
			const Shogi::Move* pmove = trees[0].getPv().getTop();
			result.pv.copy(trees[0].getPv());
			result.counter = counter;
			result.sec = timer.elapsed();
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
			{
				boost::mutex::scoped_lock lock(flagMutex);
				running = false;
				signalInterrupt = false;
				signalForceInterrupt = false;
			}
			return ok;
		}

		// TODO:
		static Evaluates::Value getFutMgn(int depth, int count) {
			if (depth < PLY1) {
				return 0;
			}
			return 120 * depth / PLY1 + 4 * count;
		}

		void updateGain(const Search::Tree& tree,
				const Evaluates::Value& before, const Evaluates::Value& after) {
			const Shogi::Move* pmove = tree.getCurrentMove();
			if (pmove != NULL) {
				updateGain(pmove->getPiece(), pmove->getTo(), before, after);
			}
		}

		void updateGain(const Shogi::Piece& piece, const Shogi::Square& square,
				const Evaluates::Value& before, const Evaluates::Value& after) {
			gain[piece][square] = std::max(-(before + after), gain[piece][square] - 1);
		}

		Evaluates::Value getGain(const Search::Tree& tree) const {
			const Shogi::Move* pmove = tree.getCurrentMove();
			if (pmove != NULL) {
				return getGain(pmove->getPiece(), pmove->getTo());
			}
			return Evaluates::Value(0);
		}

		Evaluates::Value getGain(const Shogi::Piece& piece, const Shogi::Square& square) const {
			assert((unsigned)piece <= Shogi::Piece::WDRAGON);
			assert((unsigned)square <= Shogi::Square::END);
			return gain[piece][square];
		}

		// TODO:
		int extension(Tree& tree) const {
			if (tree.getDepth() < rootDepth) {
				return PLY1;
			} else if (tree.getDepth() < rootDepth * 2) {
				return PLY1 * 3 / 4;
			} else if (tree.getDepth() < rootDepth * 3) {
				return PLY1 / 2;
			} else {
				return PLY1 / 4;
			}
		}

		bool isInterrupted() const {
			if (signalForceInterrupt) {
				return true;
			} else if (trees[0].getPv().getTop() != NULL) {
				return signalInterrupt || (config.limitEnable
					&& timer.elapsed() >= config.limitSeconds);
			}
			return false;
		}

		Shek::ShekStat shekCheck(Tree& tree) const {
			return tree.shekCheck();
		}

		unsigned getShekCount(Tree& tree) const {
			return tree.getShekCount();
		}

		void shekDebug(Tree& tree) const {
			tree.shekDebug();
		}

		bool nullMove(Tree& tree, bool shek = true) {
			if (shek) {
				tree.shekSet();
			}
			if (tree.nullMove()) {
				return true;
			} else {
				if (shek) {
					tree.shekUnset();
				}
				return false;
			}
		}

		void makeMove(Tree& tree, bool shek = true) {
			if (shek) {
				tree.shekSet();
			}
			tree.makeMove();
		}

		void unmakeMove(Tree& tree, bool shek = true) {
			tree.unmakeMove();
			if (shek) {
				tree.shekUnset();
			}
		}

		static int genTreeSize(int workerSize, int treeSize) {
			return treeSize > 0 ? treeSize : (workerSize * 4 - 3);
		}

		void buildWorkers() {
			if (workerSize >= 1) {
				workers = new Worker[workerSize];
				for (int i = 0; i < workerSize; i++) {
					workers[i].init(this, i);
				}
			}
		}

		void buildInstances(const Evaluates::Param& param,
				History& history) {
			trees = (Tree*)new char[sizeof(Tree) * treeSize];
			for (int i = 0; i < treeSize; i++) {
				new (trees + i) Tree(param, history);
			}
			buildWorkers();
		}

		void buildInstances(const Evaluates::Param& param,
				const Shogi::Position& pos, History& history) {
			trees = (Tree*)new char[sizeof(Tree) * treeSize];
			for (int i = 0; i < treeSize; i++) {
				new (trees + i) Tree(param, pos, history);
			}
			buildWorkers();
		}

		void releaseInstances() {
			for (int i = 0; i < treeSize; i++) {
				trees[i].~Tree();
			}
			delete [] (char*)trees;
			if (workerSize >= 1) {
				delete [] workers;
			}
		}

	public:
		static const int PLY1 = 4;

		Searcher(const Evaluates::Param& param,
				int workerSize = DEFAULT_WORKER_SIZE,
				int treeSize = 0) :
				treeSize(genTreeSize(workerSize, treeSize)),
				workerSize(workerSize - 1),
				hashStack(Records::HashStack::nan()),
				running(false),
				signalInterrupt(false),
				signalForceInterrupt(false) {
			buildInstances(param, history);
		}

		Searcher(const Evaluates::Param& param,
				const Shogi::Position& pos,
				int workerSize = DEFAULT_WORKER_SIZE,
				int treeSize = 0) :
				treeSize(genTreeSize(workerSize, treeSize)),
				workerSize(workerSize - 1),
				hashStack(Records::HashStack::nan()),
				running(false),
				signalInterrupt(false),
				signalForceInterrupt(false) {
			buildInstances(param, pos, history);
		}

		virtual ~Searcher() {
			releaseInstances();
		}

		void init(const Shogi::Position& pos,
				const Records::HashStack& hashStack) {
			for (int i = 0; i < treeSize; i++) {
				trees[i].init(pos);
			}
			this->hashStack = hashStack;
		}

		void init(const Records::Record& record) {
			init(record.getPosition(), record.getHashStack());
		}

		const SearchConfig& getSearchConfig() const {
			return config;
		}

		void setSearchConfig(const SearchConfig& config) {
			this->config = config;
		}

		bool isMate1Ply(const Shogi::Position& pos) {
			trees[0].init(pos);
			return isMate1Ply(trees[0]);
		}

		bool isMate1Ply() {
			return isMate1Ply(trees[0]);
		}

		bool search(SearchResult& result,
				Evaluates::Value alpha = Evaluates::Value::MIN,
				Evaluates::Value beta = Evaluates::Value::MAX);

		boost::mutex& getSplitMutex() {
			return splitMutex;
		}

		// iterative-deepening search
		bool idSearch(SearchResult& result);

		bool interrupt() {
			boost::mutex::scoped_lock lock(flagMutex);
			if (running) {
				bool ret = !signalInterrupt;
				signalInterrupt = true;
				return ret;
			}
			return false;
		}

		bool forceInterrupt() {
			boost::mutex::scoped_lock lock(flagMutex);
			if (running) {
				bool ret = !signalForceInterrupt;
				signalForceInterrupt = true;
				return ret;
			}
			return false;
		}

		bool isRunning() {
			boost::mutex::scoped_lock lock(flagMutex);
			return running;
		}

	private:
		class NodeController {
		private:
			Searcher& searcher;
			Tree& tree;
			const int rootDepth;
			const NodeStat stat;
			NodeStat newStat;
			int depth;
			const Evaluates::Value alpha;
			const Evaluates::Value standPat;
			const Evaluates::Estimate estimate;
			int reduction;
			bool pruning;
			const unsigned moveCount;
			const bool _isNullWindow;
			const bool _isMateThreat;
			const bool _isHash;
			const bool _isCheckMove;
			const bool _isTacticalMove;
			const bool _isRecapture;

			template <bool isRoot>
			void execute();

			int extension() const;

			static Evaluates::Value getFutMgn(int depth, int count);

		public:
			NodeController(Searcher& searcher, Tree& tree,
					int rootDepth, const NodeStat& stat, int depth,
					Evaluates::Value alpha, Evaluates::Value standPat,
					bool isNullWindow, bool isMateThreat) :
					searcher(searcher), tree(tree),
					rootDepth(rootDepth), stat(stat), depth(depth),
					alpha(alpha), standPat(standPat),
					estimate(tree.negaEstimate()), reduction(0),
					pruning(false), moveCount(tree.getMoveIndex()),
					_isNullWindow(isNullWindow),
					_isMateThreat(isMateThreat),
					_isHash(tree.isHashMove()),
					_isCheckMove(tree.isCheckMove()),
					_isTacticalMove(tree.isTacticalMove()),
					_isRecapture(tree.isRecapture()) {
			}

			void execute(bool isRoot = false) {
				if (isRoot) {
					execute<true>();
				} else {
					execute<false>();
				}
			}

			bool isPruning() const {
				return pruning;
			}

			int getDepth(bool reduced = true) const {
				return depth - (reduced ? reduction : 0);
			}

			int getReduction() const {
				return reduction;
			}

			bool isReduced() const {
				return reduction != 0;
			}

			bool isNullWindow() const {
				return _isNullWindow;
			}

			bool isCheckMove() const {
				return _isCheckMove;
			}

			bool isTacticalMove() const {
				return _isTacticalMove;
			}

			bool isRecapture() const {
				return _isRecapture;
			}

			bool isMateThreat() const {
				return _isMateThreat;
			}

			bool isHash () const {
				return _isHash;
			}

			unsigned getMoveCount() const {
				return moveCount;
			}

			Evaluates::Value getStandPat() const {
				return standPat;
			}

			NodeStat getStat() const {
				return newStat;
			}

			const Evaluates::Estimate& getEstimate() const {
				return estimate;
			}
		};
	};
}

#endif // SEARCHER_H_
