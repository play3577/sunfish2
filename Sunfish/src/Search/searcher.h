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
#include "../Table/tt.h"
#include <boost/timer.hpp>

namespace Search {
	struct SearchConfig {
		int depth;
		PvHandler* pvHandler;
	};

	struct SearchResult {
		bool noMoves;
		Shogi::Move move;
		Pv pv;
		Evaluate::Value value;
		Util::uint64 nodes;
		double sec;
		double nps;

		std::string toString() const {
			std::ostringstream oss;
			oss << "* VALUE :" << value << '\n';
			oss << "* NODES :" << nodes << '\n';
			oss << "* SEC   :" << sec << '\n';
			oss << "* NPS   :" << nps << '\n';
			if (!noMoves) {
				oss << "* " << move.toString() << '\n';
			}
			return oss.str();
		}
	};

	class Searcher {
	private:
		Tree tree;
		Table::TT tt;
		History history;
		SearchConfig config;
		static const int PLY1 = 4;
		Util::uint64 cntNodes;
		boost::timer timer;

		Evaluate::Value quies(Tree& tree, int ply,
				Evaluate::Value alpha,
				Evaluate::Value beta);

		template <bool pvNode, bool nullMoveNode>
		Evaluate::Value negaMax(Tree& tree, int depth,
				Evaluate::Value alpha,
				Evaluate::Value beta);

		void before(SearchResult& result) {
			cntNodes = 0;
			memset(&result, 0, sizeof(SearchResult));
			tt.init();
			timer.restart();
		}

		bool after(SearchResult& result, Evaluate::Value value) {
			result.value = value;
			const Shogi::Move* pmove = tree.getPv().getTop();
			result.pv.copy(tree.getPv());
			result.nodes = cntNodes;
			result.sec = timer.elapsed();
			result.nps = result.nodes / result.sec;
			if (pmove != NULL) {
				result.noMoves = false;
				result.move = *pmove;
				return true;
			} else {
				result.noMoves = true;
				return false;
			}
		}

		static Value getFutMgn(int depth, int count) {
			return 600 / PLY1 * depth + 10 * count;
		}

	public:
		Searcher(const Evaluate::Param& param) :
			tree(param, history) {
		}

		Searcher(const Evaluate::Param& param,
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

		bool search(SearchResult& result);

		// iterative-deepening search
		bool idSearch(SearchResult& result);
	};
}

#endif // SEARCHER_H_
