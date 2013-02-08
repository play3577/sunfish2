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
#include "../Records/record.h"
#include "pvHandler.h"
#include "tree.h"
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
		static const int PLY1 = 4;

		Tree tree;
		Table::TT tt;
		Shek::ShekTable shekTable;

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
				for (int i = 0; i < hashStack.size; i++) {
					shekTable.set(hashStack.stack[i]);
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
				for (int i = hashStack.size - 1; i >= 0; i--) {
					shekTable.unset(hashStack.stack[i]);
				}
			}
			result.value = value;
			const Shogi::Move* pmove = tree.getPv().getTop();
			result.pv.copy(tree.getPv());
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
			} else if (tree.getPv().getTop() != NULL) {
				return signalInterrupt || (config.limitEnable
					&& timer.elapsed() >= config.limitSeconds);
			}
			return false;
		}

		Shek::ShekStat shekCheck() {
			return shekTable.check(tree.getPosition());
		}

		unsigned getShekCount() {
			return shekTable.getCount(tree.getPosition());
		}

		void shekDebug() {
			shekTable.debugPrint(tree.getPosition());
			
		}

		bool nullMove(bool shek = true) {
			if (shek) {
				shekTable.set(tree.getPosition());
			}
			if (tree.nullMove()) {
				return true;
			} else {
				if (shek) {
					shekTable.unset(tree.getPosition());
				}
				return false;
			}
		}

		void makeMove(bool shek = true) {
			if (shek) {
				shekTable.set(tree.getPosition());
			}
			tree.makeMove();
		}

		void unmakeMove(bool shek = true) {
			tree.unmakeMove();
			if (shek) {
				shekTable.unset(tree.getPosition());
			}
		}

	public:
		Searcher(const Evaluates::Param& param) :
			tree(param, history),
			hashStack(Records::HashStack::nan()),
			running(false),
			signalInterrupt(false),
			signalForceInterrupt(false) {
		}

		Searcher(const Evaluates::Param& param,
				const Shogi::Position& pos) :
			tree(param, pos, history),
			hashStack(Records::HashStack::nan()),
			running(false),
			signalInterrupt(false),
			signalForceInterrupt(false) {
		}

		void init(const Shogi::Position& pos,
				const Records::HashStack& hashStack) {
			tree.init(pos);
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
			tree.init(pos);
			return isMate1Ply(tree);
		}

		bool isMate1Ply() {
			return isMate1Ply(tree);
		}

		bool search(SearchResult& result,
				Evaluates::Value alpha = Evaluates::Value::MIN,
				Evaluates::Value beta = Evaluates::Value::MAX);

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
	};
}

#endif // SEARCHER_H_
