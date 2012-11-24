/*
 * searcher.h
 *
 *  Created on: 2012/07/03
 *      Author: ryosuke
 */

#ifndef SEARCHER_H_
#define SEARCHER_H_

#include "tree.h"
#include "pvHandler.h"
#include "../Util/tableString.h"
#include "../Table/tt.h"
#include "../Shek/shekTable.h"
#include <algorithm>
#include <boost/timer.hpp>

namespace Search {
	struct SearchConfig {
		unsigned depth;
		PvHandler* pvHandler;
		bool limitEnable;
		double limitSeconds;
	};

	struct SearchCounter {
		Util::uint64 nodes;
		Util::uint64 nullMovePruning;
		Util::uint64 futilityPruning;
		Util::uint64 exFutilityPruning;
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
			table.row() << "NULL MOVE PRUNING" << counter.nullMovePruning;
			table.row() << "FUTILITY PRUNING" << counter.futilityPruning;
			table.row() << "EXTENDED FUTILITY PRUNING" << counter.exFutilityPruning;
			table.row() << "SEC" << sec;
			table.row() << "NPS" << nps;
			if (!resign) {
				table.row() << "MOVE" << move.toString();
			}
			return table.get();
		}
	};

	class Searcher {
	private:
		Tree tree;
		Table::TT tt;
		Shek::ShekTable shekTable;
		History history;
		SearchConfig config;
		static const int PLY1 = 4;
		SearchCounter counter;
		boost::timer timer;
		const bool* pinterrupt;
		int rootDepth;
		Evaluates::Value gain[Shogi::Piece::WDRAGON+1][Shogi::Square::END+1];

		Evaluates::Value quies(Tree& tree, int ply,
				Evaluates::Value alpha,
				Evaluates::Value beta);

		bool isMate1Ply(Tree& tree);

		template <bool pvNode, bool nullMoveNode>
		Evaluates::Value negaMax(Tree& tree, int depth,
				Evaluates::Value alpha,
				Evaluates::Value beta);

		void before(SearchResult& result) {
			// TODO: SHEK
			memset(&counter, 0, sizeof(SearchCounter));
			memset(&result, 0, sizeof(SearchResult));
			memset(&gain, 0, sizeof(gain));
			tt.init();
			timer.restart();
		}

		bool after(SearchResult& result, Evaluates::Value value) {
			// TODO: SHEK
			result.value = value;
			const Shogi::Move* pmove = tree.getPv().getTop();
			result.pv.copy(tree.getPv());
			result.counter = counter;
			result.sec = timer.elapsed();
			result.nps = result.counter.nodes / result.sec;
			if (pmove != NULL) {
				result.resign = false;
				result.move = *pmove;
				return true;
			} else {
				result.resign = true;
				return false;
			}
		}

		static Evaluates::Value getFutMgn(int depth, int count) {
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

		Evaluates::Value getGain(Shogi::Piece piece, Shogi::Square square) const {
			assert((unsigned)piece <= Shogi::Piece::WDRAGON);
			assert((unsigned)square <= Shogi::Square::END);
			return gain[piece][square];
		}

		int extension(Tree& tree) const {
			if (tree.getDepth() < rootDepth) {
				return PLY1;
			} else if (tree.getDepth() < rootDepth * 3 / 2) {
				return PLY1 * 3 / 4;
			} else if (tree.getDepth() < rootDepth * 2) {
				return PLY1 / 2;
			} else {
				return PLY1 / 4;
			}
		}

		bool interrupt() const {
			return tree.getPv().getTop() != NULL &&
					config.limitEnable &&
					timer.elapsed() >= config.limitSeconds;
		}

		Shek::ShekStat shekCheck() {
			return shekTable.check(tree.getPosition());
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
			tree(param, history) {
		}

		Searcher(const Evaluates::Param& param,
				const Shogi::Position& pos) :
			tree(param, pos, history) {
		}

		void init(const Shogi::Position& pos) {
			tree.init(pos);
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

		bool search(SearchResult& result);

		// iterative-deepening search
		bool idSearch(SearchResult& result);
	};
}

#endif // SEARCHER_H_
