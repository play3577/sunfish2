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
#include <algorithm>
#include <boost/timer.hpp>

namespace Search {
	struct SearchConfig {
		unsigned depth;
		PvHandler* pvHandler;
		bool limitEnable;
		double limitSeconds;
	};

	struct SearchResult {
		bool resign;
		Shogi::Move move;
		Pv pv;
		Evaluates::Value value;
		Util::uint64 nodes;
		double sec;
		double nps;

		std::string toString() const {
			std::ostringstream oss;
			oss << "* VALUE :" << value << '\n';
			oss << "* NODES :" << nodes << '\n';
			oss << "* SEC   :" << sec << '\n';
			oss << "* NPS   :" << nps << '\n';
			if (!resign) {
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
		const bool* pinterrupt;
		int rootDepth;
		Evaluates::Value gain[Shogi::Piece::WDRAGON+1][Shogi::Square::END+1];

		Evaluates::Value quies(Tree& tree, int ply,
				Evaluates::Value alpha,
				Evaluates::Value beta);

		template <bool pvNode, bool nullMoveNode>
		Evaluates::Value negaMax(Tree& tree, int depth,
				Evaluates::Value alpha,
				Evaluates::Value beta);

		void before(SearchResult& result) {
			cntNodes = 0;
			memset(&result, 0, sizeof(SearchResult));
			memset(&gain, 0, sizeof(gain));
			tt.init();
			timer.restart();
		}

		bool after(SearchResult& result, Evaluates::Value value) {
			result.value = value;
			const Shogi::Move* pmove = tree.getPv().getTop();
			result.pv.copy(tree.getPv());
			result.nodes = cntNodes;
			result.sec = timer.elapsed();
			result.nps = result.nodes / result.sec;
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

		bool search(SearchResult& result);

		// iterative-deepening search
		bool idSearch(SearchResult& result);
	};
}

#endif // SEARCHER_H_
