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
#include "history.h"

namespace Search {
	class Node {
	private:
		PhasedMoveGenerator* pgen;
		const Shogi::Move* pmove;
		Shogi::Change change;
		Evaluates::Value baseValue;
		Evaluates::Value posValue;
		bool isNullMoveMaked;
		Pv pv;
		bool shek;

	public:
		Node() : pgen(NULL), pmove(NULL), isNullMoveMaked(false) {
		}

		Node(const Shogi::Position& pos, const Evaluates::Param& param,
				const History& history) : pgen(NULL), pmove(NULL),
				isNullMoveMaked(false) {
			init(pos, param, history);
		}

		virtual ~Node() {
			if (pgen != NULL) {
				delete pgen;
			}
		}

		void init(const Shogi::Position& pos,
				const Evaluates::Param& param,
				const History& history) {
			if (pgen != NULL) { delete pgen; }
			pgen = new PhasedMoveGenerator(pos, param, history);
			pmove = NULL;
			isNullMoveMaked = false;
		}

		void arrive() {
			pv.init();
		}

		void initKiller() {
			pgen->getKiller().init();
		}

		void addKiller(const Shogi::Move& move, const Evaluates::Value& value) {
			pgen->getKiller().add(move, value);
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

		void generateTacticalMoves() {
			pgen->initTac();
			pmove = NULL;
		}

		void generateCaptures() {
			pgen->initCap();
			pmove = NULL;
		}

		void generateCheck() {
			pgen->initChk();
			pmove = NULL;
		}

		void setHashMove(const HashMove& hashMove) {
			pgen->setHashMove(hashMove);
		}

		bool isHashMove() const {
			return pgen->isHashMove();
		}

		void setMoveIndex(int index) {
			pgen->setCurrent(index);
		}

		int getMoveIndex() const {
			return pgen->getCurrent();
		}

		int getNumberOfMoves() const {
			return pgen->getNumber();
		}

		bool next() {
			return (pmove = pgen->next()) != NULL;
		}

		void makeMove(Shogi::Position& pos,
				Evaluates::Evaluate& eval) {
			baseValue = eval.getBaseValue();
			posValue = eval.getPositionalValue();
			pos.moveUnsafe(*pmove, change, eval);
		}

		bool nullMove(Shogi::Position& pos,
				Evaluates::Evaluate& eval) {
			isNullMoveMaked = true;
			baseValue = eval.getBaseValue();
			posValue = eval.getPositionalValue();
			return pos.nullMove(change);
		}

		void unmakeMove(Shogi::Position& pos,
				Evaluates::Evaluate& eval) {
			isNullMoveMaked = false;
			eval.setBaseValue(baseValue);
			eval.setPositionalValue(posValue);
			pos.back(change);
		}

		const Shogi::Move* getMove() const {
			return pmove;
		}

		void setMove(const Shogi::Move* pmove) {
			this->pmove = pmove;
		}

		bool isNullMove() const {
			return isNullMoveMaked;
		}

		const Shogi::Change& getChange() const {
			return change;
		}

		void addHistory(History& history, int depth) const {
			addHistory(history, depth, pgen->getCurrent());
		}

		void addHistory(History& history, int depth, int index) const {
			assert(pmove != NULL);
			for (int i = 0; i < index; i++) {
				history.addAppear(pgen->get(i), depth);
			}
			history.addGood(pgen->get(index-1), depth);
		}

		void sort(Evaluates::Value values[]) {
			pgen->sort(values);
		}

		void setShek(bool shek) {
			this->shek = shek;
		}

		bool isShek() {
			return shek;
		}
	};
}

#endif // NODE_H_
