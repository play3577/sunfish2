/*
 * tree.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef TREE_H_
#define TREE_H_

#include "../Evaluates/evaluate.h"
#include "node.h"
#include <boost/algorithm/string.hpp>

namespace Search {
	class Tree {
	private:
		Shogi::Position pos;
		Evaluates::Evaluate eval;
		const History& history;
		Node* nodes;
		int depth;
		int maxDepth;

	public:
		static const int DEF_MAX_DEPTH = 64;

		Tree(const Evaluates::Param& param,
				const History& history,
				int maxDepth = DEF_MAX_DEPTH) :
				eval(param), history(history),
				nodes(NULL) {
			init(maxDepth);
		}

		Tree(const Evaluates::Param& param,
				const Shogi::Position& pos,
				const History& history,
				int maxDepth = DEF_MAX_DEPTH) :
				pos(pos), eval(param),
				history(history), nodes(NULL) {
			eval.init(pos);
			init(maxDepth);
		}

		virtual ~Tree() {
			if (nodes != NULL) {
				delete[] nodes;
			}
		}

		int getDepth() const {
			return depth;
		}

		int getMaxDepth() const {
			return maxDepth;
		}

		bool isMaxDepth() {
			return depth == maxDepth;
		}

		void init(int maxDepth = DEF_MAX_DEPTH) {
			if (nodes != NULL) {
				delete[] nodes;
			}
			this->maxDepth = maxDepth;
			nodes = new Node[maxDepth+1];
			for (int i = 0; i < maxDepth; i++) {
				nodes[i].init(pos, eval.getParam(), history);
			}
			depth = 0;
		}

		void init(const Shogi::Position& pos, int maxDepth = DEF_MAX_DEPTH) {
			this->pos.copy(pos);
			eval.init(this->pos);
			init(maxDepth);
		}

		void initNode() {
			nodes[depth].arrive();
			nodes[depth+1].initKiller();
		}

		int updatePv() {
			return nodes[depth].setPv(nodes[depth+1]);
		}

		const Pv& getPv() const {
			return nodes[depth].getPv();
		}

		void generateMoves() {
			nodes[depth].generateMoves();
		}

		void generateTacticalMoves() {
			nodes[depth].generateTacticalMoves();
		}

		void generateCaptures() {
			nodes[depth].generateCaptures();
		}

		void generateCheck() {
			nodes[depth].generateCheck();
		}

		bool next() {
			return nodes[depth].next();
		}

		bool makeMove() {
			if (depth < maxDepth) {
				nodes[depth++].makeMove(pos, eval);
				return true;
			}
			return false;
		}

		bool nullMove() {
			if (depth < maxDepth && nodes[depth].nullMove(pos, eval)) {
				depth++;
				return true;
			}
			return false;
		}

		void unmakeMove() {
			nodes[--depth].unmakeMove(pos, eval);
		}

		const Shogi::Move* getPrevMove() const {
			return depth > 0 ? nodes[depth-1].getMove() : NULL;
		}

		const Shogi::Move* getCurrentMove() const {
			return nodes[depth].getMove();
		}

		void debugPrint() const {
			Log::debug << debugString() << "\n";
		}

		std::string debugString() const {
			std::ostringstream oss;
			for (int i = 0; i < depth; i++) {
				const Shogi::Move* pmove = nodes[i].getMove();
				if (pmove != NULL) {
					oss << pmove->toString() << ' ';
				} else {
					oss << "null ";
				}
			}
			return oss.str();
		}

		const Shogi::Change* getChange() const {
			return depth > 0 ? &nodes[depth-1].getChange() : NULL;
		}

		const Shogi::Position& getPosition() const {
			return pos;
		}

		bool isMate() const {
			return pos.isMate();
		}

		bool isCheck() const {
			return pos.isCheck();
		}

		bool isCheckMove() const {
			const Shogi::Move* pmove = getCurrentMove();
			if (pmove != NULL) {
				return pos.isCheckMove(*pmove);
			}
			return false;
		}

		bool isCapture() const {
			const Shogi::Move* pmove = getCurrentMove();
			if (pmove != NULL) {
				return pos.isCapturingMove(*pmove);
			}
			return false;
		}

		bool isRecapture() const {
			const Shogi::Move* pmove;
			const Shogi::Move* pprev;
			if ((pmove = getCurrentMove()) != NULL &&
					(pprev = getPrevMove()) != NULL) {
				return pmove->getTo() == pprev->getTo();
			}
			return false;
		}

		bool isTacticalMove() const {
			const Shogi::Move* pmove = getCurrentMove();
			if (pmove != NULL) {
				return pmove->isPromotion() || pos.isCapturingMove(*pmove);
			}
			return false;
		}

		Evaluates::Value evaluate() {
			return eval.getValue(pos);
		}

		Evaluates::Value negaEvaluate() {
			if (pos.isBlackTurn()) {
				return eval.getValue(pos);
			} else {
				return -eval.getValue(pos);
			}
		}

		Evaluates::Estimate estimate() const {
			const Shogi::Move* pmove = getCurrentMove();
			if (pmove != NULL) {
				return eval.estimate(pos, *pmove);
			}
			return Evaluates::Estimate();
		}

		Evaluates::Estimate negaEstimate() const {
			return pos.isBlackTurn() ? estimate() : -estimate();
		}

		void setHashMove(const HashMove& hashMove) {
			return nodes[depth].setHashMove(hashMove);
		}

		bool isHashMove() const {
			return nodes[depth].isHashMove();
		}

		void addKiller(Evaluates::Value value) const {
			const Shogi::Move* pmove = getCurrentMove();
			if (pmove != NULL) {
				value -= Killer::calcCurrentChange(pos, *pmove, eval.getParam());
				nodes[depth].addKiller(*pmove, value);
			}
		}

		void setMoveIndex(int index) {
			nodes[depth].setMoveIndex(index);
		}

		int getMoveIndex() const {
			return nodes[depth].getMoveIndex();
		}

		int getNumberOfMoves() const {
			return nodes[depth].getNumberOfMoves();
		}

		void getHistory(History& history, int depth) const {
			nodes[this->depth].getHistory(history, depth);
		}

		void sort(Evaluates::Value values[]) {
			nodes[0].sort(values);
		}

		std::string toString() const {
			return pos.toString();
		}

		bool is(const char* route) const {
			std::vector<std::string> tokens;
			boost::algorithm::split(tokens, route, boost::is_any_of(" "));
			if (tokens.size() != (unsigned)depth) {
				return false;
			}
			for (int i = 0; i < depth; i++) {
				if (nodes[i].isNullMove()) {
					if (tokens[i] != "null") {
						return false;
					}
				} else {
					const Shogi::Move* pmove = nodes[i].getMove();
					if (pmove == NULL || tokens[i] != pmove->toStringCsa()){
						return false;
					}
				}
			}
			return true;
		}
	};
}

#endif // TREE_H_
