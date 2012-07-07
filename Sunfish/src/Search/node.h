/*
 * node.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef NODE_H_
#define NODE_H_

#include "pv.h"
#include "phasedMoveGenerator.h"

namespace Search {
	class Node {
	private:
		PhasedMoveGenerator* pgen;
		const Shogi::Move* pmove;
		Shogi::Change change;
		Evaluate::Value baseValue;
		Pv pv;

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
			pgen = new PhasedMoveGenerator(pos);
			pmove = NULL;
		}

		void initPv() {
			pv.init();
		}

		int setPv(const Node& node) {
			return pv.set(*pmove, node.pv);
		}

		const Pv& getPv() const {
			return pv;
		}

		void generateMoves() {
			pgen->init();
			pmove = NULL;
		}

		void setHashMove(const HashMove& hashMove) {
			pgen->setHashMove(hashMove);
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
