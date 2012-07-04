/*
 * searcher.h
 *
 *  Created on: 2012/07/03
 *      Author: ryosuke
 */

#ifndef SEARCHER_H_
#define SEARCHER_H_

#include "tree.h"

namespace Search {
	struct SearchConfig {
		int depth;
	};

	struct SearchResult {
		Shogi::Move move;
		Evaluate::Value value;
		Pv pv;
	};

	class Searcher {
	private:
		Tree tree;
		SearchConfig config;

		template <bool root>
		Evaluate::Value negaMax(Tree& tree,
				int depth,
				Evaluate::Value alpha,
				Evaluate::Value beta,
				Shogi::Move* pmove);

	public:
		Searcher(const Evaluate::Param& param) :
			tree(param) {
		}

		Searcher(const Evaluate::Param& param,
				const Shogi::Position& pos) :
			tree(param, pos) {
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
	};
}

#endif // SEARCHER_H_
