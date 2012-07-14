/*
 * tree.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef TREE_H_
#define TREE_H_

#include "../Evaluate/evaluate.h"
#include "node.h"

namespace Search {
	class Tree {
	private:
		Shogi::Position pos;
		Evaluate::Evaluate eval;
		Node* nodes;
		int depth;
		int maxDepth;

	public:
		static const int DEF_MAX_DEPTH = 64;

		Tree(const Evaluate::Param& param,
				int maxDepth = DEF_MAX_DEPTH) :
				eval(param), nodes(NULL) {
			init(maxDepth);
		}

		Tree(const Evaluate::Param& param,
				const Shogi::Position& pos,
				int maxDepth = DEF_MAX_DEPTH) :
				pos(pos), eval(param), nodes(NULL) {
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
				nodes[i].init(pos, eval.getParam());
			}
			depth = 0;
		}

		void init(const Shogi::Position& pos, int maxDepth = DEF_MAX_DEPTH) {
			this->pos.copy(pos);
			eval.init(pos);
			init(maxDepth);
		}

		void initNode() {
			nodes[depth].initPv();
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

		const Shogi::Move* getCurrentMove() {
			return nodes[depth].getMove();
		}

		const Shogi::Change* getChange() const {
			return depth > 0 ? &nodes[depth-1].getChange() : NULL;
		}

		const Shogi::Position& getPosition() const {
			return pos;
		}

		Evaluate::Value evaluate() {
			return eval.getValue(pos);
		}

		Evaluate::Value negaEvaluate() {
			if (pos.isBlackTurn()) {
				return eval.getValue(pos);
			} else {
				return -eval.getValue(pos);
			}
		}

		void setHashMove(const HashMove& hashMove) {
			return nodes[depth].setHashMove(hashMove);
		}

		std::string toString() const {
			return pos.toString();
		}
	};
}

#endif // TREE_H_
