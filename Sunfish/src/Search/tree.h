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
		static const int MAX_DEPTH = 64;
		Shogi::Position pos;
		Evaluate::Evaluate eval;
		Node* nodes;
		int depth;
		int maxDepth;

	public:
		Tree(const Evaluate::Param& param) :
				eval(param), nodes(NULL) {
		}

		Tree(const Evaluate::Param& param,
				const Shogi::Position& pos,
				int maxDepth = MAX_DEPTH) :
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

		void init(int maxDepth = MAX_DEPTH) {
			if (nodes != NULL) {
				delete[] nodes;
			}
			this->maxDepth = maxDepth;
			nodes = new Node[maxDepth+1];
			for (int i = 0; i < maxDepth; i++) {
				nodes[i].init(pos);
			}
			depth = 0;
		}

		void init(const Shogi::Position& pos, int maxDepth = MAX_DEPTH) {
			this->pos.copy(pos);
			eval.init(pos);
			init(maxDepth);
		}

		int generateMoves() {
			return nodes[depth].generateMoves();
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

		void unmakeMove() {
			nodes[--depth].unmakeMove(pos, eval);
		}

		const Shogi::Move* getPrevMove() const {
			return depth > 0 ? nodes[depth-1].getMove() : NULL;
		}

		const Shogi::Change* getChange() const {
			return depth > 0 ? &nodes[depth-1].getChange() : NULL;
		}

		const Shogi::Position& getPosition() const {
			return pos;
		}

		Evaluate::Value evaluate() const {
			return eval.getValue();
		}

		std::string toString() const {
			return pos.toString();
		}
	};
}

#endif // TREE_H_
