/*
 * node.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef NODE_H_
#define NODE_H_

#include "../Shogi/moveGenerator.h"

namespace Search {
	class Node {
	private:
		Shogi::MoveGenerator<Shogi::Move>* pgen;
		const Shogi::Move* pmove;
		Shogi::Change change;
		Evaluate::Value baseValue;

	public:
		Node() {
			pgen = NULL;
			pmove = NULL;
		}

		Node(Shogi::Position& pos) {
			pmove = NULL;
			init(pos);
		}

		virtual ~Node() {
			if (pgen != NULL) {
				delete pgen;
			}
		}

		void init(Shogi::Position& pos) {
			pgen = new Shogi::MoveGenerator<Shogi::Move>(pos);
			pmove = NULL;
		}

		int generateMoves() {
			pgen->clear();
			pmove = NULL;
			return pgen->generate();
		}

		bool next() {
			return (pmove = pgen->next()) != NULL;
		}

		void makeMove(Shogi::Position& pos,
				Evaluate::Evaluate& eval) {
			baseValue = eval.getBaseValue();
			pos.moveUnsafe(*pmove, change, eval);
		}

		void unmakeMove(Shogi::Position& pos,
				Evaluate::Evaluate& eval) {
			eval.setBaseValue(baseValue);
			pos.back(change);
		}

		const Shogi::Move* getMove() const {
			return pmove;
		}

		const Shogi::Change& getChange() const {
			return change;
		}
	};
}

#endif // NODE_H_
