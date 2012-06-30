/*
 * node.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef NODE_H_
#define NODE_H_

#include "moveGenerator.h"

namespace Shogi {
	class Node {
	private:
		MoveGenerator<Move>* pgen;
		const Move* pmove;
		Change change;

	public:
		Node() {
			pgen = NULL;
		}

		Node(Position& pos) {
			init(pos);
		}

		virtual ~Node() {
			if (pgen != NULL) {
				delete pgen;
			}
		}

		void init(Position& pos) {
			pgen = new MoveGenerator<Move>(pos);
		}

		int generateMoves() {
			pgen->clear();
			return pgen->generate();
		}

		bool next() {
			return (pmove = pgen->next()) != NULL;
		}

		void makeMove(Position& pos) {
			pos.moveUnsafe(*pmove, change);
		}

		void unmakeMove(Position& pos) {
			pos.back(change);
		}

		const Move* getMove() const {
			return pmove;
		}

		const Change& getChange() const {
			return change;
		}
	};
}

#endif // NODE_H_
