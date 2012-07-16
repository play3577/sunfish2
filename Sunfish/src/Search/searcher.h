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

namespace Search {
	struct SearchConfig {
		int depth;
		PvHandler* pvHandler;
	};

	struct SearchResult {
		bool noMoves;
		Shogi::Move move;
		Evaluate::Value value;
		Pv pv;
	};

	class Searcher {
	private:
		Tree tree;
		Table::TT tt;
		History history;
		SearchConfig config;
		static const int PLY1 = 4;

		template <bool pvNode, bool nullMoveNode>
		Evaluate::Value negaMax(Tree& tree,
				int depth,
				Evaluate::Value alpha,
				Evaluate::Value beta);

		void before(SearchResult& result) {
			memset(&result, 0, sizeof(SearchResult));
			tt.init();
		}

		bool after(SearchResult& result, Evaluate::Value value) {
			result.value = value;
			const Shogi::Move* pmove = tree.getPv().getTop();
			result.pv.copy(tree.getPv());
			if (pmove != NULL) {
				result.noMoves = false;
				result.move = *pmove;
				return true;
			} else {
				result.noMoves = true;
				return false;
			}
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
