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

	public:
		Node() {
			pgen = NULL;
		}

		Node(Shogi::Position& pos) {
			init(pos);
		}

		virtual ~Node() {
			if (pgen != NULL) {
				delete pgen;
			}
		}

		void init(Shogi::Position& pos) {
			pgen = new Shogi::MoveGenerator<Shogi::Move>(pos);
		}

		int generateMoves() {
			pgen->clear();
			return pgen->generate();
		}

		bool next() {
			return (pmove = pgen->next()) != NULL;
		}

		void makeMove(Shogi::Position& pos) {
			pos.moveUnsafe(*pmove, change);
		}

		void unmakeMove(Shogi::Position& pos) {
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
